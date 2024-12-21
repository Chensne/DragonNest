// MonsterCounterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MonsterCounterDlg.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "TEtWorldEventArea.h"
#include "DNTableFile.h"
#include "Resource.h"
#include "EtWorldGrid.h"

extern DNTableFileFormat *s_pSox[5];

// CMonsterCounterDlg dialog

const char *s_szMonsterRaceStr[] = {
	"하운드노말",
	"하운드설원",
	"하운드사막",
	"헬하운드",
	"하운드노말강화",
	"하운드설원강화",
	"하운드사막강화",
	"헬하운드강화",
	"박쥐블랙",
	"박쥐그레이",
	"박쥐블루",
	"고블린스켈레톤",
	"오크스켈레톤",
	"다크엘프스켈레톤",
	"트롤스켈레톤",
	"스파이더옐로우",
	"스파이더블랙",
	"스파이더블루",
	"고블린그린",
	"고블린블루",
	"고블린화이트",
	"고블린블랙",
	"고블린레드",
	"고블린그레이",
	"코볼트워리어옐로우",
	"코볼트워리어그린",
	"코볼트워리어그레이",
	"코볼트워리어블루",
	"코볼트워리어블랙",
	"코볼트워리어레드",
	"코볼트아처옐로우",
	"코볼트아처그린",
	"코볼트아처블루",
	"코볼트아처레드",
	"코볼트아처블랙",
	"위습블루",
	"위습그레이",
	"위습그린",
	"위습옐로우",
	"위습블랙",
	"위습레드",
	"스켈레톤그레이",
	"스켈레톤블루",
	"스켈레톤화이트",
	"스켈레톤블랙",
	"스켈레톤그린",
	"스켈레톤골드",
	"스켈레톤레드",
	"구울그레이",
	"구울그린",
	"구울블루",
	"구울레드",
	"구울화이트",
	"구울머미",
	"칵퉤꽃그린",
	"칵퉤꽃블루",
	"칵퉤꽃레드",
	"칵퉤꽃화이트",
	"리빙소드그레이",
	"리빙소드블루",
	"리빙북레드",
	"리빙북블랙",
	"리빙해머그레이",
	"리빙해머레드",
	"리빙완드옐로우",
	"리빙완드블루",
	"프로그맨워리어그린",
	"프로그맨워리어퍼플",
	"프로그맨워리어블랙",
	"드레이크해츨링그린",
	"드레이크해츨링블루",
	"드레이크해츨링레드",
	"카벙클퍼플",
	"카벙클화이트",
	"카벙클블랙",
	"큐브그레이",
	"큐브블랙",
	"아사이워리어레드",
	"아사이워리어그레이",
	"아사이워리어블랙",
	"아사이샤먼레드",
	"아사이샤먼블루",
	"아사이샤먼화이트",
	"그레이트비틀그린",
	"그레이트비틀블루",
	"그레이트비틀레드",
	"그레이트비틀블랙",
	"그레이트비틀퍼플",
	"그레이트비틀그레이",
	"그레이트비틀골드",
	"코볼트포병",
	"다크엘프그레이",
	"다크엘프블루",
	"다크엘프레드",
	"다크엘프블랙",
	"오크그린",
	"오크블루",
	"오크레드",
	"오크블랙",
	"오크그레이",
	"리자드맨그린",
	"리자드맨블루",
	"리자드맨화이트",
	"리자드맨블랙",
	"리자드맨레드",
	"브로화이트",
	"브로블루",
	"브로레드",
	"브로블랙",
	"미믹노말",
	"미믹실버",
	"미믹골드",
	"미믹레어",
	"트롤블루",
	"트롤블랙",
	"트롤화이트",
	"트롤레드",
	"홉고블린그린",
	"홉고블린옐로우",
	"홉고블린블랙",
	"홉고블린화이트",
	"홉고블린레드",
	"하피화이트",
	"하피레드",
	"하피블랙",
	"하피블루",
	"가고일그레이",
	"가고일블루",
	"가고일레드",
	"가고일블랙",
	"가고일그린",
	"라미아그린",
	"라미아그레이",
	"라미아블루",
	"라미아화이트",
	"라미아레드",
	"프로그맨소서러그레이",
	"프로그맨소서러레드",
	"프로그맨소서러블랙",
	"룬타이거블루",
	"룬타이거블랙",
	"룬타이거화이트",
	"미티어페어리레드",
	"미티어페어리블랙",
	"미티어페어리화이트",
	"용추종자매지션블루",
	"용추종자매지션블랙",
	"용추종자퍼니셔그린",
	"용추종자퍼니셔레드",
	"용추종자어쌔신그레이",
	"용추종자어쌔신블랙",
	"용추종자클레릭화이트",
	"용추종자클레릭블랙",
	"엘븐밴쉬그레이",
	"엘븐밴쉬블루",
	"엘븐밴쉬화이트",
	"룬엘프블루",
	"룬엘프화이트",
	"룬엘프레드",
	"엘븐데스나이트그레이",
	"엘븐데스나이트블루",
	"엘븐데스나이트블랙",
	"언스테이블블랙",
	"언스테이블블루",
	"언스테이블화이트",
	"미노타우르스바이슨",
	"미노타우르스바이슨강화",
	"미노타우르스노말",
	"미노타우르스노말강화",
	"미노타우르스레드",
	"쉐도우다크",
	"쉐도우파이어",
	"쉐도우아이스",
	"쉐도우라이트",
	"그린하프골렘",
	"스톤하프골렘",
	"아이언하프골렘",
	"아이스하프골렘",
	"파이어하프골렘",
	"레이스레드",
	"레이스그레이",
	"레이스블랙",
	"레이스블루",
	"오우거화이트",
	"오우거블랙",
	"오우거레드",
	"거대거미",
	"위습자이언트",
	"용추종자비숍블루",
	"용추종자비숍화이트",
	"용추종자비숍블랙",
	"키메라화이트",
	"키메라블루",
	"키메라레드",
	"엘븐리치레드",
	"엘븐리치블루",
	"엘븐리치블랙",
	"초원골렘",
	"스톤골렘",
	"아이스골렘",
	"파이어골렘",
	"아이언골렘",
	"만티코어노말",
	"만티코어레이드",
	"켈베로스",
	"씨드레이크",
	"비홀더",
	"비홀더촉수",
	"바실리스크",
	"그린드래곤",
	"하운드블랙강화",
	"용추종자신도",
	"오우거그레이",
	"블래스트데몬레드",
	"놀워리어옐로우",
	"놀거너옐로우",
	"보어워리어그레이",
	"보어거너그레이",
	"크로우어쌔신그레이",
	"크로우다이버그레이",
	"사이클롭스그린",
	"라이노맨그레이",
};

