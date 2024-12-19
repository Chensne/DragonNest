#include "StdAfx.h"
#include "EtTestCollision.h"
#include "EtFindCollision.h"
#include "EtCollisionEntity.h"

CEtCollisionEntity::CEtCollisionEntity(void)
{
	m_pvecOriginalPrimitive = NULL;
	m_vScale = EtVector3( 1.0f, 1.0f, 1.0f );
	m_nCollisionGroup = 0;
	m_nTargetCollisionGroup = -1;
	m_bDraw = false;
	m_bEnableCollision = false;

	SetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) );
	SetTargetCollisionGroup( 0 );
}

CEtCollisionEntity::~CEtCollisionEntity(void)
{
	SAFE_DELETE_PVEC( m_vecPrimitive );
}

void CEtCollisionEntity::Initialize( std::vector< SCollisionPrimitive * > &vecPrimitive, std::vector< int > &vecPrimitiveBoneIndex )
{
	int i;
	SCollisionPrimitive *pBasePrimitive;

	SAFE_DELETE_PVEC( m_vecPrimitive );
	m_vecPrimitiveParentIndex.clear();
	m_vecPrimitiveEnable.clear();

	m_vecPrimitiveParentIndex.insert( m_vecPrimitiveParentIndex.end(), vecPrimitiveBoneIndex.begin(), vecPrimitiveBoneIndex.end() );
	m_vecPrimitiveEnable.resize( vecPrimitive.size() );
	m_pvecOriginalPrimitive = &vecPrimitive;
	for( i = 0; i < ( int )m_pvecOriginalPrimitive->size(); i++ )
	{
		switch( ( *m_pvecOriginalPrimitive )[ i ]->Type )
		{
			case CT_SPHERE:
				pBasePrimitive = new SCollisionSphere();
				break;
			case CT_BOX:
				pBasePrimitive = new SCollisionBox();
				break;
			case CT_CAPSULE:
				pBasePrimitive = new SCollisionCapsule();
				break;
			case CT_TRIANGLE_LIST:
				pBasePrimitive = new SCollisionTriangleList();
				break;
			default:
				pBasePrimitive = NULL;
				continue;
		}
		m_vecPrimitive.push_back( pBasePrimitive );
		m_vecPrimitiveEnable[ i ] = true;
	}
	m_bEnableCollision = true;

	ShowCollisionPrimitive( true );
}

void CEtCollisionEntity::UpdateCollisionPrimitive( int nIndex, EtMatrix &WorldMat )
{
	if( nIndex >= ( int )m_vecPrimitive.size() )
	{
		ASSERT( 0 && "Invalid Collision Primitive Index!!!" );
		return;
	}
	if( !m_vecPrimitiveEnable[ nIndex ] )
	{
		return;
	}
	::UpdateCollisionPrimitive( *m_vecPrimitive[ nIndex ], *( *m_pvecOriginalPrimitive )[ nIndex ], WorldMat, m_vScale );
}

void CEtCollisionEntity::UpdateCollisionPrimitive( EtMatrix &WorldMat )
{
	if( !m_pvecOriginalPrimitive )
	{
		ASSERT( 0 && "Not Found Original CollisionMesh!!!" );
		return;
	}

	int i;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		UpdateCollisionPrimitive( i, WorldMat );
	}

}

SCollisionPrimitive *CEtCollisionEntity::FindCollisionPrimitive( int nBoneIndex )
{
	int i;

	for( i = 0; i < ( int )m_vecPrimitiveParentIndex.size(); i++ )
	{
		if( m_vecPrimitiveParentIndex[ i ] == nBoneIndex )
		{
			return m_vecPrimitive[ i ];
		}
	}

	return NULL;
}

void CEtCollisionEntity::DrawCollisionPrimitive()
{
}

bool CEtCollisionEntity::TestCollision( SCollisionPrimitive &Primitive )
{
	int i;
	SCollisionResponse Response;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		if( CEtCollisionFinder::GetInstance().FindCollision( *m_vecPrimitive[ i ], Primitive, Response, false ) )
		{
			return true;
		}
	}

	return false;
}

bool CEtCollisionEntity::FindCollision( SCollisionPrimitive &TragetPrimitive, EtVector3 &vMove, SCollisionResponse &Response, bool bCalcContactTime )
{
	int i;

	Response.vMove = vMove;
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		if( CEtCollisionFinder::GetInstance().FindCollision( *m_vecPrimitive[ i ], TragetPrimitive, Response, bCalcContactTime ) )
		{
			return true;
		}
	}

	return false;
}

