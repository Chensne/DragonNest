#pragma once
#include "dnblow.h"

// �� ���� ����ȿ��
//ȿ�� �ε���: 86
//ȿ�� ��ġ: ��ġ�� ���� ����
//ȿ�� ����:
//ȿ���ð� ���� ������ ���Ӽ��� ������Ų��.
//�Ʊ��� ��� ��������, ������ ��� �Ʊ�����
class CDnReverseTeamBlow : public CDnBlow
{
public:
	// #13885 �̽� ����. ��Ʈ �ñ׳� ���� Ÿ���̸� ��� ��Ʈ �ñ׳��� Ÿ���� ������.
	// �ش� ȿ���� �ɸ� ����� ����ϴ� �߻�ü�� �׼��� ��� hit�ñ׳��� TargetType�� DamageType�� �����ȴ�.
	// Enemy -> Friend
	// Friend -> Enemy
	// all �� ��쿡�� �������� �ʴ´�.
	// ����Ʈ�Ĺݿ� �����ϴ� ���� ��(1197)�� ����� �����Դϴ�.
	enum
	{
		HIT_SIGNAL_TARGET_CHANGE,
		TEAM_NUMBER_CHANGE,
		HIT_SIGNAL_TARGET_ALL,			// �� �Ʊ� ���о��� ��� Ÿ�ݵ�. (#24337)
	};

private:
	int m_iType;


public:
	CDnReverseTeamBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnReverseTeamBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	int GetType( void ) { return m_iType; };

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
