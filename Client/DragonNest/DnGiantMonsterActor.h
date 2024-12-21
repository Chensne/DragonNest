#pragma once

#include "DnNormalMonsterActor.h"

class CDnGiantMonsterActor : public CDnNormalMonsterActor
{
public:
	CDnGiantMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnGiantMonsterActor();

protected:
	float m_fShakeDelta;

public:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
};