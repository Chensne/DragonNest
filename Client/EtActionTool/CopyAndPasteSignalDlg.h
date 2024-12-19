#pragma once
#include "afxcmn.h"
#include "ColorListCtrl.h"

// CCopyAndPasteSignalDlg 대화 상자입니다.

class CCopyAndPasteSignalDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CCopyAndPasteSignalDlg)

public:
	CCopyAndPasteSignalDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCopyAndPasteSignalDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COPYPASTE_SIGNAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CColorListCtrl m_ListCtrl;

	virtual BOOL OnInitDialog();
	
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangingList1(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	void ResetList( bool bOKButton );
};
