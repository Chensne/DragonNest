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


/* 释放GPLUS */
void UnloadGPLUS()
{
    g_GPlusDx9 = NULL;
    g_GPlusApp = NULL;

    g_pInitialize = NULL;
    if (g_pTerminal)
    {  // 释放DLL前总是先调用gpTerminal
        g_pTerminal();
        g_pTerminal = NULL;
    }
    if (g_hInstGPLUSDll)
    {
        FreeLibrary(g_hInstGPLUSDll);
        g_hInstGPLUSDll = NULL;
    }
}

/* 加载GPLUS */
void LoadGPLUS()
{
    WCHAR strExePath[MAX_PATH] = {0};
    WCHAR strExeName[MAX_PATH] = {0};
    WCHAR* strLastSlash = NULL;
    GetModuleFileNameW( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH-1]=0;
    strLastSlash = wcsrchr( strExePath, TEXT('\\') );
    if( strLastSlash )
    {	// 得到EXE所在路径
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
    {	// 找到组件则加载	
        g_hInstGPLUSDll = LoadLibrary(strGameWidgetDll);
        if (g_hInstGPLUSDll)
        {
            //获取导出函数
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

                // 注意GPlusAppInfo类型所有字段必须填
                GPlusAppInfo tmpAppInfo = {
                    sizeof(GPlusAppInfo),       // 结构体大小
                    appId,						// 接入应用ID，从开发者网站中申请
                    L"龙潭虎穴OnLine",          // 应用名称
                    L"1.0.0.1",
                    GPLUS_RENDERTYPE_D3D9,      // 客户端支持的图形引擎类型
                    0,
                    1,
                    2,
                };

                //初始化
                if (g_pInitialize(GPLUS_SDK_VERSION, &tmpAppInfo) == GPLUS_OK)
                {	// 初始化组件成功

                    bLoadOk = TRUE;

                    // 获取组件接口	
                    g_pGetModule(__uuidof(IGPLUSApp), (void**)&g_GPlusApp);
                    g_pGetModule(__uuidof(IGPLUSDx9), (void**)&g_GPlusDx9);

                    /* 注意:下面这一段代码,应该在游戏角色登录成功再调用,这里仅作演示用
                    if ( g_GPlusDx9 && g_GPlusApp)
                    {
                    GPlusUserInfo tmpRoleInfo = { sizeof(GPlusUserInfo),
                    L"数字id" , 
                    L"角色名"} ;

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

/* 释放IGW */
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
    {  // 释放DLL前总是先调用igwTerminal
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

/* 加载IGW */
void OpenIGW()
{
    WCHAR strExePath[MAX_PATH] = {0};
    WCHAR strExeName[MAX_PATH] = {0};
    WCHAR* strLastSlash = NULL;
    GetModuleFileNameW( NULL, strExePath, MAX_PATH );
    strExePath[MAX_PATH-1]=0;
    strLastSlash = wcsrchr( strExePath, TEXT('\\') );
    if( strLastSlash )
    {	// 得到EXE所在路径
        StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
        *strLastSlash = 0;
        strLastSlash = wcsrchr( strExeName, TEXT('.') );
        if( strLastSlash )
            *strLastSlash = 0;
    }

    WCHAR strGameWidgetDll[MAX_PATH] = {0};
    StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\GameWidget.dll", strExePath );

    if ( !(GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF) )
    { // 如果程序运行路径中不存在则换个路径再试
        StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\..\\..\\..\\..\\bin\\GameWidget.dll", strExePath );
    }

    CloseIGW();

    if ( GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF )
    {	// 找到组件则加载	
        g_hInstIGWDll = LoadLibraryW(strGameWidgetDll);
        if (g_hInstIGWDll)
        {
            igwInitialize = (LPigwInitialize)GetProcAddress( g_hInstIGWDll, "igwInitialize" );
            igwGetModule = (LPigwGetModule)GetProcAddress( g_hInstIGWDll, "igwGetModule" );
            igwTerminal = (LPigwTerminal)GetProcAddress( g_hInstIGWDll, "igwTerminal" );
            if ( igwInitialize && igwGetModule && igwTerminal)
            {
                // 加载成功

                // 注意AppInfo类型所有字段必须填
                AppInfo tmpAppInfo = {
                    sizeof(AppInfo),       // 结构体大小，方便扩展
                    9999,                  // 接入应用ID，从开发者网站中申请
                    L"龙潭虎穴OnLine",     // 应用名称
                    L"0.1.2.0",            // 应用客户端当前版本号
                    SDOA_RENDERTYPE_D3D9,  // 客户端支持的图形引擎类型，这里同时支持d3d9和d3d8
                    1,                     // 可同时在一台机器上游戏的最大人数（通常为1，例如:KOF应该是2）
                    -1,                    // 游戏区ID，不可用时传入-1
                    -1                     // 游戏组ID，不可用时传入-1

                };

                if (igwInitialize(SDOA_SDK_VERSION, &tmpAppInfo) == SDOA_OK)
                {	// 初始化组件成功

                    // 获取组件接口
                    igwGetModule(__uuidof(ISDOADx9), (void**)&g_SDOADx9);
                    igwGetModule(__uuidof(ISDOAApp), (void**)&g_SDOAApp);

                    if ( (!g_SDOADx9) || (!g_SDOAApp))
                        CloseIGW();

                }
                else // 初始化失败清除环境
                    CloseIGW();
            }
            else  // 加载DLL失败清除环境
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
    {	// 初始化GPLUS内部图形引擎
        D3DPRESENT_PARAMETERS d3dpp = DXUTGetD3D9PresentParameters ();
        g_GPlusDx9->Initialize( pd3dDevice, &d3dpp, FALSE );
    }
    if (g_SDOADx9)
    {	// 初始化IGW内部图形引擎
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
    {	// 通知GPLUS d3d设备重置成功
        D3DPRESENT_PARAMETERS d3dpp = DXUTGetD3D9PresentParameters () ;	
        g_GPlusDx9->OnDeviceReset ( &d3dpp ) ;
    }
    if(g_SDOADx9)
    {	// 通知IGW d3d设备重置成功
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
        /* 绘制GPLUS界面 */
        if (g_GPlusDx9)
            g_GPlusDx9->RenderEx();

        /* 绘制IGW界面 */
        if (g_SDOADx9)
            g_SDOADx9->RenderEx();

        V( g_UI.OnRender( fElapsedTime ) );
        V( pd3dDevice->EndScene() );
    }
}

BOOL isSDOLogin = false; // 注意一定要是BOOL类型，不能为bool否则可能会导致问题
LoginResult loginResult;

BOOL CALLBACK OnLogin(int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved) 
{ // 注意该回调返回true则关闭登录对话框，否则登录对话框仍然显示

    isSDOLogin = (SDOA_ERRORCODE_OK == nErrorCode); 
    if (isSDOLogin) 
    { 
        loginResult = *pLoginResult; 

        // 登录成功后,把用户使用的游戏角色信息传入IGW
        if (g_SDOAApp)
        {
            RoleInfo sdoatmpRoleInfo = { sizeof(RoleInfo), L"美貌仙子", 0 } ;
            g_SDOAApp->SetRoleInfo(&sdoatmpRoleInfo);  // 设置角色信息，可多次调用，只要用户切换了角色就调用通知IGW

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
    { // 登录错误消息处理
        if (nReserved != 0)
        {
            LoginMessage* pCurLoginMsg = (LoginMessage*)nReserved;
            if (pCurLoginMsg->dwSize >= sizeof(LoginMessage))
            { // 兼容性处理 
                // 修改错误提示对话框标题
                //SysReAllocString(pCurLoginMsg->pbstrTitle, L"[测试游戏错误提示]");

                // 此时返回FALSE则使用默认提示

                // 此时返回TRUE则使用自定义的提示
                //isSDOLogin = TRUE;
                //::MessageBox(0, *pCurLoginMsg->pbstrContent, *pCurLoginMsg->pbstrTitle, MB_OK);
            }
        }
    }
    return isSDOLogin; // 常规逻辑，登录成功则关闭登录对话框
} 


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         bool* pbNoFurtherProcessing, void* pUserContext )
{
    if(g_GPlusDx9) 
    {	// 把消息交给GPLUS模块处理 
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
            *pbNoFurtherProcessing = true;// 不让自身再处理消息 
            return lResult; 
        } 
    } 
    if(g_SDOADx9) 
    {	// 把消息交给IGW模块处理 
        LRESULT lResult; 
        if (g_SDOADx9->OnWindowProc( hWnd, uMsg, wParam, lParam, &lResult ) == SDOA_OK ) 
        { 
            *pbNoFurtherProcessing = true;// 不让自身再处理消息 
            return lResult; 
        } 
    } 
    if ( (UINT)SDOA_WM_CLIENT_RUN == uMsg )
    {
        // 此刻可以开始登录
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
    /* 通知GPLUS d3d设备丢失 */
    if(g_GPlusDx9)
        g_GPlusDx9->OnDeviceLost();
    /* 通知IGW d3d设备丢失 */
    if(g_SDOADx9)
        g_SDOADx9->OnDeviceLost();

    g_DialogResourceManager.OnD3D9LostDevice();
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    /* 释放GPLUS图形引擎 */
    if (g_GPlusDx9)
        g_GPlusDx9->Finalize();
    /* 释放IGW图形引擎 */
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

    /* 加载GPLUS */
    LoadGPLUS();

    /* 加载IGW */
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
    g_UI.AddButton( IDC_BUTTON_LOGOUT, L"注销", 200, 10, 125, 22 );
    g_UI.SetVisible(false);

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"EmptyProject_dx9" );
    DXUTCreateDevice( true, 800, 600 );

    if (g_pInitialize)
    {	// 加载成功
        wchar_t wszCaption[256];
        swprintf_s(wszCaption, _countof(wszCaption), L"%d:EmptyProject [加载GPLUS成功]", GetCurrentProcessId());
        SetWindowTextW(DXUTGetHWND(), wszCaption);
    }	

    // Start the render loop
    DXUTMainLoop();

    /* 释放GPLUS */
    UnloadGPLUS();


    /* 释放IGW */
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


