//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#if _MSC_VER == 1600
#define POINTER_64 __ptr64
#endif

 

#include "DXUT.h"
#include "DXUTgui.h"
#include "resource.h"
#include <strsafe.h>
#include <GPLUSDx9.h>
#include <GPLUSClient.h>

#include <SDOA4Client.h>
#include <SDOADx9.h>

CDXUTDialogResourceManager  g_DialogResourceManager;
CDXUTDialog          g_UI;
#define IDC_BUTTON_LOGOUT          1


HINSTANCE          g_hInstGPLUSDll = 0;
LPGPlusInitialize    g_pInitialize = NULL;
LPGPlusGetModule     g_pGetModule = NULL;
LPGPlusTerminal      g_pTerminal = NULL;


IGPLUSApp*			g_GPlusApp = NULL;
IGPLUSDx9*			g_GPlusDx9 = NULL;

HINSTANCE g_hInstIGWDll  = 0;
LPigwInitialize    igwInitialize = NULL;
LPigwGetModule     igwGetModule = NULL;
LPigwTerminal      igwTerminal = NULL;

ISDOADx9*			g_SDOADx9 = NULL;
ISDOAApp*			g_SDOAApp = NULL;


/* �ͷ�GPLUS */
void UnloadGPLUS()
{
    g_GPlusDx9 = NULL;
    g_GPlusApp = NULL;

    g_pInitialize = NULL;
    if (g_pTerminal)
    {  // �ͷ�DLLǰ�����ȵ���gpTerminal
        g_pTerminal();
        g_pTerminal = NULL;
    }
    if (g_hInstGPLUSDll)
    {
        FreeLibrary(g_hInstGPLUSDll);
        g_hInstGPLUSDll = NULL;
    }
}

/* ����GPLUS */
void LoadGPLUS()
{
    WCHAR strExePath[MAX_PATH] = {0};
    WCHAR strExeName[MAX_PATH] = {0};
    WCHAR* strLastSlash = NULL;
    GetModuleFileNameW( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH-1]=0;
    strLastSlash = wcsrchr( strExePath, TEXT('\\') );
    if( strLastSlash )
    {	// �õ�EXE����·��
        StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
        *strLastSlash = 0;
        strLastSlash = wcsrchr( strExeName, TEXT('.') );
        if( strLastSlash )
            *strLastSlash = 0;
    }

    WCHAR strGameWidgetDll[MAX_PATH] = {0};

    StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\GPlus\\GPlusBridge.dll", strExePath );


    int appId = 200010900;
    FILE* f = fopen("./appid.txt","r");
    if(f != NULL){
        char value[1024] = {0};
        if(fgets(value,1024,f))
        {
            appId = atoi(value);
        }
        fclose(f);
    }

    UnloadGPLUS();
    BOOL bLoadOk = FALSE;

    if ( GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF )
    {	// �ҵ���������	
        g_hInstGPLUSDll = LoadLibrary(strGameWidgetDll);
        if (g_hInstGPLUSDll)
        {
            //��ȡ��������
            g_pInitialize = (LPGPlusInitialize)GetProcAddress( g_hInstGPLUSDll, "GPlusInitialize" );
            g_pGetModule = (LPGPlusGetModule)GetProcAddress( g_hInstGPLUSDll, "GPlusGetModule" );
            g_pTerminal = (LPGPlusTerminal)GetProcAddress( g_hInstGPLUSDll, "GPlusTerminal" );
            if ( g_pInitialize && g_pGetModule && g_pTerminal)
            {
                int appId = 200010900;
                FILE* f = fopen("./appid.txt","r");
                if(f != NULL){
                    char value[1024] = {0};
                    if(fgets(value,1024,f))
                    {
                        appId = atoi(value);
                    }
                    fclose(f);
                }

                // ע��GPlusAppInfo���������ֶα�����
                GPlusAppInfo tmpAppInfo = {
                    sizeof(GPlusAppInfo),       // �ṹ���С
                    appId,						// ����Ӧ��ID���ӿ�������վ������
                    L"��̶��ѨOnLine",          // Ӧ������
                    L"1.0.0.1",
                    GPLUS_RENDERTYPE_D3D9,      // �ͻ���֧�ֵ�ͼ����������
                    0,
                    1,
                    2,
                };

                //��ʼ��
                if (g_pInitialize(GPLUS_SDK_VERSION, &tmpAppInfo) == GPLUS_OK)
                {	// ��ʼ������ɹ�

                    bLoadOk = TRUE;

                    // ��ȡ����ӿ�	
                    g_pGetModule(__uuidof(IGPLUSApp), (void**)&g_GPlusApp);
                    g_pGetModule(__uuidof(IGPLUSDx9), (void**)&g_GPlusDx9);

                    /* ע��:������һ�δ���,Ӧ������Ϸ��ɫ��¼�ɹ��ٵ���,���������ʾ��
                    if ( g_GPlusDx9 && g_GPlusApp)
                    {
                    GPlusUserInfo tmpRoleInfo = { sizeof(GPlusUserInfo),
                    L"����id" , 
                    L"��ɫ��"} ;

                    g_GPlusApp->Login(&tmpRoleInfo);
                    }*/
                }
            }
        }
    }

    if (TRUE != bLoadOk)
    {
        UnloadGPLUS();
    }
}

