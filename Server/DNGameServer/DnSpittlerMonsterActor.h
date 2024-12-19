
#pragma once

#include "DnNormalMonsterActor.h"

class CDnSpittlerMonsterActor : public CDnNormalMonsterActor , public TBoostMemoryPool< CDnSpittlerMonsterActor >
{
public:
	CDnSpittlerMonsterActor( CMultiRoom *pRoom, int nClassID ):CDnNormalMonsterActor( pRoom, nClassID ){}
	virtual ~CDnSpittlerMonsterActor(){}

	virtual bool OnAINonTarget()
	{
		if( IsUnderGround() )
			return true;

		CmdAction( "Summon_Off", 0 );
		return true;
	}
};
