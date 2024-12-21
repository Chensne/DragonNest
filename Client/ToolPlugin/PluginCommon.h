#pragma once


enum PluginTypeEnum {
	EVENTAREA_DLL,
	ACTIONSIGNAL_DLL,
};

/* Common Func List
__declspec( dllexport ) int GetPluginType();
__declspec( dllexport ) int GetPluginParam( int nIndex );
__declspec( dllexport ) HINSTANCE RegResource()
*/


/* EVENTAREA_DLL
__declspec( dllexport ) bool OnLButtonClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
__declspec( dllexport ) bool OnRButtonClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
__declspec( dllexport ) bool OnLButtonDoubleClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
__declspec( dllexport ) bool OnRButtonDoubleClick( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea )
__declspec( dllexport ) bool OnProcess( CWnd *pWnd, CEtEngine *pRender, bool bSelect, CEtWorldEventArea *pArea )
__declspec( dllexport ) bool OnChangeName( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea, const char *szPrevName, const char *szNewName )
__declspec( dllexport ) bool OnChangeParameter( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea, CPropertyVariable *pVariable, int nIndex )
__declspec( dllexport ) bool OnInitialize( CWnd *pWnd, CEtEngine *pRender, CEtWorldEventArea *pArea, const char *szName, CPropertyVariable **ppVariable, int nParamCount )
*/

/* ACTIONSIGNAL_DLL
*/