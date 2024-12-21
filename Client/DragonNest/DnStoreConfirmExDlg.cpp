#include "StdAfx.h"
#include "DnStoreConfirmExDlg.h"
#include "DnItem.h"
#include "DnInterfaceString.h"
#include "DnItemTask.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "DnRepStoreBenefitTooltipDlg.h"
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnTradeTask.h"
#include "DnTradeItem.h"

#include "DnCommonTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStoreConfirmExDlg::CDnStoreConfirmExDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pItemSlotButton(NULL)
	, m_pItemCount(NULL)
	, m_pItemAmount1(NULL)
	, m_pItemAmount2(NULL)
	, m_pItemText(NULL)
	, m_pEditBoxItemCount(NULL)
	, m_emConfirmType(CDnInterface::STORE_CONFIRM_SELL)
	, m_pItemName(NULL)
	, m_pFromSlotBtnCache(NULL)
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	, m_pReputationBenefit( NULL )
	, m_pStoreBenefitTooltip( NULL )
	, m_iNowStoreBenefit( NpcReputation::StoreBenefit::Max )
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#ifdef PRE_MOD_SELL_SEALEDITEM
	, m_pStaticSellSealedItem(NULL)
#endif
	, m_pButtonMin( NULL )
	, m_pButtonMax( NULL )
	, m_pOKButton(NULL)
	, m_pCancelButton(NULL)
	, m_pItemDown(NULL)
	, m_pItemUp(NULL)
{
}

CDnStoreConfirmExDlg::~CDnStoreConfirmExDlg(void)
{
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	SAFE_DELETE( m_pStoreBenefitTooltip );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
}

void CDnStoreConfirmExDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StoreConfirmExDlg.ui" ).c_str(), bShow );
}

void CDnStoreConfirmExDlg::InitialUpdate()
{
	m_pItemName = GetControl<CEtUIStatic>("ID_ITEM_NAME"); 
	m_pItemCount = GetControl<CEtUIStatic>("ID_ITEM_COUNT");
	m_pItemAmount1 = GetControl<CEtUIStatic>("ID_ITEM_AMOUNT");
	m_pItemAmount2 = GetControl<CEtUIStatic>("ID_ITEM_AMOUNT2");
	m_pItemText = GetControl<CEtUIStatic>("ID_STATIC_TEXT");
	m_pEditBoxItemCount = GetControl<CEtUIEditBox>("ID_EDITBOX_COUNT");
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	m_pReputationBenefit = GetControl<CEtUIStatic>("ID_ICON_REPUTE");
	m_pReputationBenefit->Show( false );

	m_pStoreBenefitTooltip = new CDnRepStoreBenefitTooltipDlg( UI_TYPE_CHILD, this );
	m_pStoreBenefitTooltip->Initialize( false );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#ifdef PRE_MOD_SELL_SEALEDITEM
	m_pStaticSellSealedItem = GetControl<CEtUIStatic>("ID_STATIC_TEXT1");
#endif

	m_pItemUp = GetControl<CEtUIButton>("ID_ITEM_UP");
	m_pItemDown = GetControl<CEtUIButton>("ID_ITEM_DOWN");

	m_pButtonMin = GetControl<CEtUIButton>("ID_BT_MINI");
	m_pButtonMin = GetControl<CEtUIButton>("ID_BT_MAX");

	m_pOKButton = GetControl<CEtUIButton>("ID_OK");
	m_pCancelButton = GetControl<CEtUIButton>("ID_CANCEL");
	m_SmartMove.SetControl(m_pOKButton);
}

void CDnStoreConfirmExDlg::InitCustomControl( CEtUIControl *pControl )
{
	m_pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
}

void CDnStoreConfirmExDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_SmartMove.MoveCursor();

		RequestFocus( m_pEditBoxItemCount );
		if (m_emConfirmType == CDnInterface::NPC_REPUTATION_GIFT)
		{
			m_pEditBoxItemCount->SetIntToText( 99999 );
		}
		UpdateAmount();
#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
		m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
#endif
	}
	else
	{
		Reset();
	}
}

