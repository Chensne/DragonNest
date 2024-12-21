#include "StdAfx.h"
#include "DnCharPetDlg.h"
#include "DnItem.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnInventoryDlg.h"
#include "DnItemTask.h"
#include "DnFadeInOutDlg.h"
#include "DnInvenTabDlg.h"
#include "DnPetOptionDlg.h"
#include "DnCharPetPreviewDlg.h"
#include "DnCharPetInfoDlg.h"
#include "DnVehicleTask.h"
#include "TimeSet.h"
#include "DnPetTask.h"
#include "DnCharStatusDlg.h"
#include "DnCommonUtil.h"
#include "DnSkillTask.h"
#include "TaskManager.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharPetDlg::CDnCharPetDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pPetSlot( NULL )
, m_pPetPartsSlot1( NULL )
, m_pPetPartsSlot2( NULL )
, m_pCancelButton( NULL )
, m_pStaticPetName( NULL )
, m_pStaticPetLevel( NULL )
, m_pStaticPetExpireDate( NULL )
, m_pProgressBarPetExp( NULL )
, m_pStaticPetExp( NULL )
, m_pProgressBarPetSatiety( NULL )
, m_pStaticPetSatiety( NULL )
, m_pPetOptionDlg( NULL )
, m_pPreviewDlg( NULL )
, m_pPetInfoDlg( NULL )
, m_nPetLevelTableID( 0 )
{
	memset( m_pSkillSlotButton, 0, sizeof(m_pSkillSlotButton) );
	memset( m_pBackgroundSkillSlot, 0, sizeof(m_pBackgroundSkillSlot) );
}

CDnCharPetDlg::~CDnCharPetDlg()
{
	SAFE_DELETE( m_pPetOptionDlg );
	SAFE_DELETE( m_pPreviewDlg );
	SAFE_DELETE( m_pPetInfoDlg );

	SAFE_RELEASE_SPTR( m_hSkill1 );
	SAFE_RELEASE_SPTR( m_hSkill2 );
}

void CDnCharPetDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pPetSlot = GetControl<CDnItemSlotButton>( "ID_BUTTON_RIDE0" );
	m_pPetSlot->SetSlotIndex( PetSlot );
	m_pPetSlot->SetSlotType( ST_CHARSTATUS );

	m_pPetPartsSlot1 = GetControl<CDnItemSlotButton>( "ID_BUTTON_RIDE1" );
	m_pPetPartsSlot1->SetSlotIndex( PetPartsSlot1 );
	m_pPetPartsSlot1->SetSlotType( ST_CHARSTATUS );

	m_pPetPartsSlot2 = GetControl<CDnItemSlotButton>( "ID_BUTTON_RIDE2" );
	m_pPetPartsSlot2->SetSlotIndex( PetPartsSlot2 );
	m_pPetPartsSlot2->SetSlotType( ST_CHARSTATUS );

	m_pStaticPetName = GetControl<CEtUIStatic>( "ID_TEXT_PETNAME" );
	m_pStaticPetLevel = GetControl<CEtUIStatic>( "ID_TEXT_PETLEVEL" );
	m_pStaticPetExpireDate = GetControl<CEtUIStatic>( "ID_TEXT_PETLIMIT" );

	m_pProgressBarPetExp = GetControl<CEtUIProgressBar>( "ID_EXP_GAUGE" );
	m_pStaticPetExp = GetControl<CEtUIStatic>( "ID_TEXT_EXP" );

	m_pProgressBarPetSatiety = GetControl<CEtUIProgressBar>( "ID_PROGRESSBAR_FOOD" );
	m_pStaticPetSatiety = GetControl<CEtUIStatic>( "ID_TEXT_FOOD" );

	m_pCancelButton = GetControl<CEtUIButton>( "ID_BUTTON_CANCEL" );
	m_pCancelButton->Enable( false );

	m_pPreviewDlg = new CDnCharPetPreviewDlg( UI_TYPE_CHILD, this );
	m_pPreviewDlg->Initialize( false );

	m_pPetOptionDlg = new CDnPetOptionDlg( UI_TYPE_TOP_MSG );
	m_pPetOptionDlg->Initialize( false );

	m_pPetInfoDlg = new CDnCharPetInfoDlg( UI_TYPE_CHILD, this );
	m_pPetInfoDlg->Initialize( false );

	char szType[32]={0,};
	for( int i=0; i<MAX_PET_SKILL; ++i )
	{
		sprintf_s( szType, "ID_BT_SKILL%d", i );

		m_pSkillSlotButton[i] = static_cast<CDnSkillSlotButton*>( GetControl<CEtUIControl>(szType) );
		m_pSkillSlotButton[i]->SetSlotType( ST_SKILL );
		m_pSkillSlotButton[i]->SetSlotState( SKILLSLOT_DISABLE );
		m_pSkillSlotButton[i]->Show( false );

		sprintf_s( szType, "ID_BG_SKILL%d", i );
		m_pBackgroundSkillSlot[i] = GetControl<CEtUIStatic>( szType );
		m_pBackgroundSkillSlot[i]->Show( false );
	}
}

void CDnCharPetDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharPetDlg.ui" ).c_str(), bShow );
}

void CDnCharPetDlg::Show( bool bShow )
{
	CEtUIDialog::Show( bShow );

	bool bShowPreviewDlg = false;
	if( CDnActor::s_hLocalActor )
	{
		CDnLocalPlayerActor *pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if( bShow && pLocalPlayer && pLocalPlayer->IsSummonPet() && pLocalPlayer->GetSummonPet() )
			bShowPreviewDlg = true;
	}
	else
	{
		CEtUIDialog::Show( false );
		return;
	}

	if( bShow )
	{
		RefreshPetEquip();
#ifdef PRE_ADD_INSTANT_CASH_BUY
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_PET );
		if( nShowType == INSTANT_BUY_HIDE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( false );
		}
		else if( nShowType == INSTANT_BUY_SHOW )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( true );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( true );
		}
		else if( nShowType == INSTANT_BUY_DISABLE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( false );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( true );
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
	}

	ShowChildDialog( m_pPreviewDlg, bShowPreviewDlg );
	if( !bShow && m_pPetInfoDlg )
		ShowChildDialog( m_pPetInfoDlg, bShow );

	if( !bShow && m_pPetOptionDlg )
		m_pPetOptionDlg->Show( bShow );
}

void CDnCharPetDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( !IsShow() )
		return;

	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor* pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
	if( pActor == NULL )
		return;

	if( drag::IsValid() )
	{
		CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();
		switch( pDragButton->GetSlotType() ) {
			case ST_INVENTORY_VEHICLE:
				break;
			case ST_INVENTORY_CASH:
				break;
			default: return;
		}

		CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
		if( !pItem ) return;

		switch( pItem->GetItemType() ) 
		{
			case ITEMTYPE_PET:
				{
					m_pPetSlot->SetWearable( true );
				}
				break;
			case ITEMTYPE_PETPARTS:
				{
					DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
					if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItem->GetClassID()) )
						break;

					int equipType = -1;
					equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();

					int VehicleClassType = -1;
					VehicleClassType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehicleClassID" )->GetInteger();

					// 해당 펫은 그 아이템을 장착 할 수 없습니다.
					if( pActor->GetPetClassType() == VehicleClassType && VehicleClassType != -1 )
					{
						if( equipType == 0 )
							m_pPetPartsSlot1->SetWearable( true );
						else if( equipType == 1 )
							m_pPetPartsSlot2->SetWearable( true );
					}
				}
				break;
		}
	}
	else
	{
		m_pPetSlot->SetWearable( false );
		m_pPetPartsSlot1->SetWearable( false );
		m_pPetPartsSlot2->SetWearable( false );
	}

	TVehicleCompact* pPetCompact = &pActor->GetPetInfo();
	if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nItemID == 0 ) return;

	if( m_hSkill1 )
	{
		MIInventoryItem* phSkill = pActor->FindSkill( pPetCompact->nSkillID1 );
		if( phSkill )
		{
			m_hSkill1->SetForceCoolTime( phSkill->GetCoolTime() );
		}
		else
		{
			m_hSkill1->ResetCoolTime();
		}
	}

	if( m_hSkill2 )
	{
		MIInventoryItem* phSkill = pActor->FindSkill( pPetCompact->nSkillID2 );
		if( phSkill )
		{
			m_hSkill2->SetForceCoolTime( phSkill->GetCoolTime() );
		}
		else
		{
			m_hSkill2->ResetCoolTime();
		}
	}
}

