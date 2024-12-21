
#pragma once

#include "PvPScoreSystem.h"

class CPvPCaptainScoreSystem:public CPvPScoreSystem,public TBoostMemoryPool<CPvPCaptainScoreSystem>
{
public:

	virtual void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
	virtual void OnLeaveUser( DnActorHandle hActor );
	virtual UINT GetCaptainKillCount( DnActorHandle hActor );
#if defined( _GAMESERVER )
	virtual void SendScore( CDNUserSession* pGameSession );
#else
	virtual bool SetCaptainKillCount( DnActorHandle hActor, const void* pData );
#endif // #if defined( _GAMESERVER )

protected:

	std::map<std::wstring,UINT>	m_mCaptainKillCount;

private:

	void _UpdateCaptainKillCount( DnActorHandle hActor, UINT uiCount=0 );
#if defined( _GAMESERVER )
	void _SendCaptainKillCount( CDNUserSession* pSession );
#endif // #if defined( _GAMESERVER )
};
