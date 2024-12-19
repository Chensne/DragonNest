#include "StdAfx.h"
#include "DnMarketBuyListDlg.h"
#include "DnItemTask.h"
#include "DnItem.h"
#include "DnMoneyInputDlg.h"
#include "DnInterfaceString.h"
#include "DnTableDB.h"
#include "DnMarketTabDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(PRE_ADD_PETALTRADE)
std::string CDnMarketBuyListDlg::m_sUIFileName = std::string("MarketBuyListDlg_wPetal.ui");
#else
std::string CDnMarketBuyListDlg::m_sUIFileName = std::string("MarketBuyListDlg.ui");
#endif	//#if defined(PRE_ADD_PETALTRADE)

CDnMarketBuyListDlg::CDnMarketBuyListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
, m_pItem( NULL )
, m_nMarketDBID ( -1 )
, m_nGold( 0 )
, m_nSilver( 0 )
, m_nCooper( 0 )
, m_nOnePrice( -1 )
, m_nPetalPrice( 0 )
, m_bPetalTrade( false )
, m_pItemSlotButton( NULL )
{
}

CDnMarketBuyListDlg::~CDnMarketBuyListDlg(void)
{
	SAFE_DELETE( m_pItem );
}

void CDnMarketBuyListDlg::Initialize(bool bShow)
{
	BaseClass::Initialize(CEtResourceMng::GetInstance().GetFullName( m_sUIFileName.c_str() ).c_str(), bShow);
}

void CDnMarketBuyListDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_BUTTON_ITEMSLOT");
	m_pItemSlotButton->SetSlotType( ST_INVENTORY );
	m_pItemSlotButton->SetSlotIndex( 0 );
}

void CDnMarketBuyListDlg::InitCustomControl(CEtUIControl *pControl)
{
}

void CDnMarketBuyListDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	BaseClass::Show(bShow);
}

void CDnMarketBuyListDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	BaseClass::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnMarketBuyListDlg::ProcessAlpha( float fElapsedTime )
{
	if( m_pItemSlotButton && m_pItemSlotButton->IsEnable() ) {
		static float fAlphaSpeed = 3.0f;
		float fNewAlpha = m_pItemSlotButton->GetImageAlpha() + fElapsedTime * fAlphaSpeed;
		if( fNewAlpha > 1.0f ) fNewAlpha = 1.0f;
		m_pItemSlotButton->SetImageAlpha( fNewAlpha );
	}
}

void CDnMarketBuyListDlg::Process( float fElapsedTime )
{	
	if( m_nOnePrice != -1 ) {
		CDnMarketTabDlg* pMarketTabDlg = (CDnMarketTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG);
		if( pMarketTabDlg ) {
			bool bShowToolTip = CONTROL( Button, ID_MONEY_OVER)->IsMouseEnter();
			if( bShowToolTip  ) {
				pMarketTabDlg->SetMoneyToolTipValue( m_nOnePrice, m_bPetalTrade );
			}
		}	
	}

	BaseClass::Process( fElapsedTime );
}

void CDnMarketBuyListDlg::SetProperty( TMarketInfo &Info, float fAlpha )
{
	m_nMarketDBID =  Info.nMarketDBID;
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
	itemInfo.Item.cPotentialMoveCount = Info.cPotentialMoveCount;
#endif

	CONTROL( Static, ID_SELLER )->SetText( Info.wszSellerName );

	int nLevelLimit = 0;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( pSox && pSox->IsExistItem( Info.nItemID ) ) {
		nLevelLimit = pSox->GetFieldFromLablePtr( Info.nItemID, "_LevelLimit" )->GetInteger();
	}

	WCHAR wszBuffer[32]={ 0 };
	_itow_s( nLevelLimit, wszBuffer, 32, 10 );
	CONTROL( Static, ID_LEVEL )->SetText( wszBuffer );

	INT64 nMoney = 0;
	std::wstring strString;

	m_bPetalTrade = Info.cMethodCode == 3 ? true : false;

	SetPetalUI();

	m_nOnePrice = Info.wCount > 1 ? (Info.nPrice + Info.wCount - 1 ) / Info.wCount : -1;

	if( false == m_bPetalTrade )
	{
		CDnMoneyControl::GetStrMoneyG( Info.nPrice, wszBuffer, &nMoney );
		m_nGold = (int)nMoney;
		CONTROL( Static, ID_GOLD )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_GOLD )->SetTooltipText( strString.c_str() );

		CDnMoneyControl::GetStrMoneyS( Info.nPrice, wszBuffer, &nMoney );
		m_nSilver = (int)nMoney;
		CONTROL( Static, ID_SILVER )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_SILVER )->SetTooltipText( strString.c_str() );

		CDnMoneyControl::GetStrMoneyC( Info.nPrice, wszBuffer, &nMoney );
		m_nCooper = (int)nMoney;
		CONTROL( Static, ID_BRONZE )->SetText( wszBuffer );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nMoney, strString );
		CONTROL( Static, ID_BRONZE )->SetTooltipText( strString.c_str() );
	}
	else
	{
		m_nPetalPrice = Info.nPrice;
#ifdef PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_PETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetalPrice ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_PETAL )->SetIntToText( m_nPetalPrice );
#endif // PRE_MOD_PETAL_WRITE
		DN_INTERFACE::UTIL::GetValue_2_String( m_nPetalPrice, strString );
		CONTROL( Static, ID_PETAL )->SetTooltipText( strString.c_str() );
	}

	SAFE_DELETE( m_pItem );
	m_pItem = GetItemTask().CreateItem( itemInfo );
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

wchar_t* CDnMarketBuyListDlg::GetItemName()
{
	return m_pItem ? m_pItem->GetName() : L"" ;
}

int CDnMarketBuyListDlg::GetItemSound()
{
	return m_pItem ? m_pItem->GetDragSoundIndex() : -1;
}

void CDnMarketBuyListDlg::SetPetalUI()
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