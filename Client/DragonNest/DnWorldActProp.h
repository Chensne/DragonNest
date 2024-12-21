#pragma once

#include "DnWorldProp.h"
#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "DnDropItem.h"
#include "Timer.h"
#include "DnActionSignalImp.h"
#include "TDnFiniteStateMachine.h"


class CDnWorldActProp : public CDnWorldProp, 
						//virtual public CDnActionBase, 
						public CDnRenderBase, 
						public CDnActionSignalImp
{
public:
	CDnWorldActProp();
	virtual ~CDnWorldActProp();

protected:
	bool m_bDestroy;
	std::string m_szActName;
	std::string m_szAniName;
	std::string m_szDefaultActionName;

	TDnFiniteStateMachine<DnPropHandle>* m_pFSM;
	
	// 소환용 프랍에서 사용하는 제한시간.
	LOCAL_TIME m_LifeTime;

protected:
	virtual bool InitializeTable( int nTableID );
	virtual bool CreateObject();

	virtual void _OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta ) { m_bDestroy = true; };

public:
	// CDnWorldProp
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f );

	// CDnRenderBase 
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	void SyncRandomCreated();
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE

	virtual int GetAniIndex( const char *szAniName );
	virtual void ShowRenderBase( bool bShow );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
	virtual void OnChangeAction( const char *szPrevAction );
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	static void CheckPreSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase );
#endif


	// CdnActionSignalImp
	virtual MatrixEx *GetObjectCross() { return &m_matExWorld; }
	virtual CDnRenderBase *GetRenderBase() { return this; }
	virtual EtAniObjectHandle GetAniObjectHandle() { return m_Handle; }

	// CDnRenderBase
	virtual EtAniObjectHandle GetObjectHandle() { return m_Handle; }

	bool IsDestroy() { return m_bDestroy; }
	virtual void Show( bool bShow );
	virtual bool IsShow() { return m_bShow; }

	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );

	// FSM 의 State 에서 조회할 정보들의 인터페이스..
	virtual int GetDurability( void ) { return 0; };
	virtual int GetLastHitRandomSeed( void ) { return 0; };
	virtual bool IsHittedAndReset( void ) { return false; };

	virtual int GetNumDropItem( void ) { return 0; };
	virtual const CDnItem::DropItemStruct* GetDropItemByIndex( int iIndex ) { return NULL; };
	virtual void ClearDropItemList( void ) {};

	virtual int GetOperateState( void ) { return -1; };

	virtual EtMatrix GetBoneMatrix( const char *szBoneName, bool *bExistBone = NULL );

	virtual void SetLifeTime( LOCAL_TIME LifeTimeMS ) { m_LifeTime = LifeTimeMS; };

	bool IsIgnoreVillageSignal( ActionElementStruct *pElement, CEtActionBase *pActionBase );
};
