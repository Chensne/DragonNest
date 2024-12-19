#pragma once

#ifdef PRE_ADD_GUILD_CONTRIBUTION
#include "DnCustomDlg.h"

class CDnGuildContributionRankDlg : public CDnCustomDlg
{
public:
	CDnGuildContributionRankDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false );
	virtual ~CDnGuildContributionRankDlg();
protected:

	CEtUIStatic *m_pStaticWeeklyRank;
	CEtUIListBoxEx *m_pListBoxRankList;
	
public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	
	void SetGuildRankList( GuildContribution::TGuildContributionRankingData* pData, int nSize );
};

#endif 