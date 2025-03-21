#pragma once
#include "afxwin.h"


// CDnDynamicCtrl 대화 상자입니다.

class CDnDynamicCtrl : public CDialog
{
	DECLARE_DYNAMIC(CDnDynamicCtrl)

public:
	CDnDynamicCtrl(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDnDynamicCtrl(int nLastSelCtrl, CWnd* pParent = NULL);
	CDnDynamicCtrl(CString szEnum, CString szName, CWnd* pParent = NULL);

	virtual ~CDnDynamicCtrl();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DYNAMIC_CREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_szEnum;
	CString m_szName;
	CEdit	m_editEnum;
	CEdit	m_editCtrlName;
	int		m_nLastSelCtrl;
	afx_msg void OnEnChangeEditEnumValue();
	afx_msg void OnEnChangeEditCtrlName();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnSetfocusEditCtrlName();
};
