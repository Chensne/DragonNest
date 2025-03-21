#pragma once


// CAddressDlg dialog

class CAddressDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddressDlg)

public:
	CAddressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddressDlg();

// Dialog Data
	enum { IDD = IDD_ADDRESS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CString m_szAddress;
	int m_nPort;
	CString m_szID;
	CString m_szPassword;
};
