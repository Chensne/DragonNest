#pragma once
#include "DnBlow.h"

/*
	이슈 #16331

	새로운 스킬 효과가 필요해서 요청 드립니다.

	<스킬 효과를 필요로 하는 상황>
	아사이샤먼이 아군에게 버프를 시전하게 되면
	버프를 받은 몬스터들은 체력이0이 될 경우
	스킬을 시전하며 죽게 됩니다.(예: 챠지 볼트)

	<효과 정의>
	버프 형태의 스킬로
	해당 버프가 걸린 경우 체력이0이 되면
	죽으면서 스킬을 시전하게 됩니다.

	<개발요청내역>
	1. SkillTable의 효과종류(_EffectClass)에 새로운 효과 추가(예:DieSkill)
	2. SKillLevelTable의 효과수치(_EffectClassValue)에 사용할 스킬 인덱스와 스킬의 레벨을 지정할 수 있도록 한다.

	기획팀이 구두로 자세히 물어본 내용은 다음과 같습니다.

	- 이 상태효과가 걸려있는 몬스터들은 죽을 때 100% 스킬을 사용하면서 죽는다.
	- 죽으면서 스킬을 쓸 때는 일반적으로 죽을 때와 마찬가지로 무적 상태가 된다.
	- 현재로썬 죽을 때 특정 액션을 하는 스킬은 없으나 추후엔 추가될지도 모른다.
	- 현재는 느려짐, 결빙, 챠지볼트 번개 정도가 고려됨.
*/

class CDnInvincibleBlow;

class CDnUsingSkillWhenDieBlow : public CDnBlow
{
private:
	bool m_bDead;					// 죽은 상태임. OnDie 호출된 상태.
	bool m_bUsedDieSkill;			// 죽을 때 스킬 사용됐음.

public:
	CDnUsingSkillWhenDieBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnUsingSkillWhenDieBlow( void );

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void OnDie( void );

	// 죽어서 스킬이 나갔는지 안나갔는지.
	bool IsUsedSkill( void ) { return m_bUsedDieSkill; };

#if defined(_GAMESERVER)
#else
	//#44884 서버/클라이언트 동기화 문제로 서버에서 플래그 설정될때 패킷을 보내서 동기화 시킴.
	void ShowGraphicEffect(bool bShow) { 
		m_bUsedDieSkill = bShow; 
	}
#endif // _GAMESERVER

#if defined(PRE_FIX_44884)
protected:
	DnActorHandle m_FinalHitterActor;		//Die 호출한 액터 저장..
public:
	void SetFinalHitterActor(DnActorHandle hHitter) { m_FinalHitterActor = hHitter; }
#endif // PRE_FIX_44884


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
