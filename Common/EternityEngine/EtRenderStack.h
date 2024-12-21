#pragma once

#include "Singleton.h"
#include "D3DDevice9/EtStateManager.h"

class CEtLight;
enum RenderPriorityEnum
{
	RP_NONE = -1,
	RP_EXTRA_HIGH = 0,
	RP_HIGH = 1,
	RP_ABOVE_NORMAL = 2,
	RP_NORMAL = 3,
	RP_BELOW_NORMAL = 4,
	RP_LOW = 5,
};

enum DepthTechniqueEnum {
	DT_NONE = -1,
	DT_NORMAL = 0,
	DT_ANIMATION = 1,
	DT_SKYBOX = 2,
	DT_GRASS = 3,
	DT_OPAQUE = 4,
	DT_VOLUME = 6,
	DT_TERRAIN = 8,
	DT_TOTAL,
};

struct SRenderStackElement
{
	SRenderStackElement()
	{
		nSaveMatIndex = -1;
		nTechniqueIndex = -1;
		pvecCustomParam = NULL;
		pRenderSubMesh = NULL;
		pRenderMeshStream = NULL;
		nDrawStart = 0;
		nDrawCount = 0;
		for( int i = 0; i < MAX_POINT_LIGHT_COUNT + MAX_SPOT_LIGHT_COUNT; i++) {
			pInfluenceLight[ i ] = NULL;
		}
		fDirLightAttenuation = 1.0f;
		fDist = 0.f;
		nStateBlockIndex = -1;
		renderPriority = RP_NORMAL;
		nDiffuseTexIndex = -1;
		nBakeDepthIndex = DT_NORMAL;
		nRenderUniqueID = -1;
		bIsTwoPassAlpha = false;
		bLastElement = false;		// for internal use, do not setting this flag;
		bIsAlphaSky = false;
	}
	EtMaterialHandle hMaterial;
	EtMatrix WorldMat;
	EtMatrix PrevWorldMat;
	int nSaveMatIndex;
	int nTechniqueIndex;
	std::vector< SCustomParam > *pvecCustomParam;
	CEtSubMesh *pRenderSubMesh;
	CEtMeshStream *pRenderMeshStream;
	int nDrawStart;
	int nDrawCount;
	CEtLight *pInfluenceLight[ MAX_POINT_LIGHT_COUNT + MAX_SPOT_LIGHT_COUNT ];
	float fDirLightAttenuation;
	float fDist;
	int nStateBlockIndex;
	RenderPriorityEnum	renderPriority;
	int nDiffuseTexIndex;
	int nBakeDepthIndex;
	int nRenderUniqueID;
	bool bIsTwoPassAlpha;
	bool bLastElement;
	bool bIsAlphaSky;
};

struct SOcclusionCheck
{
	int nOcclusionIndex;
	int nRenderUniqueID;
};

#include "EtInstancingMng.h"
#include "EtOcclusionMng.h"

#if defined(_RDEBUG) || defined(_DEBUG)
//#define _CHECK_RENDER_ORDER
#endif

class CEtRenderStack
{	
public:
	enum PassType {
		PASS_SHADOW = 0,
		PASS_OPAQUE = 1,
		PASS_INSTANCING = 2,
		PASS_ALPHADEPTH = 3,
		PASS_ALPHA = 4,
		PASS_USEBACKBUFFER = 5,
		PASS_ALPHASKYBOX = 6,
		PASS_WATER = 7, 
		PASS_COUNT = 22,
	};
public:
	CEtRenderStack();
	virtual ~CEtRenderStack();

protected:
	std::vector< SRenderStackElement* > m_vecRenderElement[ PASS_COUNT ];

	float m_fElapsedTime;
	CEtInstancingMng m_InstancingManager;
	CEtOcclusionMng m_OcclusionMng[ 2 ];
	int m_nCurOcclusionMngIndex;
	int m_nCurentUniqueID;
	std::vector< SOcclusionCheck > m_vecOcclusionCheck[ 2 ];
	std::map< int, int > m_mapOcclusionCheck[ 2 ];

