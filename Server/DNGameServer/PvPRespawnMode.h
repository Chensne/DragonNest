
#pragma once

#include "PvPGameMode.h"

class CPvPRespawnMode:public CPvPGameMode
{
public:

	CPvPRespawnMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPRespawnMode();

	virtual void OnFinishProcessDie( DnActorHandle hActor );
	virtual void OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
};

class IBoostPoolPvPRespawnMode : public CPvPRespawnMode, public TBoostMemoryPool<IBoostPoolPvPRespawnMode>
{
public :
	IBoostPoolPvPRespawnMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
		:CPvPRespawnMode(pGameRoom,pPvPGameModeTable,pPacket){}
	virtual ~IBoostPoolPvPRespawnMode(){}
};
