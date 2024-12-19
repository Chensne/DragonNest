#include "StdAfx.h"
#include "DnGuildWarSkillSlotDlg.h"
#include "DnQuickSlotButton.h"
#include "DnActor.h"
#include "DnSkill.h"
#include "DnLocalPlayerActor.h"
#include "DnPlayerActor.h"
#include "DnMainDlg.h"
#include "DnTableDB.h"
#include "DnSkillTask.h"
#include "DnPvPGameTask.h"
#include "TaskManager.h"
#include "DnMutatorGuildWar.h"
#include "DnMainMenuDlg.h"
#include "DnCharVehicleDlg.h"
#include "DnGuildWarTask.h"
#include "DnOccupationTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildWarSkillSlotDlg::CDnGuildWarSkillSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_nSkillDragSoundIndex( -1 )
, m_nSkillLaydownSoundIndex( -1 )
, m_bFixedMode( false )
{
}

CDnGuildWarSkillSlotDlg::~CDnGuildWarSkillSlotDlg()
{
	if( m_nSkillDragSoundIndex == -1 ) 
		CEtSoundEngine::GetInstance().RemoveSound( m_nSkillDragSoundIndex );
	
	if( m_nSkillLaydownSoundIndex == -1 ) 
		CEtSoundEngine::GetInstance().RemoveSound( m_nSkillLaydownSoundIndex );
}

void CDnGuildWarSkillSlotDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildSlotDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10005 );
	if( strlen( szFileName ) > 0 )
		m_nSkillDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );

	szFileName = CDnTableDB::GetInstance().GetFileName( 10006 );
	if( strlen( szFileName ) > 0 )
		m_nSkillLaydownSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnGuildWarSkillSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnQuickSlotButton *pQuickSlotButton(NULL);
	pQuickSlotButton = static_cast<CDnQuickSlotButton*>(pControl);

	SUIControlProperty *pProperty(NULL);
	pProperty = pQuickSlotButton->GetProperty();

	int nOrder;
	nOrder = pProperty->szUIName[ strlen( pProperty->szUIName ) - 1 ] - '1';

	if( nOrder == -1 )
	{
		nOrder = 9;
	}

	pQuickSlotButton->SetButtonOrder( nOrder );
	pQuickSlotButton->SetSlotType( ST_QUICKSLOT );
	pQuickSlotButton->SetTabIndex( 0 );
	pQuickSlotButton->SetSlotIndex( (int)m_vecSlotButton.size() );

	BYTE cVK = VK_F1;
	cVK += (BYTE)m_vecSlotButton.size();
	pQuickSlotButton->SetHotKey( cVK );

	m_vecSlotButton.push_back( pQuickSlotButton );
}

void CDnGuildWarSkillSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				if( m_bFixedMode )
					break;

				if( strstr( pControl->GetControlName(), "ID_GUILD_SLOT" ) )
				{
					CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
					CDnSlotButton *pPressedButton = static_cast<CDnSlotButton*>(pControl);

					if( pDragButton == NULL )
					{
						if( pPressedButton->IsEmptySlot() )
							return;

						pPressedButton->EnableSplitMode( 0 );
						drag::SetControl( pPressedButton );

						// 퀵슬롯에서 선택해도 소리가 나게 하자.
						MIInventoryItem *pItem = pPressedButton->GetItem();
						if( !pItem ) return;

						if( pPressedButton->GetItemType() && m_nSkillDragSoundIndex != -1 )
							CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillDragSoundIndex );
					}
					else
					{
						if( pDragButton == pPressedButton )
						{
							pDragButton->DisableSplitMode(true);
							drag::ReleaseControl();

							// 같은 자리로 이동되는 것도 소리가 나게 하자.
							MIInventoryItem *pItem = pDragButton->GetItem();
							if( !pItem ) break;

							if( pDragButton->GetItemType() == MIInventoryItem::Skill && m_nSkillLaydownSoundIndex != -1 )
								CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
						}
						else
						{
							ITEM_SLOT_TYPE soltType = pDragButton->GetSlotType();

							if( soltType == ST_SKILL )
							{
								if( pPressedButton )	// 이미 슬롯에 있는 스킬은 사라지면 안되기 때문에 실패 처리
								{
									MIInventoryItem *pPressItem = pPressedButton->GetItem();
									if( pPressItem )
										return;
								}

								ResetSlot( FindQuickSlotItem( pDragButton->GetItem() ) );
								SetSlot( pDragButton, pPressedButton );

								if( m_nSkillLaydownSoundIndex != -1 )
									CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSkillLaydownSoundIndex );
							}
							else if( soltType == ST_QUICKSLOT )
							{
								if( pDragButton && pPressedButton )
								{
									if( CDnSkillTask::IsActive() && !GetSkillTask().IsGuildWarSkill( pDragButton->GetItemID() ) )
										return;

									drag::ReleaseControl();
									GetSkillTask().SendGuildSkillSwapIndex( pDragButton->GetSlotIndex(), pPressedButton->GetSlotIndex() );
								}
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
				CDnQuickSlotButton* pButton = static_cast<CDnQuickSlotButton*>(pControl);
				if( pButton )
					UseGuildWarSkill( pButton->GetSlotIndex() );
			}
			break;
		}
	}
}

void CDnGuildWarSkillSlotDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

bool CDnGuildWarSkillSlotDlg::SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton )
{
	ASSERT( pDragButton && "CDnGuildWarSkillSlotDlg::SetSlot, pDragButton is NULL!" );
	ASSERT( pPressedButton && "CDnGuildWarSkillSlotDlg::SetSlot, pPressedButton is NULL" );

	MIInventoryItem *pInvenItem = pDragButton->GetItem();
	DnSkillHandle hSkill;
	if( pInvenItem )
		hSkill = GetSkillTask().FindGuildWarSkill( pInvenItem->GetClassID() );

	if( hSkill == NULL ) return false;
	if( !pInvenItem ) return false;
	if( !pInvenItem->IsQuickSlotItem() ) return false;

	((CDnQuickSlotButton*)pPressedButton)->SetQuickItem( pInvenItem );
//	((CDnQuickSlotButton*)pPressedButton)->SetForceUsable( true );

	pDragButton->DisableSplitMode( true );
	drag::ReleaseControl();

	CDnItem *pItem = dynamic_cast<CDnItem *>( pInvenItem );
	if( pItem ) 
		CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	return true;
}

bool CDnGuildWarSkillSlotDlg::InitSlot( int nIndex, MIInventoryItem *pItem )
{
	ASSERT( pItem && "CDnGuildWarSkillSlotDlg::InitSlot" );

	if( !pItem ) return false;
	if( !pItem->IsQuickSlotItem() ) return false;
	if( nIndex >= (int)m_vecSlotButton.size() ) return false;

	if( !m_vecSlotButton[nIndex]->IsEmptySlot() )
	{
		ASSERT( 0 && "빈 슬롯이 아닙니다." );
		return false;
	}

	m_vecSlotButton[nIndex]->SetQuickItem( pItem );
	return true;
}

bool CDnGuildWarSkillSlotDlg::SetSlot( int nIndex, MIInventoryItem *pItem )
{
	ASSERT( pItem && "CDnGuildWarSkillSlotDlg::SetSlot" );

	if( !pItem ) return false;
	if( !pItem->IsQuickSlotItem() )	return false;
	if( nIndex >= (int)m_vecSlotButton.size() ) return false;

	ResetSlot( FindQuickSlotItem( pItem ) );
	m_vecSlotButton[nIndex]->ResetSlot();
	m_vecSlotButton[nIndex]->SetQuickItem( pItem );

	CDnItem *pSetItem = dynamic_cast<CDnItem *>(pItem);
	if( pSetItem )
		CEtSoundEngine::GetInstance().PlaySound( "2D", pSetItem->GetDragSoundIndex() );

	return true;
}

void CDnGuildWarSkillSlotDlg::SwapQuickSlot( int nFromIndex, int nToIndex )
{
	if( nFromIndex < 0 || nFromIndex >= (int)m_vecSlotButton.size() )
		return;

	if( nToIndex < 0 || nToIndex >= (int)m_vecSlotButton.size() )
		return;

	CDnQuickSlotButton* pFromButton = m_vecSlotButton[nFromIndex];
	CDnQuickSlotButton* pToButton = m_vecSlotButton[nToIndex];


	if( pFromButton && pToButton )
	{
		MIInventoryItem *pItem1 = pFromButton->GetItem();
		MIInventoryItem *pItem2 = pToButton->GetItem();

		if( pItem1 )
		{
			SetSlot( pToButton->GetSlotIndex(), pItem1 );
			pFromButton->DisableSplitMode( true );

			if( pItem2 )
			{
				SetSlot( pFromButton->GetSlotIndex(), pItem2 );
				pToButton->DisableSplitMode( true );
			}
		}
	}
}

void CDnGuildWarSkillSlotDlg::ResetAllSlot()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
		m_vecSlotButton[i]->ResetSlot();
}

