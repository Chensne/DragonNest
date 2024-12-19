#include "StdAfx.h"
#include "DnTableDB.h"
#include "DnInvenSlotDlg.h"
#include "DnItemSlotButton.h"
#include "DnCharStatusDlg.h"
#include "MIInventoryItem.h"
#include "DnTradeTask.h"
#include "DnItemTask.h"
#include "DnSkillTask.h"
#include "DnInterfaceDlgID.h"
#include "DnInvenSepDlg.h"
#include "DnInvenTabDlg.h"
#include "DnStoreTabDlg.h"
#include "DnInterface.h"
#include "DnMessageBox.h"
#include "DnStoreConfirmDlg.h"
#include "DnMainDlg.h"
#include "DnTooltipDlg.h"
#include "EtUITextUtil.h"
#include "DnMainMenuDlg.h"
#include "DnPlayerActor.h"
#include "DnItemDisjointDlg.h"
#include "DnPlateMainDlg.h"
#include "DnItemUpgradeDlg.h"
#ifdef PRE_ADD_EXCHANGE_ENCHANT
#include "DnItemUpgradeExchangeDlg.h"
#endif
#include "DnItemCompoundTabDlg.h"
#include "DnMailDlg.h"
#include "DnNameLinkMng.h"
#include "DnBridgeTask.h"
#include "DnMarketTabDlg.h"
#include "DnMarketPriceDlg.h"
#include "DnAskLookChgDlg.h"
#include "DnItemSealDlg.h"
#include "DnMarketBuyDlg.h"
#include "DnItemUnSealDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnCharPlateDlg.h"
#include "DnPotentialJewelDlg.h"
#include "DnLifeStorageDlg.h"
#include "SyncTimer.h"

#include "DnChatTabDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnCashShopTask.h"
#include "DnCostumeMixDlg.h"
#include "DnCostumeDesignMixDlg.h"
#ifdef PRE_ADD_COSRANDMIX
#include "DnCostumeRandomMixDlg.h"
#endif
#include "DnChatRoomDlg.h"

#include "DnAppellationTask.h"
#include "DnGuildTask.h"
#include "DnCashShopTask.h"

#ifdef PRE_ADD_COOKING_SYSTEM
#include "DnLifeSkillCookingTask.h"
#include "ManufactureSkill.h"
#endif // PRE_ADD_COOKING_SYSTEM

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
#include "DnCashShopRefundInvenDlg.h"
#endif

#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnLifeSkillFishingTask.h"
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_SECONDARY_SKILL
#include "DnEnchantJewelDlg.h"

#include "DnPetTask.h"
#include "DnPetExtendPeriodDlg.h"
#include "DnPetNamingDlg.h"
#include "DnPetActor.h"
#include "DnVehicleTask.h"
#include "DnSystemDlg.h"

#if defined(PRE_ADD_REMOVE_PREFIX)
#include "DnRemovePrefixDlg.h"
#include "DnPotentialJewel.h"
#endif // PRE_ADD_REMOVE_PREFIX

#include "DnPGStorageTabDlg.h"
#include "DnPlayerStorageDlg.h"
#include "DnGuildStorageDlgNew.h"

#ifdef PRE_ADD_INVEN_EXTENSION
#include "DnInvenExtensionDlg.h"
#endif

#ifdef PRE_ADD_AUTOUNPACK
#include "DnAutoUnPackDlg.h"
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
#include "DnPotentialTransferDlg.h"
#endif

#include "DnCharPetDlg.h"
#include "DnPetOptionDlg.h"

#ifdef PRE_ADD_SALE_COUPON
#include "DnCashShopDlg.h"
#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_SERVER_WAREHOUSE
#include "DnWorldServerStorageInventoryDlg.h"
#endif

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
#include "DnPotentialJewelCleanDlg.h"
#endif

#ifdef PRE_ADD_TALISMAN_SYSTEM
#include "DnCharTalismanDlg.h"
#endif

#ifdef PRE_ADD_DRAGON_GEM
#include "DnDragonGemEquipDlg.h"
#include "DnDragonGemRemoveDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnInvenSlotDlg::CDnInvenSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pInvenSepDlg(NULL)
	, m_pAskLookChgDlg(NULL)
	, m_pPressedButton(NULL)
#ifdef PRE_ADD_INVEN_EXTENSION
	, m_pInvenExtensionDlg(NULL)
#endif
{
	m_SlotType = ST_INVENTORY;
	m_SlotStartIndex = 0;
#ifdef PRE_ADD_INVEN_EXTENSION
	m_nUseItemCnt = 0;
#endif
}

CDnInvenSlotDlg::~CDnInvenSlotDlg(void)
{
	SAFE_DELETE(m_pInvenSepDlg);
	SAFE_DELETE(m_pAskLookChgDlg);
#ifdef PRE_ADD_INVEN_EXTENSION
	SAFE_DELETE(m_pInvenExtensionDlg);
#endif
}

void CDnInvenSlotDlg::Initialize(bool bShow, int pageNum, int slotCount, const char* pResFileName)
{
	m_SlotStartIndex = pageNum * slotCount;
	std::string str = pResFileName;
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName( str.c_str() ).c_str(), bShow);
}

void CDnInvenSlotDlg::InitialUpdate()
{
	m_pInvenSepDlg = new CDnInvenSepDlg( UI_TYPE_MODAL, NULL, INVEN_SPLIT_DIALOG, this );
	m_pInvenSepDlg->Initialize( false );
	m_pAskLookChgDlg = new CDnAskLookChgDlg( UI_TYPE_MODAL, NULL, INVEN_HAIRDYE_DIALOG, this );
	m_pAskLookChgDlg->Initialize( false );
#ifdef PRE_ADD_INVEN_EXTENSION
	m_pInvenExtensionDlg = new CDnInvenExtensionDlg( UI_TYPE_CHILD, this, INVEN_EXTENSION_DIALOG, this );
	m_pInvenExtensionDlg->Initialize( false );
#endif
}

void CDnInvenSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnQuickSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnQuickSlotButton*>(pControl);

	pItemSlotButton->SetSlotIndex( GetSlotStartIndex() + (int)m_vecSlotButton.size() );
	m_vecSlotButton.push_back( pItemSlotButton );
}

void CDnInvenSlotDlg::SetSlotType( ITEM_SLOT_TYPE slotType )
{
	m_SlotType = slotType;
	for( DWORD i=0; i<m_vecSlotButton.size(); i++ )
	{
		m_vecSlotButton[i]->SetSlotType( slotType );
	}
#ifdef PRE_ADD_INVEN_EXTENSION
	if( m_SlotType != ST_INVENTORY )
	{
		if( m_pInvenExtensionDlg->IsShow() )
			ShowChildDialog( m_pInvenExtensionDlg, false );
	}
#endif // PRE_ADD_INVEN_EXTENSION
}

bool CDnInvenSlotDlg::IsOnMailMode(CDnMailDlg* pMailDlg) const
{
	return (pMailDlg && pMailDlg->IsShow());
}

void CDnInvenSlotDlg::HandleItemClick(CDnItem* pItem, CDnMailDlg* pMailDlg, CDnInvenTabDlg* pInvenDlg)
{
	if (m_pPressedButton->IsRegist())
		return;

	if (GetInterface().IsOpenWorldMsgDialog())
		return;

	CDnInventoryDlg::SetSelectAmount( pItem->GetOverlapCount() );

	m_pPressedButton->EnableSplitMode(pItem->GetOverlapCount());
	drag::SetControl(m_pPressedButton);

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	CDnItemUpgradeDlg *pItemUpgradeDlg = static_cast<CDnItemUpgradeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_UPGRADE_DIALOG ) ) ;
	if( pItemUpgradeDlg && pItemUpgradeDlg->IsShow() )
		pItemUpgradeDlg->SetCurrentInvenSlot( m_pPressedButton );		
#endif

	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	if( CDnInterface::IsActive() ) {
		CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
		if( pCharStatusDlg && pCharStatusDlg->IsShow() ) {

			if( pCharStatusDlg->GetWithDrawOpen() )
				return;

			pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage() );
			/*
			switch( m_SlotType ) {
				case ST_INVENTORY: pCharStatusDlg->SwapEquipButtons( false ); break;
				case ST_INVENTORY_CASH: pCharStatusDlg->SwapEquipButtons( true ); break;
			}
			*/
		}
	}
}

#ifdef PRE_ADD_INVEN_EXTENSION
void CDnInvenSlotDlg::Show( bool bShow )
{
	CEtUIDialog::Show( bShow );

	if( m_SlotType == ST_INVENTORY )
	{
		CheckShowExtensionDlg( bShow );
	}
	else
	{
		if( m_pInvenExtensionDlg->IsShow() )
			ShowChildDialog( m_pInvenExtensionDlg, false );
	}
}

void CDnInvenSlotDlg::MoveDialog( float fX, float fY )
{
	CEtUIDialog::MoveDialog( fX, fY );

	if( m_SlotType == ST_INVENTORY )
	{
		if( m_pInvenExtensionDlg ) m_pInvenExtensionDlg->MoveDialog( fX, fY );
	}
}

void CDnInvenSlotDlg::CheckShowExtensionDlg( bool bShow )
{
#ifdef PRE_ADD_INSTANT_CASH_BUY
	bool bShowDlg = ((m_nUseItemCnt < ITEM_MAX) && bShow);
#else // PRE_ADD_INSTANT_CASH_BUY
	bool bShowDlg = ((m_nUseItemCnt < ITEM_MAX) && (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage) && bShow);
#endif // PRE_ADD_INSTANT_CASH_BUY
	ShowChildDialog( m_pInvenExtensionDlg, bShowDlg );
}
#endif // PRE_ADD_INVEN_EXTENSION

void CDnInvenSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_ITEM" ) )
		{
			if( GetItemTask().IsRequestWait() )
				return;

			if( GetTradeTask().GetTradeItem().IsRequestWait() )
				return;

			if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
				return;

			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			m_pPressedButton = ( CDnQuickSlotButton * )pControl;

			if( m_pPressedButton->GetItemState() == ITEMSLOT_DISABLE ) 
				return;

#ifdef PRE_ADD_TALISMAN_SYSTEM
			CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG);
			if( pCharStatusDlg && pCharStatusDlg->IsShow() )
			{
				pCharStatusDlg->ShowTalismanSlotEffciency(false);

				/*CDnItem* pItem = (CDnItem*)m_pPressedButton->GetItem();
				if(pItem) {
					if( pItem->GetItemType() == ITEMTYPE_TALISMAN || pItem->GetItemType() == ITEMTYPE_PERIOD_TALISMAN_EX ) {
						pCharStatusDlg->ShowTalismanSlotEffciency(false);
					}
				}*/
			}
#endif

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
			if (GetInterface().IsShowCashShopDlg())
			{
				if (GetInterface().GetCashShopRefundInven() == NULL || GetInterface().GetCashShopRefundInven()->IsShow() == false)
					return;
			}
#else
			if (GetInterface().IsShowCashShopDlg())
				return;
#endif

			if(!CDnActor::s_hLocalActor)
				return;

			CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if( pLocalPlayerActor && pLocalPlayerActor->IsObserver() )
				return;

#ifdef PRE_ADD_SECONDARY_SKILL
			// ����, �丮 ���� ��� ������ ��� �Ұ�
			if( ( CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing() ) || 
				( CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking() ) )
				return;
#endif // PRE_ADD_SECONDARY_SKILL

			if( pDragButton == NULL )
			{
				CDnItem *pItem = static_cast<CDnItem *>(m_pPressedButton->GetItem());
				if( !pItem ) return;

				CDnStoreTabDlg *pStoreDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
				CDnPGStorageTabDlg* pPGStorageTabDlg = static_cast<CDnPGStorageTabDlg*>(GetInterface().GetMainMenuDialog( CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG ));
#ifdef PRE_ADD_SERVER_WAREHOUSE
				CDnPlayerStorageDlg* pStorageDlg = static_cast<CDnPlayerStorageDlg*>(pPGStorageTabDlg->GetStorageDlg(StorageUIDef::eTAB_PLAYER));
				CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageTabDlg->GetStorageDlg(StorageUIDef::eTAB_GUILD));
#else
				CDnPlayerStorageDlg* pStorageDlg = static_cast<CDnPlayerStorageDlg*>(pPGStorageTabDlg->GetStorageDlg( CDnPGStorageTabDlg::PLAYER_STORAGE_TAB ));
				CDnGuildStorageDlgNew* pGuildStorageDlg = static_cast<CDnGuildStorageDlgNew*>(pPGStorageTabDlg->GetStorageDlg( CDnPGStorageTabDlg::GUILD_STORAGE_TAB ));
#endif
				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
				CDnItemDisjointDlg *pItemDisjointDlg = (CDnItemDisjointDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_DISJOINT_DIALOG );
				CDnPlateMainDlg *pPlateMainDlg = (CDnPlateMainDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::PLATE_MAIN_DIALOG );
				CDnItemUpgradeDlg *pItemUpgradeDlg = (CDnItemUpgradeDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_UPGRADE_DIALOG );
#ifdef PRE_ADD_EXCHANGE_ENCHANT
				CDnItemUpgradeExchangeDlg* pItemUpgradeExchangeDlg = (CDnItemUpgradeExchangeDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_UPGRADE_EXCHANGE_DIALOG);
