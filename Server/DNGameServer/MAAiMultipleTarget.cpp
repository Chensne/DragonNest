
#include "stdafx.h"
#include "MAAiMultipleTarget.h"
#include "MAAiScript.h"
#include "DnMonsterActor.h"
#include "DnProjectile.h"
#include "MAScanner.h"
#include "DnPlayerActor.h"
#if defined (PRE_MOD_AIMULTITARGET)
#include "DNAggroSystem.h"
#endif

CMAAiMultipleTarget::CMAAiMultipleTarget()
:m_eType(MultipleTargetType::Max)
{
	Reset();
}

CMAAiMultipleTarget::~CMAAiMultipleTarget()
{

}

void CMAAiMultipleTarget::Reset()
{ 
#if defined (PRE_MOD_AIMULTITARGET)
	m_nMultipleTarget = 0; 
	m_nSummonerTarget =0;
#else
	m_bIsMultipleTarget	= false; 
#endif
	m_iMaxMultipleTargetCount = 0;
	m_bIsAnyMultipleTarget = false;
	m_bIsExceptCannonPlayer = false;
	m_pActionTable = NULL;
	m_pSkillTable = NULL;
}

void CMAAiMultipleTarget::SetMultipleTargetAction( ActionTable* pActionTable )
{
#if defined (PRE_MOD_AIMULTITARGET)
	m_nMultipleTarget = pActionTable->nMultipleTarget;
	if( m_nMultipleTarget <= 0 )
		return;
	m_nSummonerTarget = pActionTable->nSummonerTarget;
#else
	m_bIsMultipleTarget = pActionTable->bMultipleTarget;
	if( !m_bIsMultipleTarget )
		return;
#endif

	m_eType						= MultipleTargetType::Action;
	m_pActionTable				= pActionTable;
	m_iMaxMultipleTargetCount	= pActionTable->iMaxMultipleTargetCount;
	m_bIsAnyMultipleTarget		= pActionTable->bIsAnyMultipleTarget;
	m_bIsExceptCannonPlayer		= pActionTable->bIsExceptCannonPlayer;
}

void CMAAiMultipleTarget::SetMultipleTargetSkill( MonsterSkillTable* pSkillTable )
{
#if defined (PRE_MOD_AIMULTITARGET)
	m_nMultipleTarget = pSkillTable->nMultipleTarget;
	if( m_nMultipleTarget <= 0 )
		return;
	m_nSummonerTarget = pSkillTable->nSummonerTarget;
#else
	m_bIsMultipleTarget = pSkillTable->bMultipleTarget;
	if( !m_bIsMultipleTarget )
		return;
#endif

	m_eType						= MultipleTargetType::Skill;
	m_pSkillTable				= pSkillTable;
	m_iMaxMultipleTargetCount	= pSkillTable->iMaxMultipleTargetCount;
	m_bIsAnyMultipleTarget		= pSkillTable->bIsAnyMultipleTarget;
	m_bIsExceptCannonPlayer		= pSkillTable->bIsExceptCannonPlayer;
}

