#pragma once
#include "afxwin.h"


// CDialogList dialog

class CDialogList : public CDialog
{
	DECLARE_DYNAMIC(CDialogList)

public:
	CDialogList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogList();

// Dialog Data
	enum { IDD = IDD_DIALOG_LIST };

	void AddDialog( const char *szDialogName );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListBox;
};
