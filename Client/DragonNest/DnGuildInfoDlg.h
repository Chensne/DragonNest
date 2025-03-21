#pragma once
#include "EtUITabDialog.h"

class CDnGuildHistoryDlg;
class CDnGuildLevelDlg;
class CDnGuildRewardDlg;

#ifdef PRE_ADD_GUILD_CONTRIBUTION
class CDnGuildContributionRankDlg;
#endif 

class CDnGuildInfoDlg : public CEtUITabDialog
{
public:
	CDnGuildInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildInfoDlg(void);

protected:
	CEtUIRadioButton *m_pHistoryTabButton;
	CEtUIRadioButton *m_pLevelTabButton;
	CEtUIRadioButton *m_pRewardTabButton;

	CDnGuildHistoryDlg *m_pGuildHistoryDlg;
	CDnGuildLevelDlg *m_pGuildLevelDlg;
	CDnGuildRewardDlg *m_pGuildRewardDlg;
#ifdef PRE_ADD_GUILD_CONTRIBUTION
	CDnGuildContributionRankDlg* m_pGuildRankDlg;
#endif 

public:
	void OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket );
	void UpdateGuildRewardInfo();
	void CloseBuyGuildRewardConfirmDlg();
#ifdef PRE_ADD_GUILD_CONTRIBUTION
	void OnRecvGetGuildContributionRankList( GuildContribution::SCGuildContributionRank* pPacket );
	void OnRecvGetGuildContributionPoint( GuildContribution::SCGuildContributionPoint* pPacket );
#endif 

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};