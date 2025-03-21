#include "StdAfx.h"
#include "DnGuildWarFinalWinNoticeDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildWarFinalWinNoticeDlg::CDnGuildWarFinalWinNoticeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_fFadeAwayTime( 0.0f )
{
}

CDnGuildWarFinalWinNoticeDlg::~CDnGuildWarFinalWinNoticeDlg()
{
}

void CDnGuildWarFinalWinNoticeDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarWinNoticeDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarFinalWinNoticeDlg::InitialUpdate()
{
	CEtUIStatic* pStaticMsg = GetControl<CEtUIStatic>( "ID_TEXT" );
	if( pStaticMsg )
		pStaticMsg->SetText( L"" );
}

void CDnGuildWarFinalWinNoticeDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		m_fFadeAwayTime = 5.0f;

	CDnCustomDlg::Show( bShow );
}

void CDnGuildWarFinalWinNoticeDlg::SetFianlWinNoticeMsg( const std::wstring &strMsg )
{
	CEtUIStatic* pStaticMsg = GetControl<CEtUIStatic>( "ID_TEXT" );
	if( pStaticMsg )
		pStaticMsg->SetText( strMsg );
}

void CDnGuildWarFinalWinNoticeDlg::Process( float fElapsedTime )
{
	CDnCustomDlg::Process( fElapsedTime );

	if( IsShow() )
	{
		m_fFadeAwayTime -= fElapsedTime;
		if( m_fFadeAwayTime < 0.0f )
			Show( false );
	}
}