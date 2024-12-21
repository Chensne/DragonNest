#pragma once

#include "DnDamageBase.h"

const float g_fBendFrame				= 8.0f;
const float	g_fAggroDecreaseDeltaTime	= 5.0f;
const float g_fAggroChangeTragetRate	= 1.2f;	// 현재 타겟보다 해당 퍼센트 어그로 높으면 타겟 변경

class MAAiReceiver;
class MAAiBase
{
public:
	MAAiBase( DnActorHandle hActor, MAAiReceiver *pReceiver );
	virtual ~MAAiBase();

protected:
	MAAiReceiver *m_pReceiver;

	// Delay
	std::vector<LOCAL_TIME> m_nVecDelay;
	LOCAL_TIME m_PrevLocalTime;

	DnActorHandle m_hActor;
	DnActorHandle m_hTarget;
	CDnMonsterActor* m_pMonsterActor;

	bool m_bEnableAggroProcess;

	// AI에서 LOOK 고정 관련
	bool		m_bIsAILook;
	EtVector2	m_vAILook;
	EtVector3	m_vProjectileTarget;

	// AI에서 발사체 타겟 고정 관련 (#22838)
	DnActorHandle m_hReservedProjectileTarget;

	enum AIDelayType
	{
		Patrol = 0,		// 어슬렁
		Attack,			// 공격
		Max,
	};

	// DieAnnounce 관련
	bool	m_bNotifyDieAnnounce;
	DWORD	m_dwNotifyDieAnnounceTick;

private:

	int				_CalcAdditionalAggro( const int nAggro, const float fValue );

protected:
		
	virtual void	_OnSetProjectileTarget(){}

public:
	virtual bool Initialize();

	void	ChangeTarget( DnActorHandle hActor, bool bStop=true );
	void	FindTarget();
	void	SetTarget( DnActorHandle hActor, int nAggroValue = 0 );
	void	SetProjectileTarget();
	inline DnActorHandle GetTarget() { return m_hTarget; }
	inline DnActorHandle GetActor() { return m_hActor; }
	
	// Aggro Function
	void ProcessAggro( const LOCAL_TIME LocalTime, const float fDelta );
	void ResetAggro();

	void EnableAggorProcess( bool bEnable );

	// Delay(CoolTime) 관리
	void SetDelay( const AIDelayType Type, const int nDelay );
	bool IsDelay( const AIDelayType Type ) const;
	void ProcessDelay( const LOCAL_TIME LocalTime );
	void ResetDelay();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool IsScript(){ return false; };

	virtual void OnChangeAction(const char* szPrevAction) {}
	virtual void OnFinishAction(const char* szPrevAction, LOCAL_TIME time) {}
	virtual void OnDie(LOCAL_TIME LocalTime) { }
	virtual void OnDamage(DnActorHandle hActor, CDnDamageBase::SHitParam &HitParam, LOCAL_TIME time) { }
	virtual void OnStop( EtVector3 &vPosition ) {}
	virtual void OnBeginNaviMode( const char* szPrevAction, const bool bPrevIsMove ){}
	virtual void OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct ){}
	virtual void AICmdMsgProc( const UINT uiMsg, void* pParam ){}
	// Trigger 관련
	virtual void OnInitNaviDestination( SOBB* pOBB, UINT uiMoveFrontRate ){}

	// AI에서 LOOK 고정 관련
	void		ResetAILook();
	bool		bIsAILook(){ return m_bIsAILook; }
	bool		bIsProjectileTargetSignal();
	EtVector2*	GetAILook(){ return &m_vAILook; }
	EtVector3*	GetProjectileTarget(){ return &m_vProjectileTarget; }

	// AI에서 발사체 타겟 고정 관련 (#22838)
	void		ReservedProjectileTarget( void ) { m_hReservedProjectileTarget = m_hTarget; } 
	DnActorHandle GetReservedProjectileTarget( void ) { return m_hReservedProjectileTarget; };

	// DieAnnounce 관련
	void		NotifyDieAnnounce();
	void		ResetNotifyDieAnnounce();
	bool		bIsNotifyDieAnnounce( DWORD dwGap );
};
