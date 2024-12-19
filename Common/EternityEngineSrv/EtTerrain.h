#pragma once

#include "EtCollisionPrimitive.h"
#include "MemPool.h"

class CEtConvexVolume;

class CEtTerrain: public TBoostMemoryPool< CEtTerrain >
{
public:
	CEtTerrain(void);
	virtual ~CEtTerrain(void);
	void Clear();

protected:
	int m_nStride;
	int m_nStrideVert;
	int m_nBlockOffsetX;
	int m_nBlockOffsetY;
	STerrainInfo m_TerrainInfo;
	SAABox m_BoundingBox;

public:
	void SetStride( int nStride, int nStrideVert ) 
	{ 
		m_nStride = nStride; 
		m_nStrideVert = nStrideVert; 
	}
	void SetBlockOffset( int nOffsetX, int nOffsetY ) { m_nBlockOffsetX = nOffsetX, m_nBlockOffsetY = nOffsetY; }

	inline int TileOffset( int nX, int nY ) { return nY * m_TerrainInfo.nSizeX + nX; }
	inline int VertexOffset( int nX, int nY ) { return nY * ( m_TerrainInfo.nSizeX + 1 ) + nX; }

	inline float GetHeight( int nX, int nY ) 
	{
		if( ( nX + m_nBlockOffsetX >= m_nStride ) || ( nY + m_nBlockOffsetY >= m_nStrideVert ) || ( nX + m_nBlockOffsetX < 0 ) || ( nY + m_nBlockOffsetY < 0 ) )
		{
			return 0.0f;
		}
		else
		{
			return m_TerrainInfo.pHeight[ nY * m_nStride + nX ] * m_TerrainInfo.fHeightMultiply; 
		}
	}
	inline DWORD GetLayerValue( int nX, int nY ) 
	{ 
		if( ( nX + m_nBlockOffsetX >= m_nStride ) || ( nY + m_nBlockOffsetY >= m_nStrideVert ) )
		{
			return 0;
		}
		else
		{
			return m_TerrainInfo.pLayerDensity[ nY * m_nStride + nX ]; 
		}
	}

	virtual void Initialize();
	STerrainInfo *GetTerrainInfo() { return &m_TerrainInfo; }
	void SetTerrainInfo( STerrainInfo *pInfo )
		{ memcpy( &m_TerrainInfo, pInfo, sizeof( STerrainInfo ) ); }

	EtVector3 GetVertexNormal( int nIndexX, int nIndexY );

	void GetExtent( EtVector3 &Origin, EtVector3 &Extent );
	SAABox *GetBoundingBox() { return &m_BoundingBox; }
	void CalcBoundingBox();
	void CalcBoundingBoxHeight();

	float Pick( EtVector3 &Origin, EtVector3 &Direction, EtVector3 &PickPos );
	bool IsInTerrain( int nX, int nY );
	float TestLintToTile( EtVector3 &Origin, EtVector3 &Direction, int nTileX, int nTileY );
};
