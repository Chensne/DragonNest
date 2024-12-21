#include "StdAfx.h"
#include "DnInvenTabDlg.h"
#include "DnInvenDlg.h"
#include "DnSlotButton.h"
#include "DnMessageBox.h"
#include "DnTableDB.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnMainMenuDlg.h"
#include "DnQuestTask.h"
#include "DnInterfaceString.h"
#include "DnPrivateMarketDlg.h"
#include "DnTradeTask.h"
#include "DnMainMenuDlg.h"
#include "DnMarketTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnRestraintTask.h"
#include "DnInvenGestureDlg.h"
#include "DNGestureTask.h"
#include "DnAuthTask.h"
#include "DnFadeInOutDlg.h"
#include "DnInvenCashDlg.h"
#include "DnItemSealDlg.h"
#include "DnItemUnSealDlg.h"
#include "DnPotentialJewelDlg.h"
#include "DnSkillResetDlg.h"
#include "DnMainDlg.h"
#include "DnGuildRenameDlg.h"
#include "DnCharRenameDlg.h"
#include "DnEnchantJewelDlg.h"
#include "DnInvenPetDlg.h"

#ifdef PRE_ADD_CASHREMOVE
#include "DnInvenStandByCashRemoveDlg.h"
#endif

#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnLifeSkillCookingTask.h"
#include "DnLifeSkillFishingTask.h"
#endif // PRE_ADD_SECONDARY_SKILL

#include "DnSkillTask.h"
#if defined(PRE_ADD_REMOVE_PREFIX)
#include "DnRemovePrefixDlg.h"
#endif // PRE_ADD_REMOVE_PREFIX

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
#include "DnPotentialTransferDlg.h"
#endif

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
//#include "TaskManager.h"
//#include "DnBridgeTask.h"
#endif
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvenTabDlg::CDnInvenTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pCommonTabButton(NULL)
	, m_pCashTabButton(NULL)
	, m_pInvenCommonDlg(NULL)
	, m_pInvenCashDlg(NULL)
	, m_pDragButton(NULL)
	, m_pTrashButton(NULL)
	, m_pStaticGold(NULL)
	, m_pStaticSilver(NULL)
	, m_pStaticBronze(NULL)
	, m_nTrashSoundIndex(-1)
	, m_pInvenQuestDlg(NULL)
	, m_pQuestTabButton(NULL)
	, m_pTrashItemButton(NULL)
	, m_pPrivateMarketDlg(NULL)
	, m_pSortButton(NULL)
	, m_pGestureTabButton(NULL)
	, m_pInvenGestureDlg(NULL)
	, m_pPetTabButton(NULL)
	, m_pInvenPetDlg(NULL)
#ifdef PRE_ADD_CASHREMOVE
	, m_pInvenStandByCashRemoveBtn(NULL)
	, m_pInvenStandByCashRemoveDlg(NULL)
#endif
	, m_pCommonTabNewStatic( NULL )
	, m_pCashTabNewStatic( NULL )
	, m_pPetTabNewStatic( NULL )
#if defined(PRE_ADD_AUTOUNPACK) || defined(PRE_ADD_CASHREMOVE)
	, m_EnableInvenType(eInvenTabType::eCOMMON) // 현재활성화된 인벤타입.
#endif
{
}

CDnInvenTabDlg::~CDnInvenTabDlg(void)
{
	if( m_nTrashSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nTrashSoundIndex );
	}

	SAFE_DELETE( m_pPrivateMarketDlg );
}

void CDnInvenTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenTabDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10001 );
	if( strlen( szFileName ) > 0 )
	{
		m_nTrashSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}

	if( CDnQuestTask::IsActive() )
	{
		GetQuestTask().SetInvenDialog( this );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnInvenTabDlg::Initialize, 인벤토리 다이얼로그가 만들어 지기 전에 퀘스트 테스크가 생성되어야 합니다." );
	}
}

void CDnInvenTabDlg::InitialUpdate()
{
	m_pTrashButton = GetControl<CEtUIButton>("ID_BUTTON_TRASH");
	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");
	m_pSortButton = GetControl<CEtUIButton>("ID_BUTTON_SORT");

	m_pCommonTabButton = GetControl<CEtUIRadioButton>("ID_TAB_INVEN");
	m_pCommonTabButton->EnableDragMode( false );
	m_pInvenCommonDlg = new CDnInvenDlg( UI_TYPE_CHILD, this );
	m_pInvenCommonDlg->Initialize( false );
	m_pInvenCommonDlg->SetSlotType( ST_INVENTORY );
	AddTabDialog( m_pCommonTabButton, m_pInvenCommonDlg );
#if defined( PRE_PERIOD_INVENTORY )
	m_pInvenCommonDlg->EnablePeriodInven( false );
#endif	// #if defined( PRE_PERIOD_INVENTORY )

	m_pCashTabButton = GetControl<CEtUIRadioButton>("ID_TAB_CASH");
	m_pCashTabButton->EnableDragMode( true );
	m_pInvenCashDlg = new CDnInvenCashDlg( UI_TYPE_CHILD, this );
	m_pInvenCashDlg->Initialize(false);
	m_pInvenCashDlg->SetSlotType( ST_INVENTORY_CASH );
	AddTabDialog( m_pCashTabButton, m_pInvenCashDlg );

	m_pQuestTabButton = GetControl<CEtUIRadioButton>("ID_TAB_QUEST");
	m_pQuestTabButton->EnableDragMode( false );
	m_pInvenQuestDlg = new CDnInvenDlg( UI_TYPE_CHILD, this );
	m_pInvenQuestDlg->Initialize( false );
	m_pInvenQuestDlg->SetSlotType( ST_INVENTORY_QUEST );
	AddTabDialog( m_pQuestTabButton, m_pInvenQuestDlg );
#if defined( PRE_PERIOD_INVENTORY )
	m_pInvenQuestDlg->EnablePeriodInven( false );
#endif	// #if defined( PRE_PERIOD_INVENTORY )

	m_pGestureTabButton = GetControl<CEtUIRadioButton>("ID_TAB_GESTURE");
	m_pGestureTabButton->EnableDragMode( false );
	m_pInvenGestureDlg= new CDnInvenGestureDlg( UI_TYPE_CHILD, this );
	m_pInvenGestureDlg->Initialize( false );
	AddTabDialog( m_pGestureTabButton, m_pInvenGestureDlg );

	m_pPetTabButton = GetControl<CEtUIRadioButton>("ID_TAB_PET");
	m_pPetTabButton->EnableDragMode( false );
	m_pPetTabButton->Show(true);
#ifdef PRE_FIX_HIDE_VEHICLETAB
	m_pPetTabButton->Show(false);
#endif
	m_pInvenPetDlg = new CDnInvenPetDlg( UI_TYPE_CHILD, this );
	m_pInvenPetDlg->Initialize(false);
	m_pInvenPetDlg->SetSlotType(ST_INVENTORY_VEHICLE);
	AddTabDialog( m_pPetTabButton , m_pInvenPetDlg);

#ifdef PRE_ADD_CASHREMOVE
	m_pInvenStandByCashRemoveBtn = GetControl<CEtUIRadioButton>("ID_TAB_DELET");
	m_pInvenStandByCashRemoveBtn->EnableDragMode( false );
	m_pInvenStandByCashRemoveBtn->Show( true );
	m_pInvenStandByCashRemoveDlg = new CDnInvenStandByCashRemoveDlg( UI_TYPE_CHILD, this );
	m_pInvenStandByCashRemoveDlg->Initialize( false );
	m_pInvenStandByCashRemoveDlg->SetSlotType( ST_INVENTORY_CASHREMOVE );	
	AddTabDialog( m_pInvenStandByCashRemoveBtn, m_pInvenStandByCashRemoveDlg );
#else
	CEtUIRadioButton * pStatic = GetControl<CEtUIRadioButton>("ID_TAB_DELET"); // 캐시삭제 기능제거 - UI는 유지되므로 코드에서 hide 처리.
	if( pStatic )
		pStatic->Show( false );
#endif
	SetCheckedTab( m_pCommonTabButton->GetTabID() );
#ifdef PRE_ADD_AUTOUNPACK
	SetInvenTabType( m_pCommonTabButton->GetTabID() );
#endif

	GetItemTask().GetCharInventory().SetInventoryDialog( this );
	GetItemTask().GetQuestInventory().SetInventoryDialog( this );
	GetItemTask().GetCashInventory().SetInventoryDialog( this );
	GetItemTask().GetPetInventory().SetInventoryDialog( this );

#ifdef PRE_ADD_CASHREMOVE
	GetItemTask().GetCashRemoveInventory().SetInventoryDialog( this );
#endif

	m_pPrivateMarketDlg = new CDnPrivateMarketDlg( UI_TYPE_CHILD, this, PRIVATE_MARKET_DIALOG, this );
	m_pPrivateMarketDlg->Initialize( false );

	m_pSortButton->SetDisableTime( 3.0f );

	m_SmartMoveEx.SetControl( m_pCommonTabButton );

	m_pCommonTabNewStatic = GetControl<CEtUIStatic>( "ID_STATIC_NEW0" );
	m_pCommonTabNewStatic->Show( false );
	m_pCashTabNewStatic = GetControl<CEtUIStatic>( "ID_STATIC_NEW2" );
	m_pCashTabNewStatic->Show(false);
	m_pPetTabNewStatic = GetControl<CEtUIStatic>( "ID_STATIC_NEW4" );
	m_pPetTabNewStatic->Show( false );
}

void CDnInvenTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_TRASH" ) )
		{
			m_pDragButton = ( CDnSlotButton * )drag::GetControl();
			if( m_pDragButton )
			{
				if( (m_pDragButton->GetSlotType() == ST_INVENTORY)
					|| (m_pDragButton->GetSlotType() == ST_CHARSTATUS)
					|| (m_pDragButton->GetSlotType() == ST_INVENTORY_QUEST)
					|| (m_pDragButton->GetSlotType() == ST_INVENTORY_CASH)
					|| (m_pDragButton->GetSlotType() == ST_INVENTORY_VEHICLE)
#if defined( PRE_PERIOD_INVENTORY )
					|| (m_pDragButton->GetSlotType() == ST_INVENTORY_PERIOD)
#endif	// #if defined( PRE_PERIOD_INVENTORY )
					)
				{
					if( GetItemTask().IsRequestWait() )
						return;

					if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
						return;

					trash::SetControl( m_pDragButton );
				}
			}

			return;
		}
		else if( IsCmdControl("ID_BUTTON_SORT" ) )
		{
			if( GetItemTask().IsRequestWait() )
				return;

			// 먼저 드래그중인 아이템을 놓아야 차후 처리가 편하다.
			// 캐릭터창에서 끌어온 아이템까진 풀 필요 없으니 인벤토리 아이템만 풀자.
			if( drag::IsValid() )
			{
				CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
				if( pDragButton->GetSlotType() == ST_INVENTORY 
					|| pDragButton->GetSlotType() == ST_INVENTORY_CASH 
					|| pDragButton->GetSlotType() == ST_INVENTORY_VEHICLE )
				{
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
				}
			}

			// 현재 활성화 되어있는 탭에 따라서,
			if( GetCurrentTabID() == m_pCommonTabButton->GetTabID() )
			{
				if( m_pInvenCommonDlg->GetRegisteredItemCount() > 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 528 ), MB_OK);
					return;
				}

#if defined( PRE_PERIOD_INVENTORY )
				if( true == m_pInvenCommonDlg->IsPeriodIvenEnable() )
					GetItemTask().RequestSortPeriodInven();
				else
					GetItemTask().RequestSortInven();
#else
				GetItemTask().RequestSortInven();
#endif	// #if defined( PRE_PERIOD_INVENTORY )
			}
			else if( GetCurrentTabID() == m_pCashTabButton->GetTabID() )
			{
				if( m_pInvenCashDlg->GetRegisteredItemCount() > 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 528 ), MB_OK);
					return;
				}
				GetItemTask().SortCashInven();
			}
			else if( GetCurrentTabID() == m_pPetTabButton->GetTabID() )
			{
				if( m_pInvenPetDlg->GetRegisteredItemCount() > 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 528 ), MB_OK);
					return;
				}
				GetItemTask().SortVehicleInven();
			}

#ifdef PRE_ADD_CASHREMOVE // #52830
			else if( GetCurrentTabID() == m_pInvenStandByCashRemoveBtn->GetTabID() )
			{
				if( m_pInvenStandByCashRemoveDlg->GetRegisteredItemCount() > 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 528 ), MB_OK);
					return;
				}
				GetItemTask().SortCashRemoveInven();
			}
