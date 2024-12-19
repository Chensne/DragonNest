#include "StdAfx.h"
#include "DnAppellationTask.h"
#include "DnTableDB.h"
#include "SundriesFunc.h"
#include "AppellationSendPacket.h"
#include "DnPlayerActor.h"
#include "DnItem.h"
#include "EtUIXML.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnCharStatusDlg.h"
#include "DnAppellationDlg.h"
#include "DnUiString.h"
#include "DnAppellationBookDlg.h"
#include "DnCommonUtil.h"
#ifdef PRE_ADD_BESTFRIEND
#include "DnInspectPlayerDlg.h"
#endif
#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
#include "DnMainDlg.h"
#endif


#define	MAX_LENGTH	5096

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
int ParseCategoryID( const char * nString, const bool nMain );


#ifdef PRE_ADD_BESTFRIEND
int CDnAppellationTask::m_BFappellationIndex = -1; // 절친칭호 인덱스.
#endif

CDnAppellationTask::CDnAppellationTask()
: CTaskListener( false )
, m_nSelectAppellation( -1 )
, m_nSelectCoverAppellation( -1 )
#ifdef PRE_ADD_BESTFRIEND
, m_bAddChatBestFriend( false )
#endif
{
}

CDnAppellationTask::~CDnAppellationTask()
{
	Finalize();
}

bool CDnAppellationTask::Initialize()
{
	LoadAppellationCollectionList();
	return true;
}

void CDnAppellationTask::Finalize()
{
	SAFE_DELETE_PVEC( m_pVecAppellationList );
	m_vecAppellationCollection.clear();

#ifdef PRE_ADD_BESTFRIEND
	m_BFappellationIndex = -1; // 절친칭호 인덱스.
	m_bAddChatBestFriend = false;
#endif

}

void CDnAppellationTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
}

void CDnAppellationTask::OnDisconnectTcp( bool bValidDisconnect )
{
}

void CDnAppellationTask::OnDisconnectUdp()
{
}

void CDnAppellationTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch( nMainCmd ) {
		case SC_APPELLATION: OnRecvAppellationMessage( nSubCmd, pData, nSize ); break;
			
	}
}

void CDnAppellationTask::OnRecvAppellationMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eAppellation::SC_APPELLATION_LIST: OnRecvAppellationList( (SCAppellationList*)pData ); break;
		case eAppellation::SC_GAIN_APPELLATION: OnRecvGainAppellation( (SCAppellationGain*)pData ); break;
		case eAppellation::SC_SELECT_APPELLATION:OnRecvSelectAppellation( (SCSelectAppellation*)pData ); break;
		case eAppellation::SC_CHANGE_APPELLATION: OnRecvChangeAppellation( (SCChangeAppellation*)pData ); break;
		case eAppellation::SC_PERIOD_APPELLATION: OnRecvPeriodAppellation( (SCPeriodAppellationTIme*)pData ); break;
		case eAppellation::SC_RESET_PCBANGAPPELLATION: OnRecvPCBangAppellation(); break;
	}
}

void CDnAppellationTask::OnRecvAppellationList( SCAppellationList *pPacket )
{
	SAFE_DELETE_PVEC( m_pVecAppellationList );
	for( DWORD i=0; i<APPELLATIONMAX; ++i ) {
		if( GetBitFlag( pPacket->cGain, i ) == true ) {
			AddAppellation( i );
		}
	}
}

void CDnAppellationTask::OnRecvGainAppellation( SCAppellationGain *pPacket )
{
	AppellationStruct *pStruct = AddAppellation( pPacket->nArrayIndex, true );
	if( !pStruct ) return;

	OnGainAppellation( pStruct );
}

void CDnAppellationTask::OnRecvSelectAppellation( SCSelectAppellation *pPacket )
{
	SelectAppellation( pPacket->nCoverArrayIndex, pPacket->nArrayIndex );
	OnSelectAppellation( pPacket->nArrayIndex );
}

void CDnAppellationTask::OnRecvChangeAppellation( SCChangeAppellation *pPacket )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(hActor.GetPointer());

#ifdef PRE_ADD_BESTFRIEND

	m_BFappellationIndex = -1;

	// 절친칭호.	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );	
	int nItemID = pSox->GetItemID( pPacket->nCoverArrayIndex );
	if( nItemID != -1 && pPacket->nCoverArrayIndex != -1 ) 
	{
		DNTableCell * pCell = pSox->GetFieldFromLablePtr( nItemID, "_Type" );	
		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::BestFriend )	
			m_BFappellationIndex = pPacket->nCoverArrayIndex; // 절친칭호 인덱스.
	}

#endif

	pActor->ChangeAppellationState( pPacket->nCoverArrayIndex, pPacket->nArrayIndex );
}

void CDnAppellationTask::OnRecvPCBangAppellation()
{
	std::wstring wszString = std::wstring( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6207 ) );	// UISTRING : PC방 전용 칭호 효과가 사라졌습니다. 칭호를 다시 적용하시기 바랍니다.

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString.c_str(), false );	
	GetInterface().ShowCaptionDialog( CDnInterface::typeCaption3,  wszString.c_str(), textcolor::YELLOW ,8.0f );
}

void CDnAppellationTask::OnRecvPeriodAppellation( SCPeriodAppellationTIme *pPacket )
{
	for( int i=0; i<pPacket->cPeriodCount; i++ )
	{
		for( DWORD j=0; j<m_pVecAppellationList.size(); j++ )
		{
			if(m_pVecAppellationList[j]->nArrayIndex == pPacket->AppellationTime[i].nArrayID)
			{
				m_pVecAppellationList[j]->szPeriodAppellation = MakePeriodAppellation(pPacket->AppellationTime[i].tExpireDate);
				m_pVecAppellationList[j]->tExpireDate = pPacket->AppellationTime[i].tExpireDate;
			}
		}
	}
	
}
tstring CDnAppellationTask::MakePeriodAppellation(__time64_t tExpireDate)
{
	std::wstring result;

	WCHAR dateString[256] = {0,};	
	if (tExpireDate == 0)
	{
		return result;
	}

	tm Day;
	if( !DnLocalTime_s(&Day, &tExpireDate) ) return result;
	//아래 년도 제한 확인 코드를 여기서 검사 하도록함..
	if ((Day.tm_year + 1900) >= 2020 || (Day.tm_year + 1900) < 2010)	// temp by kalliste
		return result;

	const time_t* pExpireDate = &tExpireDate;
	CommonUtil::GetCashRemainDate(*pExpireDate, dateString, 256);
	bool bRet = false;

	std::wstring stringProcessed;
	if (CommonUtil::ConvertTimeFormatString(dateString, stringProcessed) != false)
	{
		result = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2115 ), stringProcessed.c_str()); // UISTRING : %s 까지 사용
	}

	return result;
}

void CDnAppellationTask::SelectAppellation( int nCoverArrayIndex, int nArrayIndex, bool bSetName )
{
	// 절친칭호를 사용중이었다면 HeadName을 변경한다.
	if( m_nSelectCoverAppellation == nCoverArrayIndex )
		bSetName = true;

	m_nSelectCoverAppellation = nCoverArrayIndex;
	m_nSelectAppellation = nArrayIndex;

	if( !CDnActor::s_hLocalActor ) return;

#ifdef PRE_ADD_BESTFRIEND

	m_BFappellationIndex = -1;

	// 절친칭호.	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );	
	int nItemID = pSox->GetItemID( nCoverArrayIndex );
	if( nItemID != -1 && nCoverArrayIndex != -1 ) 
	{		
		DNTableCell * pCell = pSox->GetFieldFromLablePtr( nItemID, "_Type" );	
		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::BestFriend )	
			m_BFappellationIndex = nCoverArrayIndex; // 절친칭호 인덱스.
	}

#endif

