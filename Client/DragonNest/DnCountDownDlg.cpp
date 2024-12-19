#include "StdAfx.h"
#include "DnCountDownDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCountDownDlg::CDnCountDownDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pAniCount(NULL)
{
}

CDnCountDownDlg::~CDnCountDownDlg(void)
{
}

void CDnCountDownDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CountDownDlg.ui" ).c_str(), bShow );
}

void CDnCountDownDlg::InitialUpdate()
{
	m_pAniCount = GetControl<CEtUIAnimation>("ID_ANIMATION_COUNT");
}

void CDnCountDownDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pAniCount->Play();
	}
	else
	{
		m_pAniCount->Stop();
	}

	CEtUIDialog::Show( bShow );
	//CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnCountDownDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( IsShow() && !m_pAniCount->IsPlaying() )
	{
		if( m_pCallback )
		{
			m_pCallback->OnUICallbackProc( GetDialogID(), 0, NULL, 0 );
		}
	}
}

bool CDnCountDownDlg::IsCounting()
{
	return m_pAniCount->IsPlaying();
}