
#include "StdAfx.h"
#include "DnEscapeDlg.h"
#include "DnLocalPlayerActor.h"

#ifdef PRE_FIX_ESCAPE

float CDnEscapeDlg::s_fProgressGlobalTime;

CDnEscapeDlg::CDnEscapeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	m_pStaticText = NULL;
	m_pProgressBar = NULL;

	m_fProgressBarTime = eEscape::ProgressTime;
	s_fProgressGlobalTime = 0;
}

CDnEscapeDlg::~CDnEscapeDlg(void)
{
}


void CDnEscapeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ProgressDlg.ui" ).c_str(), bShow );
}

void CDnEscapeDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT");
	m_pProgressBar = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
	m_pStaticText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120154 ));
}

void CDnEscapeDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	if( s_fProgressGlobalTime >= 0 )
		s_fProgressGlobalTime -= fElapsedTime;

	if (IsShow() == false)
		return;

	if( m_fProgressBarTime < 0 && s_fProgressGlobalTime < 0)
	{
		Show(false);
		
		if( CDnActor::s_hLocalActor )
		{
			CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			pLocalActor->CmdEscape();
			CDnLocalPlayerActor::LockInput(false);
		}

		s_fProgressGlobalTime = eEscape::GlobalTime;
	}

	m_fProgressBarTime -= fElapsedTime;
	m_pProgressBar->SetProgress((m_fProgressBarTime / eEscape::ProgressTime) * 100.0f);
}

void CDnEscapeDlg::Show(bool bShow)
{
	if(bShow)
	{
		if( s_fProgressGlobalTime > 0 )
		{
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120155 ) , (int)s_fProgressGlobalTime).c_str() , textcolor::YELLOW ,3.0f );
			return;
		}

		GetInterface().CloseAllMainMenuDialog();
		CDnLocalPlayerActor::LockInput(true);
		CDnMouseCursor::GetInstance().ShowCursor(false);

		m_fProgressBarTime = eEscape::ProgressTime;
	}

	CEtUIDialog::Show(bShow);
}

#endif