#endif
		}
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
		else if( IsCmdControl("ID_CLOSE_DIALOG" ) )
		{
			// 잠재력 부여창이 열려있으면, 인벤토리 닫기를 막는다.
			CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
			if(!pPotentialJewelDlg) 
				return;

			if(pPotentialJewelDlg->IsShow())
				return;
		}
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED ) 
	{
 		CEtUIRadioButton *pTabControl = static_cast<CEtUIRadioButton*>(pControl);
 		//m_groupTabDialog.ShowDialog( pTabControl->GetTabID(), true );	// commented by kalliste

#if defined(PRE_ADD_AUTOUNPACK) || defined(PRE_ADD_CASHREMOVE)
		SetInvenTabType( pTabControl->GetTabID() );
#endif

		// 현재는 일반인벤과 캐시인벤에서만 정렬버튼을 활성화시킨다. - 캐시삭제탭도 추가.		
#ifdef PRE_ADD_CASHREMOVE
		if( pTabControl->GetTabID() == m_pCommonTabButton->GetTabID() || pTabControl->GetTabID() == m_pCashTabButton->GetTabID() || pTabControl->GetTabID() == m_pPetTabButton->GetTabID() 
			|| pTabControl->GetTabID() == m_pInvenStandByCashRemoveBtn->GetTabID() ) // #52830
#else
		if( pTabControl->GetTabID() == m_pCommonTabButton->GetTabID() || pTabControl->GetTabID() == m_pCashTabButton->GetTabID() || pTabControl->GetTabID() == m_pPetTabButton->GetTabID() )
#endif			
		{
#ifdef PRE_ADD_SECONDARY_SKILL
			// 낚시, 요리 중일 경우 아이템 정렬 Disable
			if( ( CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing() ) || 
				( CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking() ) )
				m_pSortButton->Enable( false );
			else
				m_pSortButton->Enable( true );
#else // PRE_ADD_SECONDARY_SKILL
			m_pSortButton->Enable( true );
#endif // PRE_ADD_SECONDARY_SKILL
		}
		else
			m_pSortButton->Enable( false );
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnInvenTabDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			//if( pDragButton->GetSlotType() == ST_INVENTORY )
			{
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();
			}
		}

		if( m_pInvenCommonDlg )
		{
			m_pInvenCommonDlg->ReleaseNewGain();
		}
		if( m_pInvenCashDlg )
		{
			m_pInvenCashDlg->ReleaseNewGain();
		}
		if(m_pInvenPetDlg)
		{
			m_pInvenPetDlg->ReleaseNewGain();
		}

		m_pCommonTabNewStatic->Show( false );
		m_pCashTabNewStatic->Show( false );
		m_pPetTabNewStatic->Show( false );

		// 자신의 창만 끄지 말고, 취소패킷을 보내자.
		//ShowChildDialog( m_pPrivateMarketDlg, false );
		if( m_pPrivateMarketDlg && m_pPrivateMarketDlg->IsShow() )
		{
			GetTradeTask().GetTradePrivateMarket().RequestPrivateMarketCancel();
			ShowChildDialog( m_pPrivateMarketDlg, false );
		}

#ifdef PRE_ADD_GACHA_JAPAN
		// 가챠 다이얼로그가 떠 있다면 가챠 상점이 열린 것이므로 닫는다.
		if( GetInterface().IsShowGachaShopDlg() )
			GetInterface().CloseGachaDialog();
#endif // PRE_ADD_GACHA_JAPAN

		if( GetInterface().GetItemSealDlg() && GetInterface().GetItemSealDlg()->IsShow() )
			GetInterface().ShowItemSealDialog( false );
		if( GetInterface().GetItemUnsealDlg() && GetInterface().GetItemUnsealDlg()->IsShow() )
			GetInterface().CloseItemUnsealDialog();
		if( GetInterface().GetItemPotentialDlg() && GetInterface().GetItemPotentialDlg()->IsShow() )
			GetInterface().ShowItemPotentialDialog( false );
		if( GetInterface().GetSkillResetConfirmDlg() && GetInterface().GetSkillResetConfirmDlg()->IsShow() )
			GetInterface().CloseSkillResetConfirmDlg();
		if( GetInterface().GetGuildRenameDlg() && GetInterface().GetGuildRenameDlg()->IsShow() )
			GetInterface().ShowGuildRenameDlg( false );
		if( GetInterface().GetCharRenameDlg() && GetInterface().GetCharRenameDlg()->IsShow() )
			GetInterface().ShowCharRenameDlg( false );
		if( GetInterface().GetEnchantJewelDlg() && GetInterface().GetEnchantJewelDlg()->IsShow() )
			GetInterface().ShowEnchantJewelDialog( false );
		if (GetInterface().IsShowCostumeMixDlg() && CDnItemTask::IsActive())
		{
			if (CDnItemTask::GetInstance().RequestItemCostumeMixClose() == false)
				return;
		}

		if (GetInterface().IsShowCostumeDesignMixDlg() && CDnItemTask::IsActive())
		{
			if (CDnItemTask::GetInstance().RequestItemCostumeDesignMixClose() == false)
				return;
		}

#ifdef PRE_ADD_COSRANDMIX
		if (GetInterface().IsShowCostumeRandomMixDlg() && CDnItemTask::IsActive())
		{
			if (CDnItemTask::GetInstance().RequestItemCostumeRandomMixClose() == false)
				return;
		}
#endif

#if defined(PRE_ADD_REMOVE_PREFIX)
		if (GetInterface().GetRemovePrefixDlg() && GetInterface().GetRemovePrefixDlg()->IsShow())
			GetInterface().ShowRemovePrefixDialog(false);
#endif // PRE_ADD_REMOVE_PREFIX


#ifdef PRE_ADD_AUTOUNPACK
		if( GetInterface().GetAutoUnPackDlg() && GetInterface().IsOpenAutoUnPackDlg() )
			GetInterface().OpenAutoUnPack( 0, false );
#endif
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		if( GetInterface().GetItemPotentialTransferDlg() && GetInterface().GetItemPotentialTransferDlg()->IsShow() )
			GetInterface().ShowItemPotentialTransferDialog( false );
#endif

		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();

		ForceOpenNormalInvenTab( false );

		ReleaseMouseEnterControl();
	}
	else
	{
#ifdef PRE_ADD_SECONDARY_SKILL
		EnableSortButton();
#endif // PRE_ADD_SECONDARY_SKILL
		m_SmartMoveEx.MoveCursor();
	}

	CEtUITabDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
}


#ifdef PRE_ADD_SECONDARY_SKILL
void CDnInvenTabDlg::EnableSortButton()
{
#ifdef PRE_ADD_CASHREMOVE
	if( GetCurrentTabID() == m_pCommonTabButton->GetTabID() || GetCurrentTabID() == m_pCashTabButton->GetTabID() || GetCurrentTabID() == m_pPetTabButton->GetTabID() 
		|| GetCurrentTabID() == m_pInvenStandByCashRemoveBtn->GetTabID() ) // #52830
#else
	if( GetCurrentTabID() == m_pCommonTabButton->GetTabID() || GetCurrentTabID() == m_pCashTabButton->GetTabID() || GetCurrentTabID() == m_pPetTabButton->GetTabID() )
#endif
	{
		// 낚시, 요리 중일 경우 아이템 정렬 Disable
		if( ( CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing() ) || 
			( CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking() ) )
			m_pSortButton->Enable( false );
		else
			m_pSortButton->Enable( true );
	}
	else
		m_pSortButton->Enable( false );
}
#endif // PRE_ADD_SECONDARY_SKILL

void CDnInvenTabDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnInvenTabDlg::SetItem, pItem is NULL!");

	CDnInvenDlg *pInvenDlg(NULL);

	CDnItem *pDnItem = (CDnItem*)pItem;
	switch( pDnItem->GetItemType() ) {
		case ITEMTYPE_QUEST:
			pInvenDlg = static_cast<CDnInvenDlg*>(m_groupTabDialog.GetDialog(m_pQuestTabButton->GetTabID()));
			break;
		default:
			if( pDnItem->IsCashItem() )
			{
				if( pDnItem->GetItemType() == ITEMTYPE_VEHICLE || pDnItem->GetItemType() == ITEMTYPE_PET || pDnItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE)
				{
					m_pInvenPetDlg->SetItem(pItem);
					return;
				}

#ifdef PRE_ADD_CASHREMOVE
				if( pDnItem->IsCashRemoveItem() ) // 삭제된 캐쉬아이템.
					m_pInvenStandByCashRemoveDlg->SetItem( pItem );
				else
					m_pInvenCashDlg->SetItem( pItem );
#else
				// 캐시템 인벤토리가 가변형태로 변하면서 따로 처리한다.
				m_pInvenCashDlg->SetItem( pItem );
#endif
				return;
			}
			else pInvenDlg = static_cast<CDnInvenDlg*>(m_groupTabDialog.GetDialog(m_pCommonTabButton->GetTabID()));
			break;
	}

	if( pInvenDlg )
	{
		pInvenDlg->SetItem( pItem );
	}
}

void CDnInvenTabDlg::ResetSlot( MIInventoryItem *pItem )
{
	CDnInvenDlg *pInvenDlg(NULL);

	CDnItem *pDnItem = (CDnItem*)pItem;
	switch( pDnItem->GetItemType() ) {
		case ITEMTYPE_QUEST:
			pInvenDlg = static_cast<CDnInvenDlg*>(m_groupTabDialog.GetDialog(m_pQuestTabButton->GetTabID()));
			break;
		default:
			if( pDnItem->IsCashItem() )
			{
				if( pDnItem->GetItemType() == ITEMTYPE_VEHICLE || pDnItem->GetItemType() == ITEMTYPE_PET || pDnItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE)
				{
					m_pInvenPetDlg->ResetSlot(pItem->GetSlotIndex());
					return;
				}

#ifdef PRE_ADD_CASHREMOVE
				if( pDnItem->IsCashRemoveItem() ) // 삭제된 캐쉬아이템.
					m_pInvenStandByCashRemoveDlg->ResetSlot( pItem->GetSlotIndex() );
				else
					m_pInvenCashDlg->ResetSlot( pItem->GetSlotIndex() );
#else
				m_pInvenCashDlg->ResetSlot( pItem->GetSlotIndex() );
#endif
				return;
			}
			else pInvenDlg = static_cast<CDnInvenDlg*>(m_groupTabDialog.GetDialog(m_pCommonTabButton->GetTabID()));
			break;
	}

	if( pInvenDlg )
	{
		pInvenDlg->ResetSlot( pItem->GetSlotIndex() );
	}
}

void CDnInvenTabDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_23:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_YES") )
				{
					if( m_pTrashItemButton )
					{
						CDnItem *pItem = static_cast<CDnItem *>(m_pTrashItemButton->GetItem());
						if( pItem )
						{
							eItemPositionType itemPositionType(ITEMPOSITION_NONE);
							ITEM_SLOT_TYPE slotType = m_pTrashItemButton->GetSlotType();

							switch( slotType )
							{
							case ST_INVENTORY_QUEST:
							case ST_INVENTORY:
#if defined( PRE_PERIOD_INVENTORY )
							case ST_INVENTORY_PERIOD:
#endif	// #if defined( PRE_PERIOD_INVENTORY )
								{
									if( pItem->GetItemType() == ITEMTYPE_QUEST )
									{
										itemPositionType = ITEMPOSITION_QUESTINVEN;
									}
									else
									{
										itemPositionType = ITEMPOSITION_INVEN;
									}
								}
								break;
							case ST_CHARSTATUS:
								itemPositionType = ITEMPOSITION_EQUIP;
								break;
							default:
								CDebugSet::ToLogFile( "CDnInvenTabDlg::OnUICallbackProc, MESSAGEBOX_23 default case!" );
								break;
							}

							GetItemTask().RequestRemoveItem( itemPositionType, pItem->GetSlotIndex(), CDnInventoryDlg::GetCurrentAmount(), pItem->GetSerialID() );

							if( m_nTrashSoundIndex != -1 )
							{
								CEtSoundEngine::GetInstance().PlaySound( "2D", m_nTrashSoundIndex );
							}

							SetDragCtrl(RELEASE);
						}
					}
					else
					{
						CDebugSet::ToLogFile( "CDnInvenTabDlg::OnUICallbackProc, m_pTrashItemButton에 값이 들어가기 전에 아이템 삭제 메세지 창이 열렸다." );
					}
				}
				else
				{
					CDnSlotButton *pDragControl = (CDnSlotButton*)drag::GetControl();

					if( pDragControl )
					{
						pDragControl->DisableSplitMode(true);
						if (m_pTrashItemButton)
							m_pTrashItemButton->DisableSplitMode(true);
						drag::ReleaseControl();
					}
				}

				m_pTrashItemButton = NULL;
			}
		}
		break;
	case PRIVATE_MARKET_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_CLOSE_DIALOG") || IsCmdControl("ID_BUTTON_CANCEL") )
				{
					GetTradeTask().GetTradePrivateMarket().RequestPrivateMarketCancel();
					ShowChildDialog( m_pPrivateMarketDlg, false );
				}
			}
		}
		break;
	}
}

void CDnInvenTabDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	ASSERT(!(dwItemCnt%ITEM_X)&&"CDnInvenTabDlg::SetUseItemCnt");
	if( dwItemCnt % ITEM_X ) return;
	if( !m_pInvenCommonDlg ) return;

	// Note : 캐쉬 인벤은 모든 탭이 사용가능하다. 일반 인벤만 탭의 사용여부를 셋할 수 있다.
	//
	m_pInvenCommonDlg->SetUseItemCnt( dwItemCnt );
}

void CDnInvenTabDlg::SetUseQuestItemCnt( DWORD dwItemCnt )
{
	ASSERT(!(dwItemCnt%ITEM_X)&&"CDnInvenTabDlg::SetUseQuestItemCnt");
	if( dwItemCnt%ITEM_X )	return;
	if( !m_pInvenQuestDlg )	return;

	m_pInvenQuestDlg->SetUseItemCnt( dwItemCnt );
}

