
#pragma once

#include "PvPGameMode.h"

class CPvPOccupactionSystem;

class CPvPOccupationMode : public CPvPGameMode
{
public:

	CPvPOccupationMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPOccupationMode();
	
	virtual bool bIsOccupationMode() { return true; }
	virtual bool bIsGuildWarSystem() { return m_bGuildWar; }	
	virtual CPvPOccupactionSystem * GetOccupationSystem() { return m_pOccupationSystem; }
	virtual bool GetOccupationTeamScore(int nTeam, int &nScore);

	virtual bool SetForceWinGuild(UINT nWinGuildDBID);

	//Process
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	//Init
	virtual bool InitializeOccupationSystem(TBattleGourndModeInfo * pModeInfo);
	virtual bool InitializeOccupationMode(TOccupationModeInfo * pInfo);

	//Recv
	virtual int OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );

	//On
	virtual void OnFinishProcessDie( DnActorHandle hActor );
	virtual void OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );
	virtual void OnSuccessBreakInto( CDNUserSession* pGameSession );
	bool OnTryAcquirePoint(DnActorHandle hActor, int nAreaID, LOCAL_TIME LocalTime);
	virtual bool OnCancelAcquirePoint(DnActorHandle hActor);	

	//Game
	virtual void OnRebirth( DnActorHandle hActor, bool bForce=false );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage );

	virtual bool bIsBoss(DnActorHandle hActor);	

protected:

private:
	bool m_bGuildWar;		//시스템에 의한 생성인지?
	CPvPOccupactionSystem * m_pOccupationSystem;
	TOccupationModeInfo m_ModeInfo;

	void _RebirthProcess(DnActorHandle hActor, bool bUseCurPos = false);
};

class IBoostPoolPvPOccupationMode : public CPvPOccupationMode, public TBoostMemoryPool<IBoostPoolPvPOccupationMode>
{
public :
	IBoostPoolPvPOccupationMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
		:CPvPOccupationMode(pGameRoom,pPvPGameModeTable,pPacket){}
	virtual ~IBoostPoolPvPOccupationMode(){}
};
