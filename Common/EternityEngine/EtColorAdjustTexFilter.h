#pragma once
#include "EtPostProcessFilter.h"

class CEtColorAdjustTexFilter : public CEtPostProcessFilter
{
public:
	CEtColorAdjustTexFilter();
	virtual ~CEtColorAdjustTexFilter();
	virtual void Clear();

protected:
	EtVector4 m_SceneShadow;
	EtVector4 m_SceneMidTone;
	EtVector4 m_SceneHilight;
	EtVector4 m_SceneLuminanceWeight;
	
	// PRE_ADD_FILTEREVENT
	float m_SceneMonochrome;
	EtVector3 m_SceneAbsoluteColor;	
	//

	float m_fSceneSaturation;
	EtTextureHandle m_hLookupTex;

public:
	virtual void Initialize();
	void SetTexture( int nTextureIndex );
	void SetParam( EtVector4 &vShadow, EtVector4 &vMidTone, EtVector4 &vHilight, float fSaturation );
	EtVector4 *GetSceneShadow() 
	{ return &m_SceneShadow; }
	EtVector4 *GetSceneMidTone()
	{ return &m_SceneMidTone; }
	EtVector4 *GetSceneHilight()
	{ return &m_SceneHilight; }
	void SetSceneSaturation( float fSaturation );
	float GetSceneSaturation()
	{ return m_fSceneSaturation; }


// PRE_ADD_FILTEREVENT
	void SetSceneAbsoluteColor( bool bMonochrome, EtVector3 & vColor, float fVolume );
//

};
