#include "stdafx.h"
#include "DnMarketPosPetalDlg.h"
#include "DnTradeTask.h"
#ifdef PRE_MOD_PETAL_WRITE
#include "DnInterfaceString.h"
#endif // PRE_MOD_PETAL_WRITE


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketPosPetal::CDnMarketPosPetal( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
, m_pStaticPetal( NULL )
, m_nPetalBalance( 0 )
{
}

CDnMarketPosPetal::~CDnMarketPosPetal()
{
}

void CDnMarketPosPetal::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketPosPetal_wPetal.ui" ).c_str(), bShow );
}

void CDnMarketPosPetal::InitialUpdate()
{
	m_pStaticPetal = GetControl<CEtUIStatic>("ID_STATIC_PosPETAL");
}

void CDnMarketPosPetal::Show( bool bShow )
{
	if( bShow == m_bShow )
		return;
#if defined(PRE_ADD_PETALTRADE)
	if( bShow )
		GetTradeTask().GetTradeMarket().RequestMarketPetalBalance();
#endif	//#if defined(PRE_ADD_PETALTRADE)

	CEtUIDialog::Show( bShow );
}

void CDnMarketPosPetal::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_CANCEL" ) || IsCmdControl( "ID_BUTTON_CLOSE" ) )
			Show( false );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMarketPosPetal::SetPetalBalance( int nPetalBalance )
{
	m_nPetalBalance = nPetalBalance;

#ifdef PRE_MOD_PETAL_WRITE
	m_pStaticPetal->SetText( DN_INTERFACE::UTIL::GetAddCommaString( nPetalBalance ).c_str() );
#else // PRE_MOD_PETAL_WRITE
	m_pStaticPetal->SetIntToText( nPetalBalance );
#endif // PRE_MOD_PETAL_WRITE
}