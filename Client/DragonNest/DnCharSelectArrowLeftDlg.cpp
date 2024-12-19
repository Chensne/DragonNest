#include "StdAfx.h"
#include "DnCharSelectArrowLeftDlg.h"
#include "TaskManager.h"
#include "DnLoginTask.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef PRE_MOD_SELECT_CHAR

CDnCharSelectArrowLeftDlg::CDnCharSelectArrowLeftDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnCharSelectArrowLeftDlg::~CDnCharSelectArrowLeftDlg(void)
{
}

void CDnCharSelectArrowLeftDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ArrowLeftDlg.ui" ).c_str(), bShow );
}

void CDnCharSelectArrowLeftDlg::InitialUpdate()
{
	GetControl<CEtUIButton>( "ID_BUTTON_LEFT" )->SetBlink( true );
}

void CDnCharSelectArrowLeftDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_LEFT" ) )
		{
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pTask )
				pTask->PrevCharListPage();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#endif // PRE_MOD_SELECT_CHAR