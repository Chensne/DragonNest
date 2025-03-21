#pragma once
#include "afxcmn.h"
#include "ColorListCtrl.h"


// CAddSignalDlg 대화 상자입니다.

class CAddSignalDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CAddSignalDlg)

public:
	CAddSignalDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddSignalDlg();

	int m_nSelectSignalIndex;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADDSIGNALDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CColorListCtrl m_ListCtrl;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
};
