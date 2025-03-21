#include "StdAfx.h"

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnWorldServerStorageSlotDlg.h"
#include "DnItemTask.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnMessageBox.h"
#include "DnInvenTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnPGStorageTabDlg.h"
#include "DnWorldServerStorageInventoryDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnWorldServerStorageSlotDlg::CDnWorldServerStorageSlotDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
	, m_pDragButton(NULL)
	, m_pPressedButton(NULL)
{
}

CDnWorldServerStorageSlotDlg::~CDnWorldServerStorageSlotDlg(void)
{
}

void CDnWorldServerStorageSlotDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "StorageSlotDlg.ui" ).c_str(), bShow);
}

void CDnWorldServerStorageSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnItemSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
	pItemSlotButton->SetSlotIndex( GetSlotStartIndex() + (int)m_vecSlotButton.size() );
	m_vecSlotButton.push_back( pItemSlotButton );
}

void CDnWorldServerStorageSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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

				if (uMsg == WM_RBUTTONUP || uMsg == (WM_RBUTTONUP | VK_SHIFT))
				{
					CDnPGStorageTabDlg* pPGStorageTabDlg = static_cast<CDnPGStorageTabDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG));
					if (m_pPressedButton && pPGStorageTabDlg)
					{
						if (m_pPressedButton->GetSlotType() == ST_STORAGE_WORLDSERVER_NORMAL)
						{
							CDnWorldServerStorageInventoryDlg* pWorldServerStorageInvenDlg = static_cast<CDnWorldServerStorageInventoryDlg*>(pPGStorageTabDlg->GetStorageDlg(StorageUIDef::eTAB_WORLDSERVER_NORMAL));
							if (pWorldServerStorageInvenDlg)
							{
								pWorldServerStorageInvenDlg->MoveItemToInventory(m_pPressedButton, uMsg);
							}
						}
						else if (m_pPressedButton->GetSlotType() == ST_STORAGE_WORLDSERVER_CASH)
						{
							CDnWorldServerStorageCashInventoryDlg* pWorldServerStorageCashInvenDlg = static_cast<CDnWorldServerStorageCashInventoryDlg*>(pPGStorageTabDlg->GetStorageDlg(StorageUIDef::eTAB_WORLDSERVER_CASH));
							if (pWorldServerStorageCashInvenDlg)
							{
								pWorldServerStorageCashInvenDlg->MoveItemToInventory(m_pPressedButton, uMsg);
							}
						}
					}

					return;
				}

				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
			}
			else
			{
				CDnItem *pItem = static_cast<CDnItem *>(m_pDragButton->GetItem());
				if (pItem)
					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();

				GetInterface().MessageBox(StorageUIDef::eERR_NODRAG);
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnWorldServerStorageSlotDlg::SetItem( MIInventoryItem *pItem )
{
	int nSlotIndex = pItem->GetSlotIndex()-GetSlotStartIndex();
	ASSERT( nSlotIndex>=0&&"CDnStoreSlotDlg::SetItem" );
	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnStoreSlotDlg::SetItem" );
	m_vecSlotButton[nSlotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
}

void CDnWorldServerStorageSlotDlg::ResetSlot( int nSlotIndex )
{
	nSlotIndex -= GetSlotStartIndex();
	ASSERT( nSlotIndex>=0&&"CDnStoreSlotDlg::ResetSlot" );
	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnStoreSlotDlg::ResetSlot" );
	m_vecSlotButton[ nSlotIndex ]->ResetSlot();
	m_vecSlotButton[ nSlotIndex ]->OnRefreshTooltip();
}

void CDnWorldServerStorageSlotDlg::SetUseItemCnt( DWORD dwItemCnt )
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

int CDnWorldServerStorageSlotDlg::GetSlotStartIndex()
{
	return (GetDialogID() * ITEM_MAX);
}

void CDnWorldServerStorageSlotDlg::RequestMoveItem( CDnSlotButton *pDragButton, CDnItemSlotButton *pPressedButton )
{
	ASSERT(pDragButton&&pPressedButton);

	switch( pDragButton->GetSlotType() )
	{
	case ST_STORAGE_PLAYER:
		{
			CDnItem *pItem = (CDnItem*)pDragButton->GetItem();
			GetItemTask().RequestMoveItem( MoveType_Ware, pDragButton->GetSlotIndex(), pItem->GetSerialID(), pPressedButton->GetSlotIndex(), CDnInventoryDlg::GetCurrentAmount() );
		}
		break;
	case ST_INVENTORY:
		{
			CDnItem *pItem = static_cast<CDnItem *>(m_pDragButton->GetItem());
			if( pItem )
			{
				GetItemTask().RequestMoveItem( MoveType_InvenToWare, m_pDragButton->GetSlotIndex(), pItem->GetSerialID(), m_pPressedButton->GetSlotIndex(), pItem->GetOverlapCount() );
			}
		}
		break;
	}
}

bool CDnWorldServerStorageSlotDlg::IsEmptySlot()
{
	if( GetEmptySlot() == -1 )
	{
		return false;
	}

	return true;
}

int CDnWorldServerStorageSlotDlg::GetEmptySlot()
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

void CDnWorldServerStorageSlotDlg::SetSlotType(ITEM_SLOT_TYPE type)
{
	CDnItemSlotButton* pButton = NULL;
	DWORD dwVecSize = (DWORD)m_vecSlotButton.size();
	DWORD i = 0;

	for( ; i < dwVecSize; i++ )
	{
		pButton = m_vecSlotButton[i];
		if (pButton)
			pButton->SetSlotType(type);
	}
}

#endif // PRE_ADD_ACCOUNT_STORAGE