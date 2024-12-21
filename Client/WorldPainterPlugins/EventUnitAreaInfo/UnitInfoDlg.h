#pragma once


// CUnitInfoDlg dialog
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

class CEtWorldEventArea;
class AFX_EXT_CLASS CUnitInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CUnitInfoDlg)

public:
	CUnitInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUnitInfoDlg();

// Dialog Data
	enum { IDD = IDD_UNITAREAPROP };

public:
	void SetEventArea( CEtWorldEventArea *pArea ) { m_pEventArea = pArea; }

protected:
	CEtWorldEventArea *m_pEventArea;
	int m_nCompoundIndex;
	int m_nEventAreaCommonParamCount;

protected:
	void RefreshMonsterInfo( int nCompoundIndex, int nDifficulty );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CXTListCtrl m_ListCtrl;
	CComboBox m_ComboBox;
	int m_nMonsterGroupID;
	int m_nValueMin;
	int m_nValueMax;
	int m_nCountMin;
	int m_nCountMax;
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelchangeCombo1();
};
