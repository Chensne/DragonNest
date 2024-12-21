#pragma once

#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnMutatorTeamGame.h"

namespace AllKillModeString
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
		UIStringRoundCancel = 126186,
		UIStringKillStreak = 126004,
		UIStringALLKILL = 121133,
		UIStringPerpectDraw = 121132,
	};

}

class CDnMutatorAllKillMode:public CDnMutatorTeamGame
{
protected:	

	bool m_bFirstRound;
	int m_iRound;
	int m_iEnemyUserCount;
	int m_iFriendyUserCount;

	int m_iFinishDetailReason;
	float m_fFinishDetailReasonShowDelay;

public:
	CDnMutatorAllKillMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorAllKillMode();

	virtual int  Scoring( int &team ){return 0;}
	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );

	virtual void GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec);
	virtual void EndGame( void * pData );
	virtual void RoundEnd(void * pData );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual IScoreSystem*	CreateScoreSystem();

protected:
	virtual void _RenderGameModeScore();

public:

	void SetContinousWinCount( UINT nUserSession , UINT nWinCount );
	void SetFinishDetailReason( int nUIStringIndex );

};