#include "StdAfx.h"
#include "DnCostumeMixProgressDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCostumeMixProgressDlg::CDnCostumeMixProgressDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pProgressBar( NULL )
, m_TotalTime( 0.0f )
, m_RemainTime( 0.0f )
{
}

CDnCostumeMixProgressDlg::~CDnCostumeMixProgressDlg(void)
{
}

void CDnCostumeMixProgressDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "Com_Confirm.ui" ).c_str(), bShow );
}

void CDnCostumeMixProgressDlg::InitialUpdate()
{
	m_pProgressBar		= GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
}

void CDnCostumeMixProgressDlg::SetMessageBox(float totalTime, float remainTime, int nID, CEtUICallback *pCall)
{
	m_nDialogID = nID;
	m_pCallback = pCall;

	m_TotalTime		= totalTime;
	m_RemainTime	= remainTime;
}

void CDnCostumeMixProgressDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( bShow )
	{
		m_pProgressBar->SetProgress(0.f);
	}
	else
	{
		SetDialogID(-1);
		SetCallback(NULL);
	}

	CEtUIDialog::Show( bShow );
}

void CDnCostumeMixProgressDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	if( !IsShow() )
		return;

	if( m_RemainTime <= 0.0f )
	{
		CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pProgressBar, 0 );
	}
	else
	{
		m_RemainTime -= fElapsedTime;

		if( m_TotalTime != 0.0f)
		{
			m_pProgressBar->SetProgress((1.f - (m_RemainTime / m_TotalTime)) * 100.f);
		}
		else
		{
			m_pProgressBar->SetProgress(100.f);
		}
	}

}

bool CDnCostumeMixProgressDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnCostumeMixProgressDlg::ShowControl( bool bShow )
{
	m_pProgressBar->Show(bShow);
}