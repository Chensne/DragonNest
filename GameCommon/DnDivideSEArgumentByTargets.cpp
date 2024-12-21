#include "StdAfx.h"
#include "DnDivideSEArgumentByTargets.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDivideSEArgumentByTargets::CDnDivideSEArgumentByTargets( DnActorHandle hActor, int iArgument ) : IDnSkillProcessor( hActor ), m_iStateEffectArgument( iArgument )
{
	m_iType = DIVIDE_STATE_EFFECT_ARG;
}

CDnDivideSEArgumentByTargets::~CDnDivideSEArgumentByTargets(void)
{
}


void CDnDivideSEArgumentByTargets::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{

}


void CDnDivideSEArgumentByTargets::Process( LOCAL_TIME LocalTime, float fDelta )
{

}


void CDnDivideSEArgumentByTargets::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	
}