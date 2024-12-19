#pragma once
#include "dnblow.h"

class CDnChangeActionStrProcessor;

//
//클레릭-팔라딘의 추가 스킬로 기본 공격을 강화하는 스킬을 넣으려 합니다.
//이에 추가적으로 필요한 스킬효과가 생겼습니다.
//기존에 stand액션 변경과 같은 컨셉입니다
//하지만 end처럼 다시 돌아오는 동작은 필요 없습니다.
//
//일반 공격 변경
//캐릭터의 일반 공격을 특정 정해진 액션으로 변경.
//기능상 지속시간 동안 Attack1 액션만 다른 액션으로 수정할 수 있으면 가능
//(이후는 input액션으로 연결)
//무기별 액션에서 다른 액션으로 수정하는 것보다 우선시 되어야 함
//효과 인덱스: 129 (Attack_Change)
//스킬명: 세크레드 해머링 - 지속시간 동안 일반 공격에 추가데미지가 붙는다.

// 129) 액션 대체 skill processor 를 사용하여 액션 이름을 CDnActor::SetActionQueue 에서 변경하는 상태효과.
class CDnChangeActionSetBlow : public CDnBlow, public TBoostMemoryPool< CDnChangeActionSetBlow >
{
private:
	string m_strTriggerAction;
	string m_strCancleAction;
	bool m_bEnable;
	CDnChangeActionStrProcessor* m_pChangeActionStrProcessor;

	bool m_bChangedActionBegun;

public:
	CDnChangeActionSetBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeActionSetBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void UpdateEnable( const char* pPrevAction, const char* pCurrentAction );
	CDnChangeActionStrProcessor* GetChangeActionStrProcessor( void );

#ifdef _GAMESERVER
	void OnChangeAction( void );
	void OnNotChangeAction( void );
#endif

#if defined(PRE_FIX_68645)
	bool IsEnable() { return m_bEnable; }
#endif // PRE_FIX_68645

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
