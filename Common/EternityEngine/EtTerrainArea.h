#pragma once

#include "EtPrimitive.h"
#include "EtTerrain.h"
#ifdef _TOOLCOMPILE
#include "EtGrassBlock.h"
#endif

#define DEFAULT_TERRAIN_SIZE		50

class CEtGrassBlock;

class CEtTerrainArea;
typedef CSmartPtr< CEtTerrainArea >	EtTerrainHandle;

class CEtTerrainArea : public CSmartPtrBase< CEtTerrainArea >
{
public:
	CEtTerrainArea();
	virtual ~CEtTerrainArea();
	void Clear();


protected:
	bool m_bShow;
	static bool s_bOptimize;
	SAABox m_BoundingBox;
	STerrainInfo m_TerrainInfo;
	std::vector< CEtTerrain * > m_vecTerrain;

	EtIndexBuffer *m_pCommonIndexBuffer;
	EtIndexBuffer *m_pCommonLowIndexBuffer;

	int m_nBlockCountX;
	int m_nBlockCountY;
	int m_nBlockSizeX;
	int m_nBlockSizeY;

	std::vector< CEtGrassBlock * > m_vecGrassBlock;
	int m_nGrassBlockCountX;
	int m_nGrassBlockCountY;

	char *m_pLightMapInfluenceBuffer;
	int m_nLightMapInfluenceStride;
	static bool s_bEnableFrustumCull;

	struct SBakeLightMapParam
	{
		int nBlockIndex;
		int nWidth;
		int nHeight;
		float fBlurSize;
		int nBakeRange;
		float fSlopeBias;
	};
	std::vector< SBakeLightMapParam > m_vecBakeLightMapParam;
	
public:
	void Show( bool bShow ) { m_bShow = bShow; }
	bool IsShow() { return m_bShow; }
	static void EnableOptimize( bool bOptimize ) { s_bOptimize = bOptimize; }
	void CalcSelectCount( int nBlockIndex, int &nStart, int &nEnd );
	void DrawGrid( bool bDraw, int nBlockIndex = -1 );
	void SetGridColor( DWORD dwColor, int nBlockIndex = -1 );
	void SetTextureDistance( int nTexLayer, float fDistance, int nBlockIndex = -1 );
	float GetTextureDistance( int nTexLayer, int nBlockIndex = 0 ) 
		{ return m_vecTerrain[ nBlockIndex ]->GetTextureDistance( nTexLayer ); }
	void SetTextureRotation( int nTexLayer, float fRotation, int nBlockIndex = -1 );
	float GetTextureRotation( int nTexLayer, int nBlockIndex = 0 ) 
		{ return m_vecTerrain[ nBlockIndex ]->GetTextureRotation( nTexLayer ); }

