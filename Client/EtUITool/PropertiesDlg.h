#pragma once


// CPropertiesDlg dialog

class CPropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CPropertiesDlg)

public:
	CPropertiesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertiesDlg();

	bool m_bDisableCancel;
// Dialog Data
	enum { IDD = IDD_PROPERTIESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CString m_szShaderFolder;
	CString m_szResourceFolder;
	CString m_szUITemplateFolder;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
