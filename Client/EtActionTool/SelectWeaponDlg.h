#pragma once
#include "afxwin.h"


// SelectWeaponDlg 대화 상자입니다.

class CSelectWeaponDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectWeaponDlg)

public:
	CSelectWeaponDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSelectWeaponDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECTWEAPON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_ctrlEdit1;
	CString m_ctrlEdit2;
};