#endif
				CDnItemCompoundTabDlg *pItemCompoundDlg = (CDnItemCompoundTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND_DIALOG );
				CDnMailDlg* pMailDlg = (CDnMailDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MAIL_DIALOG);
				CDnMarketTabDlg* pMarketTabDlg = (CDnMarketTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG);
				CDnMarketPriceDlg* pMarketPriceDlg = (CDnMarketPriceDlg*) pMarketTabDlg ? pMarketTabDlg->GetPriceDialog() : NULL;
				CDnMarketBuyDlg* pMarketBuyDlg = (CDnMarketBuyDlg*) pMarketTabDlg ? pMarketTabDlg->GetBuyDialog() : NULL;
				CDnItemSealDlg *pItemSealDlg = GetInterface().GetItemSealDlg();
				CDnPotentialJewelDlg *pItemPotentialDlg = GetInterface().GetItemPotentialDlg();
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
				CDnPotentialJewelCleanDlg *pItemPotentialCleanDlg = GetInterface().GetItemPotentialCleanDlg(); 
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
				CDnCostumeMixDlg* pCostumeMixDlg = static_cast<CDnCostumeMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUMEMIX_DIALOG));
				CDnChatRoomDlg *pChatRoomDlg = GetInterface().GetChatRoomDlg();
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
				CDnCashShopRefundInvenDlg* pRefundDlg = GetInterface().GetCashShopRefundInven();
#endif
				CDnLifeStorageDlg * pLifeStorage = (CDnLifeStorageDlg *)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::LIFESTORAGE_DIALOG);
				CDnEnchantJewelDlg *pEnchantJewelDlg = GetInterface().GetEnchantJewelDlg();
				CDnCostumeDesignMixDlg* pCostumeMergeDlg = static_cast<CDnCostumeDesignMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUME_DMIX_DIALOG));
#ifdef PRE_ADD_COSRANDMIX
				CDnCostumeRandomMixDlg* pCostumeRandomMixDlg = static_cast<CDnCostumeRandomMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUME_RANDOMMIX_DIALOG));
#endif
#ifdef PRE_ADD_SERVER_WAREHOUSE
				CDnWorldServerStorageInventoryDlg* pWorldServerStorageInvenDlg = static_cast<CDnWorldServerStorageInventoryDlg*>(pPGStorageTabDlg->GetStorageDlg(StorageUIDef::eTAB_WORLDSERVER_NORMAL));
				CDnWorldServerStorageCashInventoryDlg* pWorldServerStorageCashInvenDlg = static_cast<CDnWorldServerStorageCashInventoryDlg*>(pPGStorageTabDlg->GetStorageDlg(StorageUIDef::eTAB_WORLDSERVER_CASH));
#endif
#if defined(PRE_ADD_REMOVE_PREFIX)
				CDnRemovePrefixDlg* pRemovePrefixDlg = GetInterface().GetRemovePrefixDlg();
#endif
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
				CDnPotentialTransferDlg *pPotentialTransferDlg = GetInterface().GetItemPotentialTransferDlg();
#endif
				CDnPetOptionDlg*  pPetOptionDlg = NULL;
				CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
				if( pCharStatusDlg )
				{
					CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
					if( pCharPetDlg )
					{
						pPetOptionDlg = pCharPetDlg->GetPetOptionDlg();
					}
				}

#ifdef PRE_ADD_DRAGON_GEM
				const CDnDragonGemEquipDlg*	 pDragonGemEquipDlg  = GetInterface().GetDragonGemEquipDlg();
				const CDnDragonGemRemoveDlg* pDragonGemRemoveDlg = GetInterface().GetDragonGemRemoveDlg();
#endif

				if ( uMsg == ( WM_RBUTTONUP | VK_SHIFT) )
				{
					// ����Ʈ ��Ŭ���� ��� ��� ���� ������� ���� �ȱ�.
					if( pStoreDlg && pStoreDlg->IsShow() )
					{
						if( pItem->GetItemType() != ITEMTYPE_QUEST )
						{
							GetTradeTask().GetTradeItem().RequestShopSell(*pItem, pItem->GetOverlapCount());//pItem->GetSlotIndex(), pItem->GetOverlapCount() );
						}
					}
					else if (IsOnMailMode(pMailDlg))
					{
						if( pMailDlg->IsShowWriteDlg())
						{
							pMailDlg->AttachItemToWriteDlg(m_pPressedButton, false);
						}
					}
					else if (pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog())
					{
						pInvenDlg->AddItemToPrivateMarketTradeList(m_pPressedButton, false);
					}
					else if (pStorageDlg && pStorageDlg->IsShow())
					{
						if( pItem->GetItemType() != ITEMTYPE_QUEST )
						{
							pStorageDlg->MoveItemToStorage(m_pPressedButton, false);
						}
					}
					else if (pGuildStorageDlg && pGuildStorageDlg->IsShow())
					{
						if( pItem->GetItemType() != ITEMTYPE_QUEST )
						{
							pGuildStorageDlg->MoveItemToStorage(m_pPressedButton, false);
						}
					}
#ifdef PRE_ADD_SERVER_WAREHOUSE
					else if (pWorldServerStorageInvenDlg && pWorldServerStorageInvenDlg->IsShow())
					{
						if (pItem->GetItemType() != ITEMTYPE_QUEST)
						{
							StorageUIDef::eError err = GetItemTask().IsEnableStoreOnWorldServerStorage( pItem );
							if (err != StorageUIDef::eERR_NONE)
							{
								GetInterface().MessageBox((int)err);
								return;
							}
							else
							{
								pWorldServerStorageInvenDlg->MoveItemToStorage(m_pPressedButton, true);
							}
						}
					}
					else if (pWorldServerStorageCashInvenDlg && pWorldServerStorageCashInvenDlg->IsShow())
					{
						if (pItem->GetItemType() != ITEMTYPE_QUEST)
						{
							StorageUIDef::eError err = GetItemTask().IsEnableStoreOnWorldServerStorage( pItem );
							if (err != StorageUIDef::eERR_NONE)
							{
								GetInterface().MessageBox((int)err);
								return;
							}
							else
							{
								pWorldServerStorageCashInvenDlg->MoveItemToStorage(m_pPressedButton, true);
							}
						}
					}
#endif
					else if (pMarketPriceDlg && pMarketPriceDlg->IsShow())
					{
						pMarketPriceDlg->PickUpMarketItem( m_pPressedButton , false);
					}
					else if (pItem->GetOverlapCount() > 1)
					{
						if ((pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog()) ||
							(pStoreDlg && pStoreDlg->IsShow()) ||
							(pStorageDlg && pStorageDlg->IsShow()) ||
							(pItemSealDlg && pItemSealDlg->IsShow()) ||
							(pItemDisjointDlg && pItemDisjointDlg->IsShow() ) || 
							(GetInterface().IsShowItemUnsealDialog()) ||
							(IsOnMailMode(pMailDlg)))
						{
							HandleItemClick(pItem, pMailDlg, pInvenDlg);
							return;
						}

						if( GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() || m_SlotType != ST_INVENTORY)
							return;

						// Note : ������ ������.
						//
						m_pInvenSepDlg->SetEditValue( 1, pItem->GetOverlapCount()-1 );
						m_pInvenSepDlg->Show( true );
					}
				}
				else if( uMsg == WM_RBUTTONUP )
				{
#ifdef PRE_ADD_CASHREMOVE
					// ĳ�����Ŵ�� ���̸� �����ߴ�.
					if( ITEM_SLOT_TYPE::ST_INVENTORY_CASHREMOVE == this->GetSlotType() )
					{
						this->GetParentDialog()->ProcessCommand( EVENT_BUTTON_RCLICKED, false, m_pPressedButton );
						return;
					}
#endif

					if (CDnInterface::GetInstance().IsOpenWorldMsgDialog())
						return;
					if (CDnTradeTask::IsActive() && ( CDnTradeTask::GetInstance().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() ) )
						return;
#ifdef PRE_ADD_GACHA_JAPAN
					// ���� ��í�� ���̾�αװ� �� �ִٸ� �κ����� ������ ��� �� ��� ������ �ȵǰ� �� ���´�.
					if( GetInterface().IsShowGachaShopDlg() )
						return;
#endif // PRE_ADD_GACHA_JAPAN

#ifdef PRE_ADD_DONATION // #55024
					// ���â�� �������� �ߴ�.
					if( GetInterface().IsOpenDonation() )
						return;
#endif

					if( pStoreDlg && pStoreDlg->IsShow() )
					{
						if( pItem->GetItemType() != ITEMTYPE_QUEST )
						{
							// �븻����̸鼭 ��ø �������� �ƴ϶��, �׳� �ٷ� �Ǵ�.(1��¥�� ���� or �븻 ��� �����۵�) 
							if( (pItem->GetItemRank() == ITEMRANK_D) && (pItem->GetOverlapCount() == 1) && pItem->GetEnchantLevel() == 0 )
							{
								GetTradeTask().GetTradeItem().RequestShopSell(*pItem, 1);//pItem->GetSlotIndex(), 1 );
							}
							else
							{
								GetTradeTask().GetTradeItem().SetTradeSellItem( pItem );
							}

							//if ( uMsg & 0x0010 )
						}
					}
					else if( pStorageDlg && pStorageDlg->IsShow() )
					{
						if( pItem->GetItemType() != ITEMTYPE_QUEST )
						{
							pStorageDlg->MoveItemToStorage(m_pPressedButton, true);
						}
					}
					else if( pLifeStorage && pLifeStorage->IsShow() )
					{}
					else if( pGuildStorageDlg && pGuildStorageDlg->IsShow() )
					{
						if( pItem->GetItemType() != ITEMTYPE_QUEST )
						{
							pGuildStorageDlg->MoveItemToStorage(m_pPressedButton, true);
						}
					}
#ifdef PRE_ADD_SERVER_WAREHOUSE
					else if (pWorldServerStorageInvenDlg && pWorldServerStorageInvenDlg->IsShow())
					{
						if (pItem->GetItemType() != ITEMTYPE_QUEST)
						{
							StorageUIDef::eError err = GetItemTask().IsEnableStoreOnWorldServerStorage( pItem );
							if (err != StorageUIDef::eERR_NONE)
							{
								GetInterface().MessageBox((int)err);
								return;
							}
							else
							{
								pWorldServerStorageInvenDlg->MoveItemToStorage(m_pPressedButton, false);
							}
						}
					}
					else if (pWorldServerStorageCashInvenDlg && pWorldServerStorageCashInvenDlg->IsShow())
					{
						if (pItem->GetItemType() != ITEMTYPE_QUEST)
						{
							StorageUIDef::eError err = GetItemTask().IsEnableStoreOnWorldServerStorage( pItem );
							if (err != StorageUIDef::eERR_NONE)
							{
								GetInterface().MessageBox((int)err);
								return;
							}
							else
							{
								pWorldServerStorageCashInvenDlg->MoveItemToStorage(m_pPressedButton, false);
							}
						}
					}

#endif
					else if( pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog() )
					{
						// Note : ���ΰŷ����̶�� ���ΰŷ� ó���� �ѱ��.
						//
						pInvenDlg->AddItemToPrivateMarketTradeList(m_pPressedButton, true);
					}
					else if( pItemDisjointDlg && pItemDisjointDlg->IsShow() )
					{
						// ����â�� �ٷ� ���
						pItemDisjointDlg->AddItemToList( m_pPressedButton );
					}
					else if( pPlateMainDlg && pPlateMainDlg->IsShow() )
					{
						// ���庸�� â�� �÷���Ʈ ���
						pPlateMainDlg->SetPlateItem( m_pPressedButton );
					}
					else if( pItemUpgradeDlg && pItemUpgradeDlg->IsShow() ) 
					{
						 // #70609 ���� �̽� 
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT // ���Կ� �������� �ִ� ��� ó�� 
						CDnItemSlotButton* pCurrentForceSlot = pItemUpgradeDlg->GetCurrentForceSlot();
						CDnItem* pCurrentitem = static_cast<CDnItem*>(pCurrentForceSlot->GetItem());
						pItemUpgradeDlg->SetCurrentInvenSlot( m_pPressedButton );

						if( ! pItemUpgradeDlg->IsUpgradeItem( m_pPressedButton )  )
							return;

						if( pCurrentForceSlot && pCurrentitem )
						{
							int nSlotIndex = pCurrentitem->GetSlotIndex();
							CDnSlotButton* pPreEquipitem = static_cast<CDnSlotButton*> ( pItemUpgradeDlg->GetEquipSlotButton( nSlotIndex ) );
							if( pPreEquipitem ) pPreEquipitem->SetRegist( false );
							if( pItemUpgradeDlg->GetPreInvenSlot() && pItemUpgradeDlg->GetCurrentInvenSlot() && 
								pItemUpgradeDlg->GetPreInvenSlot()->GetItemSlotIndex() != pItemUpgradeDlg->GetCurrentInvenSlot()->GetItemSlotIndex() )
							{
								pItemUpgradeDlg->GetPreInvenSlot()->SetRegist( false ); 
							} 
							else if( pItemUpgradeDlg->GetCurrentInvenSlot() )
							{
								pItemUpgradeDlg->GetCurrentInvenSlot()->SetRegist( false );
							}

							pItemUpgradeDlg->SafeDelForceSlotItem();	
							pItemUpgradeDlg->SetUpgradeItem( m_pPressedButton );	// ���� ������ ���Ӱ� ���� 

						}
						else
						{
							pItemUpgradeDlg->SafeDelForceSlotItem();
							pItemUpgradeDlg->SetUpgradeItem( m_pPressedButton );
						}
						pItemUpgradeDlg->SetPreInvenSlot( pItemUpgradeDlg->GetCurrentInvenSlot() );						
#else
						pItemUpgradeDlg->SetUpgradeItem( m_pPressedButton );
#endif 

					}
#ifdef PRE_ADD_EXCHANGE_ENCHANT
					else if (pItemUpgradeExchangeDlg && pItemUpgradeExchangeDlg->IsShow())
					{
						pItemUpgradeExchangeDlg->AddAttachItem(m_pPressedButton);
					}
#endif
					else if ( pMarketPriceDlg && pMarketPriceDlg->IsShow() ) 
					{
						// ���λ��� ������ ���
						pMarketPriceDlg->PickUpMarketItem( m_pPressedButton , true);
					}
					else if( pItemCompoundDlg && pItemCompoundDlg->IsShow() )
					{
						// �ƹ��͵� �ȵǴ°� ���� ��.
						// �����Ǹ� ĳ����â ������..
					}
					else if( pMarketTabDlg && pMarketTabDlg->IsShow() )
					{
						// ���â �ȶ��־ ���λ��� ���� ��Ŭ�� �ȵǰ�....
					}					
					else if( pItemSealDlg && pItemSealDlg->IsShow() )
					{
						// ����â�� ���
						pItemSealDlg->SetSealItem( m_pPressedButton );
					}
					else if (pCostumeMixDlg && pCostumeMixDlg->IsShow())
					{
						pCostumeMixDlg->AddAttachItem(m_pPressedButton);
					}
					else if (pCostumeMergeDlg && pCostumeMergeDlg->IsShow())
					{
						pCostumeMergeDlg->AddAttachItem(m_pPressedButton);
					}
#ifdef PRE_ADD_COSRANDMIX
					else if (pCostumeRandomMixDlg && pCostumeRandomMixDlg->IsShow())
					{
						pCostumeRandomMixDlg->AddAttachItem(m_pPressedButton);
					}
#endif
					else if( pChatRoomDlg && pChatRoomDlg->IsShow() )
					{

					}
					else if( pItemPotentialDlg && pItemPotentialDlg->IsShow() ) 
					{
						pItemPotentialDlg->SetPotentialItem( m_pPressedButton );
					}
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
					else if( pItemPotentialCleanDlg && pItemPotentialCleanDlg->IsShow() )
					{
						pItemPotentialCleanDlg->SetPotentialItem(m_pPressedButton);
					}
#endif
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
					else if ( pRefundDlg && pRefundDlg->IsShow() )
					{
						pRefundDlg->Withdraw(m_pPressedButton);
					}
#endif
					else if( pEnchantJewelDlg && pEnchantJewelDlg->IsShow() )
					{
						pEnchantJewelDlg->SetEnchantJewelItem( m_pPressedButton );
					}
#if defined(PRE_ADD_REMOVE_PREFIX)
					else if ( pRemovePrefixDlg && pRemovePrefixDlg->IsShow() )
					{
						pRemovePrefixDlg->SetItem(m_pPressedButton);
					}

#ifdef PRE_ADD_AUTOUNPACK
					// �ŷ¾����� �ڵ�����â.
					else if( GetInterface().IsOpenAutoUnPackDlg() )
					{						
						GetInterface().GetAutoUnPackDlg()->RegisterItem( pItem );
					}
#endif
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
					else if( pPotentialTransferDlg && pPotentialTransferDlg->IsShow() )
					{
						pPotentialTransferDlg->OnRButtonClick( m_pPressedButton );
					}
#endif
					else if( pPetOptionDlg && pPetOptionDlg->IsShow() && m_pPressedButton->GetItem() )
					{
						pPetOptionDlg->SetPetFoodItem( m_pPressedButton->GetItem()->GetClassID() );
					}
#endif // PRE_ADD_REMOVE_PREFIX
					else 
					{
						bool bProcessed = false;
						if (IsOnMailMode(pMailDlg))
						{
#ifdef PRE_ADD_ONESTOP_TRADECHECK
							if (pMailDlg->IsShowWriteDlg())
							{
								pMailDlg->AttachItemToWriteDlg(m_pPressedButton, true);
								bProcessed = true;
							}
							else
							{
								if (pItem->IsCashItem())
								{
									GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1993 ), true); // UISTRING : ������ �̿��߿��� ĳ�þ������� ����� �� �����ϴ�.
									bProcessed = true;
								}
								else if( ( CDnItem::IsEffectSkillItem( pItem->GetItemType() ) == true ) )
								{
									// #50016 ������ �̿��߿��� �ٿ� �������� ����� �� ����.
									bProcessed = true;
								}
							}
#else
							if (pItem->IsCashItem())
							{
								GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1993 ), true); // UISTRING : ������ �̿��߿��� ĳ�þ������� ����� �� �����ϴ�.
								bProcessed = true;
							}

							if( ( CDnItem::IsEffectSkillItem( pItem->GetItemType() ) == true ) )
							{
								// #50016 ������ �̿��߿��� �ٿ� �������� ����� �� ����.
								bProcessed = true;
							}
	
							if (pMailDlg->IsShowWriteDlg())
							{
								pMailDlg->AttachItemToWriteDlg(m_pPressedButton, true);
								bProcessed = true;
							}
