#include "StdAfx.h"
#include "DnProgressMBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnProgressMBox::CDnProgressMBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pButtonDefault	= NULL;
	m_pButtonOther		= NULL;
	m_pStaticMessage	= NULL;
	m_pStaticTitle		= NULL;

	m_TotalTime		= 0.f;
	m_RemainTime	= 0.f;
	m_bManual		= false;
	m_pProgressBar		= NULL;
}

CDnProgressMBox::~CDnProgressMBox(void)
{
}

void CDnProgressMBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ProgressMBox.ui" ).c_str(), bShow );
}

void CDnProgressMBox::InitialUpdate()
{
	m_pButtonOther		= GetControl<CEtUIButton>("ID_OK");
	m_pButtonDefault	= GetControl<CEtUIButton>("ID_CANCEL");
	m_pStaticMessage	= GetControl<CEtUIStatic>("ID_MESSAGE");
	m_pStaticTitle		= GetControl<CEtUIStatic>("ID_TITLE");
	m_pProgressBar		= GetControl<CEtUIProgressBar>("ID_PROGRESSBAR0");
}

void CDnProgressMBox::SetMessageBox(LPCWSTR pwszMessage, float totalTime, float remainTime, bool bManual, int nID, CEtUICallback *pCall)
{
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_pStaticMessage->SetText( pwszMessage );

	m_TotalTime		= totalTime;
	m_RemainTime	= remainTime;
	m_bManual		= bManual;
}

void CDnProgressMBox::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( bShow )
	{
		m_SmartMove.MoveCursor();
		m_pProgressBar->SetProgress(100.f);
	}
	else
	{
		m_SmartMove.ReturnCursor();

		SetDialogID(-1);
		SetCallback(NULL);
	}

	CEtUIDialog::Show( bShow );
}

void CDnProgressMBox::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	if( !IsShow() )
		return;

	if( m_RemainTime <= 0.0f )
	{
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonDefault, 0 );
		Show(false);
	}
	else
	{
		if (m_bManual == false)
			m_RemainTime -= fElapsedTime;

		if( m_TotalTime != 0.0f)
		{
			m_pProgressBar->SetProgress( (m_RemainTime / m_TotalTime) * 100.0f );
		}
		else
		{
			m_pProgressBar->SetProgress( 0.0f );
		}
	}

}

void CDnProgressMBox::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		Show(false);
	}
}

bool CDnProgressMBox::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnProgressMBox::ShowControl( bool bShow )
{
	m_pButtonDefault->Show(bShow);
	m_pButtonOther->Show(bShow);
	m_pStaticMessage->Show(bShow);
	m_pStaticTitle->Show(bShow);
	m_pProgressBar->Show(bShow);
}

void CDnProgressMBox::SetTitle( LPCWSTR wszTitle, DWORD dwColor )
{
	m_pStaticTitle->SetText( wszTitle );
	m_pStaticTitle->SetTextColor( dwColor );
}