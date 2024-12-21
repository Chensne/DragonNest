#include "StdAfx.h"
#include "DnGestureQuickSlotDlg.h"
#include "DnGestureButton.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "DnMainDlg.h"
#include "DnTableDB.h"
#include "DNGestureTask.h"
#include "DnItemTask.h"
#include "TaskManager.h"
#include "DnSkillTask.h"
#include "DnMainMenuDlg.h"
#include "DnCharVehicleDlg.h"
#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnLifeSkillFishingTask.h"
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_SECONDARY_SKILL
#include "DnVehicleTask.h"
#include "DnPetTask.h"
#include "DnCharPetDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnLifeSkillQuickSlotDlg::CDnLifeSkillQuickSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_nSkillDragSoundIndex(-1)
, m_nSkillLaydownSoundIndex(-1)
, m_fVehicleCoolTime(0.0f)
, m_nGestureSkillIndex(0)
{
}

CDnLifeSkillQuickSlotDlg::~CDnLifeSkillQuickSlotDlg(void)
{
	if( m_nSkillDragSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nSkillDragSoundIndex );
	}
	if( m_nSkillLaydownSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nSkillLaydownSoundIndex );
	}
	std::map<CDnLifeSkillButton*, LifeSkillEx>::iterator iter = m_mapMatchedButton.begin();
	LifeSkillEx Info;
	for( ; iter != m_mapMatchedButton.end(); ++iter )
	{
		Info = iter->second;
		SAFE_DELETE( Info.pItem );
	}
}

void CDnLifeSkillQuickSlotDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GestureQuickSlotDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10005 );
	if( strlen( szFileName ) > 0 )
	{
		m_nSkillDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}

	szFileName = CDnTableDB::GetInstance().GetFileName( 10006 );
	if( strlen( szFileName ) > 0 )
	{
		m_nSkillLaydownSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}
}

void CDnLifeSkillQuickSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	if( pControl->GetProperty() && pControl->GetProperty()->nCustomControlID != idGestureButton )
	{
		if( m_nGestureSkillIndex >= (int)m_vecSlotButton.size() )
			return;

		((CDnSlotButton *)pControl)->SetSlotType( ST_LIFESKILL_QUICKSLOT_EX );
		((CDnSlotButton *)pControl)->SetSlotIndex( (int)m_mapMatchedButton.size() );
		LifeSkillEx LifeSkillExInfo;
		LifeSkillExInfo.pSlotButton = (CDnQuickSlotButton *)pControl;
		LifeSkillExInfo.pItem = NULL;
		m_mapMatchedButton.insert(std::make_pair(m_vecSlotButton[m_nGestureSkillIndex], LifeSkillExInfo));
		m_nGestureSkillIndex++;
		return;
	}

	CDnLifeSkillButton *pQuickSlotButton(NULL);
	pQuickSlotButton = static_cast<CDnLifeSkillButton*>(pControl);

	SUIControlProperty *pProperty(NULL);
	pProperty = pQuickSlotButton->GetProperty();

	pQuickSlotButton->SetSlotType( ST_LIFESKILL_QUICKSLOT );
	pQuickSlotButton->SetSlotIndex( (int)m_vecSlotButton.size() );

	m_vecSlotButton.push_back( pQuickSlotButton );
}

