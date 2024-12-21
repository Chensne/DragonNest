// DnEndingBannerDlg.h : ��� ����
//
#pragma once

#include "DnEndingBannerDefines.h"
#include "DnHtmlView.h"

// CDnEndingBannerDlg ��ȭ ����
class CDnEndingBannerDlg : public CDialog
{
// �����Դϴ�.
public:
	CDnEndingBannerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DNENDINGBANNER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
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

	// ������ �޽��� �� �Լ�
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
