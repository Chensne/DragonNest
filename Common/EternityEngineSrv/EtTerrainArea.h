#pragma once

#include "EtCollisionPrimitive.h"
#include "EtTerrain.h"
#include "MultiSmartPtr.h"
#include "MultiCommon.h"

#define DEFAULT_TERRAIN_SIZE		50

class CEtGrassBlock;

class CEtTerrainArea;
typedef CMultiSmartPtr< CEtTerrainArea >	EtTerrainHandle;

class CEtTerrainArea : public CMultiSmartPtrBase< CEtTerrainArea, MAX_SESSION_COUNT >, public TBoostMemoryPool< CEtTerrainArea >
{
public:
	CEtTerrainArea( CMultiRoom *pRoom );
	virtual ~CEtTerrainArea();
	void Clear();

protected:
	SAABox m_BoundingBox;
	STerrainInfo m_TerrainInfo;
	std::vector< CEtTerrain * > m_vecTerrain;

	int m_nBlockCountX;
	int m_nBlockCountY;
	int m_nBlockSizeX;
	int m_nBlockSizeY;

public:
	void CalcSelectCount( int nBlockIndex, int &nStart, int &nEnd );
	void Initialize( STerrainInfo *pInfo );
	STerrainInfo *GetTerrainInfo() { return &m_TerrainInfo; }
	void InitializeBlock( int nBlockIndex = -1 );
	CEtTerrain *CreateBlock( TerrainType Type );
	void ChangeBlockType( TerrainType Type, int nBlockIndex = -1 );
	TerrainType GetBlockType( int nBlockIndex );
	void GetExtent( EtVector3 &Origin, EtVector3 &Extent );
	void CalcBoundingBox();
	void UpdateHeight( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateLayer( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateNormal( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateTextureCoord( int nStartX, int nStartY, int nEndX, int nEndY );

	bool Pick( EtVector3 &Origin, EtVector3 &Direction, EtVector3 &PickPos );

	void GetBlockCount( int &nCountX, int &nCountY ) { nCountX = m_nBlockCountX; nCountY = m_nBlockCountY; }
	void GetBlockSize( int &nSizeX, int &nSizeY ) { nSizeX = m_nBlockSizeX; nSizeY = m_nBlockSizeY; }
	SAABox *GetBoundingBox( int nBlockIndex ) { return m_vecTerrain[ nBlockIndex ]->GetBoundingBox(); }
	bool IsInside( float fX, float fZ );

	void CalcCellPosition( float fX, float fZ, int &nCellX, int &nCellZ, float *pWeight );
	float GetLandHeight( float fX, float fZ, EtVector3 *pNormal );
	void GetLandNormal( EtVector3 &Normal, float fX, float fZ );
	void GetVertexNormal( EtVector3 &Normal, int nCellX, int nCellZ );

	static EtTerrainHandle GetTerrainArea( CMultiRoom *pRoom, int nIndex ) { return GetItem( pRoom, nIndex ); }
	static EtTerrainHandle GetTerrainArea( CMultiRoom *pRoom, float fX, float fZ );
};
