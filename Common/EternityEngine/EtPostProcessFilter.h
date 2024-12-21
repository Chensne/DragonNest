#pragma once

class CEtPostProcessFilter
{
public:
	CEtPostProcessFilter();
	virtual ~CEtPostProcessFilter();
	virtual void Clear();

protected:
	EtMaterialHandle m_hMaterial;
	std::vector< SCustomParam >	m_vecCustomParam;
	float m_fDrawScale;
	ScreenFilter m_Type;
	bool m_bEnable;

public:
	void Enable( bool bEnable ) { m_bEnable = bEnable; }
	bool IsEnable() { return m_bEnable; }

	virtual void Initialize() = 0;
	virtual void Render( float fElapsedTime );
	void SetDrawSacle( float fScale ) { m_fDrawScale = fScale; }
	void DrawFilter( EtTextureHandle hTarget, int nTechnique = 0, EtVector2 vStart = EtVector2(0, 0), EtVector2 vEnd = EtVector2(1, 1) , bool bClear = false);
	EtTextureHandle AddRenderTarget( int nWidth, int nHeight, const char *pParamName, EtFormat Format = FMT_A8R8G8B8 );

	ScreenFilter GetType() { return m_Type; }
};

/*
class CEtOilPaintingFilter : public CEtPostProcessFilter
{
public:
	CEtOilPaintingFilter();
	virtual ~CEtOilPaintingFilter();

protected:
	int m_nBrushRadius;
	EtTextureHandle m_hCopyTarget;
	EtTextureHandle m_hOilPaintingTarget;

public:
	void Initialize();
	void Render( float fElapsedTime );

};
*/

#include "EtBloomFilter.h"
#include "EtColorAdjustFilter.h"
#include "EtColorAdjustTexFilter.h"
#include "EtDOFFilter.h"
#include "EtGaussianFilter.h"
#include "EtHeatHazeFilter.h"
#include "EtMotionBlurFilter.h"
#include "EtOutlineFilter.h"
#include "EtRadialBlurFilter.h"
#include "EtTransitionFilter.h"
#include "EtWaterColorFilter.h"
#include "EtLightShaftFilter.h"
#include "EtCameraBlurFilter.h"