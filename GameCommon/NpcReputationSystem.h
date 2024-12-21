
#pragma once

#include "ReputationSystem.h"

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

typedef std::pair<REPUTATION_TYPE,REPUTATION_TYPE>	REPUTATION;	// <Cur,Max>

class CNpcReputationSystem:public IReputationSystem,public TBoostMemoryPool<CNpcReputationSystem>
{
public:

	CNpcReputationSystem( const int iNpcID, IReputationSystemEventHandler* pHandler );
	virtual ~CNpcReputationSystem(){}

	//#############################################################################################
	// Overiding Function
	//#############################################################################################
	virtual void			Init();
	virtual	REPUTATION_TYPE	Get( const eType Type );
	virtual	REPUTATION_TYPE	GetMax( const eType Type );
	virtual	float			GetPercent( const eType Type );
	virtual	bool			IsComplete( const eType Type );

	int	GetNpcID(){ return m_iNpcID; }

private:

	virtual void			_Add( const eType Type, const REPUTATION_TYPE value, bool bIsReachMax );
	virtual void			_Set( const eType Type, const REPUTATION_TYPE value );
	//#############################################################################################
	// Overiding Function
	//#############################################################################################

	int			m_iNpcID;
	REPUTATION	m_Favor;
	REPUTATION	m_Malice;
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )