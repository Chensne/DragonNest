#include "StdAfx.h"
#include "DnWorldBrokenDamageProp.h"
#include "TaskManager.h"
#include "DnGameTask.h"


CDnWorldBrokenDamageProp::CDnWorldBrokenDamageProp( CMultiRoom* pRoom ) : CDnWorldBrokenProp( pRoom ),
																		  CDnDamageBase( DamageObjectTypeEnum::Prop ),
																		  m_iSkillApplyType( -1 ),
																		  m_bEnableHitSignal( true )
{
}

CDnWorldBrokenDamageProp::~CDnWorldBrokenDamageProp(void)
{
	ReleasePostCustomParam();
}

bool CDnWorldBrokenDamageProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( CDnWorldBrokenProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) return false;
	if( m_hMonster ) *m_hMonster->GetMatEx() = *GetMatEx();
	return true;
}

bool CDnWorldBrokenDamageProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	if( GetData() ) {
		BrokenDamageStruct*	pStruct = (BrokenDamageStruct *)GetData();
		bool				bResult = false;

//#61146 Trap타입 프랍의 공격력을 난이도에 따라 다르게 세팅할 수 있는 구조.
#ifdef PER_ADD_PROP_TRAP_DIFFICULT_SET
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID, pStruct->MonsterWeight );
#else
		bResult = InitializeMonsterActorProp( pStruct->nMonsterTableID );
#endif	
		
		if( bResult )
		{
//			m_hMonster->SetUniqueID( STATIC_INSTANCE(CDnActor::s_dwUniqueCount)++ );

			m_nDurability = pStruct->nDurability;

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) {
					CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_nItemDropGroupTableID, m_VecDropItemList );

					if (m_VecDropItemList.empty()){
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

			m_iSkillApplyType = pStruct->SkillApplyType;
			if( 0 != pStruct->nSkillTableID )
			{
				m_SkillComponent.Initialize( pStruct->nSkillTableID, pStruct->nSkillLevel );
			}

			m_HitComponent.Initialize( m_hMonster, &m_Cross, GetMySmartPtr(), this );
		}
		else
			return false;
	}

	return true;
}


void CDnWorldBrokenDamageProp::OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam )
{
	CDnWorldBrokenProp::OnDamage( pHitter, HitParam );

	if( m_nDurability <= 0 )
	{
		// 스킬 적용타입이 막타 친 유저라면.. 내구도가 0인지 체크해서 스킬에 있는 상태효과 적용시킴.
		if( APPLY_SKILL_LASTHITTER == m_iSkillApplyType )
		{
			DnActorHandle hLastHitter = pHitter->GetActorHandle();
			if( hLastHitter )
			{
				const CDnSkill::SkillInfo& SkillInfo = m_SkillComponent.GetSkillInfo();
				map<int, bool> mapDuplicateResult;
				bool bAllowAddStateEffect = (CDnSkill::Fail != CDnSkill::CanApplySkillStateEffect( &SkillInfo, hLastHitter, SkillInfo.iSkillID, SkillInfo.iLevel, 
																								   SkillInfo.iSkillDuplicateMethod, SkillInfo.eDurationType, mapDuplicateResult ));

				if( bAllowAddStateEffect )
				{
					int iNumStateEffect = m_SkillComponent.GetNumStateEffect();
					for( int iStateEffect = 0; iStateEffect < iNumStateEffect; ++iStateEffect )
					{
						const CDnSkill::StateEffectStruct* pStateEffectInfo = m_SkillComponent.GetStateEffectInfo( iStateEffect );
						int iID = hLastHitter->CmdAddStateEffect( &SkillInfo, 
																  (STATE_BLOW::emBLOW_INDEX)pStateEffectInfo->nID, 
																  pStateEffectInfo->nDurationTime, 
																  pStateEffectInfo->szValue.c_str() );
					}
				}
				else
				{
					OutputDebug( "CDnWorldBrokenDamageProp::OnDamage() : APPLY_SKILL_LASTHITTER -> 상태효과 적용 실패!\n" );
				}
			}
		}
	}
}



void CDnWorldBrokenDamageProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	if( m_bEnableHitSignal )
	{
#ifdef PRE_ADD_HIT_PROP_ADD_SE
		if( m_hMonster )
		{
			DNVector(DnActorHandle) vlHittedActors;
			m_HitComponent.OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex, &vlHittedActors );
			if( false == vlHittedActors.empty() )
			{
				if( APPLY_SKILL_HITSIGNAL_RANGE == m_iSkillApplyType )
				{
					// 프랍에 적용될 때는 스킬에 있는 타겟 타입등은 무시하고 절차의 간소화를 위해서 hitsignal 을 통과한 경우
					// 스킬에 지정된 대상 팀 체크를 따로 하지 않는다.
					m_SkillComponent.ApplyStateBlowsToTheseTargets( m_hMonster, vlHittedActors );
				}
			}
		}
#else
		m_HitComponent.OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
#endif // #ifdef PRE_ADD_HIT_PROP_ADD_SE
	}

	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}


void CDnWorldBrokenDamageProp::_OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_bEnableHitSignal = true;

	CDnWorldBrokenProp::_OnLifeTimeEnd( LocalTime, fDelta );
}