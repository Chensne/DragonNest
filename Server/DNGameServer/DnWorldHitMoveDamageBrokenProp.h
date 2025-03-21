#pragma once
#include "DnWorldBrokenDamageProp.h"

// 점점 가짓수가 많아지는 프랍 클래스. 데코레이터로 했음 괜찮았을텐데~~
class CDnWorldHitMoveDamageBrokenProp : public CDnWorldBrokenDamageProp, 
										public TBoostMemoryPool< CDnWorldHitMoveDamageBrokenProp >
{
private:
	enum
	{
		MOVE_AXIS_NONE,
		MOVE_PROPLOCAL_AXIS_X,
		MOVE_PROPLOCAL_AXIS_Y,
		MOVE_PROPLOCAL_AXIS_Z,
	};

	//EtVector3 m_vDest;
	float m_fMoveDistance;
	bool m_bHitMove;

	float m_fWholeMoveTime;
	float m_fMovedTime;
	float m_fVelocity;
	float m_fResist;
	EtVector3 m_vHitPosition;
	EtVector3 m_vVelocity;
	EtVector3 m_vResist;
	EtVector3 m_vDir;

	NavigationCell* m_pCurCell;

	float m_fDelta;

	// 축 고정으로 기본 이동 속성이 있는 경우. (압착기 트랩)
	int m_iMovingAxis;
	float m_fAxisMovingSpeed;
	float m_fAxisMaxMoveDistance;
	EtVector3 m_vInitialPos;		// 초기 위치.
	EtVector3 m_vDestPosition;		// 최대 위치.
	bool m_bArrived;
	int m_iActivateElementIndex;
	bool m_bActivate;

protected:
	//virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool InitializeTable( int nTableID );
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual bool CreateObject( void );
	void ReleasePostCustomParam( void ) 
	{
		if( m_pData && m_bPostCreateCustomParam )
		{
			HitMoveDamageBrokenStruct* pHitMoveDamageBrokenStruct = static_cast<HitMoveDamageBrokenStruct*>( m_pData );
			SAFE_DELETE( pHitMoveDamageBrokenStruct );
			m_pData = NULL;		// 반드시 NULL 로 해주어야 이 클래스를 상속받은 클래스의 소멸자에서 다시 지워지지 않습니다.
		}
	};

	virtual void OnDamageWriteAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream );

public:
	CDnWorldHitMoveDamageBrokenProp( CMultiRoom* pRoom );
	virtual ~CDnWorldHitMoveDamageBrokenProp(void);

	// from CDnWorldProp
	virtual void OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam );

	// from CDnWorldActProp
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );

	void _UpdatePropPos();
	void _GetPosProperly( EtVector3 &vPrevPos );
};
	