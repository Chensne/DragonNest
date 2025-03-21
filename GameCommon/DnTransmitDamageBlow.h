#pragma once
#include "dnblow.h"

//주인 액터로 데미지를 전달
class CDnTransmitDamageBlow: public CDnBlow, 
								 public TBoostMemoryPool< CDnTransmitDamageBlow >
{
public:
	CDnTransmitDamageBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnTransmitDamageBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//Freezing과 같은 데미지 보정 상태효과가 같이 적용 된 경우
	//계산 순서에 따라 최종 데미지가 달라 질 수 있다.
	//51번 상태효과가 있는 경우 최종 데미지를 주인 액터로 전달 하도록 수정한다.
#if defined(_GAMESERVER)
#if defined(PRE_FIX_61382)
	void TransmitDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#else
	float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif // PRE_FIX_61382
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
