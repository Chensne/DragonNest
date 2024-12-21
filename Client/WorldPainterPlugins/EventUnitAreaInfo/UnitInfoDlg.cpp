// UnitInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UnitInfoDlg.h"
#include "EtWorldEventArea.h"
#include "DNTableFile.h"
#include "TEtWorldEventArea.h"
#include "EtUIXML.h"
#include "StringUtil.h"


// CUnitInfoDlg dialog

IMPLEMENT_DYNAMIC(CUnitInfoDlg, CDialog)


extern DNTableFileFormat *s_pSox[3];
CUnitInfoDlg::CUnitInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnitInfoDlg::IDD, pParent)
	, m_nMonsterGroupID(0)
	, m_nValueMin(0)
	, m_nValueMax(0)
	, m_nCountMin(0)
	, m_nCountMax(0)
{
	m_pEventArea = NULL;
	m_nEventAreaCommonParamCount = 1;	// WorldPainter g_EventAreaPropertyDefine 갱신될때마다 수정해야하는 구조.
}

CUnitInfoDlg::~CUnitInfoDlg()
{
}

void CUnitInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_Text(pDX, IDC_EDIT2, m_nMonsterGroupID );
	DDX_Text(pDX, IDC_EDIT3, m_nValueMin );
	DDX_Text(pDX, IDC_EDIT4, m_nValueMax );
	DDX_Text(pDX, IDC_EDIT6, m_nCountMin );
	DDX_Text(pDX, IDC_EDIT5, m_nCountMax );
}


BEGIN_MESSAGE_MAP(CUnitInfoDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CUnitInfoDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CUnitInfoDlg message handlers

BOOL CUnitInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pEventArea == NULL ) return FALSE;
	// TODO:  Add extra initialization here

	m_ComboBox.SetCurSel(0);

	m_ListCtrl.AddColumn( "Monster ID", 70, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "Value", 40, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "%", 30, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "Lv", 30, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "Name", 220, LVCFMT_LEFT );

	m_ListCtrl.SetExtendedStyle( LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_FLATSB );


	CString szAreaName = m_pEventArea->GetName();
	char szHeadStr[256] = { 0, };
	m_nCompoundIndex = -1;
	sscanf_s( szAreaName.GetBuffer(), "%s %d", szHeadStr, 256, &m_nCompoundIndex );
	_strlwr_s( szHeadStr );
	if( m_nCompoundIndex == -1 ) return FALSE;

	CTEtWorldEventArea *pArea = (CTEtWorldEventArea *)m_pEventArea;
	m_nMonsterGroupID = m_nCompoundIndex;
	//rlkt 2016
//	m_nValueMin = (int)pArea->GetProperty(0+m_nEventAreaCommonParamCount)->GetVariableVector2Ptr()->x;
//	m_nValueMax = (int)pArea->GetProperty(0+m_nEventAreaCommonParamCount)->GetVariableVector2Ptr()->y;
	UpdateData( FALSE );

	RefreshMonsterInfo( m_nCompoundIndex, m_ComboBox.GetCurSel() );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUnitInfoDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	RefreshMonsterInfo( m_nCompoundIndex, m_ComboBox.GetCurSel() );
}


void CUnitInfoDlg::RefreshMonsterInfo( int nCompoundIndex, int nDifficulty )
{
	m_ListCtrl.DeleteAllItems();
	char szLabel[64];
	char szTemp[256];
	std::vector<int> nVecList;
	for( int i=0; i<15; i++ ) {
		sprintf_s( szLabel, "_MonsterTableID%d_%d", nDifficulty + 1, i + 1 );
		int nMonsterID = s_pSox[1]->GetFieldFromLablePtr( nCompoundIndex, szLabel )->GetInteger();
		if( nMonsterID < 1 ) continue;
		sprintf_s( szLabel, "_Ratio%d_%d", nDifficulty + 1, i + 1 );
		float fRatio = s_pSox[1]->GetFieldFromLablePtr( nCompoundIndex, szLabel )->GetFloat();
		sprintf_s( szLabel, "_Value%d_%d", nDifficulty + 1, i + 1 );
		int nValue = s_pSox[1]->GetFieldFromLablePtr( nCompoundIndex, szLabel )->GetInteger();
		nVecList.push_back( nValue );

		itoa( nMonsterID, szTemp, 10 );
		int nItemID = m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), szTemp );

		itoa( nValue, szTemp, 10 );
		m_ListCtrl.SetItemText( nItemID, 1, szTemp );

		itoa( (int)(fRatio*100.f), szTemp, 10 );
		m_ListCtrl.SetItemText( nItemID, 2, szTemp );

		if( !s_pSox[0]->IsExistItem( nMonsterID ) ) {
			m_ListCtrl.SetItemText( nItemID, 3, "?" );
			m_ListCtrl.SetItemText( nItemID, 4, "Invalid Item ID" );
			continue;
		}
		nValue = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_Level" )->GetInteger();

		itoa( nValue, szTemp, 10 );
		m_ListCtrl.SetItemText( nItemID, 3, szTemp );

		nValue = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_NameID" )->GetInteger();
		std::string szName;
		ToMultiString( std::wstring(CEtUIXML::GetInstance().GetUIString( CEtUIXML::idCategory1, nValue )), szName );
		m_ListCtrl.SetItemText( nItemID, 4, szName.c_str() );

	}

	if( !nVecList.empty() ) {
		std::sort( nVecList.begin(), nVecList.end() );
		m_nCountMin = m_nValueMin / nVecList[0];
		m_nCountMax = m_nValueMax / nVecList[nVecList.size()-1];
		UpdateData( FALSE );
	}

}