void CDnInvenTabDlg::SetDragCtrl(eDragCtrlType type, bool bParam)
{
	CDnSlotButton *pDragControl = (CDnSlotButton*)drag::GetControl();
	if( pDragControl )
	{
		if (type == RELEASE)
		{
			if (bParam)
				drag::Command(UI_DRAG_CMD_CANCEL);
			drag::ReleaseControl();
		}
		else if (type == RENDER)
		{
			drag::SetRender(bParam);
		}
		else
			_ASSERT(0);
	}
}

void CDnInvenTabDlg::Process( float fElapsedTime )
{
	m_SmartMoveEx.Process();
	CEtUIDialog::Process( fElapsedTime );	

	if( IsShow() )
	{
		if( !CDnItemTask::IsActive() ) return;
		INT64 nCoin = GetItemTask().GetCoin();
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

		if( GetInterface().GetMainMenuDialog()->IsForceOpenNormalInvenTabDialog() )
		{
			if( GetInterface().GetMainMenuDialog()->IsOpenDialog( CDnMainMenuDlg::MARKET_DIALOG ) )
			{
				if( m_pQuestTabButton->IsEnable() ) m_pQuestTabButton->Enable(false);
				if( m_pGestureTabButton->IsEnable() ) m_pGestureTabButton->Enable(false);
#ifdef PRE_ADD_CASHREMOVE
				if( m_pInvenStandByCashRemoveBtn->IsEnable() ) m_pInvenStandByCashRemoveBtn->Enable(false);
#endif

			}
			else if( GetInterface().GetMainMenuDialog()->IsWithDrawGlyphOpen() )	// 문장 회수창이 있을 경우
			{
				if( m_pQuestTabButton->IsEnable() ) m_pQuestTabButton->Enable(false);
				if( m_pCashTabButton->IsEnable() ) m_pCashTabButton->Enable(true);
				if( m_pGestureTabButton->IsEnable() ) m_pGestureTabButton->Enable(false);
				if( m_pPetTabButton->IsEnable() ) m_pPetTabButton->Enable(false);
#ifdef PRE_ADD_CASHREMOVE
				if( m_pInvenStandByCashRemoveBtn->IsEnable() ) m_pInvenStandByCashRemoveBtn->Enable(false);
#endif

			}
			else if (GetInterface().GetMainMenuDialog()->IsOpenDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG)) //判断强化语句开始
			{
				if (!m_pCashTabButton->IsEnable()) m_pCashTabButton->Enable(true);
				if (!m_pCommonTabButton->IsEnable()) m_pCommonTabButton->Enable(true);
				//if (m_pQuestTabButton->IsEnable()) m_pQuestTabButton->Enable(false);
				//if (m_pGestureTabButton->IsEnable()) m_pGestureTabButton->Enable(false);
				//if (m_pCommonTabButton->IsEnable()) m_pCommonTabButton->Enable(false);
				//if (m_pInvenStandByCashRemoveBtn->IsEnable()) m_pInvenStandByCashRemoveBtn->Enable(false);
				//if (m_pPetTabButton->IsEnable()) m_pPetTabButton->Enable(false);
			}                                                                                               //判断强化语句结束
			else
			{
				if( GetCurrentTabID() != m_pCommonTabButton->GetTabID() )
					SetCheckedTab( m_pCommonTabButton->GetTabID() );

				ForceOpenNormalInvenTab( true );
			}
		}
	}

	// 아무래도 여기 있기에 좀 별로인거 같지만,
	// 쓰레기통 아이콘도 있고 기존에 계속 쓰던 코드이기도해서, 그냥 두기로 하겠다.
	if( trash::IsValid() && !m_pTrashItemButton )
	{
		m_pTrashItemButton = (CDnSlotButton*)trash::GetControl();
		trash::ReleaseControl();

		ITEM_SLOT_TYPE slotType = m_pTrashItemButton->GetSlotType();
		switch( slotType )
		{
		case ST_QUICKSLOT:			// 퀵슬롯 아이템은 빼기만 가능하다.
			{
				CDnQuickSlotButton* pDragBtn = static_cast<CDnQuickSlotButton*>(drag::GetControl());
				if (pDragBtn)
				{
					bool bGiuldWarSkill = false;
					MIInventoryItem *pInvenItem = pDragBtn->GetItem();
					if( pInvenItem )
					{
						if( GetSkillTask().IsGuildWarSkill( pInvenItem->GetClassID() ) )
							bGiuldWarSkill = true;
					}

					if( !bGiuldWarSkill )
					{
						GetItemTask().RequestDelQuickSlot(pDragBtn->GetItemSlotIndex());
						pDragBtn->ResetSlot();
					}
				}
				else
					_ASSERT(0);

				SetDragCtrl(RELEASE, true);
				m_pTrashItemButton = NULL;
			}
			return;
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		case ST_TOTAL_LEVEL_SKILL:
			{
				CDnQuickSlotButton* pDragBtn = static_cast<CDnQuickSlotButton*>(drag::GetControl());
				if (pDragBtn)
				{
					GetSkillTask().RequestRemoveTotalLevelSkill(pDragBtn->GetItemSlotIndex());
					pDragBtn->ResetSlot();
				}
				else
					_ASSERT(0);

				SetDragCtrl(RELEASE, true);
				m_pTrashItemButton = NULL;
				return;
			}
			break;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

		case ST_LIFESKILL_QUICKSLOT:	// LifeSkill 퀵슬롯 역시 빼기 가능
			{
				CDnLifeSkillButton* pDragBtn = static_cast<CDnLifeSkillButton*>(drag::GetControl());
				if (pDragBtn)
				{
					GetItemTask().RequestDelLifeSkillQuickSlot(pDragBtn->GetSlotIndex());
					pDragBtn->ResetSlot();
				}
				else
					_ASSERT(0);

				SetDragCtrl(RELEASE, true);
				m_pTrashItemButton = NULL;
			}
			return;
		case ST_LIFESKILL_QUICKSLOT_EX:
			{
				CDnQuickSlotButton* pDragBtn = static_cast<CDnQuickSlotButton*>(drag::GetControl());
				if( pDragBtn )
				{
					GetItemTask().RequestDelLifeSkillQuickSlot( pDragBtn->GetSlotIndex() );
					GetInterface().GetMainBarDialog()->ResetLifeSkillQuickSlot( pDragBtn->GetSlotIndex() );
				}
				SetDragCtrl(RELEASE, true);
				m_pTrashItemButton = NULL;
			}
			return;
		case ST_STORE:
		case ST_STORAGE_PLAYER:
		case ST_STORAGE_GUILD:
		case ST_STORAGE_FARM:
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 스킬 아이템은 버릴 수 없다. 라디오메세지 아이콘도 마찬가지.
		if( m_pTrashItemButton->GetItemType() == MIInventoryItem::Skill ||
			m_pTrashItemButton->GetItemType() == MIInventoryItem::RadioMsg ||
			m_pTrashItemButton->GetItemType() == MIInventoryItem::Gesture ||
			m_pTrashItemButton->GetItemType() == MIInventoryItem::SecondarySkill )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 거래 제재 검사
		if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true )
#ifdef PRE_ADD_DWC
			&& GetDWCTask().IsDWCChar() == false
#endif
			)
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton = NULL;
			return;
		}

