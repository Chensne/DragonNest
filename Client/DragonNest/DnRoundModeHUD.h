#pragma once
#include "EtUIDialog.h"
#include "DnPVPBaseHUD.h"
#include "DnJobIconStatic.h"

class CDnRoundModeHUD : public CDnPVPBaseHUD
{

public:
	CDnRoundModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRoundModeHUD(void);

protected:

#ifdef PRE_ADD_PVP_DUAL_INFO
	struct sDualInfoUI
	{
		CEtUIStatic *pStaticUserName[PvPCommon::TeamIndex::Max];
		CDnJobIconStatic *pUIJobIcon[PvPCommon::TeamIndex::Max];
		CEtUIStatic *pStaticVSMark;
		bool bIsShow;
	#ifdef PRE_ADD_PVP_TOURNAMENT
		bool bForceShowOff;
	#endif

		sDualInfoUI()
		{
			for(int i=0; i<PvPCommon::TeamIndex::Max; i++ )
			{
				pStaticUserName[i] = NULL;
				pUIJobIcon[i] = NULL;
			}
			pStaticVSMark = NULL;
			bIsShow = false;
	#ifdef PRE_ADD_PVP_TOURNAMENT
			bForceShowOff = false;
	#endif
		}

		void Show(bool bShow)
		{
#ifdef PRE_ADD_PVP_TOURNAMENT
			if (bForceShowOff)
				bShow = false;
#endif
			for(int i=0; i<PvPCommon::TeamIndex::Max; i++ )
			{
				if(pStaticUserName[i])
					pStaticUserName[i]->Show(bShow);
				if(pUIJobIcon[i])
					pUIJobIcon[i]->Show(bShow);
			}

			if(pStaticVSMark)
				pStaticVSMark->Show(bShow);

			bIsShow = bShow;
		}

#ifdef PRE_ADD_PVP_TOURNAMENT
		void SetForceShowOff(bool bSet)
		{
			bForceShowOff = bSet;
		}
#endif

		bool IsShow() { return bIsShow; }

	};

	sDualInfoUI m_sImpactDualInfo;
	sDualInfoUI m_sContinuousDualInfo;

	float m_fDualInfoDelayTime;
#endif

	CEtUIStatic *m_pMyTeamWinMark[PvPCommon::s_iMaxRoundNum];
	CEtUIStatic *m_pMyTeamEmptyMark[PvPCommon::s_iMaxRoundNum];
	CEtUIStatic *m_pEnemyTeamWinMark[PvPCommon::s_iMaxRoundNum];
	CEtUIStatic *m_pEnemyTeamEmptyMark[PvPCommon::s_iMaxRoundNum];
	CEtUIStatic *m_pKillMark;

#ifdef PRE_MOD_PVPOBSERVER	
	CEtUIStatic * m_pStaticFriendly;
	CEtUIStatic * m_pStaticEnemy;    
	bool m_bFirstObserver;
#endif // PRE_MOD_PVPOBSERVER

	int m_iMyTeamVicNum;
	int m_iEnemyTeamVicNum;


public:
	void SetStage( int iStageNum );
	void ResetStage();
	void SetWinStage( int iMyTeamVicNum , int iEnemyTeamVicNum  );
#ifdef PRE_ADD_PVP_DUAL_INFO
	void ProcessDualCount( float fElapsedTime );
	void OrderShowDualInfo( bool bBreakInto = false );
	void SetDualInfo( const WCHAR *blueName , int blueJobIndex , const WCHAR *redName , int redJobIndex );
	void ClearDualInfo();
	void ShowContinouosInfoForBreakInto(); // 1:1 레더 일경우만 난입했을때 표시해줍니다 / 지금 클라로써는 특수한 케이스에만 알수가있음.
	#ifdef PRE_ADD_PVP_TOURNAMENT
	void ForceShowOff_ContinuousDualInfo(bool bShowOff);
	#endif
#endif

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );

};