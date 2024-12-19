#pragma once

#include "DnSimpleRushMonsterActor.h"

class CDnHideMonsterActor : public CDnSimpleRushMonsterActor, public TBoostMemoryPool< CDnHideMonsterActor >
{
public:
	CDnHideMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnHideMonsterActor();

	virtual bool IsHittable( DnActorHandle hHitter, LOCAL_TIME LocalTime, HitStruct *pHitSignal = NULL, int iHitUniqueID = -1 );
};
