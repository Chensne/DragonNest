#pragma once


// CPropertiesDlg 대화 상자입니다.

class CPropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CPropertiesDlg)

public:
	CPropertiesDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPropertiesDlg();

	bool m_bDisableCancel;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_PROPERTIESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_szWorkingFolder;
	CString m_szShaderFolder;
	CString m_szResourceFolder;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
