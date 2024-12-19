#pragma once

#include "EtWorldDecal.h"
class CDnWorldDecal : public CEtWorldDecal {
public:
	CDnWorldDecal( CEtWorldSector *pSector );
	virtual ~CDnWorldDecal();

protected:
	EtTextureHandle m_hTexture;
	EtDecalHandle m_hDecal;

public:
	virtual bool Initialize( EtVector2 vPos, float fRadius, float fRotate, EtColor Color, float fAlpha, const char *szTextureName );
};