
#pragma once

#include "PvPGameMode.h"

#if defined( PRE_ADD_PVP_COMBOEXERCISE )

class CPvPComboExerciseMode:public CPvPGameMode
{
public:

	CPvPComboExerciseMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPComboExerciseMode();

	virtual bool bIsComboExerciseMode() { return true; }
	virtual void OnInitializeActor( CDNUserSession* pSession );
	virtual void OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );
	virtual int OnRecvPvPMessage( LOCAL_TIME LocalTime, CDNUserSession * pSession, int nSubCmd, char *pData, int nLen );

private:
	UINT m_nLastRecallMonsterTick;
};

class IBoostPoolPvPComboExerciseMode : public CPvPComboExerciseMode, public TBoostMemoryPool<IBoostPoolPvPComboExerciseMode>
{
public :
	IBoostPoolPvPComboExerciseMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
		:CPvPComboExerciseMode(pGameRoom,pPvPGameModeTable,pPacket){}
	virtual ~IBoostPoolPvPComboExerciseMode(){}
};

#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )