#pragma once
#include "IDnSkillUsableChecker.h"
#include "DnActorState.h"

// 해당 액터의 상태를 체크
class CDnActorStateChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnActorStateChecker >
{
private:
	int									m_iOperator;
	CDnActorState::ActorStateEnum		m_eRequiredState;
	

public:
	CDnActorStateChecker( DnActorHandle hActor, CDnActorState::ActorStateEnum iRequiredState, int iOperator );
	virtual ~CDnActorStateChecker(void);

	bool CanUse( void );
	int GetNumArgument( void ) { return 2; };

	virtual IDnSkillUsableChecker* Clone();
};
