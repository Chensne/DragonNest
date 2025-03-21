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
	enum { IDD = IDD_SHADERSETDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CString m_szShaderFolder;
	afx_msg void OnBnClickedButton1();
};
