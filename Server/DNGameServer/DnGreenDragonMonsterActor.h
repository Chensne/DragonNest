#pragma once

#include "DnMonsterActor.h"
#include "MASingleBody.h"
#include "MAMultiDamage.h"
#include "MAStaticLookMovement.h"
#include "DnPartsMonsterActor.h"

class CDnGreenDragonMonsterActor : public CDnPartsMonsterActor, public TBoostMemoryPool< CDnGreenDragonMonsterActor >
{
public:
	CDnGreenDragonMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnGreenDragonMonsterActor();

	virtual MAMovementBase* CreateMovement();

protected:

public:

	virtual bool bIsTurnOnAILook(){ return false; }
	virtual bool Initialize();
	virtual void OnFinishAction(const char* szPrevAction, LOCAL_TIME time);

	virtual void OnBreakSkillSuperAmmor( SHitParam &HitParam );
	virtual void CheckDamageVelocity( DnActorHandle hActor );
	virtual void Look( EtVector2 &vVec, bool bForceRotate=true )
	{
		MAMovementInterface::Look( vVec, bForceRotate );

		if( bForceRotate )
		{
			if( strstr( m_hActor->GetCurrentAction(), "Turn" ) )
			{
				SetActionQueue( "Stand" );
			}
		}
	}
};