bool CDnStoreConfirmExDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	if( uMsg == WM_MOUSEWHEEL )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if( IsMouseInDlg() )
		{
			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
			if( nScrollAmount > 0 )
			{
				ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pItemUp, 0 );
				return true;
			}
			else if( nScrollAmount < 0 )
			{
				ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pItemDown, 0 );
				return true;
			}
		}
	}

	// 에디트박스(수량입력)에 포커스가 가있더라도 위아래키가 적용되게 하려면, UI툴의 핫키로는 안된다.
	// 그래서 직접 MsgProc에서 처리하겠다.
	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_UP )
		{
			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pItemUp, 0 );
			return true;
		}
		else if ( wParam == VK_DOWN )
		{
			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pItemDown, 0 );
			return true;
		}
	}

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	if( WM_MOUSEMOVE == uMsg )
	{
		CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
		if( !pQuestTask ) 
			return false;

		CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
		if( !pReputationRepos )
			return false;

		float fMouseX = 0;
		float fMouseY = 0;
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

		bool bMouseEnter = false;
		float fReputationValue = 0.0f;
		float fReputationValueMax = 100.0f;

		ShowChildDialog( m_pStoreBenefitTooltip, false );
		if( m_pReputationBenefit->IsShow() )
		{
			if( m_pReputationBenefit->IsInside( fMouseX, fMouseY ) )
				bMouseEnter = true;

			if( !CDnMouseCursor::GetInstance().IsShowCursor() )
				bMouseEnter = false;				

			if( bMouseEnter )
			{
				if( CDnActor::s_hLocalActor )
				{
					int iBenefitValue = pQuestTask->GetStoreBenefitValue( m_iNowStoreBenefit );

					wchar_t awcBuffer[ 256 ] = { 0 };
					if( NpcReputation::StoreBenefit::BuyingPriceDiscount == m_iNowStoreBenefit )
						//swprintf_s( awcBuffer, L"호감도 효과 발동중\n아이템 %d%% 할인 (UIString 에 없는 임시 텍스트)", iBenefitValue );
						swprintf_s( awcBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3218 ), iBenefitValue );
					else
					if( NpcReputation::StoreBenefit::SellingPriceUp == m_iNowStoreBenefit )
						//swprintf_s( awcBuffer, L"호감도 효과 발동중\n아이템 판매가격 %d%% 상승 (UIString 에 없는 임시 텍스트)", iBenefitValue );
						swprintf_s( awcBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3217 ), iBenefitValue );

					m_pStoreBenefitTooltip->SetText( awcBuffer );
					ShowChildDialog( m_pStoreBenefitTooltip, true );

					SUICoord dlgCoord;
					m_pStoreBenefitTooltip->GetDlgCoord( dlgCoord );
					dlgCoord.fX = fMouseX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
					dlgCoord.fY = fMouseY + GetYCoord() - dlgCoord.fHeight - 0.004f;
					m_pStoreBenefitTooltip->SetDlgCoord( dlgCoord );
				}
			}
		}
	}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnStoreConfirmExDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_EDITBOX_CHANGE )
	{
		if( IsCmdControl("ID_EDITBOX_COUNT" ) )
		{
			UpdateAmount();
#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
#endif
			return;
		}
	}

	if ( nCommand == EVENT_BUTTON_CLICKED )
	{
		if ( IsCmdControl("ID_ITEM_UP" ) )
		{
			m_pEditBoxItemCount->SetIntToText( m_pEditBoxItemCount->GetTextToInt() + 1 );
			UpdateAmount();
#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
#endif
			return;
		}

		if ( IsCmdControl("ID_ITEM_DOWN" ) )
		{
			int nCount = m_pEditBoxItemCount->GetTextToInt() - 1;
			nCount = max(nCount, 1);
			m_pEditBoxItemCount->SetIntToText( nCount );
			UpdateAmount();
#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
#endif
			return;
		}

		if ( IsCmdControl("ID_BT_MINI" ) )
		{
			m_pEditBoxItemCount->SetIntToText( 1 );
			UpdateAmount();
#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
#endif
			return;
		}

		if ( IsCmdControl("ID_BT_MAX" ) )
		{
			m_pEditBoxItemCount->SetIntToText( 99999 );
			UpdateAmount();
#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
			m_pOKButton->Enable(m_pEditBoxItemCount->GetTextToInt() > 0);
#endif
			return;
		}
	}

	// 0개 넣고 엔터치면 포커스가 에디트박스에 있는 상태로 0개 구입 못한다는 메세지박스가 뜨는데,
	// 이때 포커스가 에디트박스에 남아있기때문에, 메세지박스 다이얼로그의 핫키들이 안먹게 된다.
	// 그래서 우선 이렇게 해서 포커스를 잃도록 하겠다.
	if( nCommand == EVENT_EDITBOX_STRING )
	{
		RequestFocus( GetControl<CEtUIButton>("ID_OK") );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnStoreConfirmExDlg::SetItem(CDnItem* pItem, CDnInterface::emSTORE_CONFIRM_TYPE type, int slotCount)
{
	if( !pItem )
		return;

	m_emConfirmType = type;
	// 상점에서 구입을 제외한 나머지 내 인벤에서 빠져나가는건 다 1개로 표시해서 여러개가 묶음으로 되지 않았음을 보여준다.
	int nSlotCount = 1;
	if( m_emConfirmType == CDnInterface::STORE_CONFIRM_BUY ) nSlotCount = CDnSlotButton::ITEM_ORIGINAL_COUNT;
	if (m_emConfirmType == CDnInterface::NPC_REPUTATION_GIFT)
		nSlotCount = slotCount;
	m_pItemSlotButton->SetItem( pItem, nSlotCount );
	m_pItemName->SetTextColor( DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
	m_pItemName->SetText( pItem->GetName() );

	wchar_t wszTemp[32] = {0};

	switch( m_emConfirmType )
	{
	case CDnInterface::STORE_CONFIRM_SELL:
		{
			// 현재 UIString 임의순번 등록이 안되서 우선 이렇게 처리하겠다.
			swprintf_s( wszTemp, _countof(wszTemp), L"(%s : %d)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1138 ), pItem->GetOverlapCount() );
			m_pItemCount->SetText( wszTemp );

			m_pItemText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1716 ) );
			m_pEditBoxItemCount->SetIntToText( pItem->GetOverlapCount() );
#ifdef PRE_MOD_SELL_SEALEDITEM
			m_pStaticSellSealedItem->Show( (pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound()) );
#endif
		}
		break;

	case CDnInterface::STORE_CONFIRM_BUY:
		{
			swprintf_s( wszTemp, _countof(wszTemp), L"(%s : %d)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1138 ), pItem->GetMaxOverlapCount() / pItem->GetOverlapCount() );
			m_pItemCount->SetText( wszTemp );

			m_pItemText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1715 ) );

			m_pEditBoxItemCount->SetIntToText( 1 );
		}
		break;

	case CDnInterface::MAIL_WRITE:
	case CDnInterface::PRIVATE_TRADE:
	case CDnInterface::STORAGE_FROM_INVENTORY:
	case CDnInterface::INVENTORY_FROM_STORAGE:
	case CDnInterface::MARKET_REGISTER:
		{
			swprintf_s( wszTemp, _countof(wszTemp), L"(%s : %d)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1138 ), pItem->GetOverlapCount() ); // UISTRING : 최대
			m_pItemCount->SetText( wszTemp );

			m_pItemText->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1987 ));	// UISTRING : 몇 개를 첨부하시겠습니까?
			m_pEditBoxItemCount->SetIntToText( 1 );
		}
		break;

	case CDnInterface::NPC_REPUTATION_GIFT:
		{
			int maxCount = (slotCount > NPCPRESENTMAX) ? NPCPRESENTMAX : slotCount;
			swprintf_s( wszTemp, _countof(wszTemp), L"(%s : %d)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1138 ), maxCount); // UISTRING : 최대
			m_pItemCount->SetText( wszTemp );

	#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
			m_pItemText->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3326 ));	// UISTRING : 몇 개를 선물하시겠습니까?
	#endif
			m_pEditBoxItemCount->SetIntToText( 1 );
		}
		break;

	default:
		ASSERT(0&&"CDnStoreConfirmExDlg::SetItem");
		break;
	}
}

void CDnStoreConfirmExDlg::SetItem(CDnSlotButton* pFromSlot, CDnInterface::emSTORE_CONFIRM_TYPE type)
{
	if (pFromSlot == NULL)
		return;

	MIInventoryItem* pInvenItem = pFromSlot->GetItem();
	if (pInvenItem == NULL || pInvenItem->GetType() != MIInventoryItem::Item)
	{
		_ASSERT(0);
		return;
	}

	CDnItem* pItem = static_cast<CDnItem*>(pInvenItem);
	int slotCount = pFromSlot->GetRenderCount();
	SetItem(pItem, type, slotCount);

	m_pFromSlotBtnCache = pFromSlot;
}

void CDnStoreConfirmExDlg::EnalbleButtons(bool bEnable)
{
	if (m_pOKButton == NULL || m_pCancelButton == NULL || m_pButtonMin == NULL || m_pButtonMax == NULL)
		return;
	m_pOKButton->Enable(bEnable);
	m_pCancelButton->Enable(bEnable);
	m_pButtonMin->Enable(bEnable);
	m_pButtonMax->Enable(bEnable);
}

void CDnStoreConfirmExDlg::UpdateAmount()
{
	CDnItem *pItem = (CDnItem*)m_pItemSlotButton->GetItem();
	if( !pItem ) return;

	int nCount = m_pEditBoxItemCount->GetTextToInt();
	int nOverCount(0);
	int nAmount(0);

#ifdef PRE_FIX_REPUTE_DISCOUNT
	bool bAvailBenefit = false;
#endif
	bool bPresentCost = true;
	switch( m_emConfirmType )
	{
	case CDnInterface::STORE_CONFIRM_SELL:
		{
			nOverCount = pItem->GetOverlapCount();
			nAmount = pItem->GetItemSellAmount();

			// 호감도 상점 혜택이 있는가.
#ifdef PRE_FIX_REPUTE_DISCOUNT
			bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::SellingPriceUp, nAmount );
			if( bAvailBenefit )
			{
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_iNowStoreBenefit = NpcReputation::StoreBenefit::SellingPriceUp;
			}
			m_pReputationBenefit->Show( bAvailBenefit );
#else
	#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
			bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::SellingPriceUp, nAmount );
			if( bAvailBenefit )
			{
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_iNowStoreBenefit = NpcReputation::StoreBenefit::SellingPriceUp;
			}
			m_pReputationBenefit->Show( bAvailBenefit );
	#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#endif
		}
		break;

	case CDnInterface::STORE_CONFIRM_BUY:
		{
			nOverCount = pItem->GetMaxOverlapCount() / pItem->GetOverlapCount();
			nAmount = pItem->GetItemAmount() * pItem->GetOverlapCount();

			// 호감도 상점 혜택이 있는가.
#ifdef PRE_FIX_REPUTE_DISCOUNT
			bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::BuyingPriceDiscount, nAmount );
			if( bAvailBenefit )
			{
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_iNowStoreBenefit = NpcReputation::StoreBenefit::BuyingPriceDiscount;
			}
#else
	#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
			bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::BuyingPriceDiscount, nAmount );
			if( bAvailBenefit )
			{
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_iNowStoreBenefit = NpcReputation::StoreBenefit::BuyingPriceDiscount;
			}
			m_pReputationBenefit->Show( bAvailBenefit );
	#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#endif
		}
		break;

	case CDnInterface::MAIL_WRITE:
	case CDnInterface::PRIVATE_TRADE:
	case CDnInterface::STORAGE_FROM_INVENTORY:
	case CDnInterface::INVENTORY_FROM_STORAGE:
	case CDnInterface::MARKET_REGISTER:
		{
			nOverCount = pItem->GetOverlapCount();
			nAmount = pItem->GetItemAmount();
			bPresentCost = false;

			if (nCount > nOverCount)
			{
				m_pEditBoxItemCount->SetIntToText(nOverCount);
			}
			else if (nCount >= 0 && nCount < 1)
			{
				m_pEditBoxItemCount->SetIntToText(1);
			}
		}
		break;

	case CDnInterface::NPC_REPUTATION_GIFT:
		{
			nOverCount = m_pFromSlotBtnCache->GetRenderCount();
			nAmount = pItem->GetItemAmount();
			bPresentCost = false;

			if (nOverCount > NPCPRESENTMAX)
				nOverCount = NPCPRESENTMAX;

			if (nCount > nOverCount)
			{
				m_pEditBoxItemCount->SetIntToText(nOverCount);
			}
			else if (nCount >= 0 && nCount < 1)
			{
				m_pEditBoxItemCount->SetIntToText(1);
			}

			m_pItemAmount1->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3324)); // UISTRING : [호감도 상승]

			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
			if (pCommonTask == NULL)
				return;

			int currentTalkNpcID = pCommonTask->GetCurrentTalkNpcID();
			if (currentTalkNpcID != 0)
			{
				DNTableFileFormat*  pReputationTable = GetDNTable( CDnTableDB::TREPUTE );
				DNTableFileFormat*  pUnionTable = GetDNTable( CDnTableDB::TUNION );
				if (pReputationTable == NULL || pUnionTable == NULL)
					return;

				int currentTableID = pReputationTable->GetItemIDFromField("_NpcID", currentTalkNpcID);
				int currentUnionTableID = pReputationTable->GetFieldFromLablePtr(currentTableID, "_UnionID")->GetInteger();
				if (currentUnionTableID == NpcReputation::UnionType::Etc)
				{
	#ifdef PRE_FIX_REPUTE_PRESENTCOUNT
					m_pItemAmount2->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3328)); // UISTRING : [소속연합 없음]
	#endif
				}
				else
				{
					currentTableID = pUnionTable->GetItemIDFromField("_UnionID", currentUnionTableID);

					int unionNameStringNum = pUnionTable->GetFieldFromLablePtr(currentTableID, "_UnionName")->GetInteger();
					const std::wstring unionName(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, unionNameStringNum));

					std::wstring unionCheckString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3325), unionName.c_str()); // UISTRING : [%s 연합포인트 획득]
					m_pItemAmount2->SetText(unionCheckString.c_str());
				}
			}
		}
		return;
	default:
		ASSERT(0&&"CDnStoreConfirmExDlg::UpdateAmount");
		break;
	}

	if( nCount > nOverCount )
	{
		nCount = nOverCount;
		m_pEditBoxItemCount->SetIntToText( nCount );
	}

	//std::wstring strCoin;
	//DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nAmount, strCoin );
	//swprintf_s( wszTemp, _countof(wszTemp), L"[%s]x[%d]=[%s]", strCoin.c_str(), nCount, strTotalAmount.c_str() );

	if (bPresentCost)
	{
		if( nCount == 0 || nCount == -1 )
		{
			m_pItemAmount1->ClearText();
			m_pItemAmount2->ClearText();
			return;
		}

		if (m_emConfirmType == CDnInterface::STORE_CONFIRM_SELL)
		{
			std::wstring strTotalAmount;
			DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nAmount*nCount, strTotalAmount );
			wchar_t wszTemp[128] = {0};
			swprintf_s( wszTemp, _countof(wszTemp), L"[%s]", strTotalAmount.c_str() );
			m_pItemAmount1->SetText( wszTemp );
		}
		else if (m_emConfirmType == CDnInterface::STORE_CONFIRM_BUY)
		{
			CDnStoreTabDlg *pStoreDlg = (CDnStoreTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::STORE_DIALOG );
			if (pStoreDlg && pStoreDlg->IsShow())
			{
				if (pStoreDlg->IsShowRepurchasableTab())
				{
					SetRepurchaseBuy(*pItem);
					return;
				}

				if (Shop::Type::Combined <= pStoreDlg->GetStoreType())
				{
					SetCombinedBuy( pItem, pStoreDlg->GetCurrentTabID(), nCount );
					return;
				}
			}
			// 먼저 메달이 있는지 검사
			if( pItem->GetNeedBuyItem() && pItem->GetNeedBuyItemCount() )
			{
				wchar_t wszTemp[128] = {0};
				std::wstring wszNeedItemName = CDnItem::GetItemFullName( pItem->GetNeedBuyItem() );
				swprintf_s( wszTemp, _countof(wszTemp), L"[%s %d%s]", wszNeedItemName.c_str(), pItem->GetNeedBuyItemCount()*nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1728 ) );
				m_pItemAmount1->SetText( wszTemp );

				if( nAmount ) {
					std::wstring strTotalAmount;
					DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nAmount*nCount, strTotalAmount );
					swprintf_s( wszTemp, _countof(wszTemp), L"[%s]", strTotalAmount.c_str() );
					m_pItemAmount2->SetText( wszTemp );
				}
			}
			else if(pItem->GetNeedBuyLadderPoint())
			{
#ifdef PRE_FIX_LADDERSHOP_SKILL
				int nPoint = GetTradeTask().GetTradeItem().CalcPurchaseLadderPoint(pItem->GetNeedBuyLadderPoint());

				wchar_t wszTemp[128] = {0};
				std::wstring wszNeedItemName = CDnItem::GetItemFullName(nPoint);
				swprintf_s( wszTemp, _countof(wszTemp), L"[%d %s]", nPoint*nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126176 ) );