#endif
							// #53422 [�ý���] ������ ���¿��� �Ϲݾ�����(�ָӴ� ��) ���� �� ���� �߻�.
							// - �Ϲݾ����� ������.
							if( ITEM_SLOT_TYPE::ST_INVENTORY == this->GetSlotType() )
							{
								if( false == pMailDlg->IsShowWriteDlg() )
								{
									GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1993 ), true); // "������ �̿��߿��� �������� ����� �� �����ϴ�."
								}

								bProcessed = true;
							}
						}
						else if (GetInterface().GetMainMenuDialog()->IsWithDrawGlyphOpen())
						{
							bProcessed = true;
							if (pItem->GetItemType() == ITEMTYPE_GLYPH)
								bProcessed = false;

							if (bProcessed)
								GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9040), true); // UISTRING : ����ȸ�� �̿��߿��� �������� ����� �� �����ϴ�.
						}
						else
						{
							if (pItem->IsSoulbBound() == false)
							{
								GetInterface().OpenItemUnsealDialog(pItem, m_pPressedButton);
								bProcessed = true;
							}
						}

						if (m_pPressedButton->GetItemCondition() == MIInventoryItem::Unusable)
						{
							bProcessed = true;
						}

						if (bProcessed == false)
						{
							if( pItem->GetItemCondition() != MIInventoryItem::Usable ) return;
							switch( pItem->GetItemType() )
							{
							case ITEMTYPE_FATIGUEUP:
								{
									const int nPlayerFatigue = GetItemTask().GetFatigue( CDnItemTask::eDAILYFTG );
									const int nMaxFatigue = GetItemTask().GetMaxFatigue( CDnItemTask::eDAILYFTG );
									if( nPlayerFatigue >= nMaxFatigue )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 195 ) );	// UISTRING : �Ƿε��� �� �̻� �ø� �� �����ϴ�.
										return;
									}

									if (IsOnMailMode(pMailDlg))
										pMailDlg->LockDlgs(true);
									char cType = 0;
									cType = ( pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);

									if( pPlayerActor && pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), cType ) )
									{
										// �������� ����Ҷ� ������ �Ƚ����� �Ҹ�����.
										//CEtSoundEngine::GetInstance().PlaySound( "2D", ((CDnItem*)pItem)->GetUseSoundIndex() );
									}
									else
									{
										if (IsOnMailMode(pMailDlg))
											pMailDlg->LockDlgs(false);
									}
								}
								break;
#if defined(PRE_ADD_EXPUP_ITEM)
							case ITEMTYPE_EXPUP_ITEM:
								{
									int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor && pPlayerActor->GetLevel() == nLimitLevel )
									{		
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114068 ) );	// UISTRING : ����ġ�� �� �̻� �ø� �� �����ϴ�.
										break;
									}

									if (IsOnMailMode(pMailDlg))
										pMailDlg->LockDlgs(true);
									char cType = 0;
									cType = ( pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;

									if( pPlayerActor && pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), cType ) )
									{
										// �������� ����Ҷ� ������ �Ƚ����� �Ҹ�����.
										//CEtSoundEngine::GetInstance().PlaySound( "2D", ((CDnItem*)pItem)->GetUseSoundIndex() );
									}
									else
									{
										if (IsOnMailMode(pMailDlg))
											pMailDlg->LockDlgs(false);
									}
								}
								break;
#endif

							case ITEMTYPE_ULTIMATEFATIGUEUP:
								{
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( NULL == pPlayerActor )
										break;

									const int nPlayerLevel = pPlayerActor->GetLevel();
									const int nItemLevelLimit = pItem->GetTypeParam( 1 );
									if(( nItemLevelLimit > 0 ) && ( nPlayerLevel >= nItemLevelLimit ))
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114069 ) );	// UISTRING : ������ ���� �������� ����� �� �����ϴ�.
										return;
									}

									const int nPlayerFatigue = GetItemTask().GetFatigue( CDnItemTask::eDAILYFTG );
									const int nMaxFatigue = GetItemTask().GetMaxFatigue( CDnItemTask::eDAILYFTG );
									if( nPlayerFatigue >= nMaxFatigue )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 195 ) );	// UISTRING : �Ƿε��� �� �̻� �ø� �� �����ϴ�.
										return;
									}

									if (IsOnMailMode(pMailDlg))
										pMailDlg->LockDlgs(true);
									char cType = 0;
									cType = ( pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;

									if( pPlayerActor && pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), cType ) )
									{
										// �������� ����Ҷ� ������ �Ƚ����� �Ҹ�����.
										//CEtSoundEngine::GetInstance().PlaySound( "2D", ((CDnItem*)pItem)->GetUseSoundIndex() );
									}
									else
									{
										if (IsOnMailMode(pMailDlg))
											pMailDlg->LockDlgs(false);
									}

								}
								break;

							case ITEMTYPE_GLYPH:
							case ITEMTYPE_WEAPON:
							case ITEMTYPE_PARTS:
#ifdef PRE_ADD_TALISMAN_SYSTEM
							case ITEMTYPE_TALISMAN:
#endif
								{
									if (IsOnMailMode(pMailDlg))
										break;

									// Note : ������ ��������.
									//
									CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
									if( pCharStatusDlg )
									{
										bool bEquip = pCharStatusDlg->SetEquipItemFromInven( pItem->GetSlotIndex(), pItem, m_pPressedButton );

										if( bEquip ) {
											CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
											if( pMainMenuDlg ) {
												pMainMenuDlg->OpenCharStatusDialog();
											}
										}
										pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage() );
									}
								}
								break;
							case ITEMTYPE_NORMAL:	
							case ITEMTYPE_COOKING:
							case ITEMTYPE_COSTUMEMIX:
							case ITEMTYPE_COSTUMEDESIGN_MIX:
#ifdef PRE_ADD_COSRANDMIX
							case ITEMTYPE_COSTUMERANDOM_MIX:
#endif
							case ITEMTYPE_SOURCE:
							case ITEMTYPE_GLOBAL_PARTY_BUFF:
							case ITEMTYPE_TRANSFORMPOTION:
							case ITEMTYPE_PET_FOOD:
							case ITEMTYPE_HEAD_SCALE_POTION:
								{
									if (IsOnMailMode(pMailDlg))
										pMailDlg->LockDlgs(true);

									// Note : ������ �������.
									if( ( CDnItem::IsEffectSkillItem( pItem->GetItemType() ) == true ) )
									{
										DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TITEM );
										if( pSox )
										{
											int nUseLevelLimit = pSox->GetFieldFromLablePtr( pItem->GetClassID(), "_UseLevelLimit" )->GetInteger();
											CDnPlayerActor* pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
											if( pPlayerActor && nUseLevelLimit > 0 && nUseLevelLimit < pPlayerActor->GetLevel() )
											{
												GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7520 ) );
												break;
											}
										}

										bool bUsableSourceItem = ( ( CDnItem::IsEffectSkillItem( pItem->GetItemType() ) == true ) && ( false == IsOnMailMode(pMailDlg ) ) );
										if( false == bUsableSourceItem )
											return;
									}

									if (CDnCashShopTask::GetInstance().IsOpenCashShop())
										return;

#ifdef PRE_ADD_COSRANDMIX
									if (ReadyCostumeMix(pItem) == false)
										break;
#else
										if (pItem->GetItemType() == ITEMTYPE_COSTUMEMIX ||
											pItem->GetItemType() == ITEMTYPE_COSTUMEDESIGN_MIX)
										{
											CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
											bool bEnable = true;
											if (pItem->GetItemType() == ITEMTYPE_COSTUMEMIX)			bEnable = dataMgr.IsEnableCostumeMix();
											else if (pItem->GetItemType() == ITEMTYPE_COSTUMEDESIGN_MIX) bEnable = dataMgr.IsEnableCostumeDesignMix();

											if (bEnable == false)
											{
												GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 130310 ) );	// UISTRING : �ڽ�Ƭ �ռ� �� ������ �߻��߽��ϴ�.
												break;
											}

											if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage)
											{
												GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 130316 ) );	// UISTRING : �ڽ�Ƭ �ռ��� ���������� �����մϴ�.
												break;
											}

											if (GetInterface().IsShowItemUnsealDialog())
											{
												GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3213));	// UISTRING : ���� ���� �ÿ��� �ͽ��ڽ��� ����� �� �����ϴ�.
												break;
											}

											if (CDnTradeTask::IsActive())
												CDnTradeTask::GetInstance().GetTradePrivateMarket().ClearTradeUserInfoList();

											if (CDnItemTask::IsActive() && CDnItemTask::GetInstance().ReadyItemCostumeMixOpen(pItem->GetItemType() == ITEMTYPE_COSTUMEDESIGN_MIX) == false)
												break;
										}
