#include "stdafx.h"
#include "DnNormalNestBossMonsterActor.h"

CDnNormalNestBossMonsterActor::CDnNormalNestBossMonsterActor( int nClassID, bool bProcess )
: CDnGiantMonsterActor( nClassID, bProcess )
{
	m_HitCheckType = HitCheckTypeEnum::Collision;
}

CDnNormalNestBossMonsterActor::~CDnNormalNestBossMonsterActor()
{
}