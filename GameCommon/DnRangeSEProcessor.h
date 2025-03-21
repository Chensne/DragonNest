#pragma once
#include "IDnSkillProcessor.h"

// #8209 관련.
// 파이어 실드/아이스 실드 관련 스킬에 있는 target 상태효과를 일정 범위내에 일정 확률로
// 대상에게 추가시키는 processor 입니다.
// 서버에서만 생성되어 돌아갑니다.
class CDnRangeSEProcessor : public IDnSkillProcessor, public TBoostMemoryPool< CDnRangeSEProcessor >
{
private:
	float m_fRange;
	float m_fProbability;
	int m_iTargetEffectOutputID;
	int m_iDurationTime;

	bool m_bFinished;
	set<DWORD> m_setAffectedActor;

public:
	CDnRangeSEProcessor( DnActorHandle hActor, float fRange, float fProbability, int iTargetEffectOutputID, int iDurationTime );
	virtual ~CDnRangeSEProcessor(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	int GetNumArgument( void ) { return 4; };

	//bool IsFinished( void ) { return m_bFinished; }
};
