#include "StdAfx.h"
#include "DnRespawnGauageDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRespawnGauageDlg::CDnRespawnGauageDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticText(NULL)
, m_pProgressBarTime(NULL)
, m_fProgressBarTime(0.0f)
, m_fElapsedTime(0.0f)
{
}

CDnRespawnGauageDlg::~CDnRespawnGauageDlg(void)
{
}

void CDnRespawnGauageDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpRespawnDlg.ui" ).c_str(), bShow );
}

void CDnRespawnGauageDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT");
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
}

void CDnRespawnGauageDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( m_fElapsedTime <= 0.0f )
	{
		//CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
		Show(false);
	}
	else
	{
		m_fElapsedTime -= fElapsedTime;

		if( m_fProgressBarTime != 0.0f)
		{
			m_pProgressBarTime->SetProgress( m_fElapsedTime / m_fProgressBarTime * 100.0f );
		}
		else
		{
			m_pProgressBarTime->SetProgress( 0.0f );
		}
	}
}

void CDnRespawnGauageDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( !bShow )
	{
		SetDialogID(-1);
		SetCallback(NULL);
		m_fProgressBarTime = 0.0f;
		m_fElapsedTime = 0.0f;
	}
	else
	{
		m_pProgressBarTime->SetProgress(100.0f);
	}

	CEtUIDialog::Show( bShow );
	//CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnRespawnGauageDlg::SetInfo( LPCWSTR pwszMessage, float fTime, int nID, CEtUICallback *pCall )
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_fProgressBarTime = fTime;
	m_fElapsedTime = m_fProgressBarTime;

	if( m_pStaticText )
		m_pStaticText->SetText( pwszMessage );
}

void CDnRespawnGauageDlg::SetSpaceText( bool bShow )
{
	CONTROL( Static, ID_STATIC2 )->Show( bShow );
}