#include "Stdafx.h"
#include "EtFindCollision.h"
#include "EtTestCollision.h"
#include "EtComputeDist.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define MIN_CONTACT_LENGTH_SQ	25.0f

CEtCollisionFinder g_CollisionFinder;
CEtCollisionFinder::CEtCollisionFinder()
{
	m_aCollisionTable[ CT_BOX ][ CT_TRIANGLE ] = ( CollisionFinder )FindCollisionDummy;
	m_aCollisionTable[ CT_BOX ][ CT_SPHERE ] = ( CollisionFinder )FindBoxToSphere;
	m_aCollisionTable[ CT_BOX ][ CT_BOX ] = ( CollisionFinder )FindBoxToBox;
	m_aCollisionTable[ CT_BOX ][ CT_CAPSULE ] = ( CollisionFinder )FindBoxToCapsule;

	m_aCollisionTable[ CT_SPHERE ][ CT_TRIANGLE ] = ( CollisionFinder )FindSphereToTriangle;
	m_aCollisionTable[ CT_SPHERE ][ CT_SPHERE ] = ( CollisionFinder )FindSphereToSphere;
	m_aCollisionTable[ CT_SPHERE ][ CT_BOX ] = ( CollisionFinder )FindSphereToBox;
	m_aCollisionTable[ CT_SPHERE ][ CT_CAPSULE ] = ( CollisionFinder )FindSphereToCapsule;

	m_aCollisionTable[ CT_CAPSULE ][ CT_TRIANGLE ] = ( CollisionFinder )FindCapsuleToTriangle;
	m_aCollisionTable[ CT_CAPSULE ][ CT_SPHERE ] = ( CollisionFinder )FindCapsuleToSphere;
	m_aCollisionTable[ CT_CAPSULE ][ CT_BOX ] = ( CollisionFinder )FindCapsuleToBox;
	m_aCollisionTable[ CT_CAPSULE ][ CT_CAPSULE ] = ( CollisionFinder )FindCapsuleToCapsule;

	m_aCollisionTable[ CT_TRIANGLE ][ CT_TRIANGLE ] = ( CollisionFinder )FindCollisionDummy;
	m_aCollisionTable[ CT_TRIANGLE ][ CT_SPHERE ] = ( CollisionFinder )FindCollisionDummy;
	m_aCollisionTable[ CT_TRIANGLE ][ CT_BOX ] = ( CollisionFinder )FindCollisionDummy;
	m_aCollisionTable[ CT_TRIANGLE ][ CT_CAPSULE ] = ( CollisionFinder )FindCollisionDummy;
}

CEtCollisionFinder::~CEtCollisionFinder()
{
}

bool CEtCollisionFinder::FindCollision( SCollisionPrimitive &Primitive1, SCollisionPrimitive &Primitive2, SCollisionResponse &Response, bool bCalcContactTime )
{
	if( ( Primitive1.Type >= CT_TRIANGLE_LIST ) || ( Primitive2.Type >= CT_TRIANGLE_LIST ) )
	{
		//ASSERT( 0 && "폴리곤 충돌인데 다이나믹 오브젝트로 설정됐다" );
		return false;
	}
	return m_aCollisionTable[ Primitive1.Type ][ Primitive2.Type ]( Primitive1, Primitive2, Response.vMove, Response, bCalcContactTime );
}

bool FindCollisionDummy( SCollisionPrimitive &Primitive1, SCollisionPrimitive &Primitive2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	return false;
}