#else
				wchar_t wszTemp[128] = {0};
				std::wstring wszNeedItemName = CDnItem::GetItemFullName( pItem->GetNeedBuyLadderPoint() );
				swprintf_s( wszTemp, _countof(wszTemp), L"[%d %s]", pItem->GetNeedBuyLadderPoint()*nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126176 ) );
#endif
				m_pItemAmount1->SetText( wszTemp );
			}
			else if(pItem->GetNeedBuyGuildWarPoint())
			{
				wchar_t wszTemp[128] = {0};
				std::wstring wszNeedItemName = CDnItem::GetItemFullName( pItem->GetNeedBuyGuildWarPoint() );
				swprintf_s( wszTemp, _countof(wszTemp), L"[%d %s]", pItem->GetNeedBuyGuildWarPoint()*nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126084 ) );
				m_pItemAmount1->SetText( wszTemp );
			}
			else if (pItem->GetNeedBuyUnionPoint())
			{
				std::wstring temp;
				std::wstring wszNeedItemName = CDnItem::GetItemFullName(pItem->GetNeedBuyGuildWarPoint());
				temp = FormatW(L"[%d %s]", pItem->GetNeedBuyUnionPoint() * nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3273 ));
				m_pItemAmount1->SetText(temp.c_str());
			}
			else
			{
				std::wstring strTotalAmount;
				DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nAmount*nCount, strTotalAmount );
				wchar_t wszTemp[128] = {0};
				swprintf_s( wszTemp, _countof(wszTemp), L"[%s]", strTotalAmount.c_str() );
				m_pItemAmount1->SetText( wszTemp );
