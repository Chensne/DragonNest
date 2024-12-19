
#pragma once

#include "DnGameTask.h"

class CPvPRespawnLogic;

class CDnPvPGameTask:public CDnGameTask,public TBoostMemoryPool<CDnPvPGameTask>
{
public:
	CDnPvPGameTask( CDNGameRoom * pRoom );
	virtual ~CDnPvPGameTask();

	int OnDispatchMessage(CDNUserSession *pSession, int nMainCmd, int nSubCmd, char *pData, int nLen );	

protected:
	int OnRecvActorMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );
	int OnRecvPvPMessage( CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );

	void InitializeRespawnArea();
	void InitializeOccupationSystem();

public:
	// Process
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool InitializeStage( int nCurrentMapIndex, int nGateIndex, TDUNGEONDIFFICULTY StageDifficulty, int nRandomSeed, bool bContinueStage, bool bDirectConnect, int nGateSelect );
	virtual bool OnInitializePlayerActor( CDNUserSession* pSession, const int iVectorIndex );

	virtual void OnInitializeStartPosition( CDNUserSession* pSession, const int iPartyIndex );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );

	CPvPRespawnLogic *GetRespawnLogic() { return m_pRespawnLogic; }

	void OnInitializeRespawnPosition( DnActorHandle hActor, const bool bIsStartPosition=false );

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	void RequestRecallAllMonster( EtVector3 &vPos, EtVector2 &vLook );
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

private:

	CPvPRespawnLogic* m_pRespawnLogic;
};
