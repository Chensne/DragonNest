#pragma once

#include "DnSimpleRushMonsterActor.h"

class CDnGhoulMonsterActor : public CDnSimpleRushMonsterActor, public TBoostMemoryPool< CDnGhoulMonsterActor >
{
public:
	CDnGhoulMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnGhoulMonsterActor();

protected:
	std::string m_szLastHitDirection;

protected:
	virtual void ProcessStiff( LOCAL_TIME LocalTime, float fDelta );

public:
	// DnActor Message
	virtual bool SetActionQueue( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bCheck = true, bool bCheckStateEffect = true );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );
};
