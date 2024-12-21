#pragma once


// CMonsterInfoDlg dialog
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

class CEtWorldEventArea;
class CMonsterInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonsterInfoDlg)

public:
	CMonsterInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMonsterInfoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

public:
	void SetEventArea( CEtWorldEventArea *pArea ) { m_pEventArea = pArea; }

protected:
	CEtWorldEventArea *m_pEventArea;
	CString m_szName;
	int m_nLevel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
