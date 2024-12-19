#pragma once
#include "EtBaseEffect.h"

class CEtVolumeFog;
typedef CSmartPtr< CEtVolumeFog >	EtVolumeFogHandle;
class CEtVolumeFog : public CEtBaseEffect
{
public:
	CEtVolumeFog();
	virtual ~CEtVolumeFog();

protected:
	EtVector4 m_CamXVector;
	EtVector4 m_CamYVector;
	EtVector4 m_CamPos;

public:
	void Initialize( const char *pFogTexture, EtVector3 Pos, float fRadius, float fDensity, float fHeight, D3DXCOLOR FogColor );
	void Clear();
	virtual void Render( float fElapsedTime );
	static EtVolumeFogHandle CreateVolumeFog( const char *pFogTexture, EtVector3 Pos, float fRadius, float fDensity, float fHeight, D3DXCOLOR FogColor  );
};
