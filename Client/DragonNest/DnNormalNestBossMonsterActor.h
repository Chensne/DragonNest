#pragma once

#include "DnGiantMonsterActor.h"

class CDnNormalNestBossMonsterActor : public CDnGiantMonsterActor
{
public:
	CDnNormalNestBossMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnNormalNestBossMonsterActor();
};