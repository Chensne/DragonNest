#pragma once
#include "DnCustomDlg.h"
#include "DnDungeonEnterDlg.h"


#define	MAX_REWARD_ITEM		6
#define EFFECT_START_GAP_TIME	0.5f
#define GAUGE_SPEED_FACTOR		60.0f

class CDnDungeonExpectRewardDlg : public CDnCustomDlg
{
public:
	CDnDungeonExpectRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDungeonExpectRewardDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

public:
	void UpdateDungeonExpectReward( int nMapIndex, int nDifficult );

private:
	void GetLevelExp( CDnPlayerActor* pActor, int nTempLevel, int& nCurLevExp, int& nNextLevExp );
	void CaclExpectExpPercent( CDnPlayerActor* pActor, int nNextLevExp, int nCurExp );
	void ClearDungeonExpectReward();
	int GetTableIndex( DNTableFileFormat*  pSox, int nMapIndex, int nDifficult );

protected:
	CDnItemSlotButton*	m_pStaticRewardItem[ MAX_REWARD_ITEM ];
	CEtUIStatic*	m_pStaticGold;
	CEtUIStatic*	m_pStaticSilver;
	CEtUIStatic*	m_pStaticBronze;
	CEtUITextBox*	m_pTextBoxExpGaugeValue;
	CEtUIProgressBar*	m_pProgressExpGauge[5];

	float	m_fExpectTotalExp;
	float	m_fStartGapTime;
	float	m_fExpGaugeDelta;
	float	m_fExpectExpPercent;
	float	m_fCurrentExpPercent;
	float	m_fGaugeSpeedFactor;
	bool	m_bStartEffectExpectExp;
	int		m_nTempLevel;
};

