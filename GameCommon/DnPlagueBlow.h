#pragma once
#include "DnBlow.h"
#include "DnActorStatIntervalManipulator.h"


// ������ ����ȿ��.
//ȿ�� Index : 136
//
//ȿ�� Name : DISEASE
//
//ȿ�� ����
//1) ������ �Ӽ��� ���� ���ݿ� �ǰݵ� ��� "������" ������� �ɸ��ϴ�.
//
//2) �ǰݵ� �ڽŰ� <������ �Ÿ�>�ȿ� �ִ� �Ʊ����� <ȿ���ð�>���� <�ش� ��ų�� ���� ���� ������������> * <ȿ����ġ(%)> �������� �����ϴ�.
//�ص������� ���� �����Ǿ��ִ� <�ߵ�>�� �����մϴ�.
//
//3) <������ Ȯ��>�� ���ظ� ���� �Ʊ����� �������� �ű�ϴ�.
//
//ȿ�� ���� ó�� 
//1) ��� �� ����Ʈ�� ������ �̱״ϼǰ� ���������� "������"�� ȹ�� �ϰ� �˴ϴ�.
//
//���뵵
//�������� �Ÿ��� ������ ���� ����
class CDnPlagueBlow : public CDnBlow
{
private:
	CDnActorStatIntervalManipulator m_IntervalChecker;
	float m_fDamageProb;
	float m_fDamageAbsolute;
	float m_fPlagueRange;		// Ŭ�󿡼� �ʿ� ������ ���߿� ����.
	float m_nPlageProbability;

	float m_fEffectLength;
	bool m_bEffecting;

#ifdef _GAMESERVER
	float m_fIntervalDamage;
#endif

public:
	CDnPlagueBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPlagueBlow(void);

	virtual float GetStatDelta( void ) { return m_fValue; };
	virtual bool OnCustomIntervalProcess( void );

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
