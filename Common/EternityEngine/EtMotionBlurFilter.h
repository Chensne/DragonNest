#pragma once
#include "EtPostProcessFilter.h"

class CEtMotionBlurFilter : public CEtPostProcessFilter
{
public:
	CEtMotionBlurFilter();
	virtual ~CEtMotionBlurFilter();

protected:
	float m_fMotionBlurScale;

public:
	virtual void Initialize();
	virtual void Render( float fElapsedTime );
};