#pragma once
#include "IDnSkillUsableChecker.h"
#include "DnActorState.h"

class CDnTargetStateChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnTargetStateChecker >
{
protected:
	CDnActorState::ActorStateEnum		m_eRequiredState;
	int									m_iOperator;
	float								m_fRadiusDistToSearch;


public:
	CDnTargetStateChecker( DnActorHandle hActor, CDnActorState::ActorStateEnum eRequiredState, int iOperator, float fRadiusDistToSearch );
	virtual ~CDnTargetStateChecker(void);

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 3; };

	virtual IDnSkillUsableChecker* Clone();
};
