#include "StdAfx.h"
#include "DnCaptionDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionDlg::CDnCaptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pCaption(NULL)
	, m_fElapsedTime(-1.0f)
	, m_fConst(1.0f)
	, m_fShowTime(0.0f)
	, m_fShowRatio(1.0f)
{
}

CDnCaptionDlg::~CDnCaptionDlg(void)
{
}

void CDnCaptionDlg::InitialUpdate()
{
	m_pCaption = GetControl<CEtUIStatic>("ID_CAPTION");
}

void CDnCaptionDlg::SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime )
{
	m_pCaption->SetText( wszMsg );
	m_pCaption->SetTextColor( dwColor );
	m_pCaption->SetShadowColor( 0xff000000 );
	m_pCaption->Show(true);

	m_TextColor = dwColor;
	m_fShowTime = fFadeTime * m_fShowRatio;
	m_fElapsedTime = fFadeTime - m_fShowTime;
	m_fConst = m_fElapsedTime;
	m_bShow = true;
}

void CDnCaptionDlg::CloseCaption()
{
	if( !m_pCaption->IsShow() ) return;
	m_fElapsedTime = 0.f;
	m_pCaption->Show( false );
}

void CDnCaptionDlg::Process( float fElapsedTime )
{
	//CEtUIDialog::Process( fElapsedTime );

	if( m_fElapsedTime <= 0.0f )
	{
		m_pCaption->Show(false);
		return;
	}

	if( m_fShowTime > 0.0f )
	{
		m_fShowTime -= fElapsedTime;
		return;
	}

	EtColor CurColor;

	EtColorLerp( &CurColor, &m_TextColor, &EtColor((DWORD)0x00000000), 1.0f-(1/powf(m_fConst,2)*powf(m_fElapsedTime,2) ) );
	m_pCaption->SetTextColor( CurColor );

	EtColorLerp( &CurColor, &EtColor((DWORD)0xff000000), &EtColor((DWORD)0x00000000), 1.0f-(1/powf(m_fConst,2)*powf(m_fElapsedTime,2) ) );
	m_pCaption->SetShadowColor( CurColor );

	m_fElapsedTime -= fElapsedTime;
}

void CDnCaptionDlg::Render( float fElapsedTime )
{
	if( m_fElapsedTime <= 0.0f )
		return;

	CEtUIDialog::Render( fElapsedTime );
}