void CDnLifeSkillQuickSlotDlg::RefreshVehicleItem() // 탈것 같은경우에는 아이템 체크를 해야하는데 . 제스쳐 버튼은 벨리드 체크를 안했는데. 인벤체크를 한번해주도록 한다.
{
	if(m_mapMatchedButton.empty() || m_vecSlotButton.empty())
		return;

	if( m_mapMatchedButton.size() == m_vecSlotButton.size() )
	{
		for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
		{
			if( m_mapMatchedButton[m_vecSlotButton[i]].pItem && m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetItemType() == ITEMTYPE_VEHICLE ||
				m_mapMatchedButton[m_vecSlotButton[i]].pItem &&	m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE)
			{
				if( CDnVehicleTask::IsActive() )
				{
					CDnVehicleTask *pVehicleTask = (CDnVehicleTask *)CTaskManager::GetInstance().GetTask( "VehicleTask" );
					if( pVehicleTask && !pVehicleTask->IsExistVehicle( m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetSerialID() ) ) // 씨리얼 아이디로 구분
						ResetSlot( i,true );
				}
			}

			if( m_mapMatchedButton[m_vecSlotButton[i]].pItem && m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetItemType() == ITEMTYPE_PET )
			{
				if( CDnVehicleTask::IsActive() )
				{
					CDnVehicleTask *pVehicleTask = (CDnVehicleTask *)CTaskManager::GetInstance().GetTask( "VehicleTask" );
					if( pVehicleTask && !pVehicleTask->IsExistVehicle( m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetSerialID() ) ) // 씨리얼 아이디로 구분
						ResetSlot( i,true );

					TVehicleCompact* pPetCompact = pVehicleTask->GetVehicleInfoFromSerial( m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetSerialID() );
					__time64_t tExpireDate = pPetCompact && m_mapMatchedButton[m_vecSlotButton[i]].pItem->GetExpireDate();
					if( pPetCompact && tExpireDate != pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )
						m_mapMatchedButton[m_vecSlotButton[i]].pItem->SetExpireDate( pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate );
				}
			}
		}
	}
}

void CDnLifeSkillQuickSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				if( strstr( pControl->GetControlName(), "ID_QUICKSLOT" ) )
				{
					CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
					CDnSlotButton *pPressedButton = static_cast<CDnSlotButton*>(pControl);

					if( pDragButton == NULL )
					{
						if( !pPressedButton->GetItemID() )
							return;

						pPressedButton->EnableSplitMode(CDnSlotButton::ITEM_ORIGINAL_COUNT);
						drag::SetControl(pPressedButton);

						// 퀵슬롯에서 선택해도 소리가 나게 하자.
						switch( pPressedButton->GetItemType() )
						{
						case MIInventoryItem::Skill:
							_ASSERT(0&&"Radio퀵슬롯에선 Skill 집을 수 없다.");
							break;
						case MIInventoryItem::Item:
						case MIInventoryItem::Gesture:
						case MIInventoryItem::SecondarySkill:
							if( m_nSkillDragSoundIndex != -1 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillDragSoundIndex );
							break;
						}
					}
					else
					{
						if( pDragButton == pPressedButton )
						{
							pDragButton->DisableSplitMode(true);
							drag::ReleaseControl();

							// 같은 자리로 이동되는 것도 소리가 나게 하자.
							switch( pDragButton->GetItemType() )
							{
							case MIInventoryItem::Skill:
								break;
							case MIInventoryItem::Item:
							case MIInventoryItem::Gesture:
							case MIInventoryItem::SecondarySkill:
								if( m_nSkillDragSoundIndex != -1 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillDragSoundIndex );
								break;
							}
						}
						else
						{
							ITEM_SLOT_TYPE soltType = pDragButton->GetSlotType();
							switch( soltType )
							{
							case ST_GESTURE:
							case ST_SECONDARYSKILL:
								{
									MIInventoryItem::InvenItemTypeEnum ItemType = (soltType == ST_GESTURE) ? MIInventoryItem::Gesture : MIInventoryItem::SecondarySkill;
									ResetSlot( FindLifeSkillSlotItem( pDragButton->GetItemID(), ItemType ) );
									SetSlot( pDragButton, pPressedButton );

									if( m_nSkillLaydownSoundIndex != -1 )
										CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
								}
								break;
							case ST_INVENTORY_VEHICLE:
								{
									ResetSlot( FindVehicleSlotItem( dynamic_cast<CDnItem*>( pDragButton->GetItem() )->GetSerialID(), MIInventoryItem::Item ) );
									SetSlot( pDragButton, pPressedButton );

									if( m_nSkillLaydownSoundIndex != -1 )
										CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
								}
								break;
							case ST_INVENTORY:
							case ST_SKILL:
							case ST_QUICKSLOT:
								break;
							case ST_LIFESKILL_QUICKSLOT:
							case ST_LIFESKILL_QUICKSLOT_EX:
								{
									// 퀵슬롯 전환
									ChangeQuickSlotButton( pDragButton, pPressedButton );

									if( m_nSkillLaydownSoundIndex != -1 )
										CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
								}
								break;
							}
						}
					}

					return;
				}
			}
			break;
		case WM_RBUTTONUP:
		case WM_KEYDOWN:
			{
				if( !CDnActor::s_hLocalActor ) break;
				if( CDnLocalPlayerActor::IsLockInput() ) break;

				CDnSlotButton *pButton = static_cast<CDnSlotButton*>(pControl);
				if( pButton->GetSlotType() == ST_LIFESKILL_QUICKSLOT )
				{
					// 단축키로 실행할 경우 컨트롤인덱스가 빠른 LifeSkillButton이 항상 먼저 체크되게 된다. 이걸 대비해서 탈것슬롯이 등록되어있는지도 확인해본다.
					if( !pButton->GetItemID() )
					{
						if( m_mapMatchedButton.size() == m_vecSlotButton.size() && m_mapMatchedButton[(CDnLifeSkillButton*)pButton].pSlotButton->IsShow() )
							pButton = m_mapMatchedButton[(CDnLifeSkillButton*)pButton].pSlotButton;
					}
				}
				if( !pButton->GetItemID() ) break;

				if( pButton->GetSlotType() == ST_LIFESKILL_QUICKSLOT )
				{
					switch( pButton->GetItemType() )
					{
					case MIInventoryItem::Gesture:
						GetGestureTask().UseGesture( pButton->GetItemID() );
						break;
#if defined (PRE_ADD_SECONDARY_SKILL)
					case MIInventoryItem::SecondarySkill:
						GetSkillTask().DoLifeSkill( pButton->GetItemID() );
						break;
#endif // PRE_ADD_SECONDARY_SKILL
					}
				}
				else if( pButton->GetSlotType() == ST_LIFESKILL_QUICKSLOT_EX )
				{
					CDnItem *pItem = (CDnItem *)pButton->GetItem();
					CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());

					if( !pItem || !pActor ) return;
					
					if( pItem->GetItemType() == ITEMTYPE_PET )
					{
						int nCashInvenSlotIndex = -1;
						CDnItem* pPetInvenItem = NULL;
						CDnPetInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetPetInventory().GetInventoryItemList().begin();
						for( ; iter != GetItemTask().GetPetInventory().GetInventoryItemList().end(); ++iter )
						{
							pPetInvenItem = iter->second;
							if( !pPetInvenItem ) continue;
							if( pPetInvenItem->GetSerialID() == pItem->GetSerialID() )
							{
								nCashInvenSlotIndex = iter->first;
								break;
							}
						}

						if( nCashInvenSlotIndex > -1 )
						{
							pItem->SetSlotIndex( nCashInvenSlotIndex );
							GetPetTask().UsePetItem( pItem, true );
						}
						else if( pActor->IsSummonPet() && pItem->GetSerialID() == pActor->GetPetInfo().Vehicle[Pet::Slot::Body].nSerial )	// 소환된 펫 사용 시 펫 해제 한다
						{
							if( !GetPetTask().CheckCanMovePet( true ) ) return;
							CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
							if( pCharStatusDlg )
							{
								CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
								if( pCharPetDlg ) pCharPetDlg->PetSummonedOff();
							}
						}

						return;
					}

					if( m_fVehicleCoolTime > 0.0f )
						break;

					if( pActor->IsSwapSingleSkin() )
						break;
					
					if( pActor->IsCanVehicleMode() )
					{
						if( pActor->GetVehicleInfo().Vehicle[Vehicle::Slot::Body].nSerial == pItem->GetSerialID() )
						{
							CDnCharStatusDlg *pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
							CDnCharVehicleDlg *pCharVehicleDlg = pCharStatusDlg->GetVehicleDlg();
							if( pCharVehicleDlg ) pCharVehicleDlg->ForceUnRideVehicle();
						}
						else
						{
							if( pActor->IsCallingVehicle() )
								break;

							if( pActor->GetLevel() < pItem->GetLevelLimit() )
							{
								CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ), textcolor::YELLOW, 4.0f );
								break;
							}

							int nCashInvenSlotIndex = -1;
							CDnItem *pPetInvenItem(NULL);
							CDnPetInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetPetInventory().GetInventoryItemList().begin();
							for( ; iter != GetItemTask().GetPetInventory().GetInventoryItemList().end(); ++iter )
							{
								pPetInvenItem = iter->second;
								if( !pPetInvenItem ) continue;
								if( pPetInvenItem->GetSerialID() == pItem->GetSerialID() )
								{
									nCashInvenSlotIndex = iter->first;
									break;
								}
							}

#ifdef PRE_ADD_SECONDARY_SKILL
							if( nCashInvenSlotIndex != -1 && !GetItemTask().IsRequestWait() 
								&& !GetLifeSkillFishingTask().IsRequestFishingReady() && !GetLifeSkillFishingTask().IsNowFishing()
								&& !GetLifeSkillCookingTask().IsRequestCooking() && !GetLifeSkillCookingTask().IsNowCooking() )
#else // PRE_ADD_SECONDARY_SKILL
							if( nCashInvenSlotIndex != -1 && !GetItemTask().IsRequestWait() )
#endif // PRE_ADD_SECONDARY_SKILL
								GetItemTask().RequestMoveItem( MoveType_VehicleInvenToVehicleBody, nCashInvenSlotIndex, pItem->GetSerialID(), Vehicle::Slot::Body, 1 );
						}
					}
					else
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
					}
					m_fVehicleCoolTime += 1.0f;
				}
			}
			break;
		}
	}
}

void CDnLifeSkillQuickSlotDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( m_fVehicleCoolTime > 0.0f )
		m_fVehicleCoolTime -= fElapsedTime;

	if( !drag::IsValid() || !CDnActor::s_hLocalActor )
		return;

	// 제스처 슬롯안에 마우스 들어왔는지 검사
	bool bMouseEnter = false;
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i]->IsMouseEnter() )
		{
			bMouseEnter = true;
			break;
		}
	}

	if( !bMouseEnter )
		return;

	// 들어온 슬롯이 아이템 중 슬롯에 넣을 수 있는 아이템(물약류)인지 검사.
	CDnSlotButton* pDragButton = (CDnSlotButton*)drag::GetControl();
	if( pDragButton->GetItemType() != MIInventoryItem::Item )
		return;

	// 여긴 일반적인 경우엔 안걸린다.
	// 상점에 팔거나 해서 리프레쉬 인벤을 통해 아이템은 삭제되었는데도,
	// drag::GetControl에 해당 슬롯버튼이 설정되있거나 했을때 들어온다.
	if( !pDragButton->GetItem() )
	{
		_ASSERT(0&&"GestureQuickSlotDlg::Process. drag::GetConrol엔 값이 있는데 m_pItem은 삭제되어있습니다.");
		return;
	}

	if( !pDragButton->GetItem()->IsQuickSlotItem() )
		return;

	CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
	if( pActor->IsBattleMode() )
		return;

	if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
	{
		if( pActor->IsCanBattleMode() )
		{
			if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
			pActor->CmdToggleBattle( true );
		}
	}	
}

