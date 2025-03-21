#pragma once
#include "afxwin.h"


// CScrollBarSelect dialog

class CScrollBarSelect : public CDialog
{
	DECLARE_DYNAMIC(CScrollBarSelect)

public:
	CScrollBarSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScrollBarSelect();

// Dialog Data
	enum { IDD = IDD_SCROLLBAR_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nSelectIndex;
	CComboBox m_ScrollBarName;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
