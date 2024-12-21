#pragma once
#include "dnblow.h"
#ifndef _GAMESERVER
#include "DnComboCalculator.h"
#endif

class CDnCantMoveBlow;
class CDnCantActionBlow;
class CDnFrameStopBlow;


// 결빙 상태효과
class CDnFreezingBlow : public CDnBlow, public TBoostMemoryPool< CDnFreezingBlow >
{
private:
	CDnCantMoveBlow*		m_pCantMoveBlow;
	CDnCantActionBlow*		m_pCantActionBlow;
	CDnFrameStopBlow*		m_pFrameStopBlow;

	DWORD					m_dwOriSpeedFrame;
	//float					m_fOriFPS;

	// 피격횟수로 차감되는 내구도
	int						m_iDurabilityCount;

	// 플레이어 캐릭터가 걸린건가
	bool					m_bPlayerCharacter;

	// 연타로 위기 탈출
#ifndef _GAMESERVER
	CDnComboCalculator m_ComboCalc;

	vector<string> m_vlBreakActions;
	bool m_bShake;
	LOCAL_TIME m_ShakeStartTime;

	void _ProcessShake( LOCAL_TIME LocalTime, float fDelta );
#endif

	// 현재 맵이 네스트인가.
	bool m_bNestMap;

	void _ForceLoopEnd( LOCAL_TIME LocalTime, float fDelta );

public:
	CDnFreezingBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFreezingBlow(void);

public:
#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

protected:
	void CheckBossMonster();

#if defined(PRE_FIX_51048)
public:
	void RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta);
#endif // PRE_FIX_51048
};
