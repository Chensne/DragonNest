#include "StdAfx.h"
#include "DnMarketTabDlg.h"
#include "DnMarketBuyDlg.h"
#include "DnMarketSellDlg.h"
#include "DnMarketPriceDlg.h"
#include "DnMarketAccountDlg.h"
#include "DnInterface.h"
#include "DnTradeTask.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnTableDB.h"
#include "DnMarketToolTipDlg.h"
#include "DnTooltipDlg.h"
#include "DnSimpleTooltipDlg.h"
#include "DnMarketPosPetalDlg.h"
#include "DnCashShopTask.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketTabDlg::CDnMarketTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pTabMarketBuy(NULL)
	, m_pTabMarketSell(NULL)
	, m_pTabMarketAccount(NULL)
	, m_pMarketBuyDlg(NULL)
	, m_pMarketSellDlg(NULL)
	, m_pMarketAccountDlg(NULL)
	, m_MarketToolTipDlg(NULL)
	, m_MarketPosPetal(NULL)
	, m_pPremiumBackGround(NULL)
	, m_pPremiumText(NULL)
	, m_bRestoreCoordNeeded( false )
	, m_fMagicScale(1.0f)
	, m_nPrevTabID( 0 )
{
}

CDnMarketTabDlg::~CDnMarketTabDlg(void)
{
	SAFE_DELETE( m_MarketToolTipDlg );
	SAFE_DELETE( m_MarketPosPetal );
}

void CDnMarketTabDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketTabDlg.ui" ).c_str(), bShow );

	if( CDnTradeTask::IsActive() )
	{
		GetTradeTask().GetTradeMarket().SetMarketDialog( this );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnMarketTabDlg::Initialize, 마켓 다이얼로그가 만들어 지기 전에 거래 테스크가 생성되어야 합니다." );
	}
}

void CDnMarketTabDlg::InitialUpdate()
{
	m_pTabMarketBuy = GetControl<CEtUIRadioButton>("ID_TAB_LIST");
	m_pMarketBuyDlg = new CDnMarketBuyDlg( UI_TYPE_CHILD, this, MARKET_BUY_DIALOG, this );
	m_pMarketBuyDlg->Initialize( false );
	AddTabDialog( m_pTabMarketBuy, m_pMarketBuyDlg );

	m_pTabMarketSell = GetControl<CEtUIRadioButton>("ID_TAB_REGISTER");
	m_pMarketSellDlg = new CDnMarketSellDlg( UI_TYPE_CHILD, this, MARKET_SELL_DIALOG, this );
	m_pMarketSellDlg->Initialize( false );
	AddTabDialog( m_pTabMarketSell, m_pMarketSellDlg );

	m_pTabMarketAccount = GetControl<CEtUIRadioButton>("ID_TAB_ACCOUNT");
	m_pMarketAccountDlg = new CDnMarketAccountDlg( UI_TYPE_CHILD, this, MARKET_ACCOUNT_DIALOG, this );
	m_pMarketAccountDlg->Initialize( false );
	AddTabDialog( m_pTabMarketAccount, m_pMarketAccountDlg );

	SetCheckedTab( m_pTabMarketBuy->GetTabID() );

	m_pPremiumBackGround = GetControl<CEtUIStatic>("ID_STATIC_PMBASE");
	m_pPremiumText = GetControl<CEtUIStatic>("ID_STATIC_PMTEXT");

	m_MarketToolTipDlg = new CDnMarketToolTipDlg( UI_TYPE_CHILD, this, -1, this );
	m_MarketToolTipDlg->Initialize( false );

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
	GetControl<CEtUIButton>("ID_BT_SMALLHELP")->Enable(true);
	GetControl<CEtUIButton>("ID_BT_SMALLHELP")->Show(true);
#endif
	

#if defined(PRE_ADD_PETALTRADE)
	m_MarketPosPetal = new CDnMarketPosPetal( UI_TYPE_MODAL, this, -1, this );
	m_MarketPosPetal->Initialize( false );
#endif	//#if defined(PRE_ADD_PETALTRADE)
}

void CDnMarketTabDlg::Process( float fElapsedTime  )
{
	m_MarketToolTipDlg->Show( false );

	BaseClass::Process( fElapsedTime );
}

bool CDnMarketTabDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMarketTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	ASSERT(pControl&&"CEtUITabDialog::ProcessCommand, pControl is NULL!");
	
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_RADIOBUTTON_CHANGED ) 
	{
		CEtUIRadioButton *pRadioButton = (CEtUIRadioButton*)pControl;
		if( !pRadioButton ) return;

		if( strstr( pControl->GetControlName(), "ID_TAB_REGISTER" ) )
		{
		}

		m_nPrevTabID = GetCurrentTabID();
	}
	else if( nCommand == EVENT_BUTTON_CLICKED )
	{
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_MARKET);
		}
