#pragma once
#include "afxcmn.h"
#include "ColorListCtrl.h"

// CCopyAndPasteSignalDlg ��ȭ �����Դϴ�.

class CCopyAndPasteSignalDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CCopyAndPasteSignalDlg)

public:
	CCopyAndPasteSignalDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCopyAndPasteSignalDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_COPYPASTE_SIGNAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
