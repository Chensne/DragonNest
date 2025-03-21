#pragma once
#include "dnblow.h"

class CDnFrameBlow;
class CDnBasicBlow;

// [동상] - Frostbite
// 효과 인덱스: 144
// 효과 시간동안 정해진 수치만큼 "프레임/이동속도"가 감소되고 추가로 "냉기 저항"도 감소
// 다른 프레임 감소 효과나 이동속도 증가효과와 중복될 경우 그 정도가 더해진다. (프레임 감소 30% + 동상 30% = 60% 프레임 감소)
// 대상의 "냉기 저항력"수치 %에 의해 지속시간이 증/감 된다. (냉기 저항력이 높을 경우 지속시간이 감소)
// 파라매터: 이동속도/프레임 증감 % ; 냉기저항 증감 %
// 
// 기존에 있는 상태효과들의 조합으로 구현한다.
class CDnFrostbiteBlow : public CDnBlow, 
					     public TBoostMemoryPool< CDnFrostbiteBlow >
{
private:
	//CDnFrameBlow* m_pFrameBlow;
	float m_fFrameBlowArg;
	CDnBasicBlow* m_pMoveSpeedBlow;
	CDnBasicBlow* m_pIceDefenseBlow;

	// #28385 네스트 맵인 경우 보스들에게 영향이 없는 상태효과로 걸림.
	bool m_bNestMap;

public:
	CDnFrostbiteBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFrostbiteBlow( void );

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

	float GetFrameBlowArg( void ) { return m_fFrameBlowArg; };

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

#if defined(PRE_FIX_51048)
public:
	void RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta);
#endif // PRE_FIX_51048
};
