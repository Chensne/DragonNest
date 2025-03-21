#include "StdAfx.h"
#include "DnWorldOperationDamageProp.h"
#include "DnTableDB.h"
#include "DnMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldOperationDamageProp::CDnWorldOperationDamageProp()
: CDnDamageBase( DamageObjectTypeEnum::Prop )
{
}

CDnWorldOperationDamageProp::~CDnWorldOperationDamageProp()
{
	
}

bool CDnWorldOperationDamageProp::InitializeTable( int nTableID )
{
	if( __super::InitializeTable( nTableID ) == false ) 
		return false;

	bool bResult = false;

	if( GetData() )
	{
		OperationDamageStruct *pStruct = (OperationDamageStruct *)GetData();
		int nMonsterTableID = pStruct->nMonsterTableID;

//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( nMonsterTableID );
#endif

	}

	return bResult;
}

bool CDnWorldOperationDamageProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = __super::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}

