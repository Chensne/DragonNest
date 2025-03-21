#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnMaxDamageAddBlow : public CDnBlow, public TBoostMemoryPool< CDnMaxDamageAddBlow >
{
public:
	CDnMaxDamageAddBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnMaxDamageAddBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	virtual void OnTargetHit( DnActorHandle hTargetActor );
#else
	virtual bool OnCustomIntervalProcess( void );
	void ShowGraphicEffect(bool bShow);
#endif

protected:
	float m_fRate;	//확률값

#if defined(_GAMESERVER)
	//////////////////////////////////////////////////////////////////////////
	// OnDamage()내에서 OnTargetHit가 호출되서 추가 데미지가 두번 적용 되는것 처럼 보여 지는 현상이 있음.
	// 그래서 다음 프레임에 추가 데미지를 처리 하기위한 처리를 위해 추가함.
	// 확률계산이 액터당 되지 않도록 하기위한 처리도 포함.
	bool m_bCalcProb;	//확률계산 여부
	bool m_bOnDamaged;	//맞았는지 확인.
	//int m_nAddDamag;	//추가 데미지
	
	bool CalcProb();
	void AddDamage();
	//////////////////////////////////////////////////////////////////////////
#else
	CDnActorStatIntervalManipulator	m_IntervalChecker;
	bool m_bGraphicEffectShow;
	bool m_bTrigger;
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
