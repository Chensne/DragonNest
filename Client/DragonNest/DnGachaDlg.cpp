#include "StdAfx.h"
#include "DnGachaDlg.h"
#include "DnGachaRouletteDlg.h"
#include "DnGachaBuyConfirm.h"
#include "DnGachaResultDlg.h"
#include "DnMainMenuDlg.h"
#include "DnGachaPreview.h"
#include "DnItemTask.h"
#include "DnTableDB.h"
#include <mmsystem.h>
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"
#include "DnNPCActor.h"
#include "DnPartyTask.h"
#include "DnCashShopTask.h"
#include "EtSoundEngine.h"
#include "DnInvenTabDlg.h"
#include "DnNPCTalkReturnDlg.h"


#ifdef PRE_ADD_GACHA_JAPAN


using namespace DN_INTERFACE;
using namespace STRING;


const float GACHA_ROULETTE_SPEED = 10.0f;
const int GACHA_ROULETTE_STOP_SEL = 20;				// 서버로부터 결과를 받아 10 번째 것으로 멈추도록 한다.

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnGachaDlg::CDnGachaDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) : 
						  CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true ),
						  m_iGachaIndex( 0 ),
						  m_pGachaPreviewDlg( NULL ),
						  m_pGachaResultDlg( NULL ),
						  m_pGachaBuyConfirm( NULL ),
						  m_pComboPartsSelect( NULL ),
						  m_pBtnOpenCashShop( NULL ),
						  m_pBtnClose( NULL ),
						  m_pStaticCoin( NULL ),
						  m_pBtnBuyGacha( NULL ),
						  m_pNameRouletteDlg( NULL ),
						  m_pStatRouletteDlg( NULL ),
						  m_pStaticItemNameArea( NULL ),
						  m_pStaticItemStatArea( NULL ),
						  m_iSelectedJobBtnID( -1 ),
						  m_iRouletteCount( 0 ),
						  m_bInitialized( false ),
						  m_bProcessRoulette( false ),
						  m_iSelectedPart( -1 ),
						  m_pResultItem( NULL ),
						  m_fStopAccel( 0.0f ),
						  m_iLeftStopSelCount( 0 ),
						  m_fRouletteSpeed( GACHA_ROULETTE_SPEED ),
						  m_fLeftRouletteDistance( 0.0f ),
						  m_fStopTimeStamp( 0.0f ),
						  m_fRouletteStopTimeGap( 0.0f ),
						  m_iResultItemStaticIndex( 0 ),
						  m_fRouletteElapsedTime( 0.0f ),
						  m_iPrevRouletteCount( 0 ),
						  m_eGachaStatus( WAITING ),
						  m_iRouletteChangeSound( -1 ),
						  m_iRouletteResultSound( -1 )
{
	SecureZeroMemory( m_apRadioBtnJob, sizeof(m_apRadioBtnJob) );
}

CDnGachaDlg::~CDnGachaDlg(void)
{
	Reset();

	SAFE_DELETE( m_pGachaPreviewDlg );
	SAFE_DELETE( m_pGachaBuyConfirm );
	SAFE_DELETE( m_pGachaResultDlg );
	SAFE_DELETE( m_pNameRouletteDlg );
	SAFE_DELETE( m_pStatRouletteDlg );
}


void CDnGachaDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "Jp_Base.ui" ).c_str(), bShow );


}


void CDnGachaDlg::_InitializeComboBox( void )
{
	// 콤보 박스 초기화.
	// 일본 가챠에서는 캐쉬템만 가챠로 뽑음.
	m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4798), NULL, -1 );

	//m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2221), NULL, 1 );	// 머리
	m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2222), NULL, CDnParts::CashHelmet );	// 캐쉬 투구 인덱스: 0
	m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2223), NULL, CDnParts::CashBody );	// 캐쉬 상의 인덱스: 1 ...
	m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2224), NULL, CDnParts::CashLeg );	// 캐쉬 하의
	m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2225), NULL, CDnParts::CashHand );	// 캐쉬 장갑
	m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2226), NULL, CDnParts::CashFoot ); // 캐쉬 신발
	//m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2227), NULL, 7 ); // 캐쉬 목걸이
	//m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2228), NULL, 8 ); // 캐쉬 귀걸이
	//m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2229), NULL, 9 ); // 캐쉬 반지
	//m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4798), NULL, 10 ); // 문장
	//m_pComboPartsSelect->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4798), NULL, 11 ); // 

	m_pComboPartsSelect->SetSelectedByIndex( 0 );
}


void CDnGachaDlg::InitialUpdate( void )
{
	m_pGachaPreviewDlg = new CDnGachaPreview( UI_TYPE_CHILD, this, PREVIEW_DLG );
	m_pGachaPreviewDlg->Initialize( false );

	m_pComboPartsSelect = GetControl<CEtUIComboBox>( "ID_COMBOBOX_PARTS" );

	char acBuf[ 64 ] = { 0 };
	for( int i = 0; i < GACHA_NUM_JOB_BTN; ++i )
	{
		sprintf_s( acBuf, sizeof(acBuf), "ID_TAB_JOB%d", i );
		m_apRadioBtnJob[ i ] = GetControl<CEtUIRadioButton>( acBuf );
		m_apRadioBtnJob[ i ]->SetButtonID( i );
	}

	m_pStaticItemNameArea = GetControl<CEtUIStatic>( "ID_STATIC_R_AREA1" );
	m_pStaticItemStatArea = GetControl<CEtUIStatic>( "ID_STATIC_R_AREA2" );

	m_pBtnOpenCashShop = GetControl<CEtUIButton>( "ID_BUTTON_BUYCOIN" );

	m_pStaticCoin = GetControl<CEtUIStatic>( "ID_STATIC_COIN" );
	m_pBtnBuyGacha = GetControl<CEtUIButton>( "ID_BUTTON_BUYGACHA" );

	m_pBtnClose = GetControl<CEtUIButton>( "ID_BUTTON_CLOSE" );

	m_pGachaBuyConfirm = new CDnGachaBuyConfirmDlg( UI_TYPE_MODAL, NULL, GACHA_BUY_CONFIRM_DLG, this );
	m_pGachaBuyConfirm->Initialize( false );

	m_pGachaResultDlg = new CDnGachaResultDlg( UI_TYPE_MODAL, NULL, GACHA_RESULT_DLG, this );
	m_pGachaResultDlg->Initialize( false );

	_InitializeComboBox();

	m_bInitialized = true;
}


