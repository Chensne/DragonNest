#include "stdafx.h"
#include "DnPvpGuildPropInfoDlg.h"
#include "DnActor.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnPvpGuildPropInfoDlg::CDnPvpGuildPropInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_fRenderTime( 0.f )
, m_pBlueIcon( NULL )
, m_pRedIcon( NULL )
, m_pStaticText( NULL )
{

}

CDnPvpGuildPropInfoDlg::~CDnPvpGuildPropInfoDlg()
{

}

void CDnPvpGuildPropInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpGuildPropInfoDlg.ui" ).c_str(), bShow );
}

void CDnPvpGuildPropInfoDlg::InitialUpdate()
{
	m_pBlueIcon = GetControl<CEtUIStatic>( "ID_STATIC_BLUEPROP" );
	m_pRedIcon = GetControl<CEtUIStatic>( "ID_STATIC_REDPROP" );
	m_pStaticText = GetControl<CEtUIStatic>( "ID_CAPTION" );
}

void CDnPvpGuildPropInfoDlg::Process( float fElapsedTime )
{
	if( 0.f > m_fRenderTime )
		Show( false );
	else 
		m_fRenderTime -= fElapsedTime;

	CEtUIDialog::Process( fElapsedTime );
}

void CDnPvpGuildPropInfoDlg::ShowInfo( DnActorHandle hActor, int nStringID )
{
	Show( true );
	m_fRenderTime = SHOW_TIME;

	if( PvPCommon::Team::A == hActor->GetTeam() )
	{
		m_pBlueIcon->Show( true );
		m_pRedIcon->Show( false );
	}
	if( PvPCommon::Team::B == hActor->GetTeam() )
	{
		m_pBlueIcon->Show( false );
		m_pRedIcon->Show( true );
	}

	m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID ) );
}