#ifdef PRE_ADD_CASHREMOVE
	
		// 캐시제거대기탭 에서는 캐시삭제Dlg를 열지않도록 한다.
		if( GetEnableInvenType() != eCASHREMOVE )
		{
			// 파괴불가 아이템은 버릴 수 없다.
			if( m_pTrashItemButton->GetItemType() == MIInventoryItem::Item )
			{				
				CDnItem *pItem = static_cast<CDnItem *>(m_pTrashItemButton->GetItem());
				if( pItem )
				{
#ifdef PRE_ADD_EQUIPLOCK
					if (CDnItemTask::IsActive())
					{
						const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
						EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
						if (status != EquipItemLock::None)
						{
							SetDragCtrl(RELEASE, true);
							m_pTrashItemButton = NULL;
							return;
						}
					}
#endif

					// 캐시템.
					if( pItem->IsCashItem()
						&& eItemTypeEnum::ITEMTYPE_VEHICLE != pItem->GetItemType()
						&& eItemTypeEnum::ITEMTYPE_VEHICLE_SHARE != pItem->GetItemType()
						&& eItemTypeEnum::ITEMTYPE_PET != pItem->GetItemType() 
					  )					
					{
						SetDragCtrl(RENDER, false);

						// #50317 ----
						bool bRes = false;						
						int itemType = pItem->GetItemType();
						DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TCASHERASABLETYPE );
						DNTableCell * pCell = NULL;
						if( pTable )
						{
							int size = pTable->GetItemCount();
							for( int i=0; i<size; ++i )
							{
								int idx = pTable->GetItemID( i );
								pCell = pTable->GetFieldFromLablePtr( idx, "_Type" );
								if( pCell )
								{ 
									if( itemType == pCell->GetInteger() )
									{
										bRes = true;
										break;
									}
								}
							}
						}						

//------------------------------------------------------------
						bRes = true;  //[debug_DelItem]  删除任意道具
//------------------------------------------------------------

						if( bRes )
						{
							bool bIsShowCostumeMix = false;
							if( GetInterface().IsShowCostumeMixDlg() )
							{
								bIsShowCostumeMix = true;
							}
							else if( GetInterface().IsShowCostumeDesignMixDlg() )
							{
								bIsShowCostumeMix = true;
							}
#ifdef PRE_ADD_COSRANDMIX
							else if( GetInterface().IsShowCostumeRandomMixDlg() )
							{
								bIsShowCostumeMix = true;
							}
#endif // #ifdef PRE_ADD_COSRANDMIX

							// 캐쉬템삭제Dlg 열기 - 미장착아이템만.
							if( (GetInterface().IsEquipped( (CDnItem *)m_pTrashItemButton->GetItem(), true ) == false) &&
								false == bIsShowCostumeMix &&
								ITEM_SLOT_TYPE::ST_INVENTORY_CASH == m_pTrashItemButton->GetSlotType() // #75398
							  )
							{				
								SetDragCtrl(RELEASE, true);	// #73782							
								GetInterface().OpenCashItemRemoveDialog( true, m_pTrashItemButton->GetItem() );								
							}
							else
							{
								SetDragCtrl(RELEASE, true);
								m_pTrashItemButton = NULL;
							}
						}
						else
						{
							// 삭제불가.
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1753 ) );

							SetDragCtrl(RELEASE, true);
							m_pTrashItemButton = NULL;
						}
						// ----#50317

						return;
					}

					// 일반템.
					else if( !pItem->IsCanDestruction() )
					{
						SetDragCtrl(RELEASE, true);
						m_pTrashItemButton = NULL;
						GetInterface().MessageBox( 1753, MB_OK );
						return;
					}
				}
			}
		}
#else
		// 파괴불가 아이템은 버릴 수 없다.
		if( m_pTrashItemButton->GetItemType() == MIInventoryItem::Item )
		{
			CDnItem *pItem = static_cast<CDnItem *>(m_pTrashItemButton->GetItem());
			if( pItem ) {
				if( !pItem->IsCanDestruction() )
				{
					SetDragCtrl(RELEASE, true);
					m_pTrashItemButton = NULL;
					GetInterface().MessageBox( 1753, MB_OK );
					return;
				}

				// 캐시템은 기본적으로 파괴가 안되는게 정상이라 한다.
				// 혹시 파괴불가가 설정되어있지 않더라도 파괴할 수 없도록 한다.
				if( pItem->IsCashItem() )
				{
					SetDragCtrl(RELEASE, true);
					m_pTrashItemButton = NULL;
					return;
				}
			}
		}
#endif

		// 거래중일때는 아이템을 버릴 수 없다.
		//
		// 꼭 이렇게 거래중일때 모든 아이템을 버릴 수 없게 해야하는지 의문이다.
		// 거래창에 등록되어있는 아이템만 버릴 수 없게해도 괜찮치 않을까..
		if( GetInterface().IsShowPrivateMarketDlg() 
			|| GetInterface().IsShowCostumeMixDlg()
			|| GetInterface().IsShowCostumeDesignMixDlg()
			)
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}

		if( GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 거래창이나 분해창에 등록중인 상태라면 버리지 않는다.
		if( m_pTrashItemButton->IsRegist() )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 일반적인 등록은 아니지만, 등록의 형태를 띈 창들이 있다.(문장보옥, 조합, 강화 등의 재료슬롯 있는 창)
		// 이땐 버릴 수 없게 해버리자.(등록 후 버리고 제작 누르는 것 방지)
		// 불편하다면 그때 가서 생각하자.
		if( GetInterface().GetMainMenuDialog(CDnMainMenuDlg::PLATE_MAIN_DIALOG)->IsShow() ||
			GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG)->IsShow() ||
			GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COMPOUND_DIALOG)->IsShow() ||
			GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG)->IsShow() )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 다이체크.
		if( GetItemTask().CheckDie( true ) )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 문장 제작 창일 경우
		if( GetInterface().GetMainMenuDialog()->IsWithDrawGlyphOpen() )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}

		// 아이템이 어딘가에 등록중이라면,(거래, 분해, 조합..)
		// 아래처럼 CDnItem으로 검사하면 안된다. 이건 아이템자체라서 슬롯으로 구분되는 형태가 아닌 특정아이템 전부가 되버린다.
		//CDnItem *pItem = static_cast<CDnItem *>(m_pTrashItemButton->GetItem());