void CDnGachaDlg::Reset( void )
{
	SAFE_DELETE( m_pResultItem );

	for_each( m_dqItemBuffer.begin(), m_dqItemBuffer.end(), 
			  DeleteData<CDnItem*>() );
	m_dqItemBuffer.clear();

	m_fStopAccel = 0.0f;
	m_fRouletteSpeed = GACHA_ROULETTE_SPEED;
	m_fLeftRouletteDistance = 0.0f;
	m_fStopTimeStamp = 0.0f;
	m_fRouletteStopTimeGap = 0.0f;
	m_fRouletteElapsedTime = 0.0f;
	m_iResultItemStaticIndex = 0;

	m_iPrevRouletteCount = 0;

	m_eGachaStatus = WAITING;

	// 룰렛 다이얼로그 위치를 나와야할 곳에 셋팅해준다.
	m_iRouletteCount = 0;
	if( m_pNameRouletteDlg )
	{
		SUICoord RouletteAreaCoord;
		m_pStaticItemNameArea->GetUICoord( RouletteAreaCoord );
		m_pNameRouletteDlg->SetPosition( RouletteAreaCoord.fX+GetXCoord(), RouletteAreaCoord.fY+GetYCoord() );
	}

	if( m_pStatRouletteDlg )
	{
		SUICoord RouletteAreaCoord;
		m_pStaticItemStatArea->GetUICoord( RouletteAreaCoord );
		m_pStatRouletteDlg->SetPosition( RouletteAreaCoord.fX+GetXCoord(), RouletteAreaCoord.fY+GetYCoord() );
	}

	// 구입 버튼 비활성화.
	//m_pBtnBuyGacha->Enable( false );

	// 룰렛 내용들 리셋
	m_pNameRouletteDlg->Clear();
	m_pStatRouletteDlg->Clear();
}


void CDnGachaDlg::_GatherGachaItems( void )
{
	DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );
	
	// 아이템 드랍 테이블 ID가 들어있다. 아이템의 직업정보는 직접 아이템 테이블을 조회해서 얻는다.
	DNTableFileFormat*  pGachaTable = GetDNTable( CDnTableDB::TGACHA_JP );

	DNTableFileFormat*  pItemDropTable = GetDNTable( CDnTableDB::TITEMDROP );
	DNTableFileFormat*  pPartsTable = GetDNTable( CDnTableDB::TPARTS );
	char acBuffer[ 256 ] = { 0 };

	// 드랍 그룹 테이블을 디벼서 나올 수 있는 모든 아이템들을 모아둔다.
	// TODO: 나중에 서버에서 가챠 테이블 번호를 보내줘야 하지만 현재 제작중이므로 인덱스 1을 사용. 추후에 npc 박으면 처리.
	//int iGachaIndex = 1;
	int iGachaIndex = m_iGachaIndex;
	vector<int> vlGachaTableIDs;
	pGachaTable->GetItemIDListFromField( "_GachaNum", iGachaIndex, vlGachaTableIDs );

	// TODO: 현재는 캐릭터 종류 4개지만 아카데믹 추가되면 5가 될 수 있음. 테이블에 넣기만 하면 된다.
	// 단정문은 고치도록 하자.
	_ASSERT( CLASSKINDMAX == vlGachaTableIDs.size() );

	// 같은 가챠폰 번호 안에서는 반드시 위에서 아래로 직업 클래스 아이디 순으로 데이터가 입력되어야 합니다!!
	int iNumTableIDs = (int)vlGachaTableIDs.size();
	for( int iTableID = 0; iTableID < iNumTableIDs; ++iTableID )
	{
		int iGachaTableID = vlGachaTableIDs.at( iTableID );
		int iNeedJobClassID = iTableID+1;
		for( int iPart = 0; iPart < CDnParts::CashPartsTypeEnum_Amount; ++iPart )
		{
			// 현재 파츠는 5개만 가챠에서 돌리게 되어있다. 추후 늘어나면 수정해야 함.
			// 없는 라벨인데 자꾸 이상한 값 나와서 안되겠음..
			if( CDnParts::CashFoot < iPart )
				continue;

			sprintf_s( acBuffer, "_LinkedDrop%d", iPart+1 );
			int iLinkedDropTableID = pGachaTable->GetFieldFromLablePtr( iGachaTableID, acBuffer )->GetInteger();
			if( 0 == iLinkedDropTableID || 1 == iLinkedDropTableID )		// 없는 테이블 레이블로 찾으면 1이 나옴...(??)
				continue;
			
			// TODO: 드랍 테이블 재귀값은 8단계 까지임.. 추후에 필요하면 처리해야 엄청난 루프에 빠지지 않음.
			vector<int> vlToCalcDropTableIDs;
			vlToCalcDropTableIDs.push_back( iLinkedDropTableID );

			for( int i = 0; i < (int)vlToCalcDropTableIDs.size(); ++i )
			{
				int iDropTableID = vlToCalcDropTableIDs.at( i );
				// 드랍테이블은 항목이 20개
				for( int iLabel = 0; iLabel <= 20; ++iLabel )
				{
					sprintf_s( acBuffer, "_IsGroup%d", iLabel );
					int iIsGroup = pItemDropTable->GetFieldFromLablePtr( iDropTableID, acBuffer )->GetInteger();
					if( iIsGroup == 1 )
					{
						sprintf_s( acBuffer, "_Item%dIndex", iLabel );
						int iReferenceDropTableID = pItemDropTable->GetFieldFromLablePtr( iDropTableID, acBuffer )->GetInteger();

						if( 0 == iReferenceDropTableID )
							continue;

						vlToCalcDropTableIDs.push_back( iReferenceDropTableID );
					}
					else
					{
						sprintf_s( acBuffer, "_Item%dIndex", iLabel );
						int iItemTableID = pItemDropTable->GetFieldFromLablePtr( iDropTableID, acBuffer )->GetInteger();

						if( 0 == iItemTableID )
							continue;

						//// 아이템의 필요 직업은 복수개일 수도 있다. 하지만 일본 가챠에서 나오는 상품엔 반드시 하나만 존재한다.
						//// 필요직업이 0인 경우 모든 직업에서 사용 가능한 것이 된다.
						//int iNeedJobClassID = CDnGachaPreview::GetItemNeedJob( pItemTable, iItemTableID );

						// 어떤 파츠인지 타입을 얻어옴. 현재 루프도는 파츠 타입과 맞아야 함.
						int iPartIndexInTable = pPartsTable->GetFieldFromLablePtr( iItemTableID, "_Parts" )->GetInteger();
						//_ASSERT( iPartIndexInTable == iPart );

						//// 아이템의 직업에 따라 맞춰서 넣어줌.
						//// 필요직업 클래스가 0 이면 모든 직업에서 장착 가능하기 때문에 모든 직업에 중복되지 않게 추가해준다.
						//if( 0 < iNeedJobClassID )
						//{
							m_mapItemsByJob[ iNeedJobClassID ].avlItemsByPart[ iPartIndexInTable ].push_back( iItemTableID );
						//}
						//else
						//{
						//	for( int iClassID = 1; iClassID <= 4; ++iClassID )
						//	{
						//		vector<int>& vlPartsItemIDs = m_mapItemsByJob[ iClassID ].avlItemsByPart[ iPartIndexInTable ];
						//		vector<int>::iterator iter = find( vlPartsItemIDs.begin(), vlPartsItemIDs.end(), iItemTableID );
						//		if( vlPartsItemIDs.end() == iter )
						//			vlPartsItemIDs.push_back( iItemTableID );
						//	}
						//}
					}
				}
			}
		}
	}
}


