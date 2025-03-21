#pragma once
#include "afxcmn.h"


// CBonePropDlg dialog

class CEtBone;
class CBonePropDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CBonePropDlg)

public:
	CBonePropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBonePropDlg();

// Dialog Data
	enum { IDD = IDD_BONEPROPDLG };

protected:
	void InsertTree( HTREEITEM hParent, CEtBone *pBone );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CXTTreeCtrl m_Tree;
	afx_msg void OnDestroy();
};
