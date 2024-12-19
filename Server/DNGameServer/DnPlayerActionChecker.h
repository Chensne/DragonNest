#pragma once

#include "DnPlayerChecker.h"
#include "DnSkill.h"

class CDnPlayerActionChecker :public IDnPlayerChecker, public TBoostMemoryPool<CDnPlayerActionChecker>
{
public:
	CDnPlayerActionChecker( CDnPlayerActor *pPlayer );
	virtual ~CDnPlayerActionChecker();

protected:
	int m_nInvalidCount;
	DWORD m_LastCheckTime;

public:
	bool IsInvalidPlayer();
	void Process( LOCAL_TIME LocalTime, float fDelta );

	void OnInvalidAction();
};