/* �ͷ�IGW */
void CloseIGW()
{
    if (g_SDOADx9)
    {
        g_SDOADx9->Release();
        g_SDOADx9 = NULL;
    }

    if (g_SDOAApp)
    {
        g_SDOAApp->Release();
        g_SDOAApp = NULL;
    }


    if (igwTerminal)
    {  // �ͷ�DLLǰ�����ȵ���igwTerminal
        igwTerminal();
        igwTerminal = NULL;
    }
    if (g_hInstIGWDll)
    {
        FreeLibrary(g_hInstIGWDll);
        g_hInstIGWDll = 0;
    }
    igwInitialize = NULL;
}

/* ����IGW */
void OpenIGW()
{
    WCHAR strExePath[MAX_PATH] = {0};
    WCHAR strExeName[MAX_PATH] = {0};
    WCHAR* strLastSlash = NULL;
    GetModuleFileNameW( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH-1]=0;
    strLastSlash = wcsrchr( strExePath, TEXT('\\') );
    if( strLastSlash )
    {	// �õ�EXE����·��
        StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
        *strLastSlash = 0;
        strLastSlash = wcsrchr( strExeName, TEXT('.') );
        if( strLastSlash )
            *strLastSlash = 0;
    }

    WCHAR strGameWidgetDll[MAX_PATH] = {0};
    StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\GameWidget.dll", strExePath );

    if ( !(GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF) )
    { // �����������·���в������򻻸�·������
        StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\..\\..\\..\\..\\bin\\GameWidget.dll", strExePath );
    }

    CloseIGW();

    if ( GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF )
    {	// �ҵ���������	
        g_hInstIGWDll = LoadLibraryW(strGameWidgetDll);
        if (g_hInstIGWDll)
        {
            igwInitialize = (LPigwInitialize)GetProcAddress( g_hInstIGWDll, "igwInitialize" );
            igwGetModule = (LPigwGetModule)GetProcAddress( g_hInstIGWDll, "igwGetModule" );
            igwTerminal = (LPigwTerminal)GetProcAddress( g_hInstIGWDll, "igwTerminal" );
            if ( igwInitialize && igwGetModule && igwTerminal)
            {
                // ���سɹ�

                // ע��AppInfo���������ֶα�����
                AppInfo tmpAppInfo = {
                    sizeof(AppInfo),       // �ṹ���С��������չ
                    9999,                  // ����Ӧ��ID���ӿ�������վ������
                    L"��̶��ѨOnLine",     // Ӧ������
                    L"0.1.2.0",            // Ӧ�ÿͻ��˵�ǰ�汾��
                    SDOA_RENDERTYPE_D3D9,  // �ͻ���֧�ֵ�ͼ���������ͣ�����ͬʱ֧��d3d9��d3d8
                    1,                     // ��ͬʱ��һ̨��������Ϸ�����������ͨ��Ϊ1������:KOFӦ����2��
                    -1,                    // ��Ϸ��ID��������ʱ����-1
                    -1                     // ��Ϸ��ID��������ʱ����-1

                };

                if (igwInitialize(SDOA_SDK_VERSION, &tmpAppInfo) == SDOA_OK)
                {	// ��ʼ������ɹ�

                    // ��ȡ����ӿ�
                    igwGetModule(__uuidof(ISDOADx9), (void**)&g_SDOADx9);
                    igwGetModule(__uuidof(ISDOAApp), (void**)&g_SDOAApp);

                    if ( (!g_SDOADx9) || (!g_SDOAApp))
                        CloseIGW();

                }
                else // ��ʼ��ʧ���������
                    CloseIGW();
            }
            else  // ����DLLʧ���������
                CloseIGW();
        }
    }


}

