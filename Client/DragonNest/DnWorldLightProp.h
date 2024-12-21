#pragma once

#include "DnWorldProp.h"

class CDnWorldLightProp : public CDnWorldProp
{
public:
	CDnWorldLightProp();
	virtual ~CDnWorldLightProp();

protected:
	EtLightHandle m_LightHandle;

public:
	// CEtWorldProp Message
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
};