#ifdef PRE_ADD_CASHREMOVE

		// 캐시제거대기탭 에서는 버릴 수 없다.
		if( GetEnableInvenType() == eCASHREMOVE )
		{
			SetDragCtrl(RELEASE, true);
			m_pTrashItemButton->DisableSplitMode(true);
			m_pTrashItemButton = NULL;
			return;
		}
#endif

		CTask* pRawTask = CTaskManager::GetInstance().GetTask("GameTask");
		if (pRawTask)
		{
			CDnGameTask* pGameTask = static_cast<CDnGameTask*>(pRawTask);
			if (pGameTask)
			{
				bool bTutorial = pGameTask->IsTutorial();
				if (bTutorial)
				{
					if (m_pTrashItemButton->GetItemType() == MIInventoryItem::Item)
					{
						MIInventoryItem* pMIInvenItem = m_pTrashItemButton->GetItem();
						if (pMIInvenItem)
						{
							CDnItem *pItem = static_cast<CDnItem *>(pMIInvenItem);
							if (pItem && pItem->GetItemType() == ITEMTYPE_WEAPON)
							{
								SetDragCtrl(RELEASE, true);
								m_pTrashItemButton->DisableSplitMode(true);
								m_pTrashItemButton = NULL;

								GetInterface().MessageBox(8428, MB_OK); // UISTRING : 현재 튜토리얼 단계에선 무기를 파괴할 수 없습니다.
								return;
							}
						}
					}
				}
			}
		}

		SetDragCtrl(RENDER, false);
		GetInterface().MessageBox( MESSAGEBOX_23, MB_YESNO, MESSAGEBOX_23, this, false, true );
	}
}

bool CDnInvenTabDlg::IsEmptySlot()
{
	if( !m_pInvenCommonDlg )
	{
		return false;
	}

	return m_pInvenCommonDlg->IsEmptySlot();
}

int CDnInvenTabDlg::GetEmptySlot()
{
	if( !m_pInvenCommonDlg )
	{
		return -1;
	}

	return m_pInvenCommonDlg->GetEmptySlot();
}

int CDnInvenTabDlg::GetEmptySlotCount()
{
	if( !m_pInvenCommonDlg )
	{
		return 0;
	}

	return m_pInvenCommonDlg->GetEmptySlotCount();
}

void CDnInvenTabDlg::ShowPrivateMarketDialog( bool bShow, bool bCancel )
{
	if (bCancel)
		m_pPrivateMarketDlg->OnCancelTrade();
	else
		ForceOpenNormalInvenTab( false );
	ShowChildDialog( m_pPrivateMarketDlg, bShow );
}

bool CDnInvenTabDlg::IsShowPrivateMarketDialog()
{
	if( !m_pPrivateMarketDlg ) return false;
	return m_pPrivateMarketDlg->IsShow();
}

void CDnInvenTabDlg::AddPrivateMarketChatMe( const wchar_t *wszChat )
{
	if( !m_pPrivateMarketDlg ) return;
	if( !m_pPrivateMarketDlg->IsShow() ) return;
	m_pPrivateMarketDlg->AddChatMe( wszChat );
}

void CDnInvenTabDlg::AddPrivateMarketChatYou( const wchar_t *wszChat )
{
	if( !m_pPrivateMarketDlg ) return;
	if( !m_pPrivateMarketDlg->IsShow() ) return;
	m_pPrivateMarketDlg->AddChatYou( wszChat );
}

void CDnInvenTabDlg::SetPrivateMarketName( const wchar_t *wszMe, const wchar_t *wszYou )
{
	if( !m_pPrivateMarketDlg ) return;
	m_pPrivateMarketDlg->SetNameMe( wszMe );
	m_pPrivateMarketDlg->SetNameYou( wszYou );
}

bool CDnInvenTabDlg::IsYourName( const wchar_t *wszName )
{
	if( !m_pPrivateMarketDlg ) return false;
	if( !m_pPrivateMarketDlg->IsShow() ) return false;

	const wchar_t *pYourName = m_pPrivateMarketDlg->GetNameYou();

	if( StrCmpW(pYourName, wszName) == 0 )
		return true;
	else
		return false;
}

void CDnInvenTabDlg::AddItemToPrivateMarketTradeList(CDnQuickSlotButton* pPressedButton, bool bItemSplit)
{
	if (!m_pPrivateMarketDlg) return;
	m_pPrivateMarketDlg->AddItemToMyTradeList(pPressedButton, bItemSplit);
}

void CDnInvenTabDlg::ShowRadioButton( bool bShow )
{
	if( !bShow ) {
		SetCheckedTab( m_pCommonTabButton->GetTabID() );
	}
	m_pCommonTabButton->Show( bShow );
	m_pCashTabButton->Show( true );
	m_pQuestTabButton->Show( bShow );
	m_pGestureTabButton->Show( bShow );
#ifndef PRE_FIX_HIDE_VEHICLETAB
	m_pPetTabButton->Show(bShow);
#endif

#ifdef PRE_ADD_CASHREMOVE
	m_pInvenStandByCashRemoveBtn->Show( bShow );
#endif

}

void CDnInvenTabDlg::ShowTab( int nTabID )
{
	Show( true );
	int nValue = 0;
	switch( nTabID ) {
		case ST_INVENTORY: nValue = m_pCommonTabButton->GetTabID(); break;
		case ST_INVENTORY_QUEST: nValue = m_pQuestTabButton->GetTabID(); break;
		case ST_INVENTORY_CASH: nValue = m_pCashTabButton->GetTabID(); break;
		case ST_INVENTORY_VEHICLE: nValue = m_pPetTabButton->GetTabID(); break;
	}
	SetCheckedTab( nValue );
}

CDnInvenDlg *CDnInvenTabDlg::GetInvenDlg( ITEM_SLOT_TYPE Type )
{
	switch( Type ) {
		case ITEM_SLOT_TYPE::ST_INVENTORY: return m_pInvenCommonDlg;
		// 캐시 인벤토리 직접 얻어와 뭔가 하는건 허용하지 않는다.
		case ITEM_SLOT_TYPE::ST_INVENTORY_CASH: return NULL;
		case ITEM_SLOT_TYPE::ST_INVENTORY_QUEST: return m_pInvenQuestDlg;
		case ITEM_SLOT_TYPE::ST_INVENTORY_VEHICLE: return NULL;
	}
	return NULL;
}

