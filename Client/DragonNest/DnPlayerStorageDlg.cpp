#include "StdAfx.h"

#include "DnPlayerStorageDlg.h"
#include "DnStorageDlg.h"
#include "DnSlotButton.h"
#include "DnItemTask.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnMoneyInputDlg.h"
#include "DnStoreConfirmExDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnPlayerStorageDlg::CDnPlayerStorageDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pStoreCommonDlg(NULL)
, m_pStaticGold(NULL)
, m_pStaticSilver(NULL)
, m_pStaticBronze(NULL)
, m_pButtonMoneyIn(NULL)
, m_pButtonMoneyOut(NULL)
, m_pMoneyInputDlg(NULL)
, m_pSplitConfirmExDlg(NULL)
, m_iRemoteItemID( 0 )
{
	m_emMoneyType = MONEY_CONFIRM_IN;
}

CDnPlayerStorageDlg::~CDnPlayerStorageDlg(void)
{
	SAFE_DELETE( m_pMoneyInputDlg );
	SAFE_DELETE( m_pSplitConfirmExDlg );
	SAFE_DELETE( m_pStoreCommonDlg );
}

void CDnPlayerStorageDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StoragePrivateDlg.ui" ).c_str(), bShow );
}

void CDnPlayerStorageDlg::InitialUpdate()
{
	m_pStoreCommonDlg = new CDnStorageDlg( UI_TYPE_CHILD, this );
	m_pStoreCommonDlg->Initialize( true );
#if defined( PRE_PERIOD_INVENTORY )
	m_pStoreCommonDlg->EnablePeriodStorage( false );
#endif	// #if defined( PRE_PERIOD_INVENTORY )

	GetItemTask().GetStorageInventory().SetInventoryDialog( this );

	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");

	m_pButtonMoneyIn = GetControl<CEtUIButton>("ID_MONEY_IN");
	m_pButtonMoneyOut = GetControl<CEtUIButton>("ID_MONEY_OUT");

	m_pMoneyInputDlg = new CDnMoneyInputDlg( UI_TYPE_MODAL, NULL, MONEY_INPUT_DIALOG, this );
	m_pMoneyInputDlg->Initialize( false );

	m_pSplitConfirmExDlg = new CDnStoreConfirmExDlg( UI_TYPE_MODAL, NULL, ITEM_ATTACH_CONFIRM_DIALOG, this );
	m_pSplitConfirmExDlg->Initialize( false );
}

void CDnPlayerStorageDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_MONEY_IN" ) )
		{
			// 보관
			m_emMoneyType = MONEY_CONFIRM_IN;
			m_pMoneyInputDlg->SetInfo( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1605 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1607 ) );
			m_pMoneyInputDlg->SetMaxMoney( -1 );
			m_pMoneyInputDlg->Show( true );
			return;
		}
		else if( IsCmdControl("ID_MONEY_OUT") )
		{
			// 인출
			m_emMoneyType = MONEY_CONFIRM_OUT;
			m_pMoneyInputDlg->SetInfo( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1606 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1608 ) );
			m_pMoneyInputDlg->SetMaxMoney( GetItemTask().GetCoinStorage() );
			m_pMoneyInputDlg->Show( true );
			return;
		}
		else if( IsCmdControl("ID_BT_SORT") )
		{
#if defined( PRE_PERIOD_INVENTORY )
			if( true == m_pStoreCommonDlg->IsPeriodStorageEnable() )
				GetItemTask().RequestSortPeriodStorage();
			else
				GetItemTask().RequestSortStorage();
#else
			GetItemTask().RequestSortStorage();
#endif	// #if defined( PRE_PERIOD_INVENTORY )
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPlayerStorageDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
	//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	if( bShow )
	{
		// 원격 창고 아이템으로 오픈한 거면 돈 인출 돈 보관은 보여주지 않는다.
		bool bEnableMoneyInOut = true;
		if( 0 < m_iRemoteItemID )
		{
			bEnableMoneyInOut = false;
		}

		m_pButtonMoneyIn->Enable( bEnableMoneyInOut );
		m_pButtonMoneyOut->Enable( bEnableMoneyInOut );
	}
	else
	{
		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if( pDragButton->GetSlotType() == ST_STORAGE_PLAYER )
			{
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();
			}
		}
	}
