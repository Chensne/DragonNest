//-----------------------------------------------------------------------------
// File: Lights.cpp
//
// Desc: Rendering 3D geometry is much more interesting when dynamic lighting
//       is added to the scene. To use lighting in D3D, you must create one or
//       lights, setup a material, and make sure your geometry contains surface
//       normals. Lights may have a position, a color, and be of a certain type
//       such as directional (light comes from one direction), point (light
//       comes from a specific x,y,z coordinate and radiates in all directions)
//       or spotlight. Materials describe the surface of your geometry,
//       specifically, how it gets lit (diffuse color, ambient color, etc.).
//       Surface normals are part of a vertex, and are needed for the D3D's
//       internal lighting calculations.
//
// Copyright (c) 2000-2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
// If app hasn't choosen, set to work with Windows 98, Windows Me, Windows 2000, Windows XP and beyond

#if _MSC_VER == 1600
#define POINTER_64 __ptr64
#endif

#include <Windows.h>
#include <d3dx8.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <strsafe.h>        // StringCch相关用

void CALLBACK OnLostDevice();

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER8 g_pVB        = NULL; // Buffer to hold vertices
D3DPRESENT_PARAMETERS   d3dpp;
bool					g_DeviceLost = false;

// A structure for our custom vertex type. We added a normal, and omitted the
// color (which is provided by the material)
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The 3D position for the vertex
    D3DXVECTOR3 normal;   // The surface normal for the vertex
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)

#include <GPLUSDx8.h>
#include <GPLUSClient.h>

#include <SDOA4Client.h>
#include <SDOADx8.h>

HINSTANCE          g_hInstGPLUSDll = 0;
LPGPlusInitialize    g_pInitialize = NULL;
LPGPlusGetModule     g_pGetModule = NULL;
LPGPlusTerminal      g_pTerminal = NULL;


IGPLUSApp*			g_GPlusApp = NULL;
IGPLUSDx8*			g_GPlusDx8 = NULL;

HINSTANCE g_hInstIGWDll  = 0;
LPigwInitialize    igwInitialize = NULL;
LPigwGetModule     igwGetModule = NULL;
LPigwTerminal      igwTerminal = NULL;

ISDOADx8*			g_SDOADx8 = NULL;
ISDOAApp*			g_SDOAApp = NULL;

void CALLBACK OnResetDevice()           ;


