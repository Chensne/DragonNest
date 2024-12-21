#pragma once

#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnMutatorTeamGame.h"

#if defined( PRE_ADD_RACING_MODE )

namespace RacingModeString
{
	enum eGameMessage{
		UIStringVictory = 120301,
		UIStringDefeated = 120302,
		UIStringDraw =	120311,
		UIStringStarting =	121021,
		UIStringIndividualDraw = 120068,
		UIStringIndividualVictory = 120065,
	};
}

class CDnMutatorRacingMode : public CDnMutatorTeamGame
{
public:
	enum
	{
		eWARNING_TIME = 21,
	};

public:
	CDnMutatorRacingMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorRacingMode();

protected:
	int m_nWarningTime;
	bool m_bLocalPlayerGoal;

public:
	void LocalPlayerGoal()	{ m_bLocalPlayerGoal = true; }
	void SetLapTime( const SCPVP_RACING_RAPTIME * pData );
	bool IsWarning( int & nRemainSec );

public:
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec);
	virtual void EndGame( void * pData );

	// ScoreSystem
	virtual IScoreSystem*	CreateScoreSystem();
};

#endif	// #if defined( PRE_ADD_RACING_MODE )