#endif // PRE_ADD_COSRANDMIX

									if( pItem->GetSkillID() == 0 && pItem->GetItemType() != ITEMTYPE_COSTUMEMIX 
										&& pItem->GetItemType() != ITEMTYPE_COSTUMEDESIGN_MIX
#ifdef PRE_ADD_COSRANDMIX
										&& pItem->GetItemType() != ITEMTYPE_COSTUMERANDOM_MIX
#endif
#ifdef PRE_ADD_HEAD_SCALE_POTION
										&& pItem->GetItemType() != ITEMTYPE_HEAD_SCALE_POTION
#endif
										)
									{
										if (IsOnMailMode(pMailDlg))
											pMailDlg->LockDlgs(false);
										break;
									}

									char cType = 0;
									cType = ( pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor && pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), cType ) )
									{

									}
									else
									{
										if (IsOnMailMode(pMailDlg))
											pMailDlg->LockDlgs(false);
#ifdef PRE_FIX_COSMIX_NORESPONSE_ON_ERROR
										if (pItem && IsOnCostumeMixMode(*pItem))
											CancelCostumeMixReady(*pItem);
#endif
									}
								}
								break;
							case ITEMTYPE_HAIRDYE:		// SkillID 0 �̾ ��밡��.
							case ITEMTYPE_EYEDYE:
							case ITEMTYPE_HAIRDRESS:
							case ITEMTYPE_SKINDYE:
							case ITEMTYPE_FACIAL:
								{
									if( m_pAskLookChgDlg->SetItem( pItem ) ) {
										m_pAskLookChgDlg->Show( true );
									}
								}
								break;
							case ITEMTYPE_RANDOM:
							case ITEMTYPE_SKILLBOOK:
								{
									if (IsOnMailMode(pMailDlg))
										pMailDlg->LockDlgs(true);

									// ���� ������ ���.
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor ) {
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );
									}
									else {
										if (IsOnMailMode(pMailDlg))
											pMailDlg->LockDlgs(false);
									}
								}
								break;
							case ITEMTYPE_REMOTE_ENCHANT:
							case ITEMTYPE_REMOTE_ITEMCOMPOUND:
							case ITEMTYPE_REMOTE_WAREHOUSE:
								{
									if(CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
										break;

									if (IsOnMailMode(pMailDlg))
										break;

									// ���� ������ ���.
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor ) {
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );
									}
								}
								break;
							case ITEMTYPE_SEAL:
								{
									GetInterface().ShowItemSealDialog( true, pItem );
								}
								break;
							case ITEMTYPE_WORLDMSG:
								{
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor )
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), ITEMPOSITION_CASHINVEN );
								}
								break;

							case ITEMTYPE_RESET_SKILL:
							case ITEMTYPE_INFINITY_RESET_SKILL:
								{
									if(pItem && pItem->GetItemType() == ITEMTYPE_INFINITY_RESET_SKILL && pItem->IsEternityItem() == false && pItem->GetExpireDate()) // �Ⱓ�� �������̸� , �Ⱓ�� �����Ǿ��ִ� ���.
									{
										const time_t tNowTime = CSyncTimer::GetInstance().GetCurTime();
										const time_t tExpireDate = *pItem->GetExpireDate();
										if( tNowTime > tExpireDate ) // ���Ⱓ�� ������� ����.
										{
											GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1118 ) ); // �ش�������� �Ⱓ�� �������� , ������ �����Դϴ�. ��� �޼���. ǥ����
											return;
										}
									}
	
									// ĳ�þ����� ��ų���·� ������. ���������� ��� ����
									if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100301 ) );
										return;
									}

									// ��� ������ �������� �ƴ��� �������� ������ �´�.
									// �ű⼭ Ȯ��â�� ���� Ȯ���� ������ �� ��ų���� ��û ��Ŷ�� ������.
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor )
#ifdef PRE_MOD_SKILLRESETITEM
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );
#else
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), ITEMPOSITION_CASHINVEN );
#endif
								}
								break;

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
							case ITEMTYPE_CHANGEJOB:
								{
									// ���� �������� ���������� ��� ������.
									if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100301 ) );
										return;
									}

									if (CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().GetPartyRole() != CDnPartyTask::SINGLE)
									{
										GetInterface().ServerMessageBox( 463 );	// ��Ƽ ���¿����� ����� �Ұ����� ������ �Դϴ�.
										return;
									}

									// �ѹ��̶� ������ �� ���¿��� ������ ��� ����.
									vector<int> vlJobHistory;
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor )
										pPlayerActor->GetJobHistory( vlJobHistory );

#ifdef PRE_FIX_CHANGEJOBITEM_CHECK_NEEDJOB
									std::vector<int> enableJobList;
									CommonUtil::GetItemJobUsable(enableJobList, pItem->GetClassID());

									bool bEnableJob = false;
									std::vector<int>::const_iterator iter = std::find(enableJobList.begin(), enableJobList.end(), 0);
									if (iter != enableJobList.end())
									{
										bEnableJob = true;
									}
									else
									{
										int i = 0;
										for (; i < (int)vlJobHistory.size(); ++i)
										{
											int job = vlJobHistory[i];
											std::vector<int>::const_iterator iter = std::find(enableJobList.begin(), enableJobList.end(), job);
											if (iter != enableJobList.end())
											{
												bEnableJob = true;
												break;
											}
										}
									}

									if (bEnableJob == false)
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4851), MB_OK ); // UISTRING �߰� ��û by kalliste
										return;
									}
#endif // PRE_FIX_CHANGEJOBITEM_CHECK_NEEDJOB

									if( 1 < (int)vlJobHistory.size() )
									{
										GetInterface().ShowChangeJobDialog( true, pItem );
									}
									else
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4851), MB_OK );
									}
								}
								break;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
							case ITEMTYPE_NPC_VOICEPLAYER:
								{
									// npc ��Ҹ��� ������ش�. ���� ��� �Ұ����� ���� ���� ���� �� ����.
									
									// ��Ҹ� ����� �ĺ��� ���� ���ϵ� ��Ƶд�.
									CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
									pLocalPlayerActor->UseNpcVoicePlayer( pItem );
								}
								break;
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
							case ITEMTYPE_POTENTIAL_JEWEL:
								{
#if defined(PRE_ADD_REMOVE_PREFIX)
									CDnPotentialJewel* pPotentialJewel = static_cast<CDnPotentialJewel*>(pItem);
									if (pPotentialJewel && pPotentialJewel->IsSuffix() == true)
										GetInterface().ShowRemovePrefixDialog(true, pItem);
									else
										GetInterface().ShowItemPotentialDialog( true, pItem );
#else
									GetInterface().ShowItemPotentialDialog( true, pItem );
#endif // PRE_ADD_REMOVE_PREFIX
								}
								break;
							case ITEMTYPE_PLATE :
								{
								}
								break;
							case ITEMTYPE_CHARM:
#if defined( PRE_ADD_EASYGAMECASH )
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
							case ITEMTYPE_CHARMRANDOM:
#endif
							case ITEMTYPE_ALLGIVECHARM:
#endif
								{
									CDnPlayerActor* pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor && pItem->GetLevelLimit() > pPlayerActor->GetLevel() )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ) ); 
										break;
									}									
#ifdef PRE_ADD_AUTOUNPACK 
									// �ڵ�����â�� �����ִ�.									
									if( GetInterface().IsOpenAutoUnPackDlg() )
									{
										GetInterface().GetAutoUnPackDlg()->RegisterItem( pItem );
									}
									// �ڵ�����â�� ������������ ������ �����ϰ� ó��.
									else
									{
										// ���������۵� ���â�̶� ���������� ����߿� ��� ���Ұ� ����.
										GetInterface().ShowCharmItemKeyDialog( true, pItem );
									}
#else

									// ���������۵� ���â�̶� ���������� ����߿� ��� ���Ұ� ����.
									GetInterface().ShowCharmItemKeyDialog( true, pItem );
#endif
								}
								break;
							case ITEMTYPE_PET:
								{
									if( IsOnMailMode( pMailDlg ) )
										break;
#ifdef PRE_ADD_PET_EXTEND_PERIOD
									CDnPetExtendPeriodDlg* pPetExtendPeriodDlg = GetInterface().GetPetExtendPeriodDlg();
									if( pPetExtendPeriodDlg && pPetExtendPeriodDlg->IsShow() )
									{
										pPetExtendPeriodDlg->SetPetItem( m_pPressedButton );
										return;
									}
									// ���� �����۾ƴ� ��� ����Ⱓ�� ������ �ڵ����� ���Ǹ� ����â���� �̵�
									TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pItem->GetSerialID() );
									if( pPetCompact == NULL ) return;
									const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
									if( !pItem->IsEternityItem() && pNowTime >= pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )
									{
										if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
										{
											GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9281 ), false );
											return;
										}

										GetInterface().ShowBuyPetExtendPeriodItemDlg( true, pItem->GetSerialID() );
									}
									else
									{
										if( CDnPetTask::IsActive() )
											GetPetTask().UsePetItem( pItem );
									}
#else // PRE_ADD_PET_EXTEND_PERIOD
									CDnPetExtendPeriodDlg* pPetExtendPeriodDlg = GetInterface().GetPetExtendPeriodDlg();
									if( pPetExtendPeriodDlg && pPetExtendPeriodDlg->IsShow() )
									{
										pPetExtendPeriodDlg->SetPetItem( m_pPressedButton );
										return;
									}
									else
									{
										if( CDnPetTask::IsActive() )
											GetPetTask().UsePetItem( pItem );
									}
#endif // PRE_ADD_PET_EXTEND_PERIOD
								}
								break;
							case ITEMTYPE_PETPARTS:
								{
									if( IsOnMailMode( pMailDlg ) )
										break;

									CDnLocalPlayerActor* plocalActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

									if( pItem == NULL || plocalActor == NULL )
										return;

									CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
									if( pCharStatusDlg )
									{
										DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
										if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem( pItem->GetClassID() ) )
											return;

										int nPartsType = -1;

										nPartsType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();

										bool bEquip = false;
										bEquip = pCharStatusDlg->SetEquipItemFromInven( pItem->GetSlotIndex(), pItem, m_pPressedButton );

										if( bEquip )
										{
											CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
											if( pMainMenuDlg ) 
												pMainMenuDlg->OpenCharStatusDialog();

											pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage(), true );
										}
									}
								}
								break;
							case ITEMTYPE_PET_EXPIRE:
								{
									if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100301 ) );
										return;
									}
									GetInterface().ShowPetExtendPeriodDlg( pItem, true );
								}
								break;
							case ITEMTYPE_PETCOLOR_BODY:
							case ITEMTYPE_PETCOLOR_TATOO:
								{
									if( CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady() )
										break;

									if( IsOnMailMode( pMailDlg ) )
										break;

									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor )
									{
										if( !pPlayerActor->IsSummonPet() )
										{
											CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9235 ), textcolor::YELLOW, 4.0f );
											return;
										}
										if (pItem->GetItemType() == ITEMTYPE_PETCOLOR_BODY)
										{
											if (pPlayerActor->GetPetInfo().dwPartsColor1 == pItem->GetTypeParam(0))
											{
												GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114005 ) );
												return;
											}
										}
										else
										{
											if (pPlayerActor->GetPetInfo().dwPartsColor2 == pItem->GetTypeParam(0))
											{
												GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114005 ) );
												return;
											}
										}

										if( m_pAskLookChgDlg->SetItem( pItem ) ) 
										{
											m_pAskLookChgDlg->Show( true );
										}
									}
								}
								break;
							case ITEMTYPE_PET_SKILL:
								{
									CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
									if( !pPlayerActor )
										return;

									if( !pPlayerActor->IsSummonPet() )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3319 ) );
										return;
									}

									TVehicleCompact* pPetCompact = &pPlayerActor->GetPetInfo();
									if( pPetCompact == NULL ) 
										return;

									if( ( pPetCompact->nSkillID1 > 0 && pPetCompact->nSkillID1 == pItem->GetTypeParam( 0 ) )
											|| ( pPetCompact->nSkillID2 > 0 && pPetCompact->nSkillID2 == pItem->GetTypeParam( 0 ) ) )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9260 ) );
									}
									else if( !pPetCompact->bSkillSlot && pPetCompact->nSkillID1 > 0 )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9259 ) );
									}
									else if( pPetCompact->bSkillSlot && pPetCompact->nSkillID1 > 0 && pPetCompact->nSkillID2 > 0 )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9258 ) );
									}
									else
									{
										GetInterface().ShowPetAddSkillDlg( pItem, true );
									}
								}
								break;
							case ITEMTYPE_PET_SKILLSLOT:
								{
									CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
									if( !pPlayerActor )
										return;

									if( !pPlayerActor->IsSummonPet() )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3319 ) );
										return;
									}

									TVehicleCompact* pPetCompact = &pPlayerActor->GetPetInfo();
									if( pPetCompact == NULL ) 
										return;

									if( pPetCompact->bSkillSlot )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9253 ) );
									}
									else
									{
										GetInterface().ShowPetExpandSkillSlotDlg( pItem, true );
									}
								}
								break;
							case ITEMTYPE_VEHICLEEFFECT:
								{
									if (IsOnMailMode(pMailDlg))
										break;

									// Note : ������ ��������.
									CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
									if( pCharStatusDlg )
									{
										bool bEquip = pCharStatusDlg->SetEquipItemFromInven( pItem->GetSlotIndex(), pItem, m_pPressedButton );

										if( bEquip ) {
											CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
											if( pMainMenuDlg ) {
												pMainMenuDlg->OpenCharStatusDialog();
											}
										}
										pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage() );
									}
								}
								break;
							case ITEMTYPE_VEHICLE_SHARE:
							case ITEMTYPE_VEHICLE:
								{
									if (IsOnMailMode(pMailDlg))
										break;

									// Note : ������ ��������.
									CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
									if( pCharStatusDlg )
									{
										bool bEquip = pCharStatusDlg->SetEquipItemFromInven( pItem->GetSlotIndex(), pItem, m_pPressedButton );

										if( bEquip ) {
											CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
											if( pMainMenuDlg ) {
												pMainMenuDlg->OpenCharStatusDialog();
											}
										}
										pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage() );
									}
								}
								break;
							case ITEMTYPE_VEHICLEPARTS:
								{
									if (IsOnMailMode(pMailDlg))
										break;

									// Note : ������ ��������.
									CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
									if( pCharStatusDlg )
									{
										DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
										if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItem->GetClassID())) 
											return;
								
										int nPartsType = -1;

										nPartsType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();

										bool bEquip = false;
										
										if(nPartsType == Vehicle::Parts::Saddle)
										{
											bEquip = pCharStatusDlg->SetEquipItemFromInven( pItem->GetSlotIndex(), pItem, m_pPressedButton );
										}
										else if(nPartsType == Vehicle::Parts::Hair)
										{
											if(!CDnActor::s_hLocalActor) break;
											
											CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
											
											if(!pPlayerActor )	break;

											if( !pPlayerActor->IsCanVehicleMode()) 
											{
												CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
												return; // ���������� ž�� �Ұ���
											}

											if(!pPlayerActor->IsVehicleMode())
											{
												CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9209 ), textcolor::YELLOW, 4.0f );
												return;
											}

											if( m_pAskLookChgDlg->SetItem( pItem ) ) {
												m_pAskLookChgDlg->Show( true );
											}
										}
											
										if( bEquip ) {
											CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
											if( pMainMenuDlg ) {
												pMainMenuDlg->OpenCharStatusDialog();
											}
											pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage(), true );
										}
									}
								}
								break;
							case ITEMTYPE_VEHICLEHAIRCOLOR:
								{
									if (IsOnMailMode(pMailDlg))
										break;

									// Note : ������ ��������.
									CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
									if( pCharStatusDlg )
									{
										if(!CDnActor::s_hLocalActor) break;
										
										CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
										
										if(!pPlayerActor )	break;

										if( !pPlayerActor->IsCanVehicleMode()) 
										{
											CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
											return; // ���������� ž�� �Ұ���
										}

										if(!pPlayerActor->IsVehicleMode())
										{
											CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9209 ), textcolor::YELLOW, 4.0f );
											return;
										}

										if( m_pAskLookChgDlg->SetItem( pItem ) ) {
											m_pAskLookChgDlg->Show( true );
											pCharStatusDlg->ChangeEquipPage( (CDnCharStatusDlg::EquipPageType)pItem->GetClickedEquipPage() );
										}
									}
								}
								break;
							case ITEMTYPE_PETALTOKEN:
								{
									GetInterface().ShowPetalTokenDialog( true, pItem );
								}
								break;
							case ITEMTYPE_APPELLATION:
								{
									if( CDnAppellationTask::IsActive() ) {
										int nAppellationArrayIndex = pItem->GetTypeParam(0) - 1;
										if( CDnAppellationTask::GetInstance().IsExistAppellation( nAppellationArrayIndex ) ) {
											WCHAR wszStr[256];
											swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7134 ), CDnAppellationTask::GetAppellationName( nAppellationArrayIndex ).c_str() );
											GetInterface().MessageBox( wszStr, MB_OK );
											return;
										}
									}
									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;