void CDnGachaDlg::Show( bool bShow )
{
	CDnCustomDlg::Show( bShow );
	
	// 가챠 프리뷰 다이얼로그에 샵 아이디 셋팅
	m_pGachaPreviewDlg->SetGachaponShopID( m_iGachaIndex );
	m_pGachaPreviewDlg->Show( bShow );

	if( bShow )
	{
		Reset();

		m_pComboPartsSelect->Enable( true );
		m_pComboPartsSelect->SetSelectedByIndex( 0 );

		m_iSelectedJobBtnID = CDnActor::s_hLocalActor->GetClassID() - 1;
		m_apRadioBtnJob[ m_iSelectedJobBtnID ]->SetChecked( true );

		for( int i = 0; i < GACHA_NUM_JOB_BTN; ++i )
			m_apRadioBtnJob[ i ]->Enable( true );

		m_pBtnOpenCashShop->Enable( true );
		m_pBtnBuyGacha->Enable( true );

		// 유저가 보유하고 있는 가챠 코인의 갯수를 세어서 표시해준다.
		UpdateGachaCoin();

		// 지금 열은 가챠폰에서 나올 수 있는 모든 아이템 id들을 모아놓는다.
		m_mapItemsByJob.clear();
		//SecureZeroMemory( m_aiDropTableIDs, sizeof(m_aiDropTableIDs) );
		_GatherGachaItems();

		// 사운드 로드
		if( -1 == m_iRouletteChangeSound )
			m_iRouletteChangeSound = CEtSoundEngine::GetInstance().LoadSound( "UI_Gacha_roulette.wav", false, false );

		if( -1 == m_iRouletteResultSound )
			m_iRouletteResultSound = CEtSoundEngine::GetInstance().LoadSound( "UI_Gacha_get.wav", false, false );

		// 인벤을 캐쉬탭으로 바꿔준다.
		CDnInvenTabDlg* pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		pInvenDlg->ShowTab( ST_INVENTORY_CASH );
	}
	else
	{
		GetItemTask().ResetSendGachaReq();
		m_bProcessRoulette = false;

		// 그냥 pMainMenu->CloseInvenDialog로 닫아버리면,
		// 마을 진입시 다이얼로그 로딩하면서 퀘스트 알리미까지 다 닫아버려서 열려있는지를 검사하고 닫도록 하겠다.
		// 인벤토리 다이얼로그의 경우 차일드이므로 Show( false ) 대신 부모를 통해 닫아야한다.
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() )
		{
			// 인벤을 원래 첫번째 탭으로 복구시킨다.
			pInvenDlg->ShowTab( ST_INVENTORY );
			GetInterface().GetMainMenuDialog()->CloseInvenDialog();
		}

		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

}


// 가챠폰에서 표시될 아이템 정보는 힘, 지능, 민첩, 건강만 보여준다.
void CDnGachaDlg::_MakeItemStatString( CDnState* pItemState, wstring& strItemStat )
{
#ifdef PRE_MOD_GACHA_SYSTEM
	if (pItemState)
		CommonUtil::GetItemStatString_Basic(strItemStat, pItemState);
#else
	WCHAR awcBuffer[ 256 ] = { 0 };

	if( pItemState->GetAttackPMin() > 0 || pItemState->GetAttackPMax() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d~%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pItemState->GetAttackPMin(), pItemState->GetAttackPMax() );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetAttackMMin() > 0 || pItemState->GetAttackMMax() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d~%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pItemState->GetAttackMMin(), pItemState->GetAttackMMax() );
		strItemStat.append( awcBuffer );
	}

	if( pItemState->GetDefenseP() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pItemState->GetDefenseP() );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetDefenseM() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pItemState->GetDefenseM() );
		strItemStat.append( awcBuffer );
	}

	if( pItemState->GetSpirit() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pItemState->GetSpirit() );
		strItemStat.append( awcBuffer );
	}

	int nTemp(0);
	INT64 nTempHP = 0;

	// 내부 스탯 증가
	nTemp = pItemState->GetStiff();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetStiffResistance();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetStun();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetStunResistance();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetCritical();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetCriticalResistance();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetMoveSpeed();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%d ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTempHP = pItemState->GetMaxHP();
	if( nTempHP > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s +%I64d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), nTempHP );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetMaxSP();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s +%d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetRecoverySP();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), nTemp );
		strItemStat.append( awcBuffer );
	}

	// 슈퍼아머, 파이널데미지
	nTemp = pItemState->GetSuperAmmor();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), nTemp );
		strItemStat.append( awcBuffer );
	}

	nTemp = pItemState->GetFinalDamage();
	if( nTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %d ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), nTemp );
		strItemStat.append( awcBuffer );
	}

	// 아이템 능력치 표시 두번째. Ratio관련.
	// 공격력/방어력
	if( pItemState->GetAttackPMinRatio() > 0 || pItemState->GetAttackPMaxRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.1f%%~%.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5018 ), pItemState->GetAttackPMinRatio() * 100.0f, pItemState->GetAttackPMaxRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetAttackMMinRatio() > 0 || pItemState->GetAttackMMaxRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.1f%%~%.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5019 ), pItemState->GetAttackMMinRatio() * 100.0f, pItemState->GetAttackMMaxRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}

	if( pItemState->GetDefensePRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5020 ), pItemState->GetDefensePRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetDefenseMRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.1f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5062 ), pItemState->GetDefenseMRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}

	// 기본 스택 정보(랜덤 최고치의 능력치)
	if( pItemState->GetStrengthRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STRENGTH ), pItemState->GetStrengthRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetAgilityRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_AGILITY ), pItemState->GetAgilityRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetIntelligenceRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_INTELLIGENCE ), pItemState->GetIntelligenceRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetStaminaRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STAMINA ), pItemState->GetStaminaRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}
	if( pItemState->GetSpiritRatio() > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_SPIRIT ), pItemState->GetSpiritRatio() * 100.0f );
		strItemStat.append( awcBuffer );
	}

	float fTemp(0);

	// 내부 스탯 증가
	fTemp = pItemState->GetStiffRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_P ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetStiffResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STIFFNESS_R ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetStunRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_P ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetStunResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_STUN_R ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetCriticalRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_P ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetCriticalResistanceRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_CRITICAL_R ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}


	fTemp = pItemState->GetMoveSpeedRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_MOVE ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetMaxHPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s +%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2254 ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetMaxSPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s +%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2255 ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetRecoverySPRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s +%.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2256 ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	// 슈퍼아머, 파이널데미지
	fTemp = pItemState->GetSuperAmmorRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2279 ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}

	fTemp = pItemState->GetFinalDamageRatio();
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : %.2f%% ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2280 ), fTemp * 100.0f );
		strItemStat.append( awcBuffer );
	}



	// 속성 공격력/저항 만 별도의 Ratio가 없으므로 아래 출력한다.
	//
	fTemp = ( pItemState->GetElementAttack( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_A ), fTemp );
		strItemStat.append( awcBuffer );
	}

	fTemp = ( pItemState->GetElementAttack( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_A ), fTemp );
		strItemStat.append( awcBuffer );
	}

	fTemp = ( pItemState->GetElementAttack( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_A ), fTemp );
		strItemStat.append( awcBuffer );
	}

	fTemp = ( pItemState->GetElementAttack( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_A ), fTemp );
		strItemStat.append( awcBuffer );
	}


	fTemp = ( pItemState->GetElementDefense( CDnState::Light ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_LIGHT_R ), fTemp );
		strItemStat.append( awcBuffer );
	}

	fTemp = ( pItemState->GetElementDefense( CDnState::Dark ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_DARK_R ), fTemp );
		strItemStat.append( awcBuffer );
	}

	fTemp = ( pItemState->GetElementDefense( CDnState::Fire ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_FIRE_R ), fTemp );
		strItemStat.append( awcBuffer );
	}

	fTemp = ( pItemState->GetElementDefense( CDnState::Ice ) * 100.f );
	if( fTemp > 0 )
	{
		swprintf_s( awcBuffer, 256, L"%s : +%.2f%% ", ITEM::GET_STATIC( emTOOLTIP_STATIC::ITEM_ICE_R ), fTemp );
		strItemStat.append( awcBuffer );
	}
