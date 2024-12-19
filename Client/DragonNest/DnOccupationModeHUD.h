#pragma once
#include "DnCustomDlg.h"
#include "DnPVPBaseHUD.h"
#include "DnClimaxEffect.h"

class CDnExpGauge;
class CDnOccupationModeNoticeDlg;
class CDnOccupationModeClimaxDlg;

class CDnOccupationModeHUD : public CDnPVPBaseHUD, public CDnClimaxEffect
{

public:
	CDnOccupationModeHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnOccupationModeHUD(void);

protected:

	enum{
		CLIMAX_EFFECT_TIME = 2,
	};

	CEtUIStatic * m_pStaticClimaxString;
	CEtUIStatic * m_pStaticClimaxBackground;

	CEtUIStatic * m_pStaticClimaxMinute;
	CEtUIStatic * m_pStaticClimaxSecond;
	CEtUIStatic * m_pStaticClimaxMilliSecond;
	CEtUIStatic * m_pStaticClimaxSemicolon1;
	CEtUIStatic * m_pStaticClimaxSemicolon2;
	CEtUIStatic * m_pStaticClimaxTimeBackground;

	CDnExpGauge * m_pBlueBar;
	CDnExpGauge * m_pRedBar;

#ifdef PRE_MOD_PVPOBSERVER		
	bool m_bFirstObserver;
#endif // PRE_MOD_PVPOBSERVER


	CDnOccupationModeNoticeDlg * m_pNoticeDlg;
	CDnOccupationModeClimaxDlg * m_pClimaxDlg;

	bool  m_bClimaxModeStart;
	bool  m_bClimaxModeEffect;

	int m_nGoalScore;
	int m_nClimaxTime;

	virtual void UpdateTimer();

	void ProcessScore();
	void ProcessClimax( float fElapsedTime );

public:
	void StartClimaxEffect();
	void StartClimaxMode( bool bStart );
#ifdef PRE_MOD_PVPOBSERVER
	void ShowOccupationInfo( DnActorHandle hActor, std::wstring & wszFlagName, int nBlueStringID, int nRedStringID, int nObserverBlueStringID, int nObserverRedStringID );	
#else
	void ShowOccupationInfo( DnActorHandle hActor, std::wstring & wszFlagName, int nBlueStringID, int nRedStringID );
#endif // PRE_MOD_PVPOBSERVER
	

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
};