#ifdef PRE_ADD_COOKING_SYSTEM
							case ITEMTYPE_SECONDARYSKILL_RECIPE:
								{
									// �̹� �߰��� ���������� Ȯ��
									CManufactureSkill* pCookingSkill = GetLifeSkillCookingTask().GetCookingSkill();
									if( pCookingSkill && pCookingSkill->GetRecipe( pItem->GetClassID() ) )
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7489 ) );
									else
										GetLifeSkillCookingTask().RequestAddRecipe( pItem );
								}
								break;
#endif // PRE_ADD_COOKING_SYSTEM
							case ITEMTYPE_GUILDRENAME :
								{
									if(!CDnActor::s_hLocalActor) 
										break;
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if(!pPlayerActor )	
										break;
									
									// ���� ���� ������..����� üũ
									if( pItem->GetItemType() == ITEMTYPE_GUILDRENAME  && !pPlayerActor->IsGuildMaster() )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3921 ) ); // UISTRING : ����常 �� �� �ֽ��ϴ�.
										return;
									}
									GetInterface().ShowGuildRenameDlg( true, pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN, pItem->GetSlotIndex(), pItem->GetSerialID()  );
								}
								break;
							case ITEMTYPE_CHARNAME:
								{
									GetInterface().ShowCharRenameDlg( true, pItem );
								}
								break;
							case ITEMTYPE_ENCHANT_JEWEL:
								{
									GetInterface().ShowEnchantJewelDialog( true, pItem );
								}
								break;
							case ITEMTYPE_GUILDMARK:
								{
									if( !CDnActor::s_hLocalActor ) break;
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( !pPlayerActor )	break;
									if( !pPlayerActor->IsGuildMaster() )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3921 ) ); // UISTRING : ����常 �� �� �ֽ��ϴ�.
										break;
									}
									if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ) );
										break;
									}
									GetInterface().ShowGuildMarkCreateDialog( true, pItem );
								}
								break;
							case ITEMTYPE_HELPBOOK:	//���� �˸� ��ȭ ���
								{
									CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();//->GetSystemDlg()
									if (pMainMenuDlg)
									{
										int nIndex = pItem->GetTypeParam(0);
										CDnSystemDlg *pSystemDlg = pMainMenuDlg->GetSystemDlg();
										if (pSystemDlg)
											pSystemDlg->ShowHelpDlg(nIndex);
									}
								}
								break;
							case ITEMTYPE_REPAIR_EQUIPITEM:
								{
									if( GetItemTask().IsDisableItemInCharInven( pItem ) == true )
										break;

									if (IsOnMailMode(pMailDlg))
										break;

									int nPrice = GetItemTask().CalcRepairEquipPrice();
									if( nPrice == 0 )
									{
										GetInterface().MessageBox( 1736, MB_OK );
									}
									else
									{
										GetInterface().MessageBox( 3288, MB_YESNO, INVEN_REMOTE_REPAIR_DIALOG, this );
									}
								}
								break;
							case ITEMTYPE_PERIOD_APPELLATION:
								{
									if( CDnAppellationTask::IsActive() ) {
										int nAppellationArrayIndex = pItem->GetTypeParam(0) - 1;
										if( CDnAppellationTask::GetInstance().IsExistAppellation( nAppellationArrayIndex ) ) {
											WCHAR wszStr[256];
											swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7134 ), CDnAppellationTask::GetAppellationName( nAppellationArrayIndex ).c_str() );
											GetInterface().MessageBox( wszStr, MB_OK );
											return;
										}
									}
									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;
							case ITEMTYPE_INCREASE_LIFE:
								{									
									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;
							case ITEMTYPE_REBIRTH_COIN:
							case ITEMTYPE_REBIRTH_COIN_EX:
								{									
									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;

							case ITEMTYPE_INVENTORY_SLOT_EX:
								{
									int nUsableInvenCount = CDnItemTask::GetInstance().GetCharInventory().GetUsableSlotCount();
									int nTypeParam = pItem->GetTypeParam();
									if( nUsableInvenCount + nTypeParam > INVENTORYMAX )
									{
										GetInterface().MessageBox( 114034 );
										return;
									}

									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;

							case ITEMTYPE_WAREHOUSE_SLOT_EX:
								{
									int nUsableWareCount = CDnItemTask::GetInstance().GetStorageInventory().GetUsableSlotCount();
									int nTypeParam = pItem->GetTypeParam();
									if( nUsableWareCount + nTypeParam > WAREHOUSEMAX )
									{
										GetInterface().MessageBox( 114036 );
										return;
									}

									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;

#if defined(PRE_PERIOD_INVENTORY)
							case ITEMTYPE_PERIOD_INVENTORY:
								{
									if( GetItemTask().IsDisableItemInCharInven( pItem ) == true )
										break;

									if (IsOnMailMode(pMailDlg))
										break;

									GetInterface().MessageBox( 8435, MB_YESNO, INVEN_PERIOD_INVEN, this );	// UISTRING : ����� ���� �������� ����Ͻðڽ��ϱ�?\n(�̹� ����� ������ ��� ���ΰ��, �ش� �������� �Ⱓ��ŭ �������� ���� �˴ϴ�.)
								}
								break;

							case ITEMTYPE_PERIOD_WAREHOUSE:
								{
									if( GetItemTask().IsDisableItemInCharInven( pItem ) == true )
										break;

									if (IsOnMailMode(pMailDlg))
										break;

									GetInterface().MessageBox( 8434, MB_YESNO, INVEN_PERIOD_STORAGE, this );	// UISTRING : ����� â�� �������� ����Ͻðڽ��ϱ�?\n(�̹� ����� â�� ��� ���ΰ��, �ش� �������� �Ⱓ��ŭ �������� ���� �˴ϴ�.)
								}
								break;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

							case ITEMTYPE_PERIOD_PLATE_EX:
								{
									int nUsableGlyphCount = CDnItemTask::GetInstance().GetCashGlyphCount();
									int nTypeParam = pItem->GetTypeParam();
									if( nUsableGlyphCount + nTypeParam > CASHGLYPHSLOTMAX )
									{
										GetInterface().MessageBox( 4942 );
										return;
									}

									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;

							case ITEMTYPE_RETURN_HOME:
								{
									if(CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
										break;

									if (IsOnMailMode(pMailDlg))
										break;

									// ���� ������ ���.
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor ) {
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );
									}
								}
								break;

							case ITEMTYPE_PET_RENAME:
								{
									CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
									if (!pPlayerActor)
										break;

									CDnPetActor* pPetActor = pPlayerActor->GetMyPetActor();
									if (!pPetActor)
										break;

									CDnPetNamingDlg* pPetNamingDlg = GetInterface().GetPetNamingDlg();
									if (pPetNamingDlg)
									{
										pPetNamingDlg->SetChangeItemSerial(pItem->GetSerialID());
										pPetNamingDlg->SetPetSerial(pPetActor->GetPetInfo().Vehicle[Pet::Slot::Body].nSerial);
										pPetNamingDlg->Show(true);
									}
								}
								break;
							case ITEMTYPE_EXPAND_SKILLPAGE:
								{
									if( GetSkillTask().GetSkillPageCount() > DualSkill::Type::Secondary )
										GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1102 ));
									else
										GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;
							case ITEMTYPE_PET_EXP :
								{
									if( CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady() )
										break;

									if( IsOnMailMode( pMailDlg ) )
										break;

									WCHAR wszText[256] = {0,};
									swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8053), pItem->GetName() );									

									GetInterface().MessageBox( wszText, MB_YESNO, INVEN_PET_ADDEXP_DIALOG, this );									
								}
								break;

							case ITEMTYPE_FREE_PASS:
							case ITEMTYPE_UNLIMITED_FREE_PASS:
								{
									// ������ ���� ����Ʈ ���� ��û ��Ŷ ����.
									GetInterface().SetLastTryUseMapMoveCashItem( pItem );
									GetItemTask().RequestWarpVillageList( pItem->GetSerialID() );
								}
								break;

#ifdef PRE_ADD_AUTOUNPACK
							case ITEMTYPE_AUTOUNPACK:
								{
									// ���������� ��밡��.
									CDnWorld * m_pWorld = CDnWorld::GetInstancePtr();
									if( CDnWorld::MapTypeEnum::MapTypeVillage == m_pWorld->GetMapType() )
									{
										//GetItemTask().RequestAutoUnpackItem( pItem->GetSerialID(), true );										
										GetInterface().OpenAutoUnPack( pItem->GetSerialID(), true, pItem->GetTypeParam(1) );
									}
								}
								break;
#endif
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
							case ITEMTYPE_POTENTIAL_EXTRACTOR:
								{
									GetInterface().ShowItemPotentialTransferDialog( true );
								}
								break;
#endif
#ifdef PRE_ADD_CHAOSCUBE
							case ITEMTYPE_CHAOSCUBE:
								{
									GetInterface().OpenChaosCubeDialog( true, pItem );
								}
								break;
#endif
#ifdef PRE_ADD_CHOICECUBE
							case ITEMTYPE_CHOICECUBE:
								{
									GetInterface().OpenChoiceCubeDialog( true, pItem );
								}
								break;
#endif
#ifdef PRE_ADD_BESTFRIEND
							case ITEMTYPE_BESTFRIEND:
								{
									GetInterface().OpenBFRegistrationDlg( true, pItem->GetSerialID() );
								}
								break;

							case ITEMTYPE_GIFTBOX:
								{									
									GetInterface().OpenBFRewardDlg( true, pItem->GetSerialID(), pItem->GetTypeParam(0) ); // ���������Dlg.
								}
								break;
#endif
#ifdef PRE_ADD_SALE_COUPON
							case ITEMTYPE_SALE_COUPON:
								{
									if( GetInterface().GetMainMenuDialog() )
										GetInterface().GetMainMenuDialog()->CloseMenuDialog();

									CDnLocalPlayerActor::LockInput( false );
									CDnCashShopTask::GetInstance().RequestCashShopOpen();
#ifdef PRE_ADD_SALE_COUPON_CATEGORY
									int nCouponID = pItem->GetTypeParam( 0 );

									DNTableFileFormat* pCouponSox = GetDNTable( CDnTableDB::TSALECOUPON );
									if( pCouponSox )
									{
										int nSubCategory = pCouponSox->GetFieldFromLablePtr( nCouponID, "_SubCategory" )->GetInteger();
										GetInterface().GetCashShopDlg()->SetSaleCouponOpen( true, nSubCategory );
									}
#else // PRE_ADD_SALE_COUPON_CATEGORY
									GetInterface().GetCashShopDlg()->SetSaleCouponOpen( true );
#endif // PRE_ADD_SALE_COUPON_CATEGORY
								}
								break;
#endif // PRE_ADD_SALE_COUPON
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
							case ITEMTYPE_TOTALSKILLLEVEL_SLOT:
								{
									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;
#endif
#ifdef PRE_ADD_REMOTE_QUEST
							case ITEMTYPE_ADD_QUEST:
								{
									if( IsOnMailMode( pMailDlg ) )
										pMailDlg->LockDlgs( true );

									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor )
									{
										pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );
									}
									else 
									{
										if( IsOnMailMode( pMailDlg ) )
											pMailDlg->LockDlgs( false );
									}
								}
								break;
