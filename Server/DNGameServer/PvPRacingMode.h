
#pragma once

#if defined( PRE_ADD_RACING_MODE )

#include "PvPGameMode.h"

class CPvPRacingMode:public CPvPGameMode
{
public:

	CPvPRacingMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPRacingMode();

	virtual bool bIsRacingMode() { return true; }

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );	
	virtual void OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason );	

	virtual void SetRacingStart();
	virtual void SetRacingEnd(UINT nSessionID);
	virtual void OnLeaveUser( DnActorHandle hActor );

	virtual BYTE GetRacingRanking(UINT uiSessionID);

private:	
	bool  m_bStartRacing;
	bool  m_bEndRacing;	
	bool  m_bFirstEnd;									// 1등이 들어왔는지
	std::map<UINT, DWORD> m_mStartTick;				// 유저별 시작시간
	std::map<UINT, DWORD> m_mEndTick;					// 유저별 골인시간	
	UINT m_nRacingRanking[PvPCommon::Common::MaxPlayer];  // 순위별 SessionID
	int m_nRacingIndex;	

	void SendRacingRapTime();	
	void SendRacingFirstEnd(UINT uiSessionID);
	bool CheckRacingEnd(DnActorHandle hActor);	
};

class IBoostPoolPvPRacingMode : public CPvPRacingMode, public TBoostMemoryPool<IBoostPoolPvPRacingMode>
{
public :
	IBoostPoolPvPRacingMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
		:CPvPRacingMode(pGameRoom,pPvPGameModeTable,pPacket){}
	virtual ~IBoostPoolPvPRacingMode(){}
};

#endif //#if defined( PRE_ADD_RACING_MODE )
