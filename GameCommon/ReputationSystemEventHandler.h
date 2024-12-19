
#pragma once

#include "ReputationSystem.h"

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

class IReputationSystemEventHandler
{
public:

	virtual ~IReputationSystemEventHandler(){}

	virtual	void OnComplete( IReputationSystem* pReputation, IReputationSystem::eType Type ) = 0;
	virtual void OnChange( IReputationSystem::eType Type ) = 0;
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
	virtual void OnCommit( std::map<IReputationSystem*,size_t>& Reputation, bool bClientSend ) = 0;
#else
	virtual void OnCommit( std::map<IReputationSystem*,int>& Reputation, bool bClientSend ) = 0;
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
	virtual void OnConnect( __time64_t tTime, bool bSend ) = 0;
};

// Null-Interface Class
class CNullReputationSystemEventHandler:public IReputationSystemEventHandler,public TBoostMemoryPool<CNullReputationSystemEventHandler>
{
public:

	virtual ~CNullReputationSystemEventHandler(){}

	virtual	void OnComplete( IReputationSystem* pReputation, IReputationSystem::eType Type ){}
	virtual void OnChange( IReputationSystem::eType Type ){}
#if defined( PRE_ADD_REPUTATION_EXPOSURE )
	virtual void OnCommit( std::map<IReputationSystem*,size_t>& Reputation, bool bClientSend ){}
#else
	virtual void OnCommit( std::map<IReputationSystem*,int>& Reputation, bool bClientSend ){}
#endif // #if defined( PRE_ADD_REPUTATION_EXPOSURE )
	virtual void OnConnect( __time64_t tTime, bool bSend ){}
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