//#ifdef PRE_ADD_NEWCOMEBACK
//	// 능력칭호 -
//	// 귀환자 칭호를 적용할때만 귀환자처리를 하도록 함.	
//	DNTableFileFormat* pTable = GetDNTable( CDnTableDB::TAPPELLATION );	
//	int nID = pSox->GetItemID( m_nSelectAppellation );
//	if( nID != -1 && m_nSelectAppellation != -1 ) 
//	{		
//		DNTableCell * pCell = pTable->GetFieldFromLablePtr( nID, "_Type" );	
//		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::ComeBack )	
//			GetInterface().SetComebackLocalUser( true );
//		else
//			GetInterface().SetComebackLocalUser( false );
//	}
//
//	// 커버칭호 -
//	nID = pSox->GetItemID( m_nSelectCoverAppellation );
//	if( nID != -1 && m_nSelectCoverAppellation != -1 ) 
//	{		
//		DNTableCell * pCell = pTable->GetFieldFromLablePtr( nID, "_Type" );	
//		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::ComeBack )	
//			GetInterface().SetComebackAppellation( true );
//		else
//			GetInterface().SetComebackAppellation( false );
//	}
//#endif // PRE_ADD_NEWCOMEBACK

	if( bSetName )
	{
		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		pActor->ChangeAppellationState( m_nSelectCoverAppellation, m_nSelectAppellation );
	}

}

CDnAppellationTask::AppellationStruct *CDnAppellationTask::AddAppellation( int nArrayIndex, bool nNew )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
	int nItemID = pSox->GetItemID( nArrayIndex );
	if( nItemID == -1 ) return NULL;

	AppellationStruct *pStruct = new AppellationStruct;
	pStruct->nArrayIndex = nArrayIndex;

#ifdef PRE_ADD_BESTFRIEND
	DNTableCell * pCell = pSox->GetFieldFromLablePtr( nItemID, "_Type" );
	
	// 절친칭호.	
	if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::BestFriend )	
	{
		wchar_t strName[256] = {0,};		
		const TCHAR * pBFname = GetInterface().GetNameBF();
		if( pBFname )
		{
			m_BFappellationIndex = nArrayIndex; // 절친칭호 인덱스.

			swprintf_s( strName, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() ), 
					    pBFname );
			pStruct->szName.assign( strName );		
		}
		else
			pStruct->szName.assign( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4349 ) );
	}

	// 일반칭호.
	else
		pStruct->szName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() );

#else
	pStruct->szName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() );
#endif
	pStruct->szEffectDescription = MakeDescription( nArrayIndex, EFFECT_DESC );
	pStruct->szContantDescription = MakeDescription( nArrayIndex, CONTANT_DESC );
	pStruct->szTakeCondition = MakeDescription( nArrayIndex, CONDITION_DESC );
	pStruct->bNew = nNew;
	
	pStruct->cColorType = pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();
	char* szColorValue = pSox->GetFieldFromLablePtr(nItemID, "_ColorValue")->GetString();
	DWORD colorValue = 0xFFFFFFFF;
	if (szColorValue != NULL)
		colorValue = strtoul(szColorValue, NULL, 16);
	if (colorValue == 0)
		colorValue = 0xFFFFFFFF;

	pStruct->ColorValue = colorValue;

	if( pStruct->cColorType == AppellationType::Type::PCBang )
	{
		//피씨방 칭호이면 뒤에 (PC방) 을 붙여준다.
		pStruct->szName += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3318);
	}

	GetParamIndex( nArrayIndex, pStruct->vParamIndex );

	bool bInsert = false;
	for( DWORD i=0; i<m_pVecAppellationList.size(); i++ ) {
		if( m_pVecAppellationList[i]->nArrayIndex > nArrayIndex ) {
			bInsert = true;
			m_pVecAppellationList.insert( m_pVecAppellationList.begin() + i, pStruct );
			break;
		}
	}
	if( !bInsert ) m_pVecAppellationList.push_back( pStruct );

	return pStruct;
}

