#pragma once
#include "EtUIDialog.h"
#include "DnPVPBaseHUD.h"

class CDnIndividualRoundModeHUD : public CDnPVPBaseHUD
{

public:
	CDnIndividualRoundModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnIndividualRoundModeHUD(void);

protected:
	CEtUIStatic *m_pRoundMark[PvPCommon::s_iMaxRoundNum];
	CEtUIStatic *m_pRoundEmptyMark[PvPCommon::s_iMaxRoundNum];

	CEtUIStatic *m_pStatic_TeamName_A;
	CEtUIStatic *m_pStatic_TeamName_B;

#ifdef PRE_MOD_PVPOBSERVER	
	bool m_bFirstObserver;
#endif // PRE_MOD_PVPOBSERVER

	int m_iRoundCount;
	int m_nStageNum;

protected:
	virtual void Process( float fElapsedTime );
public:

	void SetStage( int iStageNum );
	void ChangeTeamName(int nMyTeamUIString,int nEnemyTeamUIString);
#ifdef PRE_MOD_PVPOBSERVER
	void SetWinStage( int iMyTeamVicNum , int iEnemyTeamVicNum, bool bZombie=false );
#else
	void SetWinStage( int iMyTeamVicNum , int iEnemyTeamVicNum );
#endif // #ifdef PRE_MOD_PVPOBSERVER
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

};