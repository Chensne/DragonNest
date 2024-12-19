#include "stdafx.h"
#include "DnMarketAboutTaxDlg.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketAboutDlg::CDnMarketAboutDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnMarketAboutDlg::~CDnMarketAboutDlg()
{
}

void CDnMarketAboutDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketAboutTax_wPetal.ui" ).c_str(), bShow );
}

void CDnMarketAboutDlg::InitialUpdate()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTAX );

	float nGoldUnder1 = pSox->GetFieldFromLablePtr( GOLD_TAX_UNDER1, "_Rate" )->GetFloat() * 100.f;
	float nGoldOver1 = pSox->GetFieldFromLablePtr( GOLD_TAX_OVER1, "_Rate" )->GetFloat() * 100.f;
	float nPetalTax = pSox->GetFieldFromLablePtr( PETAL_TAX_ID, "_Rate" )->GetFloat() * 100.f;

	CEtUIStatic * pStaticUnder1 = GetControl<CEtUIStatic>("ID_GOLD_UNDER1");
	CEtUIStatic * pStaticOver1 = GetControl<CEtUIStatic>("ID_GOLD_OVER1");
	CEtUIStatic * pStaticPetal = GetControl<CEtUIStatic>("ID_PETAL");

	// 4156
	WCHAR strMessage[256];

	swprintf_s( strMessage, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4156 ), nGoldUnder1 );
	pStaticUnder1->SetText( strMessage );

	swprintf_s( strMessage, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4156 ), nGoldOver1 );
	pStaticOver1->SetText( strMessage );

	swprintf_s( strMessage, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4156 ), nPetalTax );
	pStaticPetal->SetText( strMessage );
}

void CDnMarketAboutDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
			Show( false );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}