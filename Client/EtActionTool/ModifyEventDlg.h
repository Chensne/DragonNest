#pragma once


// CModifyEventDlg 대화 상자입니다.

#include "WizardCommon.h"
#include "afxwin.h"
class CModifyEventDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyEventDlg)

public:
	CModifyEventDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModifyEventDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CREATEEVENT_DLG };

protected:
	CWizardCommon::VIEW_TYPE m_CurrentViewType;
	CWnd *m_pCurrentView;

	std::vector<CWnd *> m_pVecViewList;

public:
	void SetCurrentView( CWizardCommon::VIEW_TYPE Type, bool bPreview = false );

	CButton m_BackButton;
	CButton m_NextButton;
	CButton m_FinishButton;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedBack();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