void CDnAppellationTask::GetParamIndex( int nArrayIndex, std::vector< std::pair<int,int> > & nParamIndex )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
	int nItemID = pSox->GetItemID( nArrayIndex );
	if( nItemID == -1 ) return;

	char szLabel[32];
	for( int i=0; i<10; i++ ) 
	{
		sprintf_s( szLabel, "_State%d", i + 1 );
		int nStateType = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
		if( nStateType >= 0 ) 
		{
			nParamIndex.push_back( make_pair(nStateType, i) );
		}
	}
}

bool CDnAppellationTask::IsExistAppellation( int nArrayIndex )
{
	for( DWORD i=0; i<m_pVecAppellationList.size(); i++ ) {
		if( m_pVecAppellationList[i]->nArrayIndex == nArrayIndex ) return true;
	}
	return false;
}

tstring CDnAppellationTask::MakeDescription( int nArrayIndex, const DESC_INDEX nIndex )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
	int nItemID = pSox->GetItemID( nArrayIndex );
	if( nItemID == -1 ) return L"";

	tstring wszDesc;
#ifdef PRE_ADD_NONE_EFFECT_APPELLATION_NOTIFY
	bool bHasCondition = false;
#endif

	//칭호 설명 부분
	if( EFFECT_DESC != nIndex && CONDITION_DESC != nIndex )	//효과 설명 일 경우에는 칭호 설명을 빼야 한다.
	{
		wszDesc = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_DescriptionID" )->GetInteger() );
		if ( !wszDesc.empty() )
			wszDesc += L"\n";
	}

	//효과 설명 부분
	// 여기서 능력치가 있을경우 Description 추가로 미리 만들어놓차
	if( CONTANT_DESC != nIndex && CONDITION_DESC != nIndex )	//칭호 설명일 경우에는 효과 설명을 빼야 한다.
	{
		char szLabel[32];
		WCHAR wszTemp[128] = { 0, };
		for( int i=0; i<10; i++ ) {
			sprintf_s( szLabel, "_State%d", i + 1 );
			int nStateType = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetInteger();
			if( nStateType >= 0 ) {
				sprintf_s( szLabel, "_StateValue%d", i + 1 );
				char *szStateValue = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetString();
				// + State
				if( (nStateType < 50 && !(16 <= nStateType && nStateType <= 23)) || nStateType == 100 ) {
					if( nStateType == 100 ) nStateType = 30;		// 근성
					if( nStateType == 4 || nStateType == 6 )		// 최소공격력(일반공, 마법공)
						swprintf_s( wszTemp, L"%s %c%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStateType == 4 ? 5018 : 5019 ), ( atoi(szStateValue) >= 0 ) ? '+' : '-', abs(atoi(szStateValue)) );
					else if( nStateType == 5 || nStateType == 7 )	// 최대공격력
						swprintf_s( wszTemp, L"~ %c%d\n", ( atoi(szStateValue) >= 0 ) ? '+' : '-', abs(atoi(szStateValue)) );
					else
						swprintf_s( wszTemp, L"%s %c%d\n", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CDnItem::GetStateValueMatchUIStringIndex(nStateType) ), ( atoi(szStateValue) >= 0 ) ? '+' : '-', abs(atoi(szStateValue)) );
				}
				// % State
				else {
					if( nStateType >= 50 ) nStateType -= 50;
					float fValue = (float)atof(szStateValue);
					if( nStateType == 4 || nStateType == 6 )
						swprintf_s( wszTemp, L"%s %c%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStateType == 4 ? 5018 : 5019 ), ( fValue >= 0.f ) ? '+' : '-', abs( fValue ) * 100.f );
					else if( nStateType == 5 || nStateType == 7 )
						swprintf_s( wszTemp, L"~ %c%.2f%%\n", ( fValue >= 0.f ) ? '+' : '-', abs( fValue ) * 100.f );
					else
						swprintf_s( wszTemp, L"%s %c%.2f%%\n", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CDnItem::GetStateValueMatchUIStringIndex(nStateType) ), ( fValue >= 0.f ) ? '+' : '-', abs( fValue ) * 100.f );
				}
				wszDesc += wszTemp;
#ifdef PRE_ADD_NONE_EFFECT_APPELLATION_NOTIFY
				bHasCondition = true;
#endif
			}
		}
	}

	//조건 설명 부분
	if( CONDITION_DESC == nIndex )
	{
		char * szParam;
		WCHAR wszTemp[128] = { 0, };
		tstring wszTempDesc;
		int nNameID;
		int nCategoryMain, nCategorySub;
		pSox = GetDNTable( CDnTableDB::TMISSION );
		bool bFind = false;
		for( int itr = 0; itr < pSox->GetItemCount(); ++itr )
		{
			if( pSox->GetFieldFromLablePtr(pSox->GetItemID(itr), "_RewardAppellation")->GetInteger() == nItemID )
			{
				nItemID = pSox->GetItemID(itr);	
				bFind = true;
				break;
			}
		}
#ifdef PRE_ADD_MONTHLY_MISSION
		// 미션 테이블에서 찾지 못했을 경우 데일리 미션 테이블에서 찾는다.
		if( ! bFind )
		{	
			pSox = GetDNTable( CDnTableDB::TDAILYMISSION );
			for( int itr = 0; itr < pSox->GetItemCount(); ++itr )
			{
				if( pSox->GetFieldFromLablePtr(pSox->GetItemID(itr), "_RewardAppellation")->GetInteger() == nItemID )
				{
					nItemID = pSox->GetItemID(itr);	
					bFind = true;
					break;
				}
			}
		}
#endif 

		// 미션으로 얻지 못한 호칭일 경우 찾을 수 없을거다.
		if( bFind )
		{
			szParam = pSox->GetFieldFromLablePtr( nItemID, "_SubCategoryID" )->GetString();
			nCategoryMain = ParseCategoryID(szParam, true);
			nCategorySub = ParseCategoryID(szParam, false);

			if( -1 != nCategorySub )
			{
				swprintf_s( wszTemp, L"%s : %s\n", 
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9164 ),
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nCategorySub) );
				wszDesc = wszTemp;
			}
			else
			{
				swprintf_s( wszTemp, L"%s : %s\n", 
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9164 ), 
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nCategoryMain) );
				wszDesc = wszTemp;
			}

			nNameID = pSox->GetFieldFromLablePtr( nItemID, "_TitleNameID" )->GetInteger();
			szParam = pSox->GetFieldFromLablePtr( nItemID, "_TitleNameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszTempDesc, nNameID, szParam);

			swprintf_s( wszTemp, L"%s : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9165), wszTempDesc.c_str() );

			wszDesc += wszTemp;		
		}
	}

