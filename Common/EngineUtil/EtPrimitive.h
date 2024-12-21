#pragma once

#include "MemPool.h"

struct SAABox2D;

struct SSegment
{
	EtVector3 vOrigin;
	EtVector3 vDirection;
};

struct SCircle
{
	bool IsInside( EtVector2 &vPoint )
	{
		if( EtVec2LengthSq( &( vPoint - Center ) ) < fRadius * fRadius )
		{
			return true;
		}
		return false;
	}
	bool IsInside( SCircle &Circle )
	{
		float fRadiusDiff;

		if( fRadius < Circle.fRadius )
		{
			return false;
		}
		fRadiusDiff = fRadius - Circle.fRadius;
		if( EtVec2LengthSq( &( Center - Circle.Center ) ) < fRadiusDiff * fRadiusDiff )
		{
			return true;
		}
		return false;
	}
	bool IsInside( SAABox2D &Box );

	EtVector2 Center;
	float fRadius;
};

struct SAABox2D
{
	SAABox2D() {}
	SAABox2D( EtVector2 *pMin, EtVector2 *pMax )
	{
		Min = *pMin;
		Max = *pMax;
	}

	void GetVertices( DNVector(EtVector2) &vVecResult )
	{
		vVecResult.push_back( EtVector2( Min.x, Min.y ) );
		vVecResult.push_back( EtVector2( Max.x, Min.y ) );
		vVecResult.push_back( EtVector2( Max.x, Max.y ) );
		vVecResult.push_back( EtVector2( Min.x, Max.y ) );
	}
	void Reset()
	{
		Min = EtVector2( FLT_MAX, FLT_MAX );
		Max = EtVector2( -FLT_MAX, -FLT_MAX );
	}
	void AddPoint( EtVector2 &Point )
	{
		Min.x = min( Point.x, Min.x );
		Max.x = max( Point.x, Max.x );
		Min.y = min( Point.y, Min.y );
		Max.y = max( Point.y, Max.y );
	}
	bool IsInside( SCircle &Circle )
	{
		if( Max.x < Circle.Center.x + Circle.fRadius )
		{
			return false;
		}
		if( Max.y < Circle.Center.y + Circle.fRadius )
		{
			return false;
		}
		if( Min.x > Circle.Center.x - Circle.fRadius )
		{
			return false;
		}
		if( Min.y > Circle.Center.y - Circle.fRadius )
		{
			return false;
		}

		return true;
	}
	EtVector2 Min;
	EtVector2 Max;
};

inline bool SCircle::IsInside( SAABox2D &Box )
{
	int i;
	float fRadiusSq;
	DNVector(EtVector2) vecVertex;
	Box.GetVertices( vecVertex );
	fRadiusSq = fRadius * fRadius;
	for( i = 0; i < ( int )vecVertex.size(); i++ )
	{
		if( EtVec2LengthSq( &( vecVertex[ i ] - Center ) ) > fRadiusSq )
		{
			return false;
		}
	}
	return true;
}

struct SAABox;
struct SSphere
{
	SSphere()
		: Center(0.0f,0.0f,0.0f), fRadius(0.0f)
	{

	}
	bool IsInside( SAABox &Box );
	bool IsInside( SSphere &Sphere )
	{
		if( fRadius < Sphere.fRadius )
			return false;
	
		EtVector3	CenterDist	= Center - Sphere.Center;
		float		fRadiusDist = fRadius - Sphere.fRadius;

		return EtVec3LengthSq( &CenterDist ) <= fRadiusDist * fRadiusDist;
	}
	bool IsInside( EtVector3 &Point )
	{
		if( EtVec3LengthSq( &( Point - Center ) ) < fRadius * fRadius )
		{
			return true;
		}
		return false;
	}

	EtVector3 Center;
	float fRadius;
};

struct SCapsule
{
public:
	SSegment Segment;
	float fRadius;
};

struct SAABox
{
	SAABox() {}
	SAABox( EtVector3 *pMin, EtVector3 *pMax )
	{
		Min = *pMin;
		Max = *pMax;
	}

	void GetVertex( int nIndex, EtVector3 &vPoint )
	{
		switch( nIndex )
		{
			case 0:
				vPoint = EtVector3( Min.x, Min.y, Min.z );
				break;
			case 1:
				vPoint = EtVector3( Max.x, Min.y, Min.z );
				break;
			case 2:
				vPoint = EtVector3( Min.x, Max.y, Min.z );
				break;
			case 3:
				vPoint = EtVector3( Max.x, Max.y, Min.z );
				break;
			case 4:
				vPoint = EtVector3( Min.x, Min.y, Max.z );
				break;
			case 5:
				vPoint = EtVector3( Max.x, Min.y, Max.z );
				break;
			case 6:
				vPoint = EtVector3( Min.x, Max.y, Max.z );
				break;
			case 7:
				vPoint = EtVector3( Max.x, Max.y, Max.z );
				break;
		}
	}

