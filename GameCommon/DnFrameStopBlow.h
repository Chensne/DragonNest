#pragma once
#include "DnBlow.h"

#ifndef _GAMESERVER
#include "DnComboCalculator.h"
#endif

//시간을 완전히 멈추는 효과(프레임을 0으로 만드는 효과) 추가
//효과 인덱스: 146
//모든 프레임관련 효과들 중에 가장 우선시 된다. (다른 프레임 조절 효과를 무시한다.) 
class CDnFrameStopBlow : public CDnBlow, public TBoostMemoryPool< CDnFrameStopBlow >
{
public:
	CDnFrameStopBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnFrameStopBlow(void);

protected:

#if defined(PRE_FIX_53274)
	// 플레이어 캐릭터가 걸린건가
	bool					m_bPlayerCharacter;

#ifndef _GAMESERVER
	// 연타로 위기 탈출
	CDnComboCalculator m_ComboCalc;
#endif // _GAMESERVER
#endif // PRE_FIX_53274

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
#if defined(PRE_FIX_53274)
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
#endif // PRE_FIX_53274

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
