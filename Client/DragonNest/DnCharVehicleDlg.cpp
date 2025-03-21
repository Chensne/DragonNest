#include "Stdafx.h"
#include "DnTableDB.h"
#include "DnCharVehicleDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnInvenTabDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnCharVehiclePreviewDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif



CDnCharVehicleDlg::CDnCharVehicleDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pVehicleSlot(NULL)
, m_pVehiclePartsSlot(NULL)
, m_pCancle(NULL)
, m_pTextVehicleInfo(NULL)
, m_pPreviewDlg(NULL)
, m_pVehicleEffectSlot(NULL)
{

}

CDnCharVehicleDlg::~CDnCharVehicleDlg()
{
	SAFE_DELETE(m_pPreviewDlg);
}

void CDnCharVehicleDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("CharRide.ui").c_str(), bShow );
}

void CDnCharVehicleDlg::InitialUpdate()
{
#ifdef _ADD_MAINBAR_NEW
	m_pCancle  = GetControl<CEtUIButton>("ID_BUTTON_CANCEL0");
#else
	m_pCancle  = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
#endif
	//rlkt addon
	GetControl<CEtUIButton>("ID_BT_RIDECANCEL")->Show(false);
	GetControl<CEtUIButton>("ID_BT_OPTION")->Show(false);

	m_pTextVehicleInfo = GetControl<CEtUITextBox>("ID_TEXTBOX_DETAIL");
	
	m_pVehicleSlot = GetControl<CDnItemSlotButton>("ID_BUTTON_RIDE0");
	m_pVehicleSlot->SetSlotIndex(VehicleSlot);
	m_pVehicleSlot->SetSlotType(ST_CHARSTATUS);

	m_pVehiclePartsSlot = GetControl<CDnItemSlotButton>("ID_BUTTON_RIDE1");
	m_pVehiclePartsSlot->SetSlotIndex(VehiclePartsSlot);
	m_pVehiclePartsSlot->SetSlotType(ST_CHARSTATUS);

	m_pVehicleEffectSlot = GetControl<CDnItemSlotButton>("ID_BUTTON_RIDE2");
	m_pVehicleEffectSlot->SetSlotIndex(VehicleEffectSlot);
	m_pVehicleEffectSlot->SetSlotType(ST_CHARSTATUS);

	m_pPreviewDlg = new CDnCharVehiclePreviewDlg( UI_TYPE_CHILD, this );
	m_pPreviewDlg->Initialize( false );
}

void CDnCharVehicleDlg::InitCustomControl( CEtUIControl *pControl )
{
}

void CDnCharVehicleDlg::Show( bool bShow )
{
	CEtUIDialog::Show( bShow );

	bool bShowPreviewDlg = false;
	if(CDnActor::s_hLocalActor) // 콜로세움에서 탭을 켜놓은 채로 나가면 로컬 액터가 먼저 없어진 경우가 생깁니다.
	{
		CDnLocalPlayerActor *pLocalP = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if(bShow && pLocalP && pLocalP->IsVehicleMode() && pLocalP->GetMyVehicleActor())
			bShowPreviewDlg = true;
	}
	else
	{
		CEtUIDialog::Show(false);
		return;
	}

	RefreshVehicleEquip();
	ShowChildDialog( m_pPreviewDlg, bShowPreviewDlg );
}

void CDnCharVehicleDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	ProcessUI();
}


void CDnCharVehicleDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( !IsShow() )
		return;

	if( !CDnActor::s_hLocalActor )
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

		switch( pItem->GetItemType() ) {
			case ITEMTYPE_VEHICLE:
			case ITEMTYPE_VEHICLE_SHARE:
				{
					m_pVehicleSlot->SetWearable( true );
				}
				break;
			case ITEMTYPE_VEHICLEPARTS:
				{
					DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
					if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItem->GetClassID()))
						break;

					int equipType = -1;
					equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();
					
					if(equipType == Vehicle::Parts::Hair)
						break;
					
					m_pVehiclePartsSlot->SetWearable( true );
				}
				break;

			case ITEMTYPE_VEHICLEEFFECT:
 				{
					m_pVehicleEffectSlot->SetWearable(true);
				}
				break;

		}
	} // 슬롯 EnableEquip
	else
	{
		m_pVehicleSlot->SetWearable( false );
		m_pVehiclePartsSlot->SetWearable( false );
		m_pVehicleEffectSlot->SetWearable( false );

	} // 슬롯 리셋

}