	void GetVertices( DNVector(EtVector3) &vVecResult )
	{
		vVecResult.push_back( EtVector3( Min.x, Min.y, Min.z ) );
		vVecResult.push_back( EtVector3( Max.x, Min.y, Min.z ) );
		vVecResult.push_back( EtVector3( Min.x, Max.y, Min.z ) );
		vVecResult.push_back( EtVector3( Max.x, Max.y, Min.z ) );
		vVecResult.push_back( EtVector3( Min.x, Min.y, Max.z ) );
		vVecResult.push_back( EtVector3( Max.x, Min.y, Max.z ) );
		vVecResult.push_back( EtVector3( Min.x, Max.y, Max.z ) );
		vVecResult.push_back( EtVector3( Max.x, Max.y, Max.z ) );
	}

	void GetVertices( EtVector3 (&vVecResult)[ 8 ] )
	{
		vVecResult[0] =( EtVector3( Min.x, Min.y, Min.z ) );
		vVecResult[1] = ( EtVector3( Max.x, Min.y, Min.z ) );
		vVecResult[2] = ( EtVector3( Min.x, Max.y, Min.z ) );
		vVecResult[3] = ( EtVector3( Max.x, Max.y, Min.z ) );
		vVecResult[4] = ( EtVector3( Min.x, Min.y, Max.z ) );
		vVecResult[5] = ( EtVector3( Max.x, Min.y, Max.z ) );
		vVecResult[6] = ( EtVector3( Min.x, Max.y, Max.z ) );
		vVecResult[7] = ( EtVector3( Max.x, Max.y, Max.z ) );
	}
	void Reset()
	{
		Min = EtVector3( FLT_MAX, FLT_MAX, FLT_MAX );
		Max = EtVector3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	}
	void AddPoint( EtVector3 &Point )
	{
		Min.x = min( Point.x, Min.x );
		Max.x = max( Point.x, Max.x );
		Min.y = min( Point.y, Min.y );
		Max.y = max( Point.y, Max.y );
		Min.z = min( Point.z, Min.z );
		Max.z = max( Point.z, Max.z );
	}

	void GetPlanes( DNVector(EtVector4) &vVecResult)
	{
		// -y plane
		vVecResult.push_back( EtVector4( 0.0f, -1.0f, 0.0f, fabs( Min.y ) ) );
		// +y plane
		vVecResult.push_back( EtVector4( 0.0f, 1.0f, 0.0f, fabs( Max.y ) ) );
		// -x plane
		vVecResult.push_back( EtVector4( -1.0f, 0.0f, 0.0f, fabs( Min.x ) ) );
		// +x plane
		vVecResult.push_back( EtVector4( 1.0f, 0.0f, 0.0f, fabs( Max.x ) ) );
		// -z plane
		vVecResult.push_back( EtVector4( 0.0f, 0.0f, -1.0f, fabs( Min.z ) ) );
		// +z plane
		vVecResult.push_back( EtVector4( 0.0f, 0.0f, 1.0f, fabs( Max.z ) ) );
	}
	void Transform( EtMatrix &TransformMat )
	{
		int i;
		DNVector(EtVector3) Vertices;
		GetVertices( Vertices );

		Reset();	
		for( i = 0; i < ( int )Vertices.size(); i++ )
		{
			EtVec3TransformCoord( &Vertices[ i ], &Vertices[ i ], &TransformMat );
			AddPoint( Vertices[ i ] );
		}
	}
	bool IsInside( SSphere &Sphere )
	{
		if( Max.x < Sphere.Center.x + Sphere.fRadius )
		{
			return false;
		}
		if( Max.y < Sphere.Center.y + Sphere.fRadius )
		{
			return false;
		}
		if( Max.z < Sphere.Center.z + Sphere.fRadius )
		{
			return false;
		}
		if( Min.x > Sphere.Center.x - Sphere.fRadius )
		{
			return false;
		}
		if( Min.y > Sphere.Center.y - Sphere.fRadius )
		{
			return false;
		}
		if( Min.z > Sphere.Center.z - Sphere.fRadius )
		{
			return false;
		}

		return true;
	}
	bool IsInside( SAABox &Box )
	{
		if( Max.x < Box.Max.x )
		{
			return false;
		}
		if( Max.y < Box.Max.y )
		{
			return false;
		}
		if( Max.z < Box.Max.z )
		{
			return false;
		}
		if( Min.x > Box.Min.x )
		{
			return false;
		}
		if( Min.y > Box.Min.y )
		{
			return false;
		}
		if( Min.z > Box.Min.z )
		{
			return false;
		}

		return true;
	}
	bool IsInside( EtVector3 &Point )
	{
		if( Max.x < Point.x )
		{
			return false;
		}
		if( Max.y < Point.y )
		{
			return false;
		}
		if( Max.z < Point.z )
		{
			return false;
		}
		if( Min.x > Point.x )
		{
			return false;
		}
		if( Min.y > Point.y )
		{
			return false;
		}
		if( Min.z > Point.z )
		{
			return false;
		}

		return true;
	}
	bool IsValid()
	{
		return (Min.x <= Max.x) && (Min.y <= Max.y) && (Min.z <= Max.z);
	}

