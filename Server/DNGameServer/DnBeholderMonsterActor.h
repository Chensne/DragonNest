#pragma once


#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"

#include "DnPartsMonsterActor.h"

class CDnBeholderMonsterActor : public CDnPartsMonsterActor, public TBoostMemoryPool< CDnBeholderMonsterActor >
{
public:
	CDnBeholderMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnBeholderMonsterActor();

protected:
	float m_fShakeDelta;
	DnActorHandle	m_DummyBeholderEye;

public:
	virtual bool Initialize();

	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );
};