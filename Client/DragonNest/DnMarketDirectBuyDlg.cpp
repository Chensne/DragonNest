#include "StdAfx.h"
#include "DnMarketDirectBuyDlg.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnMainFrame.h"
#include "DnTradeTask.h"
#include "DnItemTask.h"
#include "TradeSendPacket.h"
#include "DnMainMenuDlg.h"
#include "DnMarketToolTipDlg.h"
#ifdef PRE_ADD_PETALTRADE
#include "DnCashShopTask.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 



#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM

#define SEARCH_CLICK_DELAY_TIME 2.0f

CDnMarketDirectBuyDlg::CDnMarketDirectBuyDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pItemBuyOKButton(NULL)
, m_pItemBuyCancelButton(NULL)
, m_nMaxPageNumber(0)
, m_nCurrentPageNumber(0)
, m_nSelectedItemIndex(-1)
, m_nBuyItemDBID(-1)
, m_pPrevSelectedItem(NULL)
, m_pItemPriceTooltip(NULL)
, m_nOnePrice(-1)
, m_nMouseOverIndex(-1)
, m_bPetalItem(false)
, m_pButtonReScan( NULL )
, m_pPrevButton( NULL )
, m_pNextButton( NULL )
, m_pStaticPageNumber( NULL )
, m_fElapseTime(.0f)
, m_bIsClickSearchButton(false)
#ifdef PRE_ADD_PETALTRADE
, m_nCurrentMyPetal(0)
, m_pCheckBoxGold( NULL )
, m_pCheckBoxPetal( NULL )
, m_pStaticPetalMessage( NULL )
, m_pStaticMyPetal( NULL )
, m_pButtonSearch( NULL )
#endif
{
	int tempBaseName[] = { 11, 12, 16, 20, 24 };
	for(int i = 0 ; i < SLOT_MAX ; ++i )
	{
		m_pStaticSelect[i] = NULL;
		m_pItemSlotButton[i] = NULL;
		m_pStaticItemName[i] = NULL;
		m_pStaticGold[i] = NULL;
		m_pStaticSilver[i] = NULL;
		m_pStaticBronze[i] = NULL;
		m_pStaticPremiumItem[i] = NULL;
		m_pStaticMoneyBackBase[i] = NULL;
#ifdef PRE_ADD_PETALTRADE
		m_pStaticPetal[i] = NULL;
		m_pStaticPetalBase[i] = NULL;
#endif
		m_nMoneyBackBaseName[i] = tempBaseName[i];
	}	
}

CDnMarketDirectBuyDlg::~CDnMarketDirectBuyDlg(void)
{
	SAFE_DELETE(m_pItemPriceTooltip);
}

void CDnMarketDirectBuyDlg::Initialize( bool bShow )
{
#ifdef PRE_ADD_PETALTRADE
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemUpgradeReserveMarketDlg.ui" ).c_str(), bShow );
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemUpgradeMarketDlg.ui" ).c_str(), bShow );
#endif

	m_pItemPriceTooltip = new CDnMarketToolTipDlg(UI_TYPE_CHILD, this);
	ASSERT(m_pItemPriceTooltip && "CDnMarketDirectBuyDlg::m_pItemPriceTooltip Initialize Error");
	m_pItemPriceTooltip->Initialize(false);
}

