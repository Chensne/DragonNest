#include "StdAfx.h"
#include "DnStoreSlotDlg.h"
#include "DnTradeTask.h"
#include "DnItemTask.h"
#include "DnStoreTabDlg.h"
#include "DnInterfaceDlgID.h"
#include "DnInterface.h"
#include "DnStoreConfirmDlg.h"
#include "DnStoreConfirmExDlg.h"
#include "DnInvenTabDlg.h"
#include "DnTooltipDlg.h"
#include "DnMainMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStoreSlotDlg::CDnStoreSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pDragButton(NULL)
	, m_pPressedButton(NULL)
	, m_pStoreConfirmExDlg(NULL)
	, m_pStoreConfirmDlg(NULL)
{
}

CDnStoreSlotDlg::~CDnStoreSlotDlg(void)
{
	SAFE_DELETE(m_pStoreConfirmExDlg);
	SAFE_DELETE(m_pStoreConfirmDlg);
}

void CDnStoreSlotDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StoreSlotDlg.ui" ).c_str(), bShow );
}

void CDnStoreSlotDlg::InitialUpdate()
{
	m_pStoreConfirmExDlg = new CDnStoreConfirmExDlg( UI_TYPE_MODAL, NULL, STORE_CONFIRMEX_DIALOG, this );
	m_pStoreConfirmExDlg->Initialize( false );

	m_pStoreConfirmDlg = new CDnStoreConfirmDlg( UI_TYPE_MODAL, NULL, STORE_CONFIRM_DIALOG, this );
	m_pStoreConfirmDlg->Initialize( false );
}

void CDnStoreSlotDlg::InitCustomControl( CEtUIControl *pControl )
{
	CDnItemSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
	pItemSlotButton->SetSlotType( ST_STORE );
	pItemSlotButton->SetSlotIndex( GetSlotStartIndex() + (int)m_vecSlotButton.size() );

	m_vecSlotButton.push_back( pItemSlotButton );
}

void CDnStoreSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_ITEM" ) )
		{
			if( GetItemTask().IsRequestWait() )
				return;

			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( !pInvenDlg ) return;
			if( !(pInvenDlg->IsShow()) ) return;

			CDnStoreTabDlg *pStoreTabDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
			if( !pStoreTabDlg ) return;
			if( !(pStoreTabDlg->IsShow()) ) return;
			int nCurTabID = pStoreTabDlg->GetCurrentTabID();

			m_pDragButton = (CDnItemSlotButton*)drag::GetControl();
			m_pPressedButton = (CDnItemSlotButton*)pControl;

			if( m_pPressedButton->GetItemState() == ITEMSLOT_DISABLE ) 
				return;

			if( m_pDragButton == NULL )
			{
				CDnItem *pItem = static_cast<CDnItem *>(m_pPressedButton->GetItem());
				if( !pItem ) return;

				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
				if( pInvenDlg && pInvenDlg->IsShow() ) 
				{
					if(pItem->IsCashItem()) // Rotha 상점에서 해당 아이템을 집으면 오른쪽 인벤 다이얼로그를 스왑해줍니다.
					{
						if( pItem->GetItemType() == ITEMTYPE_VEHICLE || pItem->GetItemType() == ITEMTYPE_PET || pItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE)
							pInvenDlg->ShowTab( ST_INVENTORY_VEHICLE );
						else
							pInvenDlg->ShowTab(ST_INVENTORY_CASH);

					}
					else
					{
						pInvenDlg->ShowTab( ST_INVENTORY );
					}
				}

				if( uMsg == ( WM_RBUTTONUP | 0x0010 ) )
				{
					// 쉬프트 우클릭의 경우 등급 여부 상관없이 1묶음 사기.
					GetTradeTask().GetTradeItem().RequestShopBuy( nCurTabID, pItem->GetSlotIndex(), pItem->GetOverlapCount() );
				}
				else if( uMsg == WM_RBUTTONUP )
				{
					if (pStoreTabDlg->IsShowRepurchasableTab())
					{
						const CDnStoreRepurchaseSystem& rs = GetTradeTask().GetTradeItem().GetStoreRepurchaseSystem();
						CDnStoreRepurchaseSystem::eEnableRepurchaseResult rst = rs.CheckEnableRepurchase(*pItem);
						if (rst == CDnStoreRepurchaseSystem::eOK)
						{
							ShowTooltipDlg(m_pPressedButton, false);
							m_pStoreConfirmDlg->SetItem(pItem, CDnInterface::STORE_CONFIRM_BUY);
							m_pStoreConfirmDlg->Show(true);
						}
					}
					else
					{
						if( pItem->GetMaxOverlapCount() > 1 )
						{
							m_pStoreConfirmExDlg->SetItem( m_pPressedButton, CDnInterface::STORE_CONFIRM_BUY );
							m_pStoreConfirmExDlg->Show( true );
						}
						else
						{
							m_pStoreConfirmDlg->SetItem( pItem, CDnInterface::STORE_CONFIRM_BUY );
							m_pStoreConfirmDlg->Show( true );
						}
					}
				}
				else if( ( uMsg & 0x0010 ) && (pItem->GetMaxOverlapCount() > 1) )
				{
					if (pStoreTabDlg->IsShowRepurchasableTab() == false)
					{
						m_pStoreConfirmExDlg->SetItem( m_pPressedButton, CDnInterface::STORE_CONFIRM_BUY );
						m_pStoreConfirmExDlg->Show( true );
					}
				}
				else
				{
					m_pPressedButton->EnableSplitMode(pItem->GetOverlapCount());
					drag::SetControl(m_pPressedButton);

					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				}
			}
			else
			{
				// 스플릿 정책상 서버응답이 오기 전에 미리 드래그모드를 해제하면 안된다.
				//drag::Command(UI_DRAG_CMD_CANCEL);
				//drag::ReleaseControl();
				MIInventoryItem *pDragItem = m_pDragButton->GetItem();
				if( !pDragItem ) return;

				if( pDragItem->GetType() != MIInventoryItem::Item )
				{
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();

					return;
				}

				if( m_pDragButton->GetSlotType() == ST_STORE )
				{
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();

					return;
				}

				CDnItem *pItem = static_cast<CDnItem *>(m_pDragButton->GetItem());
				if( !pItem ) return;

				if (GetTradeTask().GetTradeItem().IsShopSellableItem(*pItem, pItem->GetOverlapCount()) == false)
				{
					GetInterface().ServerMessageBox(ERROR_SHOP_NOTFORSALE);

					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();

					return;
				}

				if( (pItem->GetItemRank() == ITEMRANK_D) && (pItem->GetOverlapCount() == 1) )
				{
					GetTradeTask().GetTradeItem().RequestShopSell(*pItem, 1);//pItem->GetSlotIndex(), 1 );

					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();

					return;
				}

#ifdef PRE_MOD_SELL_SEALEDITEM
#else
				// 귀속아이템이면서 봉인상태의 아이템인지 확인
				if( pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound() )
				{
					GetInterface().MessageBox( 1751, MB_OK );
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					return;
				}
#endif	// #ifdef PRE_MOD_SELL_SEALEDITEM

				//drag::ReleaseControl();
				drag::SetRender(false);

				if( pItem->GetItemType() == ITEMTYPE_QUEST )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
					return;
				}

				if( m_pDragButton != m_pPressedButton )
				{
					if( pItem->GetMaxOverlapCount() > 1 )
					{
						m_pStoreConfirmExDlg->SetItem( m_pDragButton, CDnInterface::STORE_CONFIRM_SELL );
						m_pStoreConfirmExDlg->Show( true );
					}
					else
					{
						m_pStoreConfirmDlg->SetItem( pItem, CDnInterface::STORE_CONFIRM_SELL );
						m_pStoreConfirmDlg->Show( true );
					}
				}
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnStoreSlotDlg::SetItem( MIInventoryItem *pItem )
{
	int nSlotIndex = pItem->GetSlotIndex() % emINVEN::ITEM_MAX;
	if( nSlotIndex < 0 || nSlotIndex > (int)m_vecSlotButton.size() ) return;

	if( m_vecSlotButton[nSlotIndex]->IsEmptySlot() ) {
		m_vecSlotButton[nSlotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		return;
	}

	CDebugSet::ToLogFile( "CDnStoreSlotDlg::SetItem, 빈 슬롯이 없습니다." );
}

int CDnStoreSlotDlg::GetSlotStartIndex()
{
	return GetDialogID() * ITEM_MAX;
}

void CDnStoreSlotDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( nID ) 
		{
		case STORE_CONFIRMEX_DIALOG:
			{
				if( IsCmdControl("ID_OK") )
				{
					CDnItem *pItem = m_pStoreConfirmExDlg->GetItem();
					if (pItem)
					{
						int nCount = m_pStoreConfirmExDlg->GetItemCount();

						if (m_pStoreConfirmExDlg->GetConfirmType() == CDnInterface::STORE_CONFIRM_SELL)
						{
							if (nCount == 0)
							{
								GetInterface().MessageBox(1718, MB_OK);
								return;
							}
							else if (nCount < 0)
							{
								m_pStoreConfirmExDlg->Show(false);
								if (drag::IsValid())
									drag::Command(UI_DRAG_CMD_CANCEL);
								drag::ReleaseControl();
								return;
							}
							else
							{
								m_pStoreConfirmExDlg->Show(false);
								GetTradeTask().GetTradeItem().RequestShopSell(*pItem, nCount);//pItem->GetSlotIndex(), nCount );
								drag::ReleaseControl();
							}
						}
						else
						{
							CDnStoreTabDlg *pStoreDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
							if( !pStoreDlg || !pStoreDlg->IsShow() || pStoreDlg->GetStoreType() < Shop::Type::Combined )
								nCount *= pItem->GetOverlapCount();
							if( nCount == 0 )
							{
								GetInterface().MessageBox( 1717, MB_OK );
								return;
							}
							else if (nCount < 0)
							{
								m_pStoreConfirmExDlg->Show(false);
								if (drag::IsValid())
									drag::Command(UI_DRAG_CMD_CANCEL);
								drag::ReleaseControl();
								return;
							}
							else
							{
								CDnStoreTabDlg *pStoreTabDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
								if( !pStoreTabDlg ) return;
								if( !(pStoreTabDlg->IsShow()) ) return;
								int nCurTabID = pStoreTabDlg->GetCurrentTabID();

								m_pStoreConfirmExDlg->Show( false );
								GetTradeTask().GetTradeItem().RequestShopBuy( nCurTabID, pItem->GetSlotIndex(), nCount );
								drag::ReleaseControl();
							}
						}
					}
				}

				if( IsCmdControl("ID_CANCEL") )
				{
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					m_pStoreConfirmExDlg->Show( false );
				}
			}
			break;
		case STORE_CONFIRM_DIALOG:
			{
				if( IsCmdControl("ID_OK") )
				{
					CDnItem *pItem = m_pStoreConfirmDlg->GetItem();
					if( pItem )
					{
						if( m_pStoreConfirmDlg->GetConfirmType() == CDnInterface::STORE_CONFIRM_SELL )
						{
							m_pStoreConfirmDlg->Show( false );
							GetTradeTask().GetTradeItem().RequestShopSell(*pItem, pItem->GetOverlapCount());// pItem->GetSlotIndex(), pItem->GetOverlapCount() );
							drag::ReleaseControl();
						}
						else
						{
							CDnStoreTabDlg *pStoreTabDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
							if( !pStoreTabDlg ) return;
							if( !(pStoreTabDlg->IsShow()) ) return;

							if (pStoreTabDlg->IsShowRepurchasableTab())
							{
								const CDnStoreRepurchaseSystem& rpsSystem = GetTradeTask().GetTradeItem().GetStoreRepurchaseSystem();
								rpsSystem.RequestRepurchase(*pItem);
							}
							else
							{
								int nCurTabID = pStoreTabDlg->GetCurrentTabID();

								m_pStoreConfirmDlg->Show( false );
								GetTradeTask().GetTradeItem().RequestShopBuy( nCurTabID, pItem->GetSlotIndex(), pItem->GetOverlapCount() );
							}

							drag::ReleaseControl();
						}
					}
				}

				if( IsCmdControl("ID_CANCEL") )
				{
					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
					m_pStoreConfirmDlg->Show( false );
				}
			}
			break;
		}
	}
}

void CDnStoreSlotDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() && CDnTradeTask::IsActive() )
	{
		CDnItem *pSellItem = GetTradeTask().GetTradeItem().GetTradeSellItem();
		CDnItem *pBuyItem = GetTradeTask().GetTradeItem().GetTradeBuyItem();
		CDnItem *pItem(NULL);
		CDnInterface::emSTORE_CONFIRM_TYPE confirmType;

		if( pSellItem && pBuyItem )
		{
			ASSERT( 0&&"CDnStoreSlotDlg::Process, 동시에 값을 가질 수 없다." );
		}

		if( pSellItem )
		{
			GetTradeTask().GetTradeItem().SetTradeSellItem( NULL );

			if (GetTradeTask().GetTradeItem().IsShopSellableItem(*pSellItem, pSellItem->GetOverlapCount()) == false)
			{
				GetInterface().ServerMessageBox(ERROR_SHOP_NOTFORSALE);
			}
#ifdef PRE_MOD_SELL_SEALEDITEM
#else
			else if( pSellItem->GetReversion() > CDnItem::NoReversion && !pSellItem->IsSoulbBound() )
			{
				GetInterface().MessageBox( 1751, MB_OK );
			}
#endif	// #ifdef PRE_MOD_SELL_SEALEDITEM
			else
			{
				confirmType = CDnInterface::STORE_CONFIRM_SELL;
				pItem = pSellItem;
			}
		}
		else if( pBuyItem )
		{
			GetTradeTask().GetTradeItem().SetTradeBuyItem( NULL );
			confirmType = CDnInterface::STORE_CONFIRM_BUY;
			pItem = pBuyItem;
		}

		if( pItem )
		{
			if( pItem->GetMaxOverlapCount() > 1 )
			{
				bool bNeedConfirmExDlg = true;
				const CDnStoreTabDlg *pStoreTabDlg = static_cast<CDnStoreTabDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::STORE_DIALOG));
				if (pStoreTabDlg && pStoreTabDlg->IsShow())
				{
					if (pStoreTabDlg->IsShowRepurchasableTab() && (confirmType == CDnInterface::STORE_CONFIRM_BUY))
					{
						const CDnStoreRepurchaseSystem& rs = GetTradeTask().GetTradeItem().GetStoreRepurchaseSystem();
						CDnStoreRepurchaseSystem::eEnableRepurchaseResult rst = rs.CheckEnableRepurchase(*pItem);
						if (rst == CDnStoreRepurchaseSystem::eOK)
						{
							m_pStoreConfirmDlg->SetItem(pItem, CDnInterface::STORE_CONFIRM_BUY);
							m_pStoreConfirmDlg->Show(true);
							bNeedConfirmExDlg = false;
						}
					}
				}

				if (bNeedConfirmExDlg)
				{
					m_pStoreConfirmExDlg->SetItem( pItem, confirmType, pItem->GetOverlapCount() );
					m_pStoreConfirmExDlg->Show( true );
				}
			}
			else
			{
				m_pStoreConfirmDlg->SetItem( pItem, confirmType );
				m_pStoreConfirmDlg->Show( true );
			}
		}
	}
}

void CDnStoreSlotDlg::ResetAllSlot()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); ++i )
	{
		m_vecSlotButton[i]->ResetSlot();
	}
}

bool CDnStoreSlotDlg::IsEmptySlotDialog()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); ++i )
	{
		if( !m_vecSlotButton[i]->IsEmptySlot() )
			return false;
	}

	return true;
}

bool CDnStoreSlotDlg::IsFullSlotDialog()
{
	for( int i=0; i<(int)m_vecSlotButton.size(); ++i )
	{
		if( m_vecSlotButton[i]->IsEmptySlot() )
			return false;
	}

	return true;
}

void CDnStoreSlotDlg::ResetSlot( int nSlotIndex )
{
	nSlotIndex -= GetSlotStartIndex();
	ASSERT( nSlotIndex>=0&&"CDnStoreSlotDlg::ResetSlot" );
	ASSERT( nSlotIndex<(int)m_vecSlotButton.size()&&"CDnStoreSlotDlg::ResetSlot" );
	m_vecSlotButton[ nSlotIndex ]->ResetSlot();
}

void CDnStoreSlotDlg::ResetRepurchaseSlot(int nSlotIndex)
{
	ResetSlot(nSlotIndex);
	m_pStoreConfirmDlg->Show(false);
}