void CDnCharVehicleDlg::SetSlotMagneticMode( bool bMagnetic )
{
	m_pVehicleSlot->SetMagnetic( bMagnetic );
	m_pVehiclePartsSlot->SetMagnetic( bMagnetic );
	m_pVehicleEffectSlot->SetMagnetic( bMagnetic );

}

void CDnCharVehicleDlg::ProcessUI()
{
	if(CDnActor::s_hLocalActor )
	{
		CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
		if(pActor && pActor->IsVehicleMode() && pActor->IsCanVehicleMode())
		{
			m_pCancle->Enable(true);
			if(!m_pVehicleSlot || !m_pVehicleSlot->GetItem())
				return;

			DNTableFileFormat* pItemTable = GetDNTable( CDnTableDB::TITEM);
			int nVehicleActorSpeed =  pItemTable->GetFieldFromLablePtr( m_pVehicleSlot->GetItem()->GetClassID(), "_TypeParam2" )->GetInteger();

			m_pTextVehicleInfo->ClearText();
			WCHAR wszMsg[256] = {0,};
			wsprintf( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9215 ), nVehicleActorSpeed );
			m_pTextVehicleInfo->AddText( wszMsg , textcolor::FONT_ORANGE );
			m_pTextVehicleInfo->AddText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9216 ) , textcolor::FONT_GREEN );

			if( m_bShow && !m_pPreviewDlg->IsShow() ) ShowChildDialog( m_pPreviewDlg, true );
		}
		else
		{
			m_pTextVehicleInfo->ClearText();
			m_pCancle->Enable(false);
			if( m_pPreviewDlg->IsShow() ) ShowChildDialog( m_pPreviewDlg, false );
		}
	}
}

