#pragma once



// 상태 이상이라던지 기타 등등의 처리해야 하는 상황들의 추상화
class IDnSkillProcessor
{
public:
	//// 상태 이상 적용 타겟에 따른 분류
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
		APPLY_SE_WHEN_ACTIONSET_ENABLED,			// ChangeActionSet 상태효과에 의해 ActionProcessor 가 활성화 될 때만 스킬에 있는 상태효과들이 적용되도록 처리. (#21052)
		APPLY_SE_WHEN_TARGET_NORMAL_HIT,			// 평타로 피격된 대상에게 일정 확률로 스킬의 Target 으로 지정된 상태효과들을 부여하는 발현타입. 현재 아이템 접두어 시스템에서만 쓰임. (#23818)
		CALL_MY_SUMMONED_MONSTER,					// 자신이 소환한 몬스터를 소환하는 스킬.
		CHANGE_ACTIONSTR_BY_BUBBLE,					// 버블 갯수에 따라 스킬 액션이 변경된다.
		STATE_EFFECT_APPLY_ONOFF_BY_BUBBLE,			// 버블 갯수에 따라 유효한 상태효과 인덱스 설정.
		PLAY_PET_ANI,								// 팻의 애니메이션을 재생

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

	// 프로세싱이 끝났는지, 기본적인 쿼리 함수
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
