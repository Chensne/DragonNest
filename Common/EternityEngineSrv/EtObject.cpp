#include "StdAfx.h"
#include "EtObject.h"
#include "EtFindCollision.h"
#include "EtLoader.h"
#include "PerfCheck.h"
#include "EtCollisionMng.h"

using namespace EternityEngine;

DECL_MULTISMART_PTR_STATIC( CEtObject, MAX_SESSION_COUNT, 1000 )

STATIC_DECL_INIT( CEtObject, CEtOctree< CEtObject * >, s_DynamicOctree );

float CEtOctreeNode< CEtObject * >::s_fMinRadius = 1000.0f;

CEtObject::CEtObject( CMultiRoom *pRoom )
: CMultiSmartPtrBase< CEtObject, MAX_SESSION_COUNT >(pRoom)
{
	EtMatrixIdentity( &m_WorldMat );
	m_pCurOctreeNode = NULL;
	m_BoundingBox.Reset();
}

CEtObject::~CEtObject(void)
{
	if( m_pCurOctreeNode )
	{
		STATIC_INSTANCE(s_DynamicOctree).Remove( this, m_pCurOctreeNode );
	}

	Clear();
}

void CEtObject::Clear()
{
	SAFE_RELEASE_SPTR( m_hSkin );
}

int CEtObject::Initialize( EtSkinHandle hSkin )
{
	Clear();
	m_hSkin = hSkin;
	CommonInitialize();

	return ET_OK;
}

void CEtObject::CommonInitialize()
{
	if( !m_hSkin || !m_hSkin->GetMeshHandle() )
	{
		_ASSERT(0);
		return;
	}
	CEtCollisionEntity::Initialize( *m_hSkin->GetMeshHandle()->GetCollisionPrimitive(), *m_hSkin->GetMeshHandle()->GetCollisionPrimitiveParentIndex() );
	UpdateCollisionPrimitive( m_WorldMat );
	GetMeshBoundingBox( m_OriginalBoundingBox );
	GetMeshBoundingSphere( m_OriginalBoundingSphere );
}

void CEtObject::Update( EtMatrix *pWorldMat ) 
{ 
	SSphere Sphere;

	m_WorldMat = *pWorldMat;
	UpdateBoundingPrimitive();
	if( m_hSkin )
	{
		UpdateCollisionPrimitive( m_WorldMat );
		GetBoundingSphere( Sphere );
		if( GetCollisionGroup() >= COLLISION_GROUP_DYNAMIC( 2 ) )	// 다이나믹 컬리젼 타입인 애들만 옥트리에 넣어서 체크 스태틱은 KdTree로 구현.
		{
			if( m_pCurOctreeNode )
			{
				m_pCurOctreeNode = STATIC_INSTANCE(s_DynamicOctree).Update( this, Sphere, m_pCurOctreeNode );
			}
			else
			{
				m_pCurOctreeNode = STATIC_INSTANCE(s_DynamicOctree).Insert( this, Sphere );
			}
		}
	}
}

void CEtObject::GetMeshBoundingBox( SAABox &AABox )
{
	EtMeshHandle hMesh;

	hMesh = m_hSkin->GetMeshHandle();
	if( hMesh )
	{
		hMesh->GetBoundingBox( AABox ); 
	}
	else
	{
		AABox.Min = EtVector3( -1.0f, -1.0f, -1.0f );
		AABox.Max = EtVector3( 1.0f, 1.0f, 1.0f );
	}
}

void CEtObject::GetMeshBoundingSphere( SSphere &Sphere )
{ 
	EtMeshHandle hMesh;

	hMesh = m_hSkin->GetMeshHandle();
	if( hMesh )
	{
		hMesh->GetBoundingSphere( Sphere ); 
	}
	else
	{
		Sphere.Center = EtVector3( 0.0f, 0.0f, 0.0f );
		Sphere.fRadius = 1.0f;
	}
}

void CEtObject::RecalcBoundingBox()
{
	GetMeshBoundingBox( m_OriginalBoundingBox );
}

void CEtObject::RecalcBoundingSphere()
{
	EtVector3 vDirection, vOriginalEnd, vChildEnd;
	SSphere BoundingSphere;

	GetMeshBoundingSphere( m_OriginalBoundingSphere );
}

void CEtObject::UpdateBoundingPrimitive()
{
	if( !m_hSkin )
	{
		return;
	}

	int i;
	EtMatrix OutputMat;

	EtMatrixScaling( &OutputMat, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &OutputMat, &OutputMat, &m_WorldMat );
	m_BoundingBox.Reset();
	for( i = 0; i < 8; i ++ )
	{
		EtVector3 vPoint;

		m_OriginalBoundingBox.GetVertex( i, vPoint );
		EtVec3TransformCoord( &vPoint, &vPoint, &OutputMat );
		m_BoundingBox.AddPoint( vPoint );
	}

	m_BoundingSphere = m_OriginalBoundingSphere;
	EtVec3TransformCoord( &m_BoundingSphere.Center, &m_OriginalBoundingSphere.Center, &m_WorldMat );
	m_BoundingSphere.fRadius *= max( max( m_vScale.x, m_vScale.y ), m_vScale.z );
}

