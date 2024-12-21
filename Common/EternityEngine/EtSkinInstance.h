#pragma once

#include "EtRenderStack.h"

struct SSkinRenderInfo
{
	int nTechniqueIndex;
	float fAlphaValue;
	EtBlendOP BlendOP;
	EtBlendMode SrcBlend;
	EtBlendMode DestBlend;
	bool bTwoSide;
	bool bUVTiling;
	bool bEnableAlphablend;
	bool bShowSubmesh;
};

class CEtSkinInstance
{
public:	
	CEtSkinInstance(void);
	virtual ~CEtSkinInstance(void);

protected:
	EtSkinHandle m_hSkin;

	std::vector< EtMaterialHandle > m_vecMaterialHandle;
	EtMeshHandle m_MeshHandle;
	std::vector< std::vector< SCustomParam > > m_vecCustomParam;
	std::vector< SCustomParam > m_vecDepthCustomParam;
	std::vector< SSkinRenderInfo > m_vecSkinRenderInfo;
	std::vector< int > m_vecRenderUniqueID;
	int m_nAlphaParamIndex;
	float m_fDirLightAttenuation;
	bool m_bSkyBox;

	EtMaterialHandle m_hShadowMaterial;
	std::vector< std::vector< SCustomParam > > m_vecShadowParam;

	std::vector< std::vector< int > > m_vecCustomParamIndex;
	bool				m_bRenderAlphaTwoPass;

	bool				m_bSkipBakeDepth;

	RenderPriorityEnum m_RenderPriority;
	float				m_fMeshRadius;

	BYTE				m_cDepthAlphaRef;

public:
	bool IsSkyBox() { return m_bSkyBox; }

	SSkinRenderInfo *GetSkinRenderInfo( int nIndex ) { return &m_vecSkinRenderInfo[ nIndex ]; }
	std::vector< std::vector< SCustomParam > > &GetCustomParam() { return m_vecCustomParam; }

	int AddCustomParam( const char *pParamName );
	bool SetCustomParam( int nParamIndex, void *pValue, int nSubMeshIndex = -1 );
	void RestoreCustomParam( int nParamIndex, int nSubMeshIndex = -1 );

	void SetDirLightAttenuation( float fAttenuation ) { m_fDirLightAttenuation = fAttenuation; }

	void CopySkinInfo( EtSkinHandle hSkin );

	void SetBlendOP( EtBlendOP BlendOP, int nSubmeshIndex = -1 );
	void SetSrcBlend( EtBlendMode SourceBlend, int nSubmeshIndex = -1 );
	void SetDestBlend( EtBlendMode DestBlend, int nSubmeshIndex = -1 );
	void SetCullMode( bool bTwoSide, int nSubMeshIndex = -1);
	void SetUVTiling( bool bUVTiling, int nSubMeshIndex = -1);
	void SetRenderAlphaTwoPass( bool bEnable ) { m_bRenderAlphaTwoPass = bEnable;}
	bool IsAlphaTwoPass() { return m_bRenderAlphaTwoPass; }
	void EnableAlphaBlend( bool bEnable, int nSubMeshIndex = -1 );

	void Render( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, DNVector(CEtLight *) &vecInfluenceLight, bool bShadowReceive, int nSaveMatIndex = -1 );
	void RenderImmediate( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, DNVector(CEtLight *) &vecInfluenceLight, bool bShadowReceive, bool bAlphaPass, int nSaveMatIndex = -1 );
	void RenderShadow( EtMatrix &WorldMat, int nSaveMatIndex = -1 );
	void RenderWater( int index, EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, DNVector(CEtLight *) &vecInfluenceLight, bool bShadowReceive, int nSaveMatIndex = -1);

	void SetSkipBakeDepth( bool bSkipBakeDepth ) {m_bSkipBakeDepth = bSkipBakeDepth;}
	void SetRenderPriority( RenderPriorityEnum renderPriority ) {m_RenderPriority = renderPriority;}
	void SetMeshRadius( float fRadius ) { m_fMeshRadius = fRadius; }

};