bool CDnLifeSkillQuickSlotDlg::SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	ASSERT(pDragButton&&"CDnGestureQuickSlotDlg::SetSlot, pDragButton is NULL!");
	ASSERT(pPressedButton&&"CDnGestureQuickSlotDlg::SetSlot, pPressedButton is NULL");

	if( pDragButton->GetItemID() == 0 )
		return false;

	int nIndex = pPressedButton->GetSlotIndex();
	ResetSlot( nIndex, false );

	INT64 biID = pDragButton->GetItemID();
	if( pDragButton->GetItemType() == MIInventoryItem::Gesture )
	{
		m_vecSlotButton[nIndex]->SetGestureInfo( pDragButton->GetItemID() );
	}
	else if( pDragButton->GetItemType() == MIInventoryItem::SecondarySkill )
	{
		m_vecSlotButton[nIndex]->SetSecondarySkillInfo( pDragButton->GetItemID() );
	}
	else if( pDragButton->GetItemType() == MIInventoryItem::Item )
	{
		if( m_mapMatchedButton.size() == m_vecSlotButton.size() )
		{
			TItemInfo itemInfo;
			((CDnItem*)pDragButton->GetItem())->GetTItemInfo(itemInfo);
			itemInfo.Item.bEternity = ((CDnItem*)pDragButton->GetItem())->IsEternityItem();
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem = GetItemTask().CreateItem(itemInfo);
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->SetQuickItem( m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem );
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->Show( true );
			focus::ReleaseControl();
			biID = ((CDnItem*)pDragButton->GetItem())->GetSerialID();
		}
	}

	pDragButton->DisableSplitMode(false);
	drag::ReleaseControl();

	GetItemTask().RequestAddLifeSkillQuickSlot( pPressedButton->GetSlotIndex(), pDragButton->GetItemType(), biID );

	return true;
}

bool CDnLifeSkillQuickSlotDlg::InitSlot( int nIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType )
{
	if( nIndex >= static_cast<int>( m_vecSlotButton.size() ) )
		return false;

	// 처음 들어왔을때 서버에서 리스트 받으면 이 함수로 설정한다.
	if( ItemType != MIInventoryItem::Item )
	{
		ASSERT(nLifeSkillID&&"CDnLifeSkillQuickSlotDlg::InitSlot");
		if( !nLifeSkillID ) false;
	}
	else
	{
		if( !pItem ) false;
	}

	if( m_vecSlotButton[nIndex]->GetItemID() )
	{
		ASSERT(0&&"빈 슬롯이 아닙니다." );
		return false;
	}

	if( ItemType == MIInventoryItem::Gesture )
		m_vecSlotButton[nIndex]->SetGestureInfo( nLifeSkillID );
	else if( ItemType == MIInventoryItem::SecondarySkill )
		m_vecSlotButton[nIndex]->SetSecondarySkillInfo( nLifeSkillID );
	else if( ItemType == MIInventoryItem::Item )
	{
		if( m_mapMatchedButton.size() == m_vecSlotButton.size() )
		{
			if (pItem == NULL)
			{
				ASSERT(0&&"pItem NULL" );
				return false;
			}

			TItemInfo itemInfo;
			((CDnItem*)pItem)->GetTItemInfo(itemInfo);
			itemInfo.Item.bEternity = ((CDnItem*)pItem)->IsEternityItem();
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem = GetItemTask().CreateItem(itemInfo);
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->SetQuickItem( m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem );
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->Show( true );
			focus::ReleaseControl();
		}
	}
	return true;
}

bool CDnLifeSkillQuickSlotDlg::ResetSecondarySkillQuickSlot( int nSecondarySkillID )
{
	// 임의 삭제가 되는 구조라서 이렇게 해야한다.
	ASSERT(nSecondarySkillID&&"CDnLifeSkillQuickSlotDlg::RefreshSecondarySkillQuickSlot");
	if( !nSecondarySkillID ) false;
	ResetSlot( FindLifeSkillSlotItem( nSecondarySkillID, MIInventoryItem::SecondarySkill ) );
	return true;
}

