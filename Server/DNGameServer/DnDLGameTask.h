#pragma once
#include "DnGameTask.h"

class CDLItemRespawnLogic;

class CDnDLGameTask : public CDnGameTask
{
public:
	CDnDLGameTask( CMultiRoom *pRoom );
	virtual ~CDnDLGameTask();

	enum ChallengeResponseEnum 
	{
		Response_None = 0,
		Response_Challenge,
		Response_Deny,
	};

protected:
	int m_nTotalRound;
	int m_nCurrentRound;
#if defined( PRE_ADD_CHALLENGE_DARKLAIR )
	bool m_bChallengeDarkLair;
#endif	// #if defined( PRE_ADD_CHALLENGE_DARKLAIR )
	TDLRankHistoryPartyInfo m_CurrentRankInfo;
	bool m_bCheckDungeonClear;
	ChallengeResponseEnum m_ChallengeResponseEnum;
#ifdef PRE_MOD_DARKLAIR_RECONNECT
	bool m_bBossRound;
#endif // PRE_MOD_DARKLAIR_RECONNECT

protected:
	void ResetRound( bool bMakeQueryData = false );

	void MakeHistoryInfo( TDLRankHistoryPartyInfo *pInfo, SDarkLairHistory *pHistory );
	void MakeHistoryInfo( TDLRankHistoryPartyInfo *pInfo, SDarkLairBestHistory *pHistory );

public:
	int GetRound() { return m_nCurrentRound; }
	int GetTotalRound() { return m_nTotalRound; }
	void SetStartRound( int iRound );
#ifdef PRE_MOD_DARKLAIR_RECONNECT
	void UpdateRound( int iNextTotalRound, bool bBoss );
#else // PRE_MOD_DARKLAIR_RECONNECT
	void UpdateRound( int iNextTotalRound );
#endif // PRE_MOD_DARKLAIR_RECONNECT
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	bool IsChallengeDarkLair() { return m_bChallengeDarkLair; }
#endif

	virtual bool InitializeStage( int nCurrentMapIndex, int nGateIndex, TDUNGEONDIFFICULTY StageDifficulty, int nRandomSeed, bool bContinueStage, bool bDirectConnect, int nGateSelect );
	virtual bool InitializeNextStage( const char *szGridName, int nMapTableID, TDUNGEONDIFFICULTY StageDifficulty, int nStartPositionIndex );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool PostInitializeStage( int nRandomSeed );
	virtual void OnGhost( DnActorHandle hActor );

	virtual void RequestDungeonClear( bool bClear = true, DnActorHandle hIgnoreActor = CDnActor::Identity(), bool bIgnoreRewardItem = false );

	virtual void ChangeDungeonClearState( DungeonClearStateEnum State );
	virtual bool ProcessDungeonClearState( LOCAL_TIME LocalTime, float fDelta );

	virtual int OnRecvRoomSelectRewardItem( CDNUserSession * pSession, CSSelectRewardItem *pPacket );

	void CheckAndRequestDungeonClear( DnActorHandle hActor = CDnActor::Identity() );

	void ClearChallengeResponse(){ m_ChallengeResponseEnum = Response_None; }
	ChallengeResponseEnum GetChallengeResponse() const { return m_ChallengeResponseEnum; }
	virtual int OnRecvRoomDLChallengeResponse( CDNUserSession* pSession, CSDLChallengeResponse* pPacket );
	int GetStartFloor();
#if defined(PRE_MOD_DARKLAIR_RECONNECT)
	virtual bool OnInitializeBreakIntoActor( CDNUserSession* pSession, const int iVectorIndex );
#endif // PRE_MOD_DARKLAIR_RECONNECT

private:
	CDLItemRespawnLogic* m_pItemRespawnLogic;
};