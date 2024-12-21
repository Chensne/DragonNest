#include "stdafx.h"
#include "DnServiceModuleJPN.h"

#if defined(_JPN)

extern CString g_szCmdLine;

BOOL CDnServiceModuleJPN::Initialize()
{
	DnNHNService::CreateInstance();
	if( DnNHNService::GetInstance().UpdateInfo( g_szCmdLine ) == FALSE )
		return FALSE;

	return TRUE;
}

void CDnServiceModuleJPN::Destroy()
{
	DnNHNService::DestroyInstance();
}

#endif // _JPN