//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                     bool bWindowed, void* pUserContext )
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
        D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{
    if (g_GPlusDx9)
    {	// ��ʼ��GPLUS�ڲ�ͼ������
        D3DPRESENT_PARAMETERS d3dpp = DXUTGetD3D9PresentParameters ();
        g_GPlusDx9->Initialize( pd3dDevice, &d3dpp, FALSE );
    }
    if (g_SDOADx9)
    {	// ��ʼ��IGW�ڲ�ͼ������
        D3DPRESENT_PARAMETERS d3dpp = DXUTGetD3D9PresentParameters ();
        g_SDOADx9->Initialize( pd3dDevice, &d3dpp, FALSE );
    }

    HRESULT hr;
    V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                   void* pUserContext )
{
    if(g_GPlusDx9)
    {	// ֪ͨGPLUS d3d�豸���óɹ�
        D3DPRESENT_PARAMETERS d3dpp = DXUTGetD3D9PresentParameters () ;	
        g_GPlusDx9->OnDeviceReset ( &d3dpp ) ;
    }
    if(g_SDOADx9)
    {	// ֪ͨIGW d3d�豸���óɹ�
        D3DPRESENT_PARAMETERS d3dpp = DXUTGetD3D9PresentParameters () ;	
        g_SDOADx9->OnDeviceReset ( &d3dpp ) ;
    }

    HRESULT hr;
    V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );

    g_UI.SetLocation( 0, 0 );
    g_UI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        /* ����GPLUS���� */
        if (g_GPlusDx9)
            g_GPlusDx9->RenderEx();

        /* ����IGW���� */
        if (g_SDOADx9)
            g_SDOADx9->RenderEx();

        V( g_UI.OnRender( fElapsedTime ) );
        V( pd3dDevice->EndScene() );
    }
}

BOOL isSDOLogin = false; // ע��һ��Ҫ��BOOL���ͣ�����Ϊbool������ܻᵼ������
LoginResult loginResult;

