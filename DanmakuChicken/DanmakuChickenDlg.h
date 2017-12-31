
// DanmakuChickenDlg.h : 头文件
//

#pragma once
#include "OverlayDlg.h"
#undef min
#undef max
#include "..\Simple-Web-Server\server_http.hpp"
#include <thread>
#include "afxwin.h"
#include "afxcmn.h"

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;


// CDanmakuChickenDlg 对话框
class CDanmakuChickenDlg : public CDialogEx
{
// 构造
public:
	CDanmakuChickenDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DANMAKUCHICKEN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButton2();

	// 处理添加弹幕请求
	void HandleAddDanmaku(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);


	COverlayDlg m_overlayDlg;
	HttpServer m_server;
	std::thread m_serverThread;

	CSliderCtrl m_danmakuSizeSlider;
	CSliderCtrl m_danmakuSpeedSlider;
	CSliderCtrl m_danmakuOpacitySlider;
};
