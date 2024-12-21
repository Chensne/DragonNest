
#pragma once

#include "PvPRespawnMode.h"

class CPvPIndividualRespawnMode:public CPvPRespawnMode,public TBoostMemoryPool<CPvPIndividualRespawnMode>
{
public:

	CPvPIndividualRespawnMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket );
	virtual ~CPvPIndividualRespawnMode();

	virtual bool bIsIndividualMode(){ return true; }

	virtual void OnInitializeActor( CDNUserSession* pSession );
	virtual UINT OnCheckZeroUserWinTeam( const bool bCheckBreakIntoUser=true );
	virtual UINT OnCheckFinishWinTeam();
	virtual void OnLeaveUser( DnActorHandle hActor );
	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );

	void	PushUsedRespawnArea( const std::string& name );
	bool	bIsUsedRespawnArea( const std::string& name );

	long m_lDieCount;	// #22708 이슈와 관련해서 개인전에서 죽은 카운트 계산해서 중복 리스폰 되지 않게 처리하는 변수로 사용

protected:

	std::list<int>			m_UsableTeamIndex;
	std::list<std::string>	m_UsedRespawnAreaList;
};
