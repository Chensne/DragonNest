#include "StdAfx.h"
#include "DnGuildStorageSlotDlg.h"
#include "DnItemTask.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnMessageBox.h"
#include "DnInvenTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildStorageSlotDlg::CDnGuildStorageSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
	, m_pDragButton(NULL)
	, m_pPressedButton(NULL)
{
}

CDnGuildStorageSlotDlg::~CDnGuildStorageSlotDlg(void)
{
}

void CDnGuildStorageSlotDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildStorageSlotDlg.ui" ).c_str(), bShow );
}

void CDnGuildStorageSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnItemSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);

	pItemSlotButton->SetSlotType( ST_STORAGE_GUILD );
	pItemSlotButton->SetSlotIndex( GetSlotStartIndex() + (int)m_vecSlotButton.size() );
	m_vecSlotButton.push_back( pItemSlotButton );
}

void CDnGuildStorageSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_ITEM" ) )
		{
			if( GetItemTask().IsRequestWait() )
				return;

			m_pDragButton = (CDnItemSlotButton*)drag::GetControl();
			m_pPressedButton = (CDnItemSlotButton*)pControl;

			if( m_pPressedButton->GetItemState() == ITEMSLOT_DISABLE ) 
				return;

			if( m_pDragButton == NULL )
			{
				CDnItem *pItem = static_cast<CDnItem *>(m_pPressedButton->GetItem());
				if( !pItem ) return;

				if( uMsg == WM_RBUTTONUP )
				{
					// 인벤토리 창에서 적절한 슬롯을 찾는다.
					// 현재 슬롯이 닫혀있는지 검사하지는 않으므로
					// 차후 아이템이 들어있는채로 UnUsable이 된다면 이 루틴도 수정해야할 것이다.(현재는 인벤토리확장이 기간제가 아니라 일어날 수 없다.)
					int nInsertableSlotIndex = -1;
					std::vector<CDnItem*> vecItem;
					GetItemTask().GetCharInventory().ScanItemFromID( pItem->GetClassID(), &vecItem );
					for( int i = 0; i < (int)vecItem.size(); ++i )
					{
						if( ( vecItem[i]->IsSoulbBound() == pItem->IsSoulbBound() && vecItem[i]->GetSealCount() == pItem->GetSealCount() ) &&
							( vecItem[i]->GetOverlapCount() + pItem->GetOverlapCount() <= pItem->GetMaxOverlapCount() ) )
						{
							nInsertableSlotIndex = vecItem[i]->GetSlotIndex();
							break;
						}
					}

					if( nInsertableSlotIndex == -1 )
					{
						if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG ) )
						{
							GetInterface().MessageBox( 1925, MB_OK );
							return;
						}
						nInsertableSlotIndex = GetInterface().GetEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG );
					}

					if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_TAKEITEM ) == false )
					{
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3914 ), MB_OK );
						return;
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
							return;
						}

						int nRemainCount = nMaxTakeItemCount - GetItemTask().GetTakeGuildWareItemCount();
						if( nRemainCount <= 0 )
						{
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory2, 555 ), MB_OK );
							return;
						}
					}

					GetItemTask().RequestMoveItem(	MoveType_GuildWareToInven, 
													m_pPressedButton->GetSlotIndex(), 
													pItem->GetSerialID(),
													nInsertableSlotIndex,
													pItem->GetOverlapCount() );
					return;
				}

				CDnInventoryDlg::SetSelectAmount( pItem->GetOverlapCount() );

				m_pPressedButton->EnableSplitMode(pItem->GetOverlapCount());
				drag::SetControl(m_pPressedButton);

				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
			}
			else
			{
				CDnItem *pItem = static_cast<CDnItem *>(m_pDragButton->GetItem());
				if( !pItem ) return;

				if( pItem->GetItemType() == ITEMTYPE_QUEST || pItem->IsCashItem() )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK ); // UISTRING : 이동이 불가능한 아이템입니다.

					//m_pDragButton->DisableSplitMode(true);
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					return;
				}

				if( pItem->GetReversion() == CDnItem::Belong && pItem->IsSoulbBound() )
				{
					eItemTypeEnum type = pItem->GetItemType();
					if (type == ITEMTYPE_WEAPON || 
						type == ITEMTYPE_PARTS ||
						type == ITEMTYPE_GLYPH ||
						type == ITEMTYPE_POTENTIAL_JEWEL)
					{
						if( pItem->GetSealCount() > 0 )
						{
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3629 ), MB_OK ); // UISTRING : 재봉인 가능한 귀속 아이템을 넣으려면 봉인의 인장으로 봉인해야 합니다
							drag::Command(UI_DRAG_CMD_CANCEL);
							drag::ReleaseControl();
							return;
						}
					}

					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3836 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					return;
				}
				else if( pItem->GetReversion() == CDnItem::GuildBelong )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3836 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					return;
				}

				if( m_pDragButton != m_pPressedButton )
				{
					RequestMoveItem( m_pDragButton, m_pPressedButton );
				}
				else
				{
					// 제자리 이동 때의 소리 처리.
					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
				}
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildStorageSlotDlg::SetItem( MIInventoryItem *pItem )
{
	int nSlotIndex = pItem->GetSlotIndex()-GetSlotStartIndex();
	ASSERT( nSlotIndex>=0&&"CDnGuildStorageSlotDlg::SetItem" );
	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnGuildStorageSlotDlg::SetItem" );
	m_vecSlotButton[nSlotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
}

void CDnGuildStorageSlotDlg::ResetSlot( int nSlotIndex )
{
	nSlotIndex -= GetSlotStartIndex();
	ASSERT( nSlotIndex>=0&&"CDnGuildStorageSlotDlg::ResetSlot" );
	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnGuildStorageSlotDlg::ResetSlot" );
	m_vecSlotButton[ nSlotIndex ]->ResetSlot();
}

void CDnGuildStorageSlotDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	ASSERT( dwItemCnt <= ITEM_MAX );
	if( dwItemCnt > ITEM_MAX )
		return;

	CDnItemSlotButton *pButton(NULL);
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
}

