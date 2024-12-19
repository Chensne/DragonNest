#include "StdAfx.h"
#include "DnMarketPriceTooltipDlg.h"
#include "DnMoneyInputDlg.h"
#include "DnInterfaceString.h"
#include "DnItem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketPriceTooltipDlg::CDnMarketPriceTooltipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnMarketPriceTooltipDlg::~CDnMarketPriceTooltipDlg()
{
}

void CDnMarketPriceTooltipDlg::Initialize( bool bShow )
{
#if defined(PRE_ADD_PETALTRADE)
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketPetalTooltipDlg.ui" ).c_str(), bShow );
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketGoldTooltipDlg.ui" ).c_str(), bShow );
#endif	//#if defined(PRE_ADD_PETALTRADE)
	//SetFadeMode(true);
}

void CDnMarketPriceTooltipDlg::InitialUpdate()
{
}

void CDnMarketPriceTooltipDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			Show( false );
			return;
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMarketPriceTooltipDlg::SetItemName( CDnItem * pItem )
{
	WCHAR szTempSub1[256] = {0,};
	WCHAR szTemp[256] = {0,};
	if( pItem->GetEnchantLevel() != 0 )
		swprintf_s( szTempSub1, 256, L"+%d ", pItem->GetEnchantLevel() );
	swprintf_s( szTemp, 256, L"%s%s", szTempSub1, pItem->GetName() );
	CONTROL( Static, ID_TEXT_NAME )->SetText( szTemp );
	CONTROL( Static, ID_TEXT_NAME )->SetTextColor( DN_INTERFACE::STRING::ITEM::RANK_2_COLOR( pItem->GetItemRank() ) );
}

void CDnMarketPriceTooltipDlg::OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice )
{
	INT64 nMoney = 0;
	wchar_t wszBuffer[16]={0,};

	CDnMoneyControl::GetStrMoneyG( vecPrice[0].nMinPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_LOW_GOLD )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyS( vecPrice[0].nMinPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_LOW_SILVER )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyC( vecPrice[0].nMinPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_LOW_BRONZE )->SetText( wszBuffer );

	CDnMoneyControl::GetStrMoneyG( vecPrice[0].nMaxPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_HIGH_GOLD )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyS( vecPrice[0].nMaxPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_HIGH_SILVER )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyC( vecPrice[0].nMaxPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_HIGH_BRONZE )->SetText( wszBuffer );

	CDnMoneyControl::GetStrMoneyG( vecPrice[0].nAvgPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_AVERAGE_GOLD )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyS( vecPrice[0].nAvgPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_AVERAGE_SILVER )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyC( vecPrice[0].nAvgPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_AVERAGE_BRONZE )->SetText( wszBuffer );

#if defined(PRE_ADD_PETALTRADE)
#ifdef PRE_MOD_PETAL_WRITE
	CONTROL( Static, ID_LOWPETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( vecPrice[1].nMinPrice ).c_str() );
	CONTROL( Static, ID_HIGHPETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( vecPrice[1].nMaxPrice ).c_str() );
	CONTROL( Static, ID_AVERAGEPETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( vecPrice[1].nAvgPrice ).c_str() );
#else // PRE_MOD_PETAL_WRITE
	CDnMoneyControl::GetStrMoneyP( vecPrice[1].nMinPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_LOWPETAL )->SetText( wszBuffer );

	CDnMoneyControl::GetStrMoneyP( vecPrice[1].nMaxPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_HIGHPETAL )->SetText( wszBuffer );

	CDnMoneyControl::GetStrMoneyP( vecPrice[1].nAvgPrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_AVERAGEPETAL )->SetText( wszBuffer );
#endif // PRE_MOD_PETAL_WRITE
#endif // PRE_ADD_PETALTRADE
}