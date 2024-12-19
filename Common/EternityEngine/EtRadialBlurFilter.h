#pragma once
#include "EtPostProcessFilter.h"

class CEtRadialBlurFilter : public CEtPostProcessFilter
{
public:
	CEtRadialBlurFilter();
	virtual ~CEtRadialBlurFilter();

protected:
	float m_fBlurSize;
	EtVector4 m_vBlurCenter;

public:
	virtual void Initialize();

	void SetBlurSize( float fSize ) { m_fBlurSize = fSize; }
	float GetBlurSize() { return m_fBlurSize; }

	void SetBlurCenter( float fX, float fY ) { m_vBlurCenter.x = fX; m_vBlurCenter.y = fY; }
	void GetBlurCenter(float &fX, float &fY ) { fX = m_vBlurCenter.x; fY = m_vBlurCenter.y; }
};