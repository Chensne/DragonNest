#include "StdAfx.h"
#include "DnDungeonNestEnterLevelDlg.h"
#include "DnItemTask.h"
#include "TaskManager.h"
#include "PartySendPacket.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnDungeonNestEnterLevelDlg::CDnDungeonNestEnterLevelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pStaticTimer( NULL )
	, m_pButtonEnter( NULL )
	, m_pButtonCancel( NULL )
	, m_pTextureDungeonArea( NULL )
	, m_pSelectName( NULL )
	, m_pRecommLevel( NULL )
	, m_pRecommCount( NULL )
	, m_fElapsedTime( 0.0f )
	, m_nCount( 0 )
	, m_nSelectedLevelIndex( 0 )
	, m_nMapIndex( 0 )
	, m_pNeedItem( NULL )
	, m_pItemSlotButton( NULL )
	, m_bEnoughNeedItem( false )
	, m_cDungeonEnterPermit( 0 )
	, m_pNeedItemCount( NULL )
	, m_pDungeonFatigueDegree( NULL )
	, m_pNeedItemStatic( NULL )
#ifdef PRE_FIX_DUNGEONENTER_CLOSE_BY_QUEST
	, m_bCloseProcessingByUser(false)
#endif
{
}

CDnDungeonNestEnterLevelDlg::~CDnDungeonNestEnterLevelDlg(void)
{
	SAFE_RELEASE_SPTR( m_hIntroArea );
	SAFE_DELETE( m_pNeedItem );
}

void CDnDungeonNestEnterLevelDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "HardMoveLevelDlg.ui" ).c_str(), bShow );
}

void CDnDungeonNestEnterLevelDlg::InitialUpdate()
{
	m_pSelectName = GetControl<CEtUIStatic>( "ID_SELECT_NAME" );
	m_pDungeonFatigueDegree = GetControl<CEtUIStatic>( "ID_TEXT_TCOUNT" );
	m_pRecommLevel = GetControl<CEtUIStatic>( "ID_RECOMM_LEVEL" );
	m_pRecommCount = GetControl<CEtUIStatic>( "ID_RECOMM_COUNT" );
	m_pStaticTimer = GetControl<CEtUIStatic>( "ID_STATIC_TIMER" );
	m_pButtonEnter = GetControl<CEtUIButton>( "ID_BUTTON_ENTER" );
	m_pButtonCancel = GetControl<CEtUIButton>( "ID_BUTTON_CANCEL" );
	m_pTextureDungeonArea = GetControl<CEtUITextureControl>( "ID_TEXTUREL_AREA" );
	m_pItemSlotButton = GetControl<CDnItemSlotButton>( "ID_ITEM" );
	m_pNeedItemStatic = GetControl<CEtUIStatic>( "ID_ITEMSLOT" );
	m_pNeedItemStatic->Show( false );
	m_pNeedItemCount = GetControl<CEtUIStatic>( "ID_STATIC_COUNT" );
 
}

void CDnDungeonNestEnterLevelDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( m_nCount > 0 )
	{
		m_fElapsedTime += fElapsedTime;

		if( m_fElapsedTime > 1.0f )
		{
			m_nCount--;
			SetTime( m_nCount );

			m_fElapsedTime -= 1.0f;
		}
	}
	else
	{
		if( IsShow() )
		{
			Show( false );
			if( m_pButtonEnter->IsEnable() )
				CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonEnter, 0 );
			else if( m_pButtonCancel->IsEnable() )
				CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
		}
	}
}

void CDnDungeonNestEnterLevelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( strcmp( pControl->GetControlName(), "ID_BUTTON_ENTER" ) == 0 || strcmp( pControl->GetControlName(), "ID_BUTTON_CANCEL" ) == 0 ) 
		{
			m_pButtonEnter->Enable( false );
			m_pButtonCancel->Enable( false );
		}

#ifdef PRE_FIX_DUNGEONENTER_CLOSE_BY_QUEST
		m_bCloseProcessingByUser = true;
#endif
	}
}

void CDnDungeonNestEnterLevelDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_nCount = 60;
		SetControlState();
		SetTime( m_nCount );
#ifdef PRE_FIX_DUNGEONENTER_CLOSE_BY_QUEST
		m_bCloseProcessingByUser = false;
