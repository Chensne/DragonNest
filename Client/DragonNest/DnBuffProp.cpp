#include "StdAfx.h"
#include "DnBuffProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBuffProp::CDnBuffProp(void) : CDnDamageBase( DamageObjectTypeEnum::Prop )
{
}

CDnBuffProp::~CDnBuffProp(void)
{
	ReleasePostCustomParam();
}


bool CDnBuffProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false )
		return false; 

	bool bResult = false;

	if( GetData() )
	{
		BuffStruct* pStruct = (BuffStruct*)GetData();
		int nMonsterTableID = pStruct->nMonsterTableID;
		
		int nSkillTableID = pStruct->nSkillTableID;
		int nSkillLevel = pStruct->nSkillLevel;
		

		bResult = InitializeMonsterActorProp( nMonsterTableID );

		_ASSERT( nSkillTableID != 0 && nSkillLevel != 0 );
		if( nSkillTableID == 0 || nSkillLevel == 0 )
			return false;
	}

	return bResult;
}

bool CDnBuffProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}