void CDnMarketDirectBuyDlg::InitialUpdate()
{
	m_pItemBuyOKButton = GetControl<CEtUIButton>("ID_BT_BUY");
	m_pItemBuyOKButton->Enable(false);
	m_pItemBuyCancelButton = GetControl<CEtUIButton>("ID_BT_CANCEL");
	m_pItemBuyCancelButton = GetControl<CEtUIButton>("ID_BT_CLOSE");
	
	m_pPrevButton = GetControl<CEtUIButton>("ID_BT_PRIOR");
	m_pNextButton = GetControl<CEtUIButton>("ID_BT_NEXT");
	m_pStaticPageNumber = GetControl<CEtUIStatic>("ID_TEXT_PAGE");

	for(int i = 0 ; i < SLOT_MAX ; ++i)
	{
		m_pStaticSelect[i]	 = GetControl<CEtUIStatic>( FormatA("ID_STATIC_SELECT%d",i).c_str() );
		m_pStaticSelect[i]->Show(false);
		m_pStaticSelect[i]->Enable(false);

		m_pItemSlotButton[i] = GetControl<CDnItemSlotButton>( FormatA("ID_ITEM%d",i).c_str() );
		m_pItemSlotButton[i]->SetSlotType( ST_MARKET_REGIST );

		m_pStaticItemName[i] = GetControl<CEtUIStatic>( FormatA("ID_TEXT_NAME%d",i).c_str() );
	
		m_pStaticGold[i]	 = GetControl<CEtUIStatic>( FormatA("ID_GOLD%d",i).c_str() );
		m_pStaticSilver[i]	 = GetControl<CEtUIStatic>( FormatA("ID_SILVER%d",i).c_str() );
		m_pStaticBronze[i]	 = GetControl<CEtUIStatic>( FormatA("ID_BRONZE%d",i).c_str() );

		m_pStaticPremiumItem[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_PMBAR%d",i).c_str() );
		m_pStaticPremiumItem[i]->Show(false);
		m_pStaticPremiumItem[i]->Enable(false);

		m_pStaticToolTipArea[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_TOOLTIP%d",i).c_str() );

		m_pStaticMoneyBackBase[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC%d", m_nMoneyBackBaseName[i]).c_str() );

#ifdef PRE_ADD_PETALTRADE
		m_pStaticPetal[i] = GetControl<CEtUIStatic>( FormatA("ID_PETAL%d",i).c_str() );
		m_pStaticPetalBase[i] = GetControl<CEtUIStatic>( FormatA("ID_PETALBASE%d",i).c_str() );
#endif
	}
	m_pButtonReScan = GetControl<CEtUIButton>("ID_BT_RESCAN");

#ifdef PRE_ADD_PETALTRADE
	m_pCheckBoxGold  = GetControl<CEtUICheckBox>("ID_CHECKBOX_GOLD");
	m_pCheckBoxPetal = GetControl<CEtUICheckBox>("ID_CHECKBOX_RESERVE");
	m_pCheckBoxGold->SetChecked(true, false);
	m_pCheckBoxPetal->SetChecked(true, false);
	
	m_pStaticPetalMessage = GetControl<CEtUIStatic>("ID_TEXT_ASKCOUNT");
	m_pStaticPetalMessage->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4644)); // 현재 소지한 페탈.
		
	m_pStaticMyPetal = GetControl<CEtUIStatic>("ID_TEXT_COUNT");
	
	m_pButtonSearch = GetControl<CEtUIButton>("ID_BT_SEARCH");
	//m_pButtonSearch->Enable(false);
#endif
}

void CDnMarketDirectBuyDlg::PrevPage()
{
	if( m_nMaxPageNumber == 1)
		return;

	m_nCurrentPageNumber--;

	if(m_nCurrentPageNumber < 0)
		m_nCurrentPageNumber = m_nMaxPageNumber-1;
	
	SetCurrentItemPage(m_nCurrentPageNumber);
}

void CDnMarketDirectBuyDlg::NextPage()
{
	if(m_nMaxPageNumber == 1)
		return;

	m_nCurrentPageNumber++;

	if(m_nCurrentPageNumber >= m_nMaxPageNumber)
		m_nCurrentPageNumber = 0;
	
	SetCurrentItemPage(m_nCurrentPageNumber);
}

void CDnMarketDirectBuyDlg::SetCurrentItemPage(int nPageNum)
{
	if( m_nCurrentPageNumber > m_nMaxPageNumber ) return;

	ResetUI();

	WCHAR wszStr[64];
	swprintf_s( wszStr, L"%d / %d", m_nCurrentPageNumber + 1 , m_nMaxPageNumber);
	m_pStaticPageNumber->SetText(wszStr);

	// 아이템 리스트 설정.
	if(m_vecDirectMarketList.empty() || m_vecDirectMarketList.size() > LIST_MAX) return;

	TItemInfo	itemInfo;
	TMarketInfo ItemData;
	int			nPageStart	= (m_nCurrentPageNumber * SLOT_MAX) + SLOT_MAX;
	int			nPageEnd	= (m_nCurrentPageNumber * SLOT_MAX) + SLOT_MAX;
	int			nUiIndex	= 0;

	if(nPageEnd > (int)m_vecDirectMarketList.size()) 
		nPageEnd = (int)m_vecDirectMarketList.size();

	for(int i = nPageStart-SLOT_MAX ; i < nPageEnd ; ++i)
	{
		ItemData = m_vecDirectMarketList[i];

		if(CDnItem::MakeItemInfo(ItemData.nItemID, ItemData.wCount, itemInfo))
		{
			CDnItem* pItem = GetItemTask().CreateItem(itemInfo);
			if(!pItem) continue;

			//WCHAR wszTemp[256] = {0,};
			//swprintf_s( wszTemp, 256, L"현재%d\n", i );
			//OutputDebugStringW( wszTemp );

			m_pItemSlotButton[nUiIndex]->SetItem(pItem, ItemData.wCount);
			m_pItemSlotButton[nUiIndex]->Show(true);

			m_pStaticItemName[nUiIndex]->SetText(pItem->GetName());
			m_pStaticSelect[nUiIndex]->Enable(true);

			if(ItemData.bPremiumTrade) 
			{
				m_pStaticPremiumItem[nUiIndex]->Show(true);
				m_pStaticPremiumItem[nUiIndex]->Enable(true);
			}
		}
		m_pStaticToolTipArea[nUiIndex]->Show(true);

		if(ItemData.cMethodCode == 1) // 게임머니
		{
			int nGold   = ItemData.nPrice / 10000;
			int nSilver = (ItemData.nPrice % 10000) / 100;
			int nBronze = ItemData.nPrice % 100;

			m_pStaticGold[nUiIndex]->SetIntToText(nGold);
			m_pStaticGold[nUiIndex]->Show(true);
			m_pStaticSilver[nUiIndex]->SetIntToText(nSilver);
			m_pStaticSilver[nUiIndex]->Show(true);
			m_pStaticBronze[nUiIndex]->SetIntToText(nBronze);
			m_pStaticBronze[nUiIndex]->Show(true);

			m_pStaticMoneyBackBase[nUiIndex]->Show(true);
		}
		else if(ItemData.cMethodCode == 3) // 페탈
		{
#ifdef PRE_ADD_PETALTRADE
			m_pStaticPetal[nUiIndex]->SetIntToText(ItemData.nPrice);
			m_pStaticPetal[nUiIndex]->Show(true);
			m_pStaticPetalBase[nUiIndex]->Show(true);
#endif
		}

		nUiIndex++;
	}
}

void CDnMarketDirectBuyDlg::ResetUI()
{
	for( int i = 0 ; i < SLOT_MAX; ++i )
	{
		m_pStaticSelect[i]->Show(false);
		m_pStaticSelect[i]->Enable(false);
		m_pItemSlotButton[i]->ResetSlot();
		m_pStaticItemName[i]->ClearText();
		
		m_pStaticGold[i]->ClearText();
		m_pStaticGold[i]->Show(false);
		m_pStaticSilver[i]->ClearText();
		m_pStaticSilver[i]->Show(false);
		m_pStaticBronze[i]->ClearText();
		m_pStaticBronze[i]->Show(false);

		m_pStaticMoneyBackBase[i]->Show(false);

		m_pStaticPremiumItem[i]->Show(false);
		m_pStaticPremiumItem[i]->Enable(false);		
		m_pStaticToolTipArea[i]->Show(false);

#ifdef PRE_ADD_PETALTRADE
		m_pStaticPetal[i]->Show(false);
		m_pStaticPetal[i]->ClearText();
		m_pStaticPetalBase[i]->Show(false);
#endif
	}
	m_pPrevSelectedItem = NULL; // 선택한 아이템 이름 저장용..
	
	m_pPrevButton->Enable(true);
	m_pNextButton->Enable(true);
}

void CDnMarketDirectBuyDlg::ResetData()
{
	m_nSelectedItemIndex = -1;
	m_nBuyItemDBID = -1;
	m_nMaxPageNumber = 0;
	m_nCurrentPageNumber = 0;
	m_vecDirectMarketList.clear();
}

void CDnMarketDirectBuyDlg::Process(float fElapsedTime)
{
	// 구입버튼 enable / disable
	bool bIsEnable = true;
	if( m_nSelectedItemIndex == -1 )
		bIsEnable = false;
	m_pItemBuyOKButton->Enable(bIsEnable);
	
	// 구입 리스트 낱개당 가격 툴팁
	if( IsMouseInDlg() )
	{
		if(m_nMouseOverIndex < SLOT_MAX)
		{
			bool bIsShow = m_pStaticToolTipArea[m_nMouseOverIndex]->IsMouseEnter();

			if( m_nOnePrice == -1 )
				m_pItemPriceTooltip->Show(false);
			else	
				m_pItemPriceTooltip->Show(bIsShow);
		}
	}

#ifdef PRE_ADD_PETALTRADE
	wchar_t buf[64] = { 0, };
	swprintf_s( buf, 64, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4647), m_nCurrentMyPetal); // %d페탈
	if(m_pStaticMyPetal->GetText() != std::wstring(buf))
		m_pStaticMyPetal->SetText(std::wstring(buf));
	
	if(m_bIsClickSearchButton)
	{
		if(m_fElapseTime > .0f)
			m_fElapseTime -= fElapsedTime;
		else
		{
			m_pButtonSearch->Enable(true);
			m_pCheckBoxGold->Enable(true);
			m_pCheckBoxPetal->Enable(true);
			m_bIsClickSearchButton = false;
		}
	}
#else
	if(m_bIsClickSearchButton)
	{
		if(m_fElapseTime > .0f)
			m_fElapseTime -= fElapsedTime;
		else
		{
			m_pButtonReScan->Enable(true);
			m_bIsClickSearchButton = false;
		}
	}
#endif // PRE_ADD_PETALTRADE

	CDnCustomDlg::Process(fElapsedTime);
}

bool CDnMarketDirectBuyDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		{
			if( IsMouseInDlg() )
			{
				int nCurrentIndex = m_nCurrentPageNumber * SLOT_MAX;

				for(int i = 0 ; i < SLOT_MAX ; ++i)
				{
					if( m_pStaticToolTipArea[i]->IsMouseEnter() && m_pStaticToolTipArea[i]->IsShow() )
					{
						nCurrentIndex += i; // 현재 UI인덱스
						m_nMouseOverIndex = i;

						if( nCurrentIndex > (int)m_vecDirectMarketList.size()-1 )
						{
							m_nOnePrice = -1;
							return false;
						}

						int nPrice = m_vecDirectMarketList[nCurrentIndex].nPrice;
						int wCount = m_vecDirectMarketList[nCurrentIndex].wCount;

						m_nOnePrice  = wCount > 1 ? (nPrice + wCount - 1) / wCount : -1;
						m_bPetalItem = (m_vecDirectMarketList[nCurrentIndex].cMethodCode == 3)? true : false;

						// 강화템바로사기는 페탈템을 안불러오지만 우선 작업은 해둠
						if(m_bPetalItem) m_pItemPriceTooltip->SetPetal(m_nOnePrice);
						else			 m_pItemPriceTooltip->SetMoney(m_nOnePrice);

						break;
					}
				}
			}
		}
		break;

	case WM_LBUTTONDOWN:
		{
		}
		break;

	case WM_LBUTTONDBLCLK:
		{
			float fMouseX = .0f;
			float fMouseY = .0f;
			POINT MousePoint;
			MousePoint.x = short(LOWORD(lParam));
			MousePoint.y = short(HIWORD(lParam));
			PointToFloat(MousePoint, fMouseX, fMouseY);

			for(int i = 0 ; i < SLOT_MAX ; ++i)
			{
				if(m_pStaticSelect[i]->IsInside(fMouseX, fMouseY))
				{
					if(m_nSelectedItemIndex > -1)
						ProcessItemBuy();
				}
			}

		}
		break;

	case WM_LBUTTONUP:
		{
			float fMouseX = .0f;
			float fMouseY = .0f;
			POINT MousePoint;
			MousePoint.x = short(LOWORD(lParam));
			MousePoint.y = short(HIWORD(lParam));
			PointToFloat(MousePoint, fMouseX, fMouseY);

			int nSlotIndex = m_nCurrentPageNumber * SLOT_MAX;
			for(int i = 0 ; i < SLOT_MAX ; ++i)
			{
				if( m_pStaticSelect[i]->IsInside(fMouseX, fMouseY) && m_pStaticSelect[i]->IsEnable() )
				{
					for(int j = 0 ; j < SLOT_MAX ; ++j)
					{
						m_pStaticSelect[j]->Show(false);
						m_nSelectedItemIndex = -1;
					}
					m_pStaticSelect[i]->Show(true);

					nSlotIndex += i;
					m_nSelectedItemIndex = nSlotIndex;
					break;
				}
			}
		}
		break;

	case WM_MOUSEWHEEL:
		{
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			ScreenToClient( hWnd, &MousePoint );
			lParam = MAKELPARAM( MousePoint.x, MousePoint.y );

			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount > 0 )
				{
					ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_PRIOR"), 0 );
				}
				else if( nScrollAmount < 0 )
				{
					ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_NEXT"), 0 );
				}

				CEtUIDialogBase::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );
				return true;
			}
		}
		break;
	}
	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMarketDirectBuyDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl("ID_BT_BUY") )
		{
			ProcessItemBuy();
		}
		else if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			Show(false);
			ResetUI();
			//ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl("ID_BT_CLOSE"), 0);
		}
		else if( IsCmdControl("ID_BT_PRIOR") )
		{
			PrevPage();
		}
		else if( IsCmdControl("ID_BT_NEXT") )
		{
			NextPage();
		}
		else if( IsCmdControl("ID_BT_RESCAN") )
		{
			// 새로고침
			RefreshItemPage();
			m_bIsClickSearchButton = true;
		}
