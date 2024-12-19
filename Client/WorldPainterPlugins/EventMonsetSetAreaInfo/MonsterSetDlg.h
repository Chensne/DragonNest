#pragma once
#include "afxcmn.h"


// CMonsterSetDlg dialog

class CEtWorldEventArea;
class CMonsterSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CMonsterSetDlg)

public:
	CMonsterSetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMonsterSetDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

public:
	void SetEventArea( CEtWorldEventArea *pArea ) { m_pEventArea = pArea; }

protected:
	CEtWorldEventArea *m_pEventArea;
	CString m_szName;
	int m_nSetID;
	int m_nPosID;
	int m_nEventAreaCommonParamCount;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CXTTreeCtrl m_TreeCtrl;
};
