#pragma once
#include "dnblow.h"
#ifndef _GAMESERVER
#include "DnComboCalculator.h"
#endif

#include "DnActorStatIntervalManipulator.h"

class CDnCantMoveBlow;
class CDnCantActionBlow;
class CDnFrameStopBlow;


// 결빙 상태효과
class CDnEscapeBlow : public CDnBlow, public TBoostMemoryPool< CDnEscapeBlow >
{
private:
	//////////////////////////////////////////////////////////////////////////
	//행동 불능 상태용
	CDnCantMoveBlow*		m_pCantMoveBlow;
	CDnCantActionBlow*		m_pCantActionBlow;
	CDnFrameStopBlow*		m_pFrameStopBlow;
	//////////////////////////////////////////////////////////////////////////

	DWORD					m_dwOriSpeedFrame;
	///float					m_fOriFPS;

// 	// 피격횟수로 차감되는 내구도
// 	int						m_iDurabilityCount;

	// 플레이어 캐릭터가 걸린건가
	bool					m_bPlayerCharacter;
	

	bool m_bAllowShakeControl;		//흔들림? 여부
	std::string m_strHitedAction;	//피격 액션

	bool m_isAbleIntervalChecker;
	CDnActorStatIntervalManipulator	m_IntervalChecker;

	float m_fOrigWight;
	int m_iOrigPressLevel;

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

public:
	CDnEscapeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnEscapeBlow(void);

public:
#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

	virtual bool OnCustomIntervalProcess( void );

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
