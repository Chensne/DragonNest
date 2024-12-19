//////////////////////////////////////////////////////////////////////////
// DnOptionDlg.h : header file
//////////////////////////////////////////////////////////////////////////

#pragma once
#ifdef _SKY
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
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnClose();
	// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
#endif