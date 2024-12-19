#pragma once

class CEtShadowMap;
extern CEtShadowMap *g_pEtShadowMap;

class CEtShadowMap
{
public:
	CEtShadowMap(void);
	virtual ~CEtShadowMap(void);
	void Clear();

protected:
	int m_nShadowMapSize;
	ShadowQuality m_ShadowQuality;
	ShadowType m_ShadowType;
	EtFormat m_ShadowMapFormat;

	EtMatrix m_LightViewMat;
	EtMatrix m_LightProjMat;
	EtMatrix m_LightProjDepthMat;

	float m_fShadowRange;
	EtColor m_ShadowColor;
	EtTextureHandle m_hShadowMap;
	EtTextureHandle m_hBlurShadowMap;
	EtDepthHandle m_hShadowMapDepth;
	float m_fDistanceLightAt;

	EtSurface *m_pBackupRenderTarget;
	EtSurface *m_pBackupDepthTarget;
	
	bool	m_bEnable;

	EtMaterialHandle		m_hShadowMaterial;
	std::vector< SCustomParam > m_vecCustomParam;
	int			m_nVertexDecl;
	bool		m_bUseSoftShadow;

public:
	virtual void Initialize( ShadowQuality Quality, ShadowType Type );

	void ResetRenderTarget();
	bool IsEnable() { return m_bEnable; }

	EtMatrix *GetLightViewMat() { return &m_LightViewMat; }
	EtMatrix *GetLightProjMat() { return &m_LightProjMat; }
	EtMatrix *GetLightProjDepthMat() { return &m_LightProjDepthMat; }

	float GetShadowRange() { return m_fShadowRange; }
	void SetShadowRange( float fRange ) { m_fShadowRange = fRange; }

	void SetShadowQuality( ShadowQuality Quality ) { m_ShadowQuality = Quality; }
	ShadowQuality GetShadowQuality() { return m_ShadowQuality; }

	EtColor *GetShadowColor() { return &m_ShadowColor; }
	void SetShadowColor( EtColor &Color ) { m_ShadowColor = Color; }
	void DownSampling();
	void ClearBuffer();

	EtBaseTexture *GetShadowMapTexture();
	virtual void DebugRender();

	virtual void CalcShadowMat();
	virtual void BeginShadow();
	virtual void EndShadow();

	void ResetShadowMap();

	void LoadMaterial();
	void Bake( CEtRenderStack *pStack );

	virtual float GetObjectScale() { return 1.f; } 
};

CEtShadowMap *GetEtShadowMap();
void DeleteShadowMap();
void CreateSimpleSahdowMap( ShadowQuality Quality, ShadowType Type );
