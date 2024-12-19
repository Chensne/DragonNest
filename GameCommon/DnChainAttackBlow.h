#pragma once
#include "DnBlow.h"

// #7189
// OBT ���̵� ���� �̺��ν��� �����ϸ鼭 �پ��� ��ų�� �����ϴٰ� ���ο� ȿ���� �ʿ��ϰԵǾ�
// ���ο� ��ų ȿ���� ��û�մϴ�.
// ������ [�����̻�]�� ���� ��ų�� �ֺ��� �Ʊ����� �������� �ְ� �Ǵ� ȿ���Դϴ�.
//
// ��ųȿ�� ��: ü�� ����
// ��ų ȿ�� �ε���: 60
// ȿ�� ����
// �ش� ȿ���� �ɸ� ����� �ش� ȿ���� �ɷ��ִ� ���� �����Ÿ����� �Ʊ����� ���� ���ݸ� ���� �����ϰ� �ȴ�.
// �ֺ��� ��󿡰� ������ ������ ȿ���� �������ų� ������ �� �ִ�.
// ���ο��� ������ ������ Ƚ���� ������ �־�� �Ѵ�.
// �ʿ� �Ķ����
// �Ʊ����� �����ϰ� �Ǵ� ���� ����
// ���ݽ� ������ ���� %
// �ݺ��Ǵ� Ƚ��
// �����ؼ� ���� ���Ǹ� �ϸ鼭 �����ؾ� �� �� �����ϴ�.
// �ʾ��� ���ð� �ǰ� �ֽñ� �ٶ��ϴ�.
class CDnChainAttackBlow : public CDnBlow, public TBoostMemoryPool<CDnChainAttackBlow>
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

#ifdef _GAMESERVER
	CDnState m_RootAttackerState;
	ProjectileStruct m_ProjectileSignalInfo;
#else
	ProjectileStruct m_ProjectileSignalInfo;
	
	// �ڽſ��� ���������� ������ ����.
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
