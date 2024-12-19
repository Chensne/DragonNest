#pragma once
#include "dnblow.h"


//////////////////////////////////////////////////////////////////////////
//공격용
//크리티컬 공격시, 크리티컬 활률을 n% 증가 시키는 상태효과를 n초 동안 적용
class CDnAddCriticalRateBlow : public CDnBlow, public TBoostMemoryPool<CDnAddCriticalRateBlow>
{
public:
	CDnAddCriticalRateBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddCriticalRateBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	bool IsActivated() { return m_bActivated; }
	float GetLeftCoolTime() { return m_fLeftCoolTime; }

#if defined(_GAMESERVER)
	void ApplyCriticalIncBlow();
#endif // _GAMESERVER
protected:
	int m_nCoolTime;		//추가될 상태효과 지속시간 & 이 상태효과 쿨타임
	float m_fLeftCoolTime;		
	bool m_bActivated;
	float m_fCriticalRate;	//크리티컬 증가 수치

	void SetInfo(const char* szValue);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};

//////////////////////////////////////////////////////////////////////////
//방어용
//HP가 n%미만이 되는 경우, 데미지 감소 n% 증가
class CDnDamageChagneBlow : public CDnBlow, public TBoostMemoryPool<CDnDamageChagneBlow>
{
public:
	CDnDamageChagneBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnDamageChagneBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	bool IsActivated() { return m_bActivated; }
	float GetLeftCoolTime() { return m_fLeftCoolTime; }
	float GetDamageRate() { return m_fDamagRate; }
	float GetHpLimit() { return m_fLimitHPRate; }

#if defined(_GAMESERVER)
	void ApplyDamageChange();
#endif // _GAMESERVER

protected:
	float m_fLimitHPRate;	//기존 HP비율.
	float m_fDamagRate;		//데미지 감소 비율
	int m_nCoolTime;		//데미지 감소 상태효과 지속시간
	float m_fLeftCoolTime;	//적용후 쿨타임..
	bool m_bActivated;		//활성화 여부

	void SetInfo(const char* szValue);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};

//통합 레벨 스킬의 기능으로 분류 되는 상태효과들

//1. 사냥시 경험치 추가 n%증가
class CDnExpIncBlow : public CDnBlow, public TBoostMemoryPool< CDnExpIncBlow >
{
public:
	CDnExpIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnExpIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//2. 펫 경험치 n%증가
class CDnPetExpIncBlow : public CDnBlow, public TBoostMemoryPool< CDnExpIncBlow >
{
public:
	CDnPetExpIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPetExpIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//3. 마을에서 캐릭터 이동속도 n%증가
class CDnMoveSpeedIncBlow : public CDnBlow, public TBoostMemoryPool< CDnMoveSpeedIncBlow >
{
public:
	CDnMoveSpeedIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnMoveSpeedIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//4. 탈것 이동 속도 n%증가
class CDnVehicleMoveSpeedBlow : public CDnBlow, public TBoostMemoryPool< CDnVehicleMoveSpeedBlow >
{
public:
	CDnVehicleMoveSpeedBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnVehicleMoveSpeedBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//5. 캐릭터 사망시 내구도 깍이지 않는 스킬(사망시만)
class CDnIgnoreDurationBlow : public CDnBlow, public TBoostMemoryPool< CDnIgnoreDurationBlow >
{
public:
	CDnIgnoreDurationBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnIgnoreDurationBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//6. NPC에게 선물할 때 호감도 10%증가
class CDnNpcReputationIncBlow : public CDnBlow, public TBoostMemoryPool< CDnNpcReputationIncBlow >
{
public:
	CDnNpcReputationIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnNpcReputationIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//7. 콜로세움 메달 n개 추가 획득
class CDnAddMedalBlow : public CDnBlow, public TBoostMemoryPool< CDnAddMedalBlow >
{
protected:
	int m_nAddMedal;
	int m_nLimitMedal;

public:
	CDnAddMedalBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddMedalBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	int GetAddMedal() { return m_nAddMedal; }
	int GetLimitMedal() { return m_nLimitMedal; }

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//8. 농장 재배 시간 n% 감소
class CDnFarmingTimeBlow : public CDnBlow, public TBoostMemoryPool< CDnFarmingTimeBlow >
{
public:
	CDnFarmingTimeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFarmingTimeBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//9. 래더 상점 아이템 1% 할인
class CDnLadderStoreSaleBlow : public CDnBlow, public TBoostMemoryPool< CDnLadderStoreSaleBlow >
{
public:
	CDnLadderStoreSaleBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnLadderStoreSaleBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//10. 자동 낚시 시간 n초 감소
class CDnAutoFishingTimeBlow : public CDnBlow, public TBoostMemoryPool< CDnAutoFishingTimeBlow >
{
public:
	CDnAutoFishingTimeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAutoFishingTimeBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//11. 요리 숙련 n% 증가
class CDnCookingExpIncBlow : public CDnBlow, public TBoostMemoryPool< CDnCookingExpIncBlow >
{
public:
	CDnCookingExpIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCookingExpIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:
	int m_AddExpValue;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//12. 금화 획득량 n%증가
class CDnGoldIncBlow : public CDnBlow, public TBoostMemoryPool< CDnGoldIncBlow >
{
public:
	CDnGoldIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnGoldIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};

//13. 궁극기 쿨타임 감소
class CDnGlobalCoolTimeBlow : public CDnBlow, public TBoostMemoryPool< CDnGlobalCoolTimeBlow >
{
public:
	CDnGlobalCoolTimeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnGlobalCoolTimeBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};


//////////////////////////////////////////////////////////////////////////
//귀환자 시스템 관련 상태효과
//파티원의 드랍율 n%상승
class CDnPartyDropRateBlow : public CDnBlow, public TBoostMemoryPool< CDnPartyDropRateBlow >
{
public:
	CDnPartyDropRateBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPartyDropRateBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};