bool CEtCollisionEntity::FindCollision( std::vector< SCollisionPrimitive * > &vecTragetPrimitive, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime )
{
	int i, j;
	SCollisionResponse Response;
	bool bResult = false;

	Response.vMove = vMove;
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		for( j = 0; j < ( int )vecTragetPrimitive.size(); j++ )
		{
			if( CEtCollisionFinder::GetInstance().FindCollision( *m_vecPrimitive[ i ], *vecTragetPrimitive[ j ], Response, bCalcContactTime ) )
			{
				Response.pCollisionPrimitive = vecTragetPrimitive[ j ];
				vecResponse.push_back( Response );
				if( Response.vExtraNormal.x != FLT_MAX )
				{
					Response.vNormal = Response.vExtraNormal;
					vecResponse.push_back( Response );
				}
				bResult = true;
			}
		}
	}

	return bResult;
}

bool CEtCollisionEntity::TestSegmentCollision( SSegment &Segment )
{
	if( !IsEnableCollision() )
	{
		return false;
	}

	int i;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		switch( m_vecPrimitive[ i ]->Type )
		{
			case CT_BOX:
				if( TestSegmentToBox( Segment, *( ( SCollisionBox * )m_vecPrimitive[ i ] ) ) )
				{
					return true;
				}
				break;
			case CT_SPHERE:
				if( TestSegmentToSphere( Segment, *( ( SCollisionSphere * )m_vecPrimitive[ i ] ) ) )
				{
					return true;
				}
				break;
			case CT_CAPSULE:
				if( TestSegmentToCapsule( Segment, *( ( SCollisionCapsule * )m_vecPrimitive[ i ] ) ) )
				{
					return true;
				}
				break;
		}
	}

	return false;
}


