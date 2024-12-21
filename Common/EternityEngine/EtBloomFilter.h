#pragma once
#include "EtPostProcessFilter.h"
#include "LostDeviceProcess.h"

class CEtBloomFilter : public CEtPostProcessFilter, public CLostDeviceProcess
{
public:
	CEtBloomFilter();
	virtual ~CEtBloomFilter();
	virtual void Clear();

protected:
	float m_fBloomScale;
	float m_fBloomsensitivity;
	EtTextureHandle m_hBrightTarget;
	EtTextureHandle m_hHoriBlurTarget;
	EtTextureHandle m_hVertBlurTarget;

public:
	virtual void Initialize();
	void Render( float fElapsedTime );
	void SetBloomScale( float fScale ) { m_fBloomScale = fScale; }
	float GetBloomScale() { return m_fBloomScale; }
	void SetBloomSensitivity( float fSensitivity ) { m_fBloomsensitivity = fSensitivity; }
	float GetBloomSensitivity() { return m_fBloomsensitivity; }

	virtual void OnResetDevice();
};