#endif // PRE_MOD_GACHA_SYSTEM
}



void CDnGachaDlg::_UpdateRouletteItemNameInfo( void )
{
	// TODO: 아이템 룰렛 시작점은 랜덤일 필요가 있겠다....
	DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );
	_ASSERT( NULL != pItemTable );

	int iJobClassID = m_iSelectedJobBtnID + 1;
	for( int i = 0; i < NUM_GACHA_ROULETTE_ITEM; ++i )
	{
		int iPartItemID = 0;
		int iNowRouletteStaticIndex = m_iRouletteCount + (NUM_GACHA_ROULETTE_ITEM-i-1);

		int iNumGachaItems = (int)m_mapItemsByJob[ iJobClassID ].avlItemsByPart[ m_iSelectedPart ].size();
		if( 0 == iNumGachaItems )
			continue;

		//TCHAR atcBuffer[ 256 ] = { 0 };
		if( m_pResultItem && (STOPPING == m_eGachaStatus || END == m_eGachaStatus) &&
			m_iResultItemStaticIndex == iNowRouletteStaticIndex )
		{
			// 결과물로 온 아이템을 셋팅해준다.
			iPartItemID = m_pResultItem->GetClassID();
			//int iNameID = pItemTable->GetFieldFromLablePtr( iPartItemID, "_NameID" )->GetInteger();
			//_stprintf_s( atcBuffer, _T("%d (결과물) %s"), iNowRouletteStaticIndex, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNameID ) );
		}
		else
		{
			iPartItemID = m_vlMixedSelectedPartsItems.at( iNowRouletteStaticIndex%iNumGachaItems );
			//iPartItemID = m_mapItemsByJob[ iJobClassID ].avlItemsByPart[ m_iSelectedPart ].at( iNowRouletteStaticIndex%iNumGachaItems );
			//int iNameID = pItemTable->GetFieldFromLablePtr( iPartItemID, "_NameID" )->GetInteger();
			//_stprintf_s( atcBuffer, _T("%d %s"), iNowRouletteStaticIndex, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNameID ) );
		}

		//_ASSERT( pItemTable->IsExistItem( iPartItemID ) );
		//if( 0 == iPartItemID )
		//	continue;

		// 아이템 이름.
		int iNameID = pItemTable->GetFieldFromLablePtr( iPartItemID, "_NameID" )->GetInteger();
		//m_pNameRouletteDlg->SetStaticText( i, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNameID ) );

		// 아이템 등급.
		int iItemRank = pItemTable->GetFieldFromLablePtr( iPartItemID, "_Rank" )->GetInteger();
		m_pNameRouletteDlg->SetItemText( i, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNameID ),
											GetEtUIXML().GetUIString( CEtUIXML::idCategory1, ITEM::RANK_2_INDEX((eItemRank)iItemRank) ),
											ITEM::RANK_2_COLOR( (eItemRank)iItemRank ) );

		// 가운데 인덱스의 아이템을 기준으로 프리뷰의 코스튬 변경.
		if( END != m_eGachaStatus && NUM_GACHA_ROULETTE_ITEM/2 == i )
			m_pGachaPreviewDlg->AttachPart( iPartItemID );
	}
}



