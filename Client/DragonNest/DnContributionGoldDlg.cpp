#include "Stdafx.h"

#ifdef PRE_ADD_DONATION


#include "DnItemTask.h"
#include "DnContributionTask.h"

#include "DnInterface.h"

#include "DnMoneyInputDlg.h"
#include "DnContributionGoldDlg.h"


CDnContributionGoldDlg::CDnContributionGoldDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pStaticGold( NULL )
, m_pStaticSilver( NULL )
, m_pStaticCopper( NULL )
, m_pBtnContribute( NULL )
, m_pMoneyControl( NULL )
{
}

void CDnContributionGoldDlg::ReleaseDlg()
{
	SAFE_DELETE( m_pMoneyControl );
}


// Overrid - CEtUIDialog 	
void CDnContributionGoldDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DonationBoxDlg.ui" ).c_str(), bShow );
}


void CDnContributionGoldDlg::InitialUpdate()
{
	m_pStaticGold = GetControl<CEtUIStatic>( "ID_GOLD" );
	m_pStaticSilver = GetControl<CEtUIStatic>( "ID_SILVER" );
	m_pStaticCopper = GetControl<CEtUIStatic>( "ID_BRONZE" );	
	m_pBtnContribute = GetControl< CEtUIButton >("ID_BT_DONATION");

	// MoneyControl.
	m_pMoneyControl = new CDnMoneyControl( this ); 
	m_pMoneyControl->InitialUpdate();
}


//void CDnContributionGoldDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
//{
//}


void CDnContributionGoldDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	// MoneyControl.
	if( !m_pMoneyControl->ProcessCommand( nCommand, false, pControl, uMsg ) )
		return;


	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_CANCEL" ) )
			Show( false );
	
		// 최대.
		if( IsCmdControl( "ID_BUTTON_MAX" ) )
			m_pMoneyControl->SetMax();

		// 초기값.
		if( IsCmdControl( "ID_BUTTON_INIT" ) )
			m_pMoneyControl->Init();

		// 기부하기.
		if( IsCmdControl( "ID_BUTTON_OK" ) )
		{
			// 기부하기.
			MakeDonations();
		}

	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


bool CDnContributionGoldDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( m_pMoneyControl && m_pMoneyControl->MsgProc( hWnd, uMsg, wParam, lParam ) )
		return true;


	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}


void CDnContributionGoldDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	m_pMoneyControl->Show( bShow );
	CEtUIDialog::Show( bShow );
	
	if( bShow )
	{
		SetData();
	}
}


void CDnContributionGoldDlg::SetData()
{
	// 기부하기 버튼 활성.
	m_pBtnContribute->Enable( true );

	m_pMoneyControl->Init();

	INT64 nCoin = GetItemTask().GetCoin();
	if( nCoin > 0 )
	{
		wchar_t buf[64]={0,};
		m_pStaticGold->SetText( std::wstring( _i64tow(nCoin/10000, buf, 10) ) );
		m_pStaticSilver->SetText( std::wstring( _i64tow(nCoin%10000/100, buf, 10) ) );
		m_pStaticCopper->SetText( std::wstring( _i64tow(nCoin%100, buf, 10) ) );
	}
	else
	{
		std::wstring str(L"0");
		m_pStaticGold->SetText( str );
		m_pStaticSilver->SetText( str );
		m_pStaticCopper->SetText( str );
	}	
}


// 기부하기.
void CDnContributionGoldDlg::MakeDonations()
{
	// 기부하기 버튼 비활성.
	m_pBtnContribute->Enable( false );

	INT64 coin = m_pMoneyControl->GetMoney();

	// 골드이하( 실버, 쿠퍼 ) 절삭.
	coin = ( coin / 10000 ) * 10000;

	if( coin > 0 )
	{
		// Task에 Send요청.
		CDnContributionTask::GetInstance().OnSendMakeDonations( coin );

		// 기부창 닫기.
		GetInterface().OpenContributionDialog( false );		
	}
	
}


#endif