void CMAAiMultipleTarget::CalcTarget( DnActorHandle hActor, MAAiScript* pScript )
{
	m_vTargetPos.clear();
	m_vTargetActor.clear();
	
#if defined (PRE_MOD_AIMULTITARGET)
	if( m_nMultipleTarget <= 0)
		return;
#else
	if( !m_bIsMultipleTarget )
		return;
#endif

	DNVector(DnActorHandle)	vTarget;
	CMAAiCheckerManager*	pCheckerManager = NULL;

	if( m_eType == MultipleTargetType::Action )
	{
		DN_ASSERT( m_pActionTable != NULL, "m_pActionTable == NULL" );

		float fMaxRange = pScript->GetScriptData().m_AITable[m_pActionTable->nDistanceState].fNearValue;
		if( m_bIsAnyMultipleTarget )
			GetMAScanner().Scan( MAScanner::eType::MonsterSkillOpponentTeam, hActor, 0.f, fMaxRange, vTarget );
		else
			pScript->GetTargetDistance( m_pActionTable->nDistanceState, vTarget, false );
		
		pCheckerManager = m_pActionTable->pMAAiCheckerManager;
	}
	else if( m_eType == MultipleTargetType::Skill )
	{
		DN_ASSERT( m_pSkillTable != NULL, "m_pSkillTable == NULL" );

		if( m_bIsAnyMultipleTarget )
			GetMAScanner().Scan( MAScanner::eType::MonsterSkillOpponentTeam, hActor, 0.f, static_cast<float>(m_pSkillTable->nRangeMax), vTarget );
		else
			pScript->GetTargetDistance( m_pSkillTable->nRangeMin, m_pSkillTable->nRangeMax, vTarget, false );
		if( vTarget.empty() )
			return;

		pCheckerManager = m_pSkillTable->pMAAiCheckerManager;
	}

	// AIChecker 로 조건 체크
	if( pCheckerManager )
	{
		for( UINT i=0 ; i<vTarget.size() ; ++i )
		{
			// CannonActor 검사
			if( vTarget[i]->GetActorType() == CDnActorState::ActorTypeEnum::Cannon )
				continue;

			if( m_bIsExceptCannonPlayer && vTarget[i]->IsPlayerActor() )
			{	
				if( static_cast<CDnPlayerActor*>(vTarget[i].GetPointer())->IsCannonMode() )
					continue;
			}

#if defined (PRE_MOD_AIMULTITARGET)
			if (m_nSummonerTarget && vTarget[i]->IsMonsterActor())
			{
				DnActorHandle hSummonMasterPlayerActor = static_cast<CDnMonsterActor*>(vTarget[i].GetPointer())->GetSummonerPlayerActor();
				if (hSummonMasterPlayerActor)
					continue;
			}
#endif

			if( !pCheckerManager->bIsTargetActorChecker( hActor, vTarget[i] ) )
				continue;

			m_vTargetPos.push_back( *vTarget[i]->GetPosition() );
			m_vTargetActor.push_back( vTarget[i] );
		}
	}

	// 이 값이 0이면 기존과 마찬가지로 모든 타겟
	if( m_iMaxMultipleTargetCount == 0 )
		return;

#if defined (PRE_MOD_AIMULTITARGET)
	// Type = 1 일반
	if (m_nMultipleTarget == 2) // Type = 2 어그로 1순위 -> 랜덤
	{
		if (true == CalcTargetByAggro(hActor))
			return;
	}

	// MultipleTarget 최대 수 보다 작으면 그냥 다 쏘면 된다.
	if( m_vTargetPos.size() <= static_cast<size_t>(m_iMaxMultipleTargetCount) )
		return;

	do 
	{
		size_t RandVal = _rand( hActor->GetRoom() )%m_vTargetPos.size();

		m_vTargetPos.erase( m_vTargetPos.begin()+RandVal );
		m_vTargetActor.erase( m_vTargetActor.begin()+RandVal );
	}while( m_vTargetPos.size() > static_cast<size_t>(m_iMaxMultipleTargetCount) );

#else // #if defined (PRE_MOD_AIMULTITARGET)

	// MultipleTarget 최대 수 보다 작으면 그냥 다 쏘면 된다.
	if( m_vTargetPos.size() <= static_cast<size_t>(m_iMaxMultipleTargetCount-1) )
		return;

	do 
	{
		size_t RandVal = _rand( hActor->GetRoom() )%m_vTargetPos.size();

		m_vTargetPos.erase( m_vTargetPos.begin()+RandVal );
		m_vTargetActor.erase( m_vTargetActor.begin()+RandVal );
	}while( m_vTargetPos.size() > static_cast<size_t>(m_iMaxMultipleTargetCount-1) );

#endif // #if defined (PRE_MOD_AIMULTITARGET)
}

