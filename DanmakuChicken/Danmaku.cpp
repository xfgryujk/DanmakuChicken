#include "stdafx.h"
#include "Danmaku.h"
#include "msxml2.h"
#include "rapidjson/document.h"
using namespace rapidjson;
using namespace std;
using namespace Gdiplus;


// 获取弹幕的地址
const CString DANMAKU_URL = _T("http://buaaacg.org/wx/danmaku.php");


Danmaku::Danmaku(const CString& content, const FontFamily* font, REAL size)
{
	m_content = content;

	GraphicsPath path;
	path.AddString(m_content, -1, font, FontStyle::FontStyleBold, size, Point(0, 0), StringFormat::GenericDefault());

	Pen blackPen(Color::Black, 1.5F);
	Rect rect;
	path.GetBounds(&rect, NULL, &blackPen);
	m_size.Width = rect.Width;
	m_size.Height = rect.Height;

	m_img.Create(m_size.Width, m_size.Height, 32, CImage::createAlphaChannel);
	Graphics graph(m_img.GetDC());
	graph.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	SolidBrush whiteBrush(Color::White);
	graph.FillPath(&whiteBrush, &path);
	graph.DrawPath(&blackPen, &path);
	m_img.ReleaseDC();
}

Danmaku::Danmaku(Danmaku&& other)
{
	*this = move(other);
}

Danmaku& Danmaku::operator= (Danmaku&& other)
{
	m_content = move(other.m_content);
	m_pos = move(other.m_pos);
	m_size = move(other.m_size);
	m_img.Destroy();
	m_img.Attach(other.m_img.Detach());
	return *this;
}


DanmakuManager::DanmakuManager() :
	m_danmakuFont(make_unique<FontFamily>(L"黑体"))
{
	// 获取弹幕线程
	m_getNewThread = thread([this] {
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		{ // 取最后弹幕ID
		CComPtr<IServerXMLHTTPRequest> m_xml;
		m_xml.CoCreateInstance(__uuidof(ServerXMLHTTP));
		m_xml->open(_bstr_t("GET"), _bstr_t(DANMAKU_URL + _T("?action=getlastid")), _variant_t(false), _variant_t(), _variant_t());
		m_xml->send(_variant_t());
		_bstr_t text;
		if (SUCCEEDED(m_xml->get_responseText(text.GetAddress())))
		{
			Document document;
			document.Parse(text);
			if (!document.HasParseError() && document.IsObject())
				m_lastDanmakuID = document["lastid"].GetInt();
		}
		}

		while (!m_stopThreads)
		{
			GetNewDanmaku();
			Sleep(2000);
		}

		CoUninitialize();
	});

	// 更新弹幕线程
	m_updateThread = thread([this] {
		while (!m_stopThreads)
		{
			UpdateDanmaku();
			Sleep(1000 / 30); // 30fps
		}
	});
}

DanmakuManager::~DanmakuManager()
{
	m_stopThreads = TRUE;
	if (m_getNewThread.joinable())
		m_getNewThread.join();
	if (m_updateThread.joinable())
		m_updateThread.join();
}

// 获取新弹幕，添加到m_danmakuSet
void DanmakuManager::GetNewDanmaku()
{
	CComPtr<IServerXMLHTTPRequest> m_xml;
	m_xml.CoCreateInstance(__uuidof(ServerXMLHTTP));
	CString url;
	url.Format(DANMAKU_URL + _T("?action=getdanmaku&start=%d"), m_lastDanmakuID + 1);
	m_xml->open(_bstr_t("GET"), _bstr_t(url), _variant_t(false), _variant_t(), _variant_t());
	m_xml->send(_variant_t());
	_bstr_t text;
	if (FAILED(m_xml->get_responseText(text.GetAddress())))
		return;
	GenericDocument<UTF16<> > document;
	document.Parse(text);
	if (document.HasParseError() || !document.IsArray())
		return;

	for (auto it = document.Begin(); it != document.End(); ++it)
	{
		int id = (*it)[L"id"].GetInt();
		if (id > m_lastDanmakuID)
			m_lastDanmakuID = id;

		AddDanmaku((*it)[L"content"].GetString());
	}
}

// 添加新弹幕
void DanmakuManager::AddDanmaku(const CString& content)
{
	Danmaku danmaku(content, m_danmakuFont.get(), m_danmakuSize);
	danmaku.m_pos.X = m_danmakuBoxSize.Width;
	danmaku.m_pos.Y = 0;

	// 寻找不被遮挡的Y坐标
	BOOL hasCollision;
	do
	{
		hasCollision = FALSE;
		lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
		for (const auto& i : m_danmakuSet)
		{
			// 有碰撞
			if (i.m_pos.X + i.m_size.Width > danmaku.m_pos.X && i.m_pos.X < danmaku.m_pos.X + danmaku.m_size.Width
				&& i.m_pos.Y + i.m_size.Height > danmaku.m_pos.Y && i.m_pos.Y < danmaku.m_pos.Y + danmaku.m_size.Height)
			{
				danmaku.m_pos.Y = i.m_pos.Y + i.m_size.Height;
				hasCollision = TRUE;
				break;
			}
		}
	} while (hasCollision && danmaku.m_pos.Y + danmaku.m_size.Height <= m_danmakuBoxSize.Height);

	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	m_danmakuSet.push_back(move(danmaku));
}

// 更新弹幕位置
void DanmakuManager::UpdateDanmaku()
{
	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	for (auto it = m_danmakuSet.begin(); it != m_danmakuSet.end(); )
	{
		it->m_pos.X -= m_danmakuSpeed;
		if (it->m_pos.X + it->m_size.Width <= 0) // 删除边界外的弹幕
			it = m_danmakuSet.erase(it);
		else
			++it;
	}
}

// 渲染m_danmakuSet到hdc
void DanmakuManager::RenderDanmakuSet(HDC hdc)
{
	lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	for (const auto& i : m_danmakuSet)
		i.m_img.AlphaBlend(hdc, i.m_pos.X, i.m_pos.Y, m_danmakuAlpha, AC_SRC_OVER);
}
