#pragma once
#include "EtUITabDialog.h"

class CDnGuildRecruitTabDlg;

class CDnGuildTabDlg : public CEtUITabDialog
{
public:
	CDnGuildTabDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnGuildTabDlg(void);

protected:
	enum TabType {
		MAIN_PAGE,
		GUILD_MEMBERS,
		GUILD_REWARDS,
		RECRUIT,
		GUILD_HISTORY,
		ACHIVEMENTS,
		MAX_TABS
	};

	CEtUIRadioButton *m_pMenuTabButtons[MAX_TABS];

	CEtUIButton *m_pHomeButton;
	CEtUIButton *m_pHelpButton;
	CEtUIButton *m_pCloseButton;


	CDnGuildRecruitTabDlg *m_pGuildRecruitTabDlg;

public:
	void OnRecvGuildRecruitRegisterInfo(GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket);
	void OnRecvGuildRecruitCharacter(GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket);
	void OnRecvGuildRecruitAcceptResult(GuildRecruitSystem::SCGuildRecruitAccept *pPacket);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
}; 
