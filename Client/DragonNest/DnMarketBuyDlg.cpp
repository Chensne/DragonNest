#include "StdAfx.h"
#include "DnInterface.h"
#include "DnMarketBuyDlg.h"
#include "DnTradeTask.h"
#include "DnTradeMarket.h"
#include "DnTableDB.h"
#include "DnMarketBuyListDlg.h"
#include "DnMarketTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnItemTask.h"
#include "DnMainFrame.h"
#include "DnChatTabDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnMarketPriceTooltipDlg.h"
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#include "DnNameAutoComplete.h"
#include "DnInterfaceString.h"
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketBuyDlg::CDnMarketBuyDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )	
	, m_nTotalItemCount( 0 )
	, m_nBuyItemDBID( -1 )
	, m_pMarketPriceTooltipDlg( NULL )
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	, m_pSearchItemListBox( NULL )
	, m_pSearchItemListBoxBoard( NULL )
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE
{
	m_LastSearchInfo.cJob = -1;
	MakeEmptyKey ( m_LastBuyKey );
}

CDnMarketBuyDlg::~CDnMarketBuyDlg(void)
{
	CONTROL( ListBoxEx, ID_LISTBOXEX )->RemoveAllItems();
	for( int itr = 0; itr < (int)m_pNodes.size(); ++itr )
		SAFE_DELETE( m_pNodes[itr] );

	SAFE_DELETE( m_pMarketPriceTooltipDlg );
}

void CDnMarketBuyDlg::Initialize( bool bShow )
{
#if defined(PRE_ADD_PETALTRADE)
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketBuyDlg_wPetal.ui" ).c_str(), bShow );	
#else
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketBuyDlg.ui" ).c_str(), bShow );	
#endif	//#if defined(PRE_ADD_PETALTRADE)
}

void CDnMarketBuyDlg::InitialUpdate()
{
	m_pMarketPriceTooltipDlg = new CDnMarketPriceTooltipDlg( UI_TYPE_MODAL, this );
	m_pMarketPriceTooltipDlg->Initialize( false );

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TEXCHANGETRADE );

	CTreeItem *parentNode = NULL, *lastNode = NULL;
	int nSize = pSox->GetItemCount();
	std::vector<NODE>::iterator itor;

	m_Nodes.reserve( nSize );
	for( int itr = 0; itr < nSize; ++itr )
	{
		int nItemID = pSox->GetItemID( itr );

		NODE * pNode = new NODE( pSox->GetFieldFromLablePtr( nItemID, "_NameID" )->GetInteger()
						, ALL
						, ALL
						, ALL
						, ALL
						, nItemID
						, pSox->GetFieldFromLablePtr( nItemID, "_GroupID" )->GetInteger()
						, pSox->GetFieldFromLablePtr( nItemID, "_ExchangeType" )->GetInteger() );

		m_pNodes.push_back( pNode );

		// 0일경우 부모 노드이다
		if( 0 == pSox->GetFieldFromLablePtr( nItemID, "_ExchangeType" )->GetInteger() ) 
		{
			parentNode = CONTROL( TreeCtl, ID_TREE_LIST )->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pNode->m_nStringID ), textcolor::FONT_GREEN );
			parentNode->SetItemData( pNode );
		}
		else
		{
			lastNode = CONTROL( TreeCtl, ID_TREE_LIST )->AddChildItem( parentNode, CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pNode->m_nStringID ), textcolor::DARKGRAY );
			lastNode->SetItemData( pNode );
		}
	}

	boost::tuple<int, int, eSortType> sortTypes[] = {
		boost::make_tuple( 4040, 4069, MARKETSORT_ONEPRICE_ASC),
		boost::make_tuple( 4040, 4070, MARKETSORT_ONEPRICE_DESC),
		boost::make_tuple( 11, 4069, MARKETSORT_ITEM_ASC),
		boost::make_tuple( 11, 4070, MARKETSORT_ITEM_DESC),
		boost::make_tuple( 64, 4069, MARKETSORT_LEVEL_ASC),
		boost::make_tuple( 64, 4070, MARKETSORT_LEVEL_DESC),
		boost::make_tuple( 8002, 4069, MARKETSORT_ITEMGRADE_ASC),
		boost::make_tuple( 8002, 4070, MARKETSORT_ITEMGRADE_DESC)
	};

	for( int i = 0; i < _countof(sortTypes); i++ ) {
		int nPreUIString = sortTypes[i].get<0>();
		int nPostUIString = sortTypes[i].get<1>();
		eSortType sortType = sortTypes[i].get<2>();

		CONTROL( ComboBox, ID_COMBOBOX_SORT )->AddItem( 
			(std::wstring(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nPreUIString ))+ L" " + std::wstring(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nPostUIString ))).c_str(), 
			NULL, sortType, true );
	}

	CONTROL( ComboBox, ID_COMBOBOX_CLASS )->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 601), NULL, 0 );

	pSox = GetDNTable( CDnTableDB::TJOB );
	for( int itr = 0; itr < pSox->GetItemCount(); ++itr )
	{
		int nJobID = pSox->GetItemID(itr);
		if( 0 != pSox->GetFieldFromLablePtr( nJobID, "_Service")->GetInteger() )
		{
			int nJobStringID = pSox->GetFieldFromLablePtr( pSox->GetItemID(itr), "_JobName")->GetInteger();

			CONTROL( ComboBox, ID_COMBOBOX_CLASS )->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nJobStringID), NULL, nJobID );
		}
	}

	SetDefaultState();
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	m_pSearchItemListBox = GetControl<CEtUIListBox>( "ID_LISTBOX_SEARCHLIST" );
	m_pSearchItemListBox->Show( false );
	m_pSearchItemListBox->Enable( false );
	m_pSearchItemListBoxBoard = GetControl<CEtUIStatic>( "ID_STATIC_SEARCHBOARD" );
	m_pSearchItemListBoxBoard->Show( false );
//	CONTROL( Button, ID_BUTTON_SEARCH )->Enable( false );
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE
}

void CDnMarketBuyDlg::SetDefaultState()
{
	CONTROL( CheckBox, ID_CHECKBOX_NORMAL )->SetChecked( true );
	CONTROL( CheckBox, ID_CHECKBOX_MAGIC )->SetChecked( true );
	CONTROL( CheckBox, ID_CHECKBOX_RARE )->SetChecked( true );
	CONTROL( CheckBox, ID_CHECKBOX_EPIC )->SetChecked( true );
	CONTROL( CheckBox, ID_CHECKBOX_UNIQUE )->SetChecked( true );
	CONTROL( CheckBox, ID_CHECKBOX_LEGEND )->SetChecked( true );

	CONTROL( EditBox, ID_EDITBOX_LV_MIN )->SetText( L"1");
	CONTROL( EditBox, ID_EDITBOX_LV_MAX )->SetText( L"99");
	CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->SetText( L"");

	CONTROL( ListBoxEx, ID_LISTBOXEX )->RemoveAllItems();

	CONTROL( ComboBox, ID_COMBOBOX_SORT )->SetSelectedByIndex( 0 );
	CONTROL( TreeCtl, ID_TREE_LIST )->CollapseAll();
	CONTROL( TreeCtl, ID_TREE_LIST )->ResetSelectedItem();
	CONTROL( TreeCtl, ID_TREE_LIST )->ScrollToBegin();

	CONTROL( ComboBox, ID_COMBOBOX_CLASS )->SetSelectedByIndex( 0 );
	CONTROL( CheckBox, ID_CHECKBOX_CLASS )->SetChecked( true );

	CONTROL( CheckBox, ID_CHECKBOX_GOLD )->SetChecked( true );	// 골드
	CONTROL( CheckBox, ID_CHECKBOX0 )->SetChecked( true );		// 페탈

	GetControl<CEtUIButton>("ID_BUTTON0")->Show(true);
	GetControl<CEtUIButton>("ID_BUTTON0")->Enable(true);

	if( CDnActor::s_hLocalActor )
	{
		CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));
		CONTROL( ComboBox, ID_COMBOBOX_CLASS )->SetSelectedByValue( localActor->GetJobClassID() );
	}

	m_nTotalItemCount = 0;
	m_LastSearchInfo.nPageIndex = 1;
	m_LastSearchInfo.cJob = -1;
	m_CachedBuyList.clear();
	m_IsNewKey.clear();
	RefreshCount();
	RefreshPageControls();
}

void CDnMarketBuyDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_EDITBOX_CHANGE )
	{
		if( IsCmdControl( "ID_EDITBOX_LV_MIN" ) )
		{
			if( wcslen( CONTROL( EditBox, ID_EDITBOX_LV_MIN )->GetText() ) > 0 ) {
				int nMinLv = CONTROL( EditBox, ID_EDITBOX_LV_MIN )->GetTextToInt();
				if( nMinLv < 1 ) {
					CONTROL( EditBox, ID_EDITBOX_LV_MIN )->SetIntToText( 1 );
				}
				else if( nMinLv > PLAYER_MAX_LEVEL ) {
					CONTROL( EditBox, ID_EDITBOX_LV_MIN )->SetIntToText( PLAYER_MAX_LEVEL );
				}
			}

			return;
		}
		else if( IsCmdControl( "ID_EDITBOX_LV_MAX" ) )
		{
			if( wcslen( CONTROL( EditBox, ID_EDITBOX_LV_MAX )->GetText() ) > 0 ) {
				int nMaxLv = CONTROL( EditBox, ID_EDITBOX_LV_MAX )->GetTextToInt();
				if( nMaxLv < 1 ) {
					CONTROL( EditBox, ID_EDITBOX_LV_MAX )->SetIntToText( 1 );
				}
				else if( nMaxLv > PLAYER_MAX_LEVEL ) {
					CONTROL( EditBox, ID_EDITBOX_LV_MAX )->SetIntToText( PLAYER_MAX_LEVEL );
				}
			}
			return;
		}
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
		else if( IsCmdControl( "ID_IMEEDITBOX_SEARCHWORD" ) && uMsg != 44648 )
		{
			OnSetSearchItemList( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() + CEtUIIME::GetCompString() );
			return;
		}
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE
	}
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	else if( nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl( "ID_IMEEDITBOX_SEARCHWORD" ) )
		{
			std::wstring strEdit = CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText();
			std::wstring strCompString = CEtUIIME::GetCompString();
			if( strEdit.length() == 0 && strCompString.length() == 0 && uMsg == WM_IME_ENDCOMPOSITION )
				return;

			OnSetSearchItemList( strEdit + strCompString );
			return;
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION_END )
	{
		if( IsCmdControl( "ID_LISTBOX_SEARCHLIST" ) )
		{
			SListBoxItem* pSelectItem = m_pSearchItemListBox->GetSelectedItem();
			if( pSelectItem )
			{
				CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->ClearText();
				CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->SetText( pSelectItem->strText );
				m_pSearchItemListBox->RemoveAllItems();
				m_pSearchItemListBox->Show( false );
				m_pSearchItemListBox->Enable( false );
				m_pSearchItemListBoxBoard->Show( false );
//				CONTROL( Button, ID_BUTTON_SEARCH )->Enable( true );
				OnSearchItem();
			}
		}
		return;
	}
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE
	else if ( nCommand == EVENT_LISTBOX_ITEM_DBLCLK ) {
		if( IsCmdControl("ID_LISTBOXEX")) {
			ProcessBuy();
		}
	}
	else if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl( "ID_BUTTON_SEARCH" ) ) {
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
			OnSearchItem();
#else // PRE_ADD_ITEMNAME_AUTOCOMPLETE
#if defined(PRE_ADD_PETALTRADE)
			if( !CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked() && !CONTROL( CheckBox, ID_CHECKBOX0 )->IsChecked() )
			{
				GetInterface().MessageBox( 7614 );	// [캐시]와 [페탈]을 모두 선택하지 않는 것은 불가능합니다.
				return;
			}
#endif // PRE_ADD_PETALTRADE
			eItemRank ItemRank = ITEMRANK_D;

			std::vector< char > itemRanks;
			if( CONTROL( CheckBox, ID_CHECKBOX_NORMAL )->IsChecked() ) itemRanks.push_back( ITEMRANK_D );
			if ( CONTROL( CheckBox, ID_CHECKBOX_MAGIC )->IsChecked() ) itemRanks.push_back( ITEMRANK_C );
			if ( CONTROL( CheckBox, ID_CHECKBOX_RARE )->IsChecked() ) itemRanks.push_back( ITEMRANK_B );
			if ( CONTROL( CheckBox, ID_CHECKBOX_EPIC )->IsChecked() ) itemRanks.push_back( ITEMRANK_A );
			if ( CONTROL( CheckBox, ID_CHECKBOX_UNIQUE )->IsChecked() ) itemRanks.push_back( ITEMRANK_S );
			if ( CONTROL( CheckBox, ID_CHECKBOX_LEGEND )->IsChecked() ) itemRanks.push_back( ITEMRANK_SS );

			if (itemRanks.size() == 0) return;

			AssertEqual( itemRanks.size(), (ITEMRANK_MAX) );

			CTreeItem *pSelectedItem = CONTROL( TreeCtl, ID_TREE_LIST )->GetSelectedItem();

			NODE nodeAll(-1);
			NODE *pNode = NULL;
			if( pSelectedItem && pSelectedItem->GetItemData() ) {
				pNode = (NODE *)pSelectedItem->GetItemData();
				if( CONTROL( ComboBox, ID_COMBOBOX0 )->GetSelectedIndex() == 0 && pNode->m_MainType == ITEMTYPE_PETALTOKEN )
					return;
			}
			else {	

				std::vector<std::wstring> tokens;
				int length = (int)wcslen( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() );

				if( 2 > length ) 
				{
					GetInterface().MessageBox( 4165 );	// 모든 카테고리를 검색하려면 최소 2글자 이상 입력되어야 합니다.
					return;
				}

				CONTROL( ComboBox, ID_COMBOBOX0 )->GetSelectedIndex() == 0 ? nodeAll.m_MainType = -50 : nodeAll.m_MainType = -51;

				nodeAll.m_DetailType = -1;
				nodeAll.m_Job = JOB_NONE;
				pNode = &nodeAll;
			}

			if( pNode && m_RequestedKeys.empty() ) {		// 이전에 요청했던것을 전부 받았어야만 검색이 가능하다.
				int nMinLevel = CONTROL( EditBox, ID_EDITBOX_LV_MIN )->GetTextToInt();
				int nMaxLevel = CONTROL( EditBox, ID_EDITBOX_LV_MAX )->GetTextToInt();
				if( nMinLevel < 1 || nMinLevel > PLAYER_MAX_LEVEL ) {
					nMinLevel = 1;
					CONTROL( EditBox, ID_EDITBOX_LV_MIN )->SetIntToText( nMinLevel );
				}
				if( nMaxLevel < 1 || nMaxLevel > PLAYER_MAX_LEVEL - 1 ) {
					nMaxLevel = PLAYER_MAX_LEVEL - 1;
					CONTROL( EditBox, ID_EDITBOX_LV_MAX )->SetIntToText( nMaxLevel );
				}
//				nMinLevel = EtClamp( nMinLevel, 1, PLAYER_MAX_LEVEL );
//				nMaxLevel = EtClamp( nMaxLevel, 1, PLAYER_MAX_LEVEL );

				if ( nMinLevel > nMaxLevel ) {
					GetInterface().MessageBox( MESSAGEBOX_LEVEL_ERROR, MB_OK, MESSAGEBOX_LEVEL_ERROR, this); // UISTRING : 최저 레벨은 최대 레벨보다 클 수 없습니다
				}
				else {
					std::vector<std::wstring> tokens;
					TokenizeW( std::wstring( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() ) , tokens, L" ");

					for( int i = 0; i < (int)tokens.size(); i++) {	// 3글자 이상은 자른다.
						if( tokens.size() > 3 ) {
							tokens.erase( tokens.begin() + 3 , tokens.end() );
						}
					}

					int nSortValue = -1;
					bool bResult = CONTROL( ComboBox, ID_COMBOBOX_SORT )->GetSelectedValue( nSortValue );
					if( bResult && nSortValue != -1 ) {
						m_LastSearchInfo.cItemGradeArray.clear();
						m_LastSearchInfo.cItemGradeArray.resize( ITEMRANK_MAX, -1 );

						m_LastSearchInfo.nPageIndex = 1;
						m_LastSearchInfo.cMinLevel = nMinLevel;
						m_LastSearchInfo.cMaxLevel = nMaxLevel;
						m_LastSearchInfo.cItemGradeArray = itemRanks;
						m_LastSearchInfo.pwszSearch1 = tokens.size() > 0 ? const_cast<wchar_t*>(tokens[0].c_str()) : std::wstring(L"");
						m_LastSearchInfo.pwszSearch2 = tokens.size() > 1 ? const_cast<wchar_t*>(tokens[1].c_str()) : std::wstring(L"");
						m_LastSearchInfo.pwszSearch3 = tokens.size() > 2 ? const_cast<wchar_t*>(tokens[2].c_str()) : std::wstring(L"");
						int nJobValue = -1;
						CONTROL( ComboBox, ID_COMBOBOX_CLASS )->GetSelectedValue( nJobValue );
						m_LastSearchInfo.cJob = nJobValue;
						m_LastSearchInfo.nExchangeItemID = pNode->m_ExcahngeItemID;
						m_LastSearchInfo.bLowJobGroup = CONTROL( CheckBox, ID_CHECKBOX_CLASS )->IsChecked();
						m_LastSearchInfo.pwszSearch = std::wstring( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() );

						if( CONTROL( ComboBox, ID_COMBOBOX0 )->GetSelectedIndex() != 0 && pSelectedItem && pSelectedItem->GetItemData() )
						{
							m_LastSearchInfo.nMainType = pNode->m_MainType > 0 ?  pNode->m_MainType + 100 : pNode->m_MainType - 100;
							m_LastSearchInfo.cDetailType =  pNode->m_CashDetailType;
						}
						else
						{
							m_LastSearchInfo.nMainType = pNode->m_MainType;
							m_LastSearchInfo.cDetailType =  pNode->m_DetailType;
						}
						
						m_LastSearchInfo.cSortType = nSortValue;

						// 페이지 이동이 아닌 새로운 검색을 하게 되면 
						// 캐쉬된 데이타를 모두 지워준다.
						m_CachedBuyList.clear();
						m_IsNewKey.clear();
						RequestMarketBuyList();

						CONTROL( Button, ID_BUTTON_SEARCH )->Enable( false );
					}
				}
			}
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE
		}
		else if( IsCmdControl( "ID_BUTTON_BUY" ) ) {
			ProcessBuy();
		}
		else if( IsCmdControl( "ID_BUTTON_RESET" ) ) {
			SetDefaultState();
		}
		else if( IsCmdControl( "ID_BUTTON_REFRESH") ) {			
			m_CachedBuyList.clear();
			m_IsNewKey.clear();
			RequestMarketBuyList();
		}
		else if( IsCmdControl( "ID_BUTTON_BACK") ) {
			if( m_LastSearchInfo.nPageIndex > 1 ) {
				m_LastSearchInfo.nPageIndex--;
				RequestMarketBuyList();
				RefreshPageControls();
			}
			else {
				ASSERT( false );
			}
		}
		else if( IsCmdControl( "ID_BUTTON_NEXT") ) {
			if( m_LastSearchInfo.nPageIndex < GetTotalPage() ) {
				m_LastSearchInfo.nPageIndex++;
				RequestMarketBuyList();
				RefreshPageControls();
			}
			else {
				ASSERT( false );
			}
		}		
		else if( IsCmdControl( "ID_BUTTON_FIRST") ) {
			if( m_LastSearchInfo.nPageIndex > PAGE_COUNT_FOR_SCREEN ) {
				m_LastSearchInfo.nPageIndex = GetStartPage() - PAGE_COUNT_FOR_SCREEN;
				RequestMarketBuyList();
				RefreshPageControls();
			}
			else {
				ASSERT( false );
			}
		}
		else if( IsCmdControl( "ID_BUTTON_LAST") ) {
			if( (m_LastSearchInfo.nPageIndex + PAGE_COUNT_FOR_SCREEN - 1) / PAGE_COUNT_FOR_SCREEN  
				< (GetTotalPage() + PAGE_COUNT_FOR_SCREEN - 1 )  / PAGE_COUNT_FOR_SCREEN  ) {
				m_LastSearchInfo.nPageIndex = GetStartPage() + PAGE_COUNT_FOR_SCREEN;
				RequestMarketBuyList();
				RefreshPageControls();
			}
			else {
				ASSERT( false );
			}
		}
		else if( strstr( pControl->GetControlName(), "ID_STATIC_PAGE" ) != NULL ) {
			int nIndex = -1;
			if( sscanf( pControl->GetControlName() + strlen("ID_STATIC_PAGE"), "%d", &nIndex) == 1 ) {
				m_LastSearchInfo.nPageIndex = GetStartPage() + nIndex - 1;
				RequestMarketBuyList();
				RefreshPageControls();
			}
		}
		else if( IsCmdControl( "ID_SORT_NAME") ) {
			if( m_LastSearchInfo.cSortType == MARKETSORT_ITEM_DESC ) m_LastSearchInfo.cSortType = MARKETSORT_ITEM_ASC;
			else if( m_LastSearchInfo.cSortType == MARKETSORT_ITEM_ASC ) m_LastSearchInfo.cSortType = MARKETSORT_ITEM_DESC;
			else m_LastSearchInfo.cSortType = MARKETSORT_ITEM_ASC;
			RequestMarketBuyList();
		}
		else if( IsCmdControl( "ID_SORT_LEVEL") ) {
			if( m_LastSearchInfo.cSortType == MARKETSORT_LEVEL_DESC ) m_LastSearchInfo.cSortType = MARKETSORT_LEVEL_ASC;
			else if( m_LastSearchInfo.cSortType == MARKETSORT_LEVEL_ASC ) m_LastSearchInfo.cSortType = MARKETSORT_LEVEL_DESC;
			else m_LastSearchInfo.cSortType = MARKETSORT_LEVEL_ASC;
			RequestMarketBuyList();
		}
		else if( IsCmdControl( "ID_SORT_PRICE") ) {
			if( m_LastSearchInfo.cSortType == MARKETSORT_PRICE_DESC ) m_LastSearchInfo.cSortType = MARKETSORT_PRICE_ASC;
			else if( m_LastSearchInfo.cSortType == MARKETSORT_PRICE_ASC ) m_LastSearchInfo.cSortType = MARKETSORT_PRICE_DESC;
			else if( m_LastSearchInfo.cSortType == MARKETSORT_ONEPRICE_DESC ) m_LastSearchInfo.cSortType = MARKETSORT_ONEPRICE_ASC;
			else if( m_LastSearchInfo.cSortType == MARKETSORT_ONEPRICE_ASC ) m_LastSearchInfo.cSortType = MARKETSORT_ONEPRICE_DESC;
			else m_LastSearchInfo.cSortType = MARKETSORT_PRICE_ASC;
			RequestMarketBuyList();
		}
		else if( IsCmdControl( "ID_WHISPER" ) ) {
			CDnChatTabDlg * pChatTabDlg = GetInterface().GetChatDialog();
			SListBoxItem  * pSelectedItem = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSelectedItem();

			if( pChatTabDlg && pSelectedItem )
			{
				CDnMarketBuyListDlg *pItemDlg = (CDnMarketBuyListDlg*)pSelectedItem->pData;

				pChatTabDlg->ShowEx(true);
				pChatTabDlg->SetPrivateName( pItemDlg->GetSellerName() );
			}
		}
		else if( IsCmdControl( "ID_NAMESAVE" ) ) {
			SListBoxItem  * pSelectedItem = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSelectedItem();

			if( pSelectedItem ) 
			{
				CDnMarketBuyListDlg *pItemDlg = (CDnMarketBuyListDlg*)pSelectedItem->pData;

				CEtUILineEditBox::CopyStringToClipboard( const_cast<wchar_t*>( pItemDlg->GetSellerName() ) );
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1422 ), false );
			}
		}
		else if( IsCmdControl( "ID_BUTTON_PosPETAL" ) )
		{
			CDnMarketTabDlg * pMarketTab = (CDnMarketTabDlg *)m_pParentDialog;
			pMarketTab->ShowPosPetal( true );
		}
		else if( IsCmdControl( "ID_BT_PRICE" ) )
		{
			SListBoxItem  * pSelectedItem = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSelectedItem();

			if( pSelectedItem ) 
			{
				CDnMarketBuyListDlg *pItemDlg = (CDnMarketBuyListDlg*)pSelectedItem->pData;
				CDnItem * pItem = pItemDlg->GetItem();
				if( pItem )
				{
					m_pMarketPriceTooltipDlg->SetItemName( pItem );
					GetTradeTask().GetTradeMarket().RequestMarketPrice( -1, pItem->GetClassID(), pItem->GetEnchantLevel(), pItem->GetOptionIndex() );
				}
			}
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		else if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_TRADE, this );
			return;
		}
		else if( IsCmdControl( "ID_CLOSE_DIALOG" ) )
		{
			CDnMarketTabDlg* pDnMarketTabDlg = dynamic_cast<CDnMarketTabDlg*>( GetParentDialog() );
			if( pDnMarketTabDlg && pDnMarketTabDlg->GetCallBack() )
				pDnMarketTabDlg->GetCallBack()->OnUICallbackProc( pDnMarketTabDlg->GetDialogID(), EVENT_BUTTON_CLICKED, GetControl( "ID_CLOSE_DIALOG" ) );

			return;
		}
		else if( IsCmdControl("ID_BUTTON0") )
		{
			int a = 1;
			a = 2;
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMarketBuyDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

#ifdef PRE_ADD_INSTANT_CASH_BUY
	if( bShow )
	{
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_TRADE );
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
	}
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	if( !bShow )
	{
		m_pSearchItemListBox->RemoveAllItems();
		m_pSearchItemListBox->Show( false );
		m_pSearchItemListBoxBoard->Show( false );
	}
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE

	BaseClass::Show( bShow );
}

