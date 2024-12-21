#include "StdAfx.h"
#include "DnLPALencea.h"
#include "InputWrapper.h"
#include "EtSoundEngine.h"
#include "DnWeapon.h"
#include "DnSkill.h"
#include "VelocityFunc.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnLPALencea::CDnLPALencea( int nClassID, bool bProcess )
: TDnPlayerLencea< CDnLocalPlayerActor >( nClassID, bProcess )
{
}

CDnLPALencea::~CDnLPALencea()
{
}

void CDnLPALencea::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerLencea< CDnLocalPlayerActor >::Process( LocalTime, fDelta );
}