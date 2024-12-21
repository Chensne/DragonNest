#pragma once
#include "dnblow.h"

/********************************************************************
	created:	2011/02/11
	author:		semozz
	
	purpose:	특정 스킬만 사용 가능 하도록 제한
*********************************************************************/
class CDnAllowedSkillsBlow : public CDnBlow, public TBoostMemoryPool< CDnAllowedSkillsBlow >
{
protected:
	

public:
	CDnAllowedSkillsBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAllowedSkillsBlow(void);

private:
	typedef std::map<int, int> ALLOW_SKILL_LIST;
	ALLOW_SKILL_LIST m_AllowSkillList;

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//문자열로 설정되어 있는 무적대상 스킬ID들 처리(##;##;...;)
	void AddAllowSkills(const char* szValue);

	//nSkillID에 대한 무적 설정을 추가한다.
	void AddAllowSkill(int nSkillID);

	//nSkillID에 대한 무적 설정이 되어 있는지 확인
	bool IsAllowSkill(int nSkillID);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