void CDnMarketBuyDlg::Process( float fElapsedTime ) 
{
	int nSize = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSize();
	for( int i = 0; i < nSize; i++) {
		CDnMarketBuyListDlg *pItemDlg = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetItem<CDnMarketBuyListDlg>( i );
		pItemDlg->ProcessAlpha( fElapsedTime );
	}

	if( CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSelectedItem() )
		CONTROL( Button, ID_BT_PRICE )->Enable( true );
	else
		CONTROL( Button, ID_BT_PRICE )->Enable( false );

	BaseClass::Process( fElapsedTime );
}

bool CDnMarketBuyDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() ) {
		return false;
	}

	if( ( CONTROL( ComboBox, ID_COMBOBOX_SORT ) && !CONTROL( ComboBox, ID_COMBOBOX_SORT )->IsOpenedDropDownBox() ) 
		&& ( CONTROL( ComboBox, ID_COMBOBOX_CLASS ) && !CONTROL( ComboBox, ID_COMBOBOX_CLASS )->IsOpenedDropDownBox() ) 
		)
	{
		switch( uMsg )
		{
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
						if( m_LastSearchInfo.nPageIndex > 1 )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BUTTON_BACK"), 0 );
					}
					else if( nScrollAmount < 0 )
					{
						if( m_LastSearchInfo.nPageIndex < GetTotalPage() )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BUTTON_NEXT"), 0 );
					}

					CEtUIDialogBase::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );
					return true;
				}
			}
			break;
		}
	}

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	if( uMsg == WM_LBUTTONDOWN )
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->IsInside( fMouseX, fMouseY ) )
		{
			std::wstring curText = CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText();
			OnSetSearchItemList( curText );
		}
		else if( m_pSearchItemListBox && m_pSearchItemListBox->IsShow() && m_pSearchItemListBox->IsInside( fMouseX, fMouseY ) == false )
		{
			m_pSearchItemListBox->Show( false );
			m_pSearchItemListBoxBoard->Show( false );
		}
	}
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE

	return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMarketBuyDlg::OnRecvUpdateList( std::vector< TMarketInfo > &vecBuyList, int nTotalCount )
{
	if( vecBuyList.empty() && nTotalCount < 0 ) {		// Something Error
		m_RequestedKeys.pop_front();
		if( m_RequestedKeys.empty() ) {
			CONTROL( Button, ID_BUTTON_SEARCH )->Enable( true );
		}
		return;
	}
	if( -1 != nTotalCount )
		m_nTotalItemCount = nTotalCount;

	if( m_RequestedKeys.empty() ) {
		return;		// NPC 클릭할때 최초로 한번 들어온다... 캐쉬사용해서 등록한 템들이 나중에 오면 될듯..;;
	}

	std::pair<int, char > requestedKey = m_RequestedKeys.front();
	m_RequestedKeys.pop_front();
	if( m_RequestedKeys.empty() ) {
		CONTROL( Button, ID_BUTTON_SEARCH )->Enable( true );
	}

	int nRemainCount = (int)vecBuyList.size();
	
	std::pair<int, char > requestedKeyIter = requestedKey;
	for( int i = 0; i < PAGE_COUNT_FOR_REQUEST; i++) 
	{
		AssertEqual( m_CachedBuyList.count( requestedKeyIter ) , 0 );
		m_CachedBuyList[ requestedKeyIter ].clear();
		int nInsertItemCount = EtMin( nRemainCount, MAX_ITEM_COUNT_FOR_PAGE );
		m_CachedBuyList[ requestedKeyIter ].insert( m_CachedBuyList[ requestedKeyIter ].begin(),
																	 vecBuyList.begin() + i * MAX_ITEM_COUNT_FOR_PAGE,   
																	vecBuyList.begin() + i * MAX_ITEM_COUNT_FOR_PAGE + nInsertItemCount  );
		requestedKeyIter.first++;
		nRemainCount -= nInsertItemCount;
		if( nRemainCount <= 0 ) break;
	}

	// 현재 페이지에 보여질 정보가 왔다면....
	if( m_CachedBuyList.count( std::make_pair( m_LastSearchInfo.nPageIndex, m_LastSearchInfo.cSortType ) ) != 0 ) {
		UpdateListItems();
	}

	RefreshCount();
	RefreshPageControls();
}

