#pragma once
#include "IDnSkillProcessor.h"

class CDnDivideSEArgumentByTargets : public IDnSkillProcessor, public TBoostMemoryPool< CDnDivideSEArgumentByTargets >
{
private:
	int						m_iStateEffectArgument;

public:
	CDnDivideSEArgumentByTargets( DnActorHandle hActor, int iArgument );
	virtual ~CDnDivideSEArgumentByTargets(void);

	int GetStateEffectArgument( void ) { return m_iStateEffectArgument; };

	// from IDnSkillProcessor
	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	int GetNumArgument( void ) { return 1; };
};