#endif
	}
	else
	{
		m_nMapIndex = 0;
		m_fElapsedTime = 0.0f;
		m_vecRecommandLevel.clear();
		m_vecRecommandPartyCount.clear();

		m_pSelectName->ClearText();
		m_pDungeonFatigueDegree->ClearText();
		m_pRecommLevel->ClearText();
		m_pRecommCount->ClearText();
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
		GetInterface().ShowDungeonLimitDlg( false , 0 );
#endif
	}

	CEtUIDialog::Show( bShow );
}

bool CDnDungeonNestEnterLevelDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
			return true;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnDungeonNestEnterLevelDlg::SetTime( int nTime )
{
	wchar_t wszTimer[80] = {0};
	swprintf_s( wszTimer, _countof(wszTimer), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2312 ), nTime );
	m_pStaticTimer->SetText( wszTimer );
}

void CDnDungeonNestEnterLevelDlg::SetImage( int nMapIndex )
{
	m_nMapIndex = nMapIndex;
	SAFE_RELEASE_SPTR( m_hIntroArea );

	char szImageName[128] = {0};
	sprintf_s( szImageName, _countof(szImageName), "DungeonImage_%d.dds", nMapIndex );

	m_hIntroArea = LoadResource( CEtResourceMng::GetInstance().GetFullName( szImageName ).c_str(), RT_TEXTURE );

	// 텍스처컨트롤을 렌더링할때 Trans,Scale 행렬만들어 설정 후 ID3DXSprite사용해서 렌더링하는데,
	// 현재 UI특성상 소수점 아래가 깔끔하지 않은 float으로 설정될 수 있어서, 한픽셀씩 어긋날때가 있다.(0.5문제)
	// 버텍스버퍼를 사용하는 것도 아니라서 최종적으로 얻은 값에다가 0.5 조정도 할 수 없기때문에,
	// 현재로선 특정스케일된 상황에서 한픽셀씩 어긋나는 것을 막을 깔끔한 방법이 없는 듯 하다.
	// 개선되기 전까지 텍스처의 우측 한픽셀을 사용하지 않는 것으로 처리하겠다.(우측만 알파값처리가 되있어서 문제로 여겨진다.)
	if( m_hIntroArea )
		m_pTextureDungeonArea->SetTexture( m_hIntroArea, 0, 0, m_hIntroArea->Width()-1, m_hIntroArea->Height() );
}

void CDnDungeonNestEnterLevelDlg::SetControlState()
{
	bool bEnable(false);

	CDnPartyTask::PartyRoleEnum partyState = GetPartyTask().GetPartyRole();

	if( partyState == CDnPartyTask::LEADER || partyState == CDnPartyTask::SINGLE )
	{
		bEnable = true;
	}
	else
	{
		bEnable = false;
	}

	m_pButtonEnter->ClearTooltipText();
	m_pButtonEnter->Enable(bEnable);
	m_pButtonCancel->Enable(bEnable);

	// 우선 자신의 아이템 여부를 확인. 자신이 가지고 있지 않다면 자기관련으로 툴팁메세지를 띄워준다.
	WCHAR wszMsg[1024];
	if( m_cDungeonEnterPermit != CDnWorld::PermitEnter )
		m_pButtonEnter->Enable( false );

	if( m_cDungeonEnterPermit & CDnWorld::PermitNotEnoughItem ) 
	{
		if( m_pNeedItem ) 
		{
			std::wstring wszStr;
			if( !m_bEnoughNeedItem )
				swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2375 ), m_pNeedItem->GetName() );
			else 
				swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2376 ), m_pNeedItem->GetName() );

			// 아이템이 모자른 놈들의 목록을 보여주려고 했는데 싱크 마추는거 관련되서 시간이 없어서 일단 코드는 두고 나중에
			// 작업 합니다. ( 하게되면 위에 m_bEnoughNeedItem 코드 없어도 됩니다. ) - siva
			wszStr += wszMsg;

			for( DWORD i=0; i<m_hVecPermitActorList.size() ;i++ )
			{
				DnActorHandle hActor = m_hVecPermitActorList[i];
				if( hActor )
				{
					wszStr += L" - ";
					wszStr += hActor->GetName();
					wszStr += L"\n";
				}
			}
			m_pButtonEnter->SetTooltipText( wszStr.c_str() );
		}
	}
	else if( m_cDungeonEnterPermit & CDnWorld::PermitExceedTryCount )
	{
		m_pButtonEnter->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 821 ) );
	}
}

void CDnDungeonNestEnterLevelDlg::SetDungeonName( const wchar_t *wszDungeonName )
{
	m_pSelectName->SetText( wszDungeonName );
}

