#pragma once

class CEtTerrainArea;

class CEtGrassBlock
{
public:
	CEtGrassBlock(void);
	virtual ~CEtGrassBlock(void);
	void Clear();

protected:
	int m_nGrassCount;
	SGrassBlockInfo m_GrassBlockInfo;
	CEtMeshStream m_MeshStream;
	EtMaterialHandle m_hMaterial;
	std::vector< SCustomParam >	m_vecCustomParam;
	EtTextureHandle m_hTexture;

	SAABox m_BoundingBox;

	EtVector4		m_InteractivePos;
	EtVector2		m_InteractiveVelocity;

	int m_nRenderUniqueID;

public:
	int GetGrassCount() { return m_nGrassCount; }
	void SetGrassBlockInfo( SGrassBlockInfo *pInfo ) { m_GrassBlockInfo = *pInfo; }
	SGrassBlockInfo *GetGrassBlockInfo() { return &m_GrassBlockInfo; }
	void Initialize( CEtTerrainArea *pTerrainArea );
	void CreateGrass( CEtTerrainArea *pTerrainArea, int nTileX, int nTileY, int nGrassIndex, char cKind, EtVector3 *pPosition, 
		EtVector2 *pTexCoord, float *pShake, WORD *pIndex );
	void CreateMaterial();

	inline char GetGrassValue( int nX, int nY )
	{
		return m_GrassBlockInfo.pGrassBuffer[ nY * m_GrassBlockInfo.nStride + nX ];
	}

	void SetTexture( const char *pFileName );
	const char *GetTextureName();
	void Render();
	void GetExtent( EtVector3 &Origin, EtVector3 &Extent );

	void SetLightMapInfluence( char *pTexBuffer, int nTexStride, int nTexWidth, int nTexHeight, float fOffsetU, float fOffsetV );

	void SetInteractivePos( EtVector3 *vPos, float fDelta );

#ifdef _TOOLCOMPILE
	EtTextureHandle GetTextureHandle() { return m_hTexture; }
#endif
};