const char *s_szMonsterGradeStr[] = {
	"미니언",
	"노말",
	"챔피언",
	"엘리트",
	"네임드",
	"보스",
	"",
	"",
	"",
	"",
	"이벤트",
};

IMPLEMENT_DYNAMIC(CMonsterCounterDlg, CXTPDialog)

CMonsterCounterDlg::CMonsterCounterDlg(CWnd* pParent /*=NULL*/)
	: CXTPDialog(CMonsterCounterDlg::IDD, pParent)
{
	m_nLastEventAreaCount = 0;
	m_pSector = NULL;
	m_nEventAreaCommonParamCount = 1;
}

CMonsterCounterDlg::~CMonsterCounterDlg()
{
}

void CMonsterCounterDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_Combo);
	DDX_Control(pDX, IDC_LIST1, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CMonsterCounterDlg, CXTPDialog)
	ON_BN_CLICKED(IDOK, &CMonsterCounterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMonsterCounterDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMonsterCounterDlg::OnCbnSelchangeCombo1)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, &CMonsterCounterDlg::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_BUTTON1, &CMonsterCounterDlg::OnBnClickedExport)
END_MESSAGE_MAP()


// CMonsterCounterDlg message handlers

void CMonsterCounterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	OnOK();
}

void CMonsterCounterDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
//	OnCancel();
}

int GetAverageValue( int nMin, int nMax )
{
	if( nMin == nMax ) return nMin;
	return ( nMin + ( ( nMax - nMin ) / 2 ) );
}


