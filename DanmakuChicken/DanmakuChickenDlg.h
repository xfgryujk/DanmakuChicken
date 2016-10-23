
// DanmakuChickenDlg.h : 头文件
//

#pragma once
#include "OverlayDlg.h"
#include "afxwin.h"


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
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();


	ULONG_PTR m_gdiplusToken;
	COverlayDlg* m_overlayDlg = NULL;

	CEdit m_danmakuSizeEdit;
	CEdit m_danmakuSpeedEdit;
};
