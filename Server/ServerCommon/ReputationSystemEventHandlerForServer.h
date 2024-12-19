
#pragma once

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

#include "ReputationSystemEventHandler.h"

class CDNUserSession;

class CServerReputationSystemEventHandler:public IReputationSystemEventHandler,public TBoostMemoryPool<CServerReputationSystemEventHandler>
{
public:

	CServerReputationSystemEventHandler( CDNUserSession* pSession );
	virtual ~CServerReputationSystemEventHandler(){}

	virtual	void OnComplete( IReputationSystem* pReputation, IReputationSystem::eType Type );
	virtual void OnChange( IReputationSystem::eType Type );
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
	virtual void OnCommit( std::map<IReputationSystem*,size_t>& Reputation, bool bClientSend );
#else
	virtual void OnCommit( std::map<IReputationSystem*,int>& Reputation, bool bClientSend );
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
	virtual void OnConnect( __time64_t tTime, bool bSend );

private:

	CDNUserSession* m_pSession;
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