void CEtObject::UpdateCollisionPrimitive( EtMatrix &WorldMat )
{
	if( !IsEnableCollision() )
	{
		return;
	}

	int i;
	EtMatrix *pBoneMat, AniMat;

	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		pBoneMat = &WorldMat;
		if( m_vecPrimitiveParentIndex[ i ] != -1 )
		{
			pBoneMat = GetBoneMat( m_vecPrimitiveParentIndex[ i ] );
			if( pBoneMat )
			{
				pBoneMat = EtMatrixMultiply( &AniMat, pBoneMat, &WorldMat );
			}
			else
			{
				pBoneMat = &WorldMat;
			}
		}
		CEtCollisionEntity::UpdateCollisionPrimitive( i, *pBoneMat );
	}
}

bool CEtObject::FindDynamicCollision( EtMatrix &WorldMat, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime, bool bUpdatePrimitive )
{
	SAABox BoundingBox;
	EtVector3 vMove;

	vMove.x = WorldMat._41 - m_WorldMat._41;
	vMove.y = WorldMat._42 - m_WorldMat._42;
	vMove.z = WorldMat._43 - m_WorldMat._43;
	GetBoundingBox( BoundingBox );
	BoundingBox.Max += vMove;
	BoundingBox.Min += vMove;
	if( bUpdatePrimitive )
	{
		UpdateCollisionPrimitive( WorldMat );
	}

	return _FindDynamicCollision( BoundingBox, vMove, vecResponse, bCalcContactTime );
}

bool CEtObject::_FindDynamicCollision( SAABox &BoundingBox, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime )
{
	STATIC_DECL( DNVector(CEtObject*) vecPickObject );

	bool bRet = false;
	STATIC_INSTANCE( vecPickObject ).clear();
	STATIC_INSTANCE( s_DynamicOctree ).Pick( BoundingBox, STATIC_INSTANCE( vecPickObject ) );
	if( !STATIC_INSTANCE( vecPickObject ).empty() )
	{
		int i;

		for( i = 0; i < ( int )STATIC_INSTANCE( vecPickObject ).size(); i++ )
		{
			std::vector< SCollisionPrimitive * > *pTargetPrimitive;

			if( !STATIC_INSTANCE( vecPickObject )[ i ]->IsEnableCollision() )
			{
				continue;
			}
			if( ( STATIC_INSTANCE( vecPickObject )[ i ]->GetCollisionGroup() & GetTargetCollisionGroup() ) == 0 )
			{
				continue;
			}
			pTargetPrimitive = STATIC_INSTANCE( vecPickObject )[ i ]->GetCollisionPrimitive();
			if( !pTargetPrimitive )
			{
				continue;
			}
			if( CEtCollisionEntity::FindCollision( *pTargetPrimitive, vMove, vecResponse, bCalcContactTime ) )
			{
				bRet = true;
			}
		}
	}

	return bRet;
}

bool CEtObject::FindCollision( EtMatrix &WorldMat, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime, bool bUpdaePrimitive )
{
	if( !IsEnableCollision() )
	{
		return false;
	}

	SAABox BoundingBox;
	EtVector3 vMove;
	bool bResult;
	PROFILE_TIME_TEST_BLOCK_START( "FindCollision1" );

	vMove.x = WorldMat._41 - m_WorldMat._41;
	vMove.y = WorldMat._42 - m_WorldMat._42;
	vMove.z = WorldMat._43 - m_WorldMat._43;
	GetBoundingBox( BoundingBox );
	BoundingBox.Max += vMove;
	BoundingBox.Min += vMove;

	if( bUpdaePrimitive )
	{
		UpdateCollisionPrimitive( WorldMat );
	}

	bResult = CEtCollisionMng::GetInstance(GetRoom()).FindCollision( m_vecPrimitive, BoundingBox, vMove, vecResponse, bCalcContactTime );

	if( _FindDynamicCollision( BoundingBox, vMove, vecResponse, bCalcContactTime ) )
	{
		bResult = true;
	}
	PROFILE_TIME_TEST_BLOCK_END();

	return bResult;
}

bool CEtObject::FindCollision( EtObjectHandle hObject, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime )
{
	int i, j;
	SCollisionResponse Response;
	bool bResult = false;
	std::vector< SCollisionPrimitive * > &vecTragetPrimitive = *hObject->GetCollisionPrimitive();

	PROFILE_TIME_TEST_BLOCK_START( "FindCollision2" );
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		for( j = 0; j < ( int )vecTragetPrimitive.size(); j++ )
		{
			if( !hObject->IsEnableCollision( vecTragetPrimitive[ j ] ) )
			{
				continue;
			}
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
	PROFILE_TIME_TEST_BLOCK_END();

	return bResult;
}

void CEtObject::GetExtent( EtVector3 &Origin, EtVector3 &Extent )
{
	Origin = ( m_BoundingBox.Max + m_BoundingBox.Min ) / 2;
	Extent = ( m_BoundingBox.Max - m_BoundingBox.Min ) / 2;
}

void CEtObject::SetWorldSize( CMultiRoom *pRoom, EtVector3 &WorldCenter, float fRadius ) 
{ 
	STATIC_INSTANCE_(s_DynamicOctree).Initialize( WorldCenter, fRadius );
	STATIC_INSTANCE_(s_DynamicOctree).UseLock( false );

	int i, nItemCount;

	nItemCount = GetItemCount(pRoom);
	for( i = 0; i < nItemCount; i++ )
	{
		EtObjectHandle hObject = GetItem( pRoom, i );
		if( hObject ) hObject->m_pCurOctreeNode = NULL;
	}
}
