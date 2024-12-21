#pragma once

#include "DnMutatorTeamGame.h"

namespace OccupationModeString
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
		UIStringBossVictory = 126262,
		UIStringBossDefeated = 126263,
		UIStringTimeVictory = 120301,
		UIStringTimeDefeated = 120302,
		WaitRespawn = 121014,
	};
}

class CDnOccupationTask;

class CDnMutatorOccupationMode : public CDnMutatorTeamGame, public CEtCustomRender
{
public :
	CDnMutatorOccupationMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorOccupationMode();

	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	virtual void GameStart( int nObjectiveUIString, float nRemainCountDown, float RemainSec, bool IsInGameJoin, float nMaxSec );
	virtual void EndGame( void * pData );
	virtual void OnPostInitializeStage();
	virtual void RenderCustom( float fElapsedTime );

	virtual IScoreSystem*	CreateScoreSystem();

	void _RenderGameModeScore();
	void _RenderOccupationZone();

	void ClimaxModeStart();
	void CheckResource();

	bool IsClimaxMode();

public:

	enum{
		TARGET_RESOURCE_MAX = 2,
	};

protected:

	CDnOccupationTask * m_pOccupationTask;

	int m_nMsgResource[TARGET_RESOURCE_MAX];
	int m_nMsgResourceStringID[TARGET_RESOURCE_MAX];
	bool m_bMsgResource[TARGET_RESOURCE_MAX];

	int m_nClimaxTime;
	int m_nClimaxRespawnTime;
	bool m_bClimaxMode;
	std::string m_szClimaxBGM;
};