#endif
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMarketTabDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( !bShow ) {
		m_pMarketBuyDlg->RefreshRequestNeed();
		m_pMarketSellDlg->RefreshRequestNeed();
		m_pMarketAccountDlg->RefreshRequestNeed();
#if defined(PRE_ADD_PETALTRADE)
		m_MarketPosPetal->Show( false );
#endif	//#if defined(PRE_ADD_PETALTRADE)
	}

	BaseClass::Show( bShow );

	CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( bShow ) {
		SetCheckedTab( m_pTabMarketBuy->GetTabID() );
		pInvenDlg->ShowTab( ITEM_SLOT_TYPE::ST_INVENTORY );
	
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );	
		SUICoord Coord, InvenCoord;
		GetDlgCoord( Coord );  m_SaveDlgCoord= Coord;
		Coord.fX = 0.f;
		Coord.fY = 0.f;
		pInvenDlg->GetDlgCoord( InvenCoord );  m_SaveInvenDlgCoord = InvenCoord;
		InvenCoord.fX = 0.f;
		InvenCoord.fY = 0.f;
		Coord = DlgCoordToScreenCoord( Coord );
		InvenCoord = pInvenDlg->DlgCoordToScreenCoord( InvenCoord );

		float fTotalWidthWithBlank = (Coord.fX + Coord.fWidth + (1.0f-InvenCoord.fX));
		if( fTotalWidthWithBlank > 0.98f ) {

			float fTotalWidth = (Coord.fWidth + InvenCoord.fWidth + pInvenDlg->GetCommonTab()->GetUICoord().fWidth);
			if( fTotalWidth < 1.0f ) {
				float fTotalBlank = ( Coord.fX + (1.0f - InvenCoord.fX - InvenCoord.fWidth ) );
				float fNeedRemoveBlank = fTotalWidthWithBlank - 1.0f;
				float fDownScaleBlank = ( fTotalBlank - fNeedRemoveBlank ) / fTotalBlank;				
				Coord.fX = Coord.fX * fDownScaleBlank;
				InvenCoord.fX = 1.0f - ((1.0f - InvenCoord.fX - InvenCoord.fWidth ) * fDownScaleBlank) - InvenCoord.fWidth;
				Coord = ScreenCoordToDlgCoord( Coord );
				InvenCoord = pInvenDlg->ScreenCoordToDlgCoord( InvenCoord );				
				Coord.fX += m_SaveDlgCoord.fX;
				Coord.fY += m_SaveDlgCoord.fY;
				InvenCoord.fX += m_SaveInvenDlgCoord.fX;
				InvenCoord.fY += m_SaveInvenDlgCoord.fY;
				
				MoveDialog( Coord.fX - m_SaveDlgCoord.fX , Coord.fY - m_SaveDlgCoord.fY );
				pInvenDlg->MoveDialog( InvenCoord.fX - m_SaveInvenDlgCoord.fX, InvenCoord.fY - m_SaveInvenDlgCoord.fY );

				m_bRestoreCoordNeeded = true;
			}
			else {
				Coord.fX = 0.f;
				InvenCoord.fX = 1.0f - InvenCoord.fWidth;
				Coord = ScreenCoordToDlgCoord( Coord );
				InvenCoord = pInvenDlg->ScreenCoordToDlgCoord( InvenCoord );
				Coord.fX += m_SaveDlgCoord.fX;
				Coord.fY += m_SaveDlgCoord.fY;
				InvenCoord.fX += m_SaveInvenDlgCoord.fX;
				InvenCoord.fY += m_SaveInvenDlgCoord.fY;
				
				MoveDialog( Coord.fX - m_SaveDlgCoord.fX , Coord.fY - m_SaveDlgCoord.fY );
				pInvenDlg->MoveDialog( InvenCoord.fX - m_SaveInvenDlgCoord.fX, InvenCoord.fY - m_SaveInvenDlgCoord.fY );

				m_fMagicScale = 1.0f / fTotalWidth;
				CEtUIDialog::SetForceDialogScale( CEtUIDialog::GetDialogScale() * m_fMagicScale );
				m_bRestoreCoordNeeded = true;
			}
		}

		// 맵이름에 맞는 거래소 이름을 세팅한다.	
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
		if ( pSox ) {
			int nCurMap = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
			std::string szToolName = pSox->GetFieldFromLablePtr( nCurMap, "_ToolName1" )->GetString();
			std::wstring wszToolName;
			ToWideString(szToolName, wszToolName);		
			int nStringTableID = pSox->GetFieldFromLablePtr( nCurMap, "_MapNameID" )->GetInteger();
			std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringTableID );
			CONTROL( Static, ID_STATIC_TITLE )->SetText( FormatW( CONTROL( Static, ID_STATIC_TITLE )->GetPropertyString(), wszName.c_str() ) );
		}

		//	OBT TEMP by kalliste
		m_pPremiumText->Show(false);
		m_pPremiumBackGround->Show(false);

#if defined(PRE_ADD_PETALTRADE)
		GetTradeTask().GetTradeMarket().RequestMarketPetalBalance();
