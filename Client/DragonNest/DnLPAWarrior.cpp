#include "StdAfx.h"
#include "DnLPAWarrior.h"
#include "InputWrapper.h"
#include "EtSoundEngine.h"
#include "DnWeapon.h"
#include "DnSkill.h"
#include "VelocityFunc.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnLPAWarrior::CDnLPAWarrior( int nClassID, bool bProcess )
: TDnPlayerWarrior< CDnLocalPlayerActor >( nClassID, bProcess )
{
}

CDnLPAWarrior::~CDnLPAWarrior()
{
}

void CDnLPAWarrior::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerWarrior< CDnLocalPlayerActor >::Process( LocalTime, fDelta );
}