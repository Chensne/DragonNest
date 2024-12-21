#include "StdAfx.h"
#include "DnAcceptDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnAcceptDlg::CDnAcceptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pStaticText(NULL)
	, m_pProgressBarTime(NULL)
	, m_pButtonCancel(NULL)
	, m_fProgressBarTime(0.0f)
	, m_fElapsedTime(0.0f)
{
}

CDnAcceptDlg::~CDnAcceptDlg(void)
{
}

void CDnAcceptDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AcceptDlg.ui" ).c_str(), bShow );
}

void CDnAcceptDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT");
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");
}

void CDnAcceptDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( m_fElapsedTime <= 0.0f )
	{
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonCancel, 0 );
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

void CDnAcceptDlg::Show( bool bShow )
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

void CDnAcceptDlg::SetInfo( LPCWSTR pwszMessage, float fTotalTime, float fElapsedTime, int nID, CEtUICallback *pCall )
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_fProgressBarTime = fTotalTime;
	m_fElapsedTime = fElapsedTime;

	m_pStaticText->SetText( pwszMessage );
}