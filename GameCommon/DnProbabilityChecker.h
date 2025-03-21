#pragma once
#include "IDnSkillUsableChecker.h"


// 일정 확률로 체크.
class CDnProbabilityChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnProbabilityChecker >
{
private:
	float			m_fProbability;

public:
	CDnProbabilityChecker( DnActorHandle hActor, float fProbability );		// Probability 는 0부터 100까지
	virtual ~CDnProbabilityChecker(void);

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 1; };

	float GetProbability() { return m_fProbability; }

	virtual IDnSkillUsableChecker* Clone();
};