void CDnDungeonNestEnterLevelDlg::UpdateDungeonInfo( CDnDungeonEnterDlg::SDungeonGateInfo* pInfo )
{
	if( pInfo == NULL )
		return;

	SetDungeonName( pInfo->m_pButtonGate->GetDungeonName() );
	SetImage( pInfo->m_nMapIndex );

	UpdateRecommandLevel( pInfo->m_vecRecommandLevel );
	UpdateRecommandPartyCount( pInfo->m_vecRecommandPartyCount );
	UpdateNeedItem( pInfo->m_nNeedItemID, pInfo->m_nNeedItemCount, pInfo->m_cDungeonEnterPermit, &pInfo->m_hVecNeedItemActorList );


	for( int i=0; i<5; i++ )
	{
		if( pInfo->m_cCanDifficult[i] > 0 )
			m_nSelectedLevelIndex = i;
	}

	wchar_t wszTemp[128] = {0};
	if( !m_vecRecommandLevel.empty() )
	{
		int nLevel = m_vecRecommandLevel[m_nSelectedLevelIndex];
		if( nLevel > 0 )
		{
			swprintf_s( wszTemp, _countof(wszTemp), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2368 ), nLevel );
			m_pRecommLevel->SetText( wszTemp );
		}
	}

	if( m_vecRecommandPartyCount.size() > 4 )
	{
		int nPartyCount = m_vecRecommandPartyCount[4];
		if( nPartyCount > 0 )
		{
			swprintf_s( wszTemp, _countof(wszTemp), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2369 ), nPartyCount );
			m_pRecommCount->SetText( wszTemp );
		}
	}

	SetFatigueDegree( m_nMapIndex, m_nSelectedLevelIndex );

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	GetInterface().ShowDungeonLimitDlg( true , pInfo->m_nMapIndex );
#endif

	::SendSelectDungeonInfo( m_nMapIndex, m_nSelectedLevelIndex );
}

void CDnDungeonNestEnterLevelDlg::UpdateRecommandLevel( std::vector<int> &vecRecommandLevel )
{
	m_vecRecommandLevel.clear();

	for( int i=0; i<(int)vecRecommandLevel.size(); i++ )
	{
		m_vecRecommandLevel.push_back( vecRecommandLevel[i] );
	}
}

void CDnDungeonNestEnterLevelDlg::UpdateRecommandPartyCount( std::vector<int> &vecRecommandPartyCount )
{
	m_vecRecommandPartyCount.clear();

	for( int i=0; i<(int)vecRecommandPartyCount.size(); i++ )
	{
		m_vecRecommandPartyCount.push_back( vecRecommandPartyCount[i] );
	}
}