// 구입 성공시...
void CDnMarketBuyDlg::OnRecvRemoveItem( int nMarketDBID )
{
	if( nMarketDBID == -1 ) {
		MakeEmptyKey( m_LastBuyKey );		// 뭔가 사는것이 실패 햇다...
		return;
	}
	if( m_CachedBuyList.count( m_LastBuyKey ) == 0 ) {
		return;
	}

	for( int i = 0; i < (int)m_CachedBuyList[ m_LastBuyKey ].size(); i++) {
		if( m_CachedBuyList[ m_LastBuyKey ][ i ].nMarketDBID == nMarketDBID ) {
			m_CachedBuyList[ m_LastBuyKey ].erase ( m_CachedBuyList[ m_LastBuyKey ].begin() + i );
			m_nTotalItemCount--;
			break;
		}
	}
	RefreshCount();
	RefreshPageControls();

	// 현재 페이지에 해당 아이템이 디스플레이 중인 경우에도 지워준다.
	int nSize = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSize();
	for( int i = 0; i < nSize; i++) {
		CDnMarketBuyListDlg *pItemDlg = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetItem<CDnMarketBuyListDlg>( i );
		if( pItemDlg->GetMarketDBID() == nMarketDBID ) {
			CEtSoundEngine::GetInstance().PlaySound( "2D", pItemDlg->GetItemSound() );
			std::wstring wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4073 /*%s 아이템을 구입하였습니다.*/) , pItemDlg->GetItemName() );
			GetInterface().MessageBox( wszMsg.c_str(), MB_OK );
			CONTROL( ListBoxEx, ID_LISTBOXEX )->RemoveItem( i );
			break;
		}
	}
	MakeEmptyKey( m_LastBuyKey );
}

void CDnMarketBuyDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
#ifdef PRE_ADD_INSTANT_CASH_BUY
	if( pControl == NULL && nID == INSTANT_CASHSHOP_BUY_DIALOG )
	{
		if( nCommand == 1 )
		{
			if( GetInterface().GetMainMenuDialog() )
				GetInterface().GetMainMenuDialog()->CloseMenuDialog();
		}
		return;
	}
#endif // PRE_ADD_INSTANT_CASH_BUY

	if( pControl )
		SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{		
		switch( nID ) 
		{
		case MESSAGEBOX_LEVEL_ERROR:			
			if (IsCmdControl("ID_OK")) {
				RequestFocus( CONTROL( EditBox, ID_EDITBOX_LV_MIN ) );
			}			
			break;
		case MESSAGEBOX_BUY_ITEM:
			if (IsCmdControl("ID_YES")) {
				GetTradeTask().GetTradeMarket().RequestMarketBuy( m_nBuyItemDBID );
				m_nBuyItemDBID = -1;
				m_LastBuyKey = GetLastKey();
			}
			break;
		}
	}
}

int CDnMarketBuyDlg::GetTotalPage()
{
	int nTotalPage = ( ( m_nTotalItemCount + MAX_ITEM_COUNT_FOR_PAGE - 1 ) / MAX_ITEM_COUNT_FOR_PAGE );
	return nTotalPage;
}

int CDnMarketBuyDlg::GetStartPage()
{
	int nStartPage = (m_LastSearchInfo.nPageIndex - 1) / PAGE_COUNT_FOR_SCREEN * PAGE_COUNT_FOR_SCREEN + 1;
	return nStartPage;
}

int CDnMarketBuyDlg::GetReqStartPage()
{
	int nReqStartPage = ( m_LastSearchInfo.nPageIndex - 1 ) / PAGE_COUNT_FOR_REQUEST * PAGE_COUNT_FOR_REQUEST + 1;
	return nReqStartPage;
}

void CDnMarketBuyDlg::UpdateListItems()
{
	AssertNotEqual( m_CachedBuyList.count( GetLastKey() ) ,  0 );
	std::vector<TMarketInfo>  &vecBuyList = m_CachedBuyList[ GetLastKey() ];

	CONTROL( ListBoxEx, ID_LISTBOXEX )->RemoveAllItems();

	bool bNewItems = (m_IsNewKey.count( GetLastKey() ) == 0 );
	if( bNewItems ) {
		m_IsNewKey.insert( GetLastKey() );
	}

	float fAlpha = -0.5f;
	static float fAlphaTerm = 0.5f;
	for each( TMarketInfo market in vecBuyList ) {

		if( market.bPremiumTrade )
		{
#if defined(PRE_ADD_PETALTRADE)
			CDnMarketBuyListDlg::SetUIFileName( std::string("MarketPRMBuyListDlg_wPetal.ui") );
#else
			CDnMarketBuyListDlg::SetUIFileName( std::string("MarketPRMBuyListDlg.ui") );
#endif	//#if defined(PRE_ADD_PETALTRADE)
		}
		else
		{
#if defined(PRE_ADD_PETALTRADE)
			CDnMarketBuyListDlg::SetUIFileName( std::string("MarketBuyListDlg_wPetal.ui") );
#else
			CDnMarketBuyListDlg::SetUIFileName( std::string("MarketBuyListDlg.ui") );
#endif	//#if defined(PRE_ADD_PETALTRADE)
		}

		CDnMarketBuyListDlg* pItemDlg = CONTROL( ListBoxEx, ID_LISTBOXEX )->AddItem<CDnMarketBuyListDlg>();
		CDnMarketTabDlg* pMarketTabDlg = (CDnMarketTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG);
		if( bNewItems ) {
			pItemDlg->SetProperty( market,  fAlpha);
			fAlpha -= fAlphaTerm;
		}
		else {
			pItemDlg->SetProperty( market,  1.0f );
		}
	}
}