int CDnLifeSkillQuickSlotDlg::FindLifeSkillSlotItem( int nLifeSkillID, MIInventoryItem::InvenItemTypeEnum ItemType )
{
	ASSERT( nLifeSkillID&&"CDnGestureQuickSlotDlg::FindQuickSlotItem, ID is 0!" );

	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_mapMatchedButton.size() == m_vecSlotButton.size() && m_mapMatchedButton[m_vecSlotButton[i]].pSlotButton->IsShow() && m_mapMatchedButton[m_vecSlotButton[i]].pSlotButton->GetItemID() == nLifeSkillID )
			return i;

		if( !m_vecSlotButton[i]->GetItemID() )
			continue;

		if( m_vecSlotButton[i]->GetItemType() != ItemType )
			continue;

		if( m_vecSlotButton[i]->GetItemID() == nLifeSkillID )
			return i;
	}

	return -1;
}

int CDnLifeSkillQuickSlotDlg::FindVehicleSlotItem( INT64 nItemSerialID, MIInventoryItem::InvenItemTypeEnum ItemType )
{
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_mapMatchedButton.size() == m_vecSlotButton.size() && m_mapMatchedButton[m_vecSlotButton[i]].pSlotButton->IsShow()
			&& dynamic_cast<CDnItem*>( m_mapMatchedButton[m_vecSlotButton[i]].pSlotButton->GetItem() )->GetSerialID() == nItemSerialID )
			return i;

		if( !m_vecSlotButton[i]->GetItemID() )
			continue;

		if( m_vecSlotButton[i]->GetItemType() != ItemType )
			continue;

		if (dynamic_cast<CDnItem*>( m_vecSlotButton[i]->GetItem() ) == NULL)
			continue;

		if( dynamic_cast<CDnItem*>( m_vecSlotButton[i]->GetItem() )->GetSerialID() == nItemSerialID )
			return i;
	}

	return -1;
}

void CDnLifeSkillQuickSlotDlg::ResetSlot( int nIndex, bool bSendPacket )
{
	if( m_vecSlotButton.empty() ) return;
	if( nIndex < 0 ) return;
	if( nIndex >= (int)m_vecSlotButton.size() ) return;

	if( bSendPacket ) GetItemTask().RequestDelLifeSkillQuickSlot( m_vecSlotButton[nIndex]->GetSlotIndex() );
	if( m_mapMatchedButton.size() == m_vecSlotButton.size() && m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->IsShow() )
	{
		SAFE_DELETE( m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem );
		m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->ResetSlot();
		m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->Show( false );
		focus::ReleaseControl();
	}
	else
	{
		m_vecSlotButton[nIndex]->ResetSlot();
	}
}

void CDnLifeSkillQuickSlotDlg::ChangeQuickSlotButton( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	if( !pDragButton || !pPressedButton )
		return;

	int nDragLifeSkillID = pDragButton->GetItemID();
	int nPressedLifeSkillID = pPressedButton->GetItemID();
	MIInventoryItem *pDragItem = pDragButton->GetItem();
	MIInventoryItem *pPressedItem = pPressedButton->GetItem();
	MIInventoryItem::InvenItemTypeEnum DragItemType = pDragButton->GetItemType();
	MIInventoryItem::InvenItemTypeEnum PressedItemType = pPressedButton->GetItemType();

	if( !nDragLifeSkillID )
		return;

	// PressedButton이 탈것 퀵슬롯이라면, 내부적으로 가지고 있는 DnItem객체가
	// 아래 먼저 호출되는 DragItem-SetSlot에 의해 삭제될 것이다.
	// 그럼 Pressed를 DragSlotIndex로 옮기는 과정에서 pPressedItem값이 이미 삭제되어있기 때문에, 덤프가 발생한다.
	// 이걸 막기 위해 임시객체를 만들어 전달하도록 한다.
	CDnItem *pTempPressedItem = NULL;
	if( pPressedButton->GetSlotType() == ST_LIFESKILL_QUICKSLOT_EX && pPressedItem )
	{
		TItemInfo itemInfo;
		((CDnItem*)pPressedItem)->GetTItemInfo(itemInfo);
		itemInfo.Item.bEternity = ((CDnItem*)pPressedItem)->IsEternityItem();
		pTempPressedItem = GetItemTask().CreateItem(itemInfo);
		pPressedItem = pTempPressedItem;
	}

	int nSlotIndex = pPressedButton->GetSlotIndex();
	SetSlot( nSlotIndex, nDragLifeSkillID, pDragItem, DragItemType );

	if( nPressedLifeSkillID )
	{
		nSlotIndex = pDragButton->GetSlotIndex();
		SetSlot( nSlotIndex, nPressedLifeSkillID, pPressedItem, PressedItemType );
	}

	SAFE_DELETE( pTempPressedItem );

	pDragButton->DisableSplitMode(false);
	pPressedButton->DisableSplitMode(false);
	drag::ReleaseControl();
}

