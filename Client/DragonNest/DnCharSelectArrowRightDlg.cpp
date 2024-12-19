#include "StdAfx.h"
#include "DnCharSelectArrowRightDlg.h"
#include "TaskManager.h"
#include "DnLoginTask.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef PRE_MOD_SELECT_CHAR

CDnCharSelectArrowRightDlg::CDnCharSelectArrowRightDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnCharSelectArrowRightDlg::~CDnCharSelectArrowRightDlg(void)
{
}

void CDnCharSelectArrowRightDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ArrowRightDlg.ui" ).c_str(), bShow );
}

void CDnCharSelectArrowRightDlg::InitialUpdate()
{
	GetControl<CEtUIButton>( "ID_BUTTON_RIGHT" )->SetBlink( true );
}

void CDnCharSelectArrowRightDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_RIGHT" ) )
		{
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pTask )
				pTask->NextCharListPage();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#endif // PRE_MOD_SELECT_CHAR