#ifdef PRE_ADD_NONE_EFFECT_APPELLATION_NOTIFY
	if(!bHasCondition && nIndex == ANOTHER_DESC)
	{
		WCHAR wszTemp[128] = { 0, };
		swprintf_s( wszTemp, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9196) );
		wszDesc += wszTemp;
		return wszDesc;
	}
#endif

	return wszDesc;
}

void CDnAppellationTask::OnGainAppellation( AppellationStruct *pStruct )
{
	CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( !pCharStatusDlg ) return;

	CDnAppellationDlg *pAppDlg = pCharStatusDlg->GetAppellationDlg();
	if( !pAppDlg ) return;

	if( pAppDlg->IsShow() )
		pAppDlg->RefreshList();

	UpdateMyAppellationCollectionList();

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	CDnMainDlg* pMainBarDlg = GetInterface().GetMainBarDialog();
	if( pMainBarDlg ) 
		pMainBarDlg->ShowAppellationNotify(true);
#endif

#ifdef PRE_ADD_BESTFRIEND
	// 이곳에 호출된 시점에 절친정보는 아직넘어오지 않는다. 보관해두었다가 절친정보가 넘어온 뒤에 처리해야한다.
	if( m_BFappellationIndex != -1 )
	{
		m_bAddChatBestFriend = true;	
		return;
	}
#endif

	WCHAR wszTemp[256];
	swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7130 ), pStruct->szName.c_str() );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
}

