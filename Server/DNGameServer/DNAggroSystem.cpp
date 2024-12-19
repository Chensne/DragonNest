
#include "stdafx.h"
#include "DNAggroSystem.h"
#include "DnActor.h"
#include "DnMonsterActor.h"
#include "DnWorldProp.h"
#include "DnStateBlow.h"

CDNAggroSystem::CDNAggroSystem( DnActorHandle hActor )
: m_hActor( hActor )
{
	m_bIgnore = false;
}

CDNAggroSystem::~CDNAggroSystem()
{

}

void CDNAggroSystem::ResetAggro()
{
	m_AggroList.clear();
}

void CDNAggroSystem::ResetAggro( DnActorHandle hActor )
{
	if( !hActor )
		return;

	for( std::list<AggroStruct>::iterator itor=m_AggroList.begin() ; itor!=m_AggroList.end() ; ++itor )
	{
		if( (*itor).hActor == hActor )
		{
			m_AggroList.erase( itor );
			return;
		}
	}
}

void CDNAggroSystem::AddAggro( DnActorHandle hActor, int iAggroValue, bool bForce/*=false*/ )
{
	if( !hActor || m_hActor == hActor )
		return;

	// 같은팀끼리는 어그로가 쌓이지 않는다.
	if( m_hActor->GetTeam() == hActor->GetTeam() )
		return;

	// 운영자 난입은 어그로를 먹지 않는다.
	if( hActor->IsGMTrace() )
		return;

	// 148)	상태효과가 적용되어 있으면 Aggro 가 쌓이지 않는다.
	if( m_hActor && m_hActor->GetStateBlow() && m_hActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_148) )
		return;

	// CannonActor 검사
	if( hActor->GetActorType() == CDnActorState::ActorTypeEnum::Cannon )
		return;
	if( hActor->GetActorType() == CDnActorState::ActorTypeEnum::NoAggro || hActor->GetActorType() == CDnActorState::ActorTypeEnum::NoAggroTrap )
		return;

	// Prop액터 검사
	if( hActor->GetActorType() == CDnActorState::ActorTypeEnum::PropActor )
	{
		bool bCheckProp = false;
		if( hActor->IsMonsterActor() )
		{
			CDnMonsterActor*	pMonster	= static_cast<CDnMonsterActor*>(hActor.GetPointer());
			DnPropHandle		hProp		= pMonster->GetPropHandle();
			if( hProp )
			{
				if( hProp->IsEnableAggro() && hProp->IsShow() )
					bCheckProp = true;
			}
		}
		if( !bCheckProp )
			return;
	}

	// 어그로 보정치 적용
	OnAggroRegulation( hActor, iAggroValue );

	if( iAggroValue == 0 )
		iAggroValue = 1;

	if( m_bIgnore == true && bForce == false )
		return;

	AggroStruct* pStruct = GetAggroStruct( hActor );
	if( pStruct ) 
		pStruct->iAggro += iAggroValue;
	else 
	{
		if( iAggroValue > 0 )
			m_AggroList.push_back( AggroStruct( hActor, iAggroValue ) );
	}
}

CDNAggroSystem::AggroStruct* CDNAggroSystem::GetAggroStruct( DnActorHandle hActor )
{
	if( !hActor )
		return NULL;

	for( std::list<AggroStruct>::iterator itor=m_AggroList.begin() ; itor!=m_AggroList.end() ; ++itor )
	{
		if( (*itor).hActor == hActor )
		{
			return &(*itor);
		}
	}

	return NULL;
}

void CDNAggroSystem::Convert( DNVector(DnActorHandle)& vOutput )
{
	vOutput.reserve( m_AggroList.size() );
	for( std::list<AggroStruct>::iterator itor=m_AggroList.begin() ; itor!=m_AggroList.end() ; ++itor )
		vOutput.push_back( (*itor).hActor );
}

int CDNAggroSystem::GetAggroSum() const
{
	int aggroSum = 0;
	std::list<AggroStruct>::const_iterator iter = m_AggroList.begin();
	for( ; iter != m_AggroList.end() ; ++iter )
	{
		const AggroStruct& curAggroStruct = *iter;
		aggroSum += curAggroStruct.iAggro;
	}

	return aggroSum;
}