bool CDnLifeSkillQuickSlotDlg::SetSlot( int nIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType )
{
	ASSERT(nLifeSkillID&&"CDnRadioMsgQuickSlotDlg::SetSlot");
	if( !nLifeSkillID ) false;

	TItemInfo itemInfo;
	memset(&itemInfo,0,sizeof(itemInfo));

	if( ItemType == MIInventoryItem::Item && pItem )
	{
		((CDnItem*)pItem)->GetTItemInfo(itemInfo);
		itemInfo.Item.bEternity = ((CDnItem*)pItem)->IsEternityItem();
	}

	if( pItem && ( dynamic_cast<CDnItem*>( pItem )->GetItemType() == ITEMTYPE_VEHICLE || dynamic_cast<CDnItem*>( pItem )->GetItemType() == ITEMTYPE_PET || dynamic_cast<CDnItem*>(pItem)->GetItemType() == ITEMTYPE_VEHICLE_SHARE ) )
		ResetSlot( FindVehicleSlotItem( dynamic_cast<CDnItem*>( pItem )->GetSerialID(), ItemType ) );
	else
		ResetSlot( FindLifeSkillSlotItem( nLifeSkillID, ItemType ) );

	GetItemTask().RequestDelLifeSkillQuickSlot( m_vecSlotButton[nIndex]->GetSlotIndex() );
	ResetSlot( nIndex, false );

	INT64 biID = nLifeSkillID;
	if( ItemType == MIInventoryItem::Gesture )
		m_vecSlotButton[nIndex]->SetGestureInfo( nLifeSkillID );
	else if( ItemType == MIInventoryItem::SecondarySkill )
		m_vecSlotButton[nIndex]->SetSecondarySkillInfo( nLifeSkillID );
	else if( ItemType == MIInventoryItem::Item )
	{
		if( m_mapMatchedButton.size() == m_vecSlotButton.size() )
		{
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem = GetItemTask().CreateItem(itemInfo);
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->SetQuickItem( m_mapMatchedButton[m_vecSlotButton[nIndex]].pItem );
			m_mapMatchedButton[m_vecSlotButton[nIndex]].pSlotButton->Show( true );
			focus::ReleaseControl();
			biID = itemInfo.Item.nSerial;
		}
	}
	GetItemTask().RequestAddLifeSkillQuickSlot( m_vecSlotButton[nIndex]->GetSlotIndex(), ItemType, biID );

	return true;
}

void CDnLifeSkillQuickSlotDlg::EnableQuickSlot(bool bEnable)
{
	std::vector<CDnLifeSkillButton*>::iterator iter = m_vecSlotButton.begin();
	for (; iter != m_vecSlotButton.end(); ++iter)
	{
		CDnLifeSkillButton* pBtn = (*iter);
		if (pBtn)
			pBtn->Enable(bEnable);
	}
}