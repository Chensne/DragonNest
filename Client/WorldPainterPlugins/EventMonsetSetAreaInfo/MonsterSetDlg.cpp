// MonsterSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MonsterSetDlg.h"
#include "TEtWorldEventArea.h"
#include "DNTableFile.h"
#include "EtUIXML.h"

// CMonsterSetDlg dialog

extern DNTableFileFormat *s_pSox[3];

IMPLEMENT_DYNAMIC(CMonsterSetDlg, CDialog)

CMonsterSetDlg::CMonsterSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMonsterSetDlg::IDD, pParent)
	,m_nSetID(0)
	,m_nPosID(0)
{
	m_pEventArea = NULL;
	m_nEventAreaCommonParamCount = 1;
}

CMonsterSetDlg::~CMonsterSetDlg()
{
}

void CMonsterSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_szName );
	DDX_Text(pDX, IDC_EDIT2, m_nSetID );
	DDX_Text(pDX, IDC_EDIT3, m_nPosID );
}


BEGIN_MESSAGE_MAP(CMonsterSetDlg, CDialog)
END_MESSAGE_MAP()


// CMonsterSetDlg message handlers

BOOL CMonsterSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pEventArea == NULL ) return FALSE;

	CTEtWorldEventArea *pArea = (CTEtWorldEventArea *)m_pEventArea;
	int nSetID = pArea->GetProperty(0+m_nEventAreaCommonParamCount)->GetVariableInt();
	int nPosID = pArea->GetProperty(1+m_nEventAreaCommonParamCount)->GetVariableInt();

	m_szName = pArea->GetName();
	m_nSetID = nSetID;
	m_nPosID = nPosID;

	UpdateData( FALSE );


	if( !s_pSox[1]->IsExistItem( nSetID ) ) return FALSE;

	std::vector<int> nVecSetList;
	s_pSox[1]->GetItemIDListFromField( "_SetNo", nSetID, nVecSetList );

	HTREEITEM hDifficulty[5];
	hDifficulty[0] = m_TreeCtrl.InsertItem( "Easy" );
	hDifficulty[1] = m_TreeCtrl.InsertItem( "Normal" );
	hDifficulty[2] = m_TreeCtrl.InsertItem( "Hard" );
	hDifficulty[3] = m_TreeCtrl.InsertItem( "VeryHard" );
	hDifficulty[4] = m_TreeCtrl.InsertItem( "Nightmare" );
	char szStr[256];
	char szLabel[64];
	for( DWORD i=0; i<nVecSetList.size(); i++ ) {
		int nDifficulty = s_pSox[1]->GetFieldFromLablePtr( nVecSetList[i], "_Difficulty" )->GetInteger();
		float fWeight = s_pSox[1]->GetFieldFromLablePtr( nVecSetList[i], "_SetWeight" )->GetFloat() * 100.f;

		sprintf_s( szStr, "Weight : %.2f %%", fWeight );
		HTREEITEM hItem = m_TreeCtrl.InsertItem( szStr, hDifficulty[nDifficulty] );
		for( int j=0; j<20; j++ ) {
			sprintf_s( szLabel, "_PosID%d", j + 1 );
			int nPosition = s_pSox[1]->GetFieldFromLablePtr( nVecSetList[i], szLabel )->GetInteger();
			sprintf_s( szLabel, "_GroupID%d", j + 1 );
			int nGroup = s_pSox[1]->GetFieldFromLablePtr( nVecSetList[i], szLabel )->GetInteger();
			sprintf_s( szLabel, "_Num%d", j + 1 );
			int nCount = s_pSox[1]->GetFieldFromLablePtr( nVecSetList[i], szLabel )->GetInteger();
			if( nPosition != nPosID || nPosition < 1 || nGroup < 1 || nCount < 1 ) continue;

			sprintf_s( szStr, "Group : %d  Num : %d", nGroup, nCount );
			HTREEITEM hSetItem = m_TreeCtrl.InsertItem( szStr, hItem );

			for( int k=0; k<20; k++ ) {
				sprintf_s( szLabel, "_MonsterID%d", k + 1 );
				int nMonsterID = s_pSox[2]->GetFieldFromLablePtr( nGroup, szLabel )->GetInteger();
				sprintf_s( szLabel, "_Prob%d", k + 1 );
				float fProb = s_pSox[2]->GetFieldFromLablePtr( nGroup, szLabel )->GetInteger() / 10000000.f;
				if( nMonsterID < 1 || fProb <= 0.f ) continue;
				if( s_pSox[0]->IsExistItem( nMonsterID ) == false ) continue;
				int nNameID = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_NameID" )->GetInteger();
				std::string szName;
				ToMultiString( std::wstring(CEtUIXML::GetInstance().GetUIString( CEtUIXML::idCategory1, nNameID )), szName );

				sprintf_s( szStr, "%s - %.8f %%", szName.c_str(), fProb );
				m_TreeCtrl.InsertItem( szStr, hSetItem );
			}
			m_TreeCtrl.Expand( hSetItem, TVE_EXPAND );
		}
		m_TreeCtrl.Expand( hItem, TVE_EXPAND );
	}
	for( int i=0; i<5; i++ ) {
		m_TreeCtrl.Expand( hDifficulty[i], TVE_EXPAND );
	}


	return TRUE;
}
