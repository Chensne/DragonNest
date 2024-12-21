#include "StdAfx.h"
#include "DnFadeCaptionDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_ADD_FADE_TRIGGER

CDnFadeCaptionDlg::CDnFadeCaptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticCaption( NULL )
, m_fDelayTime( 0.0f )
{
}

CDnFadeCaptionDlg::~CDnFadeCaptionDlg(void)
{
}

void CDnFadeCaptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "BlindCaptionDlg.ui" ).c_str(), bShow );
}

void CDnFadeCaptionDlg::InitialUpdate()
{
	m_pStaticCaption = GetControl<CEtUIStatic>("ID_CAPTION");
}

void CDnFadeCaptionDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		m_pStaticCaption->ClearText();
	}

	CEtUIDialog::Show( bShow );
}

void CDnFadeCaptionDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() && m_fDelayTime > 0.0f )
	{
		m_fDelayTime -= fElapsedTime;
		if( m_fDelayTime <= 0.0f )
		{
			m_fDelayTime = 0.0f;
			Show( false );
		}
	}
}

void CDnFadeCaptionDlg::SetCaption( const wchar_t* wszCaption, float fDelayTime )
{
	if( !m_pStaticCaption ) return;
	m_pStaticCaption->SetText( wszCaption );
	m_fDelayTime = fDelayTime;
	Show( true );
}

void CDnFadeCaptionDlg::ClearCaption()
{
	if( !m_pStaticCaption ) return;
	m_pStaticCaption->ClearText();
}

#endif // PRE_ADD_FADE_TRIGGER