#endif	//#if defined(PRE_ADD_PETALTRADE)
	}
	else {
		if( m_bRestoreCoordNeeded ) {
			SUICoord Coord, InvenCoord;
			GetDlgCoord( Coord );			
			pInvenDlg->GetDlgCoord( InvenCoord );
			MoveDialog( m_SaveDlgCoord.fX - Coord.fX, m_SaveDlgCoord.fY - Coord.fY );
			pInvenDlg->MoveDialog( m_SaveInvenDlgCoord.fX - InvenCoord.fX, m_SaveInvenDlgCoord.fY - InvenCoord.fY );
			CEtUIDialog::SetForceDialogScale( CEtUIDialog::GetDialogScale() / m_fMagicScale );
			m_bRestoreCoordNeeded = false;		
			m_fMagicScale = 1.0f;
		}
		m_pMarketBuyDlg->Show( false );
		m_pMarketSellDlg->Show( false );
		m_pMarketAccountDlg->Show( false );
		pInvenDlg->ShowRadioButton( true );

		GetInterface().CloseBlind();
	}
}

void CDnMarketTabDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );
}

CDnMarketPriceDlg* CDnMarketTabDlg::GetPriceDialog()
{ 
	return m_pMarketSellDlg->GetPriceDialog(); 
}

CDnMarketBuyDlg* CDnMarketTabDlg::GetBuyDialog()
{
	return m_pMarketBuyDlg;
}

void CDnMarketTabDlg::UpdateBuyList( std::vector< TMarketInfo > &vecBuyList, int nTotalCount )
{
	m_pMarketBuyDlg->OnRecvUpdateList( vecBuyList, nTotalCount );
}

void CDnMarketTabDlg::UpdateSellList( std::vector< TMyMarketInfo > &vecSellList)
{
	m_pMarketSellDlg->OnRecvUpdateList( vecSellList );
}

void CDnMarketTabDlg::UpdateCalculationList( std::vector< TMarketCalculationInfo > &vecCalculationList )
{
	m_pMarketAccountDlg->OnRecvUpdateList( vecCalculationList );
}

void CDnMarketTabDlg::UpdatePetalBalance( int nPetalBalance )
{
	m_MarketPosPetal->SetPetalBalance( nPetalBalance );
}

void CDnMarketTabDlg::UpdateMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice )
{
	if( m_pMarketSellDlg->IsShow() )
	{
		m_pMarketSellDlg->OnRecvMarketPrice( nMarketDBID, vecPrice );
	}
	else if( m_pMarketBuyDlg->IsShow() )
	{
		m_pMarketBuyDlg->OnRecvMarketPrice( nMarketDBID, vecPrice );
	}
}

void CDnMarketTabDlg::SetSellCount( short wSellingCount, short wClosingCount, short wWeeklyRegisterCount, short wRemainRegisterItemCount, bool bPremium )
{
	m_pMarketSellDlg->SetCount( wSellingCount, wClosingCount, wWeeklyRegisterCount, wRemainRegisterItemCount, bPremium );
}

void CDnMarketTabDlg::RemoveBuyItem( int nMarketDBID )
{
	m_pMarketBuyDlg->OnRecvRemoveItem( nMarketDBID );
}

void CDnMarketTabDlg::RemoveSellItem( int nMarketDBID, int nRegisterItemCount )
{
	m_pMarketSellDlg->OnRecvRemoveItem( nMarketDBID, nRegisterItemCount );
}

void CDnMarketTabDlg::RemoveCalculationItem( int nMarketDBID )
{
	m_pMarketAccountDlg->OnRecvRemoveItem( nMarketDBID );
}

void CDnMarketTabDlg::RemoveAllCalculationItem()
{
	m_pMarketAccountDlg->RemoveAllItem();
}

void CDnMarketTabDlg::EnableChildDlg( bool bEnable )
{
	m_pMarketSellDlg->EnableAllControl( bEnable );
	m_pMarketSellDlg->EnableListBoxControl( bEnable );
}

void CDnMarketTabDlg::SetMoneyToolTipValue( int nMoney, bool bPetal )
{
	ShowChildDialog( m_MarketToolTipDlg, true);

	if( false == bPetal )
		m_MarketToolTipDlg->SetMoney( nMoney );
	else
		m_MarketToolTipDlg->SetPetal( nMoney );
}

void CDnMarketTabDlg::ShowPremium( bool bShow )
{ 
	if( m_pPremiumBackGround && m_pPremiumText ) 
	{
		m_pPremiumBackGround->Show(bShow); 
		m_pPremiumText->Show(bShow); 
	}
}

void CDnMarketTabDlg::ShowPosPetal(bool bShow)
{
#if defined(PRE_ADD_PETALTRADE)
	ShowChildDialog( m_MarketPosPetal, true);
#endif	//#if defined(PRE_ADD_PETALTRADE)
}

void CDnMarketTabDlg::EnableMarketAccountControl( const char *szControlName, bool bEnable )
{
	CEtUIControl *pControl = m_pMarketAccountDlg->GetControl( szControlName );
	if( pControl ) pControl->Enable( bEnable );
}

int CDnMarketTabDlg::GetPetalBalance()
{ 
	return m_MarketPosPetal->GetPetalBalance(); 
}