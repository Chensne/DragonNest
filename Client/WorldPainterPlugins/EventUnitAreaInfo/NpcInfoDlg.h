#pragma once

#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CNpcInfoDlg dialog

class CEtWorldEventArea;
class CNpcInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CNpcInfoDlg)

public:
	CNpcInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNpcInfoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG3 };

public:
	void SetEventArea( CEtWorldEventArea *pArea ) { m_pEventArea = pArea; }

protected:
	CEtWorldEventArea *m_pEventArea;
	CString m_szName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