#endif // PRE_ADD_REMOTE_QUEST
#ifdef PRE_ADD_PVP_EXPUP_ITEM
							case ITEMTYPE_PVPEXPUP:
								{
									if( IsOnMailMode(pMailDlg) )
										break;

									int nPvPLimitLevel = pItem->GetTypeParam( 1 );
									CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
									if( pPlayerActor && pPlayerActor->GetPvPLevel() >= nPvPLimitLevel )
									{
										DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
										if( pSox && pSox->IsExistItem( nPvPLimitLevel ) )
										{
											int nUIString = pSox->GetFieldFromLablePtr( nPvPLimitLevel, "PvPRankUIString" )->GetInteger();
											std::wstring wszPvPLevelName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString );
											WCHAR wszStr[256] = {0,};
											swprintf_s( wszStr, _countof(wszStr), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8327 ), wszPvPLevelName.c_str() );
											GetInterface().MessageBox( wszStr );
										}
										break;
									}
									GetInterface().ShowAppellationGainDialog( true, pItem );
								}
								break;
#endif
#ifdef PRE_ADD_NEW_MONEY_SEED
							case ITEMTYPE_NEWMONEY_SEED:
								{
									if( CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady() )
										break;

									if( IsOnMailMode( pMailDlg ) )
										break;

									int nChargeSeeds = pItem->GetTypeParam( 0 );
									WCHAR wszStr[256] = {0,};
									swprintf_s( wszStr, _countof(wszStr), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4983 ), nChargeSeeds );
									GetInterface().MiddleMessageBox( wszStr, MB_YESNO, INVEN_USE_SEED_EXTENSION_DIALOG, this );
								}
								break;
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_DRAGON_GEM
							case ITEMTYPE_DRAGON_GEM:
								{
									if( IsOnMailMode( pMailDlg ) )
										break;

									if(pDragonGemEquipDlg)
									{
										//pDragonGemEquipDlg;
										GetInterface().GetDragonGemEquipDlg()->OnRecvDragonGemEquip(ERROR_NONE, pItem->GetSlotIndex());//pDragButton->GetSlotIndex());
										GetInterface().GetDragonGemEquipDlg()->Show(true);
									}
								}
								break;

							case ITEMTYPE_DRAGON_GEM_REMOVE:
								{
									if( IsOnMailMode( pMailDlg ) )
										break;

									if(pDragonGemRemoveDlg)
									{
										//pDragonGemRemoveDlg;
										GetInterface().GetDragonGemRemoveDlg()->Show(true);
									}
								}
								break;
#endif // PRE_ADD_DRAGON_GEM
							} // end of Switch
						} // end of if(bProcessed == false)

#ifdef PRE_ADD_PET_EXTEND_PERIOD
						else if( pItem->GetItemType() == ITEMTYPE_PET )	// unusable && itemtype == pet
						{
							if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
							{
								GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9281 ), false );
								return;
							}

							CDnPetExtendPeriodDlg* pPetExtendPeriodDlg = GetInterface().GetPetExtendPeriodDlg();
							if( pPetExtendPeriodDlg && pPetExtendPeriodDlg->IsShow() )
							{
								pPetExtendPeriodDlg->SetPetItem( m_pPressedButton );
								return;
							}
							// ���� �����۾ƴ� ��� ����Ⱓ�� ������ �ڵ����� ���Ǹ� ����â���� �̵�
							TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pItem->GetSerialID() );
							if( pPetCompact == NULL ) return;
							const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
							if( !pItem->IsEternityItem() && pNowTime >= pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )
							{
								GetInterface().ShowBuyPetExtendPeriodItemDlg( true, pItem->GetSerialID() );
							}
						}
#endif // PRE_ADD_PET_EXTEND_PERIOD
					}
				}
				else if (uMsg & VK_SHIFT)
				{
					if( pMarketBuyDlg && pMarketBuyDlg->IsShow() )
					{
						pMarketBuyDlg->SetSearchItem( pItem );
					}
					else
					{
						if (GetInterface().SetNameLinkChat(*pItem))
							return;
					}
				}
				else
				{
					HandleItemClick(pItem, pMailDlg, pInvenDlg);
				}
			}
			else
			{
				drag::ReleaseControl();

				MIInventoryItem *pDragItem = pDragButton->GetItem();
				if( !pDragItem ) return;

				if( pDragItem->GetType() == MIInventoryItem::Skill )
					return;

				if (pDragButton != m_pPressedButton)
				{
					// ���ø� ���� ����ó��. ���ø� ���̶��,
					if( pDragButton->GetRenderCount() != 0 )
					{
						// DragButton�� �������� �ְ�, PressedButton���� �������� �ִٸ�,
						MIInventoryItem *pPressedItem = m_pPressedButton->GetItem();
						if( pPressedItem && pPressedItem->GetType() == MIInventoryItem::Item )
						{
							// ���� �ٸ� �������̶��,
							if( pDragItem->GetClassID() != pPressedItem->GetClassID() )
							{
								pDragButton->DisableSplitMode(true);
								drag::ReleaseControl();

								// ���ڸ� �̵�
								CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
								if( pItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
								return;
							}
							else
							{
								CDnItem *pTempItem1 = static_cast<CDnItem *>(pDragButton->GetItem());
								CDnItem *pTempItem2 = static_cast<CDnItem *>(m_pPressedButton->GetItem());
								if( pTempItem1 && pTempItem2 )
								{
									if( pTempItem1->IsSoulbBound() != pTempItem2->IsSoulbBound() || pTempItem1->GetSealCount() != pTempItem2->GetSealCount() )
									{
										pDragButton->DisableSplitMode(true);
										drag::ReleaseControl();
										CEtSoundEngine::GetInstance().PlaySound( "2D", pTempItem1->GetDragSoundIndex() );
										return;
									}
								}
							}
						}
					}
					// ĳ������ �̵���ų �� ���� �Ѵ�.
					bool bMovable = true;
					int strID = ERROR_ITEM_DONTMOVE;
					if( pDragButton->GetSlotType() == ST_INVENTORY_CASH ) bMovable = false;
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
					if( pDragButton->GetSlotType() == ST_INVENTORY_CASHSHOP_REFUND ) bMovable = false;
#endif
#ifdef PRE_ADD_CASHREMOVE
					if( pDragButton->GetSlotType() == ST_INVENTORY_CASHREMOVE )
					{ 
						bMovable = false;
						strID = 4902; // ĳ�� ���� ����ǰ â������ �������� �ű� �� �����ϴ�.
					}
#endif
#ifdef PRE_ADD_SERVER_WAREHOUSE
					if (pDragButton->GetSlotType() == ST_STORAGE_WORLDSERVER_CASH || pDragButton->GetSlotType() == ST_STORAGE_WORLDSERVER_NORMAL)
					{
						bMovable = false;
						pDragButton->DisableSplitMode(true);
						drag::ReleaseControl();
						GetInterface().MessageBox(1632); // UISTRING : ���� â������ �������� ���� Ŭ�����θ� �ְų� �� �� �ֽ��ϴ�.
						return;
					}
#endif
					if( !bMovable )
					{
						pDragButton->DisableSplitMode(true);
						drag::ReleaseControl();
						GetInterface().ServerMessageBox( strID );
						return;
					}
					if( pDragButton->GetSlotType() == ST_STORE ) pDragButton->DisableSplitMode(true);

					CDnCostumeMixDlg* pCostumeMixDlg = static_cast<CDnCostumeMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUMEMIX_DIALOG));
					if (pCostumeMixDlg && pCostumeMixDlg->IsShow() && pDragButton->GetSlotType() == ST_ITEM_COSTUMEMIX_STUFF)
					{
						pCostumeMixDlg->ReleaseStuffItemSlotBtn(pDragButton);
						return;
					}

					CDnCostumeDesignMixDlg* pCostumeMergeDlg = static_cast<CDnCostumeDesignMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUME_DMIX_DIALOG));
					if (pCostumeMergeDlg && pCostumeMergeDlg->IsShow() && pDragButton->GetSlotType() == ST_ITEM_COSTUME_DMIX_STUFF)
					{
						pCostumeMergeDlg->ReleaseStuffItemSlotBtn(pDragButton);
						return;
					}

#ifdef PRE_ADD_COSRANDMIX
					CDnCostumeRandomMixDlg* pCostumeRandomDlg = static_cast<CDnCostumeRandomMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUME_RANDOMMIX_DIALOG));
					if (pCostumeRandomDlg && pCostumeRandomDlg->IsShow() && pDragButton->GetSlotType() == ST_ITEM_COSTUME_RANDOMMIX_STUFF)
					{
						pCostumeRandomDlg->ReleaseStuffItemSlotBtn(pDragButton);
						return;
					}
#endif
					if( !RequestMoveItem( pDragButton, m_pPressedButton ) )
					{
						pDragButton->DisableSplitMode(true);
						drag::ReleaseControl();
						// ������ ���ڸ� �̵��� ���� ���尡 �Ʒ��κп� �־ �� �����ؾ��ߴµ�,
						// ������ ������ ó���ϹǷ� ���� ������ �ʿ�� ����.
						return;
					}

				}
				else
				{

					pDragButton->DisableSplitMode(true);
					drag::ReleaseControl();

					// ���ڸ� �̵�
					CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
					if( pItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				}

				//pDragButton->m_bSnap = false;
				//pDragButton->m_bSnapBack = false;
				//m_pPressedButton->m_bSnap = false;
				//m_pPressedButton->m_bSnapBack = false;
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnInvenSlotDlg::SetItem( MIInventoryItem *pItem )
{
	if (pItem == NULL)
	{
		_ASSERT(0);
		return;
	}
// 	ASSERT( pItem );

	int nSlotIndex = pItem->GetSlotIndex()-GetSlotStartIndex();
	if (nSlotIndex < 0 || nSlotIndex >= (int)m_vecSlotButton.size())
	{
		_ASSERT(0);
		return;
	}
// 	ASSERT( nSlotIndex>=0&&"CDnInvenSlotDlg::SetItem" );
// 	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnInvenSlotDlg::SetItem" );

	m_vecSlotButton[nSlotIndex]->SetQuickItem( pItem );

	// �������� ���� ������������ OnRefreshTooltip�� ȣ���ؾ��ϴ°ɱ�.
	//m_vecSlotButton[nSlotIndex]->OnRefreshTooltip();
}

void CDnInvenSlotDlg::SetItemByIndex( MIInventoryItem *pItem, int index )
{
	if (pItem == NULL || (int)m_vecSlotButton.size() <= index )
		return;
	
	m_vecSlotButton[ index ]->SetQuickItem( pItem );
}


void CDnInvenSlotDlg::ResetSlot( int nSlotIndex )
{
	nSlotIndex -= GetSlotStartIndex();

	if (nSlotIndex < 0 || nSlotIndex >= (int)m_vecSlotButton.size())
	{
		_ASSERT(0);
		return;
	}
// 	ASSERT( nSlotIndex>=0&&"CDnInvenSlotDlg::ResetSlot" );
// 	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnInvenSlotDlg::ResetSlot" );

	m_vecSlotButton[nSlotIndex]->ResetSlot();
	m_vecSlotButton[nSlotIndex]->OnRefreshTooltip();
}



CDnItem * CDnInvenSlotDlg::GetSlotItem( int nSlotIndex )
{
	nSlotIndex -= GetSlotStartIndex();

	if (nSlotIndex < 0 || nSlotIndex >= (int)m_vecSlotButton.size())
		return NULL;
	
	return (CDnItem *)( m_vecSlotButton[nSlotIndex]->GetItem() );	
}


void CDnInvenSlotDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	ASSERT( dwItemCnt <= ITEM_MAX );
	if( dwItemCnt > ITEM_MAX )
		return;

	CDnQuickSlotButton *pButton(NULL);
	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	DWORD i(0);

	for( ; i<dwItemCnt; i++ )
	{
		pButton = m_vecSlotButton[i];
		if( pButton )
		{
			pButton->SetItemState( ITEMSLOT_ENABLE );
		}
	}

	for( ; i<dwVecSize; i++ )
	{
		pButton = m_vecSlotButton[i];
		if( pButton )
		{
			pButton->SetItemState( ITEMSLOT_DISABLE );
		}
	}

#ifdef PRE_ADD_INVEN_EXTENSION
	m_nUseItemCnt = dwItemCnt;
#endif
}

bool CDnInvenSlotDlg::IsEmptySlot()
{
	if( GetEmptySlot() == -1 )
	{
		return false;
	}

	return true;
}

int CDnInvenSlotDlg::GetEmptySlot()
{
	CDnQuickSlotButton *pButton(NULL);

	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	for( DWORD i=0; i<dwVecSize; i++ )
	{
		pButton = m_vecSlotButton[i];
		if( !pButton ) continue;

		if( (pButton->GetItemState() == ITEMSLOT_ENABLE) && pButton->IsEmptySlot() )
		{
			return pButton->GetSlotIndex();
		}
	}

	return -1;
}

