
#pragma once

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

class IReputationSystemEventHandler;

typedef INT64 REPUTATION_TYPE;
#ifdef _CLIENT
typedef INT64 UNIONPT_TYPE;
#endif // _CLIENT

// InterfaceClass
class IReputationSystem
{
public:

	IReputationSystem():m_pEventHandler(NULL)
	{
	}

	IReputationSystem( IReputationSystemEventHandler* pHandler ):m_pEventHandler(pHandler)
	{
	}

	virtual ~IReputationSystem(){}

	enum eType
	{
		NpcFavor	= 1,	// Npc È£ÀÇ
		NpcMalice	= 2,	// Npc ¾ÇÀÇ
	};

	// templateMethod PT
	void Add( const eType Type, const REPUTATION_TYPE value, bool bIsReachMax );
	void Set( const eType Type, const REPUTATION_TYPE value );

	//#############################################################################################
	// Overiding Function
	//#############################################################################################
	virtual void			Init()													= 0;
	virtual	REPUTATION_TYPE	Get( const eType Type )									= 0;
	virtual	REPUTATION_TYPE	GetMax( const eType Type )								= 0;
	virtual	float			GetPercent( const eType Type )							= 0;
	virtual	bool			IsComplete( const eType Type )							= 0;
	
private:

	virtual void			_Add( const eType Type, const REPUTATION_TYPE value, bool bIsReachMax )	= 0;
	virtual void			_Set( const eType Type, const REPUTATION_TYPE value )	= 0;
	//#############################################################################################
	// Overiding Function
	//#############################################################################################

	IReputationSystemEventHandler* m_pEventHandler;
};

// Null-InterfaceClass
class CNullReputationSystem:public IReputationSystem
{
public:

	virtual		REPUTATION_TYPE	Get( const eType Type ){ return 0; }
	virtual		REPUTATION_TYPE	GetMax( const eType Type ){ return 0; }
	virtual		float			GetPercent( const eType Type ){ return 0.f; }
	virtual		bool			IsComplete( const eType Type ){ return false; }
};

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
