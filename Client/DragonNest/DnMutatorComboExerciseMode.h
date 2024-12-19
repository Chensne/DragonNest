#pragma once

#include "DnMutatorTeamGame.h"

#ifdef PRE_ADD_PVP_COMBOEXERCISE

class CDnMutatorComboExerciseMode : public CDnMutatorTeamGame
{	
public:	
	CDnMutatorComboExerciseMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorComboExerciseMode();

	// Override - CDnMutatorGame -
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	//virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual void GameStart( int nObjectiveUIString, float nRemainCountDown, float RemainSec, bool IsInGameJoin, float nMaxSec );
	//virtual void EndGame( void * pData );
	//virtual void RestartGame(){};
	void ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin, void * pData );
	virtual IScoreSystem*	CreateScoreSystem();


public:
	void Release(){}
};

#endif // PRE_ADD_PVP_COMBOEXERCISE