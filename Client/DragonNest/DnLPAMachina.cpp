#include "StdAfx.h"
#include "DnLPAMachina.h"
#include "InputWrapper.h"
#include "EtSoundEngine.h"
#include "DnWeapon.h"
#include "DnSkill.h"
#include "VelocityFunc.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnLPAMachina::CDnLPAMachina( int nClassID, bool bProcess )
: TDnPlayerMachina< CDnLocalPlayerActor >( nClassID, bProcess )
{
}

CDnLPAMachina::~CDnLPAMachina()
{
}

void CDnLPAMachina::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerMachina< CDnLocalPlayerActor >::Process( LocalTime, fDelta );
}