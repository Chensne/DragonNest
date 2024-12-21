#ifdef _DEBUG 
#undef THIS_FILE 
static char THIS_FILE[]=__FILE__; 
#define new DEBUG_NEW 
#endif 

#include "Stdafx.h"
#include "EternityEngine.h"
#include "EtEngine.h"
#include "EtLoader.h"
#include "EtObject.h"
#include "EtAniObject.h"

namespace EternityEngine {

EtResourceHandle LoadMesh( CMultiRoom *pRoom, const char *pFileName )
{
	return LoadResource( pRoom, pFileName, RT_MESH );
}

EtResourceHandle LoadAni( CMultiRoom *pRoom, const char *pFileName )
{
	return LoadResource( pRoom, pFileName, RT_ANI );
}

EtResourceHandle LoadSkin( CMultiRoom *pRoom, const char *pFileName )
{
	return LoadResource( pRoom, pFileName, RT_SKIN );
}

EtObjectHandle CreateStaticObject( CMultiRoom *pRoom, const char *pSkinName )
{
	EtResourceHandle hSkin;
	CEtObject *pObject;

	hSkin = LoadSkin( pRoom, pSkinName );
	if( !hSkin ) return CEtObject::Identity();

	pObject = new IBoostPoolEtObject( pRoom );
	pObject->Initialize( hSkin );

	return pObject->GetMySmartPtr();
}

EtAniObjectHandle CreateAniObject( CMultiRoom *pRoom, const char *pSkinName, const char *pAniName )
{
	EtSkinHandle hSkin;
	EtAniHandle hAni;
	CEtAniObject *pObject;

	hSkin = LoadSkin( pRoom, pSkinName );
	if( !hSkin ) return CEtAniObject::Identity();

	if( pAniName ) {
		hAni = LoadAni( pRoom, pAniName );
		if( !hAni )
		{
			hSkin->Release();
			return CEtAniObject::Identity();
		}
	}

	pObject = new CEtAniObject( pRoom );

	pObject->Initialize( hSkin, hAni );

	return pObject->GetMySmartPtr();
}

EtTerrainHandle CreateTerrain( CMultiRoom *pRoom, STerrainInfo *pTerrainInfo )
{
	CEtTerrainArea *pTerrain;		

	pTerrain = new CEtTerrainArea( pRoom );
	pTerrain->Initialize( pTerrainInfo );

	return pTerrain->GetMySmartPtr();
}


}