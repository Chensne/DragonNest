#pragma once
#include "afxwin.h"


// CMonsterCounterDlg dialog

class CEtWorldSector;
class CEtWorldEventArea;
class CMonsterCounterDlg : public CXTPDialog
{
	DECLARE_DYNAMIC(CMonsterCounterDlg)

public:
	CMonsterCounterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMonsterCounterDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

public:
	void Refresh( CEtWorldSector *pSector, bool bForce = false );
	bool CheckRefresh( CEtWorldSector *pSector );

	struct MonsterCountStruct {
		int nMonsterID;
		int nCount;
		int nLevel;
		int nRace;
		int nGrade;
	};
	std::vector<MonsterCountStruct> m_VecMonsterList;
	int m_nEventAreaCommonParamCount;

	void InsertMonsterList( int nMonsterID, int nCount, int nGenCount );
	void CalcMonsterIDMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax );
	void CalcMonsterGroupMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax );
	void CalcMonsterSetMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedExport();


	CComboBox m_Combo;
	CXTListCtrl m_ListCtrl;
	virtual BOOL OnInitDialog();
	int m_nLastEventAreaCount;
	CEtWorldSector *m_pSector;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnDestroy();

	void ExportMonsterInfo( int nDifficulty, FILE *fp );
};
