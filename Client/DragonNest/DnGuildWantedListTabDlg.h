#pragma once
#include "EtUITabDialog.h"

class CDnGuildWantedListDlg;
class CDnGuildWantedMyListDlg;
class CDnGuildWantedListTabDlg : public CEtUITabDialog
{
public:
	CDnGuildWantedListTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWantedListTabDlg(void);

	enum
	{
		GUILD_WANTEDLIST_DIALOG,
		GUILD_MYWANTEDLIST_DIALOG,
	};

protected:
	CEtUIRadioButton *m_pWantedTabButton;
	CEtUIRadioButton *m_pJoinTabButton;

	CDnGuildWantedListDlg *m_pGuildWantedListDlg;
	CDnGuildWantedMyListDlg *m_pGuildWantedMyListDlg;

public:
	void OnRecvGetGuildRecruitList( GuildRecruitSystem::SCGuildRecruitList *pPacket );
	void OnRecvGetGuildRecruitRequestCount( GuildRecruitSystem::SCGuildRecruitRequestCount *pPacket );
	void OnRecvGetGuildRecruitMyList( GuildRecruitSystem::SCMyGuildRecruitList *pPacket );
	void OnRecvGuildRecruitRequest( GuildRecruitSystem::SCGuildRecruitRequest *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};