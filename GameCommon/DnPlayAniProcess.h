#pragma once
#include "IDnSkillProcessor.h"



class CDnPlayAniProcess : public IDnSkillProcessor, public TBoostMemoryPool< CDnPlayAniProcess >
{
protected:
	string	m_strActionName;
	string	m_strChainActionName;
	string	m_strLandingActionName;		// ��ٷ� �����׼����� �̾����� �׼��� ��� �����׼Ǳ��� ������ ��ų�� ���� ������ �Ǵ��ϱ� ����.
	string	m_strOriginalActionName;	// �̰� ������ ������ �ѹ��� �ٸ� �׼����� �ȴ�.

#ifdef PRE_ADD_ADDITIOANL_SKILL_ACTION
	std::vector<std::string> m_vecAdditionalAction;
#endif

protected:
	void CheckLandingAction( const char* pActionName, DnActorHandle hActor );

public:
	CDnPlayAniProcess( void ) { m_iType = PLAY_ANI; m_fTimeLength = 0.0f; }; // �ӽ� ��ų ��������� �������� ������. �ݵ�� �ʿ��� ������ �ʱ�ȭ.
	CDnPlayAniProcess( DnActorHandle hActor, const char* pActionName );

	virtual ~CDnPlayAniProcess(void);

	const char* GetActionName( void ) { return m_strActionName.c_str(); };
	const char* GetChainActionName() { return m_strChainActionName.c_str(); }
	const char* GetLandingActionName() { return m_strLandingActionName.c_str(); }

	// from IDnSkillProcessor
	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	int GetNumArgument( void ) { return 1; };
	void CopyFrom( IDnSkillProcessor* pProcessor );

	// ���μ����� �����°�~
	virtual bool IsFinished( void );

	// ü�� �׼� ��û�� ������ ��� �׼� ������ �ð��� �ø���.
	void OnChainInput( const char* pActionName );

	void ChangeActionNameOnce( const char* pActionName );
	void RestoreActionNameOnce( void );


public:
	bool CheckAnimation();
};
