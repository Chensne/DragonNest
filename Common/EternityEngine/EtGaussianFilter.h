#pragma once
#include "EtPostProcessFilter.h"

class CEtGaussianFilter : public CEtPostProcessFilter
{
public:
	CEtGaussianFilter();
	~CEtGaussianFilter();
	virtual void Clear();

	struct STempRenderTarget
	{
		int nWidth;
		int nHeight;
		EtTextureHandle hRenderTarget;
	};

protected:
	EtTextureHandle m_hCurRenderTarget;
	EtVector4 m_vBlurRadius;
	int m_nTexParamIndex;
	int m_nBlurRadiusIndex;
	static std::vector< STempRenderTarget > s_vecTempRenderTarget;

public:
	// 가우시안 필터는 실제 렌더링 루프에서 렌더 되는게 아니라. 블룸처럼 필요한 필터들이 호출해서
	// 사용하도록 해야 한다. 그래서 Render() 코드가 없다.
	virtual void Initialize();
	virtual void Render( float fElapsedTime ) {}

	void CreateRenderTarget( int nWidth, int nHeight );
	void RenderGaussianFilter( CEtTexture *pSourTex, CEtTexture *pTargetTex, float fBlurSize );
};