void CMonsterCounterDlg::CalcMonsterIDMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax )
{
	CTEtWorldEventArea *pEventArea = (CTEtWorldEventArea *)pArea;

	char szTemp[256];
	int nMonsterID;
	sscanf_s( pArea->GetName(), "%s %d", szTemp, 256, &nMonsterID );
	if( !s_pSox[0]->IsExistItem( nMonsterID ) ) return;

	EtVector2 vVec = pEventArea->GetProperty(0+m_nEventAreaCommonParamCount)->GetVariableVector2();

	nMin = (int)vVec.x;
	nMax = (int)vVec.y;

	int nGenCount = ((CTEtWorldEventArea*)pArea)->GetProperty(1+m_nEventAreaCommonParamCount)->GetVariableInt();
	InsertMonsterList( nMonsterID, GetAverageValue( nMin, nMax ), ( nGenCount == 10 ) ? 0 : nGenCount + 1 );
}

void CMonsterCounterDlg::CalcMonsterGroupMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax )
{
	CTEtWorldEventArea *pEventArea = (CTEtWorldEventArea *)pArea;

	char szLabel[64];
	char szTemp[256];

	EtVector2 vVec = pEventArea->GetProperty(0+m_nEventAreaCommonParamCount)->GetVariableVector2();
	int nCompoundIndex;

	sscanf_s( pArea->GetName(), "%s %d", szTemp, 256, &nCompoundIndex );

	int nValueMin = (int)vVec.x;
	int nValueMax = (int)vVec.y;

	std::vector<int> nVecList;
	bool bInsertMonsterList = true;
	if( !s_pSox[1]->IsExistItem( nCompoundIndex ) ) return;
	for( int i=0; i<15; i++ ) {
		sprintf_s( szLabel, "_MonsterTableID%d_%d", nDifficulty + 1, i + 1 );
		int nMonsterID = s_pSox[1]->GetFieldFromLablePtr( nCompoundIndex, szLabel )->GetInteger();
		if( nMonsterID < 1 ) continue;
		sprintf_s( szLabel, "_Ratio%d_%d", nDifficulty + 1, i + 1 );
		float fRatio = s_pSox[1]->GetFieldFromLablePtr( nCompoundIndex, szLabel )->GetFloat();
		if( fRatio <= 0.f ) continue;
		sprintf_s( szLabel, "_Value%d_%d", nDifficulty + 1, i + 1 );
		int nValue = s_pSox[1]->GetFieldFromLablePtr( nCompoundIndex, szLabel )->GetInteger();
		if( nValue < 1 ) nValue = 1;

		if( !s_pSox[0]->IsExistItem( nMonsterID ) ) {
			continue;
		}
		nVecList.push_back( nValue );

		if( s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_HP" )->GetInteger() == 0 ) continue;
		if( bInsertMonsterList ) {
			int nGenCount = ((CTEtWorldEventArea*)pArea)->GetProperty(1+m_nEventAreaCommonParamCount)->GetVariableInt();
			InsertMonsterList( nMonsterID, GetAverageValue( nValueMin/nValue, nValueMax/nValue ), ( nGenCount == 10 ) ? 0 : nGenCount + 1 );
			bInsertMonsterList = false;
		}

//		nValue = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_Level" )->GetInteger();
	}

	if( !nVecList.empty() ) {
		std::sort( nVecList.begin(), nVecList.end() );
		nMin = nValueMin / nVecList[0];
		nMax = nValueMax / nVecList[nVecList.size()-1];
	}
}

void CMonsterCounterDlg::CalcMonsterSetMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax )
{
	CTEtWorldEventArea *pEventArea = (CTEtWorldEventArea *)pArea;
	int nSetID = pEventArea->GetProperty(0+m_nEventAreaCommonParamCount)->GetVariableInt();
	int nPosID = pEventArea->GetProperty(1+m_nEventAreaCommonParamCount)->GetVariableInt();

	if( !s_pSox[3]->IsExistItem( nSetID ) ) return;

	std::vector<int> nVecSetList;
	s_pSox[3]->GetItemIDListFromField( "_SetNo", nSetID, nVecSetList );

	char szLabel[64];

	int nCurMin = INT_MAX;
	int nCurMax = INT_MIN;

	for( DWORD i=0; i<nVecSetList.size(); i++ ) {
		int nDiff = s_pSox[3]->GetFieldFromLablePtr( nVecSetList[i], "_Difficulty" )->GetInteger();
		if( nDifficulty != nDiff ) continue;

		for( int j=0; j<20; j++ ) {
			sprintf_s( szLabel, "_PosID%d", j + 1 );
			int nPosition = s_pSox[3]->GetFieldFromLablePtr( nVecSetList[i], szLabel )->GetInteger();
			sprintf_s( szLabel, "_GroupID%d", j + 1 );
			int nGroup = s_pSox[3]->GetFieldFromLablePtr( nVecSetList[i], szLabel )->GetInteger();
			sprintf_s( szLabel, "_Num%d", j + 1 );
			int nCount = s_pSox[3]->GetFieldFromLablePtr( nVecSetList[i], szLabel )->GetInteger();
			if( nPosition != nPosID || nPosition < 1 || nGroup < 1 || nCount < 1 ) continue;

			// 확률상 0마리가 나올수도 잇으므로 그룹체크해줘야한다.
			int nProb = 0;
			for( int k=0; k<20; k++ ) {
				sprintf_s( szLabel, "_Prob%d", k + 1 );
				nProb += s_pSox[4]->GetFieldFromLablePtr( nGroup, szLabel )->GetInteger();
			}
			if( nProb < 1000000000 ) nCount = 0; // 

			if( nCount <= nCurMin ) nCurMin = nCount;
			if( nCount >= nCurMax ) nCurMax = nCount;

		}
	}

	nMin = nCurMin;
	nMax = nCurMax;
}

