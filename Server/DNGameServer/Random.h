#pragma once

#include "MtRandom.h"
#include "MultiSingleton.h"
#include "MultiCommon.h"
#include "MemPool.h"

class CMultiRoom;

class CRandom : public CMtRandom , public CMultiSingleton< CRandom, MAX_SESSION_COUNT >, public TBoostMemoryPool<CRandom>
{
public:
	CRandom( CMultiRoom *pRoom ) : CMultiSingleton<CRandom, MAX_SESSION_COUNT>( pRoom ) {}
	virtual ~CRandom() {}

	static int Seed( CMultiRoom *pRoom );
	static void LockSeed( CMultiRoom *pRoom );
	static void UnlockSeed( CMultiRoom *pRoom );

protected:
	STATIC_DECL( int s_nStaticCount );
	STATIC_DECL( int s_nLastSeed );
	STATIC_DECL( int s_nLockSeed );
	STATIC_DECL( int s_nLockLeft );
	STATIC_DECL( int s_nLockInitf );
	STATIC_DECL( unsigned long *s_pNext );
};