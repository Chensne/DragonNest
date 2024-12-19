
#include "StdAfx.h"
#include "DnGreenDragonMonsterActor.h"

CDnGreenDragonMonsterActor::CDnGreenDragonMonsterActor( int nClassID, bool bProcess )
: CDnPartsMonsterActor(nClassID,bProcess)
{
	m_HitCheckType = HitCheckTypeEnum::Collision;

	SetFindAutoTargetName("Bip01 Head");
}

CDnGreenDragonMonsterActor::~CDnGreenDragonMonsterActor()
{
}


bool CDnGreenDragonMonsterActor::Initialize()
{
	CDnPartsMonsterActor::Initialize();
	m_hObject->SetCalcPositionFlag( 0 );
	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 3 ) );
	m_hObject->SetTargetCollisionGroup( 0 );
	m_hObject->EnableCull( false );

	SetUseDamageColor(true);

#ifdef PRE_ADD_DAMAGEDPARTS
	// 60073.
	SetUseDamagedPartsEmissiveColor( true ); 
	SetUseHPPercentEmissiveColor( true );
#endif

	return true;
}

void CDnGreenDragonMonsterActor::OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time)
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
void CDnGreenDragonMonsterActor::CheckDamageVelocity( DnActorHandle hActor, bool bKeepJumpMovement )
#else
void CDnGreenDragonMonsterActor::CheckDamageVelocity( DnActorHandle hActor )
#endif
{

}