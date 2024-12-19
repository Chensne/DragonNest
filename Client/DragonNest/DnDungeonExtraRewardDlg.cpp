#include "stdafx.h"
#include "DnDungeonExtraRewardDlg.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "ItemSendPacket.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )

CDnDungeonExtraRewardDlg::CDnDungeonExtraRewardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pOKButton( NULL )
, m_dwPropIndex( 0 )
, m_nSelectIndex( 0 )
{
	memset( m_pSlotCover, NULL, sizeof( m_pSlotCover ) );
	memset( m_pSlotItem, NULL, sizeof( m_pSlotItem ) );
}

CDnDungeonExtraRewardDlg::~CDnDungeonExtraRewardDlg()
{
	ResetNeedItemSlot();
}

void CDnDungeonExtraRewardDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonExtraRewardDlg.ui" ).c_str(), bShow );
}

void CDnDungeonExtraRewardDlg::InitialUpdate()
{
	char szString[256] = {0,};
	for( int itr = 0; itr < E_NEED_ITEM_COUNT; ++itr )
	{
		sprintf_s( szString, _countof(szString), "ID_STATIC_SELECT%d", itr );
		m_pSlotCover[itr] = GetControl<CEtUIStatic>( szString );

		sprintf_s( szString, _countof(szString), "ID_BT_ITEM%d", itr );
		m_pSlotItem[itr] = GetControl<CDnItemSlotButton>( szString );
	}

	m_pOKButton = GetControl<CEtUIButton>( "ID_OK" );
}

void CDnDungeonExtraRewardDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_ITEM" ) )
		{
			SelectSlot( pControl->GetControlName() );
		}
		else if( IsCmdControl("ID_OK") )
		{
			MIInventoryItem * pItem = m_pSlotItem[m_nSelectIndex]->GetItem();
			if( NULL == pItem )
				return;

			int nItemID = pItem->GetClassID();
			SendStageClearBonusRewardSelect( m_dwPropIndex, nItemID );
			Show( false );
		}
		else if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_CLOSE") )
		{
			Show( false );
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDungeonExtraRewardDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( true == bShow )
		SetNeedItemSlot();
	else
		ResetNeedItemSlot();

	CDnMouseCursor::GetInstance().ShowCursor( bShow );
	CDnLocalPlayerActor::LockInput( bShow );

	CDnCustomDlg::Show( bShow );
}

void CDnDungeonExtraRewardDlg::ResetNeedItemSlot()
{
	for( int itr = 0; itr < E_NEED_ITEM_COUNT; ++itr )
	{
		MIInventoryItem * pItem = m_pSlotItem[itr]->GetItem();
		if( NULL != pItem )
			SAFE_DELETE( pItem );
	}

	m_dwPropIndex = 0;
	m_nSelectIndex = 0;
}

void CDnDungeonExtraRewardDlg::HideSlotCover()
{
	for( int itr = 0; itr < E_NEED_ITEM_COUNT; ++itr )
		m_pSlotCover[itr]->Show( false );

	m_pOKButton->Enable( false );
}

bool CDnDungeonExtraRewardDlg::IsMyInventoryExist( int nItemID )
{
	bool bExist = true;

	if( NULL == GetItemTask().FindItem( nItemID, ITEM_SLOT_TYPE::ST_INVENTORY ) )
		bExist = false;

	if( false == bExist && NULL == GetItemTask().FindItem( nItemID, ITEM_SLOT_TYPE::ST_INVENTORY_CASH ) )
		bExist = false;

	return bExist;
}

void CDnDungeonExtraRewardDlg::SetNeedItemSlot()
{
	ResetNeedItemSlot();
	HideSlotCover();

	std::vector<int> vecNeedItemIndex;
	Initialize_NeedItemIndex( vecNeedItemIndex );
	Initialize_NeedItemSlot( vecNeedItemIndex );
}

void CDnDungeonExtraRewardDlg::Initialize_NeedItemIndex( std::vector<int> & vecNeedItemIndex )
{
	CDnGameTask* pGameTask = (CDnGameTask *) CTaskManager::GetInstance().GetTask( "GameTask" );
	if( NULL == pGameTask )
		return;

	const int nMapIndex = pGameTask->GetGameMapIndex();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TBONUSDROP );
	if( NULL == pSox )
		return;

	int nItemCount = pSox->GetItemCount();
	for( int itr = 0; itr < nItemCount; ++itr )
	{
		int nItemID = pSox->GetItemID( itr );
		int nTableMapIndex = pSox->GetFieldFromLablePtr( nItemID, "_MapID" )->GetInteger();
		if( nMapIndex != nTableMapIndex )
			continue;

		char szString[256] = {0,};
		for( int item_count = 1; item_count <= E_NEED_ITEM_COUNT; ++item_count )
		{
			sprintf_s( szString, _countof(szString), "_NeedItemID%d", item_count );
			int nNeedItemIndex = pSox->GetFieldFromLablePtr( nItemID, szString )->GetInteger();
			vecNeedItemIndex.push_back( nNeedItemIndex );
		}
		break;
	}
}

void CDnDungeonExtraRewardDlg::Initialize_NeedItemSlot( std::vector<int> & vecNeedItemIndex )
{
	if( E_NEED_ITEM_COUNT < vecNeedItemIndex.size() )
		return;

	for( DWORD itr = 0; itr < vecNeedItemIndex.size(); ++itr )
	{
		if( 0 == vecNeedItemIndex[itr] )
			continue;

		CDnItem * pItem = CDnItem::CreateItem( vecNeedItemIndex[itr], 0 );
		if( NULL == pItem )
			return;

		m_pSlotItem[itr]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

		if( true == IsMyInventoryExist( vecNeedItemIndex[itr] ) )
			m_pSlotItem[itr]->Enable( true );
		else
			m_pSlotItem[itr]->Enable( false );
	}
}

void CDnDungeonExtraRewardDlg::SelectSlot( const char * szSlotName )
{
	std::string str( szSlotName );
	std::string strNumber = str.substr( strlen("ID_BT_ITEM"), 1 );
	int idx = atoi( strNumber.c_str() );

	if( 0 > idx || E_NEED_ITEM_COUNT <= idx )
		return;

	if( NULL == m_pSlotItem[idx]->GetItem() )
		return;

	HideSlotCover();
	
	m_pSlotCover[idx]->Show( true );
	m_nSelectIndex = idx;
	
	m_pOKButton->Enable( true );
}

#endif	// #if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )