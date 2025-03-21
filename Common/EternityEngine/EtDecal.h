#pragma once

struct CalcHeightFn
{
	virtual float GetHeight( float fX, float fY, float fZ, int nScale ) = 0;
};

struct SDecalVertex
{
	EtVector3 Vertex;
	EtVector2 TexCoord;
	float Alpha;
};

class CEtDecal;
typedef CSmartPtr< CEtDecal >	EtDecalHandle;

class CEtDecal : public CSmartPtrBase< CEtDecal >
{
public:
	CEtDecal(void);
	virtual ~CEtDecal(void);

protected:
	CEtMeshStream m_DecalStream;
	EtTextureHandle m_hDecalTexture;
	EtMaterialHandle m_hMaterial;
	EtColor m_DecalColor;
	std::vector< SCustomParam >	m_vecCustomParam;
	SAABox m_BoundingBox;
	int m_nFrustumMask;

	float m_fLifeTime;
	float m_fAppearTime;
	float m_fProgressTime;
	int m_nTechniqueIndex;

	int m_nSrcBlend;
	int m_nDestBlend;
	bool m_bUseAddressUVWrap;
	bool m_bUseFixedUV;

	bool	m_bInitialize;

	EtMatrix m_MatBias;

public:
	void Initialize( EtTextureHandle hTexture, float fX, float fZ, float fRadius, float fLifeTime, float fAppearTime, float fRotate, EtColor &DecalColor, int nSrcBlend = 0, int nDestBlend = 0, CalcHeightFn *pHeightFunc = NULL, float fYHint = 0.f);
	void Update( float fX, float fZ, float fRadius, float fRotate, EtColor &DecalColor, CalcHeightFn *pHeightFunc , float fYHint, float fHintThreshold = 1800.0f );

	void SetFrustumMask( int nMask ) { m_nFrustumMask = nMask; }
	int GetFrustumMask() { return m_nFrustumMask; }
	void AddFrustumMask( int nMask ) { 	m_nFrustumMask |= nMask; }
	void UseAddressUVWrap( bool bUse ) { m_bUseAddressUVWrap = bUse; }
	void UseFixedUV( bool bUse ) { m_bUseFixedUV = bUse; }

	void GetExtent( EtVector3 &Origin, EtVector3 &Extent );

	bool Process( float fElapsedTime );
	virtual void Render();
	virtual bool IsPointLight() {return false;}

	static void RenderDecalList( int nMask, float fElapsedTime );
	static void ProcessDeleteDecalList( float fElapsedTime );
	static void ClearFrustumMask();
	static void MaskFrustumDecalList( CEtConvexVolume *pFrustum, int nFrustumMask );
};

class CEtPointLightDecal : public CEtDecal
{
public:
	CEtPointLightDecal(void);
	virtual ~CEtPointLightDecal(void);

protected:

public:
	void Initialize( SLightInfo &LightInfo, float fRadius, float fLifeTime );
	void Update( SLightInfo &LightInfo, float fRadius );
	void CalcPointLight( EtColor &OutColor, SLightInfo &LightInfo, EtVector3 &vPosition, EtVector3 &vNormal );
	virtual bool IsPointLight() {return true;}

};
