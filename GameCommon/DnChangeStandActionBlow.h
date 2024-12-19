#pragma once
#include "dnblow.h"

class CDnCantMoveBlow;


//�⺻ ������ �����Ͽ� ������ �Ͽ��� �÷��� ������ �ٸ��� ���ֱ� ���� ȿ���Դϴ�.
//�켱������ ����
//
//ȿ�� �ε���: 121
//ȿ�� ����
//���ӽð� ���� ĳ������ �⺻ Stand�׼��� ĳ���� Act���� ���� ������ Ani�� �����Ѵ�.
//�Ķ���Ϳ��� ����� �׼� �̸��� �Է��Ѵ�.

// ���� �׼ǰ� �� �׼��� �����ؾ� ��.. ��� ������ �ʰ� assert. 
// ����/�� �׼��� ������ ���ڿ�������. 
// ChangeActionSet ����ȿ���� ������ �׼� �̸��� ��ü�Ѵٴ� ���� �ٸ���.
class CDnChangeStandActionBlow : public CDnBlow, public TBoostMemoryPool< CDnChangeStandActionBlow >
{
private:
	string m_strActionPrefixName;
	string m_strStartActionName;
	string m_strLoopActionName;
	string m_strEndActionName;
	bool m_bIgnoreEndAction;
	
public:
	CDnChangeStandActionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeStandActionBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	// #30882 �� �׼��� ���Ƿ� �ñ׳η� �ٲٴ� ��쵵 �ִ�.
	void SetEndAction( const char* pEndAction );

#ifdef _GAMESERVER
	// ���ĵ� �׼� ���濡�� �Ļ��Ǵ� �׼��ΰ�.
	bool IsChangeActionSet( const char* szActionName );
	static void ReleaseStandChangeSkill( DnActorHandle hActor, bool bCheckUnRemovableSkill, const char* szEndAction = NULL );
	static bool CheckUsableAction( DnActorHandle hActor, bool bCheckUnRemovableSkill, const char* szActionName );
#endif
	
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
