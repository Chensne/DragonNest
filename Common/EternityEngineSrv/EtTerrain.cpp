#include "StdAfx.h"
#include "EtTerrain.h"
#include "EtCollisionFunc.h"
#include "EtLoader.h"

using namespace EternityEngine;

CEtTerrain::CEtTerrain(void)
{
	m_BoundingBox.Reset();
	m_nBlockOffsetX = 0;
	m_nBlockOffsetY = 0;
}

CEtTerrain::~CEtTerrain(void)
{
}

void CEtTerrain::Initialize()
{
	CalcBoundingBoxHeight();
	CalcBoundingBox();
}

EtVector3 CEtTerrain::GetVertexNormal( int nIndexX, int nIndexY )
{
	EtVector3 Return;

	Return.x = ( GetHeight( nIndexX - 1, nIndexY ) - GetHeight( nIndexX + 1, nIndexY ) ) / ( m_TerrainInfo.fTileSize * 2 );
	Return.y = 1.414f * m_TerrainInfo.fHeightMultiply;
	Return.z = ( GetHeight( nIndexX, nIndexY - 1 ) - GetHeight( nIndexX, nIndexY + 1 ) ) / ( m_TerrainInfo.fTileSize * 2 );
	EtVec3Normalize( &Return, &Return );

	return Return;
}

void CEtTerrain::GetExtent( EtVector3 &Origin, EtVector3 &Extent )
{
	Origin = ( m_BoundingBox.Max + m_BoundingBox.Min ) / 2;
	Extent = ( m_BoundingBox.Max - m_BoundingBox.Min ) / 2;
}

void CEtTerrain::CalcBoundingBoxHeight()
{
	int i, j;
	int nSizeX, nSizeY;

	nSizeX = m_TerrainInfo.nSizeX + 1;
	nSizeY = m_TerrainInfo.nSizeY + 1;
	float fValue;
	for( i = 0; i < nSizeY; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			fValue = m_TerrainInfo.pHeight[ i * m_nStride + j ] * m_TerrainInfo.fHeightMultiply;
			if( fValue > m_BoundingBox.Max.y )
			{
				m_BoundingBox.Max.y = fValue;
			}
			if( fValue < m_BoundingBox.Min.y )
			{
				m_BoundingBox.Min.y = fValue;
			}
		}
	}
}

void CEtTerrain::CalcBoundingBox()
{
	m_BoundingBox.Max.x = m_TerrainInfo.TerrainOffset.x + m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize;
	m_BoundingBox.Max.z = m_TerrainInfo.TerrainOffset.z + m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize;

	m_BoundingBox.Min.x = m_TerrainInfo.TerrainOffset.x;
	m_BoundingBox.Min.z = m_TerrainInfo.TerrainOffset.z;

	m_BoundingBox.Max.y += 1.0f;
	m_BoundingBox.Min.y -= 1.0f;
}

float CEtTerrain::Pick( EtVector3 &Origin, EtVector3 &Direction, EtVector3 &PickPos )
{
	int nTileX, nTileY, nDirX, nDirY;
	EtVector2 Origin2D, Direction2D;
	float fTileSize, fDistX, fDistY, fRet;

	fTileSize = m_TerrainInfo.fTileSize;
	if( Direction.x >= 0.0f )
	{
		nDirX = 1;
		Direction2D.x = Direction.x;
		Origin2D.x = Origin.x - m_TerrainInfo.TerrainOffset.x;
	}
	else
	{
		nDirX = -1;
		Direction2D.x = -Direction.x;
		Origin2D.x = m_TerrainInfo.nSizeX * fTileSize - ( Origin.x - m_TerrainInfo.TerrainOffset.x );
	}
	if( Direction.z >= 0.0f )
	{
		nDirY = 1;
		Direction2D.y = Direction.z;
		Origin2D.y = Origin.z - m_TerrainInfo.TerrainOffset.z;
	}
	else
	{
		nDirY = -1;
		Direction2D.y = -Direction.z;
		Origin2D.y = m_TerrainInfo.nSizeY * fTileSize - ( Origin.z - m_TerrainInfo.TerrainOffset.z );
	}

	while( 1 )
	{
		nTileX = ( int )( Origin2D.x / fTileSize );
		if( nDirX < 0 )
		{
			nTileX = m_TerrainInfo.nSizeX - nTileX - 1;
		}
		nTileY = ( int )( Origin2D.y / fTileSize );
		if( nDirY < 0 )
		{
			nTileY = m_TerrainInfo.nSizeY - nTileY - 1;
		}
		if( !IsInTerrain( nTileX, nTileY ) )
		{
			break;
		}
		fRet = TestLintToTile( Origin, Direction, nTileX, nTileY );
		if( ( fRet != FLT_MAX ) && ( fRet >= 0.0f ) )
		{
			PickPos = Origin + Direction * fRet;
	 		return fRet;
		}

		fDistX = ( ( ( int )( Origin2D.x / fTileSize ) + 1 ) * fTileSize - Origin2D.x ) / Direction2D.x;
		fDistY = ( ( ( int )( Origin2D.y / fTileSize ) + 1 ) * fTileSize - Origin2D.y ) / Direction2D.y;
		if( fDistX >= fDistY )
		{
			Origin2D += fDistY * Direction2D;
		}
		else if( fDistX < fDistY )
		{
			Origin2D += fDistX * Direction2D;
		}
	}

	return FLT_MAX;
}

bool CEtTerrain::IsInTerrain( int nX, int nY )
{
	if( ( nX < 0 ) || ( nX >= m_TerrainInfo.nSizeX ) )
	{
		return false;
	}
	if( ( nY < 0 ) || ( nY >= m_TerrainInfo.nSizeY ) )
	{
		return false;
	}

	return true;
}

float CEtTerrain::TestLintToTile( EtVector3 &Origin, EtVector3 &Direction, int nTileX, int nTileY )
{
	float fTileSize;
	float fDist1, fDist2, fBary1, fBary2;
	EtVector3 V1, V2, V3, ExtraOrigin;

	fTileSize = m_TerrainInfo.fTileSize;

	ExtraOrigin = Origin - m_TerrainInfo.TerrainOffset;

	V1 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	nTileY++;
	V2 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	nTileX++;
	V3 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	TestLineToTriangle( ExtraOrigin, Direction, V1, V2, V3, fDist1, fBary1, fBary2 );

	nTileY--;
	V2 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	TestLineToTriangle( ExtraOrigin, Direction, V1, V3, V2, fDist2, fBary1, fBary2 );

	return min( fDist1, fDist2 );
}

