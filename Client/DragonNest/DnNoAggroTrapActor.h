
#pragma once

#include "DnMonsterActor.h"
#include "MAStaticMovement.h"
#include "MASingleBody.h"
#include "MASingleDamage.h"

class CDnNoAggroTrapActor : public TDnMonsterActor<MASingleBody, MASingleDamage>
{
public:
	CDnNoAggroTrapActor( int nClassID, bool bProcess = true );
	virtual ~CDnNoAggroTrapActor();

	virtual MAMovementBase* CreateMovement()
	{
		MAMovementBase* pMovement = new IBoostPoolMAStaticMovement();
		return pMovement;
	}
};