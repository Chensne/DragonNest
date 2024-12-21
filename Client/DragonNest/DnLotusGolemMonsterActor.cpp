#include "StdAfx.h"
#include "DnLotusGolemMonsterActor.h"
#include "DnActorClassDefine.h"

#ifdef PRE_ADD_LOTUSGOLEM

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnLotusGolemMonsterActor::CDnLotusGolemMonsterActor( int nClassID, bool bProcess )
:  CDnPartsMonsterActor( nClassID, bProcess )
{
	m_HitCheckType = HitCheckTypeEnum::Collision;
}

CDnLotusGolemMonsterActor::~CDnLotusGolemMonsterActor()
{
}

bool CDnLotusGolemMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();

	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
	m_hObject->SetTargetCollisionGroup( 0 );

	SetUseDamageColor(true);

	return true;
}

void CDnLotusGolemMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPartsMonsterActor::Process( LocalTime, fDelta );
}

#endif // PRE_ADD_LOTUSGOLEM