void CDnAppellationTask::OnSelectAppellation( int nSelectIndex )
{
	CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( !pCharStatusDlg ) return;

	CDnAppellationDlg *pAppDlg = pCharStatusDlg->GetAppellationDlg();
	if( !pAppDlg ) return;

	if( pAppDlg->IsShow() )
		pAppDlg->RefreshSelect();
}

void CDnAppellationTask::RequestSelectAppellation( int nArrayIndex, int nCoverArrayIndex )
{
	SendSelectAppellation( nArrayIndex, nCoverArrayIndex );
}

void CDnAppellationTask::LoadAppellationCollectionList()
{
	m_vecAppellationCollection.clear();

	DNTableFileFormat*  pTable = GetDNTable( CDnTableDB::TCOLLECTIONBOOK );

	if( pTable == NULL )
		return;

	int nCollectionCount = pTable->GetItemCount();
	for( int i=0; i<nCollectionCount; i++ )
	{
		int iItemID = pTable->GetItemID( i );

		stAppellationCollection AppellationCollection;
		AppellationCollection.m_nTitleID = iItemID;
		AppellationCollection.m_strCollectionTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pTable->GetFieldFromLablePtr( iItemID, "_TitleID" )->GetInteger() );
		AppellationCollection.m_strCollectionDescription = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pTable->GetFieldFromLablePtr( iItemID, "_InfoID" )->GetInteger() );

		int nPageType = pTable->GetFieldFromLablePtr( iItemID, "_PageType" )->GetInteger();

		if( nPageType == 1)
			AppellationCollection.m_bOpen = true;
		else
			AppellationCollection.m_bOpen = false;

		AppellationCollection.m_fPercent = 0.0f;
		AppellationCollection.m_nDifficulty = pTable->GetFieldFromLablePtr( iItemID, "_TitleColor" )->GetInteger();
		AppellationCollection.m_nRewardAppellationIndex = pTable->GetFieldFromLablePtr( iItemID, "_Reward" )->GetInteger();

		char szStr[64];
		for( int j=0; j<MAX_APPITEM_PER_PAGE; j++ )
		{
			sprintf_s( szStr, "_Appellation%d", j+1 );
			int nAppellationIndex = pTable->GetFieldFromLablePtr( iItemID, szStr )->GetInteger();
			if( nAppellationIndex != 0 )
				AppellationCollection.m_mapAppellationIndex.insert( std::make_pair( nAppellationIndex, 0 ) );
		}

		m_vecAppellationCollection.push_back( AppellationCollection );
	}
}

void CDnAppellationTask::UpdateMyAppellationCollectionList()
{
	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( !pCharStatusDlg ) return;

	CDnAppellationDlg* pAppDlg = pCharStatusDlg->GetAppellationDlg();
	if( !pAppDlg ) return;

	pAppDlg->SetCollectionMaxPage( static_cast<int>( m_vecAppellationCollection.size() ) );

	std::vector<stAppellationCollection>::iterator iter = m_vecAppellationCollection.begin();
	for( ; iter!=m_vecAppellationCollection.end(); iter++ )
	{
		int nAchievementCount = 0;
		std::map<int, int>::iterator iterAppellationIndex = (*iter).m_mapAppellationIndex.begin();
		for( ; iterAppellationIndex!=(*iter).m_mapAppellationIndex.end(); iterAppellationIndex++ )
		{
			if( IsExistAppellation( (*iterAppellationIndex).first - 1 ) )
			{
				(*iterAppellationIndex).second = 1;
				nAchievementCount++;
			}
		}

		float fPercent = 0.0f;
		if( nAchievementCount > 0 )
			fPercent = ( (float)( (float)( nAchievementCount ) ) / (*iter).m_mapAppellationIndex.size() ) * 100.0f;

		(*iter).m_fPercent = fPercent;

		// 2012.09.27이후로, 서버에서 처리합니다.
		/*if ((nAchievementCount == (*iter).m_mapAppellationIndex.size()) && (!IsExistAppellation((*iter).m_nRewardAppellationIndex - 1)))
			SendCollectionBook((*iter).m_nTitleID);*/
	}
}

