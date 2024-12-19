#include "StdAfx.h"
#include "DnRigidBlow.h"

CDnRigidBlow::CDnRigidBlow()
{
}

CDnRigidBlow::~CDnRigidBlow(void)
{
}

void CDnRigidBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}