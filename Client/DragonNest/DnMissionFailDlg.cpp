#include "StdAfx.h"
#include "DnMissionFailDlg.h"
#include "DnPartyTask.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMissionFailDlg::CDnMissionFailDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_fDisplayTime(0.0f)
{
}

CDnMissionFailDlg::~CDnMissionFailDlg(void)
{
}

void CDnMissionFailDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MissionFailDlg.ui" ).c_str(), bShow );
}

void CDnMissionFailDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_fDisplayTime = 5.0f;
	}
	else
	{
		m_fDisplayTime = 0.0f;
	}

	CEtUIDialog::Show( bShow );
	//CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnMissionFailDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( IsShow() )
	{
		if( m_fDisplayTime > 0.0f )
		{
			m_fDisplayTime -= fElapsedTime;
		}
		else
		{
			if( CDnPartyTask::GetInstance().GetPartyRole() != CDnPartyTask::MEMBER )
			{
				Show( false );
				GetInterface().OpenMissionFailMoveDialog();
			}
		}
	}
}

void CDnMissionFailDlg::OnBlindOpen() 
{
	CDnLocalPlayerActor::LockInput(true);
//	CDnMouseCursor::GetInstance().ShowCursor( true );
}

void CDnMissionFailDlg::OnBlindOpened()
{
	Show( true );
	GetInterface().ShowChatDialog();
}

void CDnMissionFailDlg::OnBlindClose()
{
	Show( false );
}

void CDnMissionFailDlg::OnBlindClosed()
{
	GetInterface().OpenBaseDialog();
	CDnLocalPlayerActor::LockInput(false);
//	CDnMouseCursor::GetInstance().ShowCursor( false );
}