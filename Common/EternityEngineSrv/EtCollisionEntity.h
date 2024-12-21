#pragma once

#define COLLISION_TYPE_STATIC			0x0000ffff
#define COLLISION_TYPE_DYNAMIC			0xffff0000
#define COLLISION_GROUP_STATIC( x )		( 0x00000001 << x )
#define COLLISION_GROUP_DYNAMIC( x )	( 0x00010000 << x )

class CEtCollisionEntity
{
public:
	CEtCollisionEntity(void);
	virtual ~CEtCollisionEntity(void);

protected:

	std::vector< SCollisionPrimitive * > *m_pvecOriginalPrimitive;
	std::vector< SCollisionPrimitive * > m_vecPrimitive;
	std::vector< int > m_vecPrimitiveParentIndex;
	std::vector< BOOL > m_vecPrimitiveEnable;
	EtVector3 m_vScale;
	int m_nCollisionGroup;
	int m_nTargetCollisionGroup;
	bool m_bDraw;
	bool m_bEnableCollision;

public:

	void Initialize( std::vector< SCollisionPrimitive * > &vecPrimitive, std::vector< int > &vecPrimitiveBoneIndex );
	std::vector< SCollisionPrimitive * > *GetCollisionPrimitive() { return &m_vecPrimitive; }

	void SetCollisionScale( float fScale ) { m_vScale = EtVector3( fScale, fScale, fScale ); }
	void SetCollisionScale( const EtVector3 &vScale ) { m_vScale = vScale; }
	void GetCollisionScale( EtVector3 &vScale )  { vScale = m_vScale; }

	void SetCollisionGroup( int nGroup ) { m_nCollisionGroup = nGroup; }
	int GetCollisionGroup() { return m_nCollisionGroup; }
	void SetTargetCollisionGroup( int nGroup ) { m_nTargetCollisionGroup = nGroup; }
	int GetTargetCollisionGroup() { return m_nTargetCollisionGroup; }

	void ShowCollisionPrimitive( bool bShow ) { m_bDraw = bShow; }
	bool IsShowCollisionPrimitive()  { return m_bDraw; }

	void EnableCollision( bool bEnable ) { m_bEnableCollision = bEnable; }
	bool IsEnableCollision() { return m_bEnableCollision; }

	void EnableCollision( SCollisionPrimitive *pPrimitive, bool bEnable );
	bool IsEnableCollision( SCollisionPrimitive *pPrimitive );

	int GetCollisionPrimitiveCount() { return ( int )m_vecPrimitive.size(); }
	void UpdateCollisionPrimitive( int nIndex, EtMatrix &WorldMat );
	virtual void UpdateCollisionPrimitive( EtMatrix &WorldMat );

	SCollisionPrimitive *FindCollisionPrimitive( int nBoneIndex );
	int GetParentBoneIndex( SCollisionPrimitive *pPrimitive );

	void DrawCollisionPrimitive();

	bool TestCollision( SCollisionPrimitive &Primitive );
	bool FindCollision( SCollisionPrimitive &TragetPrimitive, EtVector3 &vMove, SCollisionResponse &Response, bool bCalcContactTime );
	bool FindCollision( std::vector<SCollisionPrimitive *> &TragetPrimitive, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime = true );
	bool TestSegmentCollision( SSegment &Segment );
	bool FindSegmentCollision( SSegment &Segment, SCollisionResponse &Response, DNVector(SCollisionResponse) *pvResponse=NULL );
	bool FindCapsuleCollision( SCollisionCapsule &Capsule, SCollisionResponse &Response, DNVector(SCollisionResponse) *pvResponse=NULL );
	bool FindCollisionPrimitiveCenterInRange( EtVector3 vPosition, float fMinRangeRadius, float fRangeRadius );
};
