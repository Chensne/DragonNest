#pragma once
#include "IDnSkillUsableChecker.h"

class CDnHitChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnHitChecker >
{
protected:

public:
	CDnHitChecker( DnActorHandle hActor );
	virtual ~CDnHitChecker(void);

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 0; };

	virtual IDnSkillUsableChecker* Clone();
};
