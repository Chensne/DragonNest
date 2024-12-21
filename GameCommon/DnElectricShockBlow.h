#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnCantMoveBlow;
class CDnCantActionBlow;
class CDnBasicBlow;

// 감전 상태효과. 기본적으로 5초마자 15%씩 경직이 발생한다.
// 확률을 사용하므로 게임서버에서만 사용하는 객체입니다! 
class CDnElectricShockBlow : public CDnBlow, public TBoostMemoryPool< CDnElectricShockBlow >
{
private:
	LOCAL_TIME				m_StartTime;
	LOCAL_TIME				m_LastCheckTime;
	LOCAL_TIME				m_LastStiffStartTime;
	bool					m_bStiff;
	bool					m_bAirStiff;
	bool					m_bMyLocalPlayer;

	// 빛속성 조절 값이 추가됨. (#16000)
	float					m_fAddSuccessProb;
	float					m_fLightResistDelta;
	float					m_fDamageProb;
	float					m_fIntervalDamage;

	CDnCantMoveBlow*		m_pCantMove;
	CDnCantActionBlow*		m_pCantAction;
	CDnBasicBlow*			m_pLightDefenseBlow;

	// #27679 네임드, 보스, 8인 네스트보스에게는 효과는 적용되나 실제로 결빙의 부가효과가 적용되진 않는다.
	bool					m_bBossMonster;
	bool					m_bNestMap;

#if defined(_GAMESERVER)
protected:

	float CalcIntervalDamage(CDnState::ElementEnum haElement);
	virtual void OnSetParentSkillInfo();
#endif	// #if defined(_GAMESERVER)

public:
	CDnElectricShockBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnElectricShockBlow(void);

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnCmdActionFromPacket( const char* pActionName );

	virtual bool CalcDuplicateValue( const char* szValue );

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
