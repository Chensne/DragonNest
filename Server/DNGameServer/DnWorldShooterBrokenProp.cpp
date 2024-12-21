#include "StdAfx.h"
#include "DnWorldShooterBrokenProp.h"
#include "DnStateBlow.h"
#include "DnTableDB.h"
#include "DnProjectile.h"
#include "TaskManager.h"
#include "DnGameTask.h"


CDnWorldShooterBrokenProp::CDnWorldShooterBrokenProp( CMultiRoom* pRoom ) : CDnWorldBrokenProp( pRoom )
{

}

CDnWorldShooterBrokenProp::~CDnWorldShooterBrokenProp(void)
{
	ReleasePostCustomParam();
}

bool CDnWorldShooterBrokenProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( CDnWorldBrokenProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) return false;
	if( m_hMonster ) *m_hMonster->GetMatEx() = *GetMatEx();
	return true;
}

bool CDnWorldShooterBrokenProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable(nTableID) == false ) return false;

	bool bResult = false;

	if( GetData() )
	{
		ShooterBrokenStruct*	pStruct = (ShooterBrokenStruct*)GetData();
		int						nDurability			  = pStruct->nDurability;
		int						nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
		int						nMonsterTableID		  = pStruct->nMonsterTableID;
		int						nSkillTableID		  = pStruct->nSkillTableID;
		int						nSkillLevel			  = pStruct->nSkillLevel;

//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( nMonsterTableID, ( pStruct->MonsterWeight ? true : false ) );
#else
		bResult = InitializeMonsterActorProp( nMonsterTableID );
#endif	

		if( bResult )
		{
			m_SkillComponent.Initialize( nSkillTableID, nSkillLevel );

			m_nDurability = nDurability;

			// 일단 액션을 취하지 않도록한다. 슈터 프랍은 클라와 시작 시간을 맞춰줘야 한다..
			SetAction( "Idle", 0.0f, 0.0f );

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) {
					CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_nItemDropGroupTableID, m_VecDropItemList );

					if (m_VecDropItemList.empty())
					{
						// 프랍로그
						// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, 0, nTableID);	// 스테이지 로그 090226
					}
					else {
						for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID, m_VecDropItemList[i].nEnchantID ) == false )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID ) == false )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
							{
								m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
								i--;
								continue;
							}

							// 프랍로그
							// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, m_VecDropItemList[i].nItemID, nTableID);	// 스테이지 로그 090226
						}
					}
				}
			}

		}
	}

	return bResult;
}


// 일단 Shooter 프랍으로 스킬쓰는 프랍 테스트 해보자..

void CDnWorldShooterBrokenProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	if( m_hMonster ) 
	{
		*(m_hMonster->GetMatEx()) = m_Cross;
		m_SkillComponent.OnSignal( m_hMonster, GetMySmartPtr(), Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}

	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	// 마지막으로 맞았던 액션을 기억해서 Normal 상태일 때 다시 복구 시켜 줌.
	if( STE_Projectile == Type )
	{
		const char* pCurrentAttackAction = GetCurrentAction();
		if( strlen( pCurrentAttackAction ) )
			m_strLastActionName.assign( pCurrentAttackAction );
	}
}


void CDnWorldShooterBrokenProp::OnSyncComplete( CDNUserSession* pBreakIntoGameSession/*=NULL*/ )
{
	if( pBreakIntoGameSession )
		CmdAction( GetCurrentAction(), pBreakIntoGameSession );
	else
		CmdAction( m_szDefaultActionName.c_str(), pBreakIntoGameSession );
}


// 슈터 프랍은 발사체가 클라의 연출과 정확하게 맞아야 하기 때문에 액션 변경때마다 서버에서 패킷을 보내준다.
void CDnWorldShooterBrokenProp::OnChangeAction( const char *szPrevAction )
{
	//// 어차피 다른 액션으로 바뀌지 않는 이상 idle 이 반복되기 때문에 
	//// idle 액션인 경우엔 idle 로 변경되었을 때 한번만 보낸다. 
	//// Idle 액션이 짧은 경우 계속 패킷 나가는 문제를 감안, 수정. (#17409)
	//const char* pCurrentAction = GetCurrentAction();
	//if( strcmp(pCurrentAction, "Idle") == 0 )
	//{
	//	if( strcmp(szPrevAction, "Idle") != 0 )
	//	{
	//		ActionSync( pCurrentAction );
	//	}
	//}
	//else
	//	ActionSync( pCurrentAction );

	// 액션이 변경되었을 때만 패킷 보냄.
	if( m_nActionIndex != m_nPrevActionIndex )
		ActionSync( GetCurrentAction() );

	if( strcmp( szPrevAction, "Hit" ) )
	{
		if( !m_strLastActionName.empty() )
			SetActionQueue( m_strLastActionName.c_str() );
	}
}