void CDnGachaDlg::_UpdateRouletteItemStatInfo( void )
{
	DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );
	_ASSERT( NULL != pItemTable );

	int iJobClassID = m_iSelectedJobBtnID + 1;
	for( int i = 0; i < NUM_GACHA_ROULETTE_ITEM; ++i )
	{
		int iNowRouletteStaticIndex = m_iRouletteCount + (NUM_GACHA_ROULETTE_ITEM-i-1);
		int iNumGachaItems = (int)m_mapItemsByJob[ iJobClassID ].avlItemsByPart[ m_iSelectedPart ].size();
		if( 0 == iNumGachaItems )
			continue;

		int iPartItemID = 0;
		if( m_pResultItem &&(STOPPING == m_eGachaStatus || END == m_eGachaStatus) &&
			m_iResultItemStaticIndex == iNowRouletteStaticIndex )
		{
			iPartItemID = m_pResultItem->GetClassID();
		}
		else
		{
			//iPartItemID = m_mapItemsByJob[ iJobClassID ].avlItemsByPart[ m_iSelectedPart ].at( iNowRouletteStaticIndex%iNumGachaItems );	
			iPartItemID = m_vlMixedSelectedPartsItems.at( iNowRouletteStaticIndex%iNumGachaItems );
		}
		
		_ASSERT( pItemTable->IsExistItem( iPartItemID ) );
		if( 0 == iPartItemID )
			continue;

		if( (int)m_dqItemBuffer.size() < NUM_GACHA_ROULETTE_ITEM )
		{
			SUICoord RouletteDlgCoord;
			m_pNameRouletteDlg->GetDlgCoord( RouletteDlgCoord );
			float fSelHeight = RouletteDlgCoord.fHeight / float(NUM_GACHA_ROULETTE_ITEM);	// 잡아놓은 다이얼로그의 높이를 4등분해서 사용할 것이다. 실제 보이는 것은 3칸.

			if( (STOPPING == m_eGachaStatus || END == m_eGachaStatus) &&
				m_iResultItemStaticIndex == iNowRouletteStaticIndex )
			{
				m_dqItemBuffer.push_back( m_pResultItem );
			}
			else
			{
				CDnItem* pNewGachaItem = new CDnItem;
				pNewGachaItem->InitializeForInformation( iPartItemID, 1, 1 );
				m_dqItemBuffer.push_back( pNewGachaItem );
			}
		}
		else
		if( iPartItemID != m_dqItemBuffer.at( i )->GetClassID() )
		{
			SAFE_DELETE( m_dqItemBuffer.at( i ) );

			CDnItem* pNewGachaItem = new CDnItem;
			pNewGachaItem->InitializeForInformation( iPartItemID, 1, 1 );
			m_dqItemBuffer.at( i ) = pNewGachaItem;
		}

		CDnItem* pGachaItem = m_dqItemBuffer.at( i );
	
		// 아이템 능력치 문자열을 만들어준다.
		wstring strItemStat;
		_MakeItemStatString( pGachaItem, strItemStat );
		//m_pStatRouletteDlg->SetItemText( i, strItemStat.c_str() );
		m_pStatRouletteDlg->SetStatText( i, strItemStat.c_str(), ITEM::RANK_2_COLOR((eItemRank)pGachaItem->GetItemRank()) );
	}
}



void CDnGachaDlg::Process( float fDelta )
{
	CEtUIDialog::Process( fDelta );

	// SELF 로 생성했기 때문에 따로 Process 를 호출해줘야 한다.
	if( m_pNameRouletteDlg )
		m_pNameRouletteDlg->Process( fDelta );

	if( m_pStatRouletteDlg )
		m_pStatRouletteDlg->Process( fDelta );

	// 룰렛 이동을 시켜준다. 일단 아이템 이름과 스탯이 같이 돌아가도록 하자.
	if( m_bProcessRoulette )
	{
		SUICoord RouletteAreaCoord;
		m_pStaticItemNameArea->GetUICoord( RouletteAreaCoord );
		float fYBoderPos = RouletteAreaCoord.fY + GetYCoord();

		SUICoord RouletteDlgCoord;
		m_pNameRouletteDlg->GetDlgCoord( RouletteDlgCoord );
		float fSelHeight = RouletteDlgCoord.fHeight / float(NUM_GACHA_ROULETTE_ITEM);	// 잡아놓은 다이얼로그의 높이를 4등분해서 사용할 것이다. 실제 보이는 것은 3칸.
	
		// 가속도 운동... 하지만 가속도로 계산되는 부분은 경과 시간이 멈추기 시작한 이후의 시간이므로 감안한다.
		float fMoveDistance = fSelHeight*m_fRouletteElapsedTime*m_fRouletteSpeed;

		if( STOPPING == m_eGachaStatus )
		{
			float fStopElapsedTime = m_fRouletteElapsedTime - m_fStopTimeStamp;
			if( m_fRouletteStopTimeGap < fStopElapsedTime )
				fStopElapsedTime = m_fRouletteStopTimeGap;

			fMoveDistance += 0.5f*m_fStopAccel*fStopElapsedTime*fStopElapsedTime*fSelHeight;
		}
		else
		if( END == m_eGachaStatus )
		{
			fMoveDistance += 0.5f*m_fStopAccel*m_fRouletteStopTimeGap*m_fRouletteStopTimeGap*fSelHeight;
		}

		m_iRouletteCount = int(fMoveDistance / fSelHeight);

		float fResultYPos = RouletteAreaCoord.fY+GetYCoord() + fMoveDistance-(fSelHeight*(m_iRouletteCount+1));
		if( m_iRouletteCount != m_iPrevRouletteCount )
		{
			// 룰렛 다이얼로그의 아이템 문자열을 갱신.
			_UpdateRouletteItemNameInfo();

			// 룰렛 지나가는 사운드 출력
			CEtSoundEngine::GetInstance().PlaySound__( "2D", m_iRouletteChangeSound );

			// 멈출 거리를 칸수에 맞춰 계산해 놓았으므로 칸이 바뀌는 현재 위치에서부터 감속이 일어나도록 함
			if( ROULETTING == m_eGachaStatus )
			{
				if( m_pResultItem )
				{
					m_eGachaStatus = STOPPING;
					m_fStopTimeStamp = m_fRouletteElapsedTime;
					int iNowRouletteStaticIndex = m_iRouletteCount + (NUM_GACHA_ROULETTE_ITEM-(NUM_GACHA_ROULETTE_ITEM/2)-1);
					m_iResultItemStaticIndex = iNowRouletteStaticIndex + GACHA_ROULETTE_STOP_SEL;

					// 움직일 거리로 감소할 등가속도 값을 구한다.
					SUICoord RouletteDlgCoord;
					m_pNameRouletteDlg->GetDlgCoord( RouletteDlgCoord );
					float fSelHeight = RouletteDlgCoord.fHeight / float(NUM_GACHA_ROULETTE_ITEM);
					float fNowRouletteSelPos = fMoveDistance - (fSelHeight*m_iRouletteCount);
					float fDestRouletteSelCount = (float)GACHA_ROULETTE_STOP_SEL - (fNowRouletteSelPos/fSelHeight);

					// 간단하게 속도는 시간당 이동하는 룰렛 칸 수로 감안해서 처리.
					m_fStopAccel = (0.0f - GACHA_ROULETTE_SPEED*GACHA_ROULETTE_SPEED) / (2.0f*fDestRouletteSelCount);
					m_fRouletteStopTimeGap = (0.0f - GACHA_ROULETTE_SPEED) / m_fStopAccel;
				}
			}
		}

		m_pNameRouletteDlg->SetPosition( RouletteDlgCoord.fX, fResultYPos );
		
		// 스탯 항목 돌려줌. 이미 문자열 업데이트는 되었고 여기선 다이얼로그 위치만 조절.
		m_pStaticItemStatArea->GetUICoord( RouletteAreaCoord );
		fYBoderPos = RouletteAreaCoord.fY + GetYCoord();
		m_pStatRouletteDlg->GetDlgCoord( RouletteDlgCoord );
		fResultYPos = RouletteAreaCoord.fY+GetYCoord() + fMoveDistance-(fSelHeight*(m_iRouletteCount+1));
		if( m_iRouletteCount != m_iPrevRouletteCount )
		{
			// 룰렛 다이얼로그의 아이템 스탯 정보 문자열을 갱신.
			_UpdateRouletteItemStatInfo();
		}

		m_pStatRouletteDlg->SetPosition( RouletteDlgCoord.fX, fResultYPos );

		// 룰렛 멈추는 시간이 다 되었다면 더 이상 시간을 누적시키지 않음.
		switch( m_eGachaStatus )
		{
			case ROULETTING:
				m_fRouletteElapsedTime += fDelta;
				break;

			case STOPPING:
				if( m_fRouletteElapsedTime - m_fStopTimeStamp < m_fRouletteStopTimeGap )
					m_fRouletteElapsedTime += fDelta;
				else
				{
					m_fRouletteElapsedTime = m_fStopTimeStamp + m_fRouletteStopTimeGap;

					// 룰렛이 정지됨.
					GetItemTask().OnStopGachaRoulette();

					// 결과로 나온 아이템으로 코스튬 변경해준다.
					m_pGachaPreviewDlg->AttachPart( m_pResultItem->GetClassID() );

					// 결과 사운드 출력.
					CEtSoundEngine::GetInstance().PlaySound__( "2D", m_iRouletteResultSound );

					// 가챠폰 구입버튼 활성화
					m_pBtnBuyGacha->Enable( true );

					// 파츠 선택 콤보 박스 다시 활성화.
					m_pComboPartsSelect->Enable( true );
	
					// 캐시샵 오픈 버튼 다시 활성화.
					m_pBtnOpenCashShop->Enable( true );

					m_pBtnClose->Enable( true );

					CDnInvenTabDlg* pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
					CEtUIButton* pInvenCloseBtn = pInvenDlg->GetControl<CEtUIButton>( "ID_CLOSE_DIALOG" );
					pInvenCloseBtn->Enable( true );

					CDnNpcTalkReturnDlg* pNpcTalkReturnDlg = GetInterface().GetNpcTalkReturnDlg();
					CEtUIButton* pReturnBtn = pNpcTalkReturnDlg->GetControl<CEtUIButton>( "ID_BUTTON_RETURN" );
					pReturnBtn->Enable( true );

					// 직업 선택 라디오 버튼 다시 활성화.
					for( int i = 0; i < GACHA_NUM_JOB_BTN; ++i )
						m_apRadioBtnJob[ i ]->Enable( true );

					// 받은 아이템 다이얼로그 띄워줌.
					// 실제 아이템 객체를 전달해야 한다. 정보만을 위해 생성했던 건 안됨.
					CDnItem* pGachaResultItem = GetItemTask().GetCashInventory().FindItemFromSerialID( m_pResultItem->GetSerialID() );
					m_pGachaResultDlg->SetItem( pGachaResultItem );
					m_pGachaResultDlg->Show( true );
					focus::ReleaseControl();

					// NPC 의 결과 액션 실행.
					DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
					DnActorHandle hNpc = CDnActor::FindActorFromUniqueID( nUID );
					if( hNpc )
					{
						hNpc->SetActionQueue( "Puchase_End" );
					}

					m_eGachaStatus = END;
				}
				break;

			case END:
				m_fRouletteElapsedTime = m_fStopTimeStamp + m_fRouletteStopTimeGap;
				break;
		}

		m_iPrevRouletteCount = m_iRouletteCount;
	}
}



