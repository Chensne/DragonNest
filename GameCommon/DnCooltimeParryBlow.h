#pragma once
#include "DnBlow.h"
#include "DnObservable.h"

#ifndef _GAMSERVER
class CDnLocalPlayerActor;
#endif

class CDnCooltimeParryBlow : public CDnBlow,
							 public TBoostMemoryPool< CDnCooltimeParryBlow >,
							 // CDnPlayerActor 쪽에서 이 클래스로 static_cast 하여 Observer 객체를 넣는 코드가 있으므로 이 상속 코드만 제거되면 큰일남.
							 public CDnObservable
{
private:
#ifdef _GAMESERVER
	string m_strParringActionName;
	bool m_bEnable;
	bool m_bEnableLastLoop;
	float m_fAdditionalSignalProb;		// from ParringSignal
	float m_fAdditionalSignalProbLastLoop; // from ParringSignal
	
	DWORD m_dwCoolTime;
	DWORD m_dwLastParryingTime;
#else
	CDnLocalPlayerActor* m_pLocalPlayerActor;
	DnSkillHandle m_hParentSkillForCoolTime;			// 쿨타임을 위해 부모 스킬 id 로 객체를 생성해둔다. 쿨타임 표시용 임시임. 클라이언트에서만 사용한다.
#endif

public:
	CDnCooltimeParryBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnCooltimeParryBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	bool IsEnabled( void ) { return m_bEnable; };
	void EnableParrying( float fAdditionalProb ) { m_bEnable = true; m_fAdditionalSignalProb = fAdditionalProb; };
	void SetParringActionName( const char* pParringActionName ) { _ASSERT( pParringActionName ); m_strParringActionName.assign( pParringActionName ); };

	virtual bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
#else
	void OnSuccess( void );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};