void CDnCharVehicleDlg::ForceUnRideVehicle(bool bIgnoreRideCheck)
{
	if(!CDnActor::s_hLocalActor)
		return;
	
	if(GetItemTask().IsRequestWait() )
		return;

	CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
	if(!pActor)
		return;

	if(pActor && (pActor->IsVehicleMode() || bIgnoreRideCheck)) // 말을 내린상태에 이큅이 되어있는 경우가 있고 이럴때는 무시하고 패킷을 보내도록 설정한다.
	{
		CDnItem *pItem = (CDnItem*)m_pVehicleSlot->GetItem();
		if(!pItem)
			return;

		if(pItem->GetSlotIndex() == Vehicle::Slot::Body)
		{
			if( !pActor->IsCanVehicleMode())
			{
				CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
				return;
			}

			GetItemTask().RequestMoveItem( MoveType_VehicleBodyToVehicleInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
		}
	}
}

void CDnCharVehicleDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if(!CDnActor::s_hLocalActor )
		return;;

	CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
	if(!pActor)
		return;

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		CDnSlotButton *pDragButton;
		CDnItemSlotButton *pPressedButton;

		if( GetItemTask().IsRequestWait() ) return;

		if( IsCmdControl("ID_BUTTON_CANCEL0" ) ) 
		{
			if(pActor && pActor->IsVehicleMode())
			{
				CDnItem *pItem = (CDnItem*)m_pVehicleSlot->GetItem();
				if(!pItem)
					return;

				if(pItem->GetSlotIndex() == Vehicle::Slot::Body)
				{
					if( !pActor->IsCanVehicleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						return;
					}

					GetItemTask().RequestMoveItem( MoveType_VehicleBodyToVehicleInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
				}
			}
			return;
		}

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_VEHICLE);
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

			if(!pItem)
				return;

			if( uMsg == WM_RBUTTONUP )
			{
				if(pItem->GetSlotIndex() == Vehicle::Slot::Body)
				{
					if(!pActor->IsCanVehicleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						// 여기선 사용할 수 없습니다.
						return;
					}

					if(pActor->IsVehicleMode())
						GetItemTask().RequestMoveItem( MoveType_VehicleBodyToVehicleInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetPetInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
				}
				else if(pItem->GetSlotIndex() == Vehicle::Slot::Saddle)
				{
					if(!pActor->IsCanVehicleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						// 여기선 사용할 수 없습니다.
						return;
					}

					GetItemTask().RequestMoveItem( MoveType_VehiclePartsToCashInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetCashInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
				}
				else if(pItem->GetSlotIndex() == CASHEQUIP_EFFECT ) // 탈것이펙트 슬롯은 캐릭터 기반이기때문에 틀립니다 주의 !
				{
					if(!pActor->IsCanVehicleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						// 여기선 사용할 수 없습니다.
						return;
					}
					GetItemTask().RequestMoveItem( MoveType_CashEquipToCashInven, pItem->GetSlotIndex(), pItem->GetSerialID(),	GetItemTask().GetCashInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
				}

				return;
			}

			// todo by kalliste : refactoring with DnInvenSlotDlg.cpp
			if (uMsg & VK_SHIFT)
			{
				if (GetInterface().SetNameLinkChat(*pItem))
					return;
			}

			pPressedButton->EnableSplitMode(1);
			drag::SetControl(pPressedButton);

			CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
		}
		else
		{
			pDragButton->DisableSplitMode(true);
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
				case ITEMTYPE_VEHICLE:
				case ITEMTYPE_VEHICLE_SHARE:
				case ITEMTYPE_VEHICLEPARTS:
				case ITEMTYPE_VEHICLEEFFECT:
					break;

				default:
					return;
				}
			}

			switch( pDragButton->GetSlotType() ) 
			{
			case ST_INVENTORY_VEHICLE:
				{

					if(pPressedButton->GetSlotIndex() != VehicleSlotIndex::VehicleSlot)
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 ), textcolor::YELLOW, 4.0f );
						return;
					}

					if( !pActor->IsCanVehicleMode()) 
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						return; // 스테이지서 탑승 불가능
					}

					if( pActor->IsCallingVehicle() )
						return;
		
					if(pActor->IsBattleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9208 ), textcolor::YELLOW, 4.0f );
						return;
					}
					else 
					{
						CDnItem *pItem = static_cast<CDnItem*>(pDragButton->GetItem());
						if(pItem &&  pActor->GetLevel() < pItem->GetLevelLimit())
						{
							GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ));	// 아이템 랩제 !
							return;
						}
					}

					CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
					GetItemTask().RequestMoveItem( MoveType_VehicleInvenToVehicleBody, pDragButton->GetSlotIndex(), pItem->GetSerialID(), nEquipIndexP, CDnInventoryDlg::GetCurrentAmount() );
				}
				break;
			case ST_INVENTORY_CASH:
				{
					CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());


					if(pItem->GetItemType() == ITEMTYPE_VEHICLEPARTS)
					{

						if(pPressedButton->GetSlotIndex() != VehicleSlotIndex::VehiclePartsSlot) 
						{
							CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 ), textcolor::YELLOW, 4.0f );
							return;
						}

						DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
						if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItem->GetClassID())) 
							break;

						int equipType = -1;
						equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();

						int VehicleClassType = -1;
						VehicleClassType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehicleClassID" )->GetInteger();

						// 해당 탈것은 그 아이템을 장착 할 수 없습니다.
						if( (pActor->GetVehicleClassType() != VehicleClassType) ||  VehicleClassType == -1)
						{
							CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9212 ), textcolor::YELLOW, 4.0f );
							break;
						}

						if(equipType == Vehicle::Parts::Hair )
							break;
					}


					if( !pActor->IsCanVehicleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						break; 
					}

					if(pItem->GetItemType() == ITEMTYPE_VEHICLEEFFECT)
					{

						if(pPressedButton->GetSlotIndex() != VehicleSlotIndex::VehicleEffectSlot)
						{
							CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 ), textcolor::YELLOW, 4.0f );
							return;
						}

						GetItemTask().RequestMoveItem( MoveType_CashInvenToCashEquip, pDragButton->GetSlotIndex(), pItem->GetSerialID(), CASHEQUIP_EFFECT, CDnInventoryDlg::GetCurrentAmount() );
						break;
					}

					if( !pActor->IsVehicleMode()) 
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9209 ), textcolor::YELLOW, 4.0f );
						break;
					}

					GetItemTask().RequestMoveItem( MoveType_CashInvenToVehicleParts, pDragButton->GetSlotIndex(), pItem->GetSerialID(), nEquipIndexP, CDnInventoryDlg::GetCurrentAmount() );
				}
				break;
			}
		}
		return;
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCharVehicleDlg::SetVehicleEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	if( nEquipIndex == VehicleSlot )
	{
		m_pVehicleSlot->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		CDnItem *pItem = (CDnItem*)m_pVehicleSlot->GetItem();
		if(pItem)
			pItem->SetSoulBound(true);

		m_pVehicleSlot->OnRefreshTooltip();

	}
	else if(nEquipIndex == VehiclePartsSlot)
	{
		m_pVehiclePartsSlot->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pVehiclePartsSlot->OnRefreshTooltip();
	}
	else if(nEquipIndex == VehicleEffectSlot)
	{
		m_pVehicleEffectSlot->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pVehicleEffectSlot->OnRefreshTooltip();
	}

}

