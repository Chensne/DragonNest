#pragma once
#include "EtShadowMap.h"

class CEtTrapezoidShadowMap :
	public CEtShadowMap
{
public:
	CEtTrapezoidShadowMap();
	virtual ~CEtTrapezoidShadowMap();
	void Clear();

protected:
	std::vector< EtVector3 > m_points;
	int FindContactVertex( D3DXVECTOR3 *pPoints, D3DXVECTOR2 vDir, bool bMax);

public:	
	void CalcShadowMat();	
};
