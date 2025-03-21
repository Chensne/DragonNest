#pragma once

#include "DnMonsterActor.h"
#include "MAWalkMovement.h"
#include "MAWalkMovementNav.h"
#include "MASingleBody.h"
#include "MASingleDamage.h"

class CDnNormalMonsterActor : public TDnMonsterActor< MASingleBody, MASingleDamage >
{
public:
	CDnNormalMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnNormalMonsterActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAWalkMovementNav();
		return pMovement;
	}

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	virtual void SwapActor( int nActorIndex ) override;
#endif

protected:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
	virtual void SetPosition( EtVector3 &vPos );
};