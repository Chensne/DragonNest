#include "StdAfx.h"
#include "DnWorldLightProp.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldLightProp::CDnWorldLightProp()
{
}

CDnWorldLightProp::~CDnWorldLightProp()
{
	SAFE_RELEASE_SPTR( m_LightHandle );
}

bool CDnWorldLightProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( CDnWorldProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale ) == false ) return false;

	if( GetData() ) {
		LightStruct *pStruct = (LightStruct *)GetData();

		SLightInfo Info;
		Info.Type = (LightType)pStruct->nType;
		Info.Diffuse = *(EtColor*)pStruct->vDiffuse;
		Info.Diffuse.r *= Info.Diffuse.a;
		Info.Diffuse.g *= Info.Diffuse.a;
		Info.Diffuse.b *= Info.Diffuse.a;

		Info.Specular = *(EtColor*)pStruct->vSpecular;
		Info.Specular.r *= Info.Specular.a;
		Info.Specular.g *= Info.Specular.a;
		Info.Specular.b *= Info.Specular.a;

		Info.Position = *pStruct->vPosition;
		Info.Direction = *pStruct->vDirection;
		Info.fRange = pStruct->fRange;
		Info.fFalloff = pStruct->fFalloff;
		Info.fTheta = cos( EtToRadian( pStruct->fTheta ) );
		Info.fPhi = cos( EtToRadian( pStruct->fPhi ) );

		m_LightHandle = EternityEngine::CreateLight( &Info );
	}

	return true;
}
