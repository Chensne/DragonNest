#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_GUILD_EASYSYSTEM
class CDnGuildLevelDlg : public CEtUIDialog, public CEtUICallback
#else
class CDnGuildLevelDlg : public CEtUIDialog
#endif
{
public:
	CDnGuildLevelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildLevelDlg(void);

protected:
	CEtUIStatic *m_pStaticLevel;
	CEtUIStatic *m_pStaticPercent;
	CEtUIProgressBar *m_pProgressBar;
	CEtUIStatic *m_pStaticPoint;
	CEtUIStatic *m_pStaticStage;
	CEtUIStatic *m_pStaticGuildMission;
	CEtUIStatic *m_pStaticGuildWar;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	CEtUIIMEEditBox *m_pEditBoxHomepage;
	CEtUIStatic *m_pStaticHomepage;
	CEtUIButton *m_pButtonHomepage;
#endif
#endif

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	CEtUIStatic *m_pStaticWeeklyContribution;
	CEtUIStatic *m_pStaticCurrentContribution;
	CEtUIStatic *m_pStaticWeeklyCtriText;
	CEtUIStatic *m_pStaticCurrentCtriText;
#endif 

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	int m_nToTalContributionPoint;
	int m_nWeeklyContributionPoint;
#endif 

	void RefreshGuildLevel();
#ifdef PRE_ADD_GUILD_EASYSYSTEM
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	void RefreshGuildInfo();
#endif
#endif

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
#endif

#ifdef PRE_ADD_GUILD_CONTRIBUTION
	void SetGuildContributinPoint( int nTotalContribution, int nWeeklyContribution );
#endif 
};