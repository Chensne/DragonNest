#pragma once
#include "EtPostProcessFilter.h"

class CEtHeatHazeFilter : public CEtPostProcessFilter
{
public:
	CEtHeatHazeFilter();
	virtual ~CEtHeatHazeFilter();
	virtual void Clear();

protected:
	EtTextureHandle m_hTexture;
	float	m_fElapsedTime;

	float	m_fDistortionPower;
	float	m_fWaveSpeed;

public:
	void SetDistortionPower( float fPower ) {m_fDistortionPower = fPower;}
	void SetWaveSpeed( float fSpeed ) { m_fWaveSpeed = fSpeed; }
	void SetTexture( char *szFileName );

	virtual void Initialize();
	virtual void Render( float fElapsedTime );
};