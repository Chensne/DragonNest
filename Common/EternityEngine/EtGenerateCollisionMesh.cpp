#include "Stdafx.h"
#include "EtGenerateCollisionMesh.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void GenerateBoxCollisionMesh( CEtMeshStream &MeshStream, SCollisionBox &Box, DWORD dwColor )
{
	float Extent[ 3 ];
	int i, nVertexCount, nFaceCount;
	EtVector3 Points[ 8 ], Center;
	WORD Faces[ 12 * 3 ];
	DWORD aColor[ 8 ];

	memcpy( Extent, Box.fExtent, sizeof( float ) * 3 );
	Center = Box.vCenter;
	nVertexCount = 8;
	nFaceCount = 12;

	Points[ 0 ] = Center + Box.vAxis[ 0 ] * Extent[ 0 ] - Box.vAxis[ 1 ] * Extent[ 1 ] - Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 1 ] = Center + Box.vAxis[ 0 ] * Extent[ 0 ] + Box.vAxis[ 1 ] * Extent[ 1 ] - Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 2 ] = Center - Box.vAxis[ 0 ] * Extent[ 0 ] + Box.vAxis[ 1 ] * Extent[ 1 ] - Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 3 ] = Center - Box.vAxis[ 0 ] * Extent[ 0 ] - Box.vAxis[ 1 ] * Extent[ 1 ] - Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 4 ] = Center + Box.vAxis[ 0 ] * Extent[ 0 ] - Box.vAxis[ 1 ] * Extent[ 1 ] + Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 5 ] = Center + Box.vAxis[ 0 ] * Extent[ 0 ] + Box.vAxis[ 1 ] * Extent[ 1 ] + Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 6 ] = Center - Box.vAxis[ 0 ] * Extent[ 0 ] + Box.vAxis[ 1 ] * Extent[ 1 ] + Box.vAxis[ 2 ] * Extent[ 2 ];
	Points[ 7 ] = Center - Box.vAxis[ 0 ] * Extent[ 0 ] - Box.vAxis[ 1 ] * Extent[ 1 ] + Box.vAxis[ 2 ] * Extent[ 2 ];
	for( i = 0; i < nVertexCount; i++ )
	{
		aColor[ i ] = dwColor;
	}

	Faces[ 0 ]  = 3;
	Faces[ 1 ]  = 2;  
	Faces[ 2 ]  = 1;
	Faces[ 3 ]  = 3;  
	Faces[ 4 ]  = 1;  
	Faces[ 5 ]  = 0;
	Faces[ 6 ]  = 0;  
	Faces[ 7 ]  = 1;  
	Faces[ 8 ]  = 5;
	Faces[ 9 ]  = 0;  
	Faces[ 10 ] = 5;  
	Faces[ 11 ] = 4;
	Faces[ 12 ] = 4;  
	Faces[ 13 ] = 5;  
	Faces[ 14 ] = 6;
	Faces[ 15 ] = 4;  
	Faces[ 16 ] = 6;  
	Faces[ 17 ] = 7;
	Faces[ 18 ] = 7;  
	Faces[ 19 ] = 6;  
	Faces[ 20 ] = 2;
	Faces[ 21 ] = 7;  
	Faces[ 22 ] = 2;  
	Faces[ 23 ] = 3;
	Faces[ 24 ] = 1;  
	Faces[ 25 ] = 2;  
	Faces[ 26 ] = 6;
	Faces[ 27 ] = 1;  
	Faces[ 28 ] = 6;  
	Faces[ 29 ] = 5;
	Faces[ 30 ] = 0;  
	Faces[ 31 ] = 4;  
	Faces[ 32 ] = 7;
	Faces[ 33 ] = 0;  
	Faces[ 34 ] = 7;
	Faces[ 35 ] = 3;

	CMemoryStream Stream;

	Stream.Initialize( Points, sizeof( EtVector3 ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( aColor, sizeof( DWORD ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_COLOR, 0, nVertexCount );
	Stream.Initialize( Faces, sizeof( WORD ) * nFaceCount * 3 );
	MeshStream.LoadIndexStream( &Stream, false, nFaceCount * 3 );
}

void GenerateCapsuleCollisionMesh( CEtMeshStream &MeshStream, SCollisionCapsule &Capsule, DWORD dwColor )
{
	float fRadius;
	EtVector3 Origin, Direction;

	fRadius = Capsule.fRadius;
	Origin = Capsule.Segment.vOrigin;
	Direction = Capsule.Segment.vDirection;
	if( EtVec3LengthSq( &Direction ) < 0.0f )
	{
		SCollisionSphere Sphere;

		Sphere.vCenter = Capsule.Segment.vOrigin;
		Sphere.fRadius = fRadius;
		GenerateSphereCollisionMesh( MeshStream, Sphere );

		return;
	}

	EtVector3 Axis, StartPoint, EndPoint, UpVec, XVec;
	
	EtVec3Normalize( &Axis, &Capsule.Segment.vDirection );
	StartPoint = Origin;
	EndPoint = Origin + Direction;

	if( fabs( Axis.x ) >= fabs( Axis.y ) && fabs( Axis.x ) >= fabs( Axis.z ) )
	{
		UpVec.x = -Axis.y;
		UpVec.y = Axis.x;
		UpVec.z = Axis.z;
	}
	else
	{
		UpVec.x = Axis.x;
		UpVec.y = Axis.z;
		UpVec.z = -Axis.y;
	}
	EtVec3Cross( &XVec, &UpVec, &Axis );
	EtVec3Normalize( &XVec, &XVec );
	XVec *= fRadius;
	UpVec *= fRadius;

	int i, j, nSides, nVertexCount, nFaceCount;
	EtVector3 Points[ 16 * 4 + 2 ], CapStart, CapEnd;
	DWORD aColor[ 16 * 4 + 2 ];
	WORD Faces[ 16 * 8 * 3 ], *pTemFaces;
	float fCapScale, fInc, fTheta;

	nSides = 16;
	nVertexCount = 4 * nSides + 2;
	nFaceCount = 8 * nSides;

	Points[ nVertexCount - 2 ] = StartPoint - Axis * fRadius;
	Points[ nVertexCount - 1 ] = EndPoint + Axis * fRadius;

	fCapScale = sqrtf( 0.5f );
	CapStart = StartPoint - Axis * (fRadius * fCapScale);
	CapEnd = EndPoint + Axis * (fRadius * fCapScale);

	fInc = 2.0f * ET_PI / ( float )nSides;
	fTheta = 0.0f;
	for ( i = 0; i < nSides; i++ )
	{
		EtVector3 kOffset;

		kOffset = XVec * cos( fTheta ) + UpVec * sin( fTheta );
		Points[ i ] = CapStart + kOffset * fCapScale;
		Points[ i + nSides ] = StartPoint + kOffset;
		Points[ i + nSides * 2 ] = EndPoint + kOffset;
		Points[ i + nSides * 3 ] = CapEnd + kOffset * fCapScale;
		fTheta += fInc;
	}
	for( i = 0; i < nVertexCount; i++ )
	{
		aColor[ i ] = dwColor;
	}

	pTemFaces = Faces;
	for ( i = 0; i < nSides - 1; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			*pTemFaces++ = i + nSides * j;
			*pTemFaces++ = i + 1 + nSides * j;
			*pTemFaces++ = i + 1 + nSides * ( j + 1 );

			*pTemFaces++ = i + nSides * j;
			*pTemFaces++ = i + 1 + nSides * ( j + 1 );
			*pTemFaces++ = i + nSides * ( j + 1 );
		}
		*pTemFaces++ = i + 1;
		*pTemFaces++ = i;
		*pTemFaces++ = nVertexCount - 2;

		*pTemFaces++ = i + nSides * 3;
		*pTemFaces++ = i + 1 + nSides * 3;
		*pTemFaces++ = nVertexCount - 1;
	}

	for (j = 0; j < 3; j++)
	{
		*pTemFaces++ = nSides * ( j + 1 ) - 1;
		*pTemFaces++ = nSides * j;
		*pTemFaces++ = nSides * ( j + 1 );

		*pTemFaces++ = nSides * ( j + 1 ) - 1;
		*pTemFaces++ = nSides * ( j + 1 );
		*pTemFaces++ = nSides * ( j + 2 ) - 1;
	}

	*pTemFaces++ = 0;
	*pTemFaces++ = nSides - 1;
	*pTemFaces++ = nVertexCount - 2;

	*pTemFaces++ = nSides * 4 - 1;
	*pTemFaces++ = nSides * 3;
	*pTemFaces++ = nVertexCount - 1;

	CMemoryStream Stream;

	Stream.Initialize( Points, sizeof( EtVector3 ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( aColor, sizeof( DWORD ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_COLOR, 0, nVertexCount );
	Stream.Initialize( Faces, sizeof( WORD ) * nFaceCount * 3 );
	MeshStream.LoadIndexStream( &Stream, false, nFaceCount * 3 );
}

void GenerateSphereCollisionMesh( CEtMeshStream &MeshStream, SCollisionSphere &Sphere, DWORD dwColor )
{
	float fGold;
	EtVector3 Center;

	fGold = 0.5f * ( sqrtf( 5.0f ) + 1.0f );
	Center = Sphere.vCenter;

	EtVector3 BasePoints[ 12 ] =
	{
		EtVector3( fGold, 1.0f, 0.0f ), 
		EtVector3( -fGold, 1.0f, 0.0f ), 
		EtVector3( fGold, -1.0f, 0.0f ), 
		EtVector3( -fGold, -1.0f, 0.0f ), 
		EtVector3( 1.0f, 0.0f, fGold ), 
		EtVector3( 1.0f, 0.0f, -fGold ), 
		EtVector3( -1.0f, 0.0f, fGold ), 
		EtVector3( -1.0f, 0.0f, -fGold ), 
		EtVector3( 0.0f, fGold, 1.0f ), 
		EtVector3( 0.0f, -fGold, 1.0f ), 
		EtVector3( 0.0f, fGold, -1.0f ), 
		EtVector3( 0.0f, -fGold, -1.0f )
	};

	WORD BaseFaces[] =
	{
		0, 8, 4, 
		0, 5, 10, 
		2, 4, 9, 
		2, 11, 5, 
		1, 6, 8, 
		1, 10, 7, 
		3, 9, 6, 
		3, 7, 11, 
		0, 10, 8, 
		1, 8, 10, 
		2, 9, 11, 
		3, 11, 9, 
		4, 2, 0, 
		5, 0, 2, 
		6, 1, 3, 
		7, 3, 1, 
		8, 6, 4, 
		9, 4, 6, 
		10, 5, 7, 
		11, 7, 5
	};

	int i, nBaseVertexCount, nBaseFaceCount, nVertexCount, nFaceCount;
	EtVector3 Points[ 72 ];
	DWORD aColor[ 72 ];
	WORD Faces[ 80 * 3 ], TempFaces[ 80 * 3 ];

	nBaseVertexCount = 12;
	nBaseFaceCount = 20;
	nVertexCount = 72;
	nFaceCount = 80;

	for( i = 0; i < nBaseVertexCount; i++ )
	{
		EtVec3Normalize( Points + i, BasePoints + i );
	}
	memcpy( Faces, BaseFaces, sizeof( WORD ) * nBaseFaceCount * 3 );

	WORD *pFaceSour, *pFaceDest;

	pFaceSour = Faces;
	pFaceDest = TempFaces;

	for ( i = 0; i < nBaseFaceCount; i++ )
	{
		WORD nVertex1, nVertex2, nVertex3;

		nVertex1 = *pFaceSour++;
		nVertex2 = *pFaceSour++;
		nVertex3 = *pFaceSour++;

		Points[ nBaseVertexCount ] = Points[ nVertex1 ] + Points[ nVertex2 ];
		EtVec3Normalize( Points + nBaseVertexCount, Points + nBaseVertexCount );
		Points[ nBaseVertexCount + 1 ] = Points[nVertex2] + Points[nVertex3];
		EtVec3Normalize( Points + nBaseVertexCount + 1, Points + nBaseVertexCount + 1 );
		Points[ nBaseVertexCount + 2 ] = Points[nVertex3] + Points[nVertex1];
		EtVec3Normalize( Points + nBaseVertexCount + 2, Points + nBaseVertexCount + 2 );

		*pFaceDest++ = nVertex1;
		*pFaceDest++ = nBaseVertexCount;
		*pFaceDest++ = nBaseVertexCount + 2;

		*pFaceDest++ = nVertex2;
		*pFaceDest++ = nBaseVertexCount + 1;
		*pFaceDest++ = nBaseVertexCount;

		*pFaceDest++ = nVertex3;
		*pFaceDest++ = nBaseVertexCount + 2;
		*pFaceDest++ = nBaseVertexCount + 1;

		*pFaceDest++ = nBaseVertexCount;
		*pFaceDest++ = nBaseVertexCount + 1;
		*pFaceDest++ = nBaseVertexCount + 2;

		nBaseVertexCount += 3;
	}
	nBaseFaceCount *= 4;

	for ( i = 0; i < nBaseVertexCount; i++ )
	{
		Points[ i ] = Points[ i ] * Sphere.fRadius + Center;
	}
	for( i = 0; i < nVertexCount; i++ )
	{
		aColor[ i ] = dwColor;
	}

	CMemoryStream Stream;

	Stream.Initialize( Points, sizeof( EtVector3 ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( aColor, sizeof( DWORD ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_COLOR, 0, nVertexCount );
	Stream.Initialize( TempFaces, sizeof( WORD ) * nFaceCount * 3 );
	MeshStream.LoadIndexStream( &Stream, false, nFaceCount * 3 );
}

void GenerateTriangleListCollisionMesh( CEtMeshStream &MeshStream, SCollisionTriangleList &TriangleList, EtMatrix &WorldMat, DWORD dwColor )
{
	int i, nVertexCount;
	EtVector3 *pPoint;
	DWORD *pColor;
	WORD *pIndex;

	nVertexCount = ( int )TriangleList.vecTriangle.size() * 3;
	if( nVertexCount <= 0 )
	{
		return;
	}
	pPoint = new EtVector3[ nVertexCount ];
	pColor = new DWORD[ nVertexCount ];
	pIndex = new WORD[ nVertexCount ];

	for( i = 0; i < ( int )TriangleList.vecTriangle.size(); i++ )
	{
		pPoint[ i * 3 ] = TriangleList.vecTriangle[ i ].vOrigin;
		pColor[ i * 3 ] = dwColor;
		EtVec3TransformCoord( pPoint+ i * 3, pPoint+ i * 3, &WorldMat );
		pPoint[ i * 3 + 1 ] = TriangleList.vecTriangle[ i ].vOrigin + TriangleList.vecTriangle[ i ].vEdge1;
		pColor[ i * 3 + 1 ] = dwColor;
		EtVec3TransformCoord( pPoint+ i * 3 + 1, pPoint+ i * 3 + 1, &WorldMat );
		pPoint[ i * 3 + 2 ] = TriangleList.vecTriangle[ i ].vOrigin + TriangleList.vecTriangle[ i ].vEdge2;
		pColor[ i * 3 + 2 ] = dwColor;
		EtVec3TransformCoord( pPoint+ i * 3 + 2, pPoint+ i * 3 + 2, &WorldMat );

		pIndex[ i * 3 ] = i * 3;
		pIndex[ i * 3 + 1 ] = i * 3 + 1;
		pIndex[ i * 3 + 2 ] = i * 3 + 2;
	}

	CMemoryStream Stream;

	Stream.Initialize( pPoint, sizeof( EtVector3 ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( pColor, sizeof( DWORD ) * nVertexCount );
	MeshStream.LoadVertexStream( &Stream, MST_COLOR, 0, nVertexCount );
	Stream.Initialize( pIndex, sizeof( WORD ) * nVertexCount );
	MeshStream.LoadIndexStream( &Stream, false, nVertexCount );

	delete [] pPoint;
	delete [] pColor;
	delete [] pIndex;
}
