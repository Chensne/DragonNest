#include "StdAfx.h"
#include "DnWorldShooterBrokenProp.h"
#include "DnDropItem.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldShooterBrokenProp::CDnWorldShooterBrokenProp(void)
{
}

CDnWorldShooterBrokenProp::~CDnWorldShooterBrokenProp(void)
{
	ReleasePostCustomParam();
}


bool CDnWorldShooterBrokenProp::InitializeTable( int nTableID )
{
	// 각 프랍 데이터 구조체 별로 초기화 해야하는 거라 바로 부모 클래스의 코드를 
	// 사용할 수가 없다. ㅠㅠ
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) 
		return false;

	LoadHitEffectData( nTableID );
	m_bIsStaticCollision = false;

	if( GetData() ) {
		ShooterBrokenStruct* pStruct = (ShooterBrokenStruct *)GetData();
		bool				 bResult = false;
	
//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );
#endif

		int nDurability				= pStruct->nDurability;
		int nItemDropGroupTableID	= pStruct->nItemDropGroupTableID;
		int nMonsterTableID			= pStruct->nMonsterTableID;
		int nSkillTableID			= pStruct->nSkillTableID;
		int nSkillLevel				= pStruct->nSkillLevel;
	
		if( bResult )
		{
			m_nDurability = pStruct->nDurability;
			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) {
					CDnDropItem::CalcDropItemList( -1, m_nItemDropGroupTableID, m_VecDropItemList );

					for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
						if( CDnDropItem::PreInitializeItem( m_VecDropItemList[i].nItemID ) == false ) {
							m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
							i--;
						}
					}
				}
			}

			// 일단 액션을 취하지 않도록한다. 슈터 프랍은 클라와 시작 시간을 맞춰줘야 한다..
			SetAction( "Idle", 0.0f, 0.0f );
		}
	}

	return true;
}

bool CDnWorldShooterBrokenProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldBrokenProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}


void CDnWorldShooterBrokenProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	if( m_hMonster )
	{
		*(m_hMonster->GetMatEx()) = m_matExWorld;
		m_hMonster->OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}

	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}