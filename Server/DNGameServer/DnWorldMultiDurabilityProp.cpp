#include "StdAfx.h"
#include "DnWorldMultiDurabilityProp.h"
#include "DnStateBlow.h"
#include "DnTableDB.h"
#include "DnPlayerActor.h"
#include "DnProjectile.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnPropState.h"
#include "DnPropCondition.h"
#include "DnPropStateDoAction.h"
#include "DnPropStateTrigger.h"
#include "DnPropCondiDurability.h"
#include "DnPropActionCondition.h"
#include "DnPropCondiSubDurability.h"
#include "DnMonsterActor.h"


CDnWorldMultiDurabilityProp::CDnWorldMultiDurabilityProp( CMultiRoom* pRoom ) : CDnWorldBrokenProp( pRoom ), m_iNowStateIndex( 0 ), m_pActivateState( NULL )
{

}

CDnWorldMultiDurabilityProp::~CDnWorldMultiDurabilityProp(void)
{
	ReleasePostCustomParam();
}

bool CDnWorldMultiDurabilityProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	// 디폴트의 BrokenProp 의 Initialize 를 호출하지 않고 각 phase 별로 fsm 을 구성해준다.
	if( m_hMonster )
		*m_hMonster->GetMatEx() = *GetMatEx();

	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( !bResult )
		return false;

	// 일단 테스트
	SetActionQueue( "Activate", 0, 3.0f, 0.0f );

	return true;
}


