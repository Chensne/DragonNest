#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnCurseBlow : public CDnBlow, public TBoostMemoryPool< CDnCurseBlow >
{
private:
	float m_fDamageRate;		// 공격력(스킬에 의한 증가분 제외) % 데미지
	float m_fDamage;			// 데미지 절대 값?
	float m_fDelayTime;			// 자체 쿨타임? 용 시간
	
	float m_fCoolTime;			// 현재 쿨타임? 남은 시간?
	bool m_bAtivate;			//데미지 계산 활성화?
	int m_nDamage;
public:
	CDnCurseBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCurseBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	virtual bool CalcDuplicateValue( const char* szValue );
	void OnAttackChange();
	float GetCoolTime() { return m_fCoolTime; }
	
protected:
	void RequestDamage();
	int CalcDamage();

	DnActorHandle m_hSkillUser;
#if defined(PRE_ADD_50903)
protected:
	virtual void OnSetParentSkillInfo();
#endif // PRE_ADD_50903

#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