int CDnInvenSlotDlg::GetEmptySlotCount()
{
	CDnQuickSlotButton *pButton(NULL);
	int nCount(0);

	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	for( DWORD i=0; i<dwVecSize; i++ )
	{
		pButton = m_vecSlotButton[i];
		if( !pButton ) continue;

		if( (pButton->GetItemState() == ITEMSLOT_ENABLE) && pButton->IsEmptySlot() )
			++nCount;
	}

	return nCount;
}

void CDnInvenSlotDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case INVEN_SPLIT_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_BUTTON_OK") )
				{
					int nValue = m_pInvenSepDlg->GetEditValue();
					if( nValue > 0 )
					{
						m_pPressedButton->EnableSplitMode(nValue);
						drag::SetControl(m_pPressedButton);

						CDnItem *pItem = dynamic_cast<CDnItem *>(m_pPressedButton->GetItem());
						if( pItem ) 
						{
							if( nValue > pItem->GetOverlapCount() ) 
								nValue = pItem->GetOverlapCount();
						}

						CDnInventoryDlg::SetSelectAmount( nValue );
					}
				}
				else if( IsCmdControl("ID_BUTTON_CANCEL") )
				{
				}

				m_pPressedButton = NULL;
				m_pInvenSepDlg->Show(false);
			}
		}
		break;

	case INVEN_REMOTE_REPAIR_DIALOG:
#if defined( PRE_PERIOD_INVENTORY )
	case INVEN_PERIOD_INVEN:
	case INVEN_PERIOD_STORAGE:
#endif	// #if defined( PRE_PERIOD_INVENTORY )
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_YES") )
				{
					CDnItem *pItem = dynamic_cast<CDnItem *>(m_pPressedButton->GetItem());
					if( pItem )
					{
						char cType = 0;
						cType = ( pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
						CDnPlayerActor *pPlayerActor = (CDnPlayerActor*)((CDnActor*)CDnActor::s_hLocalActor);
						if( pPlayerActor )
							pPlayerActor->UseItemFromSlotIndex( pItem->GetSlotIndex(), cType );
					}
				}
			}
		}
		break;
	case INVEN_PET_ADDEXP_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_YES") )
				{
					CDnItem *pItem = dynamic_cast<CDnItem *>(m_pPressedButton->GetItem());
					if( pItem )
					{
						CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());									
						if( pLocalPlayer )
						{
							if( !pLocalPlayer->IsSummonPet() )																					
							{				
								CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9235 ), textcolor::YELLOW, 4.0f );
								break;	// ���� ��ȯ���� ���� ���¿����� ����� �� �����ϴ�.											
							}

							// ����ġ �˻�..										
							TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();
							if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nItemID == 0 ) break;

							if( pPetCompact->nExp >= GetPetTask().GetPetMaxLevelExp( pPetCompact->Vehicle[Pet::Slot::Body].nItemID ) )
							{
								// UISTRING : ���� ����ġ�� �� �̻� ���� �� �� �����ϴ�.
								CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3327 ), textcolor::YELLOW, 4.0f );
								break;
							}			
							pLocalPlayer->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );							
						}
					}
				}
			}
		}
		break;
#ifdef PRE_ADD_NEW_MONEY_SEED
	case INVEN_USE_SEED_EXTENSION_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_YES") )
				{
					CDnItem *pItem = dynamic_cast<CDnItem *>(m_pPressedButton->GetItem());
					if( pItem )
					{
						CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());									
						if( pLocalPlayer )
						{
							int nMaxSeed = (int)CGlobalWeightIntTable::GetInstance().GetValue( CGlobalWeightIntTable::MaxSeedPoint );

							if( pItem->GetTypeParam( 0 ) + GetItemTask().GetSeed() > nMaxSeed ) // �õ� �ѵ� �ʰ�
							{
								WCHAR wszText[256] = {0,};
								swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4977 ), nMaxSeed );
								GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszText, false );
								break;
							}

							pLocalPlayer->UseItemFromSlotIndex( pItem->GetSlotIndex(), pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN );							
						}
					}
				}
			}
		}
		break;
#endif // PRE_ADD_NEW_MONEY_SEED
	}
}

int CDnInvenSlotDlg::GetSlotStartIndex()
{
	return m_SlotStartIndex;
}

bool CDnInvenSlotDlg::RequestMoveItem( CDnSlotButton *pDragButton, CDnQuickSlotButton *pPressedButton )
{
	// ������ �������� ���̾�α� ó��
	if( GetInterface().GetItemUnsealDlg()->IsShow() )
		return false;

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT  // #70609, #70419
	CDnItemUpgradeDlg *pItemUpgradeDlg = static_cast<CDnItemUpgradeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_UPGRADE_DIALOG ) );
	if( pItemUpgradeDlg && pItemUpgradeDlg->IsShow() )
	{
		CDnItem* pCurrentitem = NULL;
		CDnItem* pPressedButtonItem = NULL;
		CDnItem* pDragButtonItem = static_cast<CDnItem*>( pDragButton->GetItem() );
		CDnItemSlotButton* pForceSlot = pItemUpgradeDlg->GetCurrentForceSlot();

		if( pItemUpgradeDlg->GetCurrentForceSlot() )
			//pCurrentitem = pItemUpgradeDlg->GetForceSlotItem();
			pCurrentitem = static_cast<CDnItem*>( pForceSlot->GetItem() );
		if( pPressedButton )
			pPressedButtonItem = static_cast<CDnItem*>( m_pPressedButton->GetItem() );

		std::wstring wszError;

		// ������ �ش� ������ ã�ư��� �Ѵ�. ( ��ȭ ���� -> �κ� ) 
		// #70419 	���� ���� 
		if(  pCurrentitem 
			&& pPressedButtonItem 
			&& pDragButtonItem
			&& pItemUpgradeDlg->IsEquipeedItem() == false 
			&& pDragButtonItem->GetSlotIndex() == pPressedButtonItem->GetSlotIndex() 
			&& pDragButton->GetSlotType() != pPressedButton->GetSlotType() )		
		{	
			pItemUpgradeDlg->SafeDelForceSlotItem();	
			m_pPressedButton->SetRegist( false );
			pDragButton->ResetSlot();

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
			pItemUpgradeDlg->CheckUpgradeItem();
			pItemUpgradeDlg->ResetUpgradeItemBtn();
#endif

		}

		return false;
	}
#endif 

	// Note : �ŷ����϶��� �������� �̵��� �� ����.
	//
	if( GetInterface().IsShowPrivateMarketDlg() 
		|| GetInterface().IsShowCostumeMixDlg()
		|| GetInterface().IsShowCostumeDesignMixDlg()
		)
		return false;

	// ������ ���⼭���� ĳ���� ���� �÷��׸� �Ǵ�.
	if( GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() )
		return false;

	if( GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG)->IsShow() ||
		GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG)->IsShow() )
	{
		return false;
	}

	if( !pDragButton )		return false;
	if( !pPressedButton)	return false;

	
#if defined(PRE_ADD_STAGE_USECOUNT_ITEM)
	// ��������� �ɸ��� �̵���Ű�� �ʴ´�.
	CDnItem* pDragButtonItem = static_cast<CDnItem*>(pDragButton->GetItem());	
	if(pDragButtonItem && pDragButtonItem->IsUseLimited())
		return false;

	CDnItem* pPressedButtonItem = static_cast<CDnItem*>(pPressedButton->GetItem());	
	if(pPressedButtonItem && pPressedButtonItem->IsUseLimited())
		return false;
#endif

	// �ŷ�â�̳� ����â�� ������� ���¶�� �̵���Ű�� �ʴ´�.
	if( pDragButton->IsRegist() || pPressedButton->IsRegist() )
		return false;

	if( pDragButton->GetItem()->GetType() != MIInventoryItem::Item )
		return false;

	if( pPressedButton->GetSlotType() == ST_INVENTORY_QUEST )
		return false;

	// ����üũ
	if( GetItemTask().CheckDie( false ) )
		return false;

	// �ߺ�üũ�� �ѵ�.. �����̽��� ���ƿ°Ŷ� ������ �ؾ��ϰ�, ��¿�� ���� ���⼭ üũ�Ѵ�.
	if( CDnActor::s_hLocalActor ) {
		CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
		if( pActor && pActor->IsCanToggleBattleMode() == false ) {
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5072 ), false );
			return false;
		}
	}

	CDnItem *pDragItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
	if( !pDragItem ) return false;

	switch( pDragButton->GetSlotType() )
	{
	case ST_INVENTORY_QUEST:
		{
			GetItemTask().RequestMoveItem( MoveType_QuestInven, 
											pDragItem->GetSlotIndex(), 
											pDragItem->GetSerialID(),
											pPressedButton->GetSlotIndex(), 
											CDnInventoryDlg::GetCurrentAmount() );
		}
		break;
	case ST_INVENTORY:
#if defined(PRE_PERIOD_INVENTORY)
	case ST_INVENTORY_PERIOD:
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		{
			GetItemTask().RequestMoveItem( MoveType_Inven, 
											pDragItem->GetSlotIndex(), 
											pDragItem->GetSerialID(),
											pPressedButton->GetSlotIndex(), 
											CDnInventoryDlg::GetCurrentAmount() );
		}
		break;
	case ST_INVENTORY_CASH:
		{
			GetItemTask().RequestMoveItem( MoveType_CashInven, 
				pDragItem->GetSlotIndex(), 
				pDragItem->GetSerialID(),
				pPressedButton->GetSlotIndex(), 
				CDnInventoryDlg::GetCurrentAmount() );
		}
		break;
	case ST_CHARSTATUS:
		{
			CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if( !pCharStatusDlg ) break;

			MIInventoryItem *pItem = pPressedButton->GetItem();

			switch( pPressedButton->GetSlotType() ) {
				case ST_INVENTORY:
					{
						if( pDragItem->IsCashItem() ) {
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK ); // UISTRING : �̵��� �Ұ����� �������Դϴ�.
							return false;
						}
						int nMoveType = -1;
						int nSrcIndex = -1;
						if( pDragItem ) {
							switch( pDragItem->GetItemType() ) {
								case ITEMTYPE_WEAPON:
								case ITEMTYPE_PARTS:
									nMoveType = MoveType_EquipToInven;
									nSrcIndex = CDnCharStatusDlg::SLOTINDEX_2_EQUIPINDEX(pDragButton->GetSlotIndex());
									break;
								case ITEMTYPE_GLYPH:
									nMoveType = MoveType_GlyphToInven;
									nSrcIndex = CDnCharPlateDlg::GLYPH_SLOTINDEX_2_EQUIPINDEX(pDragButton->GetSlotIndex());
									break;
							}
						}

#ifdef PRE_ADD_EQUIPLOCK
						if (CDnItemTask::IsActive())
						{
							CDnItem* pCheckItemPtr = NULL;
							if (pDragButton->GetSlotType() == ST_CHARSTATUS && pDragItem)
								pCheckItemPtr = pDragItem;
							else if (pPressedButton->GetSlotType() == ST_CHARSTATUS && pItem)
								pCheckItemPtr = static_cast<CDnItem *>(pItem);

							if (pCheckItemPtr != NULL)
							{
								const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
								EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pCheckItemPtr);
								if (status != EquipItemLock::None)
									return false;
							}
						}
#endif

						if( pItem ) {
							CDnItem *pItemPtr = dynamic_cast<CDnItem *>(pItem);
							if( pItemPtr ) {
								switch( pItemPtr->GetItemType() ) {
									case ITEMTYPE_WEAPON:
									case ITEMTYPE_PARTS:
										{
											eEquipType equipType;
											CDnCharStatusDlg::eRetWearable ret = pCharStatusDlg->GetWearableEquipType( pItem, equipType );
											if (ret != CDnCharStatusDlg::eWEAR_ENABLE)
											{
												if (ret == CDnCharStatusDlg::eWEAR_UNABLE)
													GetInterface().MessageBox( pCharStatusDlg->GetLastErrorMsg().c_str(), MB_OK );
												else if (ret == CDnCharStatusDlg::eWEAR_NEED_UNSEAL)
													GetInterface().OpenItemUnsealDialog(pItem, pPressedButton);
												return false;
											}
										}
										break;
									case ITEMTYPE_GLYPH:
										{
											eGlyph equipType;
											CDnCharStatusDlg::eRetWearable ret = pCharStatusDlg->GetWearableGlyphEquipType( pItem, equipType );
											if (ret != CDnCharStatusDlg::eWEAR_ENABLE)
											{
												if (ret == CDnCharStatusDlg::eWEAR_UNABLE)
													GetInterface().MessageBox( pCharStatusDlg->GetLastErrorMsg().c_str(), MB_OK );
												else if (ret == CDnCharStatusDlg::eWEAR_NEED_UNSEAL)
													GetInterface().OpenItemUnsealDialog(pItem, pPressedButton);
												return false;
											}
										}
										break;
								}
							}
						}
						if( nMoveType == -1 || nSrcIndex == -1 ) return false;
						GetItemTask().RequestMoveItem( nMoveType, nSrcIndex, pDragItem->GetSerialID(), pPressedButton->GetSlotIndex(), pDragItem->GetOverlapCount() );
					}
					break;
				case ST_INVENTORY_CASH:
					{
						if( !pDragItem->IsCashItem() ) {
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK ); // UISTRING : �̵��� �Ұ����� �������Դϴ�.
							return false;
						}

						int nMoveType = -1;
						int nSrcIndex = -1;
						if( pDragItem ) {
							switch( pDragItem->GetItemType() ) {
								case ITEMTYPE_WEAPON:
								case ITEMTYPE_PARTS:
									nMoveType = MoveType_CashEquipToCashInven;
									nSrcIndex = CDnCharStatusDlg::CASH_SLOTINDEX_2_EQUIPINDEX(pDragButton->GetSlotIndex());
									break;
								case ITEMTYPE_GLYPH:
									nMoveType = MoveType_CashGlyphToCashInven;
									nSrcIndex = CDnCharPlateDlg::GLYPH_SLOTINDEX_2_EQUIPINDEX(pDragButton->GetSlotIndex());
									break;

								case ITEMTYPE_VEHICLEEFFECT:
									{
										nMoveType = MoveType_CashEquipToCashInven;
										nSrcIndex = CASHEQUIP_EFFECT;
									
										if(!CDnActor::s_hLocalActor)
											return false;
										CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
										if(!pPlayer || !pPlayer->IsCanVehicleMode() )
											return false;
									}
									break;

								case ITEMTYPE_VEHICLEPARTS:
									{
										nMoveType = MoveType_VehiclePartsToCashInven;
										nSrcIndex = pDragButton->GetSlotIndex();

										if(!CDnActor::s_hLocalActor)
											return false;
										CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
										if(!pPlayer || !pPlayer->IsCanVehicleMode() || !pPlayer->IsVehicleMode() || !pPlayer->GetMyVehicleActor())
											return false;
									}
									break;
								case ITEMTYPE_PETPARTS:
									{
										nMoveType = MoveType_PetPartsToCashInven;
										nSrcIndex = pDragButton->GetSlotIndex();

										if(!CDnActor::s_hLocalActor)
											return false;
										CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
										if( !pPlayer || !pPlayer->IsSummonPet() || !pPlayer->GetMyPetActor() )
											return false;
									}
									break;
							}
						}

#ifdef PRE_ADD_EQUIPLOCK
						if (CDnItemTask::IsActive())
						{
							CDnItem* pCheckItemPtr = NULL;
							if (pDragButton->GetSlotType() == ST_CHARSTATUS && pDragItem)
								pCheckItemPtr = pDragItem;
							else if (pPressedButton->GetSlotType() == ST_CHARSTATUS && pItem)
								pCheckItemPtr = static_cast<CDnItem *>(pItem);

							if (pCheckItemPtr != NULL)
							{
								const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
								EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pCheckItemPtr);
								if (status != EquipItemLock::None)
									return false;
							}
						}