void CalcNpcIDMinMax( CEtWorldEventArea *pArea, int nDifficulty, int &nMin, int &nMax )
{
	CTEtWorldEventArea *pEventArea = (CTEtWorldEventArea *)pArea;

	char szTemp[256];
	int nMonsterID;
	sscanf_s( pArea->GetName(), "%s %d", szTemp, 256, &nMonsterID );
	if( !s_pSox[0]->IsExistItem( nMonsterID ) ) return;

	nMin++;
	nMax++;
}

bool CMonsterCounterDlg::CheckRefresh( CEtWorldSector *pSector )
{
	int nCurCount = 0;
	for( DWORD i=0; i<pSector->GetControlCount(); i++ ) {
		CEtWorldEventControl *pControl = pSector->GetControlFromIndex(i);
		switch( pControl->GetUniqueID() ) {
			case 0:	// ETE_UnitArea
				nCurCount += pControl->GetAreaCount();
				break;
			case 5:
				nCurCount += pControl->GetAreaCount();
				break;
		}
	}
	if( nCurCount != m_nLastEventAreaCount ) {
		m_nLastEventAreaCount = nCurCount;
		return true;
	}
	return false;
}

struct MonsterCountStruct {
	int nMonsterID;
	int nCount;
};

void CMonsterCounterDlg::InsertMonsterList( int nMonsterID, int nCount, int nGenCount )
{
	MonsterCountStruct *pStruct = NULL;
	for( DWORD i=0; i<m_VecMonsterList.size(); i++ ) {
		if( m_VecMonsterList[i].nMonsterID == nMonsterID ) {
			pStruct = &m_VecMonsterList[i];
			break;
		}
	}
	if( pStruct == NULL ) {
		MonsterCountStruct NewStruct;
		NewStruct.nMonsterID = nMonsterID;
		NewStruct.nCount = 0;
		NewStruct.nRace = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_MonsterRaceID" )->GetInteger();
		NewStruct.nGrade = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_MonsterGrade" )->GetInteger();
		NewStruct.nLevel = s_pSox[0]->GetFieldFromLablePtr( nMonsterID, "_Level" )->GetInteger();
		m_VecMonsterList.push_back( NewStruct );
		pStruct = &m_VecMonsterList[m_VecMonsterList.size()-1];
	}

	pStruct->nCount += ( nCount * nGenCount );
}

