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
	// ����þ� ���ʹ� ���� ������ �������� ���� �Ǵ°� �ƴ϶�. ���ó�� �ʿ��� ���͵��� ȣ���ؼ�
	// ����ϵ��� �ؾ� �Ѵ�. �׷��� Render() �ڵ尡 ����.
	virtual void Initialize();
	virtual void Render( float fElapsedTime ) {}

	void CreateRenderTarget( int nWidth, int nHeight );
	void RenderGaussianFilter( CEtTexture *pSourTex, CEtTexture *pTargetTex, float fBlurSize );
};