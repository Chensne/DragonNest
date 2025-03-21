#pragma once
#include "dnblow.h"


// 특정 스킬을 정해진 확률만큼 회피하는 상태효과.
// 특정 스킬의 hit 시그널들에서 데미지와 상태효과가 들어가지 않도록 한다.
class CDnProbInvincibleAtBlow : public CDnBlow,
								public TBoostMemoryPool< CDnProbInvincibleAtBlow >
{
protected:
	

public:
	CDnProbInvincibleAtBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnProbInvincibleAtBlow(void);

private:

#ifdef _GAMESERVER
	struct S_INVINCIBLE_INFO
	{
		int iMonsterID;
		map<int, float> mapSkillInfos;

		S_INVINCIBLE_INFO( void ) : iMonsterID( 0 ) {};
	};

	struct S_SKILL_INFO
	{
		DnSkillHandle hTargetSkill;
		bool bTargetSkillUsedProjectile;
		
		S_SKILL_INFO( void ) : bTargetSkillUsedProjectile( false ) {};
	};

	std::map<int, S_INVINCIBLE_INFO> m_mapInvincibleAt;
	map<int, float>* m_pNowMapSelectedSkillInfos;

	// 매 프로세스마다 end 된 상태인지 체크.
	//DnSkillHandle m_hTargetSkill;
	bool m_bNowSelectedSkillUsedProjectile;
	list<S_SKILL_INFO> m_listTargetSkills;
	map<int, bool> m_mapTargetSkillUsedProjectile;
	deque<DnSkillHandle> m_dqProbInvincibleFailedSkills;
#endif // #ifdef _GAMESERVER

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void AddInvincibleAt(const char* szValue);

#ifdef _GAMESERVER
private:
	void CheckAllTargetSkillState( void );
	void CheckTargetSkillStateAndDoEnd( DnSkillHandle hTargetSkill );
	void GatherEndTargetState( /*IN OUT*/ vector<DnSkillHandle>& vlhEndedTargetSkill );
	void CheckProbFailedSkillState( void );
	bool IsSkillFinished( DnSkillHandle hSkill );
	bool IsProbFailedSkillObject( DnSkillHandle hSkill );

public:
	//nSkillID에 대한 무적 설정이 되어 있는지 확인
	bool IsInvincibleAt( DnSkillHandle hTargetSkill );
	bool IsBeginInvincible( int iSkillID );
	S_SKILL_INFO* FindInvincibleTargetSkill( int iSkillID );
	void BeginInvincibleAt( DnSkillHandle hTargetSkill );
	void EndInvincibleAt( DnSkillHandle hTargetSkill );

	// 특정 스킬의 데미지라면 데미지를 0으로 만들고, 
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );

	// 특정 스킬에 대한 무적이 진행중이면 상태효과 들어가지 않도록.
	bool CanAddThisSkillsStateBlow( int iSkillID );

	// 그로 인한 상태효과도 들어가지 않도록 처리..
#else
	void OnSuccess( void );
#endif // _GAMESERVER

//#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
//public:
//	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
//	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
//#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
