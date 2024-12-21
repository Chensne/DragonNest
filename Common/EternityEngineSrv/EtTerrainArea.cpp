#include "StdAfx.h"
#include "EtConvexVolume.h"
#include "EtCollisionFunc.h"
#include "EtLoader.h"
#include "EtTerrainArea.h"

using namespace EternityEngine;

DECL_MULTISMART_PTR_STATIC( CEtTerrainArea, MAX_SESSION_COUNT, 20 )

CEtTerrainArea::CEtTerrainArea( CMultiRoom *pRoom )
: CMultiSmartPtrBase< CEtTerrainArea, MAX_SESSION_COUNT >(pRoom)
{
	m_BoundingBox.Reset();
	m_nBlockCountX = 0;
	m_nBlockCountY = 0;
	m_nBlockSizeX = 0;
	m_nBlockSizeY = 0;
}

CEtTerrainArea::~CEtTerrainArea(void)
{
	Clear();
}

void CEtTerrainArea::Clear()
{
	SAFE_DELETE_PVEC( m_vecTerrain );
}

void CEtTerrainArea::CalcSelectCount( int nBlockIndex, int &nStart, int &nEnd )
{
	if( nBlockIndex == -1 )
	{
		nStart = 0;
		nEnd = ( int )m_vecTerrain.size();
	}
	else
	{
		nStart = nBlockIndex;
		nEnd = nStart + 1;
	}
}

void CEtTerrainArea::Initialize( STerrainInfo *pInfo )
{
	CEtTerrain *pTerrain;
	int i, j;

	Clear();

	m_TerrainInfo = *pInfo;

	m_nBlockCountX = pInfo->nSizeX / DEFAULT_TERRAIN_SIZE;
	if( pInfo->nSizeX % DEFAULT_TERRAIN_SIZE )
	{
		m_nBlockCountX++;
	}
	m_nBlockSizeX = DEFAULT_TERRAIN_SIZE;

	m_nBlockCountY = pInfo->nSizeY / DEFAULT_TERRAIN_SIZE;
	if( pInfo->nSizeY % DEFAULT_TERRAIN_SIZE )
	{
		m_nBlockCountY++;
	}
	m_nBlockSizeY = DEFAULT_TERRAIN_SIZE;

	STerrainInfo TerrainInfo;

	memcpy( &TerrainInfo, pInfo, sizeof( STerrainInfo ) );
	TerrainInfo.nSizeX = m_nBlockSizeX;
	TerrainInfo.nSizeY = m_nBlockSizeY;
	for( i = 0; i < m_nBlockCountY; i++ )
	{
		for( j = 0; j < m_nBlockCountX; j++ )
		{
			TerrainInfo.pHeight = pInfo->pHeight + ( i * m_nBlockSizeY ) * ( pInfo->nSizeX + 1 ) + j * m_nBlockSizeX;
			TerrainInfo.pLayerDensity = pInfo->pLayerDensity + ( i * m_nBlockSizeY ) * ( pInfo->nSizeX + 1 ) + j * m_nBlockSizeX;

			TerrainInfo.TerrainOffset = pInfo->TerrainOffset 
				+ EtVector3( pInfo->fTileSize * j * m_nBlockSizeX, 0.0f, pInfo->fTileSize * i * m_nBlockSizeY );
			pTerrain = new CEtTerrain();
			pTerrain->SetTerrainInfo( &TerrainInfo );
			pTerrain->SetBlockOffset( j * m_nBlockSizeX, i * m_nBlockSizeY );
			pTerrain->SetStride( pInfo->nSizeX + 1, pInfo->nSizeY + 1 );
			m_vecTerrain.push_back( pTerrain );
		}
	}

	float fWorldSizeX, fWorldSizeY, fWorldSizeZ;
	fWorldSizeX = m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize;
	fWorldSizeY = m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize;
	fWorldSizeZ = pInfo->fHeightMultiply * 65535.f;
	CEtObject::SetWorldSize( GetRoom(), EtVector3( fWorldSizeX * 0.5f, 0.0f, fWorldSizeY * 0.5f ), max( max( fWorldSizeX, fWorldSizeY ), fWorldSizeZ ) );

	CalcBoundingBox();
}

void CEtTerrainArea::InitializeBlock( int nBlockIndex )
{
	int i, nStart, nEnd;

	CalcSelectCount( nBlockIndex, nStart, nEnd );
	for( i = nStart; i < nEnd; i++ )
	{
		m_vecTerrain[ i ]->Initialize();
	}
	CalcBoundingBox();
}

void CEtTerrainArea::ChangeBlockType( TerrainType Type, int nBlockIndex )
{
}

TerrainType CEtTerrainArea::GetBlockType( int nBlockIndex )
{
	return TT_NORMAL;
}

