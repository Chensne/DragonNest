#include "StdAfx.h"
#include "DnGhoulMonsterActor.h"

CDnGhoulMonsterActor::CDnGhoulMonsterActor( CMultiRoom *pRoom, int nClassID )
: CDnSimpleRushMonsterActor( pRoom, nClassID )
{
}

CDnGhoulMonsterActor::~CDnGhoulMonsterActor()
{
}

bool CDnGhoulMonsterActor::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bCheck, bool bCheckStateEffect )
{
	if( strcmp( szActionName, "Move_Front" ) == NULL && !m_szLastHitDirection.empty() )
		return CDnSimpleRushMonsterActor::SetActionQueue( m_szLastHitDirection.c_str(), nLoopCount, fBlendFrame, fStartFrame, bCheck );
//	else if( strcmp( szActionName, "Stand" ) == NULL )
//		return CDnSimpleRushMonsterActor::SetActionQueue( m_szLastHitDirection.c_str(), nLoopCount, fBlendFrame, fStartFrame, bCheck );

	return CDnSimpleRushMonsterActor::SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, bCheck, bCheckStateEffect );
}

void CDnGhoulMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{
	CDnSimpleRushMonsterActor::OnDamage( pHitter, HitParam, pHitStruct );

	if( IsHit() && IsStiff() ) {
		if( strstr( m_HitParam.szActionName.c_str(), "Front" ) ) m_szLastHitDirection = "Move_HitFront";
		else if( strstr( m_HitParam.szActionName.c_str(), "Up" ) ) m_szLastHitDirection = "Move_HitUp";
		else if( strstr( m_HitParam.szActionName.c_str(), "Left" ) ) m_szLastHitDirection = "Move_HitLeft";
		else if( strstr( m_HitParam.szActionName.c_str(), "Right" ) ) m_szLastHitDirection = "Move_HitRight";
	}
	else m_szLastHitDirection = "Move_Front";
}

void CDnGhoulMonsterActor::ProcessStiff( LOCAL_TIME LocalTime, float fDelta )
{
	if( IsDie() ) return;

	if( IsStiff() ) {
		if( m_fStiffDelta == 0.f ) return;
		m_fStiffDelta -= fDelta;
		if( m_fStiffDelta <= 0.f ) {
			m_fStiffDelta = 0.f;

			SetActionQueue( m_szLastHitDirection.c_str(), 0, 3.f );
		}
	}
	else m_fStiffDelta = 0.f;
}