#endif

						if( pItem ) {
							CDnItem *pItemPtr = dynamic_cast<CDnItem *>(pItem);
							if( !pItemPtr ) {
								switch( pItemPtr->GetItemType() ) {
									case ITEMTYPE_WEAPON:
									case ITEMTYPE_PARTS:
										{
											eCashEquipType equipType;
											if( pCharStatusDlg->GetWearableCashEquipType( pItem, equipType ) != CDnCharStatusDlg::eWEAR_ENABLE )
											{
												GetInterface().MessageBox( pCharStatusDlg->GetLastErrorMsg().c_str(), MB_OK );
												return false;
											}
										}
										break;
									case ITEMTYPE_GLYPH:
										{
											eGlyph equipType;
											CDnCharStatusDlg::eRetWearable ret = pCharStatusDlg->GetWearableGlyphEquipType( pItem, equipType );
											if (ret != CDnCharStatusDlg::eWEAR_ENABLE)
											{
												if (ret == CDnCharStatusDlg::eWEAR_UNABLE)
													GetInterface().MessageBox( pCharStatusDlg->GetLastErrorMsg().c_str(), MB_OK );
												else if (ret == CDnCharStatusDlg::eWEAR_NEED_UNSEAL)
													GetInterface().OpenItemUnsealDialog(pItem, pPressedButton);
												return false;
											}
										}
										break;
								}
							}
						}
						if( nMoveType == -1 || nSrcIndex == -1 ) return false;

						GetItemTask().RequestMoveItem( nMoveType, nSrcIndex, pDragItem->GetSerialID(), GetItemTask().GetCashInventory().FindFirstEmptyIndex(), pDragItem->GetOverlapCount() );
					}
					break;
				case ST_INVENTORY_VEHICLE:
					{
						if( !pDragItem->IsCashItem() ) {
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK ); // UISTRING : �̵��� �Ұ����� �������Դϴ�.
							return false;
						}

						int nMoveType = -1;
						int nSrcIndex = -1;
						if( pDragItem ) {
							switch( pDragItem->GetItemType() ) 
							{
							case ITEMTYPE_VEHICLE_SHARE:
							case ITEMTYPE_VEHICLE:
								{
									nMoveType = MoveType_VehicleBodyToVehicleInven;
									nSrcIndex = pDragButton->GetSlotIndex();

									if(!CDnActor::s_hLocalActor)
										return false;
									CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
									if(!pPlayer || !pPlayer->IsCanVehicleMode() || !pPlayer->IsVehicleMode() || !pPlayer->GetMyVehicleActor())
										return false;
								}
								break;
							case ITEMTYPE_PET:
								{
									nMoveType = MoveType_PetBodyToPetInven;
									nSrcIndex = pDragButton->GetSlotIndex();

									if( !CDnActor::s_hLocalActor )
										return false;
									CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
									if( !pPlayer || !pPlayer->IsSummonPet() || !pPlayer->GetMyPetActor() )
										return false;
								}
								break;
							}
						}

						if( nMoveType == -1 || nSrcIndex == -1 ) return false;
						GetItemTask().RequestMoveItem( nMoveType, nSrcIndex, pDragItem->GetSerialID(), GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pDragItem->GetOverlapCount() );
					}
					break;
			}
		}
		break;
	case ST_STORAGE_PLAYER:
#if defined(PRE_PERIOD_INVENTORY)
	case ST_STORAGE_PERIOD:
#endif	// #if defined(PRE_PERIOD_INVENTORY)
		{
			GetItemTask().RequestMoveItem( MoveType_WareToInven, 
														pDragItem->GetSlotIndex(),
														pDragItem->GetSerialID(),
														pPressedButton->GetSlotIndex(),
														pDragItem->GetOverlapCount() );
		}
		break;
	case ST_STORAGE_FARM :
		{
			GetInterface().RequestTakeWareHouseItem( pDragButton->GetSlotIndex() );
		}
		break;
	case ST_STORAGE_GUILD:
		{
			if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_TAKEITEM ) == false )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3914 ), MB_OK );
				return false;
			}

#ifdef PRE_ADD_CHANGEGUILDROLE
			if( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()) > GUILDROLE_TYPE_SUBMASTER )
#else
			if( !GetGuildTask().IsMaster() )
#endif
			{
				int nMaxTakeItemCount = GetGuildTask().GetMaxTakeItemByRole( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()) );
				if( nMaxTakeItemCount == 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3914 ), MB_OK );
					return false;
				}

				int nRemainCount = nMaxTakeItemCount - GetItemTask().GetTakeGuildWareItemCount();
				if( nRemainCount <= 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory2, 555 ), MB_OK );
					return false;
				}
			}

			GetItemTask().RequestMoveItem( MoveType_GuildWareToInven, 
														pDragItem->GetSlotIndex(),
														pDragItem->GetSerialID(),
														pPressedButton->GetSlotIndex(),
														pDragItem->GetOverlapCount() );
		}
		break;
	case ST_STORE:
		{
			GetTradeTask().GetTradeItem().SetTradeBuyItem( pDragItem );
		}
		break;
	case ST_QUICKSLOT:
	case ST_QUEST:
	case ST_SKILL_STORE:
		CDebugSet::ToLogFile( "CDnInvenSlotDlg::RequestMoveItem, ST_QUICKSLOT, ST_QUEST, ST_SKILL_STORE" );
		return false;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ST_TALISMAN:
		{
			switch( pPressedButton->GetSlotType() )
			{
			case ST_INVENTORY:
				{
					if( pDragItem->IsCashItem() )
					{
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK ); // UISTRING : �̵��� �Ұ����� �������Դϴ�.
						return false;
					}

					if( pPressedButton->GetItem() )
					{
						GetInterface().MessageBox(1971, MB_OK); // �� ������ ���� �������� ÷���� �� �����ϴ�. �̰� �ӽ�
						return false;
					}
					
					GetItemTask().RequestMoveItem(  MoveType_TalismanToInven, 
													pDragItem->GetSlotIndex(),
													pDragItem->GetSerialID(),
													pPressedButton->GetSlotIndex(),
													pDragItem->GetOverlapCount() );
				}
			}
		}
		break;	
#endif
	}

	return true;
}

//void CDnInvenSlotDlg::Process( float fElapsedTime )
//{
//	CEtUIDialog::Process( fElapsedTime );
//
//	if( !IsShow() )
//		return;
//
//	CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
//	if( !pDragButton )
//		return;
//
//	CDnQuickSlotButton *pSlotButton(NULL);
//	bool bMouseIn(false);
//	
//	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
//	{
//		pSlotButton = m_vecSlotButton[i];
//		if( !pSlotButton ) continue;
//
//		if( pSlotButton->GetItemState() == ITEMSLOT_DISABLE ) 
//			continue;
//
//		pSlotButton->m_bSnapBack = false;
//
//		if( pSlotButton->IsMouseEnter() )
//		{
//			pDragButton->m_bSnap = true;
//			pSlotButton->m_bSnapBack = true;
//
//			SUICoord uiCoord;
//			pSlotButton->GetUICoord( uiCoord );
//
//			uiCoord.fX += GetXCoord();
//			uiCoord.fX /= GetScreenWidthRatio();
//			uiCoord.fY += GetYCoord();
//			uiCoord.fWidth /= GetScreenWidthRatio();
//
//			pDragButton->SetCursorCoord( uiCoord );
//
//			OutputDebug( "Snap true %d\n", i );
//			bMouseIn = true;
//		}
//	}
//
//	if( !bMouseIn )
//	{
//		pDragButton->m_bSnap = false;
//		OutputDebug( "Snap false\n" );
//	}
//}

void CDnInvenSlotDlg::ReleaseNewGain()
{
	CDnQuickSlotButton *pButton(NULL);
	CDnItem *pItem(NULL);

	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	for( DWORD i=0; i<dwVecSize; i++ )
	{
		pButton = m_vecSlotButton[i];
		if( !pButton ) continue;

		pItem = dynamic_cast<CDnItem*>(pButton->GetItem());
		if( pItem ) pItem->SetNewGain( false );
	}
}

int CDnInvenSlotDlg::GetRegisteredSlotCount()
{
	CDnQuickSlotButton *pButton(NULL);
	CDnItem *pItem(NULL);
	int nCount = 0;

	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	for( DWORD i=0; i<dwVecSize; i++ )
	{
		pButton = m_vecSlotButton[i];
		if( !pButton ) continue;

		if( pButton->IsRegist() )
			++nCount;
	}
	return nCount;
}

#ifdef PRE_ADD_COSRANDMIX
bool CDnInvenSlotDlg::ReadyCostumeMix(CDnItem* pItem)
{
	if (pItem == NULL)
		return false;

	bool bEnable = true;

	eItemTypeEnum type = pItem->GetItemType();
	if (type == ITEMTYPE_COSTUMEMIX ||
		type == ITEMTYPE_COSTUMEDESIGN_MIX)
	{
		CDnCostumeMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeMixDataMgr();
		if (type == ITEMTYPE_COSTUMEMIX)			bEnable = dataMgr.IsEnableCostumeMix();
		else if (type == ITEMTYPE_COSTUMEDESIGN_MIX) bEnable = dataMgr.IsEnableCostumeDesignMix();
	}
	else if (type == ITEMTYPE_COSTUMERANDOM_MIX)
	{
		CDnCostumeRandomMixDataMgr& dataMgr = CDnItemTask::GetInstance().GetCostumeRandomMixDataMgr();
		bEnable = dataMgr.IsEnableCosRandomMix();
#ifdef PRE_ADD_COSRANDMIX_MIXERTYPE
		dataMgr.SetMixerEnablePartsType(pItem->GetTypeParam(0));
#endif
	}
	else
	{
		return true;
	}

	if (bEnable == false)
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 130310 ) );	// UISTRING : �ڽ�Ƭ �ռ� �� ������ �߻��߽��ϴ�.
		return false;
	}

	if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage)
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 130316 ) );	// UISTRING : �ڽ�Ƭ �ռ��� ���������� �����մϴ�.
		return false;
	}

	if (GetInterface().IsShowItemUnsealDialog())
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3213));	// UISTRING : ���� ���� �ÿ��� �ͽ��ڽ��� ����� �� �����ϴ�.
		return false;
	}

	if (CDnTradeTask::IsActive())
		CDnTradeTask::GetInstance().GetTradePrivateMarket().ClearTradeUserInfoList();

	if (CDnItemTask::IsActive() && CDnItemTask::GetInstance().ReadyItemCostumeMixOpen(type))
		return true;

	return false;
}
#endif

#ifdef PRE_FIX_COSMIX_NORESPONSE_ON_ERROR
bool CDnInvenSlotDlg::IsOnCostumeMixMode(const CDnItem& item) const
{
	if (item.GetItemType() == ITEMTYPE_COSTUMEMIX ||
		item.GetItemType() == ITEMTYPE_COSTUMEDESIGN_MIX ||
		item.GetItemType() == ITEMTYPE_COSTUMERANDOM_MIX)
		return true;
	return false;
}

void CDnInvenSlotDlg::CancelCostumeMixReady(const CDnItem& item)
{
	CDnLocalPlayerActor::LockInput(false);

	if (item.GetItemType() == ITEMTYPE_COSTUMEMIX)
		GetInterface().ShowCostumeMixDlg(false, false);
	else if (item.GetItemType() == ITEMTYPE_COSTUMEDESIGN_MIX)
		GetInterface().ShowCostumeDesignMixDlg(false, false);
	else if (item.GetItemType() == ITEMTYPE_COSTUMERANDOM_MIX)
		GetInterface().ShowCostumeRandomMixDlg(false, false);
}
#endif