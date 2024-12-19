#pragma once
#include "dnblow.h"

class CDnProjectile;

// 효과 인덱스: 62
// 디버프 스킬
// 타겟에게 특정 디버프를 걸어 타겟의 몸에서부터 유저가 원하는 공격이 진행되도록 한다.
// 구체적 효과
// [효과시간] 동안 타겟은 [상태이상 이펙트 인덱스]에 지정된 상태이상 Act파일을 반복적으로 실행한다.
// 지정된 상태이상 Act파일에는 Hit시그널이나 발사체관련 시그널이 들어간다.
// 지정된 상태이상 Act파일이란 SkillLevelTable에서 설정하는 상태이상 이펙트 인덱스를 의미한다.
// 효과가 적용된 대상이 상태이상 Act파일을 실행하는 것은 대상의 실제 액션에는 영향을 주지 않는다.
// 상태이상 Act파일을 사용하는 이유는 발사체등의 방향, 속도 등을 조절하기 위함
//  상태이상 Act파일을 실행할때의 타겟 설정의 주체는 효과가 걸려있는 대상이 아닌, 스킬을 시전한 대상이다.
// 예> 유저가 몬스터에게 사용하면 몬스터 몸에서 지정된 상태이상 Act파일을 실행하는데,
// 해당 Act파일에 입력한 공격 대상은 사용한 유저 중심으로 적/아군을 구분한다.
// 상태이상 Act파일에서 실행되는 Hit시그널이나 발사체의 효과는 차져효과가 적용된 SkillTable에 차져효과 외에 다른 효과 종류를 입력해서 해당 효과가 적용되도록 한다.
// 효과 종류에서 Self의 주체는 시전자가 된다.
// 예> 스킬A의 SkillTable 스킬 효과에 효과 1번은 [차져], 효과 2번에는 [감전]이 적용되어 있다면
// 차져효과에 걸린 몬스터의 몸에 걸린 상태이상 act에서 발사되는 발사체에 맞은 몬스터는 [감전]상태에 걸린다.

// 기본적인 구현 방법은 원래 플레이어가 갖고 있는 액션의 시그널(hit or projectile)만 따로 뽑아서
// 해당 액터의 onsignal 로 처리시키되 위치만 이 상태효과를 당한(?) 액터의 위치로 바꿔서 처리시킨다.
class CDnChargerBlow : public CDnBlow, public TBoostMemoryPool< CDnChargerBlow >
{
private:
//#ifdef _GAMESERVER
	float m_fFrame;
	string m_strChargerActionName;
	CEtActionBase::ActionElementStruct* m_pActionElement;

	// 게임서버에서 m_pActionElement 가 OnSignal 돌고 NULL 이 되는 경우가 간혹 있어서 필요한 데이터를 미리 복사해놓음.
	vector<CEtActionSignal *> m_vlpSignalList;
	int m_iAdditionalWeaponLength;
	boost::shared_ptr<CDnState> m_pShooterStateSnapshot;

#if defined(PRE_FIX_65287)
	float m_fShooterFinalDamageRate;
#endif // PRE_FIX_65287
//#endif

public:
	CDnChargerBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChargerBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void OnProjectileSignal( CDnProjectile* pProjectile );

	// Process에서 OnSignal함수 호출 하던 기능을 이곳으로 옮김..
	virtual void SignalProcess( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
