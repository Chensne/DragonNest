#pragma once

#include "DnActorState.h"
class CDnMonsterState : virtual public CDnActorState
{
public:
	CDnMonsterState();
	virtual ~CDnMonsterState();

	enum GradeEnum {
		Minion,
		Normal,
		Champion,
		Elite,
		Named,
		Boss,
		BossHP4,	// �ϵ��ھ ������忡�� ����ϴ� ����Ÿ��. HP4��¥���� ������ ����.
		NestBoss,
		NestBoss8,
	};

protected:
	int m_nDeadExperience;	// ����� ����ġ
	int m_nCompleteExperience; // �Ϸ�� ����ġ

	int m_nDeadDurability; // ���� ������ ��Ƽ ������ ����ġ
	GradeEnum m_Grade;
	int m_nItemDropGroupTableID;
	bool m_bSlowByDie;
	bool m_bSummonRandomFrame;
	bool m_bApplyEffectScale;
	int m_nRaceID;
	bool m_bBossKillCheck;

protected:
	virtual void CalcBaseState( StateTypeEnum Type = ST_All );
	virtual void CalcState( RefreshStateExtentEnum Extent, StateTypeEnum Type );
	virtual void GetStateList( int ListType, CDnState::ValueType Type, std::vector<CDnState *> &VecList );

public:
	virtual void Initialize( int nClassID );

	__forceinline int GetDeadExperience() { return m_nDeadExperience; }
	__forceinline int GetCompleteExperience() { return m_nCompleteExperience; }
	__forceinline int GetDeadDurability() { return m_nDeadDurability; }

	GradeEnum GetGrade() { return m_Grade; }
	bool IsSlowByDie() { return m_bSlowByDie; }
	bool IsSummonRandomFrame() { return m_bSummonRandomFrame; }
	bool IsApplyEffectScale() { return m_bApplyEffectScale; }
	int GetRaceID() {return m_nRaceID;}
	bool IsBossKillCheck() { return m_bBossKillCheck; }
#ifdef PRE_ADD_MONSTER_CHEAT
	int GetItemDropGroupID() { return m_nItemDropGroupTableID; }
#endif 
};