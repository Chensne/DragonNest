#include "stdafx.h"
#include "DnServiceModuleCHN.h"

#if defined(_CHN)

#include "../../Extern/GPK/Include/GPKitClt.h"
#pragma comment( lib, "../../Extern/GPK/Lib/GPKitClt.lib" )
using namespace SGPK;

BOOL CDnServiceModuleCHN::Initialize()
{
	IGPKCltDynCode* pCltDynCode = NULL;
	pCltDynCode = GPKStart( GPK_URL, "DN" );
	if( !pCltDynCode )
		return FALSE;

	return TRUE;
}

void CDnServiceModuleCHN::Destroy()
{
}

#endif // _CHN