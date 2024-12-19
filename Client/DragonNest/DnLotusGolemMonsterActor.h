#pragma once

#include "DnMonsterActor.h"
#include "DnPartsMonsterActor.h"
#include "MAStaticLookMovement.h"

#ifdef PRE_ADD_LOTUSGOLEM

class CDnLotusGolemMonsterActor : public CDnPartsMonsterActor
{
public:
	CDnLotusGolemMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnLotusGolemMonsterActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new IBoostPoolMAStaticMovement();
		return pMovement;
	}

public:
	virtual bool Initialize();
	
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};

#endif // PRE_ADD_LOTUSGOLEM