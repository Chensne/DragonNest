#pragma once
#include "IDnSkillProcessor.h"



class CDnPlayAniProcess : public IDnSkillProcessor, public TBoostMemoryPool< CDnPlayAniProcess >
{
protected:
	string	m_strActionName;
	string	m_strChainActionName;
	string	m_strLandingActionName;		// 곧바로 착지액션으로 이어지는 액션인 경우 착지액션까지 끝나야 스킬이 끝난 것으로 판단하기 위해.
	string	m_strOriginalActionName;	// 이게 설정된 동안은 한번만 다른 액션으로 된다.

#ifdef PRE_ADD_ADDITIOANL_SKILL_ACTION
	std::vector<std::string> m_vecAdditionalAction;
#endif

protected:
	void CheckLandingAction( const char* pActionName, DnActorHandle hActor );

public:
	CDnPlayAniProcess( void ) { m_iType = PLAY_ANI; m_fTimeLength = 0.0f; }; // 임시 스킬 백업용으로 만들어놓은 생성자. 반드시 필요한 변수들 초기화.
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

	// 프로세싱이 끝났는가~
	virtual bool IsFinished( void );

	// 체인 액션 요청이 들어왔을 경우 액션 끝나는 시간을 늘린다.
	void OnChainInput( const char* pActionName );

	void ChangeActionNameOnce( const char* pActionName );
	void RestoreActionNameOnce( void );


public:
	bool CheckAnimation();
};
