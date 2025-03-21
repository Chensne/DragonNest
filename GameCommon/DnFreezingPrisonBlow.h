#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnCantMoveBlow;
class CDnCantActionBlow;
class CDnFrameStopBlow;

//설명
//타겟 : 아무런 행동을 취하지 못한다.
//해제 : 주변에 있는 동료 유저들이 타겟에 붙어있는 얼음을 공격하여 얼음의 내구력을 0 이하로 만들면 디버프 상태가 해제된다.
//지속시간 : 디버프 지속시간이 0을 초과하게 되는 순간 디버프 상태는 해제되지만 동시에 타겟에게 파이널 데미지를 입힌다.
//
//	   1. 내구도 : 내구도 HP값 입력
//
//	   2. 파이널데미지 : 물리/마법 공격력의 몇 %로 데미지를 입힐 것인지 값입력 필요
//	   예) P30
//	   M50
//	   10000(절대값으로 데미지)
//	   3. 초당 데미지 : 디버프 상태인 타겟에게 데미지를 1초당 지속시간동안 입힌다. 입력 방법은 위와 동일
//
//	   4. 지속시간 : 기존 지속시간과 동일
//	   5. 이펙트 : 디버프 상태에 있을 때에는 얼려있는 형태를 표현
//	   파이널 데미지를 입힐 때에 피해를 입히는 표현으로 총 2가지가 필요
class CDnFreezingPrisonBlow : public CDnBlow, 
							  public TBoostMemoryPool< CDnFreezingPrisonBlow >
{
private:
	CDnActorStatIntervalManipulator m_IntervalChecker;

	CDnCantMoveBlow*		m_pCantMoveBlow;
	CDnCantActionBlow*		m_pCantActionBlow;
	CDnFrameStopBlow*		m_pFrameStopBlow;

	float m_fIntervalDamage;
	int m_iDurability;				// 내구도. 모두 소진되면 상태효과가 풀리게 된다.
	bool m_bDurabilityDestroyed;	// 내구도가 다 되어 파괴되었음.
	int m_iFinalDamage;				// 파괴 되기 전에 지속시간이 다 되면 지정된 데미지를 준다.
	int m_iOriginalTeam;			// 이 상태효과가 발동중인 액터의 원래 팀.

	float m_fOrigWight;				// 캐릭터의 기본 Weight값
	int m_iOrigPressLevel;			// 캐릭터의 기본 PressLevel값
	int m_iMaxDurability;			// 내구도 퍼센트값 계산을 위한 초기 내구도 값

protected:
#ifdef _GAMESERVER
	void _RequestDamage( float fDamage );
#endif

public:
	CDnFreezingPrisonBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFreezingPrisonBlow(void);

	virtual bool OnCustomIntervalProcess( void );

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if !defined(_GAMESERVER)
	void OnHit();
	EtVector3 GetGaugePos();
#endif

#ifdef _GAMESERVER
	virtual bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
