#pragma once
#include "afxwin.h"


// CModifyLinkAniDlg 대화 상자입니다.

class CModifyLinkAniDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyLinkAniDlg)

public:
	CModifyLinkAniDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModifyLinkAniDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHANGELINKDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_szActionName;
	CString m_szOriginalAniName;
	CComboBox m_Combo;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_szResultString;

	void SetActionName( CString szName ) { m_szActionName = szName; }
	void SetOriginalName( CString szName ) { m_szOriginalAniName = szName; }
};