void CDnDungeonNestEnterLevelDlg::UpdateNeedItem( int nItemID, int nCount, char cPermitFlag, std::vector<DnActorHandle> *pVecList )
{
	SAFE_DELETE( m_pNeedItem );

	if( nItemID > 0 )
	{
		TItemInfo Info;
		if( CDnItem::MakeItemInfo( nItemID, 0, Info ) == false ) return;
		m_pNeedItem = GetItemTask().CreateItem( Info );

		m_pItemSlotButton->SetItem(m_pNeedItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pNeedItemStatic->Show( true );

		// 아이템이 있는지 검사 후 Enter버튼 처리
		if( GetItemTask().GetCharInventory().GetItemCount( nItemID ) >= nCount ) 
		{
			m_pItemSlotButton->SetRegist( false );
			m_bEnoughNeedItem = true;
		}
#if defined(PRE_ADD_68838)
		//찾는 아이템이 던전 클리어 후, 아이템을 사용하는 방식이면..
		else if(m_pNeedItem->GetItemType() == ITEMTYPE_STAGE_COMPLETE_USEITEM)
		{
			int nUserTicketCount = 0;
			// 아이템이 있는지 검사 후 Enter버튼 처리
			if(CDnItem::IsCashItem(nItemID))
				nUserTicketCount = GetItemTask().GetCashInventory().GetItemCount( nItemID );
			else
				nUserTicketCount = GetItemTask().GetCharInventory().GetItemCount( nItemID );
			
			if( nUserTicketCount >= nCount ) 
			{
				m_pItemSlotButton->SetRegist( false );
				m_bEnoughNeedItem = true;
			}
			else
			{
				m_pItemSlotButton->SetRegist( true );
				m_bEnoughNeedItem = false;
			}
		}
#endif  // #if defined(PRE_ADD_68838)
		else 
		{
			m_pItemSlotButton->SetRegist( true );
			m_bEnoughNeedItem = false;
		}

		// 우선 수량 모자라도 붉은색으로 바꾸거나 하는건 요청 없다.
		if( nCount > 1 ) 
		{
			WCHAR wszTemp[64];
			swprintf_s(wszTemp, _countof(wszTemp), L"X %d", nCount );
			m_pNeedItemCount->SetText(wszTemp);
		}
		else 
		{
			m_pNeedItemCount->SetText(L"");
		}
	}
	else
	{
		m_pItemSlotButton->ResetSlot();
		m_pNeedItemStatic->Show( false );
		m_pNeedItemCount->SetText(L"");
	}

	// 서버에서 보내준 데이터 저장.
	m_cDungeonEnterPermit = cPermitFlag;
	if( pVecList ) m_hVecPermitActorList = *pVecList;

	// 좀 이상하게 여기서 호출하는데, 이게 NeedItem이 나중에 추가된거라
	// 기존 구조를 최대한 바꾸지 않으려고 이렇게 호출하는거다.
	SetControlState();
}

void CDnDungeonNestEnterLevelDlg::SetSelectDungeonInfo( int nMapIndex, int nDifficult )
{
	CDnGameTask *pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstance().GetTask( "GameTask" ));
	if( !pGameTask ) return;
	int nReadyGateIndex = pGameTask->GetReadyGateIndex();
	if( !CDnWorld::IsActive() ) return;
	CDnWorld::DungeonGateStruct *pGateStruct = (CDnWorld::DungeonGateStruct *)CDnWorld::GetInstance().GetGateStruct( nReadyGateIndex );
	if( !pGateStruct ) return;

	int nRecommandLv = 0, nRecommandCount = 0, nRebirthLimit = -1;
	WCHAR *wszName = L"";

	for( DWORD i=0; i<pGateStruct->pVecMapList.size(); i++ ) 
	{
		CDnWorld::DungeonGateStruct *pChildGate = (CDnWorld::DungeonGateStruct *)pGateStruct->pVecMapList[i];
		if( pChildGate->nMapIndex == nMapIndex ) 
		{
			wszName = (WCHAR *)pChildGate->szMapName.c_str();
			nRecommandLv = pChildGate->nRecommendLevel[nDifficult];
			nRecommandCount = pChildGate->nRecommendPartyCount[4];
			nRebirthLimit = pChildGate->nMaxUsableCoin;
			break;
		}
	}
	SetImage( nMapIndex );
	m_pSelectName->SetText( wszName );

	wchar_t wszTemp[128] = {0};
	swprintf_s( wszTemp, _countof(wszTemp), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2368 ), nRecommandLv );
	m_pRecommLevel->SetText( wszTemp );

	swprintf_s( wszTemp, _countof(wszTemp), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2369 ), nRecommandCount );
	m_pRecommCount->SetText( wszTemp );

	SetFatigueDegree( nMapIndex, nDifficult );
}

void CDnDungeonNestEnterLevelDlg::SetFatigueDegree( int nMapIndex, int nDifficult )
{
	int nPartyMemberCount = GetPartyTask().GetPartyCount();
	float fFatiguePercent = CFatigueAdjustmentTable::GetInstance().GetValue( nPartyMemberCount, nDifficult );

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TSTAGEREWARD );
	if( pSox == NULL ) return;

	int nIndex = -1;
	for( int i=0; i<=pSox->GetItemCount(); i++ )
	{
		int nItemID = pSox->GetItemID( i );
		if( nMapIndex == pSox->GetFieldFromLablePtr( nItemID, "_DungeonID" )->GetInteger()
			&& nDifficult == pSox->GetFieldFromLablePtr( nItemID, "_Difficulty" )->GetInteger() )
		{
			nIndex = nItemID;
			break;
		}
	}

	if( nIndex != -1 )
	{
		int nFatigueDegree = (int)( fFatiguePercent * (float)( pSox->GetFieldFromLablePtr( nIndex, "_Fatigue" )->GetInteger() ) );
		wchar_t wszTemp[128] = {0};
		swprintf_s( wszTemp, _countof(wszTemp), L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3280 ), nFatigueDegree );
		m_pDungeonFatigueDegree->SetText( wszTemp );
	}
}

#ifdef PRE_FIX_DUNGEONENTER_CLOSE_BY_QUEST
bool CDnDungeonNestEnterLevelDlg::IsCloseProcessingByUser() const
{
	return m_bCloseProcessingByUser;
}
#endif