void CEtTerrainArea::GetExtent( EtVector3 &Origin, EtVector3 &Extent )
{
	Origin = ( m_BoundingBox.Max + m_BoundingBox.Min ) / 2;
	Extent = ( m_BoundingBox.Max - m_BoundingBox.Min ) / 2;
}

void CEtTerrainArea::CalcBoundingBox()
{
	int i;
	SAABox *pBoundingBox;

	for( i = 0; i < ( int )m_vecTerrain.size(); i++ )
	{
		pBoundingBox = m_vecTerrain[ i ]->GetBoundingBox();
		if( m_BoundingBox.Min.y > pBoundingBox->Min.y )
		{
			m_BoundingBox.Min.y = pBoundingBox->Min.y;
		}
		if( m_BoundingBox.Max.y < pBoundingBox->Max.y )
		{
			m_BoundingBox.Max.y = pBoundingBox->Max.y;
		}
	}
	m_BoundingBox.Max.x = m_TerrainInfo.TerrainOffset.x + m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize;
	m_BoundingBox.Max.z = m_TerrainInfo.TerrainOffset.z + m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize;

	m_BoundingBox.Min.x = m_TerrainInfo.TerrainOffset.x;
	m_BoundingBox.Min.z = m_TerrainInfo.TerrainOffset.z;
}

bool CEtTerrainArea::Pick( EtVector3 &Origin, EtVector3 &Direction, EtVector3 &PickPos )
{
	int i;
	float fMinDist, fFindDist, fDistToBox;
	EtVector3 FindPos, ModifyOrigin;

	fMinDist = FLT_MAX;
	for( i = 0; i < ( int )m_vecTerrain.size(); i++ )
	{
		SAABox *pBoundingBox;

		pBoundingBox = m_vecTerrain[ i ]->GetBoundingBox();
		if( !TestLineToBox( Origin, Direction, *pBoundingBox, fDistToBox ) )
		{
			continue;
		}
		if( fDistToBox == 0.0f )
		{
			ModifyOrigin = Origin;
		}
		else
		{
			ModifyOrigin = Origin + Direction * fDistToBox;
		}
		fFindDist = m_vecTerrain[ i ]->Pick( ModifyOrigin, Direction, FindPos ) + fDistToBox;
		if(fFindDist < fMinDist )
		{
			fMinDist = fFindDist;
			PickPos = FindPos;
		}
	}

	if( fMinDist != FLT_MAX )
	{
		return true;
	}

	return false;
}

bool CEtTerrainArea::IsInside( float fX, float fZ )
{
	if( fX < m_TerrainInfo.TerrainOffset.x )
	{
		return false;
	}
	if( fX > m_TerrainInfo.TerrainOffset.x + m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize )
	{
		return false;
	}
	if( fZ < m_TerrainInfo.TerrainOffset.z )
	{
		return false;
	}
	if( fZ > m_TerrainInfo.TerrainOffset.z + m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize )
	{
		return false;
	}

	return true;
}

void CEtTerrainArea::CalcCellPosition( float fX, float fZ, int &nCellX, int &nCellZ, float *pWeight )
{
	float fModX, fModZ;

	fX -= m_TerrainInfo.TerrainOffset.x;
	fZ -= m_TerrainInfo.TerrainOffset.z;
	if( fX < 0.0f )
	{
		fX = 0.f;
	}
	else if( fX >= m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize ) 
	{
		fX = m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize - 0.1f;
	}

	if( fZ < 0.f )
	{
		fZ = 0.f;
	}
	else if( fZ >= m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize )
	{
		fZ = m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize - 0.1f;
	}

	nCellX = ( int )( fX / m_TerrainInfo.fTileSize );
	nCellZ = ( int )( fZ / m_TerrainInfo.fTileSize );
	fModX = fX / m_TerrainInfo.fTileSize - nCellX;
	fModZ = fZ / m_TerrainInfo.fTileSize - nCellZ;

	if( fModX > fModZ ) 
	{
		pWeight[ 0 ] = 1.0f - fModX;
		pWeight[ 1 ] = fModX - fModZ;
		pWeight[ 2 ] = 0.0f;
		pWeight[ 3 ] = fModZ;
	}
	else if( fModX < fModZ ) 
	{
		pWeight[ 0 ] = 1.0f - fModZ;
		pWeight[ 1 ] = 0.0f;
		pWeight[ 2 ] = fModZ - fModX;
		pWeight[ 3 ] = fModX;
	}
	else
	{
		pWeight[ 0 ] = 1.0f - fModZ;
		pWeight[ 1 ] = 0.0f;
		pWeight[ 2 ] = 0.0f;
		pWeight[ 3 ] = fModZ;
	}
}

