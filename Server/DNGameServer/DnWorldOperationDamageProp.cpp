#include "stdafx.h"
#include "DnWorldOperationDamageProp.h"
#include "DnPartyTask.h"
#include "DNUserSession.h"
#include "DnMonsterActor.h"

CDnWorldOperationDamageProp::CDnWorldOperationDamageProp( CMultiRoom *pRoom ) : CDnWorldOperationProp( pRoom ),
																				CDnDamageBase( DamageObjectTypeEnum::Prop )
{

}

CDnWorldOperationDamageProp::~CDnWorldOperationDamageProp()
{
	
}

bool CDnWorldOperationDamageProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( __super::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) return false;
	if( m_hMonster ) *m_hMonster->GetMatEx() = *GetMatEx();
	return true;
}

bool CDnWorldOperationDamageProp::InitializeTable( int nTableID )
{
	if( __super::InitializeTable( nTableID ) == false ) return false;

	bool bResult = false;

	if( GetData() )
	{
		TrapStruct *pStruct = (TrapStruct *)GetData();
		int nMonsterTableID = pStruct->nMonsterTableID;
		
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( nMonsterTableID );
#endif

		if( bResult )
		{			
			m_HitComponent.Initialize( m_hMonster, &m_Cross, GetMySmartPtr(), this );
		}
	}

	return bResult;
}


void CDnWorldOperationDamageProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, 
								 LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	m_HitComponent.OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	CDnWorldActProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}