CDnAppellationTask::AppellationStruct *CDnAppellationTask::GetAppellationInfo( DWORD dwIndex )
{
	if( dwIndex >= m_pVecAppellationList.size() ) return NULL;
	return m_pVecAppellationList[dwIndex];
}

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
CDnAppellationTask::AppellationStruct *CDnAppellationTask::GetAppellationInfoByArrayIndex( DWORD dwArrayID )
{
	for(DWORD i = 0 ; i < m_pVecAppellationList.size() ; ++i)
	{
		if( m_pVecAppellationList[i]->nArrayIndex == dwArrayID )
			return m_pVecAppellationList[i];
	}

	return NULL;
}
#endif

int ParseCategoryID( const char * nString, const bool nMain )
{
	int Retval = -1;

	int len = (int)strlen(nString);
	char str[128] = {0, };

	if( nMain )
	{
		for( int itr = 0; itr < len; ++itr )
		{
			if( nString[itr] != '/' )
			{
				str[itr] = nString[itr];
			}
		}

		Retval = atoi(str);
	}
	else
	{
		int jtr = 0;
		bool pass = false;

		for( int itr = 0; itr < len; ++itr )
		{
			if( pass )
			{
				str[jtr] = nString[itr];
				++jtr;
			}

			if( nString[itr] == '/' )
				pass = true;
		}

		if( pass )
			Retval = atoi(str);
	}

	return Retval;
}

tstring CDnAppellationTask::GetAppellationName( int nArrayIndex, CDnPlayerActor * pActor )
{
	if( nArrayIndex == -1 ) return L"";

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
	int nItemID = pSox->GetItemID( nArrayIndex );
	if( nItemID == -1 ) return L"";

	tstring wszStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() );

#ifdef PRE_ADD_BESTFRIEND

	if( pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger() == AppellationType::Type::eCode::BestFriend )
	{
		if( pActor )
		{	
			/*if( m_BFappellationIndex == -1 )
			{
				wszStr.assign( L"" );
			}

			// 절친칭호.
			else if( m_BFappellationIndex != -1 && m_BFappellationIndex == nArrayIndex )
			{		
				wchar_t strName[256] = {0,};
				swprintf_s( strName, 256, wszStr.c_str(), pActor->GetBestfriendName() );
				wszStr.assign( strName );
			}*/
			
			if( nArrayIndex == -1 )
			{
				wszStr.assign( L"" );
			}

			// 절친칭호.
			else
			{		
				wchar_t strName[256] = {0,};
				swprintf_s( strName, 256, wszStr.c_str(), pActor->GetBestfriendName() );
				wszStr.assign( strName );
			}

		}
		else
		{
			wchar_t strName[256] = {0,};
			swprintf_s( strName, 256, wszStr.c_str(), GetInterface().GetNameBF() );
			wszStr.assign( strName );
		}
	}

#endif

	return wszStr;
}


#ifdef PRE_ADD_BESTFRIEND
void CDnAppellationTask::SetBFappellation()
{
	if( m_BFappellationIndex != -1 )
	{
		SelectAppellation( m_BFappellationIndex, -1, false );


		for( DWORD i=0; i<m_pVecAppellationList.size(); i++ )
		{
			if( m_pVecAppellationList[i]->nArrayIndex == m_BFappellationIndex )
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );
				int nItemID = pSox->GetItemID( m_BFappellationIndex );
				if( nItemID == -1 ) 
					break;

				wchar_t strName[256] = {0,};
				swprintf_s( strName, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger() ),
							GetInterface().GetNameBF() );				

				m_pVecAppellationList[i]->szName.assign( strName );	

				CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
				if( !pCharStatusDlg )
					break;

				CDnAppellationDlg *pAppDlg = pCharStatusDlg->GetAppellationDlg();
				if( !pAppDlg )
					break;

				if( pAppDlg->IsShow() )
					pAppDlg->RefreshList();

				break;
			}
		}

	}

}
#endif