void CDnGachaDlg::_RenderRouletteDialog( float fDelta, CEtUIControl* pAreaControl, CDnGachaRouletteDlg* pRouletteDlg )
{
	RECT OriginalRect;
	RECT RouletteRect;
	GetEtDevice()->GetScissorRect( &OriginalRect );

	SUICoord RouletteAreaCoord;
	pAreaControl->GetUICoord( RouletteAreaCoord );

	RouletteRect.left = int((RouletteAreaCoord.fX+GetXCoord()) / GetScreenWidthRatio() * GetEtDevice()->Width());
	RouletteRect.top = int((RouletteAreaCoord.fY+GetYCoord()) / GetScreenHeightRatio() * GetEtDevice()->Height());
	RouletteRect.right = RouletteRect.left + int(RouletteAreaCoord.fWidth / GetScreenWidthRatio() * GetEtDevice()->Width() );
	RouletteRect.bottom = RouletteRect.top + int(RouletteAreaCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height() );

	CEtSprite::GetInstance().Flush();
	GetEtDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
	GetEtDevice()->SetScissorRect( &RouletteRect );

	pRouletteDlg->Show( true );
	pRouletteDlg->Render( fDelta );
	pRouletteDlg->Show( false );

	CEtSprite::GetInstance().Flush();
	GetEtDevice()->SetScissorRect( &OriginalRect );
	GetEtDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
}


void CDnGachaDlg::Render( float fDelta )
{
	if( !IsAllowRender() ) return;

	// 룰렛 다이얼로그가 자식이라 콤보랑 겹칠 경우 어색하게 나오는데, 그렇다고 콤보박스 하나만 따로 자식으로 또 빼기는 뭐해서 이렇게 처리합니다.
	int nIndex;
	CEtUIControl *pComboBox = NULL;
	int nVecCtlSize = (int)m_vecControl.size();
	for( nIndex = 0; nIndex < nVecCtlSize; nIndex++ )
	{
		if( !m_vecControl[nIndex] ) continue;
		if( m_vecControl[nIndex]->GetType() == UI_CONTROL_COMBOBOX )
		{
			pComboBox = m_vecControl[nIndex];
			m_vecControl[nIndex] = NULL;
			break;
		}
	}

	CEtUIDialog::Render( fDelta );

	if( IsShow() )
	{
		_RenderRouletteDialog( fDelta, m_pStaticItemNameArea, m_pNameRouletteDlg );
		_RenderRouletteDialog( fDelta, m_pStaticItemStatArea, m_pStatRouletteDlg );
	}

	if( pComboBox )
	{
		pComboBox->Render( fDelta );
		m_vecControl[nIndex] = pComboBox;
	}
}