#ifdef PRE_ADD_INVEN_EXTENSION
	// 탭다이얼로그가 아니기때문에 이렇게 강제로 호출해줘야 자식다이얼로그의 Show-Hide가 호출되면서 창고구입 버튼이 제대로 갱신된다.
	if( m_pStoreCommonDlg ) m_pStoreCommonDlg->Show( bShow );
#endif
}

void CDnPlayerStorageDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	ASSERT(!(dwItemCnt%ITEM_X)&&"CDnStoreTabDlg::SetUseItemCnt");
	if( dwItemCnt % ITEM_X )
		return;

	// Note : 캐쉬 인벤은 모든 탭이 사용가능하다.
	//		일반 인벤만 탭의 사용여부를 셋할 수 있다.
	//
	if( m_pStoreCommonDlg )
	{
		m_pStoreCommonDlg->SetUseItemCnt( dwItemCnt );
	}

}

void CDnPlayerStorageDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnPlayerStorageDlg::SetItem, pItem is NULL!");
	//CDnStorageDlg *pStorageDlg = static_cast<CDnStorageDlg*>(m_groupTabDialog.GetDialog(m_pCommonTabButton->GetTabID()));
	//pStorageDlg->SetItem( pItem );

	m_pStoreCommonDlg->SetItem( pItem );
}

void CDnPlayerStorageDlg::ResetSlot( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnPlayerStorageDlg::SetItem, pItem is NULL!");
	//CDnStorageDlg *pStorageDlg = static_cast<CDnStorageDlg*>(m_groupTabDialog.GetDialog(m_pCommonTabButton->GetTabID()));
	//pStorageDlg->ResetSlot( pItem->GetSlotIndex() );
	m_pStoreCommonDlg->ResetSlot( pItem->GetSlotIndex() );
}

bool CDnPlayerStorageDlg::IsEmptySlot()
{
	if( !m_pStoreCommonDlg )
	{
		return false;
	}

	return m_pStoreCommonDlg->IsEmptySlot();
}

int CDnPlayerStorageDlg::GetEmptySlot()
{
	if( !m_pStoreCommonDlg )
	{
		return -1;
	}

	return m_pStoreCommonDlg->GetEmptySlot();
}

void CDnPlayerStorageDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		if( !CDnItemTask::IsActive() ) return;
		INT64 nCoin = GetItemTask().GetCoinStorage();
		INT64 nGold = nCoin/10000;
		INT64 nSilver = (nCoin%10000)/100;
		INT64 nBronze = nCoin%100;
		std::wstring strString;

		m_pStaticGold->SetInt64ToText( nGold );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nGold, strString );
		m_pStaticGold->SetTooltipText( strString.c_str() );

		m_pStaticSilver->SetInt64ToText( nSilver );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nSilver, strString );
		m_pStaticSilver->SetTooltipText( strString.c_str() );

		m_pStaticBronze->SetInt64ToText( nBronze );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nBronze, strString );
		m_pStaticBronze->SetTooltipText( strString.c_str() );
	}
}

void CDnPlayerStorageDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( nID ) 
		{
		case MONEY_INPUT_DIALOG:
			{
				if( IsCmdControl("ID_BUTTON_OK") )
				{
					if( m_emMoneyType == MONEY_CONFIRM_IN )
						GetItemTask().RequestInvenToWare( m_pMoneyInputDlg->GetMoney() );
					else
						GetItemTask().RequestWareToInven( m_pMoneyInputDlg->GetMoney() );
					m_pMoneyInputDlg->Show( false );
				}
				else if( IsCmdControl("ID_BUTTON_CANCEL") )
				{
					m_pMoneyInputDlg->Show( false );
				}
			}
			break;

		case ITEM_ATTACH_CONFIRM_DIALOG:
			{
				if (IsCmdControl("ID_OK"))
				{
					CDnItem* pItem = m_pSplitConfirmExDlg->GetItem();
					if (pItem)
					{
						int nCount = m_pSplitConfirmExDlg->GetItemCount();

						if (nCount <= 0)
						{
							m_pSplitConfirmExDlg->Show(false);
							return;
						}

						CDnInterface::emSTORE_CONFIRM_TYPE confirmType = m_pSplitConfirmExDlg->GetConfirmType();
						if (confirmType == CDnInterface::STORAGE_FROM_INVENTORY)
							RequestMoveItem(MoveType_InvenToWare, pItem, nCount);
					}

					m_pSplitConfirmExDlg->Show(false);
				}
				else if (IsCmdControl("ID_CANCEL"))
				{
					m_pSplitConfirmExDlg->Show(false);
				}
			}
			break;
		}
	}
}

