#include "StdAfx.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnTimeEventTask.h"
#include "DnSystemDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnMainBarMenuDlg.h"
#include "PvPSendPacket.h"
#include "DnLocalPlayerActor.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMainBarMenuDlg::CDnMainBarMenuDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
	bShowTrigger = false;
}

CDnMainBarMenuDlg::~CDnMainBarMenuDlg(void)
{

}

void CDnMainBarMenuDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MainBar_Menu.ui").c_str(), bShow);
}

void CDnMainBarMenuDlg::InitialUpdate()
{
	/*
	m_pButtonSystem = GetControl<CEtUIStatic>("ID_STATIC_SLOT");
	m_pButtonSystem->SetText(L"ESC");
	buttonInfo.nDialogID = CDnMainMenuDlg::CHATROOM_DIALOG;
	m_pButtonSystem->Enable(true);


	SButtonInfo buttonInfo;
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_RANK");
	buttonInfo.nDialogID = CDnMainMenuDlg::PVP_RANK_DIALOG;
	m_vecButtonInfo.push_back( buttonInfo );
	GetControl<CDnMenuButton>( "ID_RANK" )->Show( true );*/

	GetControl<CEtUIStatic>("ID_STATIC_STATUSTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_SKILLTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_INVENTORYTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_QUESTTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_MISSIONTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_COMMUNITYTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_CHATTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_COLLECTIONTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_REPUTETIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_GUILDTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_COLOSSEUMTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_RANKTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_EVENTTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_MAPICONTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_HELPTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_CONTRILTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_DAMAGETIP")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_SYSTEMTIP")->Show(false);
	GetControl<CEtUIStatic>("ID_BT_DNGIRL")->Show(false);
}

void CDnMainBarMenuDlg::Show(bool bShow)
{
	if (isShow == true && bShow == true)
	{
		CEtUIDialog::Show(false);
		isShow = false;
		return;
	}

	CEtUIDialog::Show(bShow);

	if (bShow)
	{
		isShow = true;
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg)
		{
			//CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
			//if (pLocalActor)
			//	pLocalActor->LockInput(true);

			CDnLocalPlayerActor::LockInput(true);
			CDnMouseCursor::GetInstance().ShowCursor(true, true);
			pMainMenuDlg->ToggleButtonCtrl(2);
			CDnLocalPlayerActor::ShowCrosshair(false);
			bShowTrigger = true;
		}
	}
	else {
		if (bShowTrigger)
		{
			bShowTrigger = false;
			CDnLocalPlayerActor::ShowCrosshair(true);
			CDnLocalPlayerActor::LockInput(false);

		}
	}
}

void CDnMainBarMenuDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (!GetInterface().IsLockMainMenu()) {
		if (nCommand == EVENT_BUTTON_CLICKED)
		{
			pMainMenuDlg = GetInterface().GetMainMenuDialog(); //dde mutat de aici
			if (!pMainMenuDlg) return;

			if (GetInterface().IsShowPrivateMarketDlg()
				|| GetInterface().IsShowCostumeMixDlg()
				|| GetInterface().IsShowCostumeDesignMixDlg()
#ifdef PRE_ADD_COSRANDMIX
				|| GetInterface().IsShowCostumeRandomMixDlg()
#endif
				)
				return;

			if (GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd))
				return;

			//inchide fereastra
			this->Show(false);
			this->isShow = false;

			if (IsCmdControl("ID_BT_MAP"))
			{
				if (GetInterface().IsPVP())
					GetInterface().TogglePVPMapDlg();
				else
					pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ZONEMAP_DIALOG);

				return;
			}

			if (IsCmdControl("ID_BT_CHAT"))
			{
				pMainMenuDlg->SetChatRoomFlag(uMsg == WM_KEYDOWN);
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHATROOM_DIALOG);
				return;
			}


#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
			if (IsCmdControl("ID_BT_REPUTE"))
			{
#ifdef PRE_ADD_NO_REPUTATION_DLG
#else
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG);
#endif
				return;
			}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
			if (IsCmdControl("ID_BT_EVENT"))
			{
				if (!CDnTimeEventTask::IsActive()) return;
				CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();

				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG);

				return;
			}
#endif
#ifdef PRE_ADD_NO_HELP_DLG
#else
			if (IsCmdControl("ID_BT_HELP"))
			{
				pMainMenuDlg->CloseMenuDialog();
				pMainMenuDlg->GetSystemDlg()->ToggleHelpDlg();
				return;
			}
#endif
#ifdef PRE_ADD_PVP_RANKING
			if (IsCmdControl("ID_BT_RANK"))
			{
				GetInterface().OpenPvPRankDlg();
			}
