//////////////////////////////////////////////////////////////////////////
// DnOptionDlg.h : header file
//////////////////////////////////////////////////////////////////////////

#pragma once

// CDnOptionDlg dialog
class CDnAccountManagerDlg : public CDialog
{
	// Construction
public:
	CDnAccountManagerDlg( CWnd* pParent = NULL );	// standard constructor

	// Dialog Data
	enum { IDD = IDD_LOGIN };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