void CDnCharVehicleDlg::RefreshVehicleEquip()
{
	if(!CDnActor::s_hLocalActor)
		return;

	CDnLocalPlayerActor *pLocaPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if(pLocaPlayer)
	{
		CDnItem *pVehicleItem = (CDnItem*)m_pVehicleSlot->GetItem();
		CDnItem *pVehiclePartsItem = (CDnItem*)m_pVehiclePartsSlot->GetItem();
		if(pVehicleItem)
		{
			if(pLocaPlayer->GetVehicleInfo().Vehicle[Vehicle::Slot::Body].nSerial != pVehicleItem->GetSerialID()) // 틀릴경우.
				ResetVehicleEquipSlot(Vehicle::Slot::Body);
		}
		if(pVehiclePartsItem)
		{
			if(pLocaPlayer->GetVehicleInfo().Vehicle[Vehicle::Slot::Saddle].nSerial != pVehiclePartsItem->GetSerialID()) // 틀릴경우.
				ResetVehicleEquipSlot(Vehicle::Slot::Saddle);
		}
	}
}

void CDnCharVehicleDlg::ResetVehicleEquipSlot( int nEquipIndex )
{
	if( nEquipIndex == VehicleSlotIndex::VehicleSlot )
	{
		m_pVehicleSlot->ResetSlot();
		m_pVehicleSlot->OnRefreshTooltip();
	}
	if(nEquipIndex == VehicleSlotIndex::VehiclePartsSlot)
	{
		m_pVehiclePartsSlot->ResetSlot();
		m_pVehiclePartsSlot->OnRefreshTooltip();
	}

	if(nEquipIndex == VehicleSlotIndex::VehicleEffectSlot)
	{
		m_pVehicleEffectSlot->ResetSlot();
		m_pVehicleEffectSlot->OnRefreshTooltip();
	}

}

void CDnCharVehicleDlg::SetAvatarAction(const char* szActionName)
{
	if(m_pPreviewDlg)
	{
		m_pPreviewDlg->SetAvatarAction(szActionName);
	}
}


void CDnCharVehicleDlg::RefreshVehicleEquipPreview()
{
	if(m_pPreviewDlg)
		m_pPreviewDlg->RefreshVehicleEquip();
}