void CMonsterCounterDlg::Refresh( CEtWorldSector *pSector, bool bForce )
{
	m_pSector = pSector;
	if( !bForce && !CheckRefresh( pSector ) ) return;
	int nMonsterTotal[2] = { 0, };
	int nMonsterID[2] = { 0, };
	int nMonsterGroup[2] = { 0, };
	int nMonsterSet[2] = { 0, };
	int nNpcTotal[2] = { 0, };

	m_VecMonsterList.clear();

	int nDifficulty = m_Combo.GetCurSel();

	for( DWORD i=0; i<pSector->GetControlCount(); i++ ) {
		CEtWorldEventControl *pControl = pSector->GetControlFromIndex(i);
		switch( pControl->GetUniqueID() ) {
			case 0:	// ETE_UnitArea
				{
					for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) {
						CEtWorldEventArea *pArea = pControl->GetAreaFromIndex(j);

						char szHeadStr[256] = { 0, };
						sscanf_s( pArea->GetName(), "%s", szHeadStr, 256 );
						_strlwr_s( szHeadStr );

						int nMin = 0, nMax = 0;
						if( strcmp( szHeadStr, "monster" ) == NULL ) {
							CalcMonsterIDMinMax( pArea, nDifficulty, nMin, nMax );
							nMonsterID[0] += nMin;
							nMonsterID[1] += nMax;
						}
						else if( strcmp( szHeadStr, "monstergroup" ) == NULL ) {
							CalcMonsterGroupMinMax( pArea, nDifficulty, nMin, nMax );
							nMonsterGroup[0] += nMin;
							nMonsterGroup[1] += nMax;
						}
						else if( strcmp( szHeadStr, "npc" ) == NULL ) {
							CalcNpcIDMinMax( pArea, nDifficulty, nMin, nMax );
							nNpcTotal[0] += nMin;
							nNpcTotal[1] += nMax;
						}
					}
				}
				break;
			case 5:	// ETE_MonsterSet
				{
					for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) {
						CEtWorldEventArea *pArea = pControl->GetAreaFromIndex(j);

						int nMin = 0, nMax = 0;
						CalcMonsterSetMinMax( pArea, nDifficulty, nMin, nMax );
						nMonsterSet[0] += nMin;
						nMonsterSet[1] += nMax;
					}
				}
				break;

		}
	}
	nMonsterTotal[0] = nMonsterID[0] + nMonsterGroup[0] + nMonsterSet[0];
	nMonsterTotal[1] = nMonsterID[1] + nMonsterGroup[1] + nMonsterSet[1];

	
	char szTemp[256];

	sprintf_s( szTemp, "%d~%d", nMonsterTotal[0], nMonsterTotal[1] );
	SetDlgItemText( IDC_STATIC_MON_COUNTER, szTemp );

	sprintf_s( szTemp, "%d~%d", nMonsterID[0], nMonsterID[1] );
	SetDlgItemText( IDC_STATIC_ID_COUNTER, szTemp );

	sprintf_s( szTemp, "%d~%d", nMonsterGroup[0], nMonsterGroup[1] );
	SetDlgItemText( IDC_STATIC_GROUP_COUNTER, szTemp );

	sprintf_s( szTemp, "%d~%d", nMonsterSet[0], nMonsterSet[1] );
	SetDlgItemText( IDC_STATIC_SET_COUNTER, szTemp );

	sprintf_s( szTemp, "%d~%d", nNpcTotal[0], nNpcTotal[1] );
	SetDlgItemText( IDC_STATIC_NPC_COUNTER, szTemp );

	for( DWORD i=0; i<m_VecMonsterList.size(); i++ ) {
		if( m_VecMonsterList[i].nCount <= 0 ) {
			m_VecMonsterList.erase( m_VecMonsterList.begin() + i );
			i--;
		}
	}
	m_ListCtrl.DeleteAllItems();
	for( DWORD i=0; i<m_VecMonsterList.size(); i++ ) {
		sprintf_s( szTemp, "%d", m_VecMonsterList[i].nMonsterID );
		m_ListCtrl.InsertItem( i, szTemp );
		sprintf_s( szTemp, "%d", m_VecMonsterList[i].nCount );
		m_ListCtrl.SetItemText( i, 1, szTemp );
		sprintf_s( szTemp, "%d", m_VecMonsterList[i].nLevel );
		m_ListCtrl.SetItemText( i, 2, szTemp );
		if( m_VecMonsterList[i].nRace > 0 && m_VecMonsterList[i].nRace <= sizeof(s_szMonsterRaceStr) / sizeof(char*) )
			sprintf_s( szTemp, "%s", s_szMonsterRaceStr[m_VecMonsterList[i].nRace-1] );
		else sprintf_s( szTemp, "알수없음" );
		m_ListCtrl.SetItemText( i, 3, szTemp );
		sprintf_s( szTemp, "%s", s_szMonsterGradeStr[m_VecMonsterList[i].nGrade] );
		m_ListCtrl.SetItemText( i, 4, szTemp );
	};
	UpdateData( FALSE );
}
BOOL CMonsterCounterDlg::OnInitDialog()
{
	CXTPDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_Combo.SetCurSel(0);

	m_ListCtrl.AddColumn( "MonsterID", 60 );
	m_ListCtrl.AddColumn( "Count", 30, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "Level", 30, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "Type", 80, LVCFMT_RIGHT );
	m_ListCtrl.AddColumn( "Grade", 40, LVCFMT_RIGHT );
//	m_ListCtrl.AddColumn( "Area Type", 100, LVCFMT_RIGHT );

	m_ListCtrl.SetExtendedStyle( LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_FLATSB );


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMonsterCounterDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	Refresh( m_pSector, true );
}

