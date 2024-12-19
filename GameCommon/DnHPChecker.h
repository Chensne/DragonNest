#pragma once
#include "IDnSkillUsableChecker.h"


// HP üĿ, [������ ����HP (operator) ���� HP] �� ���� ���մϴ�.
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