void CDnMarketBuyDlg::RequestMarketBuyList()
{
	if( m_LastSearchInfo.cJob < 0 ) {
		return;
	}

	// 이미 받은것이 있다면 바로 뿌려주고, 없으면 요청한다.
	if( m_CachedBuyList.count( GetLastKey() ) != 0 ) 
	{
		/*ASSERT( !m_CachedBuyList[ GetLastKey() ].empty() );*/
		RefreshCount();
		UpdateListItems();
	}
	else {
		// m_lastSearchInfo.nPageIndex를 포함할 수 있고, 
		// 중복 요청을 피할 수 있는 요청 인덱스를 구한다.
		m_LastSearchInfo.nReqPageIndex = GetReqStartPage();

		// 이미 요청한적이 있는거는 응답이 올예정이므로, 두번 이상 요청하지 않는다.  ex)서버의 응답이 늦는 경우
		if( std::find( m_RequestedKeys.begin(), m_RequestedKeys.end(), GetLastReqKey() ) == m_RequestedKeys.end() )
		{
			char cPayMethodCode = 0;
#if defined(PRE_ADD_PETALTRADE)
			bool bCheckGold, bCheckPetal;
			bCheckGold = CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked();
			bCheckPetal = CONTROL( CheckBox, ID_CHECKBOX0 )->IsChecked();

			if( true == bCheckPetal && false == bCheckGold )
				cPayMethodCode = 3;
			else if( false == bCheckPetal && true == bCheckGold )
				cPayMethodCode = 1;
#endif	//#if defined(PRE_ADD_PETALTRADE)

			m_RequestedKeys.push_front( GetLastReqKey() );

		GetTradeTask().GetTradeMarket().RequestMarketBuyList( m_LastSearchInfo.nReqPageIndex,
			m_LastSearchInfo.cMinLevel,
			m_LastSearchInfo.cMaxLevel,
			&m_LastSearchInfo.cItemGradeArray.front(),
			m_LastSearchInfo.cJob,
			m_LastSearchInfo.nMainType,
			m_LastSearchInfo.cDetailType,
			m_LastSearchInfo.pwszSearch1.empty() ? NULL : const_cast<wchar_t*>(m_LastSearchInfo.pwszSearch1.c_str()),
			m_LastSearchInfo.pwszSearch2.empty() ? NULL : const_cast<wchar_t*>(m_LastSearchInfo.pwszSearch2.c_str()),
			m_LastSearchInfo.pwszSearch3.empty() ? NULL : const_cast<wchar_t*>(m_LastSearchInfo.pwszSearch3.c_str()),
			m_LastSearchInfo.cSortType,
			m_LastSearchInfo.nExchangeItemID,
			m_LastSearchInfo.bLowJobGroup,
			const_cast<wchar_t*>(m_LastSearchInfo.pwszSearch.c_str()),
			cPayMethodCode );
		}
	}
}

void CDnMarketBuyDlg::RefreshPageControls()
{
	int nStartPage = GetStartPage();

	for( int i = 0; i < PAGE_COUNT_FOR_SCREEN; i++ ) {
		CEtUIStatic *pStaticPage = _GC<CEtUIStatic>( FormatA("ID_STATIC_PAGE%d", i+1).c_str() );
		int nPage = i + nStartPage;
		if( nPage >= 100 ) {
			pStaticPage->SetText( FormatW(L"%d", nPage).c_str() );
		}
		else if( nPage >= 10 ) {
			pStaticPage->SetText( FormatW(L" %d ", nPage).c_str() );
		}
		else {
			pStaticPage->SetText( FormatW(L"  %d  ", nPage).c_str() );
		}

		if( nPage == m_LastSearchInfo.nPageIndex ) {
			pStaticPage->SetBgTextColor( D3DCOLOR_ARGB(90, 164, 32, 48), UI_STATE_MOUSEENTER );
			pStaticPage->SetBgTextColor(  D3DCOLOR_ARGB(90, 164, 32, 48) , UI_STATE_NORMAL );
		}
		else {
			pStaticPage->SetBgTextColor( D3DCOLOR_ARGB(128, 64, 86, 128), UI_STATE_MOUSEENTER );			
			pStaticPage->SetBgTextColor( D3DCOLOR_ARGB(90, 0, 0, 0), UI_STATE_NORMAL );		
		}

		pStaticPage->SetButton( true );

		bool bEnable = (m_nTotalItemCount + MAX_ITEM_COUNT_FOR_PAGE - 1 ) / MAX_ITEM_COUNT_FOR_PAGE >= nPage;

		pStaticPage->Enable( bEnable );
		pStaticPage->Show( bEnable );
	}

	if( GetTotalPage() == 0 ) {
		CONTROL( Button, ID_BUTTON_FIRST )->Enable( false );
		CONTROL( Button, ID_BUTTON_LAST )->Enable( false );

		CONTROL( Button, ID_BUTTON_BACK )->Enable( false );
		CONTROL( Button, ID_BUTTON_NEXT )->Enable( false );
	}
	else {
		CONTROL( Button, ID_BUTTON_FIRST )->Enable( GetStartPage() != 1 ? true : false );
		CONTROL( Button, ID_BUTTON_LAST )->Enable( GetStartPage() + PAGE_COUNT_FOR_SCREEN < GetTotalPage() ? true : false );

		CONTROL( Button, ID_BUTTON_BACK )->Enable( m_LastSearchInfo.nPageIndex != 1 ? true : false );
		CONTROL( Button, ID_BUTTON_NEXT )->Enable( m_LastSearchInfo.nPageIndex != GetTotalPage() ? true : false );
	}
}

void CDnMarketBuyDlg::RefreshCount()
{
	int nReadedCount = 0;
	int nMaxPage = ( (m_LastSearchInfo.nPageIndex - 1)/10 * 10) + 10;
	nMaxPage = nMaxPage > (int)m_CachedBuyList.size() ? (int)m_CachedBuyList.size() : nMaxPage;

	std::map< std::pair< int , char > , std::vector< TMarketInfo >	 >::iterator itMap = m_CachedBuyList.begin();
	while( itMap != m_CachedBuyList.end() ) {
		if( itMap->first.first <= nMaxPage &&
			itMap->first.second == m_LastSearchInfo.cSortType ) {		// 현재의 소트타입으로 읽은것들만 친다.
			nReadedCount += (int)itMap->second.size();
		}
		++itMap;
	}
	CONTROL( Static, ID_SEARCH_ACCOUNT )->SetText( FormatW(L"%d / %d", nReadedCount, m_nTotalItemCount) );
}

void CDnMarketBuyDlg::ProcessBuy()
{
	SListBoxItem *pSelectedItem = CONTROL( ListBoxEx, ID_LISTBOXEX )->GetSelectedItem();
	if( !pSelectedItem ) {
		return;
	}
	CDnMarketBuyListDlg *pItemDlg = (CDnMarketBuyListDlg*)pSelectedItem->pData;
	if( pItemDlg && pItemDlg->GetMarketDBID() != -1 ) {
		if( IsEmptyKey( m_LastBuyKey ) ) {										// 결과가 오기전까지는 중복 요청하지 않도록 한다.
#if defined(PRE_ADD_PETALTRADE)
			CDnMarketTabDlg * pMarketTab = (CDnMarketTabDlg *)m_pParentDialog;

			if( pItemDlg->IsPetalTrade() && pItemDlg->GetPetalPrice() > pMarketTab->GetPetalBalance() )
			{
				GetInterface().MessageBox( 4160, MB_OK );
				return;
			}
#endif	//#if defined(PRE_ADD_PETALTRADE)

			int nGold = 0, nSilver = 0, nCooper = 0;
			pItemDlg->GetMoney( nGold, nSilver, nCooper );			

			INT64 biPrice = nGold*10000 + nSilver*100 + nCooper;
			if( biPrice > GetItemTask().GetCoin() )
			{
				GetInterface().MessageBox( 1706, MB_OK );
				return;
			}

			m_nBuyItemDBID = pItemDlg->GetMarketDBID();
			std::wstring wszMsg;
#if defined(PRE_ADD_PETALTRADE)
			if( pItemDlg->IsPetalTrade() )
				wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, MESSAGEBOX_PETAL_BUY_ITEM ) , pItemDlg->GetItemName(), pItemDlg->GetPetalPrice() );
			else
				wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, MESSAGEBOX_BUY_ITEM ) , pItemDlg->GetItemName(), nGold, nSilver, nCooper );
#else
			wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, MESSAGEBOX_BUY_ITEM ) , pItemDlg->GetItemName(), nGold, nSilver, nCooper );
