// DuplicateFinderDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CDuplicateFinderDlg dialog
class CDuplicateFinderDlg : public CXTResizeDialog
{
// Construction
public:
	CDuplicateFinderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DUPLICATEFINDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	CXTListCtrl m_ListCtrl;
	CString m_szFolder;
	afx_msg void OnDestroy();
};
