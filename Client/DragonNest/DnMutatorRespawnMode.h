#pragma once

#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnMutatorTeamGame.h"

namespace RespawnModeString
{
	enum eGameMessage{
		Kill = 120402,
		Killed = 120403,
		SeeKill = 120404,		
		KillPoint = 120407,
		SupportPoint = 120408,
		CoopPoint = 120409,
		WaitRespawn = 121014,
		UIStringVictory = 120301,
		UIStringDefeated = 120302,
		UIStringDraw =	120311,
		UIStringStarting =	121021,
		UIStringIndividualVictory = 120065,
		UIStringIndividualDraw = 120068,
		UIStringTimeOver = 120303,
	};

}

class CDnMutatorRespawnMode:public CDnMutatorTeamGame
{
public:
	CDnMutatorRespawnMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorRespawnMode();

protected:

public:
	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual int  Scoring( int &team ){return 0;}
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	virtual void GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec);
	virtual void EndGame( void * pData );

	void	PushUsedRespawnArea( const std::string& name );
	bool	bIsUsedRespawnArea( const std::string& name );

	// ScoreSystem
	virtual IScoreSystem*	CreateScoreSystem();	
private:

	void _RenderGameModeScore();

	std::list<std::string>	m_UsedRespawnAreaList;
};