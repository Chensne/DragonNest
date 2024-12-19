#pragma once

#include "DnNormalMonsterActor.h"
class CDnSimpleRushMonsterActor : public CDnNormalMonsterActor {
public:
	CDnSimpleRushMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnSimpleRushMonsterActor();
};

class IBoostPoolDnSimpleRushMonsterActor : public CDnSimpleRushMonsterActor, public TBoostMemoryPool< IBoostPoolDnSimpleRushMonsterActor >
{
public:
	IBoostPoolDnSimpleRushMonsterActor( CMultiRoom *pRoom, int nClassID ):CDnSimpleRushMonsterActor( pRoom, nClassID ){}
	virtual ~IBoostPoolDnSimpleRushMonsterActor(){}
};

#if defined( PRE_ADD_ACADEMIC )

class CDnAcademicAutomatMonsterActor : public CDnSimpleRushMonsterActor, public TBoostMemoryPool< CDnAcademicAutomatMonsterActor >
{
public:
	CDnAcademicAutomatMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnAcademicAutomatMonsterActor();

	virtual bool OnAINonTarget();
};

#endif // #if defined( PRE_ADD_ACADEMIC )

// ActorTypeEnum::NoAggroTrap
class CDnNoAggroTrapMonsterActor:public CDnNormalMonsterActor, public TBoostMemoryPool<CDnNoAggroTrapMonsterActor>
{
public:
	CDnNoAggroTrapMonsterActor( CMultiRoom* pRoom, int nClassID ):CDnNormalMonsterActor(pRoom,nClassID){}
	virtual ~CDnNoAggroTrapMonsterActor(){}

	virtual MAMovementBase* CreateMovement();
};