void CMonsterCounterDlg::OnDestroy()
{
	CXTPDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CMonsterCounterDlg::OnBnClickedRefresh()
{
	Refresh( m_pSector, true );
}

CString s_szLastExportPath;
void CMonsterCounterDlg::OnBnClickedExport()
{
	TCHAR szFilter[] = _T( "Text File (*.txt)|*.txt|All Files (*.*)|*.*||" );
	CFileDialog dlg( FALSE, _T("txt"), _T("*.txt"), OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_ENABLESIZING , szFilter, this );

	dlg.m_ofn.lpstrTitle = "Export Text File";

	CXTBrowseDialog Dlg;
	UpdateData(TRUE);
	Dlg.SetTitle(_T("Select Export Directory"));
	if( !s_szLastExportPath.IsEmpty() ) {
		Dlg.SetSelPath(s_szLastExportPath);
	}

	if( Dlg.DoModal() == IDOK ) {
		s_szLastExportPath = Dlg.GetSelPath();
		CString szStr = Dlg.GetSelPath();
		szStr += "\\";
		szStr += m_pSector->GetParentGrid()->GetName();
		szStr += ".txt";

		FILE *fp;
		fopen_s( &fp, szStr.GetBuffer(), "wt" );
		if( fp == NULL ) {
			MessageBox( "저장할 수 없습니다." );
			return;
		}
		int nPrevCurSel = m_Combo.GetCurSel();
		for( DWORD i=0; i<5; i++ ) {
			m_Combo.SetCurSel(i);
			Refresh( m_pSector, true );
			ExportMonsterInfo( i, fp );
		}
		fclose(fp);
		m_Combo.SetCurSel( nPrevCurSel );
	}
}

void CMonsterCounterDlg::ExportMonsterInfo( int nDifficulty, FILE *fp )
{
	const char *szDifficultyStr[] = { "Easy", "Normal", "Hard", "VeryHard", "Abyss" };
	fprintf_s( fp, "%s\n", szDifficultyStr[nDifficulty] );

	for( DWORD i=0; i<m_VecMonsterList.size(); i++ ) {
		fprintf_s( fp, "%d %d %d %d %d\n", m_VecMonsterList[i].nMonsterID, m_VecMonsterList[i].nCount, m_VecMonsterList[i].nLevel, m_VecMonsterList[i].nRace, m_VecMonsterList[i].nGrade );
	}

}