#pragma once

#include "EtWorldProp.h"
#include "Timer.h"
#include "EtMatrixEx.h"
#include "MPTransAction.h"
#include "DnActor.h"
#include "PropHeader.h"
#include "TDnFSMState.h"
#include "MAChatBalloon.h"
#include "EtCustomRender.h"

class CDnWorldProp : public CEtWorldProp, public CSmartPtrBase<CDnWorldProp>, public MPTransAction, public MAChatBalloon, public CEtCustomRender
{
public:
	CDnWorldProp();
	virtual ~CDnWorldProp();

	enum IntersectionEnum {
		NoIntersectioin,
		Alpha,
		Collision,

		IntersectionEnum_Amount,
	};

protected:
	bool m_bProcess;
	EtMatrix m_matWorld;
	MatrixEx m_matExWorld;
	float m_fVisibleRange;
	bool m_bIsStaticCollision;
	int m_nSpecLevel;
	bool m_bShow;
	bool m_bProjectileSkip;

	PropTypeEnum m_PropType;
	IntersectionEnum m_IntersectionType;

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	bool m_bFirstProcess;
	int m_iVisiblePercent;
	#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	PropDef::Option::eValue m_VisibleOptions[PropDef::Option::eMaxVisibleOption];
	#else
	bool m_bRandomResultIsVisible;
	#endif
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE

protected:
	virtual bool InitializeTable( int nTableID );
	virtual bool CreateObject();
	virtual void FreeObject();
	virtual void* AllocPropData(int& usingCount);

	void CalcCustomRenderDepth();

private:
#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	void ClearVisibleOptions();
#endif

public:
	static DnPropHandle FindPropFromUniqueID( DWORD dwUniqueID );

	DWORD GetUniqueID() { return GetCreateUniqueID(); }

	// CEtWorldProp Message
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual void EnableCastShadow( bool bEnable );
	virtual void EnableReceiveShadow( bool bEnable );
	virtual void EnableCastLightmap( bool bEnable );
	virtual void EnableLightmapInfluence( bool bEnable );
	virtual void EnableIgnoreBuildColMesh( bool bEnable );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {};
	void EnableCollision( bool bEnable );

	// CDnWorldProp Message
	virtual void OnDamage( CDnDamageBase *pHitter, CDnActor::SHitParam &HitParam ) {}
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f ) {}
	virtual void CmdOperation() {}

	// MPTransAction
	virtual void OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket );

	// fsm 에서 사용함
	virtual void OnFSMStateEntry( const TDnFSMState<DnPropHandle>* pState ) {};
	virtual void OnFSMStateProcess( const TDnFSMState<DnPropHandle>* pState ) {};
	virtual void OnFSMStateLeave( const TDnFSMState<DnPropHandle>* pState ) {};

	// EtCustomRender
	virtual void RenderCustom( float fElapsedTime );

	void UpdateMatrix();
	EtMatrix *GetWorldMat() { return &m_matWorld; }
	MatrixEx *GetMatEx() { return &m_matExWorld; }
	virtual void Show( bool bShow );
	virtual bool IsShow() { return m_bShow; }
	virtual bool IsProjectileSkip(){ return m_bProjectileSkip; }

	PropTypeEnum GetPropType() { return m_PropType; }
	IntersectionEnum GetIntersectionType() { return m_IntersectionType; }

	int GetSpecLevel() { return m_nSpecLevel; }

	virtual void SetLifeTime( LOCAL_TIME LifeTimeMS ) {};

#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	bool IsVisible() const;
#endif
};