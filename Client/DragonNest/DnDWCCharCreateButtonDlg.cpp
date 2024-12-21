#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDWCCharCreateDlg.h"
#include "DnDWCCharCreateButtonDlg.h"
#include "DnDWCCharCreateSelectDlg.h"
#include "DnInterface.h"
#include "DnLoginTask.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDWCCharCreateButtonDlg::CDnDWCCharCreateButtonDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
	pParentDlg = (CDnDWCCharCreateDlg*)pParentDialog;
}

CDnDWCCharCreateButtonDlg::~CDnDWCCharCreateButtonDlg()
{
}

void CDnDWCCharCreateButtonDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvP_CharCreateBT.ui" ).c_str(), bShow );
}

void CDnDWCCharCreateButtonDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if(IsCmdControl("ID_BT_OK"))
		{
			// ���� ��Ŷ ����
			CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if(pTask)
			{
				std::vector<TCharListData> vCharList = pTask->GetMyCharListData();
				if(!vCharList.empty())
				{
					if(pParentDlg)
					{
						INT64 nCharDBID = pParentDlg->GetCharDBID();
						int   nJobID	= pParentDlg->GetCharJobID();

#if defined(PRE_MOD_SELECT_CHAR)
						SendDWCCreateChar(nCharDBID, nJobID);
#else
						CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
						if(pTask)
						{
							int nActorSlot = pTask->GetSelectExtendActorSlot();
							SendDWCCreateChar(nActorSlot, nJobID);
						}						
#endif // PRE_ADD_DWC
						
					}
				}
			}
		}
			
		else if(IsCmdControl("ID_BT_CANCEL"))
		{
			GetInterface().FadeDialog( 0x00000000, 0xFF000000, 1.0f, this );
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDWCCharCreateButtonDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( nID == FADE_DIALOG && nCommand == EVENT_FADE_COMPLETE )
	{
		GetInterface().FadeDialog( 0xFF000000, 0x00000000, 1.0f, NULL );
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( !pTask ) return;

		pTask->ChangeState( CDnLoginTask::CharSelect );
	}
}

#endif // PRE_ADD_DWC