#ifdef PRE_ADD_PETALTRADE
		else if( IsCmdControl("ID_BT_SEARCH"))
		{
			ReSearchMarketItem(); // 검색
			m_bIsClickSearchButton = true;
		}
#endif // PRE_ADD_PETALTRADE
	}
	else if(nCommand == EVENT_CHECKBOX_CHANGED)
	{
#ifdef PRE_ADD_PETALTRADE
		if(IsCmdControl("ID_CHECKBOX_GOLD") || IsCmdControl("ID_CHECKBOX_RESERVE"))
		{
			if( !m_pCheckBoxGold->IsChecked() && !m_pCheckBoxPetal->IsChecked() )
				m_pButtonSearch->Enable(false);
			else
				m_pButtonSearch->Enable(true);
		}
#endif // PRE_ADD_PETALTRADE
	}

}

void CDnMarketDirectBuyDlg::ProcessItemBuy()
{
	// 벡터 체크.
	if(m_vecDirectMarketList.empty())
		return;

	if(m_nSelectedItemIndex > (int)m_vecDirectMarketList.size() || m_nSelectedItemIndex < 0)
		return;

	// 소지금 체크.
	INT64 nNeedCoin = m_vecDirectMarketList[m_nSelectedItemIndex].nPrice;
	if( nNeedCoin > GetItemTask().GetCoin() )
	{
		// 돈이 부족함.
		GetInterface().MessageBox( 1706, MB_OK );
		return;
	}

	// 인벤부족.
	if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG, ITEM_SLOT_TYPE::ST_INVENTORY ) )
	{
		GetInterface().MessageBox( 1925, MB_OK );
		return;
	}

	// 페탈확인.
