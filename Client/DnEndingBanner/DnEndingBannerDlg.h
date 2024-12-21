// DnEndingBannerDlg.h : 헤더 파일
//
#pragma once

#include "DnEndingBannerDefines.h"
#include "DnHtmlView.h"

// CDnEndingBannerDlg 대화 상자
class CDnEndingBannerDlg : public CDialog
{
// 생성입니다.
public:
	CDnEndingBannerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DNENDINGBANNER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	enum 
	{ 
		HTMLVIEW_MAIN,
		HTMLVIEW_SUB,
		HTMLVIEW_MAX
	};

	BOOL	InitSkin();
	BOOL	InitWebPage();

	HICON			m_hIcon;
	DWORD			m_dwWidth;
	DWORD			m_dwHeight;
	HDC				m_dcBkGrnd;
	CDnHtmlView*	m_pDnHtmlView[HTMLVIEW_MAX];
	SCtrlRect		m_CtrlPosData;
	CBitmapButton	m_btnClose;

	// 생성된 메시지 맵 함수
	virtual BOOL		OnInitDialog();
	afx_msg void		OnPaint();
	afx_msg HCURSOR		OnQueryDragIcon();
	afx_msg BOOL		OnEraseBkgnd( CDC* pDC );
	afx_msg void		OnDestroy();
	afx_msg void		OnClose();
	afx_msg void		OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
