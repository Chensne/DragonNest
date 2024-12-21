
#pragma once

#include "SignalHeader.h"

struct	ActionTable;
struct	MonsterSkillTable;
class	MAAiScript;

class CMAAiMultipleTarget
{
public:

	enum MultipleTargetType
	{
		Action = 0,
		Skill,
		Max,
	};

	CMAAiMultipleTarget();
	~CMAAiMultipleTarget();

	void	Reset();
#if defined (PRE_MOD_AIMULTITARGET)
	int		GetMultipleTarget(){ return m_nMultipleTarget; }
	bool	CalcTargetByAggro(DnActorHandle hActor);
#else
	bool	bIsMultipleTarget(){ return m_bIsMultipleTarget; }
#endif

	void	SetMultipleTargetAction( ActionTable* pActionTable );
	void	SetMultipleTargetSkill( MonsterSkillTable* pSkillTable );

	void	CalcTarget( DnActorHandle hActor, MAAiScript* pScript );
	void	CreateProjectile( CDnMonsterActor* pMonsterActor, ProjectileStruct* pStruct, int iSignalIndex );

	MultipleTargetType	GetType(){ return m_eType; }

private:

#if defined (PRE_MOD_AIMULTITARGET)
	int						m_nMultipleTarget;
	int						m_nSummonerTarget;
#else
	bool					m_bIsMultipleTarget;
#endif
	int						m_iMaxMultipleTargetCount;	// 이 값이 0이면 기존과 마찬가지로 모든 타겟
	bool					m_bIsAnyMultipleTarget;
	bool					m_bIsExceptCannonPlayer;	// 캐논모드의 유저는 무시한다.

	MultipleTargetType		m_eType;

	// MultipleTargetType::Action
	ActionTable*				m_pActionTable;
	std::vector<EtVector3>		m_vTargetPos;
	DNVector(DnActorHandle)		m_vTargetActor;


	// MultipleTargetType::Skill
	MonsterSkillTable*			m_pSkillTable;
};
