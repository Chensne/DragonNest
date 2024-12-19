#pragma once
#include "DnCustomDlg.h"
#include "DnWeapon.h"

class CDnPvPKillListDlg;
class CDnPVPBaseHUD : public CDnCustomDlg
{

public:
	enum
	{
		KILLMARK_SHOW_TIME = 2,
		MAX_KILLMARK_COUNT = 8,
	};


protected:

	bool m_StartTimer;
	__time64_t  m_tAxisTime;

	int	m_nMin;	
	int	m_nSec;
	int	m_nKillSoundID;
	int	m_nStartSound;
	int m_nMyTeam_Score;
	int m_nEnemyTeam_Score;
	int m_nCurContinuousKillCount;

	float m_tMSec;    
	float m_fRemainSec;
	float m_fTotalSec;	
	float m_fKillMarkTime;

	CEtUIStatic * m_pRoomName;
	CEtUIStatic * m_pMYTeam;
	CEtUIStatic * m_pEnemyTeam;
	CEtUIStatic * m_pMYTeamScore;
	CEtUIStatic * m_pEneyTeamScore;
	CEtUIStatic * m_pTimer_Min;
	CEtUIStatic * m_pTimer_Sec;
	CEtUIStatic * m_pTimer_Semi;
	CEtUIStatic * m_pObjectiveCount;

	CEtUIStatic * m_pKillMark;
	CEtUIStatic *m_pKillMarks[MAX_KILLMARK_COUNT];
	CEtUIStatic *m_pKillTexts[MAX_KILLMARK_COUNT];

	SUICoord m_uiKillMarkCenter;
	SUICoord m_uiKillMarkLeft;
	SUICoord m_uiKillMarkRight;

	CDnPvPKillListDlg *m_pKillListDlg;

public:

	CDnPVPBaseHUD( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPBaseHUD(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ShowKillMark( int nContinuousKillCount, float fTime );
	virtual void SetWinCondition( WCHAR * wszString );
	virtual bool NearEndGame(){ if( m_fTotalSec*0.25 > m_fRemainSec )return true;return false; }

	void ShowKillInfo( DnActorHandle hKiller, DnActorHandle hKilled, CDnWeapon::EquipTypeEnum EquipType );
	
	void SetMyScore( int nScore );
	void SetEnemyScore( int nScore );
	void SetTime( float nSec ,__time64_t sTime , int sMSec);
	void HaltTimer() {m_StartTimer = false; if(m_fRemainSec != 0.f){ m_fRemainSec = 0.f; UpdateTimer(); }}
	void StartTimer();
	void SetRoomName( const WCHAR * wszRoomName );

	int GetRemainSec() { return (int)m_fRemainSec; }
	float GetFloatRemainSec() { return m_fRemainSec; }

#ifdef PRE_MOD_PVPOBSERVER
	bool SetTextObserverTeam( CEtUIStatic * pStaticFriend, CEtUIStatic * pStaticEnemy, int textIdxFriend, int textIdxEnemy );
#endif // PRE_MOD_PVPOBSERVER

protected:
	virtual void UpdateTimer();
	void UpdateKillMarkPos( int nContinuousKillCount );

};