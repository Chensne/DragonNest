#pragma once

extern int g_nLastError;
inline int GetLastErrorEt()
{
	return g_nLastError;
}
inline void SetLastErrorEt( int nError )
{
	g_nLastError = nError;
}

#define ET_OK					1

#define ETERR_FILENOTFOUND			-1
#define ETERR_FILECREATEFAIL		-2

#define ETERR_CREATEDEVICEFAIL			-10
#define ETERR_LOADEFFECTFAIL			-11
#define ETERR_INVALIDTECHNIQUEHANDLE	-12


#define ETERR_INVALIDRESOURCEHANDLE		-20
#define ETERR_INVALIDRESOURCESTREAM		-21
#define ETERR_MESHNOTFOUND				-22
#define ETERR_EFFECTNOTFOUND			-23
#define ETERR_TEXTURENOTFOUND			-24
#define ETERR_OUTOFMEMORY				-25


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tool Error Code
#define ETERR_DIFFERENTBONECOUNT	-32768