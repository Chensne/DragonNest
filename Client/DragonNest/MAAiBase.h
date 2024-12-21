#pragma once

#include "DnActor.h"
const float g_fBendFrame = 8.0f;
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

	// AI Look
	bool		m_bIsAILook;
	EtVector2	m_vAILook;
	EtVector3	m_vProjectileTarget;
	std::string		m_strAILookAction;
	
	// Aggro
	struct AggroStruct {
		DnActorHandle hActor;
		int nAggro;
		AggroStruct( DnActorHandle hAggroActor, int nDefaultAggro ) {
			hActor = hAggroActor;
			nAggro = nDefaultAggro;
		}
	};
	std::vector<AggroStruct> m_VecAggroList;
	int m_nThreatRange;
	int m_nThreatAggro;
	int m_CognizanceGentleRange;
	int m_CognizanceThreatRange;

protected:
	void AddAggro( DnActorHandle hActor, int nAggro );
	void RemoveAggro( DnActorHandle hActor );
	AggroStruct *GetAggroStruct( DnActorHandle hActor );
	void GetEnemyList( std::vector<DnActorHandle> &hVecList );
	void RefreshAggroTarget();

public:

	void		SetAILookAction( const char* szAction ){ m_strAILookAction = szAction; };
	void		SetAILook( const bool bIsAILook, EtVector2& vAILook ){ m_bIsAILook = bIsAILook; m_vAILook = vAILook; }
	bool		bIsAILook(){ return m_bIsAILook; }
	bool		bIsProjectileTargetSignal();
	EtVector2*	GetAILook(){ return &m_vAILook; }
	void		ResetAILook();
	EtVector3*	GetProjectileTarget(){ return &m_vProjectileTarget; }

	virtual bool Initialize();

	void SetTarget( DnActorHandle hActor );
	void SetProjectileTarget();
	inline DnActorHandle GetTarget() { return m_hTarget; }
	inline DnActorHandle GetActor() { return m_hActor; }
	virtual int GetThreatRange() { return m_nThreatRange; }

	// Aggro Function
	virtual void OnDamageAggro( DnActorHandle hActor, CDnActor::SHitParam &HitParam, int nDamage );
	DnActorHandle GetAggroTarget();
	void ProcessAggro();

	// Delay(CoolTime) °ü¸®
	void SetDelay( DWORD dwSlot, int nDelay );
	bool IsDelay( DWORD dwSlot );
	void ProcessDelay( LOCAL_TIME LocalTime );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnChangeAction( const char* szPrevAction );
	virtual void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time);
	virtual void OnDie(LOCAL_TIME LocalTime) { }
	virtual void OnDamage(DnActorHandle hActor, CDnActor::SHitParam &HitParam, LOCAL_TIME time) { }
	virtual void OnStop( EtVector3 &vPosition ) {}

};
