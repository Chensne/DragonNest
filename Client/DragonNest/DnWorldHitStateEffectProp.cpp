#include "StdAfx.h"
#include "DnWorldHitStateEffectProp.h"


CDnWorldHitStateEffectProp::CDnWorldHitStateEffectProp() : CDnDamageBase( DamageObjectTypeEnum::Prop )
{		
}

CDnWorldHitStateEffectProp::~CDnWorldHitStateEffectProp(void)
{	
	ReleasePostCustomParam();
}


bool CDnWorldHitStateEffectProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) 
		return false;

	if( m_hMonster ) 
		*m_hMonster->GetMatEx() = *GetMatEx();

	return true;
}

bool CDnWorldHitStateEffectProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) 
		return false;

	if( GetData() )
	{
		HitStateEffectStruct *pStruct = static_cast< HitStateEffectStruct * >( GetData() );
		bool bResult = false;
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( pStruct->MonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( pStruct->MonsterTableID );
#endif

		if( !bResult )
			return false;
	}

	return true;
}


//void CDnWorldHitStateEffectProp::_OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta )
//{
//
//}