#pragma once
#include "dnblow.h"


//////////////////////////////////////////////////////////////////////////
//���ݿ�
//ũ��Ƽ�� ���ݽ�, ũ��Ƽ�� Ȱ���� n% ���� ��Ű�� ����ȿ���� n�� ���� ����
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
	int m_nCoolTime;		//�߰��� ����ȿ�� ���ӽð� & �� ����ȿ�� ��Ÿ��
	float m_fLeftCoolTime;		
	bool m_bActivated;
	float m_fCriticalRate;	//ũ��Ƽ�� ���� ��ġ

	void SetInfo(const char* szValue);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};

//////////////////////////////////////////////////////////////////////////
//����
//HP�� n%�̸��� �Ǵ� ���, ������ ���� n% ����
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
	float m_fLimitHPRate;	//���� HP����.
	float m_fDamagRate;		//������ ���� ����
	int m_nCoolTime;		//������ ���� ����ȿ�� ���ӽð�
	float m_fLeftCoolTime;	//������ ��Ÿ��..
	bool m_bActivated;		//Ȱ��ȭ ����

	void SetInfo(const char* szValue);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};

//���� ���� ��ų�� ������� �з� �Ǵ� ����ȿ����

//1. ��ɽ� ����ġ �߰� n%����
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

//2. �� ����ġ n%����
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

//3. �������� ĳ���� �̵��ӵ� n%����
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

//4. Ż�� �̵� �ӵ� n%����
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

//5. ĳ���� ����� ������ ������ �ʴ� ��ų(����ø�)
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

//6. NPC���� ������ �� ȣ���� 10%����
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

//7. �ݷμ��� �޴� n�� �߰� ȹ��
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

//8. ���� ��� �ð� n% ����
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

//9. ���� ���� ������ 1% ����
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

//10. �ڵ� ���� �ð� n�� ����
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

//11. �丮 ���� n% ����
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

//12. ��ȭ ȹ�淮 n%����
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

//13. �ñر� ��Ÿ�� ����
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
//��ȯ�� �ý��� ���� ����ȿ��
//��Ƽ���� ����� n%���
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






