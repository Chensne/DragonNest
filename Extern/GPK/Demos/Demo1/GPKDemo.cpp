#include <d3d9.h>
#include <D3dx9core.h>
#pragma warning( disable : 4996 )
#include <strsafe.h>
#pragma warning( default : 4996 ) 

// ����GPK�ͻ���ͷ�ļ�
#include <GPKitClt.h>

// ����GPK�ͻ��˾�̬��
#pragma comment(lib, "GPKitClt.lib")

// ����libc.lib
#pragma comment(linker, "/NODEFAULTLIB:libc.lib")

// ʹ��GPK�����ռ�
using namespace SGPK;

LPDIRECT3D9             g_pD3D       = NULL; 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
ID3DXFont*              g_pFont      = NULL;
TCHAR                   g_strFont[LF_FACESIZE];

HRESULT InitD3D( HWND hWnd )
{
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    int nHeight = -12 * 80 / 72;

    StringCchCopy( g_strFont, 32, "Arial" );

    D3DXCreateFont( g_pd3dDevice,            // D3D device
        nHeight,               // Height
        0,                     // Width
        FW_BOLD,               // Weight
        1,                     // MipLevels, 0 = autogen mipmaps
        FALSE,                 // Italic
        DEFAULT_CHARSET,       // CharSet
        OUT_DEFAULT_PRECIS,    // OutputPrecision
        DEFAULT_QUALITY,       // Quality
        DEFAULT_PITCH | FF_DONTCARE, // PitchAndFamily
        g_strFont,              // pFaceName
        &g_pFont);              // ppFont

    return S_OK;
}

VOID Cleanup()
{
    if (g_pFont != NULL)
        g_pFont->Release();

    if( g_pd3dDevice != NULL) 
        g_pd3dDevice->Release();

    if( g_pD3D != NULL)
        g_pD3D->Release();
}

VOID Render()
{
    if( NULL == g_pd3dDevice )
        return;

    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
    
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        RECT rc;

        SetRect( &rc, 0, 0, 0, 0 );  
        g_pFont->DrawText( NULL, "GAME PROTECT KIT DEMO", -1, NULL, DT_CENTER | DT_VCENTER, 
            D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ));

        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;

        case WM_PAINT:
            Render();
            ValidateRect( hWnd, NULL );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //
    // ����GPK��������ʼ
    // NOTE: GPKStart����Ҫ����Ϣѭ������֮ǰִ��
    //
    if ( !GPKStart("http://127.0.0.1/Updates/TestGame", "GPKDEMO") )
    {
        MessageBox(0, "GPK ����ʧ��!", "ʧ��", 0);
        return -1;
    }

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "GPK Demo", NULL };
    RegisterClassEx( &wc );

    HWND hWnd = CreateWindow( "GPK Demo", "GPK Demo", 
                              WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
                              NULL, NULL, wc.hInstance, NULL );

    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );

        MSG msg; 
        while( GetMessage( &msg, NULL, 0, 0 ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    UnregisterClass( "GPK Demo", wc.hInstance );
    return 0;
}



