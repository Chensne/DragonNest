#pragma once

#include "DnPlayerChecker.h"

class CDnPlayerFloatingChecker:public IDnPlayerChecker, public TBoostMemoryPool<CDnPlayerFloatingChecker>
{
public:
	CDnPlayerFloatingChecker( CDnPlayerActor* pActor );
	virtual ~CDnPlayerFloatingChecker();


protected:
	int m_nInvalidCount;
	LOCAL_TIME m_LastCheckTime;
	std::vector<float> m_fVecHeightList;

protected:
	bool IsFloating();

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
};
