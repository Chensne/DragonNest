#pragma once
#include "DnWorldBrokenDamageProp.h"

// ���� �������� �������� ���� Ŭ����. ���ڷ����ͷ� ���� ���������ٵ�~~
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

	// �� �������� �⺻ �̵� �Ӽ��� �ִ� ���. (������ Ʈ��)
	int m_iMovingAxis;
	float m_fAxisMovingSpeed;
	float m_fAxisMaxMoveDistance;
	EtVector3 m_vInitialPos;		// �ʱ� ��ġ.
	EtVector3 m_vDestPosition;		// �ִ� ��ġ.
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
			m_pData = NULL;		// �ݵ�� NULL �� ���־�� �� Ŭ������ ��ӹ��� Ŭ������ �Ҹ��ڿ��� �ٽ� �������� �ʽ��ϴ�.
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
	