void CDnCharPetDlg::SetPetEquipItem( CDnItem* pItem )
{
	int nEquipIndex = pItem->GetSlotIndex();
	if( nEquipIndex == PetSlot )
	{
		m_pPetSlot->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
		CDnItem* pItem = (CDnItem*)m_pPetSlot->GetItem();
		if( pItem )
			pItem->SetSoulBound( true );

		m_pPetSlot->OnRefreshTooltip();
		SetPetInfoDetail();

		if( IsShow() && !m_pPreviewDlg->IsShow() )
			ShowChildDialog( m_pPreviewDlg, true );
		m_pCancelButton->Enable( true );
	}
	else if( nEquipIndex == PetPartsSlot1 )
	{
		m_pPetPartsSlot1->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
		m_pPetPartsSlot1->OnRefreshTooltip();
		if( m_pPetInfoDlg && m_pPetInfoDlg->IsShow() )
			m_pPetInfoDlg->SetPetAddAbility();
	}
	else if( nEquipIndex == PetPartsSlot2 )
	{
		m_pPetPartsSlot2->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
		m_pPetPartsSlot2->OnRefreshTooltip();
		if( m_pPetInfoDlg && m_pPetInfoDlg->IsShow() )
			m_pPetInfoDlg->SetPetAddAbility();
	}
}

void CDnCharPetDlg::RefreshPetEquip()
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor *pLocaPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocaPlayer )
	{
		CDnItem* pPetItem = (CDnItem*)m_pPetSlot->GetItem();
		CDnItem* pPetPartsItem1 = (CDnItem*)m_pPetPartsSlot1->GetItem();
		CDnItem* pPetPartsItem2 = (CDnItem*)m_pPetPartsSlot2->GetItem();

		if( pPetItem )
		{
			if( pLocaPlayer->GetPetInfo().Vehicle[Pet::Slot::Body].nSerial != pPetItem->GetSerialID() )
				ResetPetEquipSlot( Pet::Slot::Body );
		}
		if( pPetPartsItem1 )
		{
			if( pLocaPlayer->GetPetInfo().Vehicle[Pet::Slot::Accessory1].nSerial != pPetPartsItem1->GetSerialID() )
				ResetPetEquipSlot( Pet::Slot::Accessory1 );
		}
		if( pPetPartsItem2 )
		{
			if( pLocaPlayer->GetPetInfo().Vehicle[Pet::Slot::Accessory2].nSerial != pPetPartsItem2->GetSerialID() )
				ResetPetEquipSlot( Pet::Slot::Accessory2 );
		}
	}
	SetPetInfoDetail();
}

void CDnCharPetDlg::ResetPetEquipSlot( int nEquipIndex )
{
	if( nEquipIndex == PetSlotIndex::PetSlot )
	{
		m_pPetSlot->ResetSlot();
		m_pPetSlot->OnRefreshTooltip();
		RemovePetInfoDetail();

		if( IsShow() && m_pPreviewDlg->IsShow() )
			ShowChildDialog( m_pPreviewDlg, false );
		if( m_pPetInfoDlg )
			ShowChildDialog( m_pPetInfoDlg, false );
		m_pCancelButton->Enable( false );
	}
	else if( nEquipIndex == PetPartsSlot1 )
	{
		m_pPetPartsSlot1->ResetSlot();
		m_pPetPartsSlot1->OnRefreshTooltip();
		if( m_pPetInfoDlg && m_pPetInfoDlg->IsShow() )
			m_pPetInfoDlg->SetPetAddAbility();
	}
	else if( nEquipIndex == PetPartsSlot2 )
	{
		m_pPetPartsSlot2->ResetSlot();
		m_pPetPartsSlot2->OnRefreshTooltip();
		if( m_pPetInfoDlg && m_pPetInfoDlg->IsShow() )
			m_pPetInfoDlg->SetPetAddAbility();
	}
}

void CDnCharPetDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if(!CDnActor::s_hLocalActor )
		return;;

	CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
	if( pActor == NULL )
		return;

	bool bPetSummonEnable = true;
	if( !pActor->IsInPetSummonableMap() )	// 소환불가 지역에 있는 경우 기능 제한
		bPetSummonEnable = false;

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		CDnSlotButton *pDragButton;
		CDnItemSlotButton *pPressedButton;

		if( GetItemTask().IsRequestWait() ) return;

		if( IsCmdControl( "ID_BUTTON_CANCEL" ) ) 
		{
			if( !bPetSummonEnable ) return;
			if( !GetPetTask().CheckCanMovePet( true ) ) return;

			if( pActor && pActor->IsSummonPet() )
			{
				CDnItem* pItem = (CDnItem*)m_pPetSlot->GetItem();
				if( !pItem )
					return;

				if( pItem->GetSlotIndex() == Pet::Slot::Body )	// 펫 해제
				{
					GetItemTask().RequestMoveItem( MoveType_PetBodyToPetInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
					if( m_pPetInfoDlg )
						ShowChildDialog( m_pPetInfoDlg, false );

					m_pCancelButton->Enable( false );
					RemovePetInfoDetail();
				}
			}
			return;
		}
		else if( IsCmdControl( "ID_BT_OPTION" ) )
		{
			GetPetTask().LoadPetConfig();
			if( m_pPetOptionDlg )
				m_pPetOptionDlg->Show( true );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON0" ) )
		{
			if( m_pPetInfoDlg->IsShow() )
				ShowChildDialog( m_pPetInfoDlg, false );
			else
			{
				if( m_nPetLevelTableID > 0 )
				{
					m_pPetInfoDlg->SetPetLevelTableID( m_nPetLevelTableID );
					ShowChildDialog( m_pPetInfoDlg, true );
				}
			}
			return;
		}
		else if( strstr( pControl->GetControlName(), "ID_SKILL" ) )
		{
			CDnSkillSlotButton* pSkillControl = static_cast<CDnSkillSlotButton *>(pControl);
			CDnSkill* pSkillItem = dynamic_cast<CDnSkill*>( pSkillControl->GetItem() );
			if( !pSkillItem )	return;

			if( uMsg == WM_LBUTTONDOWN && pSkillItem->GetSkillType() == CDnSkill::Active )
			{
				CDnSlotButton* pDragButton = ( CDnSlotButton* )drag::GetControl();

				if( pDragButton == NULL )
				{
					CDnQuickSlotButton* pPressedButton = static_cast<CDnQuickSlotButton*>( pControl );

					pPressedButton->EnableSplitMode( 0 );
					drag::SetControl( pPressedButton );

					// 전투모드 미리 변경. 물약과 달리 스킬은 집는거 자체가 퀵슬롯에 등록시키려고 할때밖에 없으므로.
					CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>( CDnActor::s_hLocalActor.GetPointer() );
					if( !pActor->IsBattleMode() )
					{
						if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
						{
							if( pActor->IsCanBattleMode() )
							{
								if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
								pActor->CmdToggleBattle( true );
							}
						}
					}
				}
				else
				{
					pDragButton->DisableSplitMode( false );
					drag::ReleaseControl();
				}
			}
			else if( uMsg == WM_RBUTTONUP )
			{
				int nSlotIndex = 0;
				if( IsCmdControl( "ID_BT_SKILL0" ) )
					nSlotIndex = 1;
				else if( IsCmdControl( "ID_BT_SKILL1" ) )
					nSlotIndex = 2;

				CDnSkill* pSkillItem = dynamic_cast<CDnSkill*>( pSkillControl->GetItem() );
				if( CDnPetTask::IsActive() )
					GetPetTask().RequestRemovePetSkill( nSlotIndex, pSkillItem );
			}

			return;
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		else if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_PET, NULL );
			return;
		}
		else if( IsCmdControl( "ID_CLOSE_DIALOG" ) )
		{
			CDnCharStatusDlg* pCharStatusDlg = dynamic_cast<CDnCharStatusDlg*>( GetParentDialog() );
			if( pCharStatusDlg && pCharStatusDlg->GetCallBack() )
				pCharStatusDlg->GetCallBack()->OnUICallbackProc( pCharStatusDlg->GetDialogID(), EVENT_BUTTON_CLICKED, GetControl( "ID_CLOSE_DIALOG" ) );

			return;
		}
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_PET);
			return;
		}
