#pragma once
#include "DnBlow.h"

class CDnPingpongBlow : public CDnBlow, public TBoostMemoryPool<CDnPingpongBlow>
{
private:
	// �� ó�� ������ ����
	DnActorHandle m_hRootAttacker;
	int m_iRootAttackerTeam;

	// üũ�� �Ÿ�
	float m_fRange;
	// �ִ� Ƚ��
	int m_iMaxCount;
	// ��Ʈ �ñ׳ο� �����l hitprob (0.0~1.0 ����)
	float m_fHitApplyPercent;

	//float m_fElapsedTime;

	const float CHAINATTACK_PROJECTILE_SPEED;

#ifdef _GAMESERVER
	INT64 m_nSkillDamage;
	CDnState m_RootAttackerState;
	ProjectileStruct m_ProjectileSignalInfo;
#else
	ProjectileStruct m_ProjectileSignalInfo;
	
	// �ڽſ��� ���������� ������ ����.
	DnActorHandle m_hPrevAttacker;
	
	// �����κ��� ������ ���� ���� ����
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
