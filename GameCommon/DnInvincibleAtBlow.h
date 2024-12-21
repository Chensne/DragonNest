#pragma once
#include "dnblow.h"

/********************************************************************
	created:	2010/11/22
	author:		semozz
	
	purpose:	Ư�� ��ų�� ���� ���� ����
*********************************************************************/
class CDNInvincibleAtBlow : public CDnBlow, public TBoostMemoryPool< CDNInvincibleAtBlow >
{
protected:
	bool m_bSendResist;

public:
	CDNInvincibleAtBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDNInvincibleAtBlow(void);

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
	bool IsEnableSendResist();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