#endif

			if (IsCmdControl("ID_BT_SYSTEM"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::SYSTEM_DIALOG);
				return;
			}

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
			if (IsCmdControl("ID_BT_COLOSSEUM"))
			{
#ifdef PRE_ADD_DWC
				if (GetDWCTask().IsDWCChar() == false)
				{
					if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
					{
						if (GetInterface().IsShowPVPVillageAccessDlg())
							GetInterface().ShowPVPVillageAccessDlg(false);
						else
							SendRequestPVPVillageAcess();
					}
				}
#else	// PRE_ADD_DWC
				if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
				{
					if (GetInterface().IsShowPVPVillageAccessDlg())
						GetInterface().ShowPVPVillageAccessDlg(false);
					else
						SendRequestPVPVillageAcess();
				}
#endif	// PRE_ADD_DWC
			}
#endif // PRE_ADD_PVP_VILLAGE_ACCESS
			if (IsCmdControl("ID_BT_GUILD"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
				return;
			}
			if (IsCmdControl("ID_BT_COMMUNITY"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
				return;
			}
			if (IsCmdControl("ID_BT_STATUS"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG);
				return;
			}

			if (IsCmdControl("ID_BT_SKILL"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::SKILL_DIALOG);
				return;
			}
			if (IsCmdControl("ID_BT_INVENTORY"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				return;
			}
			if (IsCmdControl("ID_BT_QUEST"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
				return;
			}
			if (IsCmdControl("ID_BT_MISSION"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::MISSION_DIALOG);
				return;
			}

		}
	}

}

void CDnMainBarMenuDlg::SetButtonsHotKeys(int nUIWrappingKeyIndex, BYTE cVK)
{
	//	switch (nUIWrappingKeyIndex)
	//	{
	//	case IW_UI_QUICKSLOTCHANGE:
	//		if (GetControl("ID_BUTTON_SLOT_UP"))	GetControl("ID_BUTTON_SLOT_UP")->SetHotKey(cVK);
	//		break;
	//
	//	case IW_UI_CHAR:		if (GetControl<CDnMenuButton>("ID_CHAR_STATUS")) GetControl<CDnMenuButton>("ID_CHAR_STATUS")->SetHotKey(cVK);	break;
	//	case IW_UI_COMMUNITY:	if (GetControl<CDnMenuButton>("ID_COMMUNITY")) GetControl<CDnMenuButton>("ID_COMMUNITY")->SetHotKey(cVK);	break;
	//	case IW_UI_INVEN:		if (GetControl<CDnMenuButton>("ID_INVENTORY")) GetControl<CDnMenuButton>("ID_INVENTORY")->SetHotKey(cVK);	break;
	//	case IW_UI_QUEST:		if (GetControl<CDnMenuButton>("ID_QUEST")) GetControl<CDnMenuButton>("ID_QUEST")->SetHotKey(cVK);		break;
	//	case IW_UI_SKILL:		if (GetControl<CDnMenuButton>("ID_SKILL")) GetControl<CDnMenuButton>("ID_SKILL")->SetHotKey(cVK);		break;
     //case IW_UI_MISSION:		if (GetControl<CDnMenuButton>("ID_BT_MISSION")) GetControl<CDnMenuButton>("ID_BT_MISSION")->SetHotKey(cVK);		break;
	//	case IW_UI_MAP:			if (GetControl<CDnMenuButton>("ID_BT_MAP")) GetControl<CDnMenuButton>("ID_BT_MAP")->SetHotKey(cVK);			break;
	//	case IW_UI_CHAT:		if (GetControl<CDnMenuButton>("ID_CHAT")) GetControl<CDnMenuButton>("ID_CHAT")->SetHotKey(cVK);		break;
	//#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	//	case IW_UI_REPUTE:		if (GetControl<CDnMenuButton>("ID_REPUTE")) GetControl<CDnMenuButton>("ID_REPUTE")->SetHotKey(cVK);	break;
	//#endif
	//	case IW_UI_EVENT:		if (GetControl<CDnMenuButton>("ID_BT_EVENT")) GetControl<CDnMenuButton>("ID_BT_EVENT")->SetHotKey(cVK);	break;
	//	case IW_UI_USERHELP:	if (GetControl<CDnMenuButton>("ID_BT_HELP")) GetControl<CDnMenuButton>("ID_BT_HELP")->SetHotKey(cVK);	break;
	//#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	//	case IW_UI_PVP_VILLAGE_ACCESS: if (GetControl<CDnMenuButton>("ID_BT_COLOSSEUM")) GetControl<CDnMenuButton>("ID_BT_COLOSSEUM")->SetHotKey(cVK); break;
	//#endif
	//#ifdef PRE_ADD_PVP_RANKING
	//	case IW_UI_PVPRANK_OPEN: if (GetControl<CDnMenuButton>("ID_BT_RANK")) GetControl<CDnMenuButton>("ID_BT_RANK")->SetHotKey(cVK); break;
	//#endif // PRE_ADD_PVP_RANKING
	//#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS	
	//	case IW_UI_MAILBOX_OPEN: if (GetControl("ID_MAIL")) GetControl("ID_MAIL")->SetHotKey(cVK); break;
	//#endif
	//
	//	}
}