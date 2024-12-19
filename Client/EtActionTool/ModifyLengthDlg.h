#pragma once


// CModifyLengthDlg 대화 상자입니다.

class CModifyLengthDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyLengthDlg)

public:
	CModifyLengthDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModifyLengthDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_REQUEUESIGNAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOkall();
	CString m_szActionName;
	static int s_nType;
	static bool s_bYesAll;

	void SetActionName( CString szName ) { m_szActionName = szName; }
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
};