void CDnWorldMultiDurabilityProp::_InitializeFSM( void )
{
	// 초기엔 idle 상태. Activate 상태가 되면 발동한 것으로 판단.
	CDnPropState* pIdleState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	m_pFSM->AddState( pIdleState );

	// 외부 트리거에서 Activate 액션을 실행해서 작동을 시작한 경우 
	CDnPropState* pActivateState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	m_pFSM->AddState( pActivateState );
	m_pActivateState = pActivateState;

	// 메인 내구도가 다 되어 부서졌을 때는 지정된 트리거를 실행하고 Broken 액션을 수행.
	CDnPropState* pTriggerState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::TRIGGER );
	m_pFSM->AddState( pTriggerState );
	CDnPropState* pBrokenActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	m_pFSM->AddState( pBrokenActionState );
	
	m_pBrokenActionState = static_cast<CDnPropStateDoAction*>( pBrokenActionState );
	m_pTriggerActionState = static_cast<CDnPropStateTrigger*>( pTriggerState );
	m_pTriggerActionState->SetFuncName( "CDnWorldProp::OnBrokenProp" );
	
	// 조건 생성
	CDnPropCondition* pToActivate = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::ACTION_CHECK );	// 작동 시작
	static_cast<CDnPropActionCondition*>(pToActivate)->SetActionName( "Activate" );
	m_pFSM->AddCondition( pToActivate );
	CDnPropCondition* pToIdle = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::ACTION_CHECK );		// idle 액션이면 idle 상태로.
	static_cast<CDnPropActionCondition*>(pToIdle)->SetActionName( "Idle" );
	m_pFSM->AddCondition( pToIdle );

	// 외부 트리거에서 Activate 액션을 실행해서 작동을 시작한 경우, 두번째 상태로 전이할 수 있도록 해준다.
	pIdleState->AddTransitState( pActivateState, pToActivate );

	// 메인 내구도가 다 되었는지 체크하는 객체. -> 트리거 구동 상태로 전이.
	CDnPropCondition* pDurabilityCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_DURABILITY );
	m_pFSM->AddCondition( pDurabilityCondition );
	static_cast<CDnPropCondiDurability*>(pDurabilityCondition)->Initialize( 0, CDnPropCondiDurability::LESS_EQUAL );

	// 트리거 상태에서 Broken 액션을 실행하는 상태로 전이. 아무 조건이 없는 NULL 조건 객체.
	CDnPropCondition* pNULLCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::NULL_CONDITION );
	m_pFSM->AddCondition( pNULLCondition );

	// 트리거 상태에서는 곧바로 아무 조건없이 brokenstate 로 넘어간다.
	m_pTriggerActionState->AddTransitState( m_pBrokenActionState, pNULLCondition );

	for( int i = 0; i < (int)m_vlStateInfos.size(); ++i )
	{
		S_STATE_INFO& StateInfo = m_vlStateInfos.at( i );

		//// 내구도 체크 인덱스는 이전 상태에서 전이하기 위한 조건이므로 이전 상태 기준의 내구도 인덱스임.
		//int iSubDurCheckIndex = i - 1;
		//if( iSubDurCheckIndex < 0 )
		//	iSubDurCheckIndex = (int)m_vlStateInfos.size() - 1;

		// 상태 생성 및 조건과 이어주기.
		CDnPropState* pState =  CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
		m_pFSM->AddState( pState );
		static_cast<CDnPropStateDoAction*>(pState)->AddActionName( StateInfo.pActionNameWhenSubDurabilityZero );

		// 비활성 상태로 가는 조건으로 전이되게 셋팅.
		pState->AddTransitState( pIdleState, pToIdle );

		// 메인 내구도가 다 되어 부서지면 곧바로 부서지는 처리를 위한 트리거 상태로 전이.
		pState->AddTransitState( m_pTriggerActionState, pDurabilityCondition );;
		
		if( 1 == i )
		{
			// 외부 트리거에서 Activate 액션을 실행해서 작동을 시작한 경우, 현재 이미 Activate 상태이므로 
			// 다음에 전이될 대상인 두번째 상태로 전이할 수 있도록 해준다.
			// 프랍의 상태가 한바퀴 돌게 되면 그때부턴 계속 메인 내구도가 다 닳때 까지 계속 이 벡터에 있는 상태별로 로테이션을 돌게 된다.
			// 내구도 조건 생성.
			CDnPropCondiSubDurability* pSubDurabilityCondition = 
				static_cast<CDnPropCondiSubDurability*>(CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_SUB_DURABILITY ));
			m_pFSM->AddCondition( pSubDurabilityCondition );
			pSubDurabilityCondition->Initialize( 0, 0, CDnPropCondiSubDurability::LESS_EQUAL );

			pActivateState->AddTransitState( pState, pSubDurabilityCondition );
		}
		
		if( 0 < i )
		{
			// 이전 상태에게 현재 상태로 전이할 수 있도록 추가.
			CDnPropState* pPrevState = m_vlStateInfos.at( i-1 ).pState;

			// 내구도 조건 생성.
			CDnPropCondiSubDurability* pSubDurabilityCondition = 
				static_cast<CDnPropCondiSubDurability*>(CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_SUB_DURABILITY ));
			m_pFSM->AddCondition( pSubDurabilityCondition );
			pSubDurabilityCondition->Initialize( 0, i-1, CDnPropCondiSubDurability::LESS_EQUAL );

			pPrevState->AddTransitState( pState, pSubDurabilityCondition );
		}

		// 맨 끝인 경우 다시 상태 1로 전이할 수 있도록 추가.
		if( i == (int)m_vlStateInfos.size()-1 )
		{
			CDnPropState* pFirstState = m_vlStateInfos.front().pState;
			
			CDnPropCondiSubDurability* pSubDurabilityCondition = 
				static_cast<CDnPropCondiSubDurability*>(CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_SUB_DURABILITY ));
			m_pFSM->AddCondition( pSubDurabilityCondition );
			pSubDurabilityCondition->Initialize( 0, i, CDnPropCondiSubDurability::LESS_EQUAL );

			pState->AddTransitState( pFirstState, pSubDurabilityCondition );
		}

		StateInfo.pState = pState;
	}

	m_pFSM->SetEntryState( pIdleState );
}


bool CDnWorldMultiDurabilityProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable(nTableID) == false ) return false;

	bool bResult = false;

	if( GetData() )
	{
		MultiDurabilityBrokenPropStruct* pStruct = (MultiDurabilityBrokenPropStruct*)GetData();
		int nMainDurability = pStruct->nMainDurability;
		int nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
		int nMonsterTableID = pStruct->nMonsterTableID;
		int nSkillTableID = pStruct->nSkillTableID;
		int nSkillLevel = pStruct->nSkillLevel;
		bResult = InitializeMonsterActorProp( nMonsterTableID );

		if( bResult )
		{
			m_SkillComponent.Initialize( nSkillTableID, nSkillLevel );

			m_nDurability = nMainDurability;

			// 일단 액션을 취하지 않도록한다. 슈터 프랍은 클라와 시작 시간을 맞춰줘야 한다..
			SetAction( "Idle", 0.0f, 0.0f );

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) 
				{
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

			S_STATE_INFO StateInfo;
			StateInfo.pActionNameWhenSubDurabilityZero = pStruct->szActionName_SubDurabilityOne;
			StateInfo.iOriginalDurability = StateInfo.iNowDurability = pStruct->nSubDurability_One;
			m_vlStateInfos.push_back( StateInfo );

			StateInfo.pActionNameWhenSubDurabilityZero = pStruct->szActionName_SubDurabilityTwo;
			StateInfo.iOriginalDurability = StateInfo.iNowDurability = pStruct->nSubDurability_Two;
			m_vlStateInfos.push_back( StateInfo );

			StateInfo.pActionNameWhenSubDurabilityZero = pStruct->szActionName_SubDurabilityThree;
			StateInfo.iOriginalDurability = StateInfo.iNowDurability = pStruct->nSubDurability_Three;
			m_vlStateInfos.push_back( StateInfo );

			StateInfo.pActionNameWhenSubDurabilityZero = pStruct->szActionName_SubDurabilityFour;
			StateInfo.iOriginalDurability = StateInfo.iNowDurability = pStruct->nSubDurability_Four;
			m_vlStateInfos.push_back( StateInfo );

			// 디폴트의 BrokenProp 의 Initialize 를 호출하지 않고 각 상태별로 fsm 을 구성해준다.
			_InitializeFSM();
		}
	}

	return bResult;
}

int CDnWorldMultiDurabilityProp::GetSubDurability( int iIndex )
{
	int iResult = 0;

	if( iIndex < (int)m_vlStateInfos.size() )
	{
		iResult = m_vlStateInfos.at( iIndex ).iNowDurability;
	}

	return iResult;
}

void CDnWorldMultiDurabilityProp::OnFSMStateEntry( const TDnFSMState<DnPropHandle>* pState )
{
	for( int i = 0; i < (int)m_vlStateInfos.size(); ++i )
	{
		const S_STATE_INFO& StateInfo = m_vlStateInfos.at( i );
		if( StateInfo.pState == pState )
		{
			m_iNowStateIndex = i;
			break;
		}
	}
}

