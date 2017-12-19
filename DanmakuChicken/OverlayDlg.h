#pragma once
#include "Danmaku.h"


// COverlayDlg 对话框

class COverlayDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDlg)

public:
	COverlayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COverlayDlg();

// 对话框数据
	enum { IDD = IDD_OVERLAY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void UpdateUI();


	DanmakuManager m_danmakuManager;
	SIZE m_size;
	CImage m_bufferImg;
	std::thread m_renderThread;
	BOOL m_stopThreads = FALSE;
};
