#pragma once
#include "idnskillprocessor.h"
#include "DnSkill.h"


class CDnState;

// ���� ����ũ���� �ظӸ� ��ų ������ �߰��ϰ� ��. (#21052)
// ChangeActionSet �� ����ȿ���� ������ ����ũ���� �ظӸ� ���� ������ ����߸��� ����ȿ����
// �߰��ǰ� �Ǵµ� ������ ����ũ���� �ظӸ� �׼����� ġȯ�Ǿ� �׼��� �� ���� ChangeActionSet ��
// ������ ������ ����ȿ������ �߰��ϵ��� ó��..  �ڱ� �ڽſ��� �߰��ϴ� ����ȿ���� ����� �ȴ�.
// ���� ���ڴ� ���� ����.
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
