#pragma once
#include "dnblow.h"

/********************************************************************
	created:	2010/11/22
	author:		semozz
	
	purpose:	특정 스킬에 대한 무적 상태
*********************************************************************/
class CDNInvincibleAtBlow : public CDnBlow, public TBoostMemoryPool< CDNInvincibleAtBlow >
{
protected:
	bool m_bSendResist;

public:
	CDNInvincibleAtBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDNInvincibleAtBlow(void);

private:
	typedef std::map<int, int> INVINCIBLEAT_LIST;
	INVINCIBLEAT_LIST m_InvincibleAtList;

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//문자열로 설정되어 있는 무적대상 스킬ID들 처리(##;##;...;)
	void AddInvincibleAt(const char* szValue);

	//nSkillID에 대한 무적 설정을 추가한다.
	void AddInvincibleAt(int nSkillID);

	//nSkillID에 대한 무적 설정이 되어 있는지 확인
	bool IsInvincibleAt(int nSkillID);
	bool IsEnableSendResist();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
