#include "stdafx.h"
#include "DnLifeChannelListDlg.h"

CDnLifeChannelListDlg::CDnLifeChannelListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
, m_pStaticName( NULL )
, m_pStaticLevel( NULL )
, m_pStaticClass( NULL )
, m_nVectorID( -1 )
{

}

CDnLifeChannelListDlg::~CDnLifeChannelListDlg()
{

}

void CDnLifeChannelListDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeChannelListDlg.ui" ).c_str(), bShow );
}

void CDnLifeChannelListDlg::SetInfo( const WCHAR *wszName, bool bHarvest, int nTime, int nID )
{
	WCHAR wszTime[25] = {0, };
	m_nVectorID = nID;

	CONTROL( Static, ID_TEXT_NAME )->SetText( wszName );

	if( bHarvest )
	{
		CONTROL( Static, ID_TEXT_LIMIT )->Show( true );
		CONTROL( Static, ID_TEXT_WORKING )->Show( false );
	}
	else
	{
		CONTROL( Static, ID_TEXT_LIMIT )->Show( false );
		CONTROL( Static, ID_TEXT_WORKING )->Show( true );

		int nMin = nTime / 60;
		int nHour = nMin / 60;
		nMin = nMin % 60;

		swprintf_s( wszTime, L"%d:%d", nHour, nMin );
	}

	CONTROL( Static, ID_TEXT_TIME )->SetText( wszTime );
}

void CDnLifeChannelListDlg::SetEmpty()
{
	CONTROL( Static, ID_TEXT_NAME )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7423 ) );
	CONTROL( Static, ID_TEXT_LIMIT )->Show( false );
	CONTROL( Static, ID_TEXT_WORKING )->Show( false );
	CONTROL( Static, ID_TEXT_TIME )->Show( false );
}