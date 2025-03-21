#pragma once
#include "DnBlow.h"
#include "DnActorStatIntervalManipulator.h"


// 전염병 상태효과.
//효과 Index : 136
//
//효과 Name : DISEASE
//
//효과 설명
//1) 전염병 속성을 지닌 공격에 피격될 경우 "전염병" 디버프에 걸립니다.
//
//2) 피격된 자신과 <지정된 거리>안에 있는 아군에게 <효과시간>동안 <해당 스킬에 의해 계산된 데미지최종값> * <효과수치(%)> 데미지를 입힙니다.
//※데미지는 현재 구현되어있는 <중독>과 동일합니다.
//
//3) <지정된 확률>로 피해를 입힌 아군에게 전염병을 옮깁니다.
//
//효과 예외 처리 
//1) 사망 시 포인트는 차져나 이그니션과 마찮가지로 "시전자"가 획득 하게 됩니다.
//
//사용용도
//유저간의 거리를 벌리는 행위 유도
class CDnPlagueBlow : public CDnBlow
{
private:
	CDnActorStatIntervalManipulator m_IntervalChecker;
	float m_fDamageProb;
	float m_fDamageAbsolute;
	float m_fPlagueRange;		// 클라에선 필요 없으면 나중에 빼자.
	float m_nPlageProbability;

	float m_fEffectLength;
	bool m_bEffecting;

#ifdef _GAMESERVER
	float m_fIntervalDamage;
#endif

public:
	CDnPlagueBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPlagueBlow(void);

	virtual float GetStatDelta( void ) { return m_fValue; };
	virtual bool OnCustomIntervalProcess( void );

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
