#pragma once
#include "dnblow.h"

/********************************************************************
	purpose:	HitSignal�� ó���� �ϰ�, ����ȿ�� ���븸 ��ŵ �ϴ� ����ȿ��
*********************************************************************/
class CDnIngnoreEffectBlow : public CDnBlow, public TBoostMemoryPool< CDnIngnoreEffectBlow >
{
protected:
	

public:
	CDnIngnoreEffectBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnIngnoreEffectBlow(void);

private:
	typedef std::map<int, int> INVINCIBLEAT_LIST;
	INVINCIBLEAT_LIST m_InvincibleAtList;

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//���ڿ��� �����Ǿ� �ִ� ������� ��ųID�� ó��(##;##;...;)
	void AddInvincibleAt(const char* szValue);

	//nSkillID�� ���� ���� ������ �߰��Ѵ�.
	void AddInvincibleAt(int nSkillID);

	//nSkillID�� ���� ���� ������ �Ǿ� �ִ��� Ȯ��
	bool IsInvincibleAt(int nSkillID);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
