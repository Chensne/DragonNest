#include "StdAfx.h"
#include "DnAttackRateUpBlow.h"

CDnAttackRateUpBlow::CDnAttackRateUpBlow(void)
{
}

CDnAttackRateUpBlow::~CDnAttackRateUpBlow(void)
{
}

void CDnAttackRateUpBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}