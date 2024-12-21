#include "StdAfx.h"
#include "EtCollisionMng.h"
#include "EtFindCollision.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CEtCollisionMng::CEtCollisionMng(void)
{
}

CEtCollisionMng::~CEtCollisionMng(void)
{
	Clear();
}

void CEtCollisionMng::Clear()
{
	m_CollisionKdTree.Clear();
	SAFE_DELETE_AVEC( m_vecAllocPrimitive );
}

void CEtCollisionMng::AddCollisionMeshToKdTree( SCollisionPrimitive &Primitive, EtMatrix &WorldMat, const EtVector3 &vScale )
{
	if( m_CollisionKdTree.IsBuild() )
	{
		m_CollisionKdTree.Clear();
	}

	SKdTreeCollisionPrimitive KdTreePrimitive;
	float fScale = max( max( vScale.x, vScale.y ), vScale.z );

	switch( Primitive.Type )
	{
		case CT_TRIANGLE_LIST:
			{
				SCollisionTriangleList *pCollisionTriangle;
				SCollisionTriangle *pTriangle;
				int i, nTriangleCount;

				pCollisionTriangle = ( SCollisionTriangleList * )&Primitive;
				nTriangleCount = ( int )pCollisionTriangle->vecTriangle.size();
				pTriangle = new SCollisionTriangle[ nTriangleCount ];
				m_vecAllocPrimitive.push_back( pTriangle );

				for( i = 0; i < nTriangleCount; i++ )
				{
					EtVector3 vPoint[ 3 ];

					KdTreePrimitive.pPrimitive = pTriangle + i;
					vPoint[ 0 ] = pCollisionTriangle->vecTriangle[ i ].vOrigin;
					vPoint[ 1 ] = pCollisionTriangle->vecTriangle[ i ].vOrigin + pCollisionTriangle->vecTriangle[ i ].vEdge1;
					vPoint[ 2 ] = pCollisionTriangle->vecTriangle[ i ].vOrigin + pCollisionTriangle->vecTriangle[ i ].vEdge2;
					EtVec3TransformCoordArray( vPoint, sizeof( EtVector3 ), vPoint, sizeof( EtVector3 ), &WorldMat, 3 );
					pTriangle[ i ].vOrigin = vPoint[ 0 ];
					pTriangle[ i ].vEdge1 = vPoint[ 1 ] - vPoint[ 0 ];
					pTriangle[ i ].vEdge2 = vPoint[ 2 ] - vPoint[ 0 ];

					m_CollisionKdTree.AddPrimitive( KdTreePrimitive );
				}
			}
			break;
		case CT_BOX:
			{
				int i;
				SCollisionBox *pBox, *pOriginalBox;

				pOriginalBox = ( SCollisionBox * )&Primitive;
				pBox = new SCollisionBox();
				KdTreePrimitive.pPrimitive = pBox;
				m_vecAllocPrimitive.push_back( pBox );

				EtVec3TransformCoord( &pBox->vCenter, &pOriginalBox->vCenter, &WorldMat );

				EtMatrix WorldNormalMat = WorldMat;
				EtVector3 vLength;
				vLength.x = EtVec3Length((EtVector3*)&WorldNormalMat._11);
				vLength.y = EtVec3Length((EtVector3*)&WorldNormalMat._21);
				vLength.z = EtVec3Length((EtVector3*)&WorldNormalMat._31);
				
				*(EtVector3*)(&WorldNormalMat._11) /= vLength.x;
				*(EtVector3*)(&WorldNormalMat._21) /= vLength.y;
				*(EtVector3*)(&WorldNormalMat._31) /= vLength.z;

				// Non-Uniform Scale Matrix 로 좌표계를 변환할 경우 변환뒤에 Normalize를 해주더라도 
				// 세개의 축이 직교함을 보장하지 못하므로, 미리 Orthogonal Matrix 로 만든뒤 변환한다.

				for( i = 0; i < 3; i++ )
				{
					EtVec3TransformNormal( pBox->vAxis + i, pOriginalBox->vAxis + i, &WorldNormalMat );					
					pBox->fExtent[ i ] = pOriginalBox->fExtent[ i ] * ((FLOAT*)&vLength.x)[ i ];
				}

				/*EtMatrix MatIdent;
				EtMatrixIdentity(&MatIdent);
				*(EtVector3*)&MatIdent._11 = pBox->vAxis[0];
				*(EtVector3*)&MatIdent._21 = pBox->vAxis[1];
				*(EtVector3*)&MatIdent._31 = pBox->vAxis[2];
				float fDet = fabsf(EtMatrixDeterminant(&MatIdent) - 1.0f);
				ASSERT( fDet < 0.0001f );*/

				m_CollisionKdTree.AddPrimitive( KdTreePrimitive );
			}
			break;
		case CT_SPHERE:
			SCollisionSphere *pSphere, *pOriginalSphere;

			pOriginalSphere = ( SCollisionSphere * )&Primitive;
			pSphere = new SCollisionSphere();
			KdTreePrimitive.pPrimitive = pSphere;
			m_vecAllocPrimitive.push_back( pSphere );

			EtVec3TransformCoord( &pSphere->vCenter, &pOriginalSphere->vCenter, &WorldMat );
			pSphere->fRadius = pOriginalSphere->fRadius * fScale;

			m_CollisionKdTree.AddPrimitive( KdTreePrimitive );
			break;
		case CT_CAPSULE:
			SCollisionCapsule *pCapsule, *pOriginalCapsule;

			pOriginalCapsule = ( SCollisionCapsule * )&Primitive;
			pCapsule = new SCollisionCapsule();
			KdTreePrimitive.pPrimitive = pCapsule;
			m_vecAllocPrimitive.push_back( pCapsule );

			EtVec3TransformCoord( &pCapsule->Segment.vOrigin, &pOriginalCapsule->Segment.vOrigin, &WorldMat );
			EtVector3 vOriginFar = pOriginalCapsule->Segment.vOrigin + pOriginalCapsule->Segment.vDirection;
			EtVec3TransformCoord( &vOriginFar, &vOriginFar, &WorldMat );
			pCapsule->Segment.vDirection = vOriginFar - pCapsule->Segment.vOrigin;

			pCapsule->fRadius = pOriginalCapsule->fRadius * fScale;
			if( fScale != 1.0f )
			{
				pCapsule->Segment.vOrigin = pCapsule->Segment.vOrigin - ( fScale - 1.0f ) * 0.5f * pCapsule->Segment.vDirection;
				pCapsule->Segment.vDirection += pCapsule->Segment.vDirection * ( fScale - 1.0f ) * 0.5f;
			}

			m_CollisionKdTree.AddPrimitive( KdTreePrimitive );
			break;
	}
}