bool CEtCollisionEntity::FindSegmentCollision( SSegment &Segment, SCollisionResponse &Response, DNVector(SCollisionResponse) *pvResponse )
{
	if( !IsEnableCollision() )
	{
		return false;
	}

	int i;
	bool bResult;
	float fContactTime;

	bResult = false;
	Response.fContactTime = FLT_MAX;
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;;
		}
		switch( m_vecPrimitive[ i ]->Type )
		{
			case CT_BOX:
				if( FindSegmentToBox( Segment, *( ( SCollisionBox * )m_vecPrimitive[ i ] ), fContactTime ) )
				{
					if( pvResponse )
					{
						SCollisionResponse sResponse;
						sResponse.fContactTime			= fContactTime;
						sResponse.pCollisionPrimitive	= m_vecPrimitive[i];
						pvResponse->push_back( sResponse );
					}

					if( fContactTime < Response.fContactTime )
					{
						Response.fContactTime = fContactTime;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
			case CT_SPHERE:
				if( FindSegmentToSphere( Segment, *( ( SCollisionSphere * )m_vecPrimitive[ i ] ), fContactTime ) )
				{
					if( pvResponse )
					{
						SCollisionResponse sResponse;
						sResponse.fContactTime			= fContactTime;
						sResponse.pCollisionPrimitive	= m_vecPrimitive[i];
						pvResponse->push_back( sResponse );
					}

					if( fContactTime < Response.fContactTime )
					{
						Response.fContactTime = fContactTime;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
			case CT_CAPSULE:
				if( FindSegmentToCapsule( Segment, *( ( SCollisionCapsule * )m_vecPrimitive[ i ] ), fContactTime ) )
				{
					if( pvResponse )
					{
						SCollisionResponse sResponse;
						sResponse.fContactTime			= fContactTime;
						sResponse.pCollisionPrimitive	= m_vecPrimitive[i];
						pvResponse->push_back( sResponse );
					}

					if( fContactTime < Response.fContactTime )
					{
						Response.fContactTime = fContactTime;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
		}
	}

	return bResult;
}

bool CEtCollisionEntity::FindCapsuleCollision( SCollisionCapsule &Capsule, SCollisionResponse &Response, DNVector(SCollisionResponse) *pvResponse )
{
	if( !IsEnableCollision() )
	{
		return false;
	}

	int i;
	float fParam1, fParam2;
	bool bResult;

	bResult = false;
	Response.fContactTime = FLT_MAX;
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		switch( m_vecPrimitive[ i ]->Type )
		{
			case CT_BOX:
				float fBoxParam0, fBoxParam1, fBoxParam2;
				if( TestBoxToCapsule( *( ( SCollisionBox * )m_vecPrimitive[ i ] ), Capsule, fParam1, fBoxParam0, fBoxParam1, fBoxParam2 ) )
				{
					if( pvResponse )
					{
						SCollisionResponse sResponse;
						sResponse.fContactTime			= fParam1;
						sResponse.pCollisionPrimitive	= m_vecPrimitive[i];
						pvResponse->push_back( sResponse );
					}

					if( fParam1 < Response.fContactTime )
					{
						Response.fContactTime = fParam1;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
			case CT_SPHERE:
				if( TestSphereToCapsule( *( ( SCollisionSphere * )m_vecPrimitive[ i ] ), Capsule, fParam1 ) )
				{
					if( pvResponse )
					{
						SCollisionResponse sResponse;
						sResponse.fContactTime			= fParam1;
						sResponse.pCollisionPrimitive	= m_vecPrimitive[i];
						pvResponse->push_back( sResponse );
					}

					if( fParam1 < Response.fContactTime )
					{
						Response.fContactTime = fParam1;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
			case CT_CAPSULE:
				if( TestCapsuleToCapsule( Capsule, *( ( SCollisionCapsule * )m_vecPrimitive[ i ] ), fParam1, fParam2 ) )
				{
					if( pvResponse )
					{
						SCollisionResponse sResponse;
						sResponse.fContactTime			= fParam1;
						sResponse.pCollisionPrimitive	= m_vecPrimitive[i];
						pvResponse->push_back( sResponse );
					}

					if( fParam1 < Response.fContactTime )
					{
						Response.fContactTime = fParam1;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
		}
	}

	return bResult;
}

bool CEtCollisionEntity::FindCollisionPrimitiveCenterInRange( EtVector3 vPosition, float fMinRangeRadius, float fRangeRadius )
{
	if( !IsEnableCollision() )
	{
		return false;
	}

	float fMinRadiusSQ = fMinRangeRadius * fMinRangeRadius;
	float fRadiusSQ = fRangeRadius * fRangeRadius;

	for( int i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}

		// 범위 안에 중점이 포함되어있는 애들은 전부 모아놓는다.
		EtVector3 vCenter( 0.0f, 0.0f, 0.0f );
		switch( m_vecPrimitive[ i ]->Type )
		{
			case CT_BOX:
				{
					SCollisionBox* pCollisionBox = static_cast<SCollisionBox*>(m_vecPrimitive[ i ]);
					vCenter = pCollisionBox->vCenter;
				}
				break;
			case CT_SPHERE:
				{
					SCollisionSphere* pCollisionSphere = static_cast<SCollisionSphere*>( m_vecPrimitive[ i ] );
					vCenter = pCollisionSphere->vCenter;
				}
				break;
			case CT_CAPSULE:
				{
					SCollisionCapsule* pCollisionCapsule = static_cast<SCollisionCapsule*>( m_vecPrimitive[ i ] );
					SAABox AABB;
					pCollisionCapsule->GetBoundingBox( AABB );
					vCenter = AABB.GetCenter();
				}
				break;
		}

		float fDistanceSQ = EtVec3LengthSq( &EtVector3(vCenter - vPosition) );

		if( fDistanceSQ >= fMinRadiusSQ && fDistanceSQ <= fRadiusSQ )
			return true;
	}

	return false;
}


int CEtCollisionEntity::GetParentBoneIndex( SCollisionPrimitive *pPrimitive )
{
	int i;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( m_vecPrimitive[ i ] == pPrimitive )
		{
			return m_vecPrimitiveParentIndex[ i ];
		}
	}

	return -1;
}

void CEtCollisionEntity::EnableCollision( SCollisionPrimitive *pPrimitive, bool bEnable )
{
	int i;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( m_vecPrimitive[ i ] == pPrimitive )
		{
			m_vecPrimitiveEnable[ i ] = bEnable;
		}
	}
}

bool CEtCollisionEntity::IsEnableCollision( SCollisionPrimitive *pPrimitive )
{
	int i;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( m_vecPrimitive[ i ] == pPrimitive )
		{
			return m_vecPrimitiveEnable[ i ] ? true : false;
		}
	}

	return false;
}
