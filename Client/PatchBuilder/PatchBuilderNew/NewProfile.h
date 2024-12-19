#pragma once


// CNewProfile dialog

class CNewProfile : public CDialog
{
	DECLARE_DYNAMIC(CNewProfile)

public:
	CNewProfile(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewProfile();

// Dialog Data
	enum { IDD = IDD_NEW_PROFILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_szProfileName;
};
