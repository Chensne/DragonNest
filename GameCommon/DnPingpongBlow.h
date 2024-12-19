#pragma once
#include "DnBlow.h"

class CDnPingpongBlow : public CDnBlow, public TBoostMemoryPool<CDnPingpongBlow>
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

	const float CHAINATTACK_PROJECTILE_SPEED;

#ifdef _GAMESERVER
	INT64 m_nSkillDamage;
	CDnState m_RootAttackerState;
	ProjectileStruct m_ProjectileSignalInfo;
#else
	ProjectileStruct m_ProjectileSignalInfo;
	
	// 자신에게 마지막으로 전달한 액터.
	DnActorHandle m_hPrevAttacker;
	
	// 서버로부터 보내진 현재 남은 갯수
	int m_iLeaveCountFromServer;
#endif

public:
	CDnPingpongBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPingpongBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#if defined(_GAMESERVER)
	void Process_Server(LOCAL_TIME LocalTime, float fDelta);
#else
	void Process_Client(LOCAL_TIME LocalTime, float fDelta);
#endif // _GAMESERVER


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

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
