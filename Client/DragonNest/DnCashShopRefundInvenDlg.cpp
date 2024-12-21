#include "StdAfx.h"

#ifdef PRE_ADD_CASHSHOP_REFUND_CL

#include "DnCashShopRefundInvenDlg.h"
#include "DnInvenSlotDlg.h"
#include "DnInven.h"
#include "DnItemTask.h"
#include "GameOption.h"
#include "DnCashShopDefine.h"
#include "DnCashShopTask.h"
#include "DnCashShopRefundMsgBox.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCashShopRefundInvenDlg::CDnCashShopRefundInvenDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_nCurPage(0)
, m_nMaxPage(0)
, m_pButtonPagePrev(NULL)
, m_pButtonPageNext(NULL)
, m_pButtonRefund(NULL)
, m_pRefundConfirmMsgBox(NULL)
, m_pSlotBG(NULL)
{
}

CDnCashShopRefundInvenDlg::~CDnCashShopRefundInvenDlg(void)
{
	SAFE_DELETE_PVEC( m_pVecInvenSlotDlg );
	SAFE_DELETE(m_pRefundConfirmMsgBox);
}

void CDnCashShopRefundInvenDlg::Initialize( bool bShow )
{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CS_CheckoutInven_refund.ui" ).c_str(), bShow );
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSCheckoutInven_refund.ui" ).c_str(), bShow );
#endif // PRE_ADD_CASHSHOP_RENEWAL
}

void CDnCashShopRefundInvenDlg::InitialUpdate()
{
	m_pRefundConfirmMsgBox = new CDnCashShopRefundMsgBox(UI_TYPE_CHILD_MODAL, this);
	m_pRefundConfirmMsgBox->Initialize(false);

	m_pButtonPagePrev = GetControl<CEtUIButton>("ID_BT_UP");
	m_pButtonPageNext = GetControl<CEtUIButton>("ID_BT_DOWN");
	m_pButtonRefund = GetControl<CEtUIButton>("ID_BUTTON0");
	m_pSlotBG = GetControl<CEtUIStatic>("ID_STATIC0");

	m_pButtonPagePrev->Enable(false);
	m_pButtonPageNext->Enable(false);

	CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	pInvenSlotDlg->Initialize( true, 0, _MAX_REFUND_INVEN_SLOT_PER_LINE, "CS_CheckoutInvenSlot_refund.ui" );
#else
	pInvenSlotDlg->Initialize( true, 0, _MAX_REFUND_INVEN_SLOT_PER_LINE, "CSCheckoutInvenSlot_refund.ui" );
#endif // PRE_ADD_CASHSHOP_RENEWAL
	pInvenSlotDlg->SetSlotType(ST_INVENTORY_CASHSHOP_REFUND);
	m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	if( m_pButtonRefund )
		m_pButtonRefund->Show( false );
#endif // PRE_ADD_CASHSHOP_ACTOZ

	GetItemTask().GetRefundCashInventory().SetInventoryDialog(this);
}

void CDnCashShopRefundInvenDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshPageControl();
		RefreshCashInvenSlot();
	}

	CEtUIDialog::Show( bShow );
}

void CDnCashShopRefundInvenDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_UP" ) )
		{
			PrevPage();
			if (drag::IsValid())
			{
				CEtUIControl* pCtrl = drag::GetControl();
				if (pCtrl && pCtrl->GetType() == UI_CONTROL_BUTTON)
				{
					CDnSlotButton *pDragButton = static_cast<CDnSlotButton *>(pCtrl);
					pDragButton->DisableSplitMode(true);
					pDragButton->SetRegist(false);
				}
				drag::ReleaseControl();
			}
			return;
		}
		else if( IsCmdControl( "ID_BT_DOWN" ) )
		{
			NextPage();
			if (drag::IsValid())
			{
				CEtUIControl* pCtrl = drag::GetControl();
				if (pCtrl && pCtrl->GetType() == UI_CONTROL_BUTTON)
				{
					CDnSlotButton *pDragButton = static_cast<CDnSlotButton *>(pCtrl);
					pDragButton->DisableSplitMode(true);
					pDragButton->SetRegist(false);
				}
				drag::ReleaseControl();
			}
			return;
		}
		else if (IsCmdControl("ID_BUTTON0"))
		{
			if (drag::IsValid())
			{
				CEtUIControl* pCtrl = drag::GetControl();
				if (pCtrl->GetType() == UI_CONTROL_BUTTON)
				{
					CDnSlotButton *pDragButton = static_cast<CDnSlotButton *>(pCtrl);
					if (pDragButton->GetSlotType() == ST_INVENTORY_CASHSHOP_REFUND)
					{
						const CDnItem* pItem = static_cast<CDnItem*>(pDragButton->GetItem());
						if (pItem && pItem->GetCashItemSN() && pItem->GetSerialID() && m_pRefundConfirmMsgBox)
						{
							const SCashShopItemInfo* pCsItemInfo = CDnCashShopTask::GetInstance().GetItemInfo(pItem->GetCashItemSN());
							if (pCsItemInfo)
							{
								const SRefundCashItemInfo* pInfo = CDnCashShopTask::GetInstance().GetRefundItemInfo(pItem->GetSerialID());
								if (pInfo)
								{
									__time64_t current;
									__time64_t dt = pInfo->paidDate + (7 * 86400);
									_time64(&current);
									if (current > dt)
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4811 ), MB_OK ); // UISTRING : 구입 후 7일이 경과한 아이템은 환불할 수 없습니다.
										pDragButton->DisableSplitMode(true);
										pDragButton->SetRegist(false);
										drag::ReleaseControl();
										return;
									}

									if (CDnCashShopTask::GetInstance().IsRefundable(pInfo->sn) == false)
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4829 ), MB_OK ); // UISTRING : 해당 상품은 청약철회가 불가능한 상품입니다.
										pDragButton->DisableSplitMode(true);
										pDragButton->SetRegist(false);
										drag::ReleaseControl();
										return;
									}

									m_pRefundConfirmMsgBox->SetMsgBoxText(pDragButton, pCsItemInfo->nameString, pCsItemInfo->price);
									pDragButton->SetRegist(true);
									drag::ReleaseControl();
									m_pRefundConfirmMsgBox->Show(true);
								}
								return;
							}
						}
					}
				}
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCashShopRefundInvenDlg::CheckSlotIndex( int nSlotIndex, bool bInsert )
{
	if( bInsert )
	{
		// 현재 슬롯을 표현하기 위해 필요한 페이지 개수
		int nNumPage = (nSlotIndex / _MAX_REFUND_INVEN_SLOT_PER_LINE) + 1;
		if( (int)m_pVecInvenSlotDlg.size() < nNumPage )
		{
			bool bOrigSmartMove = CGameOption::GetInstance().m_bSmartMove;
			CGameOption::GetInstance().m_bSmartMove = false;
			int nNewCount = nNumPage - (int)m_pVecInvenSlotDlg.size();
			for( int i = 0; i < nNewCount; ++i )
			{
				CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
#ifdef PRE_ADD_CASHSHOP_RENEWAL
				pInvenSlotDlg->Initialize( false, (int)m_pVecInvenSlotDlg.size(), _MAX_REFUND_INVEN_SLOT_PER_LINE, "CS_CheckoutInvenSlot_refund.ui" );		
#else
				pInvenSlotDlg->Initialize( false, (int)m_pVecInvenSlotDlg.size(), _MAX_REFUND_INVEN_SLOT_PER_LINE, "CSCheckoutInvenSlot_refund.ui" );
#endif // PRE_ADD_CASHSHOP_RENEWAL
				pInvenSlotDlg->SetSlotType( ST_INVENTORY_CASHSHOP_REFUND );

				// 아무래도 거래소처럼 이동된 상태에서 새로 생성해야할 때가 있어서 첫 페이지의 위치와 정렬속성를 따라가는 형태로 하겠다.
				m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );
			}
			CGameOption::GetInstance().m_bSmartMove = bOrigSmartMove;
			RefreshPageControl( true );
		}
	}
	else
	{
		// 현재 인벤토리의 마지막 아이템을 보여주기 위해 필요한 페이지 개수를 구해와 페이지 개수 설정.
		int nLastCashInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetRefundCashInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetRefundCashInventory().GetInventoryItemList().rend() )
			nLastCashInvenSlotIndex = iter->first;
		int nMaxPage = nLastCashInvenSlotIndex / _MAX_REFUND_INVEN_SLOT_PER_LINE;

		// 현재 초기화하는 슬롯과 마지막 슬롯 인덱스가 같다면, 맨 마지막에 있는 아이템을 삭제하는거다.
		// (아이템을 삭제할때 슬롯을 먼저 초기화시키고 인벤토리 리스트에서 빼기때문에 인벤토리 내용엔 아직 삭제될게 남아있어서 이렇게 처리하는 거다.)
		if( nSlotIndex == nLastCashInvenSlotIndex )
		{
			// 이럴땐 뒤쪽에서 두번째 위치하는 아이템의 슬롯인덱스를 구해서,
			int nLastCashInvenSlotIndex2 = 0;
			std::map<int,CDnItem*>::reverse_iterator iter2 = GetItemTask().GetRefundCashInventory().GetInventoryItemList().rbegin();
			if( iter2 != GetItemTask().GetRefundCashInventory().GetInventoryItemList().rend() )
			{
				++iter2;
				if( iter2 != GetItemTask().GetRefundCashInventory().GetInventoryItemList().rend() )
				{
					nLastCashInvenSlotIndex2 = iter2->first;
					int nMaxPage2 = nLastCashInvenSlotIndex2 / _MAX_REFUND_INVEN_SLOT_PER_LINE;
					if( nMaxPage > nMaxPage2 )
					{
						nMaxPage = nMaxPage2;
						m_nMaxPage = nMaxPage;
					}
				}
			}
		}

		// 해당 슬롯을 표현하기 위해 필요한 페이지 개수
		int nNumPage = nMaxPage+1;

		if( (int)m_pVecInvenSlotDlg.size() > nNumPage )
		{
			int nDelCount = (int)m_pVecInvenSlotDlg.size() - nNumPage;
			for( int i = 0; i < nDelCount; ++i )
				SAFE_DELETE( m_pVecInvenSlotDlg[nNumPage+i] );
			for( int i = 0; i < nDelCount; ++i )
				m_pVecInvenSlotDlg.pop_back();

			RefreshPageControl();
			RefreshCashInvenSlot();
		}
	}
}

void CDnCashShopRefundInvenDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnInvenCashDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / _MAX_REFUND_INVEN_SLOT_PER_LINE;

	// 새로운 아이템이 들어올때마다 페이지를 갱신한다.
	CheckSlotIndex( nSlotIndex, true );

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenCashDlg::SetItem");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->SetItem( pItem );
}

void CDnCashShopRefundInvenDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / _MAX_REFUND_INVEN_SLOT_PER_LINE;

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenCashDlg::ResetSlot");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->ResetSlot( nSlotIndex );

	// 아이템이 삭제될때 페이지를 갱신한다.
	CheckSlotIndex( nSlotIndex, false );
}

void CDnCashShopRefundInvenDlg::ResetSlot( MIInventoryItem *pItem )
{
	if (pItem == NULL)
		return;

	ResetSlot(pItem->GetSlotIndex());
}

void CDnCashShopRefundInvenDlg::SetUseItemCnt( DWORD dwItemCnt )
{

}

void CDnCashShopRefundInvenDlg::RefreshPageControl( bool bCheckMaxPage )
{
	if( bCheckMaxPage )
	{
		// 맥스 페이지 설정하고(페이지가 한개면 0, 두개면 1인 최고 인덱스 저장이다.)
		int nLastCashInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetRefundCashInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetRefundCashInventory().GetInventoryItemList().rend() )
			nLastCashInvenSlotIndex = iter->first;
		m_nMaxPage = nLastCashInvenSlotIndex / _MAX_REFUND_INVEN_SLOT_PER_LINE;
	}

	// 현재 페이지가 맥스 페이지를 넘었다면 유효한 범위 안으로 갱신
	if( m_nCurPage > m_nMaxPage )
		m_nCurPage = m_nMaxPage;
	if( m_nCurPage < 0 ) m_nCurPage = 0;

	m_pButtonPageNext->Enable( !(m_nCurPage == m_nMaxPage) );
	m_pButtonPagePrev->Enable( !(m_nCurPage==0) );
}