void CDnPlayerStorageDlg::MoveItemToStorage(CDnQuickSlotButton* pFromSlot, bool bItemSplit)
{
	if (pFromSlot == NULL)
	{
		_ASSERT(0);
		return;
	}

	MIInventoryItem* pInvenItem = pFromSlot->GetItem();
	if (pInvenItem == NULL || pInvenItem->GetType() != MIInventoryItem::Item)
	{
		_ASSERT(0);
		return;
	}

	CDnItem* pItem = static_cast<CDnItem *>(pInvenItem);
	if (pItem == NULL)
		return;

	if (pFromSlot->GetRenderCount() > 1 && bItemSplit)
	{
		m_pSplitConfirmExDlg->SetItem(pFromSlot, CDnInterface::STORAGE_FROM_INVENTORY);
		m_pSplitConfirmExDlg->Show(true);
		return;
	}

	RequestMoveItem(MoveType_InvenToWare, pItem, pItem->GetOverlapCount());
}

void CDnPlayerStorageDlg::RequestMoveItem(eItemMoveType moveType, CDnItem* pItem, int itemCount)
{
	DWORD emptySlotType = -1;

	if (moveType == MoveType_InvenToWare)		
	{
		// 이미 이 다이얼로그를 조작하는 것 자체가 tab 이 선택된 상태임.
		// 플레이어, 길드 창고 통함됨.
		emptySlotType = CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG;
		
	}
	else if (moveType == MoveType_WareToInven)	emptySlotType = CDnMainMenuDlg::INVENTORY_DIALOG;
	else
	{
		_ASSERT(0);
		return;
	}

	int destIdx = -1;

	// 인벤에서 창고로 옮길때 수량성 아이템은 적절한 슬롯을 찾아서 넘겨준다.(MoveType_WareToInven는 여기서 안쓰는 듯.)
	if (moveType == MoveType_InvenToWare)
	{
		// 인벤토리 창에서 적절한 슬롯을 찾는다.
		// 현재 슬롯이 닫혀있는지 검사하지는 않으므로
		// 차후 아이템이 들어있는채로 UnUsable이 된다면 이 루틴도 수정해야할 것이다.(현재는 인벤토리확장이 기간제가 아니라 일어날 수 없다.)
		int nInsertableSlotIndex = -1;
		std::vector<CDnItem*> vecItem;
		GetItemTask().GetStorageInventory().ScanItemFromID( pItem->GetClassID(), &vecItem );
		for( int i = 0; i < (int)vecItem.size(); ++i )
		{
			if ( ( vecItem[i]->IsSoulbBound() == pItem->IsSoulbBound() && vecItem[i]->GetSealCount() == pItem->GetSealCount() ) 
				&& (( vecItem[i]->GetOverlapCount() + itemCount ) <= pItem->GetMaxOverlapCount()) )
			{
				nInsertableSlotIndex = vecItem[i]->GetSlotIndex();
				break;
			}
		}

		if (nInsertableSlotIndex == -1)
		{
			if (!GetInterface().IsEmptySlot(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )
			{
				GetInterface().MessageBox(1609, MB_OK);
				return;
			}
			nInsertableSlotIndex = GetInterface().GetEmptySlot(emptySlotType);
		}
		destIdx = nInsertableSlotIndex;
	}
	else if (moveType == MoveType_WareToInven)
	{
		destIdx = GetInterface().GetEmptySlot(emptySlotType);
	}

	CDnItemTask::GetInstance().RequestMoveItem(moveType,
		pItem->GetSlotIndex(),
		pItem->GetSerialID(),
		destIdx,
		itemCount);
}

void CDnPlayerStorageDlg::SetRemoteItemID( int iRemoteItemID )
{
	m_iRemoteItemID = iRemoteItemID;
	if( 0 < m_iRemoteItemID )
	{
		GetInterface().GetMainMenuDialog()->SetIgnoreShowFunc( true );
		GetInterface().OpenBlind();
		CDnLocalPlayerActor::StopAllPartyPlayer();
		CDnLocalPlayerActor::LockInput( true );
	}
}

#if defined( PRE_PERIOD_INVENTORY )
void CDnPlayerStorageDlg::EnablePeriodStorage( const bool bEnable, const __time64_t tTime )
{
	m_pStoreCommonDlg->EnablePeriodStorage( bEnable, tTime );
}
#endif	//	#if defined( PRE_PERIOD_INVENTORY )