#if defined(PRE_ADD_PETALTRADE)
	if(m_vecDirectMarketList[m_nSelectedItemIndex].cMethodCode == 3) // 페탈
	{
		if (m_nSelectedItemIndex >= 0 && m_nSelectedItemIndex < SLOT_MAX)
		{
			if( m_pStaticPetal[m_nSelectedItemIndex] )
			{
				int nPetal = m_pStaticPetal[m_nSelectedItemIndex]->GetTextToInt();
				if(m_nCurrentMyPetal < nPetal)
				{
					GetInterface().MessageBox( 4160, MB_OK );
					return;
				}
			}
		}
	}
#endif // PRE_ADD_PETALTRADE
	
	TItemInfo itemInfo;
	CDnItem* pItem = NULL;
	if(CDnItem::MakeItemInfo(m_vecDirectMarketList[m_nSelectedItemIndex].nItemID, m_vecDirectMarketList[m_nSelectedItemIndex].wCount, itemInfo))
	{
		pItem = GetItemTask().CreateItem(itemInfo);
		if(!pItem) return;
		m_pPrevSelectedItem = pItem;
	}

	if( pItem )
	{
		std::wstring wszMsg;
		int nGold, nSilver, nBronze;
		nGold   = m_vecDirectMarketList[m_nSelectedItemIndex].nPrice / 10000;
		nSilver = (m_vecDirectMarketList[m_nSelectedItemIndex].nPrice % 10000) / 100;
		nBronze = m_vecDirectMarketList[m_nSelectedItemIndex].nPrice % 100;
		m_nBuyItemDBID = m_vecDirectMarketList[m_nSelectedItemIndex].nMarketDBID;

		if(m_vecDirectMarketList[m_nSelectedItemIndex].cMethodCode == 1) // 게임코인
			wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4050 ) , pItem->GetName(), nGold, nSilver, nBronze );
		else if(m_vecDirectMarketList[m_nSelectedItemIndex].cMethodCode == 3) // 페탈
			wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4161 ) , pItem->GetName(), m_vecDirectMarketList[m_nSelectedItemIndex].nPrice );

		GetInterface().MessageBox( wszMsg.c_str(), MB_YESNO, MESSAGEBOX_DIRECT_BUY_ITEM, this );
	}
}

void CDnMarketDirectBuyDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		switch(nID)
		{
		case MESSAGEBOX_DIRECT_BUY_ITEM:
			{
				if( IsCmdControl("ID_YES") )
				{
					if(m_nBuyItemDBID != -1)
					{
						GetTradeTask().GetTradeMarket().RequestMarketMiniBuy( m_nBuyItemDBID );
					}

					m_nBuyItemDBID = -1;
				}
				else if( IsCmdControl("ID_NO"))
				{
					// 취소
					m_nBuyItemDBID = -1;
				}
			}
			break;
		}
	}
}

void CDnMarketDirectBuyDlg::SendOpenMarketPacket(int nitemID)
{
	// 아이템 정보를 셋팅하고, 패킷을 날린다.
	SendMarketMiniList(nitemID, 0);
	ResetUI();
}

void CDnMarketDirectBuyDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
	{
#ifdef PRE_ADD_PETALTRADE
		m_pCheckBoxGold->SetChecked(true, false);
		m_pCheckBoxPetal->SetChecked(true, false);
		//m_pButtonSearch->Enable(false);
#endif
	}

	CDnCustomDlg::Show( bShow );
}

void CDnMarketDirectBuyDlg::RecvOpenMarketPacket(SCMarketMiniList* pPacket)
{	// 아이템 요청후 리시브 되는곳.
	if(pPacket->nRetCode != ERROR_NONE)
	{
		// 지금은 구매할수 없습니다. 잠시후에 다시 시도해주세요.
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4667), MB_OK );
		return;
	}

	if(pPacket->cMarketCount == 0)
	{
		// 거래소에 등록된 아이템이 없습니다.
		GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4170), MB_OK );
		return;
	}

	// 변수 초기화
	ResetData();

	// 아이템 리스트 설정.
	for(int i = 0 ; i < pPacket->nMarketTotalCount ; ++i)
	{
		m_vecDirectMarketList.push_back(pPacket->MarketInfo[i]);
	}

	// 페이지 설정
	m_nMaxPageNumber = ComputeMaxPage(pPacket->nMarketTotalCount);
	SetCurrentItemPage(m_nCurrentPageNumber);
	if(m_nMaxPageNumber == 1)
	{
		m_pPrevButton->Enable(false);
		m_pNextButton->Enable(false);
	}