void CDnCashShopRefundInvenDlg::RefreshCashInvenSlot()
{
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->Show( i == m_nCurPage );
}

void CDnCashShopRefundInvenDlg::NextPage()
{
	if( m_nCurPage < m_nMaxPage ) {
		m_nCurPage++;
		if( m_nCurPage > m_nMaxPage ) m_nCurPage = m_nMaxPage;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}
}

void CDnCashShopRefundInvenDlg::PrevPage()
{
	if( m_nCurPage > 0 ) {
		m_nCurPage--;
		if( m_nCurPage < 0 ) m_nCurPage = 0;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}
}

void CDnCashShopRefundInvenDlg::ReleaseNewGain()
{
	if( !CDnItemTask::IsActive() )
		return;

	if( GetItemTask().GetRefundCashInventory().GetInventoryItemList().empty() )
		return;

	CDnItem *pItem(NULL);
	CDnCashInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetRefundCashInventory().GetInventoryItemList().begin();
	for( ; iter != GetItemTask().GetRefundCashInventory().GetInventoryItemList().end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		pItem->SetNewGain( false );
	}
}

int CDnCashShopRefundInvenDlg::GetRegisteredItemCount()
{
	if( !CDnItemTask::IsActive() )
		return 0;

	if( GetItemTask().GetRefundCashInventory().GetInventoryItemList().empty() )
		return 0;

	int nCount = 0;
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		nCount += m_pVecInvenSlotDlg[i]->GetRegisteredSlotCount();

	return nCount;
}

void CDnCashShopRefundInvenDlg::Withdraw(CDnSlotButton* pPressedBtn)
{
	if( pPressedBtn->GetSlotType() != ST_INVENTORY_CASHSHOP_REFUND )
	{
		//GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4137 ), MB_OK );
		return;
	}

	const CDnItem* pItem = static_cast<CDnItem*>(pPressedBtn->GetItem());
	if (pItem && pItem->GetSerialID())
	{
#ifdef PRE_ADD_CASHSHOP_ACTOZ
		GetInterface().ShowConfirmRefund( const_cast< CDnItem * >(pItem) ); // 아래의 처리가 DnCashShopConfirmRefundDlg 로 옮겨진다.
#else
		bool bPackage = CDnCashShopTask::GetInstance().IsPackageItem(pItem->GetCashItemSN());
		CDnCashShopTask::GetInstance().RequestCashShopWithdrawFromRefundInven(pItem->GetSerialID(), bPackage);
#endif // PRE_ADD_CASHSHOP_ACTOZ

	}
	else
	{
		_ASSERT(0);
	}
}

void CDnCashShopRefundInvenDlg::Refund(CDnSlotButton* pPressedBtn)
{
	if( pPressedBtn->GetSlotType() != ST_INVENTORY_CASHSHOP_REFUND )
	{
		//GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4137 ), MB_OK );
		return ;
	}

	const CDnItem* pItem = static_cast<CDnItem*>(pPressedBtn->GetItem());
	if (pItem && pItem->GetSerialID())
	{
		bool bPackage = CDnCashShopTask::GetInstance().IsPackageItem(pItem->GetCashItemSN());
		CDnCashShopTask::GetInstance().RequestCashShopRefund(pItem->GetSerialID(), bPackage);
	}
	else
	{
		_ASSERT(0);
	}
}

bool CDnCashShopRefundInvenDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_LBUTTONDOWN )
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

#ifdef PRE_ADD_CASHSHOP_ACTOZ
		if (m_pSlotBG->IsInside(fMouseX, fMouseY) == false )
#else
		if (m_pSlotBG->IsInside(fMouseX, fMouseY) == false && m_pButtonRefund->IsInside(fMouseX, fMouseY) == false)
#endif // PRE_ADD_CASHSHOP_ACTOZ
		{
			if (drag::IsValid())
			{
				CDnSlotButton* pDragBtn = static_cast<CDnSlotButton*>(drag::GetControl());
				pDragBtn->DisableSplitMode(true);
				drag::ReleaseControl();
			}
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

#endif // PRE_ADD_CASHSHOP_REFUND_CL