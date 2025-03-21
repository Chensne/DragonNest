#pragma once
#include "etpostprocessfilter.h"

class CEtLightShaftFilter : public CEtPostProcessFilter
{
public:
	CEtLightShaftFilter();
	virtual ~CEtLightShaftFilter();

protected:
	EtTextureHandle m_hLightShaftSour;
	EtTextureHandle m_hLightShaftDest;
	EtVector2 m_vSunPosition;

public:
	void Initialize();
	void Render( float fElapsedTime );
	void DrawFilter( EtTextureHandle hTarget, int nTechnique, EtVector2 vStart, EtVector2 vEnd, EtVector2 vStartUV, EtVector2 vEndUV );
	void SetSunPosition( EtVector2 &vPosition ) { m_vSunPosition = vPosition; }

};
