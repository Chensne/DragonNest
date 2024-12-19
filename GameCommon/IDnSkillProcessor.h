#pragma once



// ���� �̻��̶���� ��Ÿ ����� ó���ؾ� �ϴ� ��Ȳ���� �߻�ȭ
class IDnSkillProcessor
{
public:
	//// ���� �̻� ���� Ÿ�ٿ� ���� �з�
	//enum
	//{
	//	STATE_EFFECT_SELF,
	//	STATE_EFFECT_TARGET,
	//};
	enum
	{
		PLAY_ANI,
		PARTIAL_PLAY_ANI,
		CHANGE_PROJECTILE,
		DIVIDE_STATE_EFFECT_ARG,
		IMPACT_BLOW,
		AURA,
		CHANGE_ACTIONSTR,
		RANGE_STATE_EFFECT,
		APPLY_SE_WHEN_ACTIONSET_ENABLED,			// ChangeActionSet ����ȿ���� ���� ActionProcessor �� Ȱ��ȭ �� ���� ��ų�� �ִ� ����ȿ������ ����ǵ��� ó��. (#21052)
		APPLY_SE_WHEN_TARGET_NORMAL_HIT,			// ��Ÿ�� �ǰݵ� ��󿡰� ���� Ȯ���� ��ų�� Target ���� ������ ����ȿ������ �ο��ϴ� ����Ÿ��. ���� ������ ���ξ� �ý��ۿ����� ����. (#23818)
		CALL_MY_SUMMONED_MONSTER,					// �ڽ��� ��ȯ�� ���͸� ��ȯ�ϴ� ��ų.
		CHANGE_ACTIONSTR_BY_BUBBLE,					// ���� ������ ���� ��ų �׼��� ����ȴ�.
		STATE_EFFECT_APPLY_ONOFF_BY_BUBBLE,			// ���� ������ ���� ��ȿ�� ����ȿ�� �ε��� ����.
		PLAY_PET_ANI,								// ���� �ִϸ��̼��� ���

		CHECK_KEY= 16, //
		COUNT,
	};

protected:
	DnActorHandle					m_hHasActor;
	DnSkillHandle					m_hParentSkill;
	float							m_fTimeLength;
	int								m_iType;

protected:
	IDnSkillProcessor( DnActorHandle hActor );

public:
	IDnSkillProcessor( void ) : m_fTimeLength( 0.0f ), m_iType( 0 ) {};
	virtual ~IDnSkillProcessor( void );

	virtual void SetHasActor( DnActorHandle hActor ) { m_hHasActor = hActor; };
	virtual void SetParentSkill( DnSkillHandle hSkill ) { m_hParentSkill = hSkill; };

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill ) = 0;
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) = 0;
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta ) = 0;

	// ���μ����� ��������, �⺻���� ���� �Լ�
	virtual bool IsFinished( void ) { return (m_fTimeLength <= 0.0f); };

	virtual int GetNumArgument( void ) = 0;
	virtual void CopyFrom( IDnSkillProcessor* pProcessor ) {};

	int GetType( void ) { return m_iType; };

	// static factory
	static IDnSkillProcessor* Create( DnActorHandle hActor, int iProcessorIndex, 
									  int iSkillLevelTableID, /*IN OUT*/ int* pIParamOffset, /*IN OUT*/ set<string>& vlUseActions );

public:
	virtual bool CheckAnimation() { return true; }
};