#ifdef PRE_FIX_REPUTE_DISCOUNT
				m_pReputationBenefit->Show( bAvailBenefit );
#endif
			}
		}
	}
}

int CDnStoreConfirmExDlg::GetItemCount()
{
	return m_pEditBoxItemCount->GetTextToInt();
}

void CDnStoreConfirmExDlg::Reset()
{
	m_SmartMove.ReturnCursor();

	m_pItemSlotButton->ResetSlot();
	m_pItemCount->ClearText();
	m_pItemAmount1->ClearText();
	m_pItemAmount2->ClearText();
	m_pItemText->ClearText();
	m_pEditBoxItemCount->ClearText();
	m_pFromSlotBtnCache = NULL;

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	m_pReputationBenefit->Show( false );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#ifdef PRE_MOD_SELL_SEALEDITEM
	m_pStaticSellSealedItem->Show( false );
#endif
}

void CDnStoreConfirmExDlg::SetCombinedBuy( CDnItem * pItem, int nTabID, int nCount )
{
	CDnTradeItem::TCombinedShopTableData * pData = GetTradeTask().GetTradeItem().GetCombinedStoreItem( nTabID, pItem->GetSlotIndex() );

	WCHAR wszString[512], wszOrigin[512];
	int nLine = 0;
	for( int itr = 0; itr < Shop::Max::PurchaseType; ++itr )
	{
		wszOrigin[0] = NULL;
		if( Shop::PurchaseType::Gold == pData->PurchaseType[itr].PurchaseType )
		{
			int nMoney = pData->PurchaseType[itr].iPurchaseItemValue;
			int storeBenefitAmount = nMoney;
			std::wstring wszMoney;

			bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::BuyingPriceDiscount, storeBenefitAmount );
#ifdef PRE_FIX_REPUTE_DISCOUNT
			if( bAvailBenefit )
			{
				// 아이템 구매창에 호감도로 값이 할인된다는 표시 보여줌.
				m_iNowStoreBenefit = NpcReputation::StoreBenefit::BuyingPriceDiscount;
			}
			m_pReputationBenefit->Show( bAvailBenefit );
#endif

			DN_INTERFACE::UTIL::GetMoneyFormatUseStr( (bAvailBenefit ? storeBenefitAmount : nMoney) * nCount, wszMoney );
			swprintf_s( wszOrigin, _countof(wszOrigin), L"%s", wszMoney.c_str() );
		}
		else if( Shop::PurchaseType::ItemID == pData->PurchaseType[itr].PurchaseType )
		{
			std::wstring wszNeedItemName = CDnItem::GetItemFullName( pData->PurchaseType[itr].iPurchaseItemID );
			swprintf_s( wszOrigin, _countof(wszOrigin), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 80 ), wszNeedItemName.c_str(), pData->PurchaseType[itr].iPurchaseItemValue * nCount );	// UISTRING : %s %d개
		}
