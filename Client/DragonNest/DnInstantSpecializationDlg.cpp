#include "StdAfx.h"
#include "DnInstantSpecializationDlg.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "DnLocalPlayerActor.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInstantSpecializationDlg::CDnInstantSpecializationDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback),
	m_pPopupBackground(0),
	m_pPopupText(0),
	m_pBookText(0),
	m_pBookButton(0)
{
}

CDnInstantSpecializationDlg::~CDnInstantSpecializationDlg(void)
{
}

void CDnInstantSpecializationDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("levelguidebtdlg.ui").c_str(), bShow);
}

void CDnInstantSpecializationDlg::InitialUpdate()
{
	m_pPopupBackground = GetControl<CEtUIStatic>("ID_STATIC_BOARD");
	m_pPopupText = GetControl<CEtUIStatic>("ID_TEXT");
	m_pPopupText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9990201));  //A new class job is available ! Click here to learn it!
	m_pPopupText->Show(false);
	m_pPopupBackground->Show(false);

	m_pBookText = GetControl<CEtUIStatic>("ID_TEXT_LEVELGUIDE");
	m_pBookText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9990202));  //Instant Specialization
	m_pBookText->SetTooltipText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9990202));  //Instant Specialization
	m_pBookButton = GetControl<CEtUIButton>("ID_BT_LEVELGUIDE");

}

void CDnInstantSpecializationDlg::Show(bool bShow)
{
	if (m_bShow == bShow) return;

	if (bShow && CDnActor::s_hLocalActor)
	{
		int nJobDeepLevel = HasNextJob();
		int nCharLevel = CDnActor::s_hLocalActor->GetLevel();

		if (nJobDeepLevel == 1 && nCharLevel < 15 || nJobDeepLevel == 2 && nCharLevel < 45 || nJobDeepLevel == 0)
			return;
	}

	CEtUIDialog::Show(bShow);
}

void CDnInstantSpecializationDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BT_LEVELGUIDE"))
		{
			if (CTaskManager::GetInstance().GetTask("VillageTask"))
			{
				int nJobDeepLevel = HasNextJob();
				int nCharLevel = CDnActor::s_hLocalActor->GetLevel();
				if (nJobDeepLevel == 0)
				{
					GetInterface().MessageBox(9990203); //no JOB!
					this->Show(false);
					return;
				}

				if (nJobDeepLevel == 1 && nCharLevel < 15 || nJobDeepLevel == 2 && nCharLevel < 45)
				{
					GetInterface().MessageBox(9990204);
				}
				else {
					GetInterface().ShowJobChangeUI();
				}
			}
			else {
				GetInterface().MessageBox(9990200); //Instant Specialization Available only in town!
			}
		}

		if (IsCmdControl("ID_STATIC_BOARD") || IsCmdControl("ID_TEXT"))
		{
			m_pPopupText->Show(false);
			m_pPopupBackground->Show(false);
		}
	}


	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}


int CDnInstantSpecializationDlg::HasNextJob()
{
	if (CDnActor::s_hLocalActor)
	{
		int nClassID = CDnActor::s_hLocalActor->OnGetJobClassID();
		DNTableFileFormat * pTable = GetDNTable(CDnTableDB::TJOB);
		if (!pTable)
			return 0;

		for (int i = 0; i < pTable->GetItemCount(); i++)
		{
			int nItem = pTable->GetItemID(i);
			int nParentJob = pTable->GetFieldFromLablePtr(nItem, "_ParentJob")->GetInteger();
			int nService = pTable->GetFieldFromLablePtr(nItem, "_Service")->GetInteger();
			int nJobLevel = pTable->GetFieldFromLablePtr(nItem, "_JobNumber")->GetInteger();
			if (nParentJob == nClassID && nService == 1)
			{
				return nJobLevel;
			}
		}
	}

	return 0;
}