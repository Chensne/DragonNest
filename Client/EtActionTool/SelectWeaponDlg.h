#pragma once
#include "afxwin.h"


// SelectWeaponDlg ��ȭ �����Դϴ�.

class CSelectWeaponDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectWeaponDlg)

public:
	CSelectWeaponDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSelectWeaponDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SELECTWEAPON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_ctrlEdit1;
	CString m_ctrlEdit2;
};
