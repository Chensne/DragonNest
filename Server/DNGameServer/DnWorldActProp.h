#pragma once

#include "DnWorldProp.h"
#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "Timer.h"
#include "DnDropItem.h"
#include "TDnFiniteStateMachine.h"


class CDnWorldActProp : public CDnWorldProp, public CDnActionBase, public CDnRenderBase
{
public:
	CDnWorldActProp( CMultiRoom *pRoom );
	virtual ~CDnWorldActProp();

protected:
	bool m_bDestroy;
	// WorldMat를 매프레임 갱신해야 되는 프랍인지 아닌지..
//	bool m_bInstantUpdate;
	std::string m_szAniName;
	std::string m_szActName;
	std::string s_dwCreateCount;
	std::string m_szDefaultActionName;

	TDnFiniteStateMachine<DnPropHandle>* m_pFSM;

	LOCAL_TIME m_PrevLocalTime;

	// 소환용 프랍에서 사용하는 제한시간.
	LOCAL_TIME m_LifeTime;
	
	// 서버에서 20프레임을 도는 프로세스를 10프레임으로 줄이기 위해 액션 프랍들을 격자로 돌린다.
//	bool m_bProcessAllowed;
	STATIC_DECL( DWORD s_dwCreatePropCount );

private:
//	DnActorHandle m_hLastAccessActor;

protected:
	virtual bool InitializeTable( int nTableID );
	virtual bool CreateObject();

	virtual void _OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta ) { m_bDestroy = true; };

public:
	// CDnWorldProp
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void CmdAction( const char *szActionName, CDNUserSession* pSession=NULL, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void CmdAction( DnActorHandle hActor, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );
	virtual void ActionSync( const char *szActionName, CDNUserSession* pSession = NULL, int nLoopCount = 0, float fBlendFrame = 3.f );

	// CDnRenderBase 
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
	virtual int GetAniIndex( const char *szAniName );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	bool IsDestroy() { return m_bDestroy; }

	void SetLifeTime( LOCAL_TIME LifeTime ) { m_LifeTime = LifeTime; }

	// FSM 의 State 에서 조회할 정보들의 인터페이스..
	virtual int GetDurability( void ) { return 0; };
	virtual int GetLastHitRandomSeed( void ) { return 0; };
	virtual bool IsHittedAndReset( void ) { return false; };

	virtual int GetNumDropItem( void ) { return 0; };
	virtual const CDnItem::DropItemStruct* GetDropItemByIndex( int iIndex ) { return NULL; };
	virtual void ClearDropItemList( void ) {};

	virtual int GetOperateState( void ) { return -1; };
	virtual void ResetOperateState( void ) { };
	virtual void CmdOperation( int iOperateID ) {};

	virtual int GetSubDurability( int /*iDurabilityIndex*/ ) { return 0; };

	virtual void OnSyncComplete( CDNUserSession* pBreakIntoGameSession=NULL );

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	void OnDispatchMessage( CDNUserSession *pSession, DWORD dwProtocol, BYTE *pPacket );
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE
};

class IBoostPoolDnWorldActProp:public CDnWorldActProp, public TBoostMemoryPool< IBoostPoolDnWorldActProp >
{
public:
	IBoostPoolDnWorldActProp( CMultiRoom *pRoom ):CDnWorldActProp(pRoom){}
	virtual ~IBoostPoolDnWorldActProp(){}
};
