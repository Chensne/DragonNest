#pragma once
#include "EtPostProcessFilter.h"

class CEtCameraBlurFilter : public CEtPostProcessFilter, public CLostDeviceProcess
{
public:
	CEtCameraBlurFilter();
	virtual ~CEtCameraBlurFilter();
	virtual void Clear();

protected:
	EtTextureHandle m_hAccumBlurTarget;
	float m_fBlendFactor;

public:
	virtual void Initialize();
	virtual void Render( float fElapsedTime );
	virtual void OnResetDevice();
	void SetBlendFactor( float fBlendFactor ) { m_fBlendFactor = fBlendFactor; }
};