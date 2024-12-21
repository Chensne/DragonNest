// EventMonsetSetAreaInfo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "PluginCommon.h"
#include "EtResourceMng.h"
#include "DNTableFile.h"
#include "EtUIXML.h"
#include "EternityEngine.h"
#include "EtEngine.h"
#include "EtWorldEventArea.h"
#include "EtWorldEventControl.h"
#include "PropertyVariable.h"
#include "MonsterSetDlg.h"

#ifdef _MANAGED
#error Please read instructions in EventUnitAreaInfo.cpp to compile with /clr
// If you want to add /clr to your project you must do the following:
//	1. Remove the above include for afxdllx.h
//	2. Add a .cpp file to your project that does not have /clr thrown and has
//	   Precompiled headers disabled, with the following text:
//			#include <afxwin.h>
//			#include <afxdllx.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DNTableFileFormat *s_pSox[3] = { NULL, NULL, NULL };
CEtResourceMng *s_pResMng = NULL;
CEtUIXML *s_pUIXml = NULL;


static AFX_EXTENSION_MODULE EventMonsetSetAreaInfoDLL = { NULL, NULL };


// CEventMonsetSetAreaInfoApp construction

// CEventMonsetSetAreaInfoApp initialization

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("EventMonsetSetInfo.DLL Initializing!\n");

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(EventMonsetSetAreaInfoDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(EventMonsetSetAreaInfoDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("EventMonsetSetInfo.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(EventMonsetSetAreaInfoDLL);

		for( int i=0; i<3; i++ )
			SAFE_DELETE( s_pSox[i] );
		SAFE_DELETE( s_pUIXml );
	}

	return 1;   // ok
}

#ifdef _MANAGED
#pragma managed(pop)
#endif
void FindExtFileList( CEtResourceMng *pResMng, const char *szFileName, std::vector<CFileNameString> &szVecList )
{
	char szTemp[_MAX_PATH] = { 0, };
	char szName[256] = { 0, };
	char szExt[256] = { 0, };
	_GetFileName( szName, szFileName );
	_GetExt( szExt, szFileName );
	sprintf_s( szTemp, "%s*.%s", szName, szExt );
	pResMng->FindFileListAll_IgnoreExistFile( "ext", szTemp, szVecList );

}

DNTableFileFormat *LoadSox( CEtResourceMng *pResMng, const char *szFileName, bool bGenerationInverseFindMap )
{
	DNTableFileFormat *pSox = NULL;
	std::vector<CFileNameString> szVecList;
	FindExtFileList( pResMng, szFileName, szVecList );
	pSox = new DNTableFileFormat;
	for( DWORD i=0; i<szVecList.size(); i++ ) {
		pSox->Load( szVecList[i].c_str(), false );
	}
	return pSox;
}

__declspec( dllexport ) int GetPluginType()
{
	return PluginTypeEnum::EVENTAREA_DLL;
}

__declspec( dllexport ) int GetPluginParam( int nIndex )
{
	switch( nIndex ) {
		case 0: return 5;
	}
	return -1;
}

__declspec( dllexport ) HINSTANCE RegResource( CEtResourceMng *pResMng )
{
	HINSTANCE hOldInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( EventMonsetSetAreaInfoDLL.hModule );

	if( s_pResMng == NULL ) {
		s_pResMng = pResMng;
		s_pSox[0] = LoadSox( pResMng, "MonsterTable.dnt", true );
		s_pSox[1] = LoadSox( pResMng, "MonsterSetTable.dnt", true );
		s_pSox[2] = LoadSox( pResMng, "MonsterGroupTable.dnt", true );
		s_pUIXml = new CEtUIXML;

		CFileStream Stream( pResMng->GetFullName( "uistring.xml" ).c_str() );
		if( Stream.IsValid() ) {
			s_pUIXml->Initialize( &Stream, CEtUIXML::idCategory1 );
		}
	}
	return hOldInstance;      
}

__declspec( dllexport ) bool OnOpenView( CWnd *pWnd, CEtEngine *pRender )
{
	return true;
}

__declspec( dllexport ) bool OnCloseView( CWnd *pWnd, CEtEngine *pRender )
{
	return true;
}

__declspec( dllexport ) bool OnLButtonClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
{
	return false;
}

__declspec( dllexport ) bool OnRButtonClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
{
	return false;
}

__declspec( dllexport ) bool OnLButtonDoubleClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
{
	CMonsterSetDlg Dlg;
	Dlg.SetEventArea( pArea );
	Dlg.DoModal();
	/*
	CString szAreaName = pArea->GetName();
	char szHeadStr[256] = { 0, };
	sscanf_s( szAreaName.GetBuffer(), "%s", szHeadStr, 256 );
	_strlwr_s( szHeadStr );
	*/

	return true;
}

__declspec( dllexport ) bool OnRButtonDoubleClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
{
	return false;
}

__declspec( dllexport ) bool OnProcess( CWnd *pWnd, CEtEngine *pRender, bool bSelect, CEtWorldEventArea *pArea )
{
	return false;
}

__declspec( dllexport ) bool OnChangeName( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea, const char *szPrevName, const char *szNewName )
{
	return false;
}

__declspec( dllexport ) bool OnChangeParameter( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea, CPropertyVariable *pVariable, int nIndex )
{
	return false;
}

__declspec( dllexport ) bool OnInitialize( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea, const char *szName, CPropertyVariable **ppVariable, int nParamCount )
{
	return false;
}

__declspec( dllexport ) bool OnRefresh( CWnd *pWnd, CEtEngine *pRender, CEtWorldSector *pSector )
{
	return false;
}
