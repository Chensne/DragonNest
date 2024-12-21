#pragma once
#include "afxwin.h"


// CDnDynamicCtrl ��ȭ �����Դϴ�.

class CDnDynamicCtrl : public CDialog
{
	DECLARE_DYNAMIC(CDnDynamicCtrl)

public:
	CDnDynamicCtrl(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CDnDynamicCtrl(int nLastSelCtrl, CWnd* pParent = NULL);
	CDnDynamicCtrl(CString szEnum, CString szName, CWnd* pParent = NULL);

	virtual ~CDnDynamicCtrl();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_DYNAMIC_CREATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
