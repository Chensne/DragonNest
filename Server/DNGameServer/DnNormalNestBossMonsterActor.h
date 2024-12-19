#pragma once

#include "DnGiantMonsterActor.h"

class CDnNormalNestBossMonsterActor : public CDnGiantMonsterActor
{
public:
	CDnNormalNestBossMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnNormalNestBossMonsterActor();
};