void CDnGuildWarSkillSlotDlg::OnRefreshSlot()
{
	if( !CDnActor::s_hLocalActor )
		return;

	int nItemID(0);
	MIInventoryItem *pItem(NULL);

	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		nItemID = m_vecSlotButton[i]->GetItemID();
		if( nItemID == 0 ) continue;

		if( m_vecSlotButton[i]->GetItemType() == MIInventoryItem::Skill )
			pItem = CDnActor::s_hLocalActor->FindSkill( nItemID );

		if( !pItem )
		{
			m_vecSlotButton[i]->ResetSlot();
			m_vecSlotButton[i]->SetItemID( nItemID );
		}
		else
		{
			m_vecSlotButton[i]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
		}
	}
}

int CDnGuildWarSkillSlotDlg::FindQuickSlotItem( MIInventoryItem *pItem )
{
	ASSERT( pItem && "CDnGuildWarSkillSlotDlg::FindQuickSlotItem, pItem is NULL!" );

	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i]->IsEmptySlot() )
			continue;

		if( pItem->GetType() != m_vecSlotButton[i]->GetItemType() )
			continue;

		if( m_vecSlotButton[i]->GetItemID() == pItem->GetClassID() )
			return i;
	}

	return -1;
}

int CDnGuildWarSkillSlotDlg::FindEmptyQuickSlotIndex()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); i++ )
	{
		if( m_vecSlotButton[i]->IsEmptySlot() )
			return m_vecSlotButton[i]->GetItemSlotIndex();
	}
	return -1;
}

void CDnGuildWarSkillSlotDlg::ResetSlot( int nIndex )
{
	if( m_vecSlotButton.empty() ) return;
	if( nIndex < 0 ) return;
	if( nIndex >= (int)m_vecSlotButton.size() ) return;

	m_vecSlotButton[nIndex]->ResetSlot();
}

void CDnGuildWarSkillSlotDlg::EnableQuickSlot( bool bEnable )
{
	std::vector<CDnQuickSlotButton*>::iterator iter = m_vecSlotButton.begin();
	for( ; iter != m_vecSlotButton.end(); ++iter )
	{
		CDnQuickSlotButton* pBtn = (*iter);
		if( pBtn && pBtn->GetItem() != NULL )
			pBtn->Enable( bEnable );
	}
}

void CDnGuildWarSkillSlotDlg::UseGuildWarSkill( int nIndex )
{
	if( !CDnActor::s_hLocalActor ) return;
	if( CDnLocalPlayerActor::IsLockInput() ) return;

	if( nIndex < 0 || nIndex >= (int)m_vecSlotButton.size() )
		return;

	CDnQuickSlotButton* pButton = m_vecSlotButton[nIndex];
	if( !pButton ) return;
	MIInventoryItem *pItem = pButton->GetItem();
	if( !pItem ) return;

	bool bCheckResource = false;
	if( CDnOccupationTask::IsActive() )
	{
		DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
		if( pSkillLevelTable == NULL )
			return;
		int nItemID = pSkillLevelTable->GetItemIDFromField( "_SkillIndex", pItem->GetClassID() );
		int nNeedResource = pSkillLevelTable->GetFieldFromLablePtr( nItemID, "_DecreaseTIC" )->GetInteger();

		int nCurrentResource = GetOccupationTask().GetResource( (PvPCommon::Team::eTeam)CDnActor::s_hLocalActor->GetTeam() ); 
		if( nCurrentResource >= nNeedResource )	// 사용 가능한 자원이 있는지 검사
			bCheckResource = true;
	}

	if( !bCheckResource ) return;

	if( pButton->GetItemType() == MIInventoryItem::Skill )
	{
		DnSkillHandle hSkill = GetSkillTask().FindGuildWarSkill( pItem->GetClassID() );
		if( !hSkill ) return;
		if( hSkill->GetSkillType() != CDnSkill::Active ) return;

		CDnSkill::UsingResult eResult = CDnSkill::UsingResult::Failed;
		eResult = static_cast<CDnGuildWarSkill*>(hSkill.GetPointer())->CanExecuteGuildWarSkill();
		if( eResult == CDnSkill::UsingResult::Success )
		{
			CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

			if( !localActor->IsBattleMode() )
				localActor->CmdToggleBattle( true );

			if( localActor->IsVehicleMode() )
			{
				CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
				if( pCharStatusDlg )
				{
					CDnCharVehicleDlg* pCharVehicleDlg = pCharStatusDlg->GetVehicleDlg();

					if( pCharVehicleDlg )
						pCharVehicleDlg->ForceUnRideVehicle();
				}
			}

			GetSkillTask().SendUseGuildWarSkill( pItem->GetClassID() );
		}
	}
}