float CEtTerrainArea::GetLandHeight( float fX, float fZ, EtVector3 *pNormal )
{
	int i;
	float fRet;
	int nCellX, nCellZ;
	float fHeight[ 4 ], fWeight[ 4 ];
	DWORD dwSectorWidth = m_TerrainInfo.nSizeX + 1;

	CalcCellPosition( fX, fZ, nCellX, nCellZ, fWeight );
	fHeight[ 0 ] = m_TerrainInfo.pHeight[ nCellZ * dwSectorWidth + nCellX ];
	fHeight[ 1 ] = m_TerrainInfo.pHeight[ nCellZ * dwSectorWidth + nCellX + 1 ];
	fHeight[ 2 ] = m_TerrainInfo.pHeight[ ( nCellZ + 1 ) * dwSectorWidth + nCellX ];
	fHeight[ 3 ] = m_TerrainInfo.pHeight[ ( nCellZ + 1 ) * dwSectorWidth + nCellX + 1 ];
	fRet = 0.0f;
	for( i = 0; i < 4; i++ )
	{
		fRet += fHeight[ i ] * fWeight[ i ];
	}

	if( pNormal )
	{
		GetVertexNormal( *pNormal, nCellX, nCellZ );
	}

	return fRet * m_TerrainInfo.fHeightMultiply;
}

void CEtTerrainArea::GetLandNormal( EtVector3 &Normal, float fX, float fZ )
{
	int i;
	EtVector3 ReturnVec, VertexNormal[ 4 ];
	int nCellX, nCellZ;
	float fWeight[ 4 ];

	CalcCellPosition( fX, fZ, nCellX, nCellZ, fWeight );
	GetVertexNormal( VertexNormal[ 0 ], nCellX, nCellZ );
	GetVertexNormal( VertexNormal[ 1 ], nCellX + 1, nCellZ );
	GetVertexNormal( VertexNormal[ 2 ], nCellX, nCellZ + 1 );
	GetVertexNormal( VertexNormal[ 3 ], nCellX + 1, nCellZ + 1 );
	ReturnVec = EtVector3( 0.0f, 0.0f, 0.0f );;
	for( i = 0; i < 4; i++ )
	{
		ReturnVec += VertexNormal[ i ] * fWeight[ i ];
	}
	EtVec3Normalize( &Normal, &ReturnVec );
}

void CEtTerrainArea::GetVertexNormal( EtVector3 &Normal, int nCellX, int nCellZ )
{
	int nHeightIndex;
	float fDX, fDZ;
	EtVector3 Return;

	DWORD dwTileWidth = m_TerrainInfo.nSizeX + 1;
	DWORD dwTileHeight = m_TerrainInfo.nSizeY + 1;
	nHeightIndex = dwTileWidth * nCellZ + nCellX;
	if( nCellX == 0 )
	{
		fDX = ( m_TerrainInfo.pHeight[ nHeightIndex ] - m_TerrainInfo.pHeight[ nHeightIndex + 1 ] ) / m_TerrainInfo.fTileSize;
	}
	else if( nCellX == dwTileWidth - 1 ) 
	{
		fDX = ( m_TerrainInfo.pHeight[ nHeightIndex - 1 ] - m_TerrainInfo.pHeight[ nHeightIndex ] ) / m_TerrainInfo.fTileSize;
	}
	else
	{
		fDX = ( m_TerrainInfo.pHeight[ nHeightIndex - 1 ] - m_TerrainInfo.pHeight[ nHeightIndex + 1 ] ) / ( m_TerrainInfo.fTileSize * 2 );
	}

	if( nCellZ == 0 ) 
	{
		fDZ = ( m_TerrainInfo.pHeight[ nHeightIndex ] - m_TerrainInfo.pHeight[ nHeightIndex + dwTileWidth ] ) / m_TerrainInfo.fTileSize;
	}
	else if( nCellZ == dwTileHeight - 1 )
	{
		fDZ = ( m_TerrainInfo.pHeight[ nHeightIndex - dwTileWidth ] - m_TerrainInfo.pHeight[ nHeightIndex ] ) / m_TerrainInfo.fTileSize;
	}
	else
	{
		fDZ = ( m_TerrainInfo.pHeight[ nHeightIndex - dwTileWidth ] - m_TerrainInfo.pHeight[ nHeightIndex + dwTileWidth ] ) / ( m_TerrainInfo.fTileSize * 2 );
	}

	Return.x = fDX;
	Return.y = 1.414f;
	Return.z = fDZ;
	EtVec3Normalize( &Normal, &Return );
}

EtTerrainHandle CEtTerrainArea::GetTerrainArea( CMultiRoom *pRoom, float fX, float fZ )
{
	int i;

	for( i = 0; i < GetItemCount(pRoom); i++ )
	{
		EtTerrainHandle hTerrainArea;

		hTerrainArea = GetItem( pRoom, i );
		if( hTerrainArea->IsInside( fX, fZ ) )
		{
			return hTerrainArea;
		}
	}

	return CEtTerrainArea::Identity();
}
