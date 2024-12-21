#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnCantMoveBlow;
class CDnCantActionBlow;
class CDnBasicBlow;

// ���� ����ȿ��. �⺻������ 5�ʸ��� 15%�� ������ �߻��Ѵ�.
// Ȯ���� ����ϹǷ� ���Ӽ��������� ����ϴ� ��ü�Դϴ�! 
class CDnElectricShockBlow : public CDnBlow, public TBoostMemoryPool< CDnElectricShockBlow >
{
private:
	LOCAL_TIME				m_StartTime;
	LOCAL_TIME				m_LastCheckTime;
	LOCAL_TIME				m_LastStiffStartTime;
	bool					m_bStiff;
	bool					m_bAirStiff;
	bool					m_bMyLocalPlayer;

	// ���Ӽ� ���� ���� �߰���. (#16000)
	float					m_fAddSuccessProb;
	float					m_fLightResistDelta;
	float					m_fDamageProb;
	float					m_fIntervalDamage;

	CDnCantMoveBlow*		m_pCantMove;
	CDnCantActionBlow*		m_pCantAction;
	CDnBasicBlow*			m_pLightDefenseBlow;

	// #27679 ���ӵ�, ����, 8�� �׽�Ʈ�������Դ� ȿ���� ����ǳ� ������ ����� �ΰ�ȿ���� ������� �ʴ´�.
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
