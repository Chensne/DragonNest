#pragma once

#include "DnMonsterActor.h"
#include "MAWalkMovement.h"
#include "MAWalkMovementNav.h"
#include "MASingleBody.h"
#include "MASingleDamage.h"

class CDnNormalMonsterActor : public TDnMonsterActor< MASingleBody, MASingleDamage >
{
public:
	CDnNormalMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnNormalMonsterActor();

	virtual MAMovementBase* CreateMovement();

	virtual void CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	virtual void SwapActor( int nActorIndex ) override;
#endif


protected:
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void UpdateNaviMode();

	LOCAL_TIME m_nLastUpdateNaviTime;
};

class IBoostPoolDnNormalMonsterActor : public CDnNormalMonsterActor, public TBoostMemoryPool< IBoostPoolDnNormalMonsterActor >
{
public:
	IBoostPoolDnNormalMonsterActor( CMultiRoom *pRoom, int nClassID ):CDnNormalMonsterActor( pRoom, nClassID ){}
	virtual ~IBoostPoolDnNormalMonsterActor(){}
};