#endif
			GetInterface().MessageBox( wszMsg.c_str(), MB_YESNO, MESSAGEBOX_BUY_ITEM, this );			
		}
		else {
			ASSERT( false );
		}
	}
}

void CDnMarketBuyDlg::SetSearchItem( CDnItem *pItem )
{
	if( !pItem ) return;

	CONTROL( CheckBox, ID_CHECKBOX_NORMAL )->SetChecked( (pItem->GetItemRank() == ITEMRANK_D), false );
	CONTROL( CheckBox, ID_CHECKBOX_MAGIC )->SetChecked( (pItem->GetItemRank() == ITEMRANK_C), false );
	CONTROL( CheckBox, ID_CHECKBOX_RARE )->SetChecked( (pItem->GetItemRank() == ITEMRANK_B), false );
	CONTROL( CheckBox, ID_CHECKBOX_EPIC )->SetChecked( (pItem->GetItemRank() == ITEMRANK_A), false );
	CONTROL( CheckBox, ID_CHECKBOX_UNIQUE )->SetChecked( (pItem->GetItemRank() == ITEMRANK_S), false );
	CONTROL( CheckBox, ID_CHECKBOX_LEGEND )->SetChecked( (pItem->GetItemRank() == ITEMRANK_SS), false );

	CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->SetText( pItem->GetName() );

	int nLevel = pItem->GetLevelLimit();
	if( nLevel < 1 ) nLevel = 1;
	CONTROL( EditBox, ID_EDITBOX_LV_MIN )->SetIntToText( nLevel );
	CONTROL( EditBox, ID_EDITBOX_LV_MAX )->SetIntToText( nLevel );

	// 노드 벡터에서 적절한 노드를 찾아,
	NODE *pNode = NULL;

	int nExchangeType = pItem->GetExchangeType();

	for( int i = 0; i < (int)m_pNodes.size(); ++i ) 
	{
		if( m_pNodes[i]->m_ExchangeType == nExchangeType ) 
		{
			pNode = m_pNodes[i];
			break;
		}
	}

	if( 1 == pItem->GetNeedJobCount() )
		CONTROL( ComboBox, ID_COMBOBOX_CLASS )->SetSelectedByValue( pItem->GetNeedJob(0) );
	else
		CONTROL( ComboBox, ID_COMBOBOX_CLASS )->SetSelectedByValue( JOB_NONE );

	// 트리에서 데이터로 검색해 인덱스를 구한다.
	CTreeItem *pTreeItem = NULL;
	if( pNode ) {
		CONTROL( TreeCtl, ID_TREE_LIST )->CollapseAll();
		CONTROL( TreeCtl, ID_TREE_LIST )->SetSelectItem( CONTROL( TreeCtl, ID_TREE_LIST )->FindItemData( pNode ) );
	}
}

void CDnMarketBuyDlg::OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice )
{
	if( m_pMarketPriceTooltipDlg )
	{
		m_pMarketPriceTooltipDlg->Show( true );
		m_pMarketPriceTooltipDlg->OnRecvMarketPrice( nMarketDBID, vecPrice );
	}
}

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

void CDnMarketBuyDlg::OnSetSearchItemList( const std::wstring& strText )
{
//	CONTROL( Button, ID_BUTTON_SEARCH )->Enable( false );
	if( strText.length() == 0 || CDnMainFrame::GetInstance().GetNameAutoComplete() == NULL )
	{
		m_pSearchItemListBox->Show( false );
		m_pSearchItemListBox->Enable( false );
		m_pSearchItemListBoxBoard->Show( false );
		return;
	}

	//std::vector <CNameAutoComplete::TNameAutoItem*> vSearchItemList;
	std::vector <std::pair<CNameAutoComplete::TNameAutoItem*, std::vector<CNameAutoComplete::TNameAutoItem*>>> vSearchItemList;
	CDnMainFrame::GetInstance().GetNameAutoComplete()->GetMatchNameList( strText.c_str(), vSearchItemList );
	int nSerachListCount = static_cast<int>( vSearchItemList.size() );

	if( nSerachListCount > 0 )
	{
		m_pSearchItemListBox->RemoveAllItems();
		for( int i=0; i<nSerachListCount; i++ )
		{
			if (vSearchItemList[i].first == NULL && vSearchItemList[i].second.empty()) continue;

			if (vSearchItemList[i].first)
			{
				if( wcsstr( vSearchItemList[i].first->wstrName.c_str(), L"//" ) ) continue;
				if( vSearchItemList[i].first->nItemRank >= ITEMRANK_D && vSearchItemList[i].first->nItemRank < ITEMRANK_MAX )
					m_pSearchItemListBox->AddItem( vSearchItemList[i].first->wstrName.c_str(), NULL, i, DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( (eItemRank)vSearchItemList[i].first->nItemRank ) );
				else
					m_pSearchItemListBox->AddItem( vSearchItemList[i].first->wstrName.c_str(), NULL, i, textcolor::RED );
			}
			else if (vSearchItemList[i].second.empty() == false)
			{
				std::wstring wstrCollectWord;
				for (int nPartial = 0; nPartial < static_cast<int>(vSearchItemList[i].second.size()); nPartial++)
				{
					if (vSearchItemList[i].second[nPartial] == NULL) continue;

					wstrCollectWord += vSearchItemList[i].second[nPartial]->wstrName;
					if (nPartial != static_cast<int>(vSearchItemList[i].second.size()) - 1)
						wstrCollectWord += L" ";
				}
				m_pSearchItemListBox->AddItem( wstrCollectWord.c_str(), NULL, i, textcolor::RED );
			}

			//if( vSearchItemList[i] == NULL ) continue;
//			if( wcsstr( vSearchItemList[i]->wstrName.c_str(), L"//" ) ) continue;
//#ifdef PRE_ADD_LISTBOX_ELLIPSIS
//			if( vSearchItemList[i]->nItemRank >= ITEMRANK_D && vSearchItemList[i]->nItemRank < ITEMRANK_MAX )
//				m_pSearchItemListBox->AddItem( vSearchItemList[i]->wstrName.c_str(), NULL, i, DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( (eItemRank)vSearchItemList[i]->nItemRank ), true );
//			else
//				m_pSearchItemListBox->AddItem( vSearchItemList[i]->wstrName.c_str(), NULL, i, textcolor::RED, true );
//#else // PRE_ADD_LISTBOX_ELLIPSIS
//			if( vSearchItemList[i]->nItemRank >= ITEMRANK_D && vSearchItemList[i]->nItemRank < ITEMRANK_MAX )
//				m_pSearchItemListBox->AddItem( vSearchItemList[i]->wstrName.c_str(), NULL, i, DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( (eItemRank)vSearchItemList[i]->nItemRank ) );
//			else
//				m_pSearchItemListBox->AddItem( vSearchItemList[i]->wstrName.c_str(), NULL, i, textcolor::RED );
//#endif // PRE_ADD_LISTBOX_ELLIPSIS
		}

		m_pSearchItemListBox->Show( true );
		m_pSearchItemListBox->Enable( true );
		m_pSearchItemListBoxBoard->Show( true );
	}
	else
	{
		m_pSearchItemListBox->Show( false );
		m_pSearchItemListBox->Enable( false );
		m_pSearchItemListBoxBoard->Show( false );
	}
}

void CDnMarketBuyDlg::OnSearchItem()
{
#if defined(PRE_ADD_PETALTRADE)
	if( !CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked() && !CONTROL( CheckBox, ID_CHECKBOX0 )->IsChecked() )
	{
		GetInterface().MessageBox( 7614 );	// [캐시]와 [페탈]을 모두 선택하지 않는 것은 불가능합니다.
		return;
	}
#endif // PRE_ADD_PETALTRADE
	eItemRank ItemRank = ITEMRANK_D;

	std::vector< char > itemRanks;
	if( CONTROL( CheckBox, ID_CHECKBOX_NORMAL )->IsChecked() ) itemRanks.push_back( ITEMRANK_D );
	if ( CONTROL( CheckBox, ID_CHECKBOX_MAGIC )->IsChecked() ) itemRanks.push_back( ITEMRANK_C );
	if ( CONTROL( CheckBox, ID_CHECKBOX_RARE )->IsChecked() ) itemRanks.push_back( ITEMRANK_B );
	if ( CONTROL( CheckBox, ID_CHECKBOX_EPIC )->IsChecked() ) itemRanks.push_back( ITEMRANK_A );
	if ( CONTROL( CheckBox, ID_CHECKBOX_UNIQUE )->IsChecked() ) itemRanks.push_back( ITEMRANK_S );
	if ( CONTROL( CheckBox, ID_CHECKBOX_LEGEND )->IsChecked() ) itemRanks.push_back( ITEMRANK_SS );

	if (itemRanks.size() == 0) return;

	AssertEqual( itemRanks.size(), (ITEMRANK_MAX) );

	CTreeItem *pSelectedItem = CONTROL( TreeCtl, ID_TREE_LIST )->GetSelectedItem();

	NODE nodeAll(-1);
	NODE *pNode = NULL;
	if( pSelectedItem && pSelectedItem->GetItemData() ) {
		pNode = (NODE *)pSelectedItem->GetItemData();
		if( CONTROL( ComboBox, ID_COMBOBOX0 )->GetSelectedIndex() == 0 && pNode->m_MainType == ITEMTYPE_PETALTOKEN )
			return;
	}
	else {	

		std::vector<std::wstring> tokens;
		int length = (int)wcslen( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() );

		if( 2 > length ) 
		{
			GetInterface().MessageBox( 4165 );	// 모든 카테고리를 검색하려면 최소 2글자 이상 입력되어야 합니다.
			return;
		}

		CONTROL( ComboBox, ID_COMBOBOX0 )->GetSelectedIndex() == 0 ? nodeAll.m_MainType = -50 : nodeAll.m_MainType = -51;

		nodeAll.m_DetailType = -1;
		nodeAll.m_Job = JOB_NONE;
		pNode = &nodeAll;
	}

	if( pNode && m_RequestedKeys.empty() ) {		// 이전에 요청했던것을 전부 받았어야만 검색이 가능하다.
		int nMinLevel = CONTROL( EditBox, ID_EDITBOX_LV_MIN )->GetTextToInt();
		int nMaxLevel = CONTROL( EditBox, ID_EDITBOX_LV_MAX )->GetTextToInt();
		if( nMinLevel < 1 || nMinLevel > PLAYER_MAX_LEVEL ) {
			nMinLevel = 1;
			CONTROL( EditBox, ID_EDITBOX_LV_MIN )->SetIntToText( nMinLevel );
		}
		if( nMaxLevel < 1 || nMaxLevel > PLAYER_MAX_LEVEL - 1 ) {
			nMaxLevel = PLAYER_MAX_LEVEL - 1;
			CONTROL( EditBox, ID_EDITBOX_LV_MAX )->SetIntToText( nMaxLevel );
		}
//		nMinLevel = EtClamp( nMinLevel, 1, PLAYER_MAX_LEVEL );
//		nMaxLevel = EtClamp( nMaxLevel, 1, PLAYER_MAX_LEVEL );

		if ( nMinLevel > nMaxLevel ) {
			GetInterface().MessageBox( MESSAGEBOX_LEVEL_ERROR, MB_OK, MESSAGEBOX_LEVEL_ERROR, this); // UISTRING : 최저 레벨은 최대 레벨보다 클 수 없습니다
		}
		else {
			std::vector<std::wstring> tokens;
			if (m_pSearchItemListBox)
			{
				if (m_pSearchItemListBox->GetSelectedItem() == NULL)
				{
					SListBoxItem * pSelectItem = m_pSearchItemListBox->GetItem(0);
					if (pSelectItem)
					{
						CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->ClearText();
						CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->SetText( pSelectItem->strText );
						m_pSearchItemListBox->RemoveAllItems();
						m_pSearchItemListBox->Show( false );
						m_pSearchItemListBox->Enable( false );
						m_pSearchItemListBoxBoard->Show( false );
					}
				}
			}
			TokenizeW( std::wstring( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() ) , tokens, L" ");

			for( int i = 0; i < (int)tokens.size(); i++) {	// 3글자 이상은 자른다.
				if( tokens.size() > 3 ) {
					tokens.erase( tokens.begin() + 3 , tokens.end() );
				}
			}

			int nSortValue = -1;
			bool bResult = CONTROL( ComboBox, ID_COMBOBOX_SORT )->GetSelectedValue( nSortValue );
			if( bResult && nSortValue != -1 ) {
				m_LastSearchInfo.cItemGradeArray.clear();
				m_LastSearchInfo.cItemGradeArray.resize( ITEMRANK_MAX, -1 );

				m_LastSearchInfo.nPageIndex = 1;
				m_LastSearchInfo.cMinLevel = nMinLevel;
				m_LastSearchInfo.cMaxLevel = nMaxLevel;
				m_LastSearchInfo.cItemGradeArray = itemRanks;
				m_LastSearchInfo.pwszSearch1 = tokens.size() > 0 ? const_cast<wchar_t*>(tokens[0].c_str()) : std::wstring(L"");
				m_LastSearchInfo.pwszSearch2 = tokens.size() > 1 ? const_cast<wchar_t*>(tokens[1].c_str()) : std::wstring(L"");
				m_LastSearchInfo.pwszSearch3 = tokens.size() > 2 ? const_cast<wchar_t*>(tokens[2].c_str()) : std::wstring(L"");
				int nJobValue = -1;
				CONTROL( ComboBox, ID_COMBOBOX_CLASS )->GetSelectedValue( nJobValue );
				m_LastSearchInfo.cJob = nJobValue;
				m_LastSearchInfo.nExchangeItemID = pNode->m_ExcahngeItemID;
				m_LastSearchInfo.bLowJobGroup = CONTROL( CheckBox, ID_CHECKBOX_CLASS )->IsChecked();
				m_LastSearchInfo.pwszSearch = std::wstring( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText() );

				if( CONTROL( ComboBox, ID_COMBOBOX0 )->GetSelectedIndex() != 0 && pSelectedItem && pSelectedItem->GetItemData() )
				{
					m_LastSearchInfo.nMainType = pNode->m_MainType > 0 ?  pNode->m_MainType + 100 : pNode->m_MainType - 100;
					m_LastSearchInfo.cDetailType =  pNode->m_CashDetailType;
				}
				else
				{
					m_LastSearchInfo.nMainType = pNode->m_MainType;
					m_LastSearchInfo.cDetailType =  pNode->m_DetailType;
				}

				m_LastSearchInfo.cSortType = nSortValue;

				// 페이지 이동이 아닌 새로운 검색을 하게 되면 
				// 캐쉬된 데이타를 모두 지워준다.
				m_CachedBuyList.clear();
				m_IsNewKey.clear();
				RequestMarketBuyList();

				CONTROL( Button, ID_BUTTON_SEARCH )->Enable( false );
			}
		}
	}
}

#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE