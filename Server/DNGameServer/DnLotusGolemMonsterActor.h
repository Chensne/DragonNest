#pragma once


#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "DnPartsMonsterActor.h"

class CDnLotusGolemMonsterActor : public CDnPartsMonsterActor, public TBoostMemoryPool< CDnLotusGolemMonsterActor >
{
public:
	CDnLotusGolemMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnLotusGolemMonsterActor();

	virtual MAMovementBase* CreateMovement();

protected:

	float m_fShakeDelta;

public:
	virtual bool Initialize();

	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct );

};

