#include "StdAfx.h"
#include "DnGuildWarPreliminaryStatusDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildWarTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryStatusDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryStatusDlg::CDnGuildWarPreliminaryStatusDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticBlueTeamScore( NULL )
, m_pStaticRedTeamScore( NULL )
, m_pStaticMyScore( NULL )
, m_pStaticMyGuildScore( NULL )
{
	memset( m_pStaticInterimResult, 0, sizeof( m_pStaticInterimResult ) );
}

CDnGuildWarPreliminaryStatusDlg::~CDnGuildWarPreliminaryStatusDlg()
{
}

void CDnGuildWarPreliminaryStatusDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pStaticInterimResult[0] = GetControl<CEtUIStatic>( "ID_TEXT_RESULT0" );
	m_pStaticInterimResult[1] = GetControl<CEtUIStatic>( "ID_TEXT_RESULT1" );
	m_pStaticInterimResult[2] = GetControl<CEtUIStatic>( "ID_TEXT_RESULT2" );
	m_pStaticInterimResult[0]->Show( false );
	m_pStaticInterimResult[1]->Show( false );
	m_pStaticInterimResult[2]->Show( false );
	m_pStaticBlueTeamScore = GetControl<CEtUIStatic>( "ID_TEXT_BLUECOUNT" );
	m_pStaticRedTeamScore = GetControl<CEtUIStatic>( "ID_TEXT_REDCOUNT" );
	m_pStaticMyScore = GetControl<CEtUIStatic>( "ID_TEXT_MYCOUNT" );
	m_pStaticMyGuildScore = GetControl<CEtUIStatic>( "ID_TEXT_GUILDCOUNT" );
}

void CDnGuildWarPreliminaryStatusDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarBoardDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryStatusDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnGuildWarTask::stPreliminaryStatus stPreliminaryStatus = GetGuildWarTask().GetPreliminaryStatus();
		SetPresentCondition( stPreliminaryStatus.nBluePoint, stPreliminaryStatus.nRedPoint, stPreliminaryStatus.nMyPoint, stPreliminaryStatus.nMyGuildPoint );
	}
	else
	{
		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);

		if( hNpc )
			hNpc->SetActionQueue( "Close" );

		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminaryStatusDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
			Show( false );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminaryStatusDlg::SetPresentCondition( int nBlueTeamSocre, int nRedTeamScore, int nMySocre, int nMyGuildScore )
{
	if( nBlueTeamSocre > nRedTeamScore )
	{
		m_pStaticInterimResult[0]->Show( true );
		m_pStaticInterimResult[1]->Show( false );
		m_pStaticInterimResult[2]->Show( false );
	}
	else if( nBlueTeamSocre == nRedTeamScore )
	{
		m_pStaticInterimResult[0]->Show( false );
		m_pStaticInterimResult[1]->Show( false );
		m_pStaticInterimResult[2]->Show( true );
	}
	else
	{
		m_pStaticInterimResult[0]->Show( false );
		m_pStaticInterimResult[1]->Show( true );
		m_pStaticInterimResult[2]->Show( false );
	}

	m_pStaticBlueTeamScore->SetText( FormatW( L"%d", nBlueTeamSocre ) );
	m_pStaticRedTeamScore->SetText( FormatW( L"%d", nRedTeamScore ) );

	m_pStaticMyScore->SetText( FormatW( L"%d", nMySocre ) );
	m_pStaticMyGuildScore->SetText( FormatW( L"%d", nMyGuildScore ) );
}
