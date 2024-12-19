#pragma once
#include "dnblow.h"

//ȿ�� Index : 68
//
//��ġ Type : float
//
//���� : MP �Ҹ� ��ų ��� �� <ȿ�� ��ġ1>�� ������ Ȯ���� <ȿ�� ��ġ2>*<����� MP> ��ŭ ������ ���� or �Һ� �մϴ�.
//
//	 ��� ��� : 
//- ���� �Ǵ� �нú� ��ų�� ���� Ȯ���� ���� ��ų�� MP �Ҹ��� �ٿ��ְų�
//- ������� �ɾ� ����� MP �Һ��� �ø� �� �ֽ��ϴ�.(����)
class CDnPayBackMPBlow : public CDnBlow, public TBoostMemoryPool< CDnPayBackMPBlow >
{
private:
	float m_fApplyProb;
	float m_fConsumeDecreaseProb;

#ifndef _GAMESERVER
	bool m_bShowActivateEffect;
	LOCAL_TIME m_ShowActivateEffectTime;
	LOCAL_TIME m_PrevFrameLocalTime;
#endif

public:
	CDnPayBackMPBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPayBackMPBlow();

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#ifdef _GAMESERVER
	virtual int OnUseMP( int iMPDelta );
#else
	void OnActivate( void );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
