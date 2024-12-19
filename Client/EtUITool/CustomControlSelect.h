#pragma once
#include "afxwin.h"


// CCustomControlSelect dialog

class CCustomControlSelect : public CDialog
{
	DECLARE_DYNAMIC(CCustomControlSelect)

public:
	CCustomControlSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCustomControlSelect();

// Dialog Data
	enum { IDD = IDD_CUSTOM_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nCurSel;
	CComboBox m_CustomSelect;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
