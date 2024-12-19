#pragma once

#include "DnMonsterActor.h"
#include "MASingleBody.h"
#include "MAMultiDamage.h"
#include "DnPartsMonsterActor.h"
#include "MAStaticLookMovement.h"

class CDnSeadragonMonsterActor : public CDnPartsMonsterActor
{
public:
	CDnSeadragonMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnSeadragonMonsterActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new MAStaticLookMovement();
		return pMovement;
	}

protected:

public:

	virtual bool Initialize();
	virtual void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time);

#ifdef PRE_FIX_77172
	virtual void CheckDamageVelocity( DnActorHandle hActor, bool bKeepJumpMovement = false );
#else
	virtual void CheckDamageVelocity( DnActorHandle hActor );
#endif

};