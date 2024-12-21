#include "Stdafx.h"
#include "EtCollisionPrimitive.h"
#include "EtComputeDist.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void UpdateCollisionPrimitive( SCollisionPrimitive &DestPrimitive, SCollisionPrimitive &SourPrimitive, const EtMatrix &WorldMat, const EtVector3 &vScale )
{
	float fScale = max( max( vScale.x, vScale.y ), vScale.z );
	switch( SourPrimitive.Type )
	{
		case CT_BOX:
			{
				SCollisionBox *pDestBox = ( SCollisionBox * )&DestPrimitive;
				SCollisionBox *pSourBox = ( SCollisionBox * )&SourPrimitive;
				int i;

				EtVec3TransformCoord( &pDestBox->vCenter, &pSourBox->vCenter, &WorldMat );

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
				EtMatrix InvWorldMat;
				EtMatrixInverse( &InvWorldMat, NULL, &WorldMat );
				for( i = 0; i < 3; i++ )
				{
					EtVec3TransformNormal( pDestBox->vAxis + i, pSourBox->vAxis + i, &WorldNormalMat );
					pDestBox->fExtent[ i ] = pSourBox->fExtent[ i ] * ((FLOAT*)&vLength.x)[ i ];
				}
			}
			break;
		case CT_SPHERE:
			{
				SCollisionSphere *pDestSphere = ( SCollisionSphere * )&DestPrimitive;
				SCollisionSphere *pSourSphere = ( SCollisionSphere * )&SourPrimitive;

				EtVec3TransformCoord( &pDestSphere->vCenter, &pSourSphere->vCenter, &WorldMat );
				pDestSphere->fRadius = pSourSphere->fRadius * fScale;
			}
			break;
		case CT_CAPSULE:
			{
				SCollisionCapsule *pDestCapsule = ( SCollisionCapsule * )&DestPrimitive;
				SCollisionCapsule *pSourCapsule = ( SCollisionCapsule * )&SourPrimitive;

				EtVec3TransformCoord( &pDestCapsule->Segment.vOrigin, &pSourCapsule->Segment.vOrigin, &WorldMat );
				EtVector3 vOriginFar = pSourCapsule->Segment.vOrigin + pSourCapsule->Segment.vDirection;
				EtVec3TransformCoord( &vOriginFar, &vOriginFar, &WorldMat );
				pDestCapsule->Segment.vDirection = vOriginFar - pDestCapsule->Segment.vOrigin;

				pDestCapsule->fRadius = pSourCapsule->fRadius * fScale;
				if( fScale != 1.0f )
				{
					pDestCapsule->Segment.vOrigin = pDestCapsule->Segment.vOrigin - ( fScale - 1.0f ) * 0.5f * pDestCapsule->Segment.vDirection;
					pDestCapsule->Segment.vDirection += pDestCapsule->Segment.vDirection * ( fScale - 1.0f ) * 0.5f;
				}
			}
			break;
		case CT_TRIANGLE_LIST:
			{
				SCollisionTriangleList *pDestTriList = ( SCollisionTriangleList * )&DestPrimitive;
				SCollisionTriangleList *pSourTriList = ( SCollisionTriangleList * )&SourPrimitive;
				int i;

				pDestTriList->vecTriangle.resize( pSourTriList->vecTriangle.size() );
				for( i = 0; i < ( int )pDestTriList->vecTriangle.size(); i++ )
				{
					EtVector3 vPoint[ 3 ];
					vPoint[ 0 ] = pSourTriList->vecTriangle[ i ].vOrigin;
					vPoint[ 1 ] = pSourTriList->vecTriangle[ i ].vOrigin + pSourTriList->vecTriangle[ i ].vEdge1;
					vPoint[ 2 ] = pSourTriList->vecTriangle[ i ].vOrigin + pSourTriList->vecTriangle[ i ].vEdge2;
					EtVec3TransformCoordArray( vPoint, sizeof( EtVector3 ), vPoint, sizeof( EtVector3 ), &WorldMat, 3 );
					pDestTriList->vecTriangle[ i ].vOrigin = vPoint[ 0 ];
					pDestTriList->vecTriangle[ i ].vEdge1 = vPoint[ 1 ] - vPoint[ 0 ];
					pDestTriList->vecTriangle[ i ].vEdge2 = vPoint[ 2 ] - vPoint[ 0 ];
				}
			}
			break;
	}
}

bool IsInside( SCollisionPrimitive &Primitive, EtVector3 &vPoint )
{
	switch( Primitive.Type )
	{
		case CT_BOX:
			{
				SCollisionBox *pBox = ( SCollisionBox * )&Primitive;
				EtVector3 vVertex = vPoint - pBox->vCenter;

				float fDot = fabs( EtVec3Dot( &pBox->vAxis[ 0 ], &vVertex ) );
				if( fDot > pBox->fExtent[ 0 ] ) return false;

				fDot = fabs( EtVec3Dot( &pBox->vAxis[ 1 ], &vVertex ) );
				if( fDot > pBox->fExtent[ 1 ] ) return false;

				fDot = fabs( EtVec3Dot( &pBox->vAxis[ 2 ], &vVertex ) );
				if( fDot > pBox->fExtent[ 2 ] ) return false;

				return true;
			}
			break;
		case CT_SPHERE:
			{
				SCollisionSphere *pSphere = ( SCollisionSphere * )&Primitive;
				float fLengthSq;

				fLengthSq = EtVec3LengthSq( &( vPoint - pSphere->vCenter ) );
				return fLengthSq <= pSphere->fRadius * pSphere->fRadius;
			}
			break;
		case CT_CAPSULE:
			{
				SCollisionCapsule *pCapsule = ( SCollisionCapsule * )&Primitive;
				float fSegParam, fLengthSq;

				fLengthSq = DistPointToSegment( vPoint, pCapsule->Segment, fSegParam );
				return fLengthSq <= pCapsule->fRadius * pCapsule->fRadius;
			}
			break;
		default:
			return false;
	}
}

void GetCenterPos( SCollisionPrimitive &Primitive, EtVector3 &vPoint )
{
	switch( Primitive.Type )
	{
		case CT_BOX:
			{
				SCollisionBox *pBox = ( SCollisionBox * )&Primitive;
				vPoint = pBox->vCenter;
				return;
			}
			break;
		case CT_SPHERE:
			{
				SCollisionSphere *pSphere = ( SCollisionSphere * )&Primitive;
				vPoint = pSphere->vCenter;
			}
			break;
		case CT_CAPSULE:
			{
				SCollisionCapsule *pCapsule = ( SCollisionCapsule * )&Primitive;
				vPoint = pCapsule->Segment.vOrigin + pCapsule->Segment.vDirection * 0.5f;
			}
			break;
	}
}