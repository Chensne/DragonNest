#pragma once
#include "DnMutatorTeamGame.h"

/////////////////////////////////////////////////////////////////////////
//
// 길드전이라고 이름은 지었지만 점령전이 정확한 명칭이다.
//
//////////////////////////////////////////////////////////////////////////

namespace GuildWarModeString
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
		UIStringTimeVictory = 126264,
		UIStringTimeDefeated = 126265,
		WaitRespawn = 121014,
	};
}
class CDnOccupationTask;
class CDnMutatorGuildWarMode : public CDnMutatorTeamGame, public CEtUICallback
{
public:
	CDnMutatorGuildWarMode( CDnPvPGameTask * pGameTask );
	virtual ~CDnMutatorGuildWarMode();

public:
	virtual bool ProcessActorDie( DnActorHandle hKillerActor , DnActorHandle hKilledActor );
	virtual int  Scoring( int &team ){return 0;}
	virtual bool Initialize( const UINT uiItemID, DNTableFileFormat*  pSox );
	virtual void GameStart( int nObjectiveUIString, float nRemainCountDown, float RemainSec, bool IsInGameJoin, float nMaxSec );
	virtual void EndGame( void * pData );
	virtual void OnPostInitializeStage();
	virtual void ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin = false, void * pData = NULL );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual IScoreSystem*	CreateScoreSystem();

	void	PushUsedRespawnArea( const std::string& name );
	bool	bIsUsedRespawnArea( const std::string& name );
	void	ProcessObserver();
	CDnMonsterActor * GetDoorActor( int nDoorID );
private:

	void _RenderGameModeScore();
	void RequestImmediateRebirth();

	std::list<std::string>	m_UsedRespawnAreaList;
	float m_fRespawnTime;
	bool m_bRespawnMessage;

	CDnOccupationTask * m_pOccupationTask;
};
