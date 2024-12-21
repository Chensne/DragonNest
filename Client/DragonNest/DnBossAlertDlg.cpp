#include "StdAfx.h"
#include "DnBossAlertDlg.h"

CDnBossAlertDlg::CDnBossAlertDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
	m_pStaticName = NULL;
	m_pTexture = NULL;
	m_pStaticBar = NULL;
	m_pStaticBoard = NULL;

	m_fMaxDelta = m_fDelta = 0.f;
}

CDnBossAlertDlg::~CDnBossAlertDlg()
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CDnBossAlertDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NextBossDlg.ui" ).c_str(), bShow );
}

void CDnBossAlertDlg::InitialUpdate()
{
	m_pStaticName = GetControl<CEtUIStatic>("ID_TEXT_NEXT");
	m_pStaticBar = GetControl<CEtUIStatic>("ID_STATIC_BAR");
	m_pStaticBoard = GetControl<CEtUIStatic>("ID_STATIC_BOARD");
	m_pTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_AREA");

	m_pStaticName->GetUICoord( m_CoordName );
	m_pStaticBar->GetUICoord( m_CoordBar );
	m_pTexture->GetUICoord( m_CoordTexture );

	GetDlgCoord( m_CoordDlg );
}

void CDnBossAlertDlg::Process( float fElapsedTime )
{
	if( IsShow() ) {
		const float fShowDelta = 0.2f;
		if( m_fDelta > 0.f ) {
			m_fDelta -= fElapsedTime;

			float fWeight = 0.f;
			if( ( m_fMaxDelta - m_fDelta ) < fShowDelta )
				fWeight = 1.f - GetWeightValue( 0.f, fShowDelta, ( m_fMaxDelta - m_fDelta ) );
			else if( m_fDelta < fShowDelta )
				fWeight = GetWeightValue( m_fMaxDelta - fShowDelta, m_fMaxDelta, ( m_fMaxDelta - m_fDelta ) );

			SUICoord DlgCoord, Coord;
			float fWidthEnd = ( m_CoordDlg.fWidth - m_CoordDlg.fX );

			Coord = m_CoordDlg;
			Coord.fX = m_CoordDlg.fX + ( ( fWidthEnd - m_CoordDlg.fX ) * fWeight );
			SetDlgCoord( Coord );
		}
		else {
			m_fDelta = 0.f;
			Show( false );
		}
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnBossAlertDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( bShow ) {
		m_fMaxDelta = m_fDelta = 3.f;
	}
	else {
		SAFE_RELEASE_SPTR( m_hTexture );
		m_fDelta = 0.f;
	}
	CEtUIDialog::Show( bShow );
}

void CDnBossAlertDlg::SetBoss( WCHAR *wszName, const char *szImageFileName )
{
	m_pStaticName->SetText( wszName );

	m_hTexture = EternityEngine::LoadTexture( szImageFileName );
	if( m_hTexture ) {
		int nWidth = m_hTexture->Width();
		int nHeight = m_hTexture->Height();

		m_pTexture->SetTexture( m_hTexture, 0, 0, nWidth, nHeight );
	}
}

float CDnBossAlertDlg::GetWeightValue( float fStartDelta, float fEndDelta, float fCurDelta )
{
	if( fCurDelta <= fStartDelta ) return 0.f;
	if( fCurDelta >= fEndDelta ) return 1.f;

	float fWeight = ( 1.f / ( fEndDelta - fStartDelta ) ) * ( fCurDelta - fStartDelta );
	if( fWeight < 0.5f ) {
		fWeight += fWeight;
		fWeight = abs( cos( EtToRadian( 90.f * fWeight ) ) - 1.f ) * 0.5f;
	}
	else {
		fWeight -= 0.5f;
		fWeight += fWeight;

		fWeight = 0.5f + ( abs( cos( EtToRadian( 90.f + ( 90.f * fWeight ) ) ) ) * 0.5f );
	}
	return fWeight;
}