#pragma once

#include "DnCustomDlg.h"
#include "DnBlindDlg.h"
#include "DnDarklairClearImp.h"

class CDnStageClearCounterMng;
class CDnStageClearRewardGoldDlg;
class CDnDarklairClearReportDlg : public CDnCustomDlg, public CDnBlindCallBack
{
public:
	CDnDarklairClearReportDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDarklairClearReportDlg();

	enum ePhaseState
	{
		PHASE_NONE,
		PHASE_START,
		PHASE_ROUND_START,
		PHASE_ROUND_PROCESS,
		PHASE_ROUND_BONUS_COUNT_PROCESS,
		PHASE_EXP_START,
		PHASE_EXP_PROCESS,
		PHASE_FINISH,
	};

	enum
	{
		eRounNumber_1_Cipher= 0,
		eRounNumber_2_Cipher= 1,
		eRounNumber_3_Cipher= 2,
		eRounNumber_4_Cipher= 3,
		eRoundNumber_Cipher = 4,
	};

protected:
	CEtUIStatic *m_pStaticAllRoundClear;
	CEtUIStatic *m_pStaticTime;
	CEtUIStatic *m_pStaticExp;
	CEtUIStatic *m_pStaticExpValue;
	CEtUIStatic *m_pStaticRoundCheck;
	CEtUIStatic *m_pStaticRound;
	CEtUIStatic *m_pStaticChannelBonus;
	CEtUIStatic *m_pStatic2ndPwdBonus;
#ifdef PRE_ADD_CH_EKEYCLEARBONUS
	CEtUIStatic *m_pStaticEKeyBonus;
#endif // PRE_ADD_CH_EKEYCLEARBONUS
	CEtUIStatic *m_pStaticMaxLevelCharBonus;
	CEtUIStatic *m_pStaticMaxLevelCharCount;

	CEtUITextureControl *m_pTextureRound[eRoundNumber_Cipher];
	CEtUITextureControl *m_pTextureRoundAfterImage[eRoundNumber_Cipher];

	ePhaseState	m_CurPhase;
	float m_fPhaseTime;
	int m_nSoundIndex;

	CDnStageClearCounterMng *m_pExpMng;
	CDnStageClearRewardGoldDlg*	m_pRewardGoldDlg;
	EtTextureHandle m_hRoundTexture;

	float			m_PhaseTimer;
	float			m_RoundCheckTimer;
	float			m_RoundAfterImangeTimer;
	float			m_RoundAccel;
	float			m_CheckAccel;
	SUICoord		m_RoundCheckOrgCoord;
	bool			m_bRoundNextStart;
	SUICoord		m_RoundOrgCoord;
	float			m_ChannelBonusTimer;
	float			m_MaxLevelCharCountTimer;

	const float		m_RoundShowTime;
	const float		m_RoundInitScale;
	const float		m_CheckShowTime;
	const float		m_CheckInitScale;

	const float		m_RoundAfterShowTime;
	const float		m_RoundAfterLastScale;
	const float		m_RoundAfterInitAlpha;

	CDnDarklairClearImp::SDungeonClearInfo *m_pPlayerDungeonClearInfo;
	CDnDarklairClearImp::SDungeonClearValue *m_pDungeonClearValue;


protected:
	void HideAllControl();
	void SetStageClearInfo();
	void GetUICtrlCenter(EtVector2& result, CEtUIControl* ctrl, float scale);
	void TextureRoundSet( CEtUITextureControl * pTextureRound, int nNumber );

	void InitPhase_Exp(bool bWithSign, bool bCounting, int initExp, int targetExp);
	void InitPhase_RoundChecker();
	void InitPhase_Round();
	void InitPhase_ChannelBonus();
#if defined(PRE_ADD_WORLD_EVENT)
	void InitPhase_EventBouns(WorldEvent::eDetailEventType type);
#else
	void InitPhase_EventBouns(eEventType3 type);
#endif 
	void InitPhase_PromotionBonus();

	bool ShowPhase_Round(float fElapsedTime);
	bool ShowPhase_RoundChecker(float fElapsedTime);
	void ShowPhase_ChannelBonus(float fElapsedTime);
	bool ShowPhase_Exp(float fElapsedTime);
	void ShowPhase_PromotionBonus(float fElapsedTime);

	void RefreshBonusAlarmCoord();

	void OnClose();

public:
	// CDnCustomDLg
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	// CDnBlindCallBack
	virtual void OnBlindOpen();
	virtual void OnBlindOpened();
};