bool FindBoxToSphere( SCollisionBox &Box, SCollisionSphere &Sphere, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	if( !TestBoxToSphere( Box, Sphere ) )
	{
		return false;
	}

	Response.vNormal = Box.vCenter - Sphere.vCenter;
	EtVec3Normalize( &Response.vNormal, &Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	if( EtVec3LengthSq( &Move ) > MIN_CONTACT_LENGTH_SQ )
	{
		float fCurTime, fStartTime, fEndTime;
		SCollisionBox Temporary;

		fStartTime = 0.0f;
		fEndTime = 1.0f;
		fCurTime = 0.5f;
		Temporary = Box;
		while( 1 )
		{
			Temporary.vCenter = Box.vCenter - Move * ( 1.0f - fCurTime );
			if( !TestBoxToSphere( Temporary, Sphere ) )
			{
				Response.fContactTime = fCurTime;
				fStartTime = fCurTime;
			}
			else
			{
				fEndTime = fCurTime;
			}
			fCurTime = ( fStartTime + fEndTime ) * 0.5f;
			if( EtVec3LengthSq( &( Move * ( fCurTime - fStartTime ) ) ) < MIN_CONTACT_LENGTH_SQ )
			{
				break;
			}
		}
	}

	return true;
}

bool FindBoxToBox( SCollisionBox &Box1, SCollisionBox &Box2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	if( !TestBoxToBox( Box1, Box2 ) )
	{
		return false;
	}

	FindPointToBox( Box1.vCenter - Move, Box2, Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	if( EtVec3LengthSq( &Move ) > MIN_CONTACT_LENGTH_SQ )
	{
		float fCurTime, fStartTime, fEndTime;
		SCollisionBox Temporary;

		fStartTime = 0.0f;
		fEndTime = 1.0f;
		fCurTime = 0.5f;
		Temporary = Box1;
		while( 1 )
		{
			Temporary.vCenter = Box1.vCenter - Move * ( 1.0f - fCurTime );
			if( !TestBoxToBox( Temporary, Box2 ) )
			{
				Response.fContactTime = fCurTime;
				fStartTime = fCurTime;
			}
			else
			{
				fEndTime = fCurTime;
			}
			fCurTime = ( fStartTime + fEndTime ) * 0.5f;
			if( EtVec3LengthSq( &( Move * ( fCurTime - fStartTime ) ) ) < MIN_CONTACT_LENGTH_SQ )
			{
				break;
			}
		}
	}

	return true;
}

bool FindBoxToCapsule( SCollisionBox &Box, SCollisionCapsule &Capsule, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fSegParam, fBoxParam0, fBoxParam1, fBoxParam2;

	if( !TestBoxToCapsule( Box, Capsule, fSegParam, fBoxParam0, fBoxParam1, fBoxParam2 ) )
	{
		return false;
	}

	EtVector3 SegmentPoint;

	SegmentPoint = Capsule.Segment.vOrigin + fSegParam * Capsule.Segment.vDirection;
	Response.vNormal = Box.vCenter - SegmentPoint;
	EtVec3Normalize( &Response.vNormal, &Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	if( EtVec3LengthSq( &Move ) > MIN_CONTACT_LENGTH_SQ )
	{
		float fCurTime, fStartTime, fEndTime;
		SCollisionBox Temporary;

		fStartTime = 0.0f;
		fEndTime = 1.0f;
		fCurTime = 0.5f;
		Temporary = Box;
		while( 1 )
		{
			Temporary.vCenter = Box.vCenter - Move * ( 1.0f - fCurTime );
			if( !TestBoxToCapsule( Temporary, Capsule, fSegParam, fBoxParam0, fBoxParam1, fBoxParam2 ) )
			{
				Response.fContactTime = fCurTime;
				fStartTime = fCurTime;
			}
			else
			{
				fEndTime = fCurTime;
			}
			fCurTime = ( fStartTime + fEndTime ) * 0.5f;
			if( EtVec3LengthSq( &( Move * ( fCurTime - fStartTime ) ) ) < MIN_CONTACT_LENGTH_SQ )
			{
				break;
			}
		}
	}

	return true;
}

bool FindSphereToTriangle( SCollisionSphere &Sphere, SCollisionTriangle &Triangle, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fTriParam1, fTriParam2;

	if( !TestSphereToTriangle( Sphere, Triangle, fTriParam1, fTriParam2 ) )
	{
		return false;
	}

	Triangle.GetNormal( Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	float fMoveDist;

	fMoveDist = EtVec3Length( &Move );
	if( fMoveDist > 0.0f )
	{
		EtVector3 vSphereToTriNormal, vMoveNormal, vTriPoint;
		float fRadiusDiff, fSphereToTriLength, fDot;

		vMoveNormal = Move / fMoveDist;
		vTriPoint = Triangle.vOrigin + Triangle.vEdge1 * fTriParam1 + Triangle.vEdge2 * fTriParam2;
		vSphereToTriNormal = vTriPoint - Sphere.vCenter;
		fSphereToTriLength = EtVec3Length( &vSphereToTriNormal );
		vSphereToTriNormal /= fSphereToTriLength;

		fRadiusDiff = Sphere.fRadius - fSphereToTriLength;
		fDot = EtVec3Dot( &vSphereToTriNormal, &vMoveNormal );
		if( fDot != 0.0f )
		{
			fRadiusDiff /= fDot;
			Response.fContactTime = 1.0f - fRadiusDiff / fMoveDist;
			Response.fContactTime = min( max( Response.fContactTime, 0.0f ), 1.0f );
		}
	}

	return true;
}

bool FindSphereToSphere( SCollisionSphere &Sphere1, SCollisionSphere &Sphere2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	if( !TestSphereToSphere( Sphere1, Sphere2 ) )
	{
		return false;
	}

	Response.vNormal = Sphere1.vCenter - Sphere2.vCenter;
	EtVec3Normalize( &Response.vNormal, &Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	if( EtVec3LengthSq( &Move ) > MIN_CONTACT_LENGTH_SQ )
	{
		float fCurTime, fStartTime, fEndTime;
		SCollisionSphere Temporary;

		fStartTime = 0.0f;
		fEndTime = 1.0f;
		fCurTime = 0.5f;
		Temporary = Sphere1;
		while( 1 )
		{
			Temporary.vCenter = Sphere1.vCenter - Move * ( 1.0f - fCurTime );
			if( !TestSphereToSphere( Temporary, Sphere2 ) )
			{
				Response.fContactTime = fCurTime;
				fStartTime = fCurTime;
			}
			else
			{
				fEndTime = fCurTime;
			}
			fCurTime = ( fStartTime + fEndTime ) * 0.5f;
			if( EtVec3LengthSq( &( Move * ( fCurTime - fStartTime ) ) ) < MIN_CONTACT_LENGTH_SQ )
			{
				break;
			}
		}
	}

	return true;
}

bool FindSphereToBox( SCollisionSphere &Sphere, SCollisionBox &Box, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	if( !TestBoxToSphere( Box, Sphere ) )
	{
		return false;
	}

	FindPointToBox( Sphere.vCenter - Move, Box, Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	if( EtVec3LengthSq( &Move ) > MIN_CONTACT_LENGTH_SQ )
	{
		float fCurTime, fStartTime, fEndTime;
		SCollisionSphere Temporary;

		fStartTime = 0.0f;
		fEndTime = 1.0f;
		fCurTime = 0.5f;
		Temporary = Sphere;
		while( 1 )
		{
			Temporary.vCenter = Sphere.vCenter - Move * ( 1.0f - fCurTime );
			if( !TestBoxToSphere( Box, Temporary ) )
			{
				Response.fContactTime = fCurTime;
				fStartTime = fCurTime;
			}
			else
			{
				fEndTime = fCurTime;
			}
			fCurTime = ( fStartTime + fEndTime ) * 0.5f;
			if( EtVec3LengthSq( &( Move * ( fCurTime - fStartTime ) ) ) < MIN_CONTACT_LENGTH_SQ )
			{
				break;
			}
		}
	}

	return true;
}

bool FindSphereToCapsule( SCollisionSphere &Sphere, SCollisionCapsule &Capsule, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fSegParam;

	if( !TestSphereToCapsule( Sphere, Capsule, fSegParam ) )
	{
		return false;
	}

	EtVector3 SegmentPoint;

	SegmentPoint = Capsule.Segment.vOrigin + fSegParam * Capsule.Segment.vDirection;
	Response.vNormal = Sphere.vCenter - SegmentPoint;
	EtVec3Normalize( &Response.vNormal, &Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	if( EtVec3LengthSq( &Move ) > MIN_CONTACT_LENGTH_SQ )
	{
		float fCurTime, fStartTime, fEndTime;
		SCollisionSphere Temporary;

		fStartTime = 0.0f;
		fEndTime = 1.0f;
		fCurTime = 0.5f;
		Temporary = Sphere;
		while( 1 )
		{
			Temporary.vCenter = Sphere.vCenter - Move * ( 1.0f - fCurTime );
			if( !TestSphereToCapsule( Temporary, Capsule, fSegParam ) )
			{
				Response.fContactTime = fCurTime;
				fStartTime = fCurTime;
			}
			else
			{
				fEndTime = fCurTime;
			}
			fCurTime = ( fStartTime + fEndTime ) * 0.5f;
			if( EtVec3LengthSq( &( Move * ( fCurTime - fStartTime ) ) ) < MIN_CONTACT_LENGTH_SQ )
			{
				break;
			}
		}
	}

	return true;
}

bool FindCapsuleToTriangle( SCollisionCapsule &Capsule, SCollisionTriangle &Triangle, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fSegParam, fTriParam1, fTriParam2;

	if( !TestCapsuleToTriangle( Capsule, Triangle, fSegParam, fTriParam1, fTriParam2 ) )
	{
		return false;
	}
	Triangle.GetNormal( Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	float fMoveDist;

	fMoveDist = EtVec3Length( &Move );
	if( fMoveDist > 0.0f )
	{
		SSegment TestSegment;
		float fCapsuleLength, fContactTime;
		EtVector3 vTriPoint, vMoveNormal;

		vMoveNormal = Move / fMoveDist;
		vTriPoint = Triangle.vOrigin + fTriParam1 * Triangle.vEdge1 + fTriParam2 * Triangle.vEdge2;

		fCapsuleLength = EtVec3Length( &Capsule.Segment.vDirection ) + Capsule.fRadius * 2.0f;
		TestSegment.vOrigin = vTriPoint + vMoveNormal * fCapsuleLength;
		TestSegment.vDirection = -vMoveNormal * fCapsuleLength;
		FindSegmentToCapsule( TestSegment, Capsule, fContactTime );
		Response.fContactTime = min( 1.0f - fCapsuleLength * ( 1.0f - fContactTime ) / fMoveDist, 1.0f );
	}

	return true;
}

bool FindCapsuleToSphere( SCollisionCapsule &Capsule, SCollisionSphere &Sphere, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fSegParam;

	if( !TestSphereToCapsule( Sphere, Capsule, fSegParam ) )
	{
		return false;
	}

	EtVector3 vSegmentPoint;

	vSegmentPoint = Capsule.Segment.vOrigin + fSegParam * Capsule.Segment.vDirection;
	Response.vNormal = vSegmentPoint - Sphere.vCenter;
	EtVec3Normalize( &Response.vNormal, &Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	float fMoveDist;

	fMoveDist = EtVec3Length( &Move );
	if( fMoveDist > 0.0f )
	{
		EtVector3 vCapsuleToSphereNormal, vMoveNormal;
		float fRadiusDiff, fCapsuleToSphereLength, fDot;

		vMoveNormal = Move / fMoveDist;
		vCapsuleToSphereNormal = Sphere.vCenter - vSegmentPoint;
		fCapsuleToSphereLength = EtVec3Length( &vCapsuleToSphereNormal );
		vCapsuleToSphereNormal /= fCapsuleToSphereLength;

		fRadiusDiff = ( Capsule.fRadius + Sphere.fRadius ) - fCapsuleToSphereLength;
		fDot = EtVec3Dot( &vCapsuleToSphereNormal, &vMoveNormal );
		if( fDot != 0.0f )
		{
			fRadiusDiff /= fDot;
			Response.fContactTime = 1.0f - fRadiusDiff / fMoveDist;
			Response.fContactTime = min( max( Response.fContactTime, 0.0f ), 1.0f );
		}
	}

	return true;
}

bool FindCapsuleToBox( SCollisionCapsule &Capsule, SCollisionBox &Box, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fSegParam, fBoxParam[ 3 ];

	if( !TestBoxToCapsule( Box, Capsule, fSegParam, fBoxParam[ 0 ], fBoxParam[ 1 ], fBoxParam[ 2 ] ) )
	{
		return false;
	}

	int i;
	float fMoveDist;
	EtVector3 vMoveNormal;
	EtVector3 vBoxPoint, vSegmentPoint;

	vSegmentPoint = Capsule.Segment.vOrigin + fSegParam * Capsule.Segment.vDirection;
	vBoxPoint = Box.vCenter + fBoxParam[ 0 ] * Box.vAxis[ 0 ];
	vBoxPoint += fBoxParam[ 1 ] * Box.vAxis[ 1 ];
	vBoxPoint += fBoxParam[ 2 ] * Box.vAxis[ 2 ];

	fMoveDist = EtVec3Length( &Move );
	vMoveNormal = Move / fMoveDist;
	if( fMoveDist > 0.0f )
	{
		float fMaxDot = -FLT_MAX;
		for( i = 0; i < 3; i++ )
		{
			float fDot = EtVec3Dot( &vMoveNormal, Box.vAxis + i );
			float fAbsDot = fabs( fDot );
			EtVector4 vPlane;
			EtVector3 vCurPos, vPlaneNormal;
			if( fDot > 0.0f )
			{
				vPlaneNormal = -Box.vAxis[ i ];
			}
			else
			{
				vPlaneNormal = Box.vAxis[ i ];
			}
			vCurPos = Box.vCenter + vPlaneNormal * Box.fExtent[ i ];
			EtPlaneFromPointNormal( ( EtPlane * )&vPlane, &vCurPos, &vPlaneNormal );
			if( TestPlaneToCapsule( vPlane, Capsule ) )
			{
				EtVector3 *pvTarget = &Response.vNormal;
				if( fAbsDot > fMaxDot )	// 기존에 설정되 있는 노멀이 이동방향과 충돌각이 작으면 기존노멀이 vNormal에 들어가고 현재축이 vExtraNormal에 들어간다.
				{
					if( fMaxDot != -FLT_MAX )
					{
						Response.vExtraNormal = Response.vNormal;
					}
				}
				else
				{
					pvTarget = &Response.vExtraNormal;
				}
				*pvTarget = vPlaneNormal;
				fMaxDot = fAbsDot;
			}
		}
		if( fMaxDot == -FLT_MAX )
		{
			Response.vNormal = EtVector3( 0.0f, 0.0f, 0.0f );
			Response.fContactTime = 0.0;
			// 일단 충돌 아닌걸로 하자.. 클라쪽에서.. 낑기면 못빠져 나올때 있어서.. 
			return false;
		}
	}
	else
	{
		FindPointToBox( vSegmentPoint - Move, Box, Response.vNormal );
	}

	if( !bCalcContactTime )
	{
		return true;
	}

	if( fMoveDist > 0.0f )
	{
		SSegment TestSegment;
		float fCapsuleLength, fContactTime;

		fCapsuleLength = EtVec3Length( &Capsule.Segment.vDirection ) + Capsule.fRadius * 2.0f;
		TestSegment.vOrigin = vBoxPoint + vMoveNormal * fCapsuleLength;
		TestSegment.vDirection = -vMoveNormal * fCapsuleLength;
		FindSegmentToCapsule( TestSegment, Capsule, fContactTime );
		Response.fContactTime = min( 1.0f - fCapsuleLength * ( 1.0f - fContactTime ) / fMoveDist, 1.0f );
	}

	return true;
}

bool FindCapsuleToCapsule( SCollisionCapsule &Capsule1, SCollisionCapsule &Capsule2, EtVector3 &Move, SCollisionResponse &Response, bool bCalcContactTime )
{
	float fSegParam1, fSegParam2;

	if( !TestCapsuleToCapsule( Capsule1, Capsule2, fSegParam1, fSegParam2 ) )
	{
		return false;
	}

	EtVector3 vSegmentPoint1, vSegmentPoint2;

	vSegmentPoint1 = Capsule1.Segment.vOrigin + fSegParam1 * Capsule1.Segment.vDirection;
	vSegmentPoint2 = Capsule2.Segment.vOrigin + fSegParam2 * Capsule2.Segment.vDirection;
	Response.vNormal = vSegmentPoint1 - vSegmentPoint2;
	EtVec3Normalize( &Response.vNormal, &Response.vNormal );

	if( !bCalcContactTime )
	{
		return true;
	}

	float fMoveDist;

	fMoveDist = EtVec3Length( &Move );
	if( fMoveDist > 0.0f )
	{
		EtVector3 vCapsule1ToCapsule2Normal, vMoveNormal;
		float fRadiusDiff, fCapsule1ToCapsule2Length, fDot;

		vMoveNormal = Move / fMoveDist;
		vCapsule1ToCapsule2Normal = vSegmentPoint2 - vSegmentPoint1;
		fCapsule1ToCapsule2Length = EtVec3Length( &vCapsule1ToCapsule2Normal );
		vCapsule1ToCapsule2Normal /= fCapsule1ToCapsule2Length;

		fRadiusDiff = ( Capsule1.fRadius + Capsule2.fRadius ) - fCapsule1ToCapsule2Length;
		fDot = EtVec3Dot( &vCapsule1ToCapsule2Normal, &vMoveNormal );
		if( fDot != 0.0f )
		{
			fRadiusDiff /= fDot;
			Response.fContactTime = 1.0f - fRadiusDiff / fMoveDist;
			Response.fContactTime = min( max( Response.fContactTime, 0.0f ), 1.0f );
		}
	}

	return true;
}

void FindPointToBox( EtVector3 &Point, SCollisionBox &Box, EtVector3 &CollisionNormal )
{
	EtVector3 Direction, TransformDir;

	Direction = Point - Box.vCenter;
	TransformDir.x = EtVec3Dot( &Direction, Box.vAxis ) / Box.fExtent[ 0 ];
	TransformDir.y = EtVec3Dot( &Direction, Box.vAxis + 1 ) / Box.fExtent[ 1 ];
	TransformDir.z = EtVec3Dot( &Direction, Box.vAxis + 2 ) / Box.fExtent[ 2 ];

	if( fabs( TransformDir.x ) > fabs( TransformDir.y ) )
	{
		if( fabs( TransformDir.x ) > fabs( TransformDir.z ) )
		{
			if( TransformDir.x > 0 )
			{
				CollisionNormal = Box.vAxis[ 0 ];
			}
			else
			{
				CollisionNormal = -Box.vAxis[ 0 ];
			}
		}
		else
		{
			if( TransformDir.z > 0 )
			{
				CollisionNormal = Box.vAxis[ 2 ];
			}
			else
			{
				CollisionNormal = -Box.vAxis[ 2 ];
			}
		}
	}
	else
	{
		if( fabs( TransformDir.y ) > fabs( TransformDir.z ) )
		{
			if( TransformDir.y > 0 )
			{
				CollisionNormal = Box.vAxis[ 1 ];
			}
			else
			{
				CollisionNormal = -Box.vAxis[ 1 ];
			}
		}
		else
		{
			if( TransformDir.z > 0 )
			{
				CollisionNormal = Box.vAxis[ 2 ];
			}
			else
			{
				CollisionNormal = -Box.vAxis[ 2 ];
			}
		}
	}
}
