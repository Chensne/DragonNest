#include "stdafx.h"
#include "DnOccupationModeNoticeDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnOccupationModeNoticeDlg::CDnOccupationModeNoticeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticBlueNotice( NULL )
, m_pStaticRedNotice( NULL )
, m_bNotice( false )
, m_fNoticeTime( 0.f )
{

}

CDnOccupationModeNoticeDlg::~CDnOccupationModeNoticeDlg(void)
{

}

void CDnOccupationModeNoticeDlg::InitialUpdate()
{
	m_pStaticBlueNotice = GetControl<CEtUIStatic>("ID_TEXT_BLUENOTICE");
	m_pStaticRedNotice = GetControl<CEtUIStatic>("ID_TEXT_REDNOTICE");

	ShowNotice( eNOTICE_BLIND );
}

void CDnOccupationModeNoticeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpFlagNoticeDlg.ui" ).c_str(), bShow );
}

void CDnOccupationModeNoticeDlg::Process( float fElapsedTime )
{
	if( m_bNotice )
	{
		m_fNoticeTime -= fElapsedTime;

		if( m_fNoticeTime < 0.f )
		{
			m_bNotice = false;
			ShowNotice( eNOTICE_BLIND );
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnOccupationModeNoticeDlg::ShowNotice( int eTeam, int nStringID /*= 0*/ )
{
	if( eNOTICE_BLIND == eTeam )
	{
		m_pStaticBlueNotice->Show( false );
		m_pStaticRedNotice->Show( false );
		Show( false );

		return;
	}

	m_bNotice = true;
	m_fNoticeTime = eNOTICE_TIME;
	Show( true );

	if( PvPCommon::Team::A == eTeam )
	{
		m_pStaticBlueNotice->Show( true );
		m_pStaticRedNotice->Show( false );

		m_pStaticBlueNotice->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID ) );
	}
	else if( PvPCommon::Team::B == eTeam )
	{
		m_pStaticBlueNotice->Show( false );
		m_pStaticRedNotice->Show( true );

		m_pStaticRedNotice->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID ) );
	}
}
