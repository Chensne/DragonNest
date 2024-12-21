#include "StdAfx.h"
#include "DnWorldMultiDurabilityProp.h"
#include "DnDropItem.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldMultiDurabilityProp::CDnWorldMultiDurabilityProp(void)
{
}

CDnWorldMultiDurabilityProp::~CDnWorldMultiDurabilityProp(void)
{
	ReleasePostCustomParam();
}


bool CDnWorldMultiDurabilityProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) 
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );

	LoadHitEffectData( nTableID );
	m_bIsStaticCollision = false;

	if( GetData() ) {
		ShooterBrokenStruct *pStruct = (ShooterBrokenStruct *)GetData();
		bool bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );

		int nDurability = pStruct->nDurability;
		int nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
		int nMonsterTableID = pStruct->nMonsterTableID;
		int nSkillTableID = pStruct->nSkillTableID;
		int nSkillLevel = pStruct->nSkillLevel;
	
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

bool CDnWorldMultiDurabilityProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	//bool bResult = CDnWorldBrokenProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	// 클라에선 fsm 셋팅 하지 말고..
	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster )
	{
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}

//
//void CDnWorldMultiDurabilityProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
//{
//	if( m_hMonster )
//	{
//		*(m_hMonster->GetMatEx()) = m_matExWorld;
//		m_hMonster->OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
//	}
//
//	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
//}