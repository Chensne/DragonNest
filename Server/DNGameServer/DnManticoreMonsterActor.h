#pragma once


#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"

#include "DnPartsMonsterActor.h"

class CDnManticoreMonsterActor : public CDnPartsMonsterActor, public TBoostMemoryPool< CDnManticoreMonsterActor >
{
public:
	CDnManticoreMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnManticoreMonsterActor();

protected:
	float m_fShakeDelta;

	LOCAL_TIME m_nIgnoreNormalSuperArmorTime;

public:
	virtual bool Initialize();

	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );

	virtual bool CheckDamageAction( DnActorHandle hActor );
	virtual void OnDrop( float fCurVelocity );
};

