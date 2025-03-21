#pragma once
#include "afxwin.h"


// CAddActionDlg 대화 상자입니다.

class CAddActionDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddActionDlg)

public:
	CAddActionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddActionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADDACTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit2();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	CComboBox m_Combo1;
	CComboBox m_Combo2;
	CString m_szCustomEdit;
	CString m_szResultString;
	int m_nType;

	void CalcResultString();
	CString GetResultString() { return m_szResultString; }
	afx_msg void OnBnClickedOk();
};
