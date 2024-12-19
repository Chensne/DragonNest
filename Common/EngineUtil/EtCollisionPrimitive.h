#pragma once

enum CollisionType
{
	CT_BOX = 0,
	CT_SPHERE = 1,
	CT_CAPSULE = 2,
	CT_TRIANGLE = 3,
	CT_TRIANGLE_LIST = 4,
	CT_TYPE_COUNT = 5,
};

struct SCollisionPrimitive
{
	virtual ~SCollisionPrimitive() {}
	virtual void GetBoundingBox( SAABox &Box ) {}
	virtual void Load( CStream *pStream ) {}
	virtual void Save( CStream *pStream ) {}

	CollisionType Type;
};

#include "MemPool.h"
struct SCollisionTriangle : SCollisionPrimitive, public TBoostMemoryPool< SCollisionTriangle >
{
	SCollisionTriangle() { Type = CT_TRIANGLE; }
	virtual void GetBoundingBox( SAABox &Box )
	{
		Box.Reset();
		Box.AddPoint( vOrigin );
		Box.AddPoint( vOrigin + vEdge1 );
		Box.AddPoint( vOrigin + vEdge2 );
	}
	virtual void Load( CStream *pStream )
	{
		pStream->Read( vOrigin, sizeof( EtVector3 ) );
		pStream->Read( vEdge1, sizeof( EtVector3 ) );
		pStream->Read( vEdge2, sizeof( EtVector3 ) );
	}
	virtual void Save( CStream *pStream )
	{
		pStream->Write( vOrigin, sizeof( EtVector3 ) );
		pStream->Write( vEdge1, sizeof( EtVector3 ) );
		pStream->Write( vEdge2, sizeof( EtVector3 ) );
	}
	void GetNormal( EtVector3 &vNormal )
	{
		EtVec3Cross( &vNormal, &vEdge1, &vEdge2 );
		EtVec3Normalize( &vNormal, &vNormal );
	}

	EtVector3 vOrigin;
	EtVector3 vEdge1;
	EtVector3 vEdge2;
};

struct SCollisionTriangleList : SCollisionPrimitive, public TBoostMemoryPool< SCollisionTriangleList >
{
	SCollisionTriangleList() { Type = CT_TRIANGLE_LIST; }
	std::vector< SCollisionTriangle > vecTriangle;
};

struct SCollisionSphere : SCollisionPrimitive, public TBoostMemoryPool< SCollisionSphere >
{
	SCollisionSphere() : fRadius( 0.0f ) { Type = CT_SPHERE; }
	virtual void GetBoundingBox( SAABox &Box )
	{
		Box.Max = vCenter + EtVector3( fRadius, fRadius, fRadius );
		Box.Min = vCenter - EtVector3( fRadius, fRadius, fRadius );
	}
	virtual void Load( CStream *pStream )
	{
		pStream->Read( vCenter, sizeof( EtVector3 ) );
		pStream->Read( &fRadius, sizeof( float ) );
	}
	virtual void Save( CStream *pStream )
	{
		pStream->Write( vCenter, sizeof( EtVector3 ) );
		pStream->Write( &fRadius, sizeof( float ) );
	}

	EtVector3 vCenter;
	float fRadius;
};

#include "MemPool.h"
struct SCollisionBox : SCollisionPrimitive, public TBoostMemoryPool< SCollisionBox >
{
	SCollisionBox() { Type = CT_BOX; memset( fExtent, 0, sizeof( fExtent ) ); }
	virtual void GetBoundingBox( SAABox &Box )
	{
		EtVector3 TempAxis[ 3 ] = { fExtent[ 0 ] * vAxis[0], fExtent[ 1 ] * vAxis[ 1 ], fExtent[ 2 ] * vAxis[ 2 ] };
		Box.Reset();

		Box.AddPoint( vCenter - TempAxis[ 0 ] - TempAxis[ 1 ] - TempAxis[ 2 ] );
		Box.AddPoint( vCenter + TempAxis[ 0 ] - TempAxis[ 1 ] - TempAxis[ 2 ] );
		Box.AddPoint( vCenter - TempAxis[ 0 ] + TempAxis[ 1 ] - TempAxis[ 2 ] );
		Box.AddPoint( vCenter + TempAxis[ 0 ] + TempAxis[ 1 ] - TempAxis[ 2 ] );
		Box.AddPoint( vCenter - TempAxis[ 0 ] - TempAxis[ 1 ] + TempAxis[ 2 ] );
		Box.AddPoint( vCenter + TempAxis[ 0 ] - TempAxis[ 1 ] + TempAxis[ 2 ] );
		Box.AddPoint( vCenter - TempAxis[ 0 ] + TempAxis[ 1 ] + TempAxis[ 2 ] );
		Box.AddPoint( vCenter + TempAxis[ 0 ] + TempAxis[ 1 ] + TempAxis[ 2 ] );
	}
	virtual void Load( CStream *pStream )
	{
		pStream->Read( vCenter, sizeof( EtVector3 ) );
		pStream->Read( vAxis, sizeof( EtVector3 ) * 3 );
		pStream->Read( &fExtent, sizeof( float ) * 3 );
	}
	virtual void Save( CStream *pStream )
	{
		pStream->Write( vCenter, sizeof( EtVector3 ));
		pStream->Write( vAxis, sizeof( EtVector3 ) * 3 );
		pStream->Write( &fExtent, sizeof( float ) * 3 );
	}

