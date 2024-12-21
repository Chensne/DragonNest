#pragma once

#include "DnNormalMonsterActor.h"
class CDnSimpleRushMonsterActor : public CDnNormalMonsterActor {
public:
	CDnSimpleRushMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnSimpleRushMonsterActor();
};