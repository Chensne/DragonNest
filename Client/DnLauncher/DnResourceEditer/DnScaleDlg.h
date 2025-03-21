#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDnScaleDlg 대화 상자입니다.

class CDnScaleDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnScaleDlg)

public:
	CDnScaleDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDnScaleDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SCALE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
