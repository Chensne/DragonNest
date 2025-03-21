#pragma once
#include "IDnSkillProcessor.h"

class CDnCheckKeyProcess : public IDnSkillProcessor, public TBoostMemoryPool< CDnCheckKeyProcess >
{
protected:
	string				m_strActionName;
	string				m_strOriginalActionName;	// 이게 설정된 동안은 한번만 다른 액션으로 된다.

	vector<string>		strTokens;

public:
	CDnCheckKeyProcess(void) { m_iType = CHECK_KEY; m_fTimeLength = 0.0f; }; // 임시 스킬 백업용으로 만들어놓은 생성자. 반드시 필요한 변수들 초기화.
	CDnCheckKeyProcess(DnActorHandle hActor, const char* pActionName);

	virtual ~CDnCheckKeyProcess(void);

	const char* GetActionName(void) { return m_strActionName.c_str(); };

	// from IDnSkillProcessor
	void OnBegin(LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill);
	void Process(LOCAL_TIME LocalTime, float fDelta);
	void OnEnd(LOCAL_TIME LocalTime, float fDelta);
	int GetNumArgument(void) { return 1; };
	void CopyFrom(IDnSkillProcessor* pProcessor);

	// 프로세싱이 끝났는가~
	virtual bool IsFinished(void);

};