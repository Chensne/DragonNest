#include "StdAfx.h"
#include "DnStageClearMoveDlg.h"
#include "GameSendPacket.h"
#include "DnInterface.h"
#include "DnGameTask.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStageClearMoveDlg::CDnStageClearMoveDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnStageClearMoveDlg::~CDnStageClearMoveDlg(void)
{
}

void CDnStageClearMoveDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "DungeonClearNewMoveDlg.ui" ).c_str(), bShow );
}

void CDnStageClearMoveDlg::Show( bool bShow )
{
	if (m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}

void CDnStageClearMoveDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg /* = 0 */ )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_STAGECLEAR_GO_TOWN:
		{
			if (nCommand == EVENT_BUTTON_CLICKED) 
			{
				if (strcmp( pControl->GetControlName(), "ID_YES" ) == 0) 
				{
					GetInterface().TerminateStageClearWarpStandBy();
					SendWarpDungeon(true);
					Show(false);
				}
			}
		}
		break;

	case MESSAGEBOX_STAGECLEAR_GO_STAGE:
		if (nCommand == EVENT_BUTTON_CLICKED) 
		{
			if (strcmp( pControl->GetControlName(), "ID_YES" ) == 0) 
			{
				GetInterface().TerminateStageClearWarpStandBy();
				SendWarpDungeon(false);

				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( pGameTask ) {
					pGameTask->SetLookStageEntrance();
				}

				Show(false);
			}
		}
		break;
	}
}



void CDnStageClearMoveDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BTN_MOVE" ) )
		{
			GetInterface().ShowDungeonMoveDlg(true, false);
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}