void CDnInvenTabDlg::ShowTabNew( int nTabID )
{
	switch( nTabID )
	{
		case ST_INVENTORY: if( m_pCommonTabNewStatic ) m_pCommonTabNewStatic->Show( true ); break;
		case ST_INVENTORY_CASH:
			{
				if( m_pCashTabNewStatic ) m_pCashTabNewStatic->Show( true );

#ifdef PRE_ADD_CASHINVENTAB
				if( m_pInvenCashDlg )
					m_pInvenCashDlg->AddNewItem();
#endif // PRE_ADD_CASHINVENTAB

			}
			break;
		case ST_INVENTORY_VEHICLE: if( m_pPetTabNewStatic ) m_pPetTabNewStatic->Show( true ); break;
	}
}

void CDnInvenTabDlg::ForceOpenNormalInvenTab( bool bEnable )
{
#ifdef PRE_ADD_CASHREMOVE
	if( !m_pCashTabButton || !m_pQuestTabButton || !m_pGestureTabButton || !m_pInvenStandByCashRemoveBtn )
#else
	if( !m_pCashTabButton || !m_pQuestTabButton || !m_pGestureTabButton )
#endif
		return;

	if( !m_pPetTabButton)
		return;

	if(bEnable)
	{
		if( m_pPetTabButton->IsEnable() ) m_pPetTabButton->Enable(false);
	}
	else
	{
		if( !m_pPetTabButton->IsEnable() ) m_pPetTabButton->Enable(true);
	}

	if( bEnable )
	{
		if( m_pCashTabButton->IsEnable() ) m_pCashTabButton->Enable(true);
		if( m_pQuestTabButton->IsEnable() ) m_pQuestTabButton->Enable(false);
		if( m_pGestureTabButton->IsEnable() ) m_pGestureTabButton->Enable(false);
#if defined(PRE_ADD_SERVER_WAREHOUSE)
		if( m_pCommonTabButton->IsEnable() ) m_pCommonTabButton->Enable(false);
#endif
#ifdef PRE_ADD_CASHREMOVE
		if( m_pInvenStandByCashRemoveBtn->IsEnable() ) m_pInvenStandByCashRemoveBtn->Enable(false);
#endif

	}
	else
	{
		if( !m_pCashTabButton->IsEnable() ) m_pCashTabButton->Enable(true);
		if( !m_pQuestTabButton->IsEnable() ) m_pQuestTabButton->Enable(true);
		if( !m_pGestureTabButton->IsEnable() ) m_pGestureTabButton->Enable(true);
#if defined(PRE_ADD_SERVER_WAREHOUSE)
		if( !m_pCommonTabButton->IsEnable() ) m_pCommonTabButton->Enable(true);
#endif
#ifdef PRE_ADD_CASHREMOVE
		if( !m_pInvenStandByCashRemoveBtn->IsEnable() ) m_pInvenStandByCashRemoveBtn->Enable(true);
#endif

	}
}


#if defined(PRE_ADD_AUTOUNPACK) || defined(PRE_ADD_CASHREMOVE)
// 현재활성화된 인벤타입.
void CDnInvenTabDlg::SetInvenTabType( int tabID )
{
	if( tabID == m_pCommonTabButton->GetTabID() )
		m_EnableInvenType = eCOMMON;

	else if( tabID == m_pCashTabButton->GetTabID() )
		m_EnableInvenType = eCASH;

	else if( tabID == m_pQuestTabButton->GetTabID() ) 
		m_EnableInvenType = eQUEST;

	else if( tabID == m_pGestureTabButton->GetTabID() ) 
		m_EnableInvenType = eGESTURE;

#ifdef PRE_ADD_CASHREMOVE
	else if( tabID == m_pInvenStandByCashRemoveBtn->GetTabID() ) 
		m_EnableInvenType = eCASHREMOVE;
#endif
	else
		m_EnableInvenType = eCOMMON;
	
}
#endif

void CDnInvenTabDlg::EnableInvenTab(int type)
{
	if (!m_pCommonTabButton || !m_pCashTabButton || !m_pQuestTabButton || !m_pGestureTabButton)
		return;

	m_pCommonTabButton->Enable(false);
	m_pCashTabButton->Enable(true);
	m_pQuestTabButton->Enable(false);
	m_pGestureTabButton->Enable(false);

	if (type & eCOMMON)
		m_pCommonTabButton->Enable(true);
	if (type & eCASH)
		m_pCashTabButton->Enable(true);
	if (type & eQUEST)
		m_pQuestTabButton->Enable(true);
	if (type & eGESTURE)
		m_pGestureTabButton->Enable(true);

	if( !m_pPetTabButton)
		return;

	m_pPetTabButton->Enable(false);

	if(type & ePET)
		m_pPetTabButton->Enable(true);

#ifdef PRE_ADD_CASHREMOVE
	if( m_pInvenStandByCashRemoveBtn )
	{
		if( type & eCASHREMOVE )
			m_pInvenStandByCashRemoveBtn->Enable( true );
		else
			m_pInvenStandByCashRemoveBtn->Enable(false);
	}
#endif

}

void CDnInvenTabDlg::DisablePressedButtonSplitMode()
{
	if( !m_pInvenCommonDlg ) return;
	return m_pInvenCommonDlg->DisablePressedButtonSplitMode();
}




#ifdef PRE_ADD_CASHREMOVE

// 캐쉬아이템삭제 수락.
void CDnInvenTabDlg::RemoveCashItemAccept()
{	
	if( m_pTrashItemButton )
	{
		CDnItem *pItem = static_cast<CDnItem *>(m_pTrashItemButton->GetItem());
		if( pItem )
		{
			eItemPositionType itemPositionType(ITEMPOSITION_NONE);
			ITEM_SLOT_TYPE slotType = m_pTrashItemButton->GetSlotType();
									
			if( slotType == ST_INVENTORY_CASH )
			{
				GetItemTask().RequestRemoveCashItem( pItem->GetSerialID(), false );
			
				if( m_nTrashSoundIndex != -1 )
					CEtSoundEngine::GetInstance().PlaySound( "2D", m_nTrashSoundIndex );
				
				SetDragCtrl(RELEASE);
			}
		}
	}	
}


// 캐쉬아이템삭제 취소.
void CDnInvenTabDlg::RemoveCashItemCancel()
{	
	CDnSlotButton *pDragControl = (CDnSlotButton*)drag::GetControl();

	if( pDragControl )
	{
		pDragControl->DisableSplitMode(true);
		if (m_pTrashItemButton)
			m_pTrashItemButton->DisableSplitMode(true);
		drag::ReleaseControl();
	}	

	m_pTrashItemButton = NULL;
}

#endif


#ifdef PRE_ADD_CASHINVENTAB
CDnItem * CDnInvenTabDlg::GetCashInvenItemBySN( INT64 SN )
{
	if( m_pInvenCashDlg )
		return m_pInvenCashDlg->GetCashInvenItemBySN( SN );
	
	return NULL;
}
#endif // PRE_ADD_CASHINVENTAB