/* 释放GPLUS */
void UnloadGPLUS()
{
    g_GPlusDx8 = NULL;
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
                    GPLUS_RENDERTYPE_D3D8,      // 客户端支持的图形引擎类型
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
                    g_pGetModule(__uuidof(IGPLUSDx8), (void**)&g_GPlusDx8);

                    //注意:下面这一段代码,应该在游戏角色登录成功再调用,这里仅作演示用
                    if ( g_GPlusDx8 && g_GPlusApp)
                    {
                        GPlusUserInfo tmpRoleInfo = { sizeof(GPlusUserInfo),
                            L"数字id" , 
                            L"角色名"} ;

                        g_GPlusApp->Login(&tmpRoleInfo);
                    }
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
    if (g_SDOADx8)
    {
        g_SDOADx8->Release();
        g_SDOADx8 = NULL;
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
                    igwGetModule(__uuidof(ISDOADx8), (void**)&g_SDOADx8);
                    igwGetModule(__uuidof(ISDOAApp), (void**)&g_SDOAApp);

                    if ( (!g_SDOADx8) || (!g_SDOAApp))
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
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Get the current desktop display mode, so we can set up a back
    // buffer of the same format
    D3DDISPLAYMODE d3ddm;
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    // d3dpp.BackBufferWidth = 0;
    // d3dpp.BackBufferHeight = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.hDeviceWindow = hWnd;// 注意GPLUS内部总是通过它取到游戏主窗口的

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
    // Create the vertex buffer.
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
        0, D3DFVF_CUSTOMVERTEX,
        D3DPOOL_DEFAULT, &g_pVB ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer. We are algorithmically generating a cylinder
    // here, including the normals, which are used for lighting.
    CUSTOMVERTEX* pVertices;
    if( FAILED( g_pVB->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
        return E_FAIL;
    for( DWORD i=0; i<50; i++ )
    {
        FLOAT theta = (2*D3DX_PI*i)/(50-1);
        pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
        pVertices[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
        pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
        pVertices[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
    }
    g_pVB->Unlock();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pVB != NULL )
        g_pVB->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just leave it as the identity
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationX( &matWorld, timeGetTime()/500.0f );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-5.0f ),
        &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
        &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}




//-----------------------------------------------------------------------------
// Name: SetupLights()
// Desc: Sets up the lights and materials for the scene.
//-----------------------------------------------------------------------------
VOID SetupLights()
{
    // Set up a material. The material here just has the diffuse and ambient
    // colors set to yellow. Note that only one material can be used at a time.
    D3DMATERIAL8 mtrl;
    ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    g_pd3dDevice->SetMaterial( &mtrl );

    // Set up a white, directional light, with an oscillating direction.
    // Note that many lights may be active at a time (but each one slows down
    // the rendering of our scene). However, here we are just using one. Also,
    // we need to set the D3DRS_LIGHTING renderstate to enable lighting
    D3DXVECTOR3 vecDir;
    D3DLIGHT8 light;
    ZeroMemory( &light, sizeof(D3DLIGHT8) );
    light.Type       = D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;
    vecDir = D3DXVECTOR3(cosf(timeGetTime()/350.0f),
        1.0f,
        sinf(timeGetTime()/350.0f) );
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
    light.Range       = 1000.0f;
    g_pd3dDevice->SetLight( 0, &light );
    g_pd3dDevice->LightEnable( 0, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Finally, turn on some ambient light.
    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00202020 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // Clear the backbuffer and the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
        D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

    // Begin the scene
    g_pd3dDevice->BeginScene();

    // Setup the lights and materials
    SetupLights();

    // Setup the world, view, and projection matrices
    SetupMatrices();

    // Render the vertex buffer contents
    g_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

    if (g_GPlusDx8)
        g_GPlusDx8->RenderEx();

    if (g_SDOADx8)
        g_SDOADx8->RenderEx();

    // End the scene
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
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

            if ( g_GPlusDx8 && g_GPlusApp)
            {
                wchar_t ID[256]={0};
                MultiByteToWideChar(CP_ACP, 0, pLoginResult->szSndaid, -1, ID, _countof(ID));
                GPlusUserInfo gplustmpRoleInfo = { sizeof(GPlusUserInfo),
                    ID , 
                    sdoatmpRoleInfo.pwcsRoleName} ;

                g_GPlusApp->Login(&gplustmpRoleInfo);

                // g_UI.SetVisible(true);
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


//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if(g_GPlusDx8) 
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
        LRESULT lResult = 0; 
        if (g_GPlusApp->OnWindowProc( hWnd, msg, wParam, lParam, &lResult ) == GPLUS_OK ) 
        { 			
            return lResult; 
        } 
    } 
    if(g_SDOADx8) 
    {	// 把消息交给IGW模块处理 
        LRESULT lResult; 
        if (g_SDOADx8->OnWindowProc( hWnd, msg, wParam, lParam, &lResult ) == SDOA_OK ) 
        { 
            return lResult; 
        } 
    } 
    if ( (UINT)SDOA_WM_CLIENT_RUN == msg )
    {
        // 此刻可以开始登录
        if (g_SDOAApp)
        {
            g_SDOAApp->ShowLoginDialog(OnLogin, 0, 0);
        }
    }
    switch( msg )
    {
    case WM_SIZE:
        {
            OnLostDevice();
            g_pd3dDevice->Reset(&d3dpp);
            OnResetDevice();
        }
        break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );

}

void CALLBACK OnLostDevice()
{
    /* 通知d3d设备丢失 */
    if( g_pVB != NULL )
    {
        g_pVB->Release();
        g_pVB = NULL;
    }
    if (g_GPlusDx8)
        g_GPlusDx8->OnDeviceLost();

    if (g_SDOADx8)
        g_SDOADx8->OnDeviceLost();
}

void CALLBACK OnResetDevice()
{
    /* 通知d3d设备重置成功 */
    InitGeometry();
    // Turn off culling
    g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    //     // Turn on the zbuffer
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

    if (g_GPlusDx8)
        g_GPlusDx8->OnDeviceReset(&d3dpp);

    if (g_SDOADx8)
        g_SDOADx8->OnDeviceReset(&d3dpp);
}


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //////////////////////////////////////////////////////////////////////////


    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle(NULL), NULL, LoadCursor( NULL, IDC_ARROW ), NULL, NULL,
        L"D3D Tutorial", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 04: Lights",
        WS_OVERLAPPEDWINDOW, 100, 100, 800, 600,
        GetDesktopWindow(), NULL, wc.hInstance, NULL );


    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        /* 加载GPLUS */
        LoadGPLUS();

        /* 加载IGW */
        OpenIGW();

        if (g_pInitialize)
        {	// 说明加载成功，表示下......			
            SetWindowText(hWnd, L"D3D Tutorial [加载GPLUS组件成功]");
        }	
        if (g_GPlusDx8)
        {	// 初始化GPLUS内部图形引擎
            g_GPlusDx8->Initialize(g_pd3dDevice, &d3dpp, false);
        }
        if (g_SDOADx8)
        {
            g_SDOADx8->Initialize(g_pd3dDevice, &d3dpp, false);
        }

        // Create the geometry
        if( SUCCEEDED( InitGeometry() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof(msg) );
            while( msg.message!=WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );                    
                    DispatchMessage( &msg );
                }
                else
                {	// 简单处理设备丢失问题
                    if (g_DeviceLost)
                    {	// 处于设备丢失中可慢点恢复，避免长时间丢失占用过多CPU
                        Sleep(50);
                    }
                    switch (g_pd3dDevice->TestCooperativeLevel())
                    {
                    case D3D_OK:
                        {	// 
                            Render();
                            g_DeviceLost = false;
                            break;
                        }

                    case D3DERR_DEVICELOST:
                        {
                            if ( !g_DeviceLost )
                            {
                                OnLostDevice();
                                g_DeviceLost = true;
                            }							
                            break;
                        }
                    case D3DERR_DEVICENOTRESET:
                        {
                            if ( g_pd3dDevice->Reset( &d3dpp ) == D3D_OK )
                            {
                                OnResetDevice();
                                g_DeviceLost = false;
                            }
                            break;
                        }
                    
                    }                    
                }
            }
        }
    }

    if (g_GPlusDx8)
    {	// 释放GPLUS内部图形引擎
        g_GPlusDx8->Finalize();
    }
    if (g_SDOADx8)
    {
        g_SDOADx8->Finalize();
    }
    /* 释放GPLUS */
    UnloadGPLUS();

    /* 释放IGW */
    CloseIGW();

    // Clean up everything and exit the app
    Cleanup();
    UnregisterClass( L"D3D Tutorial", wc.hInstance );

    return 0;
}



