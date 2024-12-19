#pragma once

#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnMutatorTeamGame.h"

namespace CaptainModeString
{
	enum eGameMessage{
		Kill = 120402,
		Killed = 120403,
		SeeKill = 120404,		
		KillPoint = 120407,
		SupportPoint = 120408,
		CoopPoint = 120409,
		UIStringVictory = 120301,
		UIStringDefeated = 120302,
		UIStringDraw =	120311,
		UIStringStarting =	121021,
		UIStringRound =	120314,
		UIStringTimeOver = 120303,
		WaitRespawn = 121014,
	};

}

class CDnMutatorCaptainMode : public CDnMutatorTeamGame, public CEtCustomRender
{

protected:	

	DnActorHandle m_hFriendlyCaptain;

	bool m_bFirstRound;
	bool m_bIsEndGame;
	bool m_bWarningMsg;
	bool m_bState_RoundEnd;
	bool m_bAlramCaptainCondition;

	int m_nWinTeam;
	int m_iRound;
	int m_nRoundEndString;

	float m_fRoundEndCount;
	float m_fWarningMsgCount;

public:
	CDnMutatorCaptainMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorCaptainMode();

	virtual int  Scoring( int &team ){return 0;}
	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	virtual void GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec);
	virtual void EndGame( void * pData );
	virtual void RoundEnd(void * pData );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void RenderCustom( float fElapsedTime );
	virtual void OnInitializeBreakIntoActor( DnActorHandle hPlayerActor  );
	virtual void ProcessWarningMessage(LOCAL_TIME LocalTime, float fDelta);

	virtual IScoreSystem*	CreateScoreSystem();

public:

	void SetTimeOverMessage(float fRemainTime){m_fRoundEndCount = fRemainTime;};
	void ProcessTimeOverMessage(float fDelta);
	void OnSelectFriendlyCaptain( DnActorHandle hActor ){ m_hFriendlyCaptain = hActor; }
	void RenderCaptainPointMark();

protected:
	virtual void _RenderGameModeScore();

};
