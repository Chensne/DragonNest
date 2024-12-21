
#pragma once

#include "PvPScoreSystem.h"

class CPvPZombieScoreSystem:public CPvPScoreSystem,public TBoostMemoryPool<CPvPZombieScoreSystem>
{
public:

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void OnLeaveUser( DnActorHandle hActor );
	virtual UINT GetZombieKillCount( DnActorHandle hActor );
	virtual void OnFinishRound( DnActorHandle hActor, const bool bIsWin );
#if defined( _GAMESERVER )
	virtual void SendScore( CDNUserSession* pGameSession );
	virtual int	 GetModeExp( DnActorHandle hActor );
#else
	virtual bool SetZombieKillCount( DnActorHandle hActor, const void* pData );
#endif // #if defined( _GAMESERVER )

	int GetHumanWinCount( const WCHAR* pwszCharName );	

protected:

	std::map<std::wstring,UINT>	m_mZombieKillCount;
	std::map<std::wstring,int>	m_mHumanWinCount;

private:
	
#if defined( _GAMESERVER )
	void _SendZombieKillCount( CDNUserSession* pSession );
#endif // #if defined( _GAMESERVER )	
	void _UpdateZombieKillCount( DnActorHandle hActor, UINT uiCount=0 );
};

