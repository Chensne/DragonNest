#pragma once
#include "dnblow.h"

class CDnCantMoveBlow;


//기본 동작을 변경하여 유저로 하여금 플레이 느낌을 다르게 해주기 위한 효과입니다.
//우선순위가 높음
//
//효과 인덱스: 121
//효과 내용
//지속시간 동안 캐릭터의 기본 Stand액션을 캐릭터 Act파일 내에 지정된 Ani로 변경한다.
//파라매터에는 변경될 액션 이름을 입력한다.

// 시작 액션과 끝 액션이 존재해야 함.. 없어도 뻑나진 않고 assert. 
// 시작/끝 액션이 없으면 부자연스럽다. 
// ChangeActionSet 상태효과는 무조건 액션 이름만 대체한다는 것이 다르다.
class CDnChangeStandActionBlow : public CDnBlow, public TBoostMemoryPool< CDnChangeStandActionBlow >
{
private:
	string m_strActionPrefixName;
	string m_strStartActionName;
	string m_strLoopActionName;
	string m_strEndActionName;
	bool m_bIgnoreEndAction;
	
public:
	CDnChangeStandActionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeStandActionBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	// #30882 끝 액션을 임의로 시그널로 바꾸는 경우도 있다.
	void SetEndAction( const char* pEndAction );

#ifdef _GAMESERVER
	// 스탠드 액션 변경에서 파생되는 액션인가.
	bool IsChangeActionSet( const char* szActionName );
	static void ReleaseStandChangeSkill( DnActorHandle hActor, bool bCheckUnRemovableSkill, const char* szEndAction = NULL );
	static bool CheckUsableAction( DnActorHandle hActor, bool bCheckUnRemovableSkill, const char* szActionName );
#endif
	
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
