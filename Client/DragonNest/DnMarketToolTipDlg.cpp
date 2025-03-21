#include "StdAfx.h"
#include "DnMarketToolTipDlg.h"
#include "DnMoneyInputDlg.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketToolTipDlg::CDnMarketToolTipDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnMarketToolTipDlg::~CDnMarketToolTipDlg()
{
}

void CDnMarketToolTipDlg::Initialize( bool bShow )
{
#if defined(PRE_ADD_PETALTRADE)
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketToolTipDlg_wPetal.ui" ).c_str(), bShow );
#else
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketToolTipDlg.ui" ).c_str(), bShow );
#endif	//#if defined(PRE_ADD_PETALTRADE)
	//SetFadeMode(true);
}

void CDnMarketToolTipDlg::InitialUpdate()
{
}

void CDnMarketToolTipDlg::Process( float fElapsedTime )
{
	BaseClass::Process( fElapsedTime );

	if( IsShow() )
	{
		UpdateTooltipDlg();
	}
}

void CDnMarketToolTipDlg::Show( bool bShow )
{ 
	if( bShow )
	{
		UpdateTooltipDlg();
	}
	else
	{
		//SetMoney( 0 );
	}
	BaseClass::Show( bShow );
}

void CDnMarketToolTipDlg::UpdateTooltipDlg()
{
	SUICoord uiCoord;
	GetDlgCoord( uiCoord );
	GetScreenMouseMovePoints( uiCoord.fX, uiCoord.fY );
	uiCoord.fX += 0.02f;
	uiCoord.fY += 0.02f;

	if( uiCoord.Right() > GetScreenWidthRatio() )
	{
		uiCoord.fX -= (uiCoord.Right() - GetScreenWidthRatio() - 0.001f);
	}

	if( uiCoord.Bottom() > GetScreenHeightRatio() )
	{
		uiCoord.fY -= (uiCoord.Bottom() - GetScreenHeightRatio() - 0.001f);
	}

	SetDlgCoord( uiCoord );
}

void CDnMarketToolTipDlg::SetMoney( int nOnePrice )
{
	INT64 nMoney = 0;
	std::wstring strString;
	wchar_t wszBuffer[16]={0,};

	CDnMoneyControl::GetStrMoneyG( nOnePrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_GOLD )->SetText( wszBuffer );
	CDnMoneyControl::GetStrMoneyS( nOnePrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_SILVER )->SetText( wszBuffer );	
	CDnMoneyControl::GetStrMoneyC( nOnePrice, wszBuffer, &nMoney );
	CONTROL( Static, ID_BRONZE )->SetText( wszBuffer );	

#if defined(PRE_ADD_PETALTRADE)
	CONTROL( Static, ID_PETAL )->SetText( L"" );

	CONTROL( Static, ID_STATIC_PETAL )->Show( false );
	CONTROL( Static, ID_STATIC0 )->Show( true );
#endif	//#if defined(PRE_ADD_PETALTRADE)
}

void CDnMarketToolTipDlg::SetPetal( int nOnePrice )
{
	INT64 nMoney = 0;
	std::wstring strString;
	wchar_t wszBuffer[16]={0,};

#ifdef PRE_MOD_PETAL_WRITE
	CONTROL( Static, ID_PETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( nOnePrice ).c_str() );
#else // PRE_MOD_PETAL_WRITE
	CONTROL( Static, ID_PETAL )->SetIntToText( nOnePrice );
#endif // PRE_MOD_PETAL_WRITE

	CONTROL( Static, ID_GOLD )->SetText( L"" );
	CONTROL( Static, ID_SILVER )->SetText( L"" );
	CONTROL( Static, ID_BRONZE )->SetText( L"" );

	CONTROL( Static, ID_STATIC_PETAL )->Show( true );
	CONTROL( Static, ID_STATIC0 )->Show( false );
}