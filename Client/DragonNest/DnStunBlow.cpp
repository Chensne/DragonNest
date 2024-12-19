#include "StdAfx.h"
#include "DnStunBlow.h"

CDnStunBlow::CDnStunBlow()
{
}

CDnStunBlow::~CDnStunBlow(void)
{
}

void CDnStunBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}