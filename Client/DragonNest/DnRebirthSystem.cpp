#include "StdAfx.h"
#include "DnRebirthSystem.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "DnVillageTask.h"
#include "DnInterface.h"

#include "CustomSendPacket.h"



CDnRebirthSystem::CDnRebirthSystem()
{

}

CDnRebirthSystem::~CDnRebirthSystem()
{
	SAFE_DELETE(m_pData);
}

void CDnRebirthSystem::Initialize(bool bShow)
{

}

int CDnRebirthSystem::GetMinRebirthLevel(int CurRebirth, int Level)
{
	return 90; // to be done
}

void CDnRebirthSystem::OpenRebirthWindow()
{
	CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask("VillageTask");
	if (pVillageTask)
	{
		OutputDebug("Rebirth Level:%d", pVillageTask->GetLocalPlayer()->GetLevel());
		int nLevel = pVillageTask->GetLocalPlayer()->GetLevel();
		if (nLevel >= this->GetMinRebirthLevel(0, nLevel)) //to be done!
		{
			GetInterface().BigMessageBox(FormatW(L"Do you want to rebirth?\nDO IT NOW.").c_str(), MB_YESNO, 5000, CDnInterface::GetInstancePtr());
		}
	}
}


void CDnRebirthSystem::OnPressOK()
{
	SendDoRebirth();
	//TEST
	CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask("VillageTask");
	if (pVillageTask)
	{
		pVillageTask->GetLocalPlayer()->SetLevel(1);
	}
	
}
/*
//	GetInterface().BigMessageBox( wszOutputString, MB_YESNO, CDnCharStatusDlg::GLYPH_ATTACH_DIALOG, this );
void CDnRebirthSystem::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	OutputDebug("on ui callback id:%d command:%d", nID,nCommand);
	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_YES"))
		{
			OutputDebug("[%s] On YES Click!", __FUNCTION__);
		}
		else if (IsCmdControl("ID_NO"))
		{
			OutputDebug("[%s] On NO Click!", __FUNCTION__);
		}
	}

	}*/