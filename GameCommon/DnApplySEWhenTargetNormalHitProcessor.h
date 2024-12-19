#pragma once
#include "idnskillprocessor.h"
#include "DnSkill.h"

#ifdef _GAMESERVER

// ������ ���ξ� �ý��ۿ��� ����ϴ� ��ų���� ����� ����Ÿ��. (#23818)
// ��Ÿ�� �ǰݵ� ��󿡰� ���� Ȯ���� ��ų�� Target ���� ������ ����ȿ������ �ο��Ѵ�.
// ���� ��Ÿ�� ������ ���Ե� ���� �ִ�.
class CDnApplySEWhenTargetNormalHitProcessor : public IDnSkillProcessor, 
											   public TBoostMemoryPool< CDnApplySEWhenTargetNormalHitProcessor >
{
private:
	float m_fProbability;

public:
	CDnApplySEWhenTargetNormalHitProcessor( DnActorHandle hActor, float fProbability );
	virtual ~CDnApplySEWhenTargetNormalHitProcessor( void );

	void OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void OnNormalHitSuccess( DnActorHandle hHittedTarget );		// �Ϲ� ��Ÿ�� �������� ��.

	int GetNumArgument( void ) { return 0; };
};
#endif // #ifdef _GAMESERVER
