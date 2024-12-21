#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnPvPGameTask.h"

namespace GameString
{
	enum eGameMessage{		
		EnterGame = 120405,		
		ExitGame = 120406,
	};

}

class CDnPvPGameTask;
class IScoreSystem;

class CDnMutatorGame
{
public:
	CDnMutatorGame( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorGame();

public:
	CDnPvPGameTask * m_pGameTask;

protected:
	virtual void _RenderGameModeScore(){};		
public:
	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor ){ return true;}	
	virtual int  Scoring( int &team ){return 0;}
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ){};
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox ){return true;}
	virtual void GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec){}
	virtual void EndGame( void * pData ){}
	virtual void RestartGame(){}
	virtual void ProcessExitUser( DnActorHandle hActor );
	virtual void ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin = false, void * pData = NULL ){};
	virtual void RoundEnd(void * pData ){}
	virtual void OnPostInitializeStage() {}

	// ScoreSystem
	bool			InitializeBase( const UINT uiItemID, DNTableFileFormat*  pSox );
	IScoreSystem*	GetScoreSystem(){ return m_pScoreSystem; }

	virtual IScoreSystem*	CreateScoreSystem() = 0;
	virtual void			OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void			OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage );
	virtual void			OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo );
	virtual void			OnLeaveUser( DnActorHandle hActor );
	virtual void			OnInitializeBreakIntoActor( DnActorHandle hPlayerActor  ){}
	
protected:

	IScoreSystem*	m_pScoreSystem;
};
