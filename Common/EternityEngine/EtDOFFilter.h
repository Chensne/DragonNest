#pragma once
#include "EtPostProcessFilter.h"

class CEtDOFFilter : public CEtPostProcessFilter
{
public:
	CEtDOFFilter();
	virtual ~CEtDOFFilter();
	virtual void Clear();

protected:
	EtVector4 m_vDOFValue;
	EtVector4 m_vDOFBlurSize;
	float m_fFocusDistance;
	float m_fNearBlurSize;
	float m_fFarBlurSize;

	float m_fNearDOFStart;
	float m_fNearDOFEnd;
	float m_fFarDOFStart;
	float m_fFarDOFEnd;
	float m_fCamFar;

	EtTextureHandle m_hBlurSizeBuffer;
	EtTextureHandle m_hBlurSizeBlurBuffer;
	EtTextureHandle m_hDOFHoriBuffer;
	EtTextureHandle m_hDOFVertBuffer;

	bool		m_bDOFEnable;

	std::vector< std::pair<SUICoord, float> > m_blurRectList;
	float		m_blurRectSize;

public:
	virtual void Initialize();
	virtual void Render( float fElapsedTime );

	float GetFocusDistance() { return m_fFocusDistance; }
	void SetFocusDistance( float fDistance ) { m_fFocusDistance = fDistance; }

	float GetNearBlurSize() { return m_fNearBlurSize; }
	void SetNearBlurSize( float fSize ) { m_fNearBlurSize = fSize; }
	float GetFarBlurSize() { return m_fFarBlurSize; }
	void SetFarBlurSize( float fSize ) { m_fFarBlurSize = fSize; }
	void SetBlurSize( float fSize ) { SetNearBlurSize( fSize ); SetFarBlurSize( fSize ); }

	float GetNearDOFStart() { return m_fNearDOFStart; }
	float GetNearDOFEnd() { return m_fNearDOFEnd; }
	float GetFarDOFStart() { return m_fFarDOFStart; }
	float GetFarDOFEnd() { return m_fFarDOFEnd; }

	void SetNearDOFStart( float fStart ) { m_fNearDOFStart = fStart; }
	void SetNearDOFEnd( float fEnd ) { m_fNearDOFEnd = fEnd; }
	void SetFarDOFStart( float fStart ) { m_fFarDOFStart = fStart; }
	void SetFarDOFEnd( float fEnd ) { m_fFarDOFEnd = fEnd; }

	void AddBlurRect( SUICoord Coord, float fBlurWidth );
	void DrawBlurRect();
};
