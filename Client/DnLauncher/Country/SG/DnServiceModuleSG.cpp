#include "stdafx.h"
#include "DnServiceModuleSG.h"

#if defined(_SG)

#include "../../Extern/GPK/Include/GPKitClt.h"
#pragma comment( lib, "../../Extern/GPK/Lib/GPKitClt.lib" )
using namespace SGPK;

BOOL CDnServiceModuleSG::Initialize()
{
	IGPKCltDynCode* pCltDynCode = NULL;
	pCltDynCode = GPKStart( GPK_URL, "DN" );
	if( !pCltDynCode )
		return FALSE;

	return TRUE;
}

void CDnServiceModuleSG::Destroy()
{
}

#endif // _SG