	void Initialize( STerrainInfo *pInfo );
	STerrainInfo *GetTerrainInfo() { return &m_TerrainInfo; }
	void CreateCommonIndexBuffer();
	void InitializeBlock( int nBlockIndex = -1 );
	CEtTerrain *CreateBlock( TerrainType Type );
	void ChangeBlockType( TerrainType Type, int nBlockIndex = -1 );
	TerrainType GetBlockType( int nBlockIndex );
	void Render( CEtConvexVolume *pFrustum );
	void RenderWater( int index, CEtConvexVolume *pFrustum );
	void GetExtent( EtVector3 &Origin, EtVector3 &Extent );
	void CalcBoundingBox();
	void UpdateHeight( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateLayer( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateNormal( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateTextureCoord( int nStartX, int nStartY, int nEndX, int nEndY );
	void UpdateGrassBlock( int nStartX, int nStartY, int nEndX, int nEndY );

	int GetGrassBlockCount();
	void SetGrassInfo( int nBlockIndex, SGrassBlockInfo &Info );
	SGrassBlockInfo *GetGrassInfo( int nBlockIndex );

	static void RenderTerrainBlockList( CEtConvexVolume *pFrustum );

	bool Pick( EtVector3 &Origin, EtVector3 &Direction, EtVector3 &PickPos );
	bool Pick( int nX, int nY, EtVector3 &PickPos, int nCameraIndex = 0 );

	void GetBlockCount( int &nCountX, int &nCountY ) { nCountX = m_nBlockCountX; nCountY = m_nBlockCountY; }
	void GetBlockSize( int &nSizeX, int &nSizeY ) { nSizeX = m_nBlockSizeX; nSizeY = m_nBlockSizeY; }
	EtMaterialHandle GetMaterial( int nBlockIndex = 0 ) { return m_vecTerrain[ nBlockIndex ]->GetMaterial(); }
	int GetTextureCount( int nBlockIndex = 0 ) { return m_vecTerrain[ nBlockIndex ]->GetTextureCount(); }
	const char *GetTextureName( int nTexIndex, int nBlockIndex = 0 ) 
		{ return m_vecTerrain[ nBlockIndex ]->GetTextureName( nTexIndex ); }
	bool SetTexture( int nTexIndex, const char *pTexName, int nBlockIndex = -1 ); 
	const char *GetTextureSemanticName( int nTexIndex, int nBlockIndex = 0 ) 
		{ return m_vecTerrain[ nBlockIndex ]->GetTextureSemanticName( nTexIndex ); }
	void SetGrassTexture( const char *pFileName );
	const char *GetGrassTexture();
	SAABox *GetBoundingBox( int nBlockIndex ) { return m_vecTerrain[ nBlockIndex ]->GetBoundingBox(); }
	bool IsInside( float fX, float fZ );

	EtTextureHandle GetLightMap( int nBlockIndex ) { return m_vecTerrain[ nBlockIndex ]->GetLightMap(); }
	void BakeLightMap( int nBlockIndex, int nWidth, int nHeight, float fBlurSize, int nBakeRange = 3, float fSlopeBias = 10.0f );
	void _BakeLightMap( int nBlockIndex, int nWidth, int nHeight, float fBlurSize, int nBakeRange, float fSlopeBias );
	bool _BakeLightMapFromFile( int nBlockIndex, EtTextureHandle hTexture );
	void BakeLowDetailMap();
	void CreateLightCamera( int nBlockIndex, EtCameraHandle &hCamera, int nWidth, int nHeight, EtMatrix &LightMapViewProjMat );
	EtTextureHandle BakeDepthLightMap( int nBlockIndex, EtTextureHandle hLightMap, LPD3DXRENDERTOSURFACE pDepthSurface, EtMaterialHandle hMaterial,
		EtMatrix &LightMapViewProjMat, int nBakeRange, float fSlopeBias );
	void BakeRoughLightMap( int nBlockIndex, EtTextureHandle hRoughLightMap, EtTextureHandle hDepthLightMap, LPD3DXRENDERTOSURFACE pRoughSurface, EtMaterialHandle hMaterial, EtCameraHandle hCamera, int nWidth, int nHeight, EtMatrix &LightMapViewProjMat );
	void CreateLightMapInfluenceBuffer();
	void DeleteLightMapInfluenceBuffer() { SAFE_DELETE( m_pLightMapInfluenceBuffer ); }
	void SetLightMapInfluence( int nBlockIndex, char *pTexBuffer, int nTexStride, int nTexWidth, int nTexHeight );
	float GetLightMapInfluence( float fX, float fZ );
	static float CalcLightMapInfluence( float fX, float fZ );

	void CalcCellPosition( float fX, float fZ, int &nCellX, int &nCellZ, float *pWeight, int nScale = 1 );
	float GetLandHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nScale = 1 );
	void GetLandNormal( EtVector3 &Normal, float fX, float fZ );
	void GetVertexNormal( EtVector3 &Normal, int nCellX, int nCellZ );

	void CreateGrassBlock();

	int GetBakeLightMapParamCount() { return ( int )m_vecBakeLightMapParam.size(); }
	void GetBakeLightMapParam( int nIndex, SBakeLightMapParam &Param )
	{
		if( ( nIndex < 0 ) || ( nIndex >= ( int )m_vecBakeLightMapParam.size() ) )
		{
			return;
		}
		Param = m_vecBakeLightMapParam[ nIndex ];
	}
	void SetInteractivePos( EtVector3 *vPos, float fDelta );
	static void CheckBakeLightMap();

	static void ReloadMaterial();
	static EtTerrainHandle GetTerrainArea( int nIndex ) { return GetItem( nIndex ); }
	static EtTerrainHandle GetTerrainArea( float fX, float fZ );

	static void EnableFrustumCull( bool bEnable ) { s_bEnableFrustumCull = bEnable; }
	static bool IsEnableFrustumCull() { return s_bEnableFrustumCull; }

#ifdef _TOOLCOMPILE
	EtTextureHandle GetTexture( int nBlockIndex = 0, int nTexIndex = 0 ) { return m_vecTerrain[ nBlockIndex ]->GetTexture( nTexIndex ); }
	EtTextureHandle GetGrassTextureHandle() { return m_vecGrassBlock[ 0 ]->GetTextureHandle(); }
#endif
};
