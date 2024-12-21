#pragma once
#include "idnskillprocessor.h"

class CDnPartyTask;

// 오라 스킬의 전반적인 처리를 맡는 Processor. 게임서버에서만 사용된다.
class CDnAuraProcessor : public IDnSkillProcessor, public TBoostMemoryPool< CDnAuraProcessor >
{
private:
	float				m_fRange;
	float				m_fRangeSQ;
	LOCAL_TIME			m_LastAuraCheckTime;
	set<DWORD>			m_setPartyMemberInAura;

	// 걸어주었던 오라에 포함되어 있었던 상태효과들의 ID
	// 같은 상태효과가 다른 스킬에서 쓰일 수 있기 때문에 반드시 상태효과
	// 생성시 부여되는 아이디로 구분해야 한다.
	map<DWORD, vector<int> > m_mapAuraMemberSEID;

	bool				m_bAuraOff;

public:
	CDnAuraProcessor( DnActorHandle hActor, float fRange );
	virtual ~CDnAuraProcessor(void);

	// from IDnSkillProcessor
	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	// 토글 스킬과 마찬가지로 껐다 켰다 하는 스킬
	bool IsFinished( void ) { return m_bAuraOff; };
	
	int GetNumArgument( void ) { return 1; };
};
