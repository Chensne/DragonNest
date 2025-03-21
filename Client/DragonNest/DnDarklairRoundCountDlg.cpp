#include "StdAfx.h"
#include "DnDarklairRoundCountDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDarklairRoundCountDlg::CDnDarklairRoundCountDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: m_pStaticRound( NULL )
, m_pStaticBossRound( NULL )
, m_pStaticFinalRound( NULL )
, m_fElapsedTime( 0.0f )
, m_pStaticBonusRound( NULL )
{
	memset( m_pStaticRoundCount, 0, sizeof(m_pStaticRoundCount) );
}

CDnDarklairRoundCountDlg::~CDnDarklairRoundCountDlg()
{
}

void CDnDarklairRoundCountDlg::InitialUpdate()
{
	m_pStaticRound = GetControl<CEtUIStatic>( "ID_STATIC_ROUND" );
	m_pStaticBossRound = GetControl<CEtUIStatic>( "ID_STATIC_BOSS" );
	m_pStaticFinalRound = GetControl<CEtUIStatic>( "ID_STATIC_FBOSS" );
	m_pStaticRound->Show( false );
	m_pStaticBossRound->Show( false );
	m_pStaticFinalRound->Show( false );

	m_pStaticBonusRound = GetControl<CEtUIStatic>( "ID_STATIC_BONUS" );
	m_pStaticBonusRound->Show( false );

	char szStr[32] = {0,};
	for( int i=0; i<E_MAX_POSITION; i++ ) {
		for( int j=0; j<10; j++ ) {
			sprintf_s( szStr, "ID_COUNT%d_%d", i, j );
			m_pStaticRoundCount[i][j] = GetControl<CEtUIStatic>( szStr );
			m_pStaticRoundCount[i][j]->Show( false );
		}
	}
}

void CDnDarklairRoundCountDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DarkRareRoundDlg.ui" ).c_str(), bShow );
}

void CDnDarklairRoundCountDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnDarklairRoundCountDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( m_bShow == false ) {
		m_pStaticRound->Show( false );
		m_pStaticBossRound->Show( false );
		m_pStaticFinalRound->Show( false );
		for( int i=0; i<E_MAX_POSITION; i++ ) {
			for( int j=0; j<10; j++ ) {
				m_pStaticRoundCount[i][j]->Show( false );
			}
		}
	}

	CEtUIDialog::Show( bShow );
}

void CDnDarklairRoundCountDlg::Process( float fElapsedTime )
{
	if( IsShow() ) {
		if( m_fElapsedTime > 0.f ) {
			m_fElapsedTime -= fElapsedTime;
			if( m_fElapsedTime <= 0.f ) {
				m_fElapsedTime = 0.f;
				Show( false );
			}
		}
	}
	CEtUIDialog::Process( fElapsedTime );
}

void CDnDarklairRoundCountDlg::Show( int nRound, bool bBoss, bool bFinal )
{
	Show( true );
	m_fElapsedTime = 3.f;
	if( bFinal ) {
		m_pStaticFinalRound->Show( true );
	}
	else if( bBoss ) {
		m_pStaticBossRound->Show( true );
	}
	else {
		m_pStaticRound->Show( true );

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
		if( nRound >= 1000 )
		{
			int n1000 = nRound / 1000;
			int n100 = (nRound % 1000) / 100;
			int n10 = (nRound % 100) / 10;

			m_pStaticRoundCount[0][n1000]->Show( true );
			m_pStaticRoundCount[1][n100]->Show( true );
			m_pStaticRoundCount[2][n10]->Show( true );
			m_pStaticRoundCount[3][nRound%10]->Show( true );
		}
		else if( nRound >= 100 )
		{
			int n100 = nRound / 100;
			int n10 = (nRound % 100) / 10;

			m_pStaticRoundCount[0][n100]->Show( true );
			m_pStaticRoundCount[1][n10]->Show( true );
			m_pStaticRoundCount[2][nRound%10]->Show( true );
		}
		else if( nRound >= 10 )
		{
			m_pStaticRoundCount[0][nRound/10]->Show( true );
			m_pStaticRoundCount[1][nRound%10]->Show( true );
		}
		else
			m_pStaticRoundCount[1][nRound%10]->Show( true );
#else
		if( nRound >= 10 ) m_pStaticRoundCount[0][nRound/10]->Show( true );
		m_pStaticRoundCount[1][nRound%10]->Show( true );
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	}
}