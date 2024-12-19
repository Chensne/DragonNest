#pragma once
#include "DnBlow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnApplyStateBlowOnTimeBlow : public CDnBlow, public TBoostMemoryPool< CDnApplyStateBlowOnTimeBlow >
{
public:
	CDnApplyStateBlowOnTimeBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnApplyStateBlowOnTimeBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
protected:
	CDnActorStatIntervalManipulator	m_IntervalChecker;

	DnSkillHandle m_hSkill;	//������ ����ȿ���� ��ų �ڵ�

	bool OnCustomIntervalProcess( void );
	void ApplyStateEffect();
#endif // _GAMESERVER

protected:
	int m_nTime;	//�ð� ���� (1000 => 1��)
	float m_fRate;	//Ȯ��
	int m_nSkillID;	//������ ����ȿ���� ��ųID
	int m_nSkillLevel;	//������ ����ȿ���� ��ų ����

	void SetInfo(const char* szValue);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);

#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
