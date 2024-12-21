#include "StdAfx.h"
#include "EtTestCollision.h"
#include "EtFindCollision.h"
#include "EtCollisionEntity.h"
#include "EtGenerateCollisionMesh.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CEtCollisionEntity::CEtCollisionEntity(void)
{
	m_pvecOriginalPrimitive = NULL;
	m_vScale = EtVector3( 1.0f, 1.0f, 1.0f );
	m_nCollisionGroup = 0;
	m_nTargetCollisionGroup = -1;
	m_bDraw = false;
	m_bEnableCollision = false;
	m_nVertexDecl = -1;
	m_bUpdateNeeded = false;

	SetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) );
	SetTargetCollisionGroup( 0 );
}

CEtCollisionEntity::~CEtCollisionEntity(void)
{
	SAFE_DELETE_PVEC( m_vecPrimitive );
	SAFE_DELETE_PVEC( m_vecMeshStream );
}

void CEtCollisionEntity::Initialize( std::vector< SCollisionPrimitive * > &vecPrimitive, std::vector< int > &vecPrimitiveBoneIndex )
{
	int i;
	SCollisionPrimitive *pBasePrimitive;

	SAFE_DELETE_PVEC( m_vecPrimitive );
	SAFE_DELETE_PVEC( m_vecMeshStream );
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
		m_vecMeshStream.push_back( new CEtMeshStream() );
		m_vecPrimitive.push_back( pBasePrimitive );
		m_vecPrimitiveEnable[ i ] = true;
	}
	m_bEnableCollision = true;

//	ShowCollisionPrimitive( true );
}

void CEtCollisionEntity::ShowCollisionPrimitive( bool bShow ) 
{ 
	m_bDraw = bShow; 
	if( ( bShow ) && ( m_nVertexDecl == -1 ) )
	{
		D3DVERTEXELEMENT9 VertexElement[] = {	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
																	{1, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
																	D3DDECL_END() };

		m_nVertexDecl = GetEtDevice()->CreateVertexDeclaration( VertexElement );
	}
	if( bShow ) {
		CEtObject::SkipDrawCollisionList( false );
	}
}

void CEtCollisionEntity::UpdateCollisionPrimitive( int nIndex, EtMatrix &WorldMat, bool bUpdate )
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

	if( bUpdate ) {
		::UpdateCollisionPrimitive( *m_vecPrimitive[ nIndex ], *( *m_pvecOriginalPrimitive )[ nIndex ], WorldMat, m_vScale );
	}

	if( m_bDraw )
	{
		switch( ( *m_pvecOriginalPrimitive )[ nIndex ]->Type )
		{
		case CT_BOX:
			GenerateBoxCollisionMesh( *m_vecMeshStream[ nIndex ], *( SCollisionBox * )m_vecPrimitive[ nIndex ], 0xffff0000 );
			break;
		case CT_SPHERE:
			GenerateSphereCollisionMesh( *m_vecMeshStream[ nIndex ], *( SCollisionSphere * )m_vecPrimitive[ nIndex ], 0xffff0000 );
			break;
		case CT_CAPSULE:
			GenerateCapsuleCollisionMesh( *m_vecMeshStream[ nIndex ], *( SCollisionCapsule * )m_vecPrimitive[ nIndex ], 0xffff0000 );
			break;
		case CT_TRIANGLE_LIST:
			GenerateTriangleListCollisionMesh( *m_vecMeshStream[ nIndex ], *( SCollisionTriangleList * )( *m_pvecOriginalPrimitive )[ nIndex ], WorldMat );
			break;
		}
	}
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
	if( !m_bDraw )
	{
		return;
	}
	if( !IsEnableCollision() ) return;

	int i;
	EtMatrix WorldMat;
	EtCameraHandle hCamera;

	hCamera = CEtCamera::GetActiveCamera();
	EtMatrixIdentity( &WorldMat );
	GetEtDevice()->SetWorldTransform( &WorldMat );
	GetEtDevice()->SetViewTransform( hCamera->GetViewMat() );
	GetEtDevice()->SetProjTransform( hCamera->GetProjMat() );
	GetEtDevice()->SetVertexShader( NULL );
	GetEtDevice()->SetPixelShader( NULL );
	GetEtDevice()->SetFVF( FVF_XYZ | FVF_DIFFUSE );
	GetEtDevice()->SetTexture( 0, NULL );
	GetEtDevice()->SetCullMode( CULL_NONE );
	GetEtDevice()->SetWireframe( true );
	for( i = 0; i < ( int )m_vecPrimitive.size(); i++ )
	{
		if( !m_vecPrimitiveEnable[ i ] )
		{
			continue;
		}
		m_vecMeshStream[ i ]->Draw( m_nVertexDecl );
	}
	GetEtDevice()->SetWireframe( false );
	GetEtDevice()->SetCullMode( CULL_CCW );
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

bool CEtCollisionEntity::FindCollision( std::vector< SCollisionPrimitive * > &vecTragetPrimitive, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime)
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
			case CT_TRIANGLE_LIST:
				{
					SCollisionTriangleList *pTriList = ( SCollisionTriangleList * )m_vecPrimitive[ i ];
					int j;
					for( j = 0; j < ( int )pTriList->vecTriangle.size(); j++ )
					{
						if( TestSegmentToTriangle( Segment, pTriList->vecTriangle[ j ] ) )
						{
							return true;
						}
					}
				}
				break;
		}
	}

	return false;
}

bool CEtCollisionEntity::FindSegmentCollision( SSegment &Segment, SCollisionResponse &Response )
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
					if( fContactTime < Response.fContactTime )
					{
						Response.fContactTime = fContactTime;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
			case CT_TRIANGLE_LIST:
				{
					SCollisionTriangleList *pTriList = ( SCollisionTriangleList * )m_vecPrimitive[ i ];
					int j;
					for( j = 0; j < ( int )pTriList->vecTriangle.size(); j++ )
					{
						if( FindSegmentToTriangle( Segment, pTriList->vecTriangle[ j ], fContactTime ) )
						{
							if( fContactTime > 0.f && fContactTime < Response.fContactTime )
							{
								Response.fContactTime = fContactTime;
								Response.pCollisionPrimitive = &pTriList->vecTriangle[ j ];
							}
							bResult = true;
						}
					}
				}
				break;
		}
	}

	return bResult;
}

bool CEtCollisionEntity::FindCapsuleCollision( SCollisionCapsule &Capsule, SCollisionResponse &Response )
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
					if( fParam1 < Response.fContactTime )
					{
						Response.fContactTime = fParam1;
						Response.pCollisionPrimitive = m_vecPrimitive[ i ];
					}
					bResult = true;
				}
				break;
			case CT_TRIANGLE_LIST:
				{
					SCollisionTriangleList *pTriList = ( SCollisionTriangleList * )m_vecPrimitive[ i ];
					int j;
					for( j = 0; j < ( int )pTriList->vecTriangle.size(); j++ )
					{
						if( TestCapsuleToTriangle( Capsule, pTriList->vecTriangle[ j ], fParam1, fParam2, fParam2 ) )
						{
							Response.fContactTime = fParam1;
							Response.pCollisionPrimitive = m_vecPrimitive[ i ];
							return true;
						}
					}
				}
				break;
		}
	}

	return bResult;;
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
