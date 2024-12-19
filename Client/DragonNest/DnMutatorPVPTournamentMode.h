#pragma once

#pragma once
#include "DnDamageBase.h"
#include "DnActor.h"
#include "DnMutatorTeamGame.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

namespace PVPTournamentModeString
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

class CDnPVPTournamentDataMgr;
class CDnMutatorPVPTournamentMode : public CDnMutatorGame
{
public:
	CDnMutatorPVPTournamentMode(CDnPvPGameTask * pGameTask);
	virtual ~CDnMutatorPVPTournamentMode();

	virtual int Scoring(int& team) { return 0; }
	virtual void ProcessEnterUser(DnActorHandle hActor, bool isIngameJoin = false, void* pData = NULL);
	virtual bool ProcessActorDie(DnActorHandle hKillerActor , DnActorHandle hKilledActor);
	virtual bool Initialize(const UINT uiItemID, DNTableFileFormat* pSox);

	virtual void GameStart(int nObjectiveUIString, float nRemainCountDown, float RemainSec, bool IsInGameJoin, float nMaxSec);
	virtual void EndGame(void* pData);
	virtual void RoundEnd(void* pData );
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);

	virtual void RestartGame();

	virtual IScoreSystem* CreateScoreSystem();

	void SetFinishDetailReason(int nUIStringIndex);
	void SetInBattleSessionIDs(UINT blue, UINT red);
	int GetInBattleSessionIDsIndex(UINT sessionId) const;
	void ChangeActorHair(DnActorHandle hActor);

	CDnPVPTournamentDataMgr* GetTournamentDataMgr() const;
	void OnDefaultWin(const SCPvPTournamentDefaultWin& data);

protected:
	void _RenderGameModeScore();

	bool m_bFirstRound;
	int m_iEnemyUserCount;
	int m_iFriendyUserCount;

	int m_iFinishDetailReason;
	float m_fFinishDetailReasonShowDelay;
	float m_fCountDownTime;
};

#endif // PRE_ADD_PVP_TOURNAMENT