#endif
		
		if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
			return;

		// 기본적으로 거래중엔 캐릭터창이 안뜰테지만, 혹시 뜨더라도 장비해제 및 교체를 막아두겠다.
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog() ) return;

		pDragButton = ( CDnSlotButton * )drag::GetControl();
		pPressedButton = ( CDnItemSlotButton * )pControl;

		if( pDragButton == NULL )
		{
			if( !pPressedButton->GetItem() ) 
				return;

			CDnItem *pItem = (CDnItem*)pPressedButton->GetItem();

			if( !pItem )
				return;

			if( uMsg == WM_RBUTTONUP )
			{
				if( !bPetSummonEnable ) return;

				if( pItem->GetSlotIndex() == Pet::Slot::Body )
				{
					GetItemTask().RequestMoveItem( MoveType_PetBodyToPetInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
				}
				else if( pItem->GetSlotIndex() == Pet::Slot::Accessory1 || pItem->GetSlotIndex() == Pet::Slot::Accessory2 )
				{
					GetItemTask().RequestMoveItem( MoveType_PetPartsToCashInven, pItem->GetSlotIndex(), pItem->GetSerialID(), GetItemTask().GetCashInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
				}
				return;
			}

			// todo by kalliste : refactoring with DnInvenSlotDlg.cpp
			if( uMsg & VK_SHIFT )
			{
				if( GetInterface().SetNameLinkChat( *pItem ) )
					return;
			}

			pPressedButton->EnableSplitMode( 1 );
			drag::SetControl( pPressedButton );

			CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
		}
		else
		{
			pDragButton->DisableSplitMode( true );
			drag::ReleaseControl();

			int nEquipIndexP = pPressedButton->GetSlotIndex();
			if( nEquipIndexP == -1 ) return;

			if( pDragButton->GetItem() ) 
			{
				CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
				if( !pItem ) return;

				if( pDragButton == pPressedButton )
				{
					// 제자리 이동
					if( pItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
					return;
				}

				switch( pItem->GetItemType() ) 
				{
					case ITEMTYPE_PET:
					case ITEMTYPE_PETPARTS:
						break;
					default:
						return;
				}
			}

			switch( pDragButton->GetSlotType() ) 
			{
			case ST_INVENTORY_VEHICLE:
				{
					if( !bPetSummonEnable ) return;

					if( pPressedButton->GetSlotIndex() != PetSlotIndex::PetSlot )
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 ), textcolor::YELLOW, 4.0f );
						return;
					}

					CDnItem *pItem = static_cast<CDnItem*>(pDragButton->GetItem());
					if( pItem &&  pActor->GetLevel() < pItem->GetLevelLimit() )	// 아이템 랩제!
					{
						GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ));
						return;
					}

					if( CDnPetTask::IsActive() )
						GetPetTask().UsePetItem( pItem );
				}
				break;
			case ST_INVENTORY_CASH:
				{
					if( !bPetSummonEnable ) return;

					CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());

					if( pItem->GetItemType() == ITEMTYPE_PETPARTS )
					{
						DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
						if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem( pItem->GetClassID() ) ) 
							break;

						int equipType = -1;
						equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();
						if( ( pPressedButton->GetSlotIndex() == PetSlotIndex::PetPartsSlot1 && equipType != Pet::Parts::PetAccessory1 ) ||
							( pPressedButton->GetSlotIndex() == PetSlotIndex::PetPartsSlot2 && equipType != Pet::Parts::PetAccessory2 ) )
						{
							CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 ), textcolor::YELLOW, 4.0f );
							return;
						}

						int VehicleClassType = -1;
						VehicleClassType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehicleClassID" )->GetInteger();

						// 해당 펫은 그 아이템을 장착 할 수 없습니다.
						if( (pActor->GetPetClassType() != VehicleClassType) || VehicleClassType == -1 )
						{
							CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9236 ), textcolor::YELLOW, 4.0f );
							break;
						}
					}

					GetItemTask().RequestMoveItem( MoveType_CashInvenToPetParts, pDragButton->GetSlotIndex(), pItem->GetSerialID(), nEquipIndexP, CDnInventoryDlg::GetCurrentAmount() );
				}
				break;
			}
		}
		return;
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCharPetDlg::SetAvatarAction( const char* szActionName )
{
	if( m_pPreviewDlg )
		m_pPreviewDlg->SetAvatarAction( szActionName );
}

void CDnCharPetDlg::RefreshPetEquipPreview()
{
	if( m_pPreviewDlg )
		m_pPreviewDlg->RefreshPetEquip();
}