#ifdef PRE_ADD_PETALTRADE
	GetTradeTask().GetTradeMarket().RequestMarketPetalBalance(); // 페탈 요청
#endif // PRE_ADD_PETALTRADE
	
	Show(true);
}

int CDnMarketDirectBuyDlg::ComputeMaxPage(int nVectorSize)
{
	int nPageCnt   = nVectorSize / SLOT_MAX;
	int nTempValue = nVectorSize % SLOT_MAX;

	if(nTempValue > 0)
		nPageCnt += 1;

	return nPageCnt;
}

void CDnMarketDirectBuyDlg::RefreshItemPage()
{
	// 리프래시.
	if(m_vecDirectMarketList.empty()) return;
	
	int nItemID = m_vecDirectMarketList[0].nItemID;
	SendMarketMiniList(nItemID, 1);

	m_fElapseTime = SEARCH_CLICK_DELAY_TIME;
	m_pButtonReScan->Enable(false);
}

#ifdef PRE_ADD_PETALTRADE
void CDnMarketDirectBuyDlg::ReSearchMarketItem()
{
	// 1. 골드인지 페탈인지 체크하고, 서버에 요청.
	// 2. Recv가 오면, 아이템 리스트만 날리고, 리스트 재구성.
	char cPayMethodCode = 0;
	bool bIsChecked_Gold  = m_pCheckBoxGold->IsChecked();
	bool bIsChecked_Petal = m_pCheckBoxPetal->IsChecked();	
	if( !bIsChecked_Gold && !bIsChecked_Petal ) return;

	// 0 혼합 / 1 코인 / 3 페탈
	if(bIsChecked_Petal && bIsChecked_Gold)
		cPayMethodCode = 0;
	else if(bIsChecked_Gold)
		cPayMethodCode = 1;
	else if(bIsChecked_Petal)
		cPayMethodCode = 3;

	if(!m_vecDirectMarketList.empty())
	{
		int nItemID = m_vecDirectMarketList[0].nItemID;
		SendMarketMiniList(nItemID, cPayMethodCode);
		m_fElapseTime = SEARCH_CLICK_DELAY_TIME;
		m_pButtonSearch->Enable(false);
		m_pCheckBoxGold->Enable(false);
		m_pCheckBoxPetal->Enable(false);
	}
}
#endif // PRE_ADD_PETALTRADE


#endif // PRE_ADD_DIRECT_BUY_UPGRADEITEM