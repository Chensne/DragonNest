#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnAddStateOnTimerBlow : public CDnBlow, public TBoostMemoryPool< CDnAddStateOnTimerBlow >
{
private:
	float m_fRate;				//Ȯ��
	int m_nDestStateBlowIndex;	//������ ������ �߰��� ����ȿ�� Index

	std::string m_strStateAttribute;
	int m_nStateDurationTime;

	float m_fRange;		//Ž�� ����
	DWORD m_TimerTime;	//�ð� ����

	void SetInfo(const char* szValue);
public:
	CDnAddStateOnTimerBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddStateOnTimerBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual bool OnCustomIntervalProcess( void );
	
protected:
	CDnActorStatIntervalManipulator	m_IntervalChecker;
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
