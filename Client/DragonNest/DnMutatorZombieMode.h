#pragma once

#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnMutatorTeamGame.h"

namespace ZombieModeString
{
	enum eGameMessage{
		Kill = 120402,
		Killed = 120403,
		SeeKill = 120404,		
		KillPoint = 120407,
		SupportPoint = 120408,
		CoopPoint = 120409,
		UIStringVictory = 120301,
		UIStringVictory_Zombie = 120082,
		UIStringVictory_Human  = 120083,
		UIStringDefeated = 120302,
		UIStringDraw =	120311,
		UIStringStarting =	121021,
		UIStringRound =	120314,
		UIStringGainAdditionalXP = 121096, 
	};

}

class CDnMutatorZombieMode:public CDnMutatorTeamGame
{
public:
	CDnMutatorZombieMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorZombieMode();

protected:	

	bool	m_bFirstRound;
	int     m_iRound;

protected:
	virtual void _RenderGameModeScore();

public:
	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual int  Scoring( int &team ){return 0;}
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	virtual void GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec);
	virtual void EndGame( void * pData );
	virtual void RoundEnd(void * pData );
	// ScoreSystem
	virtual IScoreSystem*	CreateScoreSystem();

};

