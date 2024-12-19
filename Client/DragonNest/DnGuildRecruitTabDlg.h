#pragma once
#include "EtUITabDialog.h"

class CDnGuildRecruitDlg;
class CDnGuildRequestDlg;

class CDnGuildRecruitTabDlg : public CEtUITabDialog
{
public:
	CDnGuildRecruitTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRecruitTabDlg(void);

protected:
	CEtUIRadioButton *m_pWantedTabButton;
	CEtUIRadioButton *m_pRequestTabButton;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CDnGuildRecruitDlg *m_pGuildRecruitDlg;
	CDnGuildRequestDlg *m_pGuildRequestDlg;
	
		//CDnGuildRecruitTabDlg *m_pGuildRecruitTabDlg;

public:
	void OnRecvGuildRecruitRegisterInfo( GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket );
	void OnRecvGuildRecruitCharacter( GuildRecruitSystem::SCGuildRecruitCharacterList *pPacket );
	void OnRecvGuildRecruitAcceptResult( GuildRecruitSystem::SCGuildRecruitAccept *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};