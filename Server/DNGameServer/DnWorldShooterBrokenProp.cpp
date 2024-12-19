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

//#61146 TrapŸ�� ������ ���ݷ��� ���̵��� ���� �ٸ��� ������ �� �ִ� ����.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( nMonsterTableID, ( pStruct->MonsterWeight ? true : false ) );
#else
		bResult = InitializeMonsterActorProp( nMonsterTableID );
#endif	

		if( bResult )
		{
			m_SkillComponent.Initialize( nSkillTableID, nSkillLevel );

			m_nDurability = nDurability;

			// �ϴ� �׼��� ������ �ʵ����Ѵ�. ���� ������ Ŭ��� ���� �ð��� ������� �Ѵ�..
			SetAction( "Idle", 0.0f, 0.0f );

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) {
					CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_nItemDropGroupTableID, m_VecDropItemList );

					if (m_VecDropItemList.empty())
					{
						// �����α�
						// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, 0, nTableID);	// �������� �α� 090226
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

							// �����α�
							// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, m_VecDropItemList[i].nItemID, nTableID);	// �������� �α� 090226
						}
					}
				}
			}

		}
	}

	return bResult;
}


// �ϴ� Shooter �������� ��ų���� ���� �׽�Ʈ �غ���..

void CDnWorldShooterBrokenProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	if( m_hMonster ) 
	{
		*(m_hMonster->GetMatEx()) = m_Cross;
		m_SkillComponent.OnSignal( m_hMonster, GetMySmartPtr(), Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}

	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	// ���������� �¾Ҵ� �׼��� ����ؼ� Normal ������ �� �ٽ� ���� ���� ��.
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


// ���� ������ �߻�ü�� Ŭ���� ����� ��Ȯ�ϰ� �¾ƾ� �ϱ� ������ �׼� ���涧���� �������� ��Ŷ�� �����ش�.
void CDnWorldShooterBrokenProp::OnChangeAction( const char *szPrevAction )
{
	//// ������ �ٸ� �׼����� �ٲ��� �ʴ� �̻� idle �� �ݺ��Ǳ� ������ 
	//// idle �׼��� ��쿣 idle �� ����Ǿ��� �� �ѹ��� ������. 
	//// Idle �׼��� ª�� ��� ��� ��Ŷ ������ ������ ����, ����. (#17409)
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

	// �׼��� ����Ǿ��� ���� ��Ŷ ����.
	if( m_nActionIndex != m_nPrevActionIndex )
		ActionSync( GetCurrentAction() );

	if( strcmp( szPrevAction, "Hit" ) )
	{
		if( !m_strLastActionName.empty() )
			SetActionQueue( m_strLastActionName.c_str() );
	}
}