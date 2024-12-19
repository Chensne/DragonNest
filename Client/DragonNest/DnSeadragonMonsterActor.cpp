#include "StdAfx.h"
#include "DnSeadragonMonsterActor.h"

CDnSeadragonMonsterActor::CDnSeadragonMonsterActor( int nClassID, bool bProcess )
: CDnPartsMonsterActor(nClassID,bProcess)
{
	m_HitCheckType = HitCheckTypeEnum::Collision;
}

CDnSeadragonMonsterActor::~CDnSeadragonMonsterActor()
{
}


bool CDnSeadragonMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();
	m_hObject->SetCalcPositionFlag( 0 );
	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
	m_hObject->SetTargetCollisionGroup( 0 );
	m_hObject->EnableCull( false );

	return true;
}

void CDnSeadragonMonsterActor::OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time)
{
	if( strstr( szPrevAction, "_ChangeAxis" ) )
	{		
		EtVector2 vView = EtVec3toVec2( *GetLookDir() );
		vView *= -1.f;
		Look( vView, true );
	}

	CDnMonsterActor::OnFinishAction( szPrevAction, szNextAction, time );
}

#ifdef PRE_FIX_77172
void CDnSeadragonMonsterActor::CheckDamageVelocity( DnActorHandle hActor, bool bKeepJumpMovement )
#else
void CDnSeadragonMonsterActor::CheckDamageVelocity( DnActorHandle hActor )
#endif
{

}