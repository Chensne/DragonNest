#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class CStringSelect : public CDialog
{
	DECLARE_DYNAMIC(CStringSelect)

public:
	CStringSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStringSelect();

protected:
	int m_nSelectItem;
	int m_nStringIndex;
	CListCtrl m_ctrlListString;
	CEdit m_ctrlEditFind;

public:
	int GetStringIndex() { return m_nStringIndex; }

// Dialog Data
	enum { IDD = IDD_STRING_SELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetScroll();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
