#pragma once
#include "DnBlow.h"

#ifndef _GAMESERVER
#include "DnComboCalculator.h"
#endif

//�ð��� ������ ���ߴ� ȿ��(�������� 0���� ����� ȿ��) �߰�
//ȿ�� �ε���: 146
//��� �����Ӱ��� ȿ���� �߿� ���� �켱�� �ȴ�. (�ٸ� ������ ���� ȿ���� �����Ѵ�.) 
class CDnFrameStopBlow : public CDnBlow, public TBoostMemoryPool< CDnFrameStopBlow >
{
public:
	CDnFrameStopBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnFrameStopBlow(void);

protected:

#if defined(PRE_FIX_53274)
	// �÷��̾� ĳ���Ͱ� �ɸ��ǰ�
	bool					m_bPlayerCharacter;

#ifndef _GAMESERVER
	// ��Ÿ�� ���� Ż��
	CDnComboCalculator m_ComboCalc;
#endif // _GAMESERVER
#endif // PRE_FIX_53274

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
#if defined(PRE_FIX_53274)
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
#endif // PRE_FIX_53274

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
