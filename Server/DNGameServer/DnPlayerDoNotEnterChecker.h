#pragma once

#include "DnPlayerChecker.h"

class CDnPlayerDoNotEnterChecker:public IDnPlayerChecker, public TBoostMemoryPool<CDnPlayerDoNotEnterChecker>
{
public:
	CDnPlayerDoNotEnterChecker( CDnPlayerActor* pActor );
	virtual ~CDnPlayerDoNotEnterChecker();


protected:
	int m_nInvalidCount;
	int m_nValidCount;
	LOCAL_TIME m_LastCheckTime;

	int m_nCheckerCount;

protected:
	bool IsInvalidPosition();

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	bool IsInvalidPlayer();
	void ResetInvalid();
};
