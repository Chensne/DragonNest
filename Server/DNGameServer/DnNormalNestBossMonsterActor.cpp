#include "stdafx.h"
#include "DnNormalNestBossMonsterActor.h"

CDnNormalNestBossMonsterActor::CDnNormalNestBossMonsterActor( CMultiRoom *pRoom, int nClassID )
: CDnGiantMonsterActor( pRoom, nClassID )
{
	m_HitCheckType = HitCheckTypeEnum::Collision;
}

CDnNormalNestBossMonsterActor::~CDnNormalNestBossMonsterActor()
{
}