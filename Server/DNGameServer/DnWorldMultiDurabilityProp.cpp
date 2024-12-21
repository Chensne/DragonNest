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
	// ����Ʈ�� BrokenProp �� Initialize �� ȣ������ �ʰ� �� phase ���� fsm �� �������ش�.
	if( m_hMonster )
		*m_hMonster->GetMatEx() = *GetMatEx();

	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( !bResult )
		return false;

	// �ϴ� �׽�Ʈ
	SetActionQueue( "Activate", 0, 3.0f, 0.0f );

	return true;
}


void CDnWorldMultiDurabilityProp::_InitializeFSM( void )
{
	// �ʱ⿣ idle ����. Activate ���°� �Ǹ� �ߵ��� ������ �Ǵ�.
	CDnPropState* pIdleState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	m_pFSM->AddState( pIdleState );

	// �ܺ� Ʈ���ſ��� Activate �׼��� �����ؼ� �۵��� ������ ��� 
	CDnPropState* pActivateState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	m_pFSM->AddState( pActivateState );
	m_pActivateState = pActivateState;

	// ���� �������� �� �Ǿ� �μ����� ���� ������ Ʈ���Ÿ� �����ϰ� Broken �׼��� ����.
	CDnPropState* pTriggerState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::TRIGGER );
	m_pFSM->AddState( pTriggerState );
	CDnPropState* pBrokenActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	m_pFSM->AddState( pBrokenActionState );
	
	m_pBrokenActionState = static_cast<CDnPropStateDoAction*>( pBrokenActionState );
	m_pTriggerActionState = static_cast<CDnPropStateTrigger*>( pTriggerState );
	m_pTriggerActionState->SetFuncName( "CDnWorldProp::OnBrokenProp" );
	
	// ���� ����
	CDnPropCondition* pToActivate = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::ACTION_CHECK );	// �۵� ����
	static_cast<CDnPropActionCondition*>(pToActivate)->SetActionName( "Activate" );
	m_pFSM->AddCondition( pToActivate );
	CDnPropCondition* pToIdle = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::ACTION_CHECK );		// idle �׼��̸� idle ���·�.
	static_cast<CDnPropActionCondition*>(pToIdle)->SetActionName( "Idle" );
	m_pFSM->AddCondition( pToIdle );

	// �ܺ� Ʈ���ſ��� Activate �׼��� �����ؼ� �۵��� ������ ���, �ι�° ���·� ������ �� �ֵ��� ���ش�.
	pIdleState->AddTransitState( pActivateState, pToActivate );

	// ���� �������� �� �Ǿ����� üũ�ϴ� ��ü. -> Ʈ���� ���� ���·� ����.
	CDnPropCondition* pDurabilityCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_DURABILITY );
	m_pFSM->AddCondition( pDurabilityCondition );
	static_cast<CDnPropCondiDurability*>(pDurabilityCondition)->Initialize( 0, CDnPropCondiDurability::LESS_EQUAL );

	// Ʈ���� ���¿��� Broken �׼��� �����ϴ� ���·� ����. �ƹ� ������ ���� NULL ���� ��ü.
	CDnPropCondition* pNULLCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::NULL_CONDITION );
	m_pFSM->AddCondition( pNULLCondition );

	// Ʈ���� ���¿����� ��ٷ� �ƹ� ���Ǿ��� brokenstate �� �Ѿ��.
	m_pTriggerActionState->AddTransitState( m_pBrokenActionState, pNULLCondition );

	for( int i = 0; i < (int)m_vlStateInfos.size(); ++i )
	{
		S_STATE_INFO& StateInfo = m_vlStateInfos.at( i );

		//// ������ üũ �ε����� ���� ���¿��� �����ϱ� ���� �����̹Ƿ� ���� ���� ������ ������ �ε�����.
		//int iSubDurCheckIndex = i - 1;
		//if( iSubDurCheckIndex < 0 )
		//	iSubDurCheckIndex = (int)m_vlStateInfos.size() - 1;

		// ���� ���� �� ���ǰ� �̾��ֱ�.
		CDnPropState* pState =  CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
		m_pFSM->AddState( pState );
		static_cast<CDnPropStateDoAction*>(pState)->AddActionName( StateInfo.pActionNameWhenSubDurabilityZero );

		// ��Ȱ�� ���·� ���� �������� ���̵ǰ� ����.
		pState->AddTransitState( pIdleState, pToIdle );

		// ���� �������� �� �Ǿ� �μ����� ��ٷ� �μ����� ó���� ���� Ʈ���� ���·� ����.
		pState->AddTransitState( m_pTriggerActionState, pDurabilityCondition );;
		
		if( 1 == i )
		{
			// �ܺ� Ʈ���ſ��� Activate �׼��� �����ؼ� �۵��� ������ ���, ���� �̹� Activate �����̹Ƿ� 
			// ������ ���̵� ����� �ι�° ���·� ������ �� �ֵ��� ���ش�.
			// ������ ���°� �ѹ��� ���� �Ǹ� �׶����� ��� ���� �������� �� �⶧ ���� ��� �� ���Ϳ� �ִ� ���º��� �����̼��� ���� �ȴ�.
			// ������ ���� ����.
			CDnPropCondiSubDurability* pSubDurabilityCondition = 
				static_cast<CDnPropCondiSubDurability*>(CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_SUB_DURABILITY ));
			m_pFSM->AddCondition( pSubDurabilityCondition );
			pSubDurabilityCondition->Initialize( 0, 0, CDnPropCondiSubDurability::LESS_EQUAL );

			pActivateState->AddTransitState( pState, pSubDurabilityCondition );
		}
		
		if( 0 < i )
		{
			// ���� ���¿��� ���� ���·� ������ �� �ֵ��� �߰�.
			CDnPropState* pPrevState = m_vlStateInfos.at( i-1 ).pState;

			// ������ ���� ����.
			CDnPropCondiSubDurability* pSubDurabilityCondition = 
				static_cast<CDnPropCondiSubDurability*>(CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_SUB_DURABILITY ));
			m_pFSM->AddCondition( pSubDurabilityCondition );
			pSubDurabilityCondition->Initialize( 0, i-1, CDnPropCondiSubDurability::LESS_EQUAL );

			pPrevState->AddTransitState( pState, pSubDurabilityCondition );
		}

		// �� ���� ��� �ٽ� ���� 1�� ������ �� �ֵ��� �߰�.
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

			// �ϴ� �׼��� ������ �ʵ����Ѵ�. ���� ������ Ŭ��� ���� �ð��� ������� �Ѵ�..
			SetAction( "Idle", 0.0f, 0.0f );

			if( -1 != m_nDurability )
			{
				if( 0 == m_nDurability ) m_nDurability = 1;
				m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				if( m_nItemDropGroupTableID > 0 ) 
				{
					CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_nItemDropGroupTableID, m_VecDropItemList );

					if (m_VecDropItemList.empty()){
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

			// ����Ʈ�� BrokenProp �� Initialize �� ȣ������ �ʰ� �� ���º��� fsm �� �������ش�.
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

	// ���� �������� ���� �������� ���� ����.
	int iDurabilityAttack = (int)( HitParam.fDurability * 100.f );
	_ASSERT( 0 <= m_iNowStateIndex && m_iNowStateIndex < (int)m_vlStateInfos.size() );
	S_STATE_INFO& StateInfo = m_vlStateInfos.at( m_iNowStateIndex );
	StateInfo.iNowDurability -= iDurabilityAttack;
	m_nDurability -= iDurabilityAttack;

	// NOTE: Ʈ���ſ��� IsBroken() �����.. �ٸ� �Լ��鵵 üũ�غ��� �ҵ�.
	// Ʈ���� �����ؼ� ��Ȳ�� ������ڸ��� ��ٷ� ���� üũ�� ������ �Ѵ�.
	if( m_nDurability <= 0 )
	{
		m_bBroken = true;

		//��ȯ ������ ��� ���� ���͸� ã�Ƽ� ���� �ؾ� ��.
		DnActorHandle hHitterActor;

		if (pHitter)
			hHitterActor = pHitter->GetActorHandle();

		//���� ������ ��� 
		if (hHitterActor && hHitterActor->IsMonsterActor())
		{
			DnActorHandle hMasterActor;
			CDnMonsterActor* pMonsterActor = NULL;

			pMonsterActor = static_cast<CDnMonsterActor*>(hHitterActor.GetPointer());

			if (pMonsterActor)
				hMasterActor = pMonsterActor->GetSummonerPlayerActor();

			//�������� ���� ���͸� ã������, hHitterActor�� ���� ���ͷ� �����Ѵ�.
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
		// ���� �������� ���� ���¶�� FSM �� �����Ű�� �μ��� ������ ���� �������� ���½����ش�.
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
	//// �� ���º� �׼��̶�� ���� �������� ä���ش�.
	//for( int i = 0; i < (int)m_vlStateInfos.size(); ++i )
	//{
	//	S_STATE_INFO& StateInfo = m_vlStateInfos.at( i );

	//	if( 0 == strcmp( StateInfo.pActionNameWhenSubDurabilityZero, szActionName ) )
	//	{
	//		StateInfo.RestoreDurability();

	//		// ���� ������ �ε����� ����. �׼� �̸��� ���� �������� �μ����� �� ���� �׼��̹Ƿ�
	//		// ���� �ε����� ���� ���°� ���� ���°� �ȴ�.
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

	//// ���������� �¾Ҵ� �׼��� ����ؼ� Normal ������ �� �ٽ� ���� ���� ��.
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

	////if( strcmp( szPrevAction, "Hit" ) )
	////{
	////	if( !m_strLastActionName.empty() )
	////		SetActionQueue( m_strLastActionName.c_str() );
	////}

	// �׼��� ����Ǿ��� ���� ��Ŷ ����.
	if( m_nActionIndex != m_nPrevActionIndex )
		ActionSync( GetCurrentAction() );
}