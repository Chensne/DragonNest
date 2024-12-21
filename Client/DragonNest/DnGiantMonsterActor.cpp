#include "StdAfx.h"
#include "DnGiantMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnGiantMonsterActor::CDnGiantMonsterActor( int nClassID, bool bProcess )
: CDnNormalMonsterActor( nClassID, bProcess )
{
	m_fShakeDelta = 0.f;
}

CDnGiantMonsterActor::~CDnGiantMonsterActor()
{
}

void CDnGiantMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	float fValue = 1.f;
	if( m_fShakeDelta > 0.f ) {
		m_fShakeDelta -= fDelta;
		if( m_fShakeDelta <= 0.f ) {
			m_fShakeDelta = 0.f;
		}
		fValue = 1.f - ( ( _rand()%100 ) * 0.0005f );
	}
	MAActorRenderBase::SetScale( EtVector3( GetScale() * fValue, GetScale(), GetScale() * fValue ) );

	CDnNormalMonsterActor::Process( LocalTime, fDelta );
}

void CDnGiantMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	m_fShakeDelta = 0.3f;
	CDnNormalMonsterActor::OnDamage( pHitter, HitParam );
}