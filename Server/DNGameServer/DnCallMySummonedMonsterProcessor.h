#pragma once
#include "idnskillprocessor.h"


// 오라 스킬의 전반적인 처리를 맡는 Processor. 게임서버에서만 사용된다.
class CDnCallMySummonedMonsterProcessor : public IDnSkillProcessor, public TBoostMemoryPool< CDnCallMySummonedMonsterProcessor >
{
private:
	int m_iSummonedMonsterIDToCall;
	bool m_bCalled;

public:
	CDnCallMySummonedMonsterProcessor( DnActorHandle hActor, int iSummonedMosnterIDToCall );
	virtual ~CDnCallMySummonedMonsterProcessor(void);

	// from IDnSkillProcessor
	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	bool IsFinished( void ) { return m_bCalled; };
	
	int GetNumArgument( void ) { return 1; };
};