	static bool s_bUseOcclusion;
	static bool s_bRenderDepth;

	int m_nInteractivePosIndex;
	int m_nDiffuseTexParamIndex;
	int m_nVolumeTexParamIndex;
	EtMaterialHandle m_hBakeDepthMaterial;
	std::vector< SCustomParam > m_vecBakeDepthParam;

	std::vector< SRenderStackElement* > m_RenderBlockPoolFree;
	std::vector< SRenderStackElement* > m_RenderBlockPoolUsed;

	// RenderDepth Param
	EtVector4 m_vInteractivePos;
	float			m_fComputeAniTime;

	std::vector< SRenderStackElement* > m_vecRadixSort[ 2 ][ 10 ];

#if defined( _CHECK_RENDER_ORDER )
	int				m_nRenderStopIndex;
	int				m_nRenderCurrIndex;
#endif

public:
	void SetWaterCount( int nCount ) { 
		ASSERT( nCount < PASS_COUNT - PASS_WATER);
	}

	void ClassifyAlphaDepth();
	SRenderStackElement* AllocBlock( SRenderStackElement &RenderElement ) ;
	void RefreshAllocBlock();

	void AddShadowRenderElement( SRenderStackElement &RenderElement ) { 
		m_vecRenderElement[ PASS_SHADOW ].push_back( AllocBlock( RenderElement ) ); 
	}
	void AddWaterRenderElement( int index, SRenderStackElement &RenderElement ) { 
		m_vecRenderElement[ PASS_WATER + index ].push_back( AllocBlock( RenderElement ) ); 
	}
	int AddNormalRenderElement( SRenderStackElement &RenderElement, int nRenderUniqueID );
	void AddAlphaRenderElement( SRenderStackElement &RenderElement ) { 
		m_vecRenderElement[ PASS_ALPHA ].push_back( AllocBlock( RenderElement ) ); 
	}
	void AddAlphaDepthRenderElement( SRenderStackElement &RenderElement ) { 
		m_vecRenderElement[ PASS_ALPHADEPTH ].push_back( AllocBlock( RenderElement ) ); 
	}
	void AddUseBackBufferRenderElement( SRenderStackElement &RenderElement ) { 
		m_vecRenderElement[ PASS_USEBACKBUFFER ].push_back( AllocBlock( RenderElement ) ); 
	}
	void AddUseSkyBoxAlphaRenderElement( SRenderStackElement &RenderElement ){
		m_vecRenderElement[ PASS_ALPHASKYBOX ].push_back( AllocBlock( RenderElement ) );
	}
	void Sort( int passType, bool bOpaque );
	void RadixSort( std::vector< SRenderStackElement* > &vecSortNeed );
	void CalcOcclusion();
	void SetElapsedTime( float fElapsedTime ) { m_fElapsedTime = fElapsedTime; }	
	void RenderDepthBlock( PassType passType, bool bAutoDeleteList = false);
	void RenderBlock( PassType passType, bool bAutoDeleteList = true);
	void UpdateSkyBoxRTT();
	void FlushRender( bool bExtraRender = true);
	void ClearElement( int passType ) { m_vecRenderElement[ passType ].clear(); }

	void EmptyRenderElement();
	void EnableInstancing( bool bEnable )  {m_InstancingManager.Enable( bEnable ); }

	static void EnableOcclusion( bool bOcclusion ) { s_bUseOcclusion = bOcclusion; }
	static bool IsOcclusion() { return s_bUseOcclusion; }
	static void EnableDepthRender( bool bEnable ) { s_bRenderDepth = bEnable; }
	static bool IsDepthRender() { return s_bRenderDepth; }

	void FlushBuffer();
};

extern CEtRenderStack *g_pRenderStack[];
CEtRenderStack *GetCurRenderStack();
