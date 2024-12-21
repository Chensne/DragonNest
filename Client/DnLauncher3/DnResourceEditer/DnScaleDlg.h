#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDnScaleDlg ��ȭ �����Դϴ�.

class CDnScaleDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnScaleDlg)

public:
	CDnScaleDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDnScaleDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SCALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_slideWidth;
	CSliderCtrl m_slideHeight;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CEdit m_editScaleX;
	CEdit m_editScaleY;
	afx_msg void OnEnChangeEditScalex();
	afx_msg void OnEnChangeEditScaley();
};
