#include "StdAfx.h"
#include "DnWorldBrokenBuffProp.h"
#include "DnDropItem.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldBrokenBuffProp::CDnWorldBrokenBuffProp(void)
{
}

CDnWorldBrokenBuffProp::~CDnWorldBrokenBuffProp(void)
{
	ReleasePostCustomParam();
}


bool CDnWorldBrokenBuffProp::InitializeTable( int nTableID )
{
	// 각 프랍 데이터 구조체 별로 초기화 해야하는 거라 바로 부모 클래스의 코드를 
	// 사용할 수가 없다. ㅠㅠ
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) 
		return false;

	LoadHitEffectData( nTableID );
	m_bIsStaticCollision = false;

	if( GetData() ) {
		BuffBrokenStruct *pStruct = (BuffBrokenStruct *)GetData();
		bool bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );

		// 잘못된 스킬 정보
		int nSkillTableID = pStruct->nSkillTableID;
		int nSkillLevel = pStruct->nSkillLevel;
		_ASSERT( nSkillTableID != 0 && nSkillLevel != 0 );
		if( nSkillTableID == 0 || nSkillLevel == 0 )
			return false;

		if( bResult ) {
			//m_SkillComponent.Initialize( nSkillTableID, nSkillLevel );
//			m_BuffPropComponent.Initialize( GetRoom(), pStruct->nTeam, nSkillTableID, nSkillLevel );
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
		}
	}

	return true;
}

bool CDnWorldBrokenBuffProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldBrokenProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult && m_hMonster ) {
		*m_hMonster->GetMatEx() = *GetMatEx();
		m_hMonster->SetActionSignalScale( *GetActionSignalScale() );
	}
	return bResult;
}
