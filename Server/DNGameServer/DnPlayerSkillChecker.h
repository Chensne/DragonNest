#pragma once

#include "DnPlayerChecker.h"
#include "DnSkill.h"

class CDnPlayerSkillChecker :public IDnPlayerChecker, public TBoostMemoryPool<CDnPlayerSkillChecker>
{
public:
	CDnPlayerSkillChecker( CDnPlayerActor *pPlayer );
	virtual ~CDnPlayerSkillChecker();

protected:
	int m_nInvalidCount;
	DWORD m_LastCheckTime;

public:
	bool IsInvalidPlayer();
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void OnInvalidUseSkill( int nSkillID, CDnSkill::UsingResult Result );
};