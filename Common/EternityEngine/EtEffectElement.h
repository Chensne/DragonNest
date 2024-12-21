#pragma once

#include "EtCustomParam.h"

class CEtElementData;

class CEtEffectElement
{
public:
	CEtEffectElement();
	virtual ~CEtEffectElement(void);

protected:
	CEtElementData *m_pElementData;
	bool m_bShow;
	bool m_bPrevShow;
	EtMatrix m_WorldMat;
	EtMatrix m_PrevWorldMat;
	EtColor m_Color;
	float m_fAlpha;
	int m_nEffectTick;

public:
	bool IsShow() { return m_bShow; }
	virtual void Initialize( CEtElementData *pElementData );
	virtual void Process( EtMatrix &WorldMat, int nEffectTick, float fScaleVal );
	virtual void Render() {}
	virtual void RenderImmediate() {}

	virtual void SetRenderType( RenderType Type ) {}
	virtual void EnableCull( bool bEnable ) {}

	void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }
};


#define EFFECT_MAX_CUSTOM_PARAMETER_COUNT	4
class CEtEffectMeshElement : public CEtEffectElement
{
public:
	CEtEffectMeshElement();
	virtual ~CEtEffectMeshElement(void);

protected:
	EtSkinHandle m_hSkin;
	CEtSkinInstance *m_pSkinInstance;

	int m_nAmbientIndex;
	int m_nDiffuseIndex;
	int	m_nFXTime;

public:
	virtual void Initialize( CEtElementData *pElementData );
	virtual void Render();
	virtual void RenderImmediate();
};


class CEtEffectBillboardEffectElement : public CEtEffectElement
{
public:
	CEtEffectBillboardEffectElement();
	virtual ~CEtEffectBillboardEffectElement(void);

protected:
	EtBillboardEffectDataHandle m_hBillboardEffectData;
	EtBillboardEffectHandle m_hBillboardEffect;
	bool m_bNeedSetRenderType;
	RenderType m_RenderType;
	bool m_bNeedSetCull;
	bool m_bEnableCull;

public:
	virtual void Initialize( CEtElementData *pElementData );
	virtual void Process( EtMatrix &WorldMat, int nEffectTick, float fScaleVal );
	virtual void SetRenderType( RenderType Type );

	virtual void EnableCull( bool bEnable );
	virtual void RenderImmediate();
};


class CEtEffectPointLightElement : public CEtEffectElement
{
public:
	CEtEffectPointLightElement();
	virtual ~CEtEffectPointLightElement(void);

protected:
	EtLightHandle m_hLight;
	SLightInfo m_LightInfo;

public:
	virtual void Initialize( CEtElementData *pElementData );
	virtual void Process( EtMatrix &WorldMat, int nEffectTick, float fScaleVal );
};
