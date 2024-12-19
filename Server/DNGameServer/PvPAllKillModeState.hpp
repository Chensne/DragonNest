
#pragma once

class CPvPAllKillMode;
class CDNPvPGameRoom;

class IPvPAllKillModeState
{
public:

	IPvPAllKillModeState( CPvPAllKillMode* pMode )
		:m_pMode(pMode),m_pPvPGameRoom(pMode->GetGameRoom()->bIsPvPRoom()?reinterpret_cast<CDNPvPGameRoom*>(pMode->GetGameRoom()):NULL),m_dwBeginTick(timeGetTime())
	{
	}

	virtual ~IPvPAllKillModeState(){}

	virtual CPvPAllKillMode::eState GetState() = 0;
	virtual void BeginState() = 0;
	virtual void BeforeProcess( float fDelta ) = 0;
	virtual void AfterProcess( float fDelta ) = 0;
	virtual void EndState() = 0;
	virtual void OnLeaveUser( DnActorHandle hActor ) = 0;
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter ) = 0;
	virtual UINT GetRemainStateTick(){ return 0; }

	CDNPvPGameRoom* GetPvPGameRoom() const { return m_pPvPGameRoom; }
	CPvPAllKillMode* GetModePtr() const { return m_pMode; }
	DWORD GetElapsedTick() const { return timeGetTime()-m_dwBeginTick; }

protected:

	CDNPvPGameRoom* m_pPvPGameRoom;
	CPvPAllKillMode* m_pMode;
	DWORD m_dwBeginTick;
};

#include "PvPAllKillModeNoneState.hpp"
#include "PvPAllKillModeSelectPlayerState.hpp"
#include "PvPAllKillModeStartingState.hpp"
#include "PvPAllKillModePlayingState.hpp"
#include "PvPAllKillModeFinishRoundState.hpp"

