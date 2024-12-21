#pragma once
#include "afxwin.h"


struct SHotkey
{
	char szName[ 32 ];
	int nCode;
};
// CHotkeyDlg dialog

class CHotkeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CHotkeyDlg)

public:
	CHotkeyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHotkeyDlg();

// Dialog Data
	enum { IDD = IDD_HOTKEY };

public:
	int m_nHotkey;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeHotkeyCombo();
	CComboBox m_HotkeyList;
	afx_msg void OnBnClickedOk();
};

char *GetHotkeyString( int nHotkey );
int GetHotkeyIndex( int nHotkey );