#pragma once
#include "dnblow.h"

// ��� ����ȿ�� ���� ����ȿ��
class CDnFreezingExplosionBlow : public CDnBlow, 
							 public TBoostMemoryPool< CDnFreezingExplosionBlow >
{
private:
#ifdef _GAMESERVER
	//int m_iSkillUserAttackMMin;
	//int m_iSkillUserAttackMMax;

	int m_iOriAttackMMin;
	int m_iOriAttackMMax;

	// #28953 �� ����ȿ������ �ñ׳��� ���� hit �ñ׳η� �ǰݵǾ��� ��츦 �����ϱ� ���ؼ�. 
	bool m_bFreezingHitAction;
#endif

	CEtActionBase::ActionElementStruct* m_pActionElement;
	float m_fFrame;

	DWORD m_ActionDurationTime;
	vector<CEtActionSignal *> m_vlpSignalList;

protected:
#ifdef _GAMESERVER
	void _ApplyOtherSE( DnActorHandle hActor );
#endif

public:
	CDnFreezingExplosionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFreezingExplosionBlow(void);

	virtual bool CanBegin( void );

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnTargetHit( DnActorHandle hTargetActor );

	void SignalProcess( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
