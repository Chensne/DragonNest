#pragma once
#include "EtPostProcessFilter.h"

#define HILIGHT_MIN_PLUS		0.015686f
#define HILIGHT_MUL_PLUS		0.619354f
#define HILIGHT_CENTER_PLUS		0.0f
#define HILIGHT_MULADD_PLUS		0.0f
#define HILIGHT_MIN_MINUS		0.125490f
#define HILIGHT_MUL_MINUS		-0.281818f
#define HILIGHT_CENTER_MINUS	0.431372f
#define HILIGHT_MULADD_MINUS	0.062068f

#define MIDTONE_MIN_PLUS		0.250980f
#define MIDTONE_MUL_PLUS		-0.938461f
#define MIDTONE_CENTER_PLUS		0.254901f
#define MIDTONE_MULADD_PLUS		0.4632f
#define MIDTONE_MIN_MINUS		0.250980f
#define MIDTONE_MUL_MINUS		-0.35f
#define MIDTONE_CENTER_MINUS	0.490196f
#define MIDTONE_MULADD_MINUS	0.015384f

#define SHADOW_MIN_PLUS			0.125490f
#define SHADOW_MUL_PLUS			-0.18f
#define SHADOW_CENTER_PLUS		0.313725f
#define SHADOW_MULADD_PLUS		0.08f
#define SHADOW_MIN_MINUS		0.0f
#define SHADOW_MUL_MINUS		0.954545f
#define SHADOW_CENTER_MINUS		1.0f
#define SHADOW_MULADD_MINUS		0.0f

class CEtColorAdjustFilter : public CEtPostProcessFilter
{
public:
	CEtColorAdjustFilter();
	virtual ~CEtColorAdjustFilter();

protected:
	EtVector4 m_SceneShadow;
	EtVector4 m_SceneMidTone;
	EtVector4 m_SceneHilight;
	EtVector4 m_SceneLuminanceWeight;
	float m_fSceneSaturation;

	EtVector4 m_HilightMin;
	EtVector4 m_HilightMul;
	EtVector4 m_HilightCenter;
	EtVector4 m_HilightMulAdd;

	EtVector4 m_MidToneMin;
	EtVector4 m_MidToneMul;
	EtVector4 m_MidToneCenter;
	EtVector4 m_MidToneMulAdd;

	EtVector4 m_ShadowMin;
	EtVector4 m_ShadowMul;
	EtVector4 m_ShadowCenter;
	EtVector4 m_ShadowMulAdd;

public:
	void SetSceneShadow( EtVector4 &vShadow ) { m_SceneShadow = vShadow; }
	EtVector4 *GetSceneShadow() { return &m_SceneShadow; }
	void SetSceneMidTone( EtVector4 &vMidTone ) { m_SceneMidTone = vMidTone; }
	EtVector4 *GetSceneMidTone() { return &m_SceneMidTone; }
	void SetSceneHilight( EtVector4 &vHilight ) { m_SceneHilight = vHilight; }
	EtVector4 *GetSceneHilight() { return &m_SceneHilight; }
	void SetSceneSaturation( float fSaturation );
	float GetSceneSaturation() { return m_fSceneSaturation; }

	virtual void Initialize();
	void Render( float fElapsedTime );
};
