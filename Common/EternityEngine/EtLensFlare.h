#pragma once

#include "LostDeviceProcess.h"

class CEtLightShaftFilter;
class CEtLensFlare;
typedef CSmartPtr< CEtLensFlare >	EtLensFlareHandle;

class CEtLensFlare : public CSmartPtrBase< CEtLensFlare >, public CLostDeviceProcess
{
public:
	CEtLensFlare(void);
	virtual ~CEtLensFlare(void);

protected:
	EtVector3 m_LightDir;
	EtTextureHandle m_hSunTexture;
	EtTextureHandle m_hFlareTexture;

	float m_fSunRadius;
	float m_fAspectRatio;
	float m_fFlareRadius;
	float m_fFlareAlphaSmooth;
	EtMatrix m_FlareProjMat;

	CEtOcclusionQuery m_Query;
	CEtOcclusionQuery m_StartQuery;
	DWORD m_dwMaxVisiblePoint;

	CEtLightShaftFilter *m_pLightShaftFilter;

public:
	void Initialize( EtVector3 &LightDir, const char *pSunTextureName, const char *pFlareTextureName );
	void Render( float fElpasedTime );
	void DrawSun( float fElpasedTime );
	void CalcMaxVisiblePoint();
	void SetFlareRadius( float fSunRadius, float fFlareRadius );
	void GetFlareRadius( float &fSunRadius, float &fFlareRadius ) { fSunRadius = m_fSunRadius; fFlareRadius = m_fFlareRadius; }
	void SetDirection( EtVector3 &LightDir );
	void SetSunTexture( const char *pSunTextureName );
	void SetFlareTexture( const char *pFlareTextureName );
	float GetSunRadius() { return m_fSunRadius; }
	float GetFlareRadius() { return m_fFlareRadius; }
	void DrawImage( EtTextureHandle hTexture, SUICoord UVCoord, DWORD dwColor, SUICoord ScreenCoord,  float fZValue );

	static EtLensFlareHandle CreateLensFlare( EtVector3 &LightDir, const char *pSunTextureName, const char *pFlareTextureName );
	static void RenderFlareList( float fElpasedTime );

	virtual void OnLostDevice();
};