	EtVector3 vCenter;
	EtVector3 vAxis[ 3 ];
	float fExtent[ 3 ];
};

struct SCollisionCapsule : SCollisionPrimitive, public TBoostMemoryPool< SCollisionCapsule >
{
	SCollisionCapsule() : fRadius( 0.0f ) { Type = CT_CAPSULE; }
	virtual void GetBoundingBox( SAABox &Box )
	{
		Box.Reset();
		Box.AddPoint( Segment.vOrigin + EtVector3( fRadius, fRadius, fRadius ) );
		Box.AddPoint( Segment.vOrigin - EtVector3( fRadius, fRadius, fRadius ) );
		Box.AddPoint( Segment.vOrigin + Segment.vDirection + EtVector3( fRadius, fRadius, fRadius ) );
		Box.AddPoint( Segment.vOrigin + Segment.vDirection - EtVector3( fRadius, fRadius, fRadius ) );
	}
	virtual void Load( CStream *pStream )
	{
		pStream->Read( &Segment.vOrigin, sizeof( EtVector3 ) );
		pStream->Read( &Segment.vDirection, sizeof( EtVector3 ) );
		pStream->Read( &fRadius, sizeof( float ) );
	}
	virtual void Save( CStream *pStream )
	{
		pStream->Write( &Segment.vOrigin, sizeof( EtVector3 ) );
		pStream->Write( &Segment.vDirection, sizeof( EtVector3 ) );
		pStream->Write( &fRadius, sizeof( float ) );
	}

	SSegment Segment;
	float fRadius;
};

struct SKdTreeCollisionPrimitive
{
	SKdTreeCollisionPrimitive()
	{
		pPrimitive = NULL;
		m_bSeleDelete = false;
	}
	~SKdTreeCollisionPrimitive()
	{
		if( m_bSeleDelete )
		{
			SAFE_DELETE( pPrimitive );
		}
	}
	void GetBoundingBox( SAABox &Box )
	{
		pPrimitive->GetBoundingBox( Box );
	}
	void Load( CStream *pStream )
	{
		CollisionType Type;
		pStream->Read( &Type, sizeof( CollisionType ) );
		switch( Type )
		{
			case CT_BOX:
				pPrimitive = new SCollisionBox();
				break;
			case CT_SPHERE:
				pPrimitive = new SCollisionSphere();
				break;
			case CT_CAPSULE:
				pPrimitive = new SCollisionCapsule();
				break;
			case CT_TRIANGLE:
				pPrimitive = new SCollisionTriangle();
				break;
		}
		if( pPrimitive )
		{
			pPrimitive->Load( pStream );
			pPrimitive->Type = Type;
		}
		m_bSeleDelete = true;
	}
	void Save( CStream *pStream )
	{
		if( pPrimitive )
		{
			pStream->Write( &pPrimitive->Type, sizeof( CollisionType ) );
			pPrimitive->Save( pStream );
		}
	}

	SCollisionPrimitive *pPrimitive;
	bool m_bSeleDelete;
};

struct SCollisionResponse
{
	SCollisionResponse()
	{
		fContactTime = 1.0f;
		pCollisionPrimitive = NULL;
		vNormal.x = FLT_MAX;
		vExtraNormal.x = FLT_MAX;
	}

	EtVector3 vNormal;
	EtVector3 vExtraNormal;
	EtVector3 vMove;
	float fContactTime;
	SCollisionPrimitive *pCollisionPrimitive;
};

void UpdateCollisionPrimitive( SCollisionPrimitive &DestPrimitive, SCollisionPrimitive &SourPrimitive, const EtMatrix &WorldMat, const EtVector3 &vScale );
bool IsInside( SCollisionPrimitive &Primitive, EtVector3 &vPoint );
void GetCenterPos( SCollisionPrimitive &Primitive, EtVector3 &vPoint );
