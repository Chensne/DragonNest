#pragma once
#include "IDnSkillUsableChecker.h"


// HP 체커, [정해준 기준HP (operator) 현재 HP] 로 놓고 비교합니다.
class CDnHPChecker : public IDnSkillUsableChecker, public TBoostMemoryPool< CDnHPChecker >
{
private:
	int			m_iOperator;
	float		m_fStandardRatio;

public:
	CDnHPChecker( DnActorHandle hActor, float fStandardRatio, int iOperator );
	virtual ~CDnHPChecker(void);

	// from IDnSkillUsableChecker
	bool CanUse( void );
	int GetNumArgument( void ) { return 2; };

	virtual IDnSkillUsableChecker* Clone();
};
