#pragma once


// CTemplateNameDlg dialog

class CTemplateNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CTemplateNameDlg)

public:
	CTemplateNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTemplateNameDlg();

// Dialog Data
	enum { IDD = IDD_TEMPLATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_szTemplateName;
};
