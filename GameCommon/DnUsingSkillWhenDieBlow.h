#pragma once
#include "DnBlow.h"

/*
	�̽� #16331

	���ο� ��ų ȿ���� �ʿ��ؼ� ��û �帳�ϴ�.

	<��ų ȿ���� �ʿ�� �ϴ� ��Ȳ>
	�ƻ��̻����� �Ʊ����� ������ �����ϰ� �Ǹ�
	������ ���� ���͵��� ü����0�� �� ���
	��ų�� �����ϸ� �װ� �˴ϴ�.(��: í�� ��Ʈ)

	<ȿ�� ����>
	���� ������ ��ų��
	�ش� ������ �ɸ� ��� ü����0�� �Ǹ�
	�����鼭 ��ų�� �����ϰ� �˴ϴ�.

	<���߿�û����>
	1. SkillTable�� ȿ������(_EffectClass)�� ���ο� ȿ�� �߰�(��:DieSkill)
	2. SKillLevelTable�� ȿ����ġ(_EffectClassValue)�� ����� ��ų �ε����� ��ų�� ������ ������ �� �ֵ��� �Ѵ�.

	��ȹ���� ���η� �ڼ��� ��� ������ ������ �����ϴ�.

	- �� ����ȿ���� �ɷ��ִ� ���͵��� ���� �� 100% ��ų�� ����ϸ鼭 �״´�.
	- �����鼭 ��ų�� �� ���� �Ϲ������� ���� ���� ���������� ���� ���°� �ȴ�.
	- ����ν� ���� �� Ư�� �׼��� �ϴ� ��ų�� ������ ���Ŀ� �߰������� �𸥴�.
	- ����� ������, ���, í����Ʈ ���� ������ �����.
*/

class CDnInvincibleBlow;

class CDnUsingSkillWhenDieBlow : public CDnBlow
{
private:
	bool m_bDead;					// ���� ������. OnDie ȣ��� ����.
	bool m_bUsedDieSkill;			// ���� �� ��ų ������.

public:
	CDnUsingSkillWhenDieBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnUsingSkillWhenDieBlow( void );

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void OnDie( void );

	// �׾ ��ų�� �������� �ȳ�������.
	bool IsUsedSkill( void ) { return m_bUsedDieSkill; };

#if defined(_GAMESERVER)
#else
	//#44884 ����/Ŭ���̾�Ʈ ����ȭ ������ �������� �÷��� �����ɶ� ��Ŷ�� ������ ����ȭ ��Ŵ.
	void ShowGraphicEffect(bool bShow) { 
		m_bUsedDieSkill = bShow; 
	}
#endif // _GAMESERVER

#if defined(PRE_FIX_44884)
protected:
	DnActorHandle m_FinalHitterActor;		//Die ȣ���� ���� ����..
public:
	void SetFinalHitterActor(DnActorHandle hHitter) { m_FinalHitterActor = hHitter; }
#endif // PRE_FIX_44884


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
