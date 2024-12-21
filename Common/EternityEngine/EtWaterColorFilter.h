#pragma once
#include "EtPostProcessFilter.h"
#include "LostDeviceProcess.h"

class CEtWaterColorFilter : public CEtPostProcessFilter, public CLostDeviceProcess
{
public:
	CEtWaterColorFilter();
	virtual ~CEtWaterColorFilter();
	virtual void Clear();

protected:
	EtTextureHandle m_hHoriBlurTarget;
	EtTextureHandle m_hLookupTex;
	EtVector4 m_vBlurRadius;

public:
	virtual void Initialize();
	void Render( float fElapsedTime );
	void SetGradeCount( int nGrade );
};