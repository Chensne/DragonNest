#include "StdAfx.h"
#include "DnLPACleric.h"
#include "InputWrapper.h"
#include "EtSoundEngine.h"
#include "DnWeapon.h"
#include "DnSkill.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnLPACleric::CDnLPACleric( int nClassID, bool bProcess )
: TDnPlayerCleric< CDnLocalPlayerActor >( nClassID, bProcess )
{
}

CDnLPACleric::~CDnLPACleric()
{
}

void CDnLPACleric::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnPlayerCleric< CDnLocalPlayerActor >::Process( LocalTime, fDelta );
}
