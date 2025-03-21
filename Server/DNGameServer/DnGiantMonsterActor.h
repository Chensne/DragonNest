#pragma once

#include "DnNormalMonsterActor.h"

class CDnGiantMonsterActor : public CDnNormalMonsterActor, public TBoostMemoryPool< CDnGiantMonsterActor >
{
public:
	CDnGiantMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnGiantMonsterActor();

protected:
	float m_fShakeDelta;

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );
};