#pragma once

#include "DnCustomDlg.h"
#include "DnUIDefine.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

using namespace PvPTournamentUIDef;

class CDnPVPTournamentFinalResultListDlg : public CDnCustomDlg
{
public:
	CDnPVPTournamentFinalResultListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentFinalResultListDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	void SetInfo(const SFinalReportUserInfo& info, int rank);

private:
	CEtUIStatic* m_pRank;
	CDnJobIconStatic* m_pJobIcon;
	CEtUITextureControl* m_pGuildIcon;
	CEtUIStatic* m_pGuildName;
	CEtUITextureControl* m_pPVPRankIcon;
	CEtUIStatic* m_pName;
	CEtUIStatic* m_pKillCount;
	CEtUIStatic* m_pDeathCount;
	CEtUIStatic* m_pScore;
	CEtUIStatic* m_pPVPXP;
	CEtUIStatic* m_pPCCafe;
	CEtUITextureControl* m_pMedalIcon;
	CEtUIStatic* m_pMedal;

	EtTextureHandle m_hPVPMedalIconImage;
};

#ifdef PRE_PVP_GAMBLEROOM

class CDnPVPTournamentGambleFinalResultListDlg : public CDnCustomDlg
{
public:
	CDnPVPTournamentGambleFinalResultListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPTournamentGambleFinalResultListDlg( void );

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );

	void SetInfo( const SFinalReportUserInfo& info, int rank );

private:
	CEtUIStatic* m_pRank;
	CEtUIStatic* m_pLevel;
	CDnJobIconStatic* m_pJobIcon;
	CEtUIStatic* m_pGuildName;
	CEtUITextureControl* m_pPVPRankIcon;
	CEtUIStatic* m_pName;
	CEtUIStatic* m_pKillCount;
	CEtUIStatic* m_pDeathCount;
	CEtUIStatic* m_pPVPXP;
	CEtUITextureControl* m_pMedalIcon;
	CEtUIStatic* m_pMedal;

	EtTextureHandle m_hPVPMedalIconImage;
};

#endif // PRE_PVP_GAMBLEROOM

#endif	// PRE_ADD_PVP_TOURNAMENT