#ifdef PRE_ADD_NEW_MONEY_SEED
		else if( Shop::PurchaseType::Seed == pData->PurchaseType[itr].PurchaseType )
		{
			swprintf_s( wszOrigin, 512, L"%d%s", pData->PurchaseType[itr].iPurchaseItemValue * nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4967 ) );
		}
#endif // PRE_ADD_NEW_MONEY_SEED 
		else if( Shop::PurchaseType::LadderPoint == pData->PurchaseType[itr].PurchaseType )
		{
			int nPoint = GetTradeTask().GetTradeItem().CalcPurchaseLadderPoint(pData->PurchaseType[itr].iPurchaseItemValue);

			wchar_t wszTemp[128] = {0};
			std::wstring wszNeedItemName = CDnItem::GetItemFullName(nPoint);
			swprintf_s(wszOrigin, _countof(wszOrigin), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1769 ), nPoint*nCount);	// UISTRING : %d 래더 포인트
		}
		else if( Shop::PurchaseType::GuildPoint == pData->PurchaseType[itr].PurchaseType )
		{
			swprintf_s( wszOrigin, _countof(wszOrigin), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1771 ), pData->PurchaseType[itr].iPurchaseItemValue * nCount );	// UISTRING : %d 길드축제 포인트
		}
		else if( Shop::PurchaseType::UnionPoint== pData->PurchaseType[itr].PurchaseType )
		{
			swprintf_s( wszOrigin, _countof(wszOrigin), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1770 ), pData->PurchaseType[itr].iPurchaseItemValue * nCount );	// UISTRING : %d 연합 포인트
		}

		if( NULL != wszOrigin[0] )
		{
			swprintf_s( wszString, _countof(wszString), L"[%s]", wszOrigin );
			if( 0 == nLine )
				m_pItemAmount1->SetText( wszString );
			else
				m_pItemAmount2->SetText( wszString );

			++nLine;
		}
	}
}

void CDnStoreConfirmExDlg::SetRepurchaseBuy(const CDnItem& item)
{
	const CDnStoreRepurchaseSystem& rs = GetTradeTask().GetTradeItem().GetStoreRepurchaseSystem();
	int nRepurchaseMoney = rs.GetRepurchasePrice(item);
	if (nRepurchaseMoney == CDnStoreRepurchaseSystem::eINVALID_PRICE)
	{
		_ASSERT(0);
		return;
	}

	std::wstring wszMoney;
	DN_INTERFACE::UTIL::GetMoneyFormatUseStr(nRepurchaseMoney, wszMoney);
	m_pItemAmount1->SetText(wszMoney);
}
