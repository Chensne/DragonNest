#include "StdAfx.h"
#include "DnGuildTabDlg.h"
#include "DnGuildRecruitTabDlg.h"
#include "DnGuildRecruitDlg.h"
#include "DnGuildRequestDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildTabDlg::CDnGuildTabDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUITabDialog(dialogType, pParentDialog, nID, pCallback)
	, m_pHomeButton(NULL)
	, m_pHelpButton(NULL)
	, m_pCloseButton(NULL)
	, m_pGuildRecruitTabDlg(NULL)
{
	for (int i = 0; i < MAX_TABS; i++)
		m_pMenuTabButtons[i] = NULL;
}

CDnGuildTabDlg::~CDnGuildTabDlg(void)
{
}

void CDnGuildTabDlg::Initialize(bool bShow)
{
	CEtUITabDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("Guild_TabDlg.ui").c_str(), bShow);
}

void CDnGuildTabDlg::InitialUpdate()
{
	for (int i = 0; i < MAX_TABS; i++)
	{
		m_pMenuTabButtons[i] = GetControl<CEtUIRadioButton>(FormatA("ID_TAB%d", i).c_str());
		m_pMenuTabButtons[i]->Enable(false);
	}
	m_pGuildRecruitTabDlg = new CDnGuildRecruitTabDlg(UI_TYPE_CHILD, this);
	m_pGuildRecruitTabDlg->Initialize(false);
	AddTabDialog(m_pMenuTabButtons[RECRUIT], m_pGuildRecruitTabDlg);

	m_pHomeButton = GetControl<CEtUIButton>("ID_BT_HOMEPAGE");
	m_pHomeButton->Show(false);

	m_pHelpButton = GetControl<CEtUIButton>("ID_BT_SMALLHELP");
	m_pCloseButton = GetControl<CEtUIButton>("ID_BT_CLOSE");

	//m_pRequestTabButton = GetControl<CEtUIRadioButton>("ID_RBT_REQUEST");
	//m_pGuildRequestDlg = new CDnGuildRequestDlg(UI_TYPE_CHILD, this);
	//m_pGuildRequestDlg->Initialize(false);
	//AddTabDialog(m_pRequestTabButton, m_pGuildRequestDlg);

	SetCheckedTab(m_pMenuTabButtons[RECRUIT]->GetTabID());
}

void CDnGuildTabDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BT_CLOSE"))
		{
			Show(false);
			return;
		}
	}

	CEtUITabDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}



void CDnGuildTabDlg::OnRecvGuildRecruitRegisterInfo(GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket)
{
	if (!m_pGuildRecruitTabDlg) return;
	m_pGuildRecruitTabDlg->OnRecvGuildRecruitRegisterInfo(pPacket);
}

void CDnGuildTabDlg::OnRecvGuildRecruitCharacter(GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket)
{
	if (!m_pGuildRecruitTabDlg) return;
	m_pGuildRecruitTabDlg->OnRecvGuildRecruitCharacter(pPacket);
}

void CDnGuildTabDlg::OnRecvGuildRecruitAcceptResult(GuildRecruitSystem::SCGuildRecruitAccept *pPacket)
{
	if (!m_pGuildRecruitTabDlg) return;
	m_pGuildRecruitTabDlg->OnRecvGuildRecruitAcceptResult(pPacket);
}