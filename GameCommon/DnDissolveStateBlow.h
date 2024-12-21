#pragma once
#include "dnblow.h"

/********************************************************************
	created:	2010/11/22
	author:		semozz
	
	purpose:	Ư�� ���¸� �����ϴ� StateBlow
*********************************************************************/
class CDnDissolveStateBlow : public CDnBlow, public TBoostMemoryPool< CDnDissolveStateBlow >
{
protected:
	

public:
	CDnDissolveStateBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnDissolveStateBlow(void);

private:
	typedef std::map<int, int> DISSOLVE_LIST;
	DISSOLVE_LIST m_DissolveStateIDList;

	int m_BeginLevel;	//���� ������ ����
	int m_EndLevel;		//���� ������ ��

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//������ StateBlowIndex���ڿ� ó��(##;##;##...;)
	void AddDissolveStateID(const char* szValue);

	//������ StateBlowIndex�� ����Ѵ�.
	void AddDissolveStateID(int nStateBlowIndex);

	void SetLevelInfo(int beginLevel, int endLevel)
	{
		m_BeginLevel = beginLevel;
		m_EndLevel = endLevel;
	}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
