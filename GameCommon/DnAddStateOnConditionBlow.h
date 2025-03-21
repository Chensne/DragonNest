#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnAddStateOnConditionBlow : public CDnBlow, public TBoostMemoryPool< CDnAddStateOnConditionBlow >
{
private:
	float m_fRate;				//확률
	int m_nHitStateBlowIndex;	//Hit시 상태 효과 Index
	int m_nHitAttackAttribute;	//Hit시 공격 속성
	int m_nDestStateBlowIndex;	//조건이 맞을때 추가될 상태효과 Index

	std::string m_strStateAttribute;
	int m_nStateDurationTime;

	void SetInfo(const char* szValue);
public:
	CDnAddStateOnConditionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddStateOnConditionBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual void OnTargetHit( DnActorHandle hTargetActor );
#else
	virtual bool OnCustomIntervalProcess( void );
	void ShowGraphicEffect(bool bShow);
#endif

#if defined(_GAMESERVER)
protected:
	//////////////////////////////////////////////////////////////////////////
	// 확률계산이 액터당 되지 않도록 하기위한 처리.
	bool m_bCalcProb;	//확률계산 여부
	bool m_bOnDamaged;	//맞았는지 확인.

	bool CalcProb();
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
