#include "StdAfx.h"
#include "DnCaptionExDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionExDlg::CDnCaptionExDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_fElapsedTime(-1.0f)
, m_fConst(1.0f)
, m_fShowTime(0.0f)
, m_fShowRatio(1.0f)
{
}

CDnCaptionExDlg::~CDnCaptionExDlg(void)
{
	m_vecCaptions.clear();
}

void CDnCaptionExDlg::InitialUpdate()
{
	for( int i = 0; i < ( int )m_vecControl.size(); i++ )
		m_vecCaptions.push_back( GetControl<CEtUIStatic>( m_vecControl[ i ]->GetControlName() ) );		
}

void CDnCaptionExDlg::SetCaption( std::vector< const wchar_t * > & vecStrs, DWORD dwColor, float fFadeTime )
{
	CEtUIStatic * pCaption = NULL;
	int size = (int)m_vecCaptions.size();	
	int idx = size - 1;
	int strSize = (int)vecStrs.size();
	for( int i=0; i<size; ++i, --idx )
	{
		pCaption = m_vecCaptions[ idx ];
		if( i < strSize )
		{
			pCaption->SetText( vecStrs[i] );			
			pCaption->SetTextColor( dwColor );
			pCaption->SetShadowColor( 0xff000000 );
			pCaption->Show( true );
		}
		else
		{
			m_vecCaptions[ i ]->Show( false );
		}
	}
	
	m_TextColor = dwColor;
	m_fShowTime = fFadeTime * m_fShowRatio;
	m_fElapsedTime = fFadeTime - m_fShowTime;
	m_fConst = m_fElapsedTime;
	m_bShow = true;
}

void CDnCaptionExDlg::CloseCaption()
{	
	m_fElapsedTime = 0.f;

	int size = (int)m_vecCaptions.size();
	for( int i=0; i<size; ++i )
		m_vecCaptions[ i ]->Show( false );		
}

void CDnCaptionExDlg::Process( float fElapsedTime )
{
	//CEtUIDialog::Process( fElapsedTime );

	if( m_fElapsedTime <= 0.0f )
	{
		int size = (int)m_vecCaptions.size();
		for( int i=0; i<size; ++i )
			m_vecCaptions[ i ]->Show( false );
		return;
	}

	if( m_fShowTime > 0.0f )
	{
		m_fShowTime -= fElapsedTime;
		return;
	}

	EtColor CurColor, CurShadowColor;
	EtColorLerp( &CurColor, &m_TextColor, &EtColor((DWORD)0x00000000), 1.0f-(1/powf(m_fConst,2)*powf(m_fElapsedTime,2) ) );
	EtColorLerp( &CurShadowColor, &EtColor((DWORD)0xff000000), &EtColor((DWORD)0x00000000), 1.0f-(1/powf(m_fConst,2)*powf(m_fElapsedTime,2) ) );
	
	int size = (int)m_vecCaptions.size();
	for( int i=0; i<size; ++i )
	{
		m_vecCaptions[ i ]->SetTextColor( CurColor );
		m_vecCaptions[ i ]->SetShadowColor( CurShadowColor );
	}

	m_fElapsedTime -= fElapsedTime;
}

void CDnCaptionExDlg::Render( float fElapsedTime )
{
	if( m_fElapsedTime <= 0.0f )
		return;

	CEtUIDialog::Render( fElapsedTime );
}