void CDnGachaDlg::OnChangeResolution() 
{
	CEtUIDialog::OnChangeResolution();

	if( m_pNameRouletteDlg )
		m_pNameRouletteDlg->OnChangeResolution();

	if( m_pStatRouletteDlg )
		m_pStatRouletteDlg->OnChangeResolution();
}



void CDnGachaDlg::_OnSelectPart( void )
{
	if( -1 != m_iSelectedPart )
	{
		// 구입버튼 활성화
		//m_pBtnBuyGacha->Enable( true );

		// 돌아갈 파츠 아이템들을 복사한 후 랜덤으로 섞어준다.
		m_vlMixedSelectedPartsItems = m_mapItemsByJob[ m_iSelectedJobBtnID+1 ].avlItemsByPart[ m_iSelectedPart ];
		random_shuffle( m_vlMixedSelectedPartsItems.begin(), m_vlMixedSelectedPartsItems.end() );

		// 룰렛 내용 한번 채워줌.
		_UpdateRouletteItemNameInfo();
		_UpdateRouletteItemStatInfo();
	}
	else
	{
		m_bProcessRoulette = false;
		m_eGachaStatus = WAITING;

		// 구입 버튼 비활성화.
		//m_pBtnBuyGacha->Enable( false );
	}

	if( m_bProcessRoulette )
	{
		// 룰렛 다이얼로그 원래 위치로 돌려놓음
		if( m_pNameRouletteDlg )
		{
			SUICoord RouletteAreaCoord;
			m_pStaticItemNameArea->GetUICoord( RouletteAreaCoord );
			m_pNameRouletteDlg->SetPosition( RouletteAreaCoord.fX+GetXCoord(), RouletteAreaCoord.fY+GetYCoord() );
		}

		if( m_pStatRouletteDlg )
		{
			SUICoord RouletteAreaCoord;
			m_pStaticItemStatArea->GetUICoord( RouletteAreaCoord );
			m_pStatRouletteDlg->SetPosition( RouletteAreaCoord.fX+GetXCoord(), RouletteAreaCoord.fY+GetYCoord() );
		}

		// 아이템 능력치를 보여주기 위해 랜덤시드 셋팅.
		srand( timeGetTime() );
	}
}



void CDnGachaDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ )
{
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	SetCmdControlName( pControl->GetControlName() );

	if( false == m_bInitialized )
		return;

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_BUTTON_CLOSE" ) )
		{
			Show( false );
		}
		else
		if( IsCmdControl( "ID_BUTTON_BUYCOIN" ) )
		{
			if( CDnCashShopTask::IsActive() )
			{
				Show( false );
				static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer())->LockInput( false );
				CDnCashShopTask::GetInstance().RequestCashShopOpen();
			}
		}
		else
		if( IsCmdControl( "ID_BUTTON_BUYGACHA" ) )
		{
			// 파츠 선택 안되어있으면 선택하라 메시지 띄움.
			if( -1 == m_iSelectedPart )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4547 ) );
			}
			else
			// 코인이 없는가.
			if( 0 == GetItemTask().GetCashInventory().GetItemCount( JP_GACHACOIN_ITEM_ID ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4548 ) );
			}
			else
			//// 캐시 인벤이 꽉찼는가.
			//if( INVENTORYMAX <= GetItemTask().GetCashInventory().GetInventoryItemList().size() )
			//{
			//	GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4549 ) );
			//}
			//else
			{
				if( m_pResultItem )
					SAFE_DELETE( m_pResultItem );

				// #25546 대만에서 가챠 버튼 눌렀을 때 한번 더 섞어달라고 해서. ///////////////////////////////////////
				random_shuffle( m_vlMixedSelectedPartsItems.begin(), m_vlMixedSelectedPartsItems.end() );

				// 룰렛 내용 한번 채워줌.
				_UpdateRouletteItemNameInfo();
				_UpdateRouletteItemStatInfo();
				////////////////////////////////////////////////////////////////////////////////////////////////

				// 가챠폰 룰렛이 돌아가기 시작함.
				m_bProcessRoulette = true;
				m_eGachaStatus = ROULETTING;

				// 서버로부터 결과가 올 때 까지 선택 불가.
				if( -1 != m_iSelectedPart )
				{
					pControl->Enable( false );

					for( int i = 0; i < GACHA_NUM_JOB_BTN; ++i )
						m_apRadioBtnJob[ i ]->Enable( false );
				}

				m_pComboPartsSelect->Enable( false );
				m_pBtnOpenCashShop->Enable( false );

				m_pGachaBuyConfirm->Show( true );

				// 닫기 버튼 비활성화
				m_pBtnClose->Enable( false );

				// 같이 떠 있는 인벤 다이얼로그의 닫기 버튼도 비활성화.
				CDnInvenTabDlg* pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
				CEtUIButton* pInvenCloseBtn = pInvenDlg->GetControl<CEtUIButton>( "ID_CLOSE_DIALOG" );
				pInvenCloseBtn->Enable( false );

				CDnNpcTalkReturnDlg* pNpcTalkReturnDlg = GetInterface().GetNpcTalkReturnDlg();
				CEtUIButton* pReturnBtn = pNpcTalkReturnDlg->GetControl<CEtUIButton>( "ID_BUTTON_RETURN" );
				pReturnBtn->Enable( false );

				// NPC 의 루핑액션 실행.
				DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
				DnActorHandle hNpc = CDnActor::FindActorFromUniqueID( nUID );
				if( hNpc )
				{
					hNpc->SetActionQueue( "Puchase_Start" );
				}
			}
		}
	}
	else
	if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( strstr( pControl->GetControlName(), "ID_TAB_JOB" ) )
		{
			int iButtonID = static_cast<CEtUIRadioButton*>( pControl )->GetButtonID();
			if( iButtonID != m_iSelectedJobBtnID )
			{
				if( m_pResultItem )
					SAFE_DELETE( m_pResultItem );

				m_iSelectedJobBtnID = iButtonID;
				_UpdateJobSelection();
			}
		}
	}
	else
	if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( strstr( pControl->GetControlName(), "ID_COMBOBOX_PARTS" ) )
		{
			Reset();

			// 파츠를 선택했다면 해당 파츠로 루프를 돌림. -1 을 해주면 인덱스가 맞는다.
			int iSelectedPart = -1;
			static_cast<CEtUIComboBox*>( pControl )->GetSelectedValue( iSelectedPart );
			m_pGachaPreviewDlg->SelectedPart( iSelectedPart );

			m_iSelectedPart = iSelectedPart;
			_OnSelectPart();
		}
	}

	//if( EVENT_BUTTON_CLICKED == nCommand )
	//{
	//	if( strstr( pControl->GetControlName(), "ID_BUTTON_CLOSE" ) )
	//	{
	//		int iButtonID = static_cast<CEtButto*>( pControl )->GetButtonID();
	//		m_iSelectedJobBtnID = iButtonID;

	//		_UpdateSelectionContentDlg();

	//		if( drag::IsValid() )
	//		{
	//			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
	//			if( pDragButton->GetSlotType() == ST_SKILL )
	//				drag::ReleaseControl();
	//		}
	//	}
	//}

}



void CDnGachaDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID )
	{
		case GACHA_BUY_CONFIRM_DLG:
			if( EVENT_BUTTON_CLICKED == nCommand )
			{
				// CDnGachaBuyConfirmDlg
				if( IsCmdControl( "ID_BUTTON_OK" ) )
				{
					// 구매! 서버로 구매했다고 패킷 보냄. 현재 선택된 파츠만 보내준다.
					// 구매버튼은 구매할 파츠를 선택해서 가챠가 돌아가고 있을 때만 enable 된다.
					GetItemTask().RequestGacha_JP( m_iSelectedJobBtnID+1, m_iSelectedPart );
					m_pGachaBuyConfirm->Show( false );
				}
				else
				if( IsCmdControl( "ID_BUTTON_CANCEL" ) ) 
				{
					//m_pComboPartsSelect->SetSelectedByIndex( 0 );
					Reset();
					m_pComboPartsSelect->Enable( true );
					m_pGachaBuyConfirm->Show( false );
					m_pBtnBuyGacha->Enable( true );
					m_pBtnOpenCashShop->Enable( true );
					m_pBtnClose->Enable( true );

					CDnInvenTabDlg* pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
					CEtUIButton* pInvenCloseBtn = pInvenDlg->GetControl<CEtUIButton>( "ID_CLOSE_DIALOG" );
					pInvenCloseBtn->Enable( true );

					CDnNpcTalkReturnDlg* pNpcTalkReturnDlg = GetInterface().GetNpcTalkReturnDlg();
					CEtUIButton* pReturnBtn = pNpcTalkReturnDlg->GetControl<CEtUIButton>( "ID_BUTTON_RETURN" );
					pReturnBtn->Enable( true );

					for( int i = 0; i < GACHA_NUM_JOB_BTN; ++i )
						m_apRadioBtnJob[ i ]->Enable( true );

					// NPC 의 기본 액션으로 돌아옴.
					DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
					DnActorHandle hNpc = CDnActor::FindActorFromUniqueID( nUID );
					if( hNpc )
					{
						hNpc->SetActionQueue( "Stand" );
					}
				}
			}
			break;

		case GACHA_RESULT_DLG:
			if( EVENT_BUTTON_CLICKED == nCommand )
			{
				// CDnGachaResultDlg
				if( IsCmdControl( "ID_OK" ) )
				{
					m_pGachaResultDlg->Show( false );
				}
			}
			break;
	}
}


void CDnGachaDlg::_UpdateJobSelection( void )
{
	m_pGachaPreviewDlg->UpdateJobSelection( m_iSelectedJobBtnID+1 );

	// 직업에 따른 아이템들 모아서 랜덤으로 섞어둠.
	if( -1 != m_iSelectedPart )
	{
		m_vlMixedSelectedPartsItems = m_mapItemsByJob[ m_iSelectedJobBtnID+1 ].avlItemsByPart[ m_iSelectedPart ];
		random_shuffle( m_vlMixedSelectedPartsItems.begin(), m_vlMixedSelectedPartsItems.end() );

		// 룰렛 내용 한번 채워줌.
		_UpdateRouletteItemNameInfo();
		_UpdateRouletteItemStatInfo();
	}
}


void CDnGachaDlg::UpdateGachaCoin( void )
{
	// 일본 가챠코인 아이템ID 는 1107345408 이다.
	// 추후 다른 국가들은 변동될 수 있으므로 이 부분은 국가별로 디파인을 나누던가 해야함..
	int iNumOwnedGachaCoin = GetItemTask().GetCashInventory().GetItemCount( JP_GACHACOIN_ITEM_ID );

	m_pStaticCoin->SetIntToText( iNumOwnedGachaCoin );

	//// 가챠 코인이 한개도 없으면 구매할 수 없다.
	//if( 0 < iNumOwnedGachaCoin )
	//	m_pBtnBuyGacha->Enable( false );
}


void CDnGachaDlg::OnChangeItem( int iItemID )
{
	//if( 0 == iItemID )
	//	return;

	//DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );

	//// 우선 아이템 이름만..
	//int iNameID = pItemTable->GetFieldFromLablePtr( iItemID, "_NameID" )->GetInteger();
	//m_pLBItemName->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iNameID ), NULL, 0 );
}


void CDnGachaDlg::OnRecvGachaResult( const SCRefreshCashInven* pPacket )
{
	// 해당 아이템의 이름과 스탯으로 멈추도록 셋팅. 
	// 메모리 해제의 책임은 이 다이얼로그 클래스에서 진다.
	m_pResultItem = new CDnItem;
	if( pPacket->nCount == 1 )
	{
		// 이 타이밍에 들어오는 리프레쉬 캐시인벤 개수는 1개일 것이다.
		m_pResultItem->InitializeForInformation( pPacket->ItemList[0].nItemID, 
												 pPacket->ItemList[0].nRandomSeed, 
												 pPacket->ItemList[0].cOption );
		m_pResultItem->SetSerialID( pPacket->ItemList[0].nSerial );
	}

	// 코인 갯수 업데이트
	UpdateGachaCoin();

	// 룰렛이 정지되는 동안엔 구입 버튼 비활성화.
	m_pBtnBuyGacha->Enable( false );

	//// 움직일 거리로 감소할 등가속도 값을 구한다.
	//SUICoord RouletteDlgCoord;
	//m_pNameRouletteDlg->GetDlgCoord( RouletteDlgCoord );
	//float fSelHeight = RouletteDlgCoord.fHeight / float(NUM_GACHA_ROULETTE_ITEM);	// 잡아놓은 다이얼로그의 높이를 4등분해서 사용할 것이다. 실제 보이는 것은 3칸.

	//// 간단하게 속도는 시간당 이동하는 룰렛 칸 수로 감안해서 처리.
	//m_fStopAccel = (0.0f - GACHA_ROULETTE_SPEED*GACHA_ROULETTE_SPEED) / (2.0f*GACHA_ROULETTE_STOP_SEL);
	//m_fRouletteStopTimeGap = (0.0f - GACHA_ROULETTE_SPEED) / m_fStopAccel;

	//// 코인 갯수 업데이트
	//UpdateGachaCoin();
}
#endif // PRE_ADD_GACHA_JAPAN