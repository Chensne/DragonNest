// stdafx.cpp : source file that includes just the standard includes
// DnManualUpdater.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


bool g_bSilenceMode = false;


#ifdef	USE_SERVER_VERSION_CHECK
bool g_bPassCheckVersion = false;
#endif // USE_SERVER_VERSION_CHECK