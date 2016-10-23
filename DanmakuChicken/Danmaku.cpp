#include "stdafx.h"
#include "Danmaku.h"
#include "msxml2.h"
#include "rapidjson/document.h"
using namespace rapidjson;


Danmaku::Danmaku(const CString& content, FontFamily* font, REAL size)
{
	m_content = content;

	GraphicsPath path;
	path.AddString(m_content, -1, font, FontStyle::FontStyleBold, size, Point(0, 0), Gdiplus::StringFormat::GenericDefault());

	Rect rect;
	path.GetBounds(&rect);
	m_size.Width = int(rect.Width * 1.1);
	m_size.Height = int(rect.Height * 1.1); // 对于纯英文还是大小不够，不管了...

	m_dc.Create(m_size.Width, m_size.Height, 32, CImage::createAlphaChannel);
	Gdiplus::Graphics graph(m_dc.GetDC());
	graph.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	Gdiplus::SolidBrush whiteBrush(Color::White);
	graph.FillPath(&whiteBrush, &path);
	Gdiplus::Pen blackPen(Gdiplus::Color::Black, 1.5F);
	graph.DrawPath(&blackPen, &path);
	m_dc.ReleaseDC();
}

DanmakuManager::DanmakuManager()
{
	m_danmakuBoxSize.Width = 800;
	m_danmakuBoxSize.Height = 600;
	m_danmakuFont = new FontFamily(L"黑体");
	m_danmakuSize = 40.0F;
	m_danmakuSpeed = 6;

	// 获取弹幕线程
	m_getNewThread = new std::thread([this]{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);

		{ // 取最后弹幕ID
			CComPtr<IServerXMLHTTPRequest> m_xml;
			m_xml.CoCreateInstance(__uuidof(ServerXMLHTTP));
			m_xml->open(_bstr_t("GET"), _bstr_t("http://xfgryujk.tk/wx/danmaku.php?action=getlastid"), _variant_t(false), _variant_t(), _variant_t());
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
	m_updateThread = new std::thread([this]{
		while (!m_stopThreads)
		{
			UpdateDanmaku();
			Sleep(33);
		}
	});

}

DanmakuManager::~DanmakuManager()
{
	m_stopThreads = TRUE;
	if (m_getNewThread != NULL)
	{
		m_getNewThread->join();
		delete m_getNewThread;
	}
	if (m_updateThread != NULL)
	{
		m_updateThread->join();
		delete m_updateThread;
	}

	if (m_danmakuFont != NULL)
		delete m_danmakuFont;
}

// 获取新弹幕，添加到m_danmakuSet
void DanmakuManager::GetNewDanmaku()
{
	CComPtr<IServerXMLHTTPRequest> m_xml;
	m_xml.CoCreateInstance(__uuidof(ServerXMLHTTP));
	CString url;
	url.Format(_T("http://xfgryujk.tk/wx/danmaku.php?action=getdanmaku&start=%d"), m_lastDanmakuID + 1);
	m_xml->open(_bstr_t("GET"), _bstr_t(url), _variant_t(false), _variant_t(), _variant_t());
	m_xml->send(_variant_t());
	_bstr_t text;
	if (FAILED(m_xml->get_responseText(text.GetAddress())))
		return;
	Document document;
	document.Parse(text);
	if (document.HasParseError() || !document.IsArray())
		return;

	for (auto it = document.Begin(); it != document.End(); ++it)
	{
		int id = (*it)["id"].GetInt();
		if (id > m_lastDanmakuID)
			m_lastDanmakuID = id;

		// 转码
		int dstLen = MultiByteToWideChar(CP_UTF8, 0, (*it)["content"].GetString(), (*it)["content"].GetStringLength(), NULL, 0);
		if (dstLen == 0)
			continue;
		CStringW content;
		MultiByteToWideChar(CP_UTF8, 0, (*it)["content"].GetString(), (*it)["content"].GetStringLength(), content.GetBuffer(dstLen), dstLen);
		content.ReleaseBuffer(dstLen);
		AddDanmaku(content);
	}
}

// 添加新弹幕
void DanmakuManager::AddDanmaku(const CString& content)
{
	std::unique_ptr<Danmaku> danmaku(new Danmaku(content, m_danmakuFont, m_danmakuSize));
	danmaku->m_pos.X = m_danmakuBoxSize.Width;
	danmaku->m_pos.Y = 0;

	// 寻找不被遮挡的Y坐标
	BOOL hasCollision;
	do {
		hasCollision = FALSE;
		std::lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
		for (const auto& i : m_danmakuSet)
		{
			// 有碰撞
			if (i->m_pos.X + i->m_size.Width > danmaku->m_pos.X && i->m_pos.X < danmaku->m_pos.X + danmaku->m_size.Width
				&& i->m_pos.Y + i->m_size.Height > danmaku->m_pos.Y && i->m_pos.Y < danmaku->m_pos.Y + danmaku->m_size.Height)
			{
				danmaku->m_pos.Y = i->m_pos.Y + i->m_size.Height;
				hasCollision = TRUE;
				break;
			}
		}
	} while (hasCollision && danmaku->m_pos.Y + danmaku->m_size.Height <= m_danmakuBoxSize.Height);

	std::lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	m_danmakuSet.push_back(std::move(danmaku));
}

// 更新弹幕位置
void DanmakuManager::UpdateDanmaku()
{
	std::lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	for (auto it = m_danmakuSet.begin(); it != m_danmakuSet.end(); )
	{
		(*it)->m_pos.X -= m_danmakuSpeed;
		if ((*it)->m_pos.X + (*it)->m_size.Width <= 0) // 删除边界外的弹幕
			it = m_danmakuSet.erase(it);
		else
			++it;
	}
}

// 渲染m_danmakuSet到hdc
void DanmakuManager::RenderDanmakuSet(HDC hdc)
{
	std::lock_guard<decltype(m_danmakuSetLock)> lock(m_danmakuSetLock);
	for (const auto& i : m_danmakuSet)
		i->m_dc.AlphaBlend(hdc, i->m_pos.X, i->m_pos.Y, 255, AC_SRC_OVER);
}