void CDnCharPetDlg::PetSummonedOff()
{
	CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
	if( !pActor )
		return;

	if( GetItemTask().IsRequestWait() ) return;
	if( pActor && pActor->IsSummonPet() )
	{
		CDnItem* pItem = (CDnItem*)m_pPetSlot->GetItem();
		if( !pItem )
			return;

		if( pItem->GetSlotIndex() == Pet::Slot::Body )	// 펫 해제
		{
			GetItemTask().RequestMoveItem( MoveType_PetBodyToPetInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
		}
	}
}

void CDnCharPetDlg::RemovePetInfoDetail()
{
	m_pStaticPetName->ClearText();
	m_pStaticPetLevel->ClearText();
	m_pStaticPetExpireDate->ClearText();
	m_pStaticPetExp->ClearText();
	m_pProgressBarPetExp->SetProgress( 0.0f );
	m_pStaticPetSatiety->ClearText();
	m_pProgressBarPetSatiety->SetProgress( 0.0f );
	m_nPetLevelTableID = 0;
	char szType[32]={0,};
	for( int i=0; i<MAX_PET_SKILL; ++i )
	{
		m_pSkillSlotButton[i]->Show( false );
		m_pBackgroundSkillSlot[i]->Show( false );
	}

	CEtUIStatic* pText = GetControl<CEtUIStatic>( "ID_TEXT2" );
	pText->Show( true );
	m_pStaticPetExpireDate->Show( true );
	m_pStaticPetSatiety->Show( false );
	m_pProgressBarPetSatiety->Show( false );
}

void CDnCharPetDlg::SetPetInfoDetail()
{
	if( !IsShow() )
		return;

	RemovePetInfoDetail();

	if( CDnActor::s_hLocalActor )
	{
		CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if( pLocalPlayer == NULL ) return;
		TVehicleCompact* pPetCompact = &pLocalPlayer->GetPetInfo();
		if( pPetCompact == NULL || pPetCompact->Vehicle[Pet::Slot::Body].nItemID == 0 ) return;

		m_pStaticPetName->SetText( pPetCompact->wszNickName );

		if( pPetCompact->nType & Pet::Type::ePETTYPE_EXPIREDATE )
		{
			CEtUIStatic* pText = GetControl<CEtUIStatic>( "ID_TEXT2" );
			pText->Show( true );
			m_pStaticPetExpireDate->Show( true );
			// 만료 기간 셋팅
			WCHAR dateString[256] = {0,};
			CommonUtil::GetCashRemainDate( pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate, dateString, 256 );

			std::wstring stringProcessed;
			CommonUtil::ConvertTimeFormatString(dateString, stringProcessed);

			m_pStaticPetExpireDate->SetText( stringProcessed );
		}
		else
		{
			CEtUIStatic* pText = GetControl<CEtUIStatic>( "ID_TEXT2" );
			pText->Show( false );
			m_pStaticPetExpireDate->Show( false );

		}
		
		if( pPetCompact->nType & Pet::Type::ePETTYPE_SATIETY )
		{
			m_pStaticPetSatiety->Show( true );
			m_pProgressBarPetSatiety->Show( true );

			SetSatietyInfo();
		}
		else
		{
			m_pStaticPetSatiety->Show( false );
			m_pProgressBarPetSatiety->Show( false );
		}

		// 펫 레벨, 경험치 셋팅
		m_nPetLevelTableID = GetPetTask().GetPetLevelTableIndex( pLocalPlayer->GetUniqueID() );
		if( m_nPetLevelTableID == -1 ) return;

		DNTableFileFormat* pSoxPetLevel = GetDNTable( CDnTableDB::TPETLEVEL );
		if( pSoxPetLevel == NULL ) return;

		int nPetLevel = pSoxPetLevel->GetFieldFromLablePtr( m_nPetLevelTableID, "_PetLevel" )->GetInteger();
		m_pStaticPetLevel->SetText( FormatW( L"%d", nPetLevel ) );

		if( pPetCompact->nExp >= GetPetTask().GetPetMaxLevelExp( pPetCompact->Vehicle[Pet::Slot::Body].nItemID ) )
		{
			m_pStaticPetExp->SetText( L"Max" );
		}
		else
		{
			int nCurrentExp = 0;
			int nLevelUpExp = 0;
			if( nPetLevel == 1 )
			{
				nCurrentExp = pPetCompact->nExp;
				nLevelUpExp = pSoxPetLevel->GetFieldFromLablePtr( m_nPetLevelTableID, "_PetExp" )->GetInteger();
			}
			else
			{
				int nPrevExp = pSoxPetLevel->GetFieldFromLablePtr( m_nPetLevelTableID - 1, "_PetExp" )->GetInteger();
				nCurrentExp = pPetCompact->nExp - nPrevExp;
				nLevelUpExp = pSoxPetLevel->GetFieldFromLablePtr( m_nPetLevelTableID, "_PetExp" )->GetInteger() - nPrevExp;
			}

			if( nLevelUpExp != 0 )
			{
				float fExpPercent = static_cast<float>( nCurrentExp ) / static_cast<float>( nLevelUpExp ) * 100.0f;

				m_pProgressBarPetExp->SetProgress( fExpPercent );
				m_pProgressBarPetExp->Show( true );
				WCHAR wszText[256] = {0,};
				wsprintf( wszText, L"%d / %d", nCurrentExp, nLevelUpExp );
				m_pStaticPetExp->SetText( wszText );
			}
		}
		//스킬 슬롯
		if( pPetCompact->bSkillSlot )
		{
			char szType[32]={0,};
			for( int i=0; i<MAX_PET_SKILL; ++i )
			{
				m_pBackgroundSkillSlot[i]->Show( true );
			}
		}
		else
		{
			m_pBackgroundSkillSlot[0]->Show( true );
		}

		if( pPetCompact->nSkillID1 > 0 )
		{
			if( m_hSkill1 && m_hSkill1->GetClassID() != pPetCompact->nSkillID1 )
				SAFE_RELEASE_SPTR( m_hSkill1 );

			if( m_hSkill1 == NULL )
			{
				m_hSkill1 = CDnSkill::CreateSkill( CDnActor::Identity(), pPetCompact->nSkillID1, 0 );
				if( m_hSkill1 )
				{
					m_hSkill1->SetHasActor( CDnActor::s_hLocalActor );
					m_hSkill1->Acquire();
					((CDnQuickSlotButton*)m_pSkillSlotButton[0])->SetQuickItem( m_hSkill1 );
				}
			}
			m_pSkillSlotButton[0]->Show( true );
		}

		if( pPetCompact->bSkillSlot && pPetCompact->nSkillID2 > 0 )
		{
			if( m_hSkill2 && m_hSkill2->GetClassID() != pPetCompact->nSkillID2 )
				SAFE_RELEASE_SPTR( m_hSkill2 );

			if( m_hSkill2 == NULL )
			{
				m_hSkill2 = CDnSkill::CreateSkill( CDnActor::Identity(), pPetCompact->nSkillID2, 0 );
				if( m_hSkill2 )
				{
					m_hSkill2->SetHasActor( CDnActor::s_hLocalActor );
					m_hSkill2->Acquire();
					((CDnQuickSlotButton*)m_pSkillSlotButton[1])->SetQuickItem( m_hSkill2 );
				}
			}
			m_pSkillSlotButton[1]->Show( true );
		}
	}
}

void CDnCharPetDlg::SetEnableMode( bool bEnable )
{
	if( IsShow() )
		ShowChildDialog( m_pPreviewDlg, bEnable );
	if( m_pCancelButton )
		m_pCancelButton->Enable( bEnable );
}

void CDnCharPetDlg::SetSatietyInfo()
{
	m_pProgressBarPetSatiety->SetProgress( GetPetTask().GetSatietyPercent() );
	
	WCHAR wszText[256] = {0,};
	wsprintf( wszText, L"%d / %d [%d%%]", GetPetTask().GetCurrentSatiety(), GetPetTask().GetMaxSatiety(), 
			static_cast<int>( GetPetTask().GetSatietyPercent() ) );

	m_pStaticPetSatiety->SetText( wszText );
}

void CDnCharPetDlg::DisablePetSkill( int nSkillNum )
{
	if( nSkillNum == 1 && m_hSkill1 )
		m_hSkill1->SetHasActor( CDnActor::Identity() );
	
	if( nSkillNum == 2 && m_hSkill2 )
		m_hSkill2->SetHasActor( CDnActor::Identity() );
}

void CDnCharPetDlg::EnablePetSkill( int nSkillNum )
{
	if( nSkillNum == 1 && m_hSkill1 )
		m_hSkill1->SetHasActor( CDnActor::s_hLocalActor );

	if( nSkillNum == 2 && m_hSkill2 )
		m_hSkill2->SetHasActor( CDnActor::s_hLocalActor );
}


