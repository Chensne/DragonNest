#include "StdAfx.h"
#include "DnCharCreateSetupBackDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#ifdef PRE_MOD_SELECT_CHAR
#include "DnInterface.h"
#endif // PRE_MOD_SELECT_CHAR


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharCreateSetupBackDlg::CDnCharCreateSetupBackDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
, m_pButtonBack(NULL)
{
}

CDnCharCreateSetupBackDlg::~CDnCharCreateSetupBackDlg(void)
{
}

void CDnCharCreateSetupBackDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CharCreateBackDlg.ui").c_str(), bShow);
}

void CDnCharCreateSetupBackDlg::InitialUpdate()
{
	m_pButtonBack = GetControl<CEtUIButton>("ID_BACK");//ID_BUTTON_BACK
}

void CDnCharCreateSetupBackDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		m_pButtonBack->Enable(true);

		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
		if (pTask && pTask->IsEmptySelectActorSlot())
		{
			// 캐릭터 없어도 비활성하지 않는 것으로 리뉴얼
			//m_pButtonBack->Enable( false );
		}
	}

	CEtUIDialog::Show(bShow);
}

void CDnCharCreateSetupBackDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
#ifdef PRE_MOD_SELECT_CHAR
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
		if (!pTask) return;

		//rlkt_dark
		if (pTask->GetState() == CDnLoginTask::CharCreate_SelectClassDark)
		{
			GetInterface().FadeDialog(0x00000000, 0xFF000000, 1.0f, this);
			return;
		}

		if (pTask->IsSelectClass())
		{
			pTask->DeSelectCharacter();
			GetInterface().FadeDialog(0x00000000, 0xFF000000, 1.0f, this);
		}
		else
		{
			GetInterface().FadeDialog(0x00000000, 0xFF000000, 1.0f, this);
		}
#else // PRE_MOD_SELECT_CHAR
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
		if (!pTask) return;

		pTask->ChangeState(CDnLoginTask::CharSelect);
#endif // PRE_MOD_SELECT_CHAR
		return;
	}
}

#ifdef PRE_MOD_SELECT_CHAR

void CDnCharCreateSetupBackDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	if (nID == FADE_DIALOG &&  nCommand == EVENT_FADE_COMPLETE)
	{
		GetInterface().FadeDialog(0xFF000000, 0x00000000, 1.0f, NULL);
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
		if (!pTask) return;

		pTask->ChangeState(CDnLoginTask::CharCreate_SelectClass);
	}
}

#endif // PRE_MOD_SELECT_CHAR