void CDnWorldMultiDurabilityProp::CalcDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam )
{
	if( m_bDestroy == true ) return;
	if( m_bBroken ) return;

	m_pLastHitObject = pHitter;
	m_LastDamageTime = HitParam.RemainTime;
	m_nLastHitUniqueID = HitParam.iUniqueID;

	// 메인 내구도와 서브 내구도를 같이 차감.
	int iDurabilityAttack = (int)( HitParam.fDurability * 100.f );
	_ASSERT( 0 <= m_iNowStateIndex && m_iNowStateIndex < (int)m_vlStateInfos.size() );
	S_STATE_INFO& StateInfo = m_vlStateInfos.at( m_iNowStateIndex );
	StateInfo.iNowDurability -= iDurabilityAttack;
	m_nDurability -= iDurabilityAttack;

	// NOTE: 트리거에서 IsBroken() 사용함.. 다른 함수들도 체크해봐야 할듯.
	// 트리거 관련해선 상황이 변경되자마자 곧바로 전이 체크를 돌려야 한다.
	if( m_nDurability <= 0 )
	{
		m_bBroken = true;

		//소환 몬스터일 경우 주인 액터를 찾아서 설정 해야 함.
		DnActorHandle hHitterActor;

		if (pHitter)
			hHitterActor = pHitter->GetActorHandle();

		//몬스터 액터인 경우 
		if (hHitterActor && hHitterActor->IsMonsterActor())
		{
			DnActorHandle hMasterActor;
			CDnMonsterActor* pMonsterActor = NULL;

			pMonsterActor = static_cast<CDnMonsterActor*>(hHitterActor.GetPointer());

			if (pMonsterActor)
				hMasterActor = pMonsterActor->GetSummonerPlayerActor();

			//정상적인 주인 액터를 찾았으면, hHitterActor를 주인 액터로 변경한다.
			if (hMasterActor)
				hHitterActor = hMasterActor;
		}

		m_pTriggerActionState->AddFuncParam( "LastBrokenPropActor", hHitterActor ? hHitterActor->GetUniqueID() : -1 );

		m_pFSM->Process( 0, 0.0f );

		UpdatePropBreakToHitter( pHitter );
	}
	else
	if( StateInfo.iNowDurability <= 0 )
	{
		// 서브 내구도가 빠진 상태라면 FSM 을 진행시키고 부서진 상태의 서브 내구도를 리셋시켜준다.
		m_pFSM->Process( 0, 0.0f );
		StateInfo.RestoreDurability();
	}

	if( HitParam.hWeapon ) {
		if( HitParam.hWeapon->GetWeaponType() & CDnWeapon::Projectile ) {
			CDnProjectile *pProjectile = static_cast<CDnProjectile *>(HitParam.hWeapon.GetPointer());
			if( pProjectile ) pProjectile->OnDamageSuccess( CDnActor::Identity(), HitParam );
			else HitParam.hWeapon->SetDestroy();
		}
	}
}

void CDnWorldMultiDurabilityProp::SetActionQueue( const char *szActionName, int nLoopCount , float fBlendFrame , float fStartFrame )
{
	//// 각 상태별 액션이라면 서브 내구도를 채워준다.
	//for( int i = 0; i < (int)m_vlStateInfos.size(); ++i )
	//{
	//	S_STATE_INFO& StateInfo = m_vlStateInfos.at( i );

	//	if( 0 == strcmp( StateInfo.pActionNameWhenSubDurabilityZero, szActionName ) )
	//	{
	//		StateInfo.RestoreDurability();

	//		// 현재 상태의 인덱스를 셋팅. 액션 이름은 서브 내구도가 부셔졌을 때 행할 액션이므로
	//		// 현재 인덱스의 다음 상태가 현재 상태가 된다.
	//		m_iNowStateIndex = (i+1) % (int)m_vlStateInfos.size();
	//	}
	//}

	CDnWorldActProp::SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame );
}


void CDnWorldMultiDurabilityProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	if( m_hMonster ) 
	{
		*(m_hMonster->GetMatEx()) = m_Cross;
		m_SkillComponent.OnSignal( m_hMonster, GetMySmartPtr(), Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
	}

	CDnWorldBrokenProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	//// 마지막으로 맞았던 액션을 기억해서 Normal 상태일 때 다시 복구 시켜 줌.
	//if( STE_Projectile == Type )
	//{
	//	const char* pCurrentAttackAction = GetCurrentAction();
	//	if( strlen( pCurrentAttackAction ) )
	//		m_strLastActionName.assign( pCurrentAttackAction );
	//}
}


void CDnWorldMultiDurabilityProp::OnSyncComplete( CDNUserSession* pBreakIntoGameSession/*=NULL*/ )
{
	if( pBreakIntoGameSession )
		CmdAction( GetCurrentAction(), pBreakIntoGameSession );
	else
		CmdAction( m_szDefaultActionName.c_str(), pBreakIntoGameSession );
}


void CDnWorldMultiDurabilityProp::OnChangeAction( const char *szPrevAction )
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

	////if( strcmp( szPrevAction, "Hit" ) )
	////{
	////	if( !m_strLastActionName.empty() )
	////		SetActionQueue( m_strLastActionName.c_str() );
	////}

	// 액션이 변경되었을 때만 패킷 보냄.
	if( m_nActionIndex != m_nPrevActionIndex )
		ActionSync( GetCurrentAction() );
}