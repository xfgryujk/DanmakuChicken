#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <thread>


class Danmaku
{
	friend class DanmakuManager;

public:
	CString m_content;
	Point m_pos;
	Size m_size;
	CImage m_dc;

protected:
	Danmaku(const CString& content, FontFamily* font, REAL size);
public:
	Danmaku(const Danmaku& other) = delete;
};

class DanmakuManager
{
public:
	// 当前要渲染的所有弹幕
	std::vector<std::unique_ptr<Danmaku> > m_danmakuSet;
	std::mutex m_danmakuSetLock;
	// 包含所有弹幕的矩形大小
	Size m_danmakuBoxSize;

	// 弹幕字体
	FontFamily* m_danmakuFont;
	// 弹幕字体大小
	REAL m_danmakuSize;
	// 弹幕滚动速度
	int m_danmakuSpeed;

	// 获取弹幕线程
	std::thread* m_getNewThread = NULL;
	// 更新弹幕线程
	std::thread* m_updateThread = NULL;
	// 停止线程标志
	BOOL m_stopThreads = FALSE;

	// 最后弹幕ID
	int m_lastDanmakuID = 0;

	DanmakuManager();
	DanmakuManager(const DanmakuManager& other) = delete;
	~DanmakuManager();

	// 获取新弹幕，添加到m_danmakuSet
	virtual void GetNewDanmaku();
	// 添加新弹幕
	virtual void AddDanmaku(const CString& content);
	// 更新弹幕位置
	virtual void UpdateDanmaku();
	// 渲染m_danmakuSet到hdc
	virtual void RenderDanmakuSet(HDC hdc);
};