	EtVector3 GetExtent() 
	{
		return (Max-Min);
	}

	EtVector3 GetCenter() 
	{
		return (Max+Min)*0.5f;
	}

	EtVector3 Min;
	EtVector3 Max;
};

struct SOBB
{
	void Init( SAABox &Box, EtMatrix &WorldMat );
	void CalcVertices()
	{
		EtVector3 TempAxis[ 3 ] = { Extent[ 0 ] * Axis[0], Extent[ 1 ] * Axis[ 1 ], Extent[ 2 ] * Axis[ 2 ] };

		Vertices[ 0 ] = Center - TempAxis[ 0 ] - TempAxis[ 1 ] - TempAxis[ 2 ];
		Vertices[ 1 ] = Center + TempAxis[ 0 ] - TempAxis[ 1 ] - TempAxis[ 2 ];
		Vertices[ 2 ] = Center - TempAxis[ 0 ] + TempAxis[ 1 ] - TempAxis[ 2 ];
		Vertices[ 3 ] = Center + TempAxis[ 0 ] + TempAxis[ 1 ] - TempAxis[ 2 ];
		Vertices[ 4 ] = Center - TempAxis[ 0 ] - TempAxis[ 1 ] + TempAxis[ 2 ];
		Vertices[ 5 ] = Center + TempAxis[ 0 ] - TempAxis[ 1 ] + TempAxis[ 2 ];
		Vertices[ 6 ] = Center - TempAxis[ 0 ] + TempAxis[ 1 ] + TempAxis[ 2 ];
		Vertices[ 7 ] = Center + TempAxis[ 0 ] + TempAxis[ 1 ] + TempAxis[ 2 ];
	}

	void GetPlanes( DNVector(EtVector4) &vVecResult)
	{
		// -y plane
		vVecResult.push_back( EtVector4( -Axis[ 1 ].x, -Axis[ 1 ].y, -Axis[ 1 ].z, -EtVec3Dot( Axis + 1, Vertices ) ) );
		// +y plane
		vVecResult.push_back( EtVector4( Axis[ 1 ].x, Axis[ 1 ].y, Axis[ 1 ].z, EtVec3Dot( Axis + 1, Vertices + 2 ) ) );
		// -x plane
		vVecResult.push_back( EtVector4( -Axis[ 0 ].x, -Axis[ 0 ].y, -Axis[ 0 ].z, -EtVec3Dot( Axis, Vertices + 2 ) ) );
		// +x plane
		vVecResult.push_back( EtVector4( Axis[ 0 ].x, Axis[ 0 ].y, Axis[ 0 ].z, EtVec3Dot( Axis, Vertices + 1 ) ) );
		// -z plane
		vVecResult.push_back( EtVector4( -Axis[ 2 ].x, -Axis[ 2 ].y, -Axis[ 2 ].z, -EtVec3Dot( Axis + 2, Vertices ) ) );
		// +z plane
		vVecResult.push_back( EtVector4( Axis[ 2 ].x, Axis[ 2 ].y, Axis[ 2 ].z, EtVec3Dot( Axis + 2, Vertices + 4 ) ) );
	}
	void GetDiagonal( EtVector3 &Min, EtVector3 &Max, EtVector3 &Normal )
	{
		int	nIndex;

		nIndex = 0;
		if( EtVec3Dot( &Normal, Axis ) > 0.f )
		{
			nIndex ^= 1;
		}
		if( EtVec3Dot( &Normal, Axis + 1 ) > 0.f )
		{
			nIndex ^= 2;
		}
		if( EtVec3Dot( &Normal, Axis + 2 ) > 0.f )
		{
			nIndex ^= 4;
		}
		Max = Vertices[ nIndex ];
		Min = Vertices[ 7 - nIndex ];
	}
	bool IsInside( EtVector3 &vPoint )
	{
		EtVector3 vVertex = vPoint - Center;

		float fDot = fabs( EtVec3Dot( &Axis[0], &vVertex ) );
		if( fDot > Extent[0] ) return false;

		fDot = fabs( EtVec3Dot( &Axis[1], &vVertex ) );
		if( fDot > Extent[1] ) return false;

		fDot = fabs( EtVec3Dot( &Axis[2], &vVertex ) );
		if( fDot > Extent[2] ) return false;
		return true;
	}

	EtVector3 Center;
	EtVector3 Axis[ 3 ];
	float Extent[ 3 ];
	EtVector3 Vertices[ 8 ];

};

inline bool SSphere::IsInside( SAABox &Box )
{
	int i;
	DNVector(EtVector3) vecVertex;
	Box.GetVertices( vecVertex );

	for( i = 0; i < ( int )vecVertex.size(); i++ )
	{
		if( !IsInside( vecVertex[ i ] ) )
		{
			return false;
		}
	}
	return true;		
}
