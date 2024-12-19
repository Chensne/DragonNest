#include "Stdafx.h"
#include "EtEffectHelper.h"
#include "EtSwordTrail.h"
#include "EtEffectTrail.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void InitilaizeEffect()
{
	CEtLineTrailEffect::InitializeEffect();
	CEtSwordTrail::InitializeEffect();
	CEtEffectTrail::InitializeEffect();
}

void FinializeEffect()
{
	CEtSwordTrail::FinializeEffect();
	CEtLineTrailEffect::FinializeEffect();
	CEtEffectTrail::FinializeEffect();
}
