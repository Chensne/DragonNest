#include "StdAfx.h"
#include "DnShooterProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



CDnShooterProp::CDnShooterProp(void) : CDnDamageBase( DamageObjectTypeEnum::Prop )
{
}

CDnShooterProp::~CDnShooterProp(void)
{
}


bool CDnShooterProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	bool bResult = false;
	if( GetData() )
	{
		ProjectileShooterStruct* pStruct = (ProjectileShooterStruct*)GetData();
		int						 nMonsterTableID = pStruct->nMonsterTableID;

//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( nMonsterTableID );
#endif

		// NOTE: 클라에서는 그래픽만 표시해주면 되니깐.. 
		// 이거 맞아서 상태효과 액터에 넣는 패킷 받았을 때 이 몬스터 객체의 MASkillUser 의 FindSkill로 스킬을 찾으므로 
		// 스킬을 Add 시켜 놓는다.
		if( bResult )
		{
			//m_hMonster->AddSkill( pStruct->nSkillTableID, pStruct->nSkillLevel );

			// 일단 액션을 취하지 않도록한다. 슈터 프랍은 클라와 시작 시간을 맞춰줘야 한다..
			SetAction( "Idle", 0.0f, 0.0f );
		}
	}

	return bResult;
}

bool CDnShooterProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}


void CDnShooterProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	if( m_hMonster )
	{
		*(m_hMonster->GetMatEx()) = m_matExWorld;
		m_hMonster->OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}
}