void CEtCollisionMng::BuildCollisionTree()
{
	m_CollisionKdTree.BuildTree();
}

bool CEtCollisionMng::FindCollision( std::vector< SCollisionPrimitive * > &vecPrimitive, SAABox &BoundingBox, EtVector3 &vMove, DNVector(SCollisionResponse) &vecResponse, bool bCalcContactTime)
{
	int i;
	bool bCollision;
	static std::vector< SKdTreeCollisionPrimitive * > vecResult;
	SCollisionResponse Response;

	bCollision = false;
	vecResult.clear();
	m_CollisionKdTree.Pick( BoundingBox, vecResult );
	if( !vecResult.empty() )
	{
		std::vector< SKdTreeCollisionPrimitive * >::iterator it;
		Response.vMove = vMove;
		for( i = 0; i < ( int )vecPrimitive.size(); i++ )
		{
			for( it = vecResult.begin() ; it != vecResult.end(); it++ )
			{
				Response.vExtraNormal.x = FLT_MAX;
				if( CEtCollisionFinder::GetInstance().FindCollision( *vecPrimitive[ i ], *( ( *it )->pPrimitive), Response, bCalcContactTime ) )
				{
					Response.pCollisionPrimitive = ( *it )->pPrimitive;
					vecResponse.push_back( Response );
					if( Response.vExtraNormal.x != FLT_MAX )
					{
						Response.vNormal = Response.vExtraNormal;
						vecResponse.push_back( Response );
					}
					bCollision = true;
				}
			}
		}
	}

	return bCollision;
}
