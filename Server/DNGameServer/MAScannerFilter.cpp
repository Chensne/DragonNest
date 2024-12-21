
#include "stdafx.h"
#include "MAScannerFilter.h"
#include "DnActor.h"

// SameTeam Filter
bool MAScanSameTeamFilter::bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin/*=0.f*/ )
{
	return hActor->GetTeam() == hDestActor->GetTeam();
}

// OpponentTeam Filter
bool MAScanOpponentTeamFilter::bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin/*=0.f*/ )
{
	return hActor->GetTeam() != hDestActor->GetTeam();
}

// DestActor Die Filter
bool MAScanDestActorDieFilter::bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin/*=0.f*/ )
{
	return hDestActor->IsDie();
}

// DestActor Npc Filter
bool MAScanDestActorNpcFilter::bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin/*=0.f*/ )
{
	return hDestActor->IsNpcActor();
}

// MinDistance Filter
bool MAScanMinDistanceFilter::bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin/*=0.f*/ )
{
	if( fMin <= 0.f )
		return false;

	EtVector3	vPos	= *(hDestActor->GetPosition()) - *(hActor->GetPosition());
	float		fDistSq	= EtVec3LengthSq( &vPos );

	if ( fDistSq < fMin*fMin )
		return true;

	return false;
}

// EqualActor Filter
bool MAScanEqualActorFilter::bIsCheck( DnActorHandle hActor, DnActorHandle hDestActor, float fMin/*=0.f*/ )
{
	return hActor == hDestActor;
}
