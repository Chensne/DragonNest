#pragma once
#include "spreadsheet1.h"


// CSignalReportDlg dialog

class CSignalReportDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CSignalReportDlg)

public:
	CSignalReportDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSignalReportDlg();

// Dialog Data
	enum { IDD = IDD_SIGNALREPORTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	CSpreadsheet1 m_Sheet;
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnBnClickedSave();
};