BOOL CALLBACK OnLogin(int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved) 
{ // ע��ûص�����true��رյ�¼�Ի��򣬷����¼�Ի�����Ȼ��ʾ

    isSDOLogin = (SDOA_ERRORCODE_OK == nErrorCode); 
    if (isSDOLogin) 
    { 
        loginResult = *pLoginResult; 

        // ��¼�ɹ���,���û�ʹ�õ���Ϸ��ɫ��Ϣ����IGW
        if (g_SDOAApp)
        {
            RoleInfo sdoatmpRoleInfo = { sizeof(RoleInfo), L"��ò����", 0 } ;
            g_SDOAApp->SetRoleInfo(&sdoatmpRoleInfo);  // ���ý�ɫ��Ϣ���ɶ�ε��ã�ֻҪ�û��л��˽�ɫ�͵���֪ͨIGW

            if ( g_GPlusDx9 && g_GPlusApp)
            {
                wchar_t ID[256]={0};
                MultiByteToWideChar(CP_ACP, 0, pLoginResult->szSndaid, -1, ID, _countof(ID));
                GPlusUserInfo gplustmpRoleInfo = { sizeof(GPlusUserInfo),
                    ID , 
                    sdoatmpRoleInfo.pwcsRoleName} ;

                g_GPlusApp->Login(&gplustmpRoleInfo);

                g_UI.SetVisible(true);
            }
        }
    } 

    if (SDOA_ERRORCODE_SHOWMESSAGE == nErrorCode)
    { // ��¼������Ϣ����
        if (nReserved != 0)
        {
            LoginMessage* pCurLoginMsg = (LoginMessage*)nReserved;
            if (pCurLoginMsg->dwSize >= sizeof(LoginMessage))
            { // �����Դ��� 
                // �޸Ĵ�����ʾ�Ի������
                //SysReAllocString(pCurLoginMsg->pbstrTitle, L"[������Ϸ������ʾ]");

                // ��ʱ����FALSE��ʹ��Ĭ����ʾ

                // ��ʱ����TRUE��ʹ���Զ������ʾ
                //isSDOLogin = TRUE;
                //::MessageBox(0, *pCurLoginMsg->pbstrContent, *pCurLoginMsg->pbstrTitle, MB_OK);
            }
        }
    }
    return isSDOLogin; // �����߼�����¼�ɹ���رյ�¼�Ի���
} 


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         bool* pbNoFurtherProcessing, void* pUserContext )
{
    if(g_GPlusDx9) 
    {	// ����Ϣ����GPLUSģ�鴦�� 
       /* if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
        {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(h, &pt);
            if (FALSE == g_SDOADx9->HasUI(&pt)) {
                return FALSE;
            }
        }*/
        LRESULT lResult; 
        if (g_GPlusApp->OnWindowProc( hWnd, uMsg, wParam, lParam, &lResult ) == GPLUS_OK ) 
        { 
            *pbNoFurtherProcessing = true;// ���������ٴ�����Ϣ 
            return lResult; 
        } 
    } 
    if(g_SDOADx9) 
    {	// ����Ϣ����IGWģ�鴦�� 
        LRESULT lResult; 
        if (g_SDOADx9->OnWindowProc( hWnd, uMsg, wParam, lParam, &lResult ) == SDOA_OK ) 
        { 
            *pbNoFurtherProcessing = true;// ���������ٴ�����Ϣ 
            return lResult; 
        } 
    } 
    if ( (UINT)SDOA_WM_CLIENT_RUN == uMsg )
    {
        // �˿̿��Կ�ʼ��¼
        if (g_SDOAApp)
        {
            g_SDOAApp->ShowLoginDialog(OnLogin, 0, 0);
        }
    }

    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    *pbNoFurtherProcessing = g_UI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
    /* ֪ͨGPLUS d3d�豸��ʧ */
    if(g_GPlusDx9)
        g_GPlusDx9->OnDeviceLost();
    /* ֪ͨIGW d3d�豸��ʧ */
    if(g_SDOADx9)
        g_SDOADx9->OnDeviceLost();

    g_DialogResourceManager.OnD3D9LostDevice();
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    /* �ͷ�GPLUSͼ������ */
    if (g_GPlusDx9)
        g_GPlusDx9->Finalize();
    /* �ͷ�IGWͼ������ */
    if (g_SDOADx9)
        g_SDOADx9->Finalize();

    g_DialogResourceManager.OnD3D9DestroyDevice();
}

void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    /* ����GPLUS */
    LoadGPLUS();

    /* ����IGW */
    OpenIGW();
    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );


    // TODO: Perform any application-level initialization here

    g_UI.Init( &g_DialogResourceManager );
    g_UI.SetCallback( OnGUIEvent ); 
    g_UI.AddButton( IDC_BUTTON_LOGOUT, L"ע��", 200, 10, 125, 22 );
    g_UI.SetVisible(false);

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"EmptyProject_dx9" );
    DXUTCreateDevice( true, 800, 600 );

    if (g_pInitialize)
    {	// ���سɹ�
        wchar_t wszCaption[256];
        swprintf_s(wszCaption, _countof(wszCaption), L"%d:EmptyProject [����GPLUS�ɹ�]", GetCurrentProcessId());
        SetWindowTextW(DXUTGetHWND(), wszCaption);
    }	

    // Start the render loop
    DXUTMainLoop();

    /* �ͷ�GPLUS */
    UnloadGPLUS();


    /* �ͷ�IGW */
    CloseIGW();


    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
    case IDC_BUTTON_LOGOUT:
        g_SDOAApp->Logout();
        g_GPlusApp->Logout();
        g_UI.SetVisible(false);
        g_SDOAApp->ShowLoginDialog(OnLogin, 0, 0);
        break;

    }
}