#if defined (PRE_MOD_AIMULTITARGET)
bool CMAAiMultipleTarget::CalcTargetByAggro(DnActorHandle hActor)
{
	if( m_iMaxMultipleTargetCount <= 0 )
		return true;

	bool bIsProvocationTarget;
	DnActorHandle hTarget = hActor->GetAggroSystem()->OnGetAggroTarget (bIsProvocationTarget, CDnActor::Identity(), &m_vTargetActor);

	// TargetPos의 인덱스를 구한다
	int nIndex = -1;
	for (UINT i=0; i<m_vTargetActor.size(); i++)
	{
		if (m_vTargetActor[i] == hTarget)
		{
			nIndex = i;
			break;
		}
	}

	if (m_vTargetActor.size() <= 1)
		return true;

	// MultipleTarget 최대 수 보다 작으면 그냥 다 쏘면 된다.
	if( m_vTargetPos.size() <= static_cast<size_t>(m_iMaxMultipleTargetCount) )
		return true;

	EtVector3 vTargetPos;
	if (nIndex >= 0)	// 인덱스를 찾았다.
	{
		if (nIndex >= m_vTargetPos.size())
			return false;

		vTargetPos = m_vTargetPos[nIndex];
		m_vTargetPos.erase(m_vTargetPos.begin()+nIndex);
		m_vTargetActor.erase(m_vTargetActor.begin()+nIndex);
	}
	else // 인덱스를 못찾을 경우
		return false;

	if (m_vTargetActor.size() == 0)
	{
		m_vTargetActor.push_back(hTarget);
		m_vTargetPos.push_back(vTargetPos);
		return true;
	}

	//어그로 타겟 hTarget을 위해 한자리를 만들어준다.
	do 
	{
		size_t RandVal = _rand( hActor->GetRoom() )%m_vTargetPos.size();

		m_vTargetPos.erase( m_vTargetPos.begin()+RandVal );
		m_vTargetActor.erase( m_vTargetActor.begin()+RandVal );
	}while( m_vTargetPos.size() > static_cast<size_t>(m_iMaxMultipleTargetCount-1) );

	m_vTargetActor.push_back(hTarget);
	m_vTargetPos.push_back(vTargetPos);

	return true;
}
#endif

void CMAAiMultipleTarget::CreateProjectile( CDnMonsterActor* pMonsterActor, ProjectileStruct* pStruct, int iSignalIndex )
{
	// TargetType == Target 인 경우엔 현재 시점에서 계산하여 Multipe타겟을 잡아준다.
	if( pStruct->nTargetType == CDnProjectile::TargetTypeEnum::Target || pStruct->nTargetType == CDnProjectile::TargetTypeEnum::TargetPosition )
	{
		CalcTarget( pMonsterActor->GetActorHandle(), static_cast<MAAiScript*>(pMonsterActor->GetAIBase()) );
	}

	DN_ASSERT( m_vTargetPos.size() == m_vTargetActor.size(), "m_vTargetPos.size() != m_vTargetActor.size()" );

	MatrixEx OrgLocalCross = *pMonsterActor->GetMatEx();

	for( UINT i=0 ; i<m_vTargetPos.size() ; ++i )
	{
		// Projectile 방향 설정
		MatrixEx LocalCross = OrgLocalCross;

		// Homing/Terrain Homing 인 경우엔 원래 몬스터 LocalCross 가 변경되면 발사체를 쏘는 몬스터가 회전하는 것이 되므로 
		// 액션 툴에서 정해진 발사체가 쏴지는 오프셋도 회전하게 된다. 우선 문제되는 발사체 타입에서만 막아둔다. #26226
		if( false == (CDnProjectile::Homing == pStruct->nOrbitType ||
					  CDnProjectile::TerrainHoming == pStruct->nOrbitType) )
		{
			LocalCross.Look( &m_vTargetPos[i] );
		}

		CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( pMonsterActor->GetRoom(), pMonsterActor->GetActorHandle(), LocalCross, pStruct, &m_vTargetPos[i], m_vTargetActor[i] );
		if( pProjectile == NULL ) 
			return;
		pProjectile->SetShooterType( pMonsterActor->GetMySmartPtr(), pMonsterActor->GetCurrentActionIndex(), iSignalIndex );

		pMonsterActor->SendProjectile( pProjectile, pStruct, LocalCross, iSignalIndex );
		pMonsterActor->OnProjectile( pProjectile, pStruct, LocalCross, iSignalIndex );
		pMonsterActor->OnSkillProjectile( pProjectile );
	}
}
