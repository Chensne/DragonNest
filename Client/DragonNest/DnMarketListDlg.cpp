#include "StdAfx.h"
#include "DnMarketListDlg.h"
#include "DnMoneyInputDlg.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"
#include "DnMarketTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(PRE_ADD_PETALTRADE)
std::string CDnMarketListDlg::m_sUIFileName = std::string("MarketListDlg_wPetal.ui");;
#else
std::string CDnMarketListDlg::m_sUIFileName = std::string("MarketListDlg.ui");;
#endif	//#if defined(PRE_ADD_PETALTRADE)

CDnMarketListDlg::CDnMarketListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
, m_pItem ( NULL )
, m_nMarketDBID( -1 )
, m_nMoney( 0 )
, m_nOnePrice( -1 )
, m_bPetalTrade( false )
, m_bRemainTime( true )
, m_pItemSlotButton( NULL )
{	
}

CDnMarketListDlg::~CDnMarketListDlg(void)
{
	SAFE_DELETE( m_pItem );
}

void CDnMarketListDlg::Initialize(bool bShow)
{
	BaseClass::Initialize(CEtResourceMng::GetInstance().GetFullName( m_sUIFileName.c_str() ).c_str(), bShow);
}

void CDnMarketListDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_BUTTON_ITEMSLOT");
	m_pItemSlotButton->SetSlotType( ST_INVENTORY );
	m_pItemSlotButton->SetSlotIndex( 0 );
}

void CDnMarketListDlg::InitCustomControl( CEtUIControl *pControl )
{
}

void CDnMarketListDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	BaseClass::Show( bShow );
}

void CDnMarketListDlg::ProcessAlpha( float fElapsedTime )
{
	if( m_pItemSlotButton && m_pItemSlotButton->IsEnable() ) {
		static float fAlphaSpeed = 5.0f;
		float fNewAlpha = m_pItemSlotButton->GetImageAlpha() + fElapsedTime * fAlphaSpeed;
		if( fNewAlpha > 1.0f ) fNewAlpha = 1.0f;
		m_pItemSlotButton->SetImageAlpha( fNewAlpha );
	}
}

void CDnMarketListDlg::Process( float fElapsedTime )
{
	if( m_nOnePrice != -1 ) {
		CDnMarketTabDlg* pMarketTabDlg = (CDnMarketTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG);
		if( pMarketTabDlg ) {
			bool bShowToolTip = CONTROL( Button, ID_MONEY_OVER)->IsMouseEnter();		
			if( bShowToolTip ) {
				pMarketTabDlg->SetMoneyToolTipValue( m_nOnePrice, m_bPetalTrade );
			}
		}
	}

	BaseClass::Process( fElapsedTime );
}

void CDnMarketListDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMarketListDlg::SetProperty( TMarketCalculationInfo &Info, float fAlpha )			// MarketCalculationList
{
	m_nMarketDBID = Info.nMarketDBID;

	TItemInfo itemInfo;
	memset( &itemInfo, 0, sizeof(TItemInfo) );
	itemInfo.cSlotIndex = 0;
	itemInfo.Item.nItemID = Info.nItemID;
	itemInfo.Item.wCount = Info.wCount;
	itemInfo.Item.wDur = Info.wDur;
	itemInfo.Item.nRandomSeed = Info.nRandomSeed;
	itemInfo.Item.cLevel = Info.cLevel;
	itemInfo.Item.cPotential = Info.cPotential;
	itemInfo.Item.cOption = Info.cOption;
	itemInfo.Item.cSealCount = Info.cSealCount;
	itemInfo.Item.bEternity = true;		// 기간제를 무인상점에 팔순없다
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	itemInfo.Item.cPotentialMoveCount = Info.cPotentialMoveCount;
#endif

#if defined(PRE_ADD_PETALTRADE)
	m_bPetalTrade = Info.cPayMethodCode == 3 ? true : false;
#else
	m_bPetalTrade = false;
#endif	//#if defined(PRE_ADD_PETALTRADE)

	WCHAR wszBuffer[32]={0};
	INT64 nMoney = 0;
	std::wstring strString;

	m_nMoney = Info.nPrice;
	m_nOnePrice = Info.wCount > 1 ?  ((Info.nPrice + Info.wCount - 1) / Info.wCount ) : -1;

	SetPetalUI();
	if( false == m_bPetalTrade )
	{
		CDnMoneyControl::GetStrMoneyG( Info.nPrice, wszBuffer, &nMoney );
		CONTROL( Static, ID_GOLD )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_GOLD )->SetTooltipText( strString.c_str() );

		CDnMoneyControl::GetStrMoneyS( Info.nPrice, wszBuffer, &nMoney );
		CONTROL( Static, ID_SILVER )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_SILVER )->SetTooltipText( strString.c_str() );

		CDnMoneyControl::GetStrMoneyC( Info.nPrice, wszBuffer, &nMoney );
		CONTROL( Static, ID_BRONZE )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_BRONZE )->SetTooltipText( strString.c_str() );
	}
	else
	{
#ifdef PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_PETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( (int)m_nMoney ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_PETAL )->SetIntToText( (int)m_nMoney );
#endif // PRE_MOD_PETAL_WRITE
		DN_INTERFACE::UTIL::GetValue_2_String( (int)m_nMoney, strString );
		CONTROL( Static, ID_PETAL )->SetTooltipText( strString.c_str() );
	}

	std::wstring wszStr;
	DN_INTERFACE::STRING::GetTimeText( wszStr, Info.tBuyDate );		
	CONTROL( Static, ID_STATIC_INFO)->SetText( wszStr );

	SAFE_DELETE( m_pItem );
	m_pItem = GetItemTask().CreateItem(itemInfo);
	if( m_pItem ) {
		WCHAR szTempSub1[256] = {0,};
		WCHAR szTemp[256] = {0,};
		if( m_pItem->GetEnchantLevel() != 0 )
			swprintf_s( szTempSub1, 256, L"+%d ", m_pItem->GetEnchantLevel() );
		swprintf_s( szTemp, 256, L"%s%s", szTempSub1, m_pItem->GetName() );
		CONTROL( Static, ID_ITEM_NAME )->SetText( szTemp );
		CONTROL( Static, ID_ITEM_NAME )->SetTextColor( DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( m_pItem->GetItemRank() ) );

		m_pItemSlotButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	}
	m_pItemSlotButton->SetImageAlpha( fAlpha );

	CONTROL( Static, ID_STATIC_PRIMIUM )->Show( false );
}