int CDnGuildStorageSlotDlg::GetSlotStartIndex()
{
	return GetDialogID() * ITEM_MAX;
}

void CDnGuildStorageSlotDlg::RequestMoveItem( CDnSlotButton *pDragButton, CDnItemSlotButton *pPressedButton )
{
	ASSERT(pDragButton&&pPressedButton);

	switch( pDragButton->GetSlotType() )
	{
	case ST_STORAGE_GUILD:
		{
			if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_TAKEITEM ) == false )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3914 ), MB_OK );
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();
				return;
			}

			CDnItem *pDragItem = static_cast<CDnItem *>(pDragButton->GetItem());			
			GetItemTask().RequestMoveItem( MoveType_GuildWare, 
										pDragButton->GetSlotIndex(), 
										pDragItem->GetSerialID(),
										pPressedButton->GetSlotIndex(), 
										CDnInventoryDlg::GetCurrentAmount() );
		}
		break;
	case ST_INVENTORY:
		{
			if( GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_STOREITEM ) == false )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3914 ), MB_OK );
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();
				return;
			}

			CDnItem *pItem = static_cast<CDnItem *>(m_pDragButton->GetItem());
			if( pItem )
			{
				// 플레이어, 길드창고 통함.
				// 요청 보내는 자체가 이미 길드 창고 탭으로 떠 있는 상태임.
				// 창고 통합 작업 이전 클래스와 똑같이 사용하므로 여기서 디파인으로 분류.
				if( !GetInterface().IsEmptySlot(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )
				{
					CDnItem *pPressedItem = static_cast<CDnItem *>(m_pPressedButton->GetItem());
					if( pPressedItem )
					{
						if( pItem->GetClassID() != pPressedItem->GetClassID() )
						{
							drag::Command(UI_DRAG_CMD_CANCEL);
							drag::ReleaseControl();
							return;
						}
					}
				}

				GetItemTask().RequestMoveItem( MoveType_InvenToGuildWare, 
											m_pDragButton->GetSlotIndex(), 
											pItem->GetSerialID(),
											m_pPressedButton->GetSlotIndex(), 
											pItem->GetOverlapCount() );
			}
		}
		break;
	}
}

bool CDnGuildStorageSlotDlg::IsEmptySlot()
{
	if( GetEmptySlot() == -1 )
	{
		return false;
	}

	return true;
}

int CDnGuildStorageSlotDlg::GetEmptySlot()
{
	CDnItemSlotButton *pButton(NULL);

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