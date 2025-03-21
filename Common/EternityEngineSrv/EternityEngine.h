#pragma once

#include "AssertX.h"
#include "Singleton.h"
#include "SmartPtr.h"
#include "Stream.h"
#include "EtError.h"
#include "SundriesFunc.h"

#include <d3dx9.h>
#pragma warning(default:4819)

#include "EtMathWrapperD3D.h"
#include "EtWrapper.h"
#include "EtType.h"
#include "EtObject.h"
#include "EtAniObject.h"
#include "EtTerrainArea.h"
#include "EtLoader.h"

namespace EternityEngine
{

//--------------------------------------------------------------------------------
EtObjectHandle CreateStaticObject( CMultiRoom *pRoom, const char *pSkinName );
EtAniObjectHandle CreateAniObject( CMultiRoom *pRoom, const char *pSkinName, const char *pAniName );


//--------------------------------------------------------------------------------
EtTerrainHandle CreateTerrain( CMultiRoom *pRoom, STerrainInfo *pTerrainInfo );


EtResourceHandle LoadMesh( CMultiRoom *pRoom, const char *pFileName );
EtResourceHandle LoadAni( CMultiRoom *pRoom, const char *pFileName );
EtResourceHandle LoadSkin( CMultiRoom *pRoom, const char *pFileName );

}