void CDnMarketListDlg::SetProperty( TMyMarketInfo &Info, float fAlpha )		// MarketSellList
{
	m_nMarketDBID = Info.nMarketDBID;

	TItemInfo itemInfo;
	memset( &itemInfo, 0, sizeof(TItemInfo));
	itemInfo.cSlotIndex = 0;
	itemInfo.Item.nItemID = Info.nItemID;
	itemInfo.Item.wCount = Info.wCount;
	itemInfo.Item.wDur = Info.wDur;
	itemInfo.Item.nRandomSeed = Info.nRandomSeed;
	itemInfo.Item.cLevel = Info.cLevel;
	itemInfo.Item.cPotential = Info.cPotential;
	itemInfo.Item.cOption = Info.cOption;
	itemInfo.Item.cSealCount = Info.cSealCount;
	itemInfo.Item.bEternity = true;		// 기간제를 무인상점에 팔순없다
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	itemInfo.Item.cPotentialMoveCount = Info.cItemPotentialMoveCount;
#endif

#if defined(PRE_ADD_PETALTRADE)
	m_bPetalTrade = Info.cPayMethodCode == 3 ? true : false;
#else
	m_bPetalTrade = false;
#endif	//#if defined(PRE_ADD_PETALTRADE)

	WCHAR wszBuffer[32] = {0};
	INT64 nMoney = 0;
	std::wstring strString;

	m_nOnePrice = Info.wCount > 1 ?  ((Info.nItemPrice+Info.wCount-1) / Info.wCount) : -1;
	SetPetalUI();
	if( false == m_bPetalTrade )
	{
		CDnMoneyControl::GetStrMoneyG( Info.nItemPrice, wszBuffer, &nMoney );
		CONTROL( Static, ID_GOLD )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_GOLD )->SetTooltipText( strString.c_str() );

		CDnMoneyControl::GetStrMoneyS( Info.nItemPrice, wszBuffer, &nMoney );
		CONTROL( Static, ID_SILVER )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_SILVER )->SetTooltipText( strString.c_str() );

		CDnMoneyControl::GetStrMoneyC( Info.nItemPrice, wszBuffer, &nMoney );
		CONTROL( Static, ID_BRONZE )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_BRONZE )->SetTooltipText( strString.c_str() );
	}
	else
	{
		m_nMoney = Info.nItemPrice;
#ifdef PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_PETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( (int)m_nMoney ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_PETAL )->SetIntToText( (int)m_nMoney );
#endif // PRE_MOD_PETAL_WRITE
		DN_INTERFACE::UTIL::GetValue_2_String( (int)m_nMoney, strString );
		CONTROL( Static, ID_PETAL )->SetTooltipText( strString.c_str() );
	}

	if( Info.cSellType == 0 ) {
		swprintf_s( wszBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Info.nRemainTime > 0 ? 4068/*%d 시간 남음*/ : 4056/*기간 만료*/ ), Info.nRemainTime );
		m_bRemainTime = Info.nRemainTime > 0 ? true : false;
	}
	else {
		_wcscpy( wszBuffer, _countof(wszBuffer), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4020 /*판매완료*/ ), (int)wcslen(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4020 /*판매완료*/ )));
	}
	CONTROL( Static, ID_STATIC_INFO)->SetText( wszBuffer ); 

	SAFE_DELETE( m_pItem );
	m_pItem = GetItemTask().CreateItem( itemInfo );

	if( !m_pItem ) return;

	WCHAR szTempSub1[256] = {0,};
	WCHAR szTemp[256] = {0,};
	if( m_pItem->GetEnchantLevel() != 0 )
		swprintf_s( szTempSub1, 256, L"+%d ", m_pItem->GetEnchantLevel() );
	swprintf_s( szTemp, 256, L"%s%s", szTempSub1, m_pItem->GetName() );
	CONTROL( Static, ID_ITEM_NAME )->SetText( szTemp );
	CONTROL( Static, ID_ITEM_NAME )->SetTextColor( DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( m_pItem->GetItemRank() ) );

	m_pItemSlotButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_pItemSlotButton->SetImageAlpha( fAlpha );

	CONTROL( Static, ID_STATIC_PRIMIUM )->Show( false );
}

wchar_t* CDnMarketListDlg::GetItemName()
{
	return m_pItem ? m_pItem->GetName() : L"" ;
}

int CDnMarketListDlg::GetItemSound()
{
	return m_pItem ? m_pItem->GetDragSoundIndex() : -1;
}

void CDnMarketListDlg::SetPetalUI()
{
	if( true == m_bPetalTrade )
	{
		CONTROL( Static, ID_GOLD )->SetText( L"" );
		CONTROL( Static, ID_SILVER )->SetText( L"" );
		CONTROL( Static, ID_BRONZE )->SetText( L"" );

		CONTROL( Static, ID_STATIC_GOLD )->Show( false );
		CONTROL( Static, ID_STATIC_SILVER )->Show( false );
		CONTROL( Static, ID_STATIC_BRONZE )->Show( false );

		CONTROL( Static, ID_STATIC_PETAL )->Show( true );
	}
	else
	{
		CONTROL( Static, ID_PETAL )->SetText( L"" );

		CONTROL( Static, ID_STATIC_GOLD )->Show( true );
		CONTROL( Static, ID_STATIC_SILVER )->Show( true );
		CONTROL( Static, ID_STATIC_BRONZE )->Show( true );

		CONTROL( Static, ID_STATIC_PETAL )->Show( false );
	}
}