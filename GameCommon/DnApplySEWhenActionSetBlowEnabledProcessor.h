#pragma once
#include "idnskillprocessor.h"
#include "DnSkill.h"


class CDnState;

// 현재 세이크리드 해머링 스킬 때문에 추가하게 됨. (#21052)
// ChangeActionSet 도 상태효과기 때문에 세이크리드 해머링 동안 마공을 떨어뜨리는 상태효과가
// 추가되게 되는데 실제로 세이크리드 해머링 액션으로 치환되어 액션을 할 때만 ChangeActionSet 을
// 제외한 나머지 상태효과들을 추가하도록 처리..  자기 자신에게 추가하는 상태효과만 대상이 된다.
// 현재 인자는 따로 없다.
class CDnApplySEWhenActionSetBlowEnabledProcessor : public IDnSkillProcessor, 
													public TBoostMemoryPool< CDnApplySEWhenActionSetBlowEnabledProcessor >
{
private:
	vector<CDnSkill::StateEffectStruct*> m_vlpStateEffect;
	vector<int> m_vlAppliedStateEffectID;

public:
	CDnApplySEWhenActionSetBlowEnabledProcessor( DnActorHandle hActor );
	virtual ~CDnApplySEWhenActionSetBlowEnabledProcessor(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	void AddStateEffect( const CDnSkill::StateEffectStruct* pStateEffect );

	void OnChangeActionBegin( void );
	void OnChangeActionSetBlowEnd( void );
#endif

	int GetNumArgument( void ) { return 0; };
};
