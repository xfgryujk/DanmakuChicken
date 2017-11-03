
// DanmakuChickenDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DanmakuChicken.h"
#include "DanmakuChickenDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDanmakuChickenDlg 对话框



CDanmakuChickenDlg::CDanmakuChickenDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDanmakuChickenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDanmakuChickenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_danmakuSizeSlider);
	DDX_Control(pDX, IDC_SLIDER2, m_danmakuSpeedSlider);
	DDX_Control(pDX, IDC_SLIDER3, m_danmakuOpacitySlider);
}

BEGIN_MESSAGE_MAP(CDanmakuChickenDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BUTTON2, &CDanmakuChickenDlg::OnBnClickedButton2)
ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CDanmakuChickenDlg 消息处理程序

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDanmakuChickenDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDanmakuChickenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDanmakuChickenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_danmakuSizeSlider.SetRange(20, 100);
	m_danmakuSizeSlider.SetPos((int)m_overlayDlg.m_danmakuManager.m_danmakuSize);
	m_danmakuSpeedSlider.SetRange(1, 100);
	m_danmakuSpeedSlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuSpeed);
	m_danmakuOpacitySlider.SetRange(255 * 10 / 100, 255);
	m_danmakuOpacitySlider.SetPos(m_overlayDlg.m_danmakuManager.m_danmakuAlpha);

	// 载入弹幕窗口
	m_overlayDlg.Create(m_overlayDlg.IDD, GetDesktopWindow());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDanmakuChickenDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// 关闭弹幕窗口
	m_overlayDlg.DestroyWindow();
}

// 修改弹幕设置
void CDanmakuChickenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_THUMBTRACK)
	{
		switch (pScrollBar->GetDlgCtrlID())
		{
		case IDC_SLIDER1:
			m_overlayDlg.m_danmakuManager.m_danmakuSize = (float)nPos;
			break;
		case IDC_SLIDER2:
			m_overlayDlg.m_danmakuManager.m_danmakuSpeed = nPos;
			break;
		case IDC_SLIDER3:
			m_overlayDlg.m_danmakuManager.m_danmakuAlpha = nPos;
			break;
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

// 测试弹幕
void CDanmakuChickenDlg::OnBnClickedButton2()
{
	m_overlayDlg.m_danmakuManager.AddDanmaku(_T("测试TEST1234asdfasdfasdf！！！"));
}
