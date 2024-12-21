#pragma once
#include "DnBlow.h"

// #7189
// OBT 레이드 몬스터 켈베로스를 제작하면서 다양한 스킬을 설정하다가 새로운 효과가 필요하게되어
// 새로운 스킬 효과를 요청합니다.
// 일종의 [상태이상]과 같은 스킬로 주변에 아군에게 데미지를 주게 되는 효과입니다.
//
// 스킬효과 명: 체인 공격
// 스킬 효과 인덱스: 60
// 효과 설명
// 해당 효과에 걸린 대상은 해당 효과가 걸려있는 동안 일정거리내의 아군에게 같은 공격를 날려 공격하게 된다.
// 주변에 대상에게 공격을 날릴때 효과가 강해지거나 약해질 수 있다.
// 서로에게 공격을 날리는 횟수에 제한이 있어야 한다.
// 필요 파라매터
// 아군에게 공격하게 되는 공격 범위
// 공격시 데미지 증가 %
// 반복되는 횟수
// 관련해서 서로 논의를 하면서 진행해야 할 것 같습니다.
// 초안을 보시고 의견 주시기 바랍니다.
class CDnChainAttackBlow : public CDnBlow, public TBoostMemoryPool<CDnChainAttackBlow>
{
private:
	// 맨 처음 공격한 액터
	DnActorHandle m_hRootAttacker;
	int m_iRootAttackerTeam;

	// 체크할 거리
	float m_fRange;
	// 최대 횟수
	int m_iMaxCount;
	// 히트 시그널에 적용햘 hitprob (0.0~1.0 까지)
	float m_fHitApplyPercent;

	//float m_fElapsedTime;

#ifdef _GAMESERVER
	CDnState m_RootAttackerState;
	ProjectileStruct m_ProjectileSignalInfo;
#else
	ProjectileStruct m_ProjectileSignalInfo;
	
	// 자신에게 마지막으로 전달한 액터.
	DnActorHandle m_hPrevAttacker;
#endif

public:
	CDnChainAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChainAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if !defined(SW_ADD_CHAINATTACK_STATEEFFECT_20091029_jhk8211 )

#if _GAMESERVER

	void SetProjectileSignal( const ProjectileStruct* pProjectileSignal )
	{
		if( NULL == pProjectileSignal )
			return;

#ifdef PRE_FIX_MEMOPT_SIGNALH
		CopyShallow_ProjectileStruct(m_ProjectileSignalInfo, pProjectileSignal);
#else
		m_ProjectileSignalInfo = *pProjectileSignal;
#endif
	}

	void SetRootAttackerState( CDnState* pRootAttackerState )
	{
		if( NULL == pRootAttackerState )
			return;

		m_RootAttackerState = *pRootAttackerState;
	}

	virtual void WriteAdditionalPacket( void );
#else
	void ReadyForChainAttack( DWORD dwRootAttackerActorID, DWORD dwPrevAttackerActorID, int iActionIndex, int iProjectileSignalArrayIndex );
	void OnReceiveAddPacket( CPacketCompressStream& PacketStream );
#endif

#endif

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
