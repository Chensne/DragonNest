#pragma once
#include "DnWorldBrokenDamageProp.h"

// 맞으면 맞은 반대 방향으로 이동하고 내구도가 다 되면 폭발하여 데미지를 주는 프랍
// 클라와 서버가 공유하지 않고 각각 돌아갑니다.
class CDnWorldHitMoveDamageBrokenProp : public CDnWorldBrokenDamageProp
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

	// 축 고정으로 기본 이동 속성이 있는 경우. (압착기 트랩)
	int m_iMovingAxis;
	float m_fAxisMovingSpeed;
	float m_fAxisMaxMoveDistance;
	EtVector3 m_vInitialPos;
	EtVector3 m_vDestPosition;
	bool m_bArrived;
	float m_fHitDurability;
	int m_iActivateElementIndex;
	bool m_bActivate;

protected:
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

	virtual void OnDamageReadAdditionalPacket( CDnDamageBase::SHitParam &HitParam, CPacketCompressStream *pStream );
public:
	CDnWorldHitMoveDamageBrokenProp(void);
	virtual ~CDnWorldHitMoveDamageBrokenProp(void);

	// from CDnWorldProp
	virtual void OnDamage( CDnDamageBase *pHitter, CDnActor::SHitParam &HitParam );
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );

	void _UpdatePropPos();
	void _GetPosProperly( EtVector3 &vPrevPos );
};
