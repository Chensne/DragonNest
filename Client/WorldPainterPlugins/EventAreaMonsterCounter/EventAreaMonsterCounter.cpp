// EventAreaMonsterCounter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
//#include "PropertyVariable.h"
#include "EternityEngine.h"
#include "EtEngine.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "PluginCommon.h"
#include "EtResourceMng.h"
#include "DNTableFile.h"
#include "MonsterCounterDlg.h"
#include "TEtWorldEventArea.h"

#ifdef _MANAGED
#error Please read instructions in EventAreaMonsterCounter.cpp to compile with /clr
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


static AFX_EXTENSION_MODULE EventAreaMonsterCounterDLL = { NULL, NULL };

#ifdef _MANAGED
#pragma managed(push, off)
#endif

DNTableFileFormat *s_pSox[5] = { NULL, };
CEtResourceMng *s_pResMng = NULL;
CMonsterCounterDlg *m_pDlg = NULL;

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("EventAreaMonsterCounter.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(EventAreaMonsterCounterDLL, hInstance))
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

		new CDynLinkLibrary(EventAreaMonsterCounterDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("EventAreaMonsterCounter.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(EventAreaMonsterCounterDLL);
		if( m_pDlg ) {
			m_pDlg->DestroyWindow();
			SAFE_DELETE( m_pDlg );
		}

		for( int i=0; i<5; i++ )
			SAFE_DELETE( s_pSox[i] );
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
	_GetFileName( szName,256, szFileName );
	_GetExt( szExt,256, szFileName );
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
	return -1;
}

__declspec( dllexport ) HINSTANCE RegResource( CEtResourceMng *pResMng )
{
	HINSTANCE hOldInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( EventAreaMonsterCounterDLL.hModule );

	if( s_pResMng == NULL ) {
		s_pResMng = pResMng;
		s_pSox[0] = LoadSox( pResMng, "MonsterTable.dnt", true );
		s_pSox[1] = LoadSox( pResMng, "MonsterCompoundTable.dnt", true );
		s_pSox[2] = LoadSox( pResMng, "NpcTable.dnt", true );
		s_pSox[3] = LoadSox( pResMng, "MonsterSetTable.dnt", true );
		s_pSox[4] = LoadSox( pResMng, "MonsterGroupTable.dnt", true );
	}
	return hOldInstance;      
}

__declspec( dllexport ) bool OnOpenView( CWnd *pWnd, CEtEngine *pRender )
{
	m_pDlg = new CMonsterCounterDlg;
	m_pDlg->Create(IDD_DIALOG1, pWnd);
	m_pDlg->ShowWindow( SW_SHOW );
	return true;
}

__declspec( dllexport ) bool OnCloseView( CWnd *pWnd, CEtEngine *pRender )
{
	m_pDlg->DestroyWindow();
	SAFE_DELETE( m_pDlg );
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
	if( m_pDlg ) m_pDlg->Refresh( pSector );
	return false;
}
