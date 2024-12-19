#include "StdAfx.h"
#include "EtDevice.h"
#include <direct.h>
#include "EtStateManager.h"
#include "EtSystemFont.h"
#include "EternityEngine/EternityEngine.h"
#include "EtTexture.h"
//#include "..\\..\\..\\Client\\DragonNest\\LogWnd.h"

//#define USE_PERF_TEST

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtDevice g_EtDevice;
DWORD g_dwTextureColorKey = 0;

CEtDevice::CEtDevice()
{
	m_pD3D = NULL;
	m_pDD = NULL;
	m_pDevice = NULL;	

	m_pBackBufferSurface = NULL;
	m_pDepthStencilSurface = NULL;
	m_pFont = NULL;

	m_pCurSetRenderTarget = NULL;
	m_pCurSetDepthStencil = NULL;

	m_bShaderDebug = false;	
	m_SupportedAntiAliasType = MULTISAMPLE_NONE;

	m_pOutOfMemoryCallBack = &FnDeviceOutOfMemory();

	m_vecVertexDecl.reserve( 256 );

#ifdef PRE_MOD_MEMORY_CHECK
	m_dwLocalVideoMemory = 0;
#else
	m_bUseDDrawMemCheck = true;
#endif
	m_dwTotalAvailTextureMemory = 0;

	m_pPrePresentCallback = NULL;

	m_nCurrentPolyCount = 0;
	m_nPolyCountPerFrame = 0;
	m_bCallOutOfMemory = false;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	m_pFlushWaitDeleteCallback = NULL;
#endif
	m_hWnd = 0;
	m_pSystemFont = NULL;
	memset( &m_Param, 0, sizeof( D3DPRESENT_PARAMETERS ) );
}

CEtDevice::~CEtDevice()
{	
	Clear();
}

void CEtDevice::Clear()
{
	ClearEffectMacro();

	for( std::vector< VertexDecl >::iterator it = m_vecVertexDecl.begin(); it != m_vecVertexDecl.end(); ++it ) {
		SAFE_RELEASE( it->pDecl );
	}
	m_vecVertexDecl.clear();

	SAFE_RELEASE( m_pFont );
	SAFE_RELEASE( m_pBackBufferSurface );
	SAFE_RELEASE( m_pDepthStencilSurface );

	SAFE_RELEASE( m_pDevice );
	SAFE_RELEASE( m_pD3D );	
	SAFE_RELEASE( m_pDD );

	m_vecApplyQualityPath.clear();
}

void CEtDevice::CreateDDraw()
{
	ScopeLock<CSyncLock> Lock(m_DeviceLostLock);
	SAFE_RELEASE( m_pDD );
	DirectDrawCreateEx( NULL, ( VOID** )&m_pDD, IID_IDirectDraw7, NULL );
	ADD_D3D_RES( m_pDD );
}

void CEtDevice::EnumDisplayMode()
{
	int i, nCount;
	D3DDISPLAYMODE DisplayMode;
	m_vecDisplayMode.clear();

	nCount = m_pD3D->GetAdapterModeCount( 0, D3DFMT_X8R8G8B8 );
	for( i = 0; i < nCount; i++ )
	{
		m_pD3D->EnumAdapterModes( 0, D3DFMT_X8R8G8B8, i, &DisplayMode );
		if( ( DisplayMode.Height / ( float )DisplayMode.Width ) > 0.5f )	// 16 : 9 보다 더 작은 비율 해상도는 제외한다.
		{
			m_vecDisplayMode.push_back( DisplayMode );
		}
	}
}

void CEtDevice::FindSuitableDisplayMode( int &nWidth, int &nHeight )
{
	int i;
	std::vector< int > vecSuitableDisplayMode;
	bool bWide = false;;

	if( nHeight / ( float )nWidth < 0.7f )
	{
		bWide = true;
	}

	for( i = 0; i < ( int )m_vecDisplayMode.size(); i++ )
	{
		if( bWide ) 
		{
			if( m_vecDisplayMode[ i ].Height / ( float )m_vecDisplayMode[ i ].Width < 0.7f )
			{
				vecSuitableDisplayMode.push_back( i );
			}
		}
		else
		{
			if( m_vecDisplayMode[ i ].Height / ( float )m_vecDisplayMode[ i ].Width > 0.7f )
			{
				vecSuitableDisplayMode.push_back( i );
			}
		}
	}
	if( vecSuitableDisplayMode.empty() )
	{
		for( i = 0; i < ( int )m_vecDisplayMode.size(); i++ )
		{
			vecSuitableDisplayMode.push_back( i );
		}
	}

	for( i = ( int )vecSuitableDisplayMode.size() - 1; i >= 0; i-- )
	{
		if( ( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Width == nWidth ) && ( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Height == nHeight ) )
		{
			return;
		}
	}

	int nNearIndex, nMinValue;
	nMinValue = INT_MAX;
	nNearIndex = -1;
	for( i = ( int )vecSuitableDisplayMode.size() - 1; i >= 0; i-- )
	{
		if( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Width == nWidth )
		{
			if( abs( ( int )( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Height - nHeight ) ) < nMinValue )
			{
				nNearIndex = i;
				nMinValue = abs( ( int )( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Height - nHeight ) );
			}
		}
	}
	if( nNearIndex == -1 )
	{
		for( i = ( int )vecSuitableDisplayMode.size() - 1; i >= 0; i-- )
		{
			if( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Height == nHeight )
			{
				if( abs( ( int )( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Width - nWidth ) ) < nMinValue )
				{
					nNearIndex = i;
					nMinValue = abs( ( int )( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Width - nWidth ) );
				}
			}
		}
	}

	if( nNearIndex == -1 )
	{
		nWidth = MIN_RES_WIDTH;
		for( i = 0; i < ( int )vecSuitableDisplayMode.size(); i++ )
		{
			if( ( int )( m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Width * m_vecDisplayMode[ vecSuitableDisplayMode[ i ] ].Height ) >= nWidth * nHeight )
			{
				nNearIndex = i;
				break;
			}
		}
	}

	if( nNearIndex == -1 )
	{
		nNearIndex = ( int )vecSuitableDisplayMode.size() - 1;
	}

	if( nNearIndex == -1 ) {
		nWidth = MIN_RES_WIDTH;
		nHeight = MIN_RES_HEIGHT;
	}
	else {
		nWidth = m_vecDisplayMode[ vecSuitableDisplayMode [ nNearIndex ] ].Width;
		nHeight = m_vecDisplayMode[ vecSuitableDisplayMode[ nNearIndex ] ].Height;
	}
}

int CEtDevice::FindSuitableRefreshRate( int nWidth, int nHeight )
{
	int nResultRefreshRate = 0;
	const int nTargetRefreshRate = 60;
	for( int i = 0; i < ( int )m_vecDisplayMode.size(); i++ )
	{
		if( m_vecDisplayMode[i].Width == nWidth && m_vecDisplayMode[i].Height == nHeight ) {
			if( abs((int)m_vecDisplayMode[i].RefreshRate - nTargetRefreshRate) < abs(nResultRefreshRate - nTargetRefreshRate) ) {
				nResultRefreshRate = m_vecDisplayMode[i].RefreshRate;
			}
		}
	}
	return nResultRefreshRate;
}

int CEtDevice::Initialize( HWND hWnd, int nWidth, int nHeight, bool bWindow, bool bEnableMultiThread, bool bVSync, bool bEnableShaderDebug )
{
LabelRetry:
	m_hWnd = hWnd;
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_Caps);	
	ADD_D3D_RES( m_pD3D );

	EnumDisplayMode();
	if( bWindow == false )
	{
		FindSuitableDisplayMode( nWidth, nHeight );
	}

	m_AdapterFormat = D3DFMT_X8R8G8B8;
	memset( &m_Param, 0, sizeof( D3DPRESENT_PARAMETERS ) );
	m_Param.BackBufferWidth = nWidth;
	m_Param.BackBufferHeight = nHeight;
	m_Param.BackBufferFormat = m_AdapterFormat;
	m_Param.BackBufferCount = 1;
	m_Param.EnableAutoDepthStencil = true;
	m_Param.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_Param.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_Param.Windowed = bWindow;
	if( bWindow )
	{
		m_Param.FullScreen_RefreshRateInHz = 0;
	}
	else
	{
		m_Param.FullScreen_RefreshRateInHz = FindSuitableRefreshRate( nWidth, nHeight);
	}
	SetVSync( bVSync );
	m_Param.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	DWORD dwFlags;

	m_DevType = D3DDEVTYPE_HAL;
	dwFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	if( (m_Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || m_Caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		dwFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	if( bEnableMultiThread )
	{
		dwFlags |= D3DCREATE_MULTITHREADED;
	}
	if( bEnableShaderDebug )
	{
		m_bShaderDebug = true;
		m_DevType = D3DDEVTYPE_REF;
		dwFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	bool bProfileMode = false;
#ifndef _FINAL_BUILD
	char* lpCommandLine = ::GetCommandLine();
	if(strstr(lpCommandLine, "/perf")) 
	{
		bProfileMode = true;
	}
#endif
#ifdef USE_PERF_TEST
	bProfileMode = true;
#endif

	if( bProfileMode ) 
	{
		m_nAdapter = D3DADAPTER_DEFAULT;
		m_DevType = D3DDEVTYPE_HAL;
		// Look for 'NVIDIA PerfHUD' adapter
		// If it is present, override default settings
		for (UINT Adapter=0;Adapter<m_pD3D->GetAdapterCount();Adapter++)
		{
			D3DADAPTER_IDENTIFIER9 Identifier;
			HRESULT Res;
			Res = m_pD3D->GetAdapterIdentifier(Adapter,0,&Identifier);
			if (strstr(Identifier.Description,"PerfHUD") != 0)
			{
				m_nAdapter = Adapter;
				m_DevType = D3DDEVTYPE_REF;
				break;
			}
		}		
		if( FAILED( m_pD3D->CreateDevice( m_nAdapter, m_DevType, hWnd, dwFlags, &m_Param, &m_pDevice ) ) ) 
		{
			ASSERT( 0 && "CreateDevice() Failed!!" );
			return ETERR_CREATEDEVICEFAIL;
		}
		ADD_D3D_RES( m_pDevice );
	}
	else 
	{
		m_nAdapter = D3DADAPTER_DEFAULT;
		HRESULT hr;
		if( FAILED( hr = m_pD3D->CreateDevice( m_nAdapter, m_DevType, hWnd, dwFlags, &m_Param, &m_pDevice ) ) ) 
		{
			static int nRetryStep = 5;	// 디바이스 생성 실패시 다섯번 정도 재시도한다...
			if( nRetryStep-- > 0 ) {
				SAFE_RELEASE( m_pD3D );
				Sleep( 100 );
				goto LabelRetry;
			}
			//if( hr == D3DERR_DEVICELOST ) {
			ASSERT( 0 && "CreateDevice() Failed!!" );
			return ETERR_CREATEDEVICEFAIL;
			//}
		}
		ADD_D3D_RES( m_pDevice );
	}

	m_pDevice->GetDeviceCaps( &m_Caps );

	m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface );	
	m_pDevice->GetDepthStencilSurface( &m_pDepthStencilSurface );	

	GetEtStateManager()->Initialize( this );

	ADD_D3D_RES( m_pBackBufferSurface );
	ADD_D3D_RES( m_pDepthStencilSurface );

	EtMultiSampleType SampleTypes[] = { MULTISAMPLE_8_SAMPLES, MULTISAMPLE_4_SAMPLES, MULTISAMPLE_2_SAMPLES};
	for ( int i = 0; i < _countof(SampleTypes); i++) {		
		HRESULT resultFullScreen = m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, FALSE, (D3DMULTISAMPLE_TYPE)SampleTypes[i], NULL);
		HRESULT resultWindowed = m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, TRUE, (D3DMULTISAMPLE_TYPE)SampleTypes[i], NULL);
		HRESULT resultDepthFullScreen = m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D24S8, FALSE, (D3DMULTISAMPLE_TYPE)SampleTypes[i], NULL);
		HRESULT resultDepthWindowed = m_pD3D->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D24S8, TRUE, (D3DMULTISAMPLE_TYPE)SampleTypes[i], NULL);
		if( resultFullScreen == D3D_OK && resultWindowed == D3D_OK &&
			resultDepthFullScreen == D3D_OK && resultDepthWindowed == D3D_OK ) {
				m_SupportedAntiAliasType = SampleTypes[i];
				break;
		}
	}

	SetClipCursor( !m_Param.Windowed );

	CreateDDraw();
	GetEtStateManager()->Reset();
	SetDefaultState();

	// 검은색으로 최초로 한번 지워준다.
	ClearBuffer( 0x00000000 );	
	ShowFrame();

#ifdef PRE_MOD_MEMORY_CHECK
	CheckTotalLocalVideoMemory();
#else
	m_dwTotalAvailTextureMemory = m_pDevice->GetAvailableTextureMem( );
#endif

	return ET_OK;
}

void CEtDevice::SetVSync( bool bVSync )
{
	m_Param.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if( bVSync && m_Caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE )
	{
		m_Param.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
}

bool CEtDevice::IsOnlyLowShaderAvailable( void )
{
	if( NULL == m_pD3D )
		return false;

	// 인텔 G31/G33 카드같은 구린 것들인지 확인
	// GMA X3000 이상은 괜찮을 듯.
	bool bIsOnlyLowShaderAvailable = false;

	D3DCAPS9 Caps;					  // Device CAPs structure  
	D3DADAPTER_IDENTIFIER9 AdapterID; // Used to store device info  

	// Retrieve device capabilities  
	if( m_pD3D->GetDeviceCaps( 0, D3DDEVTYPE_HAL, &Caps ) != D3D_OK )  
	{ 
		return true;		// caps 도 못 얻어오나..
	} 

	// Check vendor and device ID and enable software vertex  
	// processing for Intel(R) Graphics...  
	// Gather the primary adapter's information...  
	if( m_pD3D->GetAdapterIdentifier(0,0,&AdapterID ) != D3D_OK )  
	{ 
		return true;		// 아답터 식별자도 못 얻어오나..
	}

	// 915, 945, G31/G33, Q35  이놈들은 하옵의 두장짜리 스플래팅 지형 쉐이더 같은 거 못돌림.
	// Intel Architecture 
	if( AdapterID.VendorId == 0x8086 )
	{
		if(	//( AdapterID.DeviceId == 0x2A02 ) ||  // GM965 Device 0 
			//( AdapterID.DeviceId == 0x2A03 ) ||  // GM965 Device 1 
			//( AdapterID.DeviceId == 0x29A2 ) ||  // G965 Device 0 
			//( AdapterID.DeviceId == 0x29A3 ) ||  // G965 Device 1 
			( AdapterID.DeviceId == 0x27A2 ) ||  // 945GM Device 0 
			( AdapterID.DeviceId == 0x27A6 ) ||  // 945GM Device 1 
			( AdapterID.DeviceId == 0x2772 ) ||  // 945G Device 0 
			( AdapterID.DeviceId == 0x2776 ) ||  // 945G Device 1 
			( AdapterID.DeviceId == 0x2592 ) ||  // 915GM Device 0 
			( AdapterID.DeviceId == 0x2792 ) ||  // 915GM Device 1 
			( AdapterID.DeviceId == 0x2582 ) ||  // 915G Device 0 
			( AdapterID.DeviceId == 0x2782 ) ||  // 915G Device 1
			( AdapterID.DeviceId == 0x2972 ) ||  // 946GZ Device 0 
			( AdapterID.DeviceId == 0x2973 ) ||  // 946GZ Device 1
			//( AdapterID.DeviceId == 0x2992 ) ||  // Q965/Q963 Device 0 
			//( AdapterID.DeviceId == 0x2993 ) ||  // Q965/Q963 Device 1
			( AdapterID.DeviceId == 0x29b2 ) ||  // Q35 Device 0 
			( AdapterID.DeviceId == 0x29b3 ) ||  // Q35 Device 1
			( AdapterID.DeviceId == 0x29c2 ) ||  // G33/G31 Device 0 
			( AdapterID.DeviceId == 0x29c3 ) ||  // G33/G31 Device 1
			( AdapterID.DeviceId == 0x29d2 ) ||  // Q33 Device 0 
			( AdapterID.DeviceId == 0x29d3 )	 // Q33 Device 1
		  )
		{  
			bIsOnlyLowShaderAvailable = true;
		}
	}
	else
	{
		// #42934 인텔 내장 계열이 아닌경우 픽셀 쉐이더의 StaticFlowControlDepth 값이 있는 경우에
		// 저사양 스플래팅 쉐이더 돌리도록.
		if( 0 == Caps.PS20Caps.StaticFlowControlDepth )
			bIsOnlyLowShaderAvailable = true;
	}

	return bIsOnlyLowShaderAvailable;  
}

void CEtDevice::Reinitialize( int nWidth, int nHeight )
{
	if( m_pDevice )
	{
		SAFE_RELEASE( m_pBackBufferSurface );
		SAFE_RELEASE( m_pDepthStencilSurface );
		m_Param.BackBufferWidth = nWidth;
		m_Param.BackBufferHeight = nHeight;
		if( m_Param.Windowed ) {
			m_Param.FullScreen_RefreshRateInHz = 0;
		}
		else {
			m_Param.FullScreen_RefreshRateInHz = FindSuitableRefreshRate( nWidth, nHeight);
		}
		HRESULT hr = m_pDevice->Reset( &m_Param );
		if( FAILED(hr) ) {
			MessageBox( 0 ,"Device cannot be reset.", "DragonNest", MB_OK);
			PostQuitMessage( 0 );
		}
		m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface );		
		m_pDevice->GetDepthStencilSurface( &m_pDepthStencilSurface );
		ADD_D3D_RES( m_pBackBufferSurface );
		ADD_D3D_RES( m_pDepthStencilSurface );		
		GetEtStateManager()->Reset();	
		SetDefaultState();
	}
	CreateDDraw();

	SetClipCursor( !m_Param.Windowed );
}

void CEtDevice::SetDefaultState()
{
	GetEtStateManager()->SetDefaultState();

	m_pCurSetRenderTarget = m_pBackBufferSurface;
	m_pCurSetDepthStencil = m_pDepthStencilSurface;
}

void CEtDevice::ClearBuffer( D3DCOLOR COLOR, float fZ, DWORD dwStencil, bool bClearColor, bool bClearZ, bool bClearStencil )
{
	DWORD dwFlags = 0;
	if( bClearColor )
	{
		dwFlags |= D3DCLEAR_TARGET;
	}
	if( IsEnableZTest() )
	{		
		if( bClearZ )
		{
			dwFlags |= D3DCLEAR_ZBUFFER;
		}
		if( bClearStencil )
		{
			dwFlags |= D3DCLEAR_STENCIL;
		}
	}
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->Clear( 0, NULL, dwFlags, COLOR, fZ, dwStencil );
}

EtDeviceCheck CEtDevice::DeviceValidCheck()
{
	HRESULT hr;

	//if( m_Param.Windowed )
	{
		ScopeLock< CSyncLock > Lock( m_DeviceLock );
		hr = m_pDevice->TestCooperativeLevel();
		if( FAILED( hr ) )
		{
			if( hr == D3DERR_DEVICENOTRESET )
			{
				// 디바이스를 소실했지만, 복구가 가능한경우.
				return DC_CAN_RESET;
			}
			else if ( hr == D3DERR_DEVICELOST )
			{
				// 디바이스를 소실했고, 복구가 불가능 상태
				return DC_CANNOT_RESET;
			}
			else 
			{
				// 그외의 경우 무조건 복구 불가능이다.
				return DC_CANNOT_RESET;
			}
		}
	}

	return DC_OK;
}

void CEtDevice::SetRenderTarget( EtSurface *pSurface ) 
{ 
	if( pSurface == m_pCurSetRenderTarget ) {
		return;
	}
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetRenderTarget( 0 , pSurface );		// MRT 는 아직 지원안한다.
	m_pCurSetRenderTarget = pSurface;
}

void CEtDevice::SetDepthStencilSurface( EtSurface *pSurface ) 
{ 
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetDepthStencilSurface( pSurface ); 
	m_pCurSetDepthStencil = pSurface;
}

void CEtDevice::DumpBackBuffer( LPDIRECT3DSURFACE9 pSurface, LPDIRECT3DSURFACE9 pSourceSurface )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->StretchRect( (pSourceSurface == NULL) ? m_pBackBufferSurface : pSourceSurface , 
		NULL, pSurface, NULL, D3DTEXF_POINT );
}

void CEtDevice::SaveBackbuffer( const char *pFileName, D3DXIMAGE_FILEFORMAT Format )
{
	D3DXSaveSurfaceToFile( pFileName, Format, m_pBackBufferSurface, NULL, NULL );
}

void CEtDevice::SaveBackbuffer( WCHAR *pFileName, D3DXIMAGE_FILEFORMAT Format )
{
	D3DXSaveSurfaceToFileW( pFileName, Format, m_pBackBufferSurface, NULL, NULL );
}

int CEtDevice::FindVertexDeclaration( D3DVERTEXELEMENT9* pVertexElements )
{
	int i, j, nElementCount, nSrcCount;
	D3DVERTEXELEMENT9 *Decl;

	nSrcCount = 0;
	while( 1 )
	{
		if( pVertexElements[ nSrcCount ].Stream == 0xff )
		{
			break;
		}
		nSrcCount++;
	}

	nElementCount = 32;
	for( i = 0; i < ( int )m_vecVertexDecl.size(); i++ )
	{
		Decl = m_vecVertexDecl[ i ].Elem;
		nElementCount = m_vecVertexDecl[ i ].nElemCount;	

		if( nSrcCount != nElementCount - 1 )
		{
			continue;
		}
		for( j = 0; j < nSrcCount; j++)
		{
			if( ( Decl[ j ].Method != pVertexElements[ j ].Method ) ||
				( Decl[ j ].Offset != pVertexElements[ j ].Offset ) ||
				( Decl[ j ].Stream != pVertexElements[ j ].Stream ) ||
				( Decl[ j ].Type != pVertexElements[ j ].Type ) ||
				( Decl[ j ].UsageIndex != pVertexElements[ j ].UsageIndex ) )			
			{
				break;
			}
		}
		if( j >= nSrcCount )
		{
			return i;
		}
	}

	return -1;
}

int CEtDevice::CreateVertexDeclaration( D3DVERTEXELEMENT9* pVertexElements )
{
	int nFindIndex;

	nFindIndex = FindVertexDeclaration( pVertexElements );
	if( nFindIndex != -1 )
	{
		return nFindIndex;
	}

	EtVertexDecl *pDecl = NULL;

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->CreateVertexDeclaration( pVertexElements, &pDecl );
	ADD_D3D_RES( pDecl );


	VertexDecl Decl;
	Decl.pDecl = pDecl;
	pDecl->GetDeclaration( Decl.Elem, ( UINT * )&Decl.nElemCount );

	m_vecVertexDecl.push_back( Decl );


	return ( int )( m_vecVertexDecl.size() - 1 );
}

EtVertexDecl *CEtDevice::GetVertexDeclaration( int nIndex )
{
	if( ( nIndex < 0 ) || ( nIndex >= ( int )m_vecVertexDecl.size() ) )
	{
		ASSERT( 0 && "Invalid Vertex Declaration Index" );
		return NULL;
	}

	return m_vecVertexDecl[ nIndex ].pDecl;
}

// Device 에서 제공하는 함수의 구현 방식을 모르므로,
// 직접 구현하는것이 더빠를 수 있다.
void CEtDevice::GetVertexElement( int nIndex, D3DVERTEXELEMENT9* pElement, UINT* pNumElements )
{
	if( ( nIndex < 0 ) || ( nIndex >= ( int )m_vecVertexDecl.size() ) )
	{
		ASSERT( 0 && "Invalid Vertex Declaration Index" );
		return ;
	}

	VertexDecl &decl = m_vecVertexDecl[ nIndex ];

	for( UINT i = 0; i < decl.nElemCount; i++ ) {
		pElement[ i ] = decl.Elem[ i ];
	}
	if( pNumElements ) {
		*pNumElements = decl.nElemCount;
	}
}

void CEtDevice::SetVertexDeclaration( int nIndex )
{
	if( ( nIndex < 0 ) || ( nIndex >= ( int )m_vecVertexDecl.size() ) )
	{
		ASSERT( 0 && "Invalid Vertex Declaration Index" );
		return;
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetVertexDeclaration( m_vecVertexDecl[ nIndex ].pDecl );
}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
bool CEtDevice::CheckFlushWaitDelete()
{
	bool bResult[2] = { true, true };

	if( m_pFlushWaitDeleteCallback ) {
		bResult[1] = m_pFlushWaitDeleteCallback();
	}
	else bResult[1] = false;

	if( CEtResource::IsEmptyWaitDelete() ) bResult[0] = false;
	else CEtResource::FlushWaitDelete();

	if( bResult[0] == true || bResult[1] == true ) return true;
	
	return false;
}
#endif

EtVertexBuffer *CEtDevice::CreateVertexBuffer( int nLength, DWORD dwFVF, EtUsage Usage, EtPool Pool )
{
	EtVertexBuffer *pBuffer = NULL;

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = m_pDevice->CreateVertexBuffer( nLength, Usage, dwFVF, ( D3DPOOL )Pool, &pBuffer, NULL );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return CreateVertexBuffer( nLength, dwFVF, Usage, Pool );
		}
		else {
			m_szLastErrorMsg = "CreateVertexBuffer";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "CreateVertexBuffer";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return CreateVertexBuffer( nLength, dwFVF, Usage, Pool );
		}
#endif
	}
	ADD_D3D_RES( pBuffer );

	return pBuffer;
}
EtIndexBuffer *CEtDevice::CreateIndexBuffer( int nLength, EtUsage Usage, EtPool Pool )
{
	EtIndexBuffer *pBuffer = NULL;

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = m_pDevice->CreateIndexBuffer( nLength, Usage, D3DFMT_INDEX16, ( D3DPOOL )Pool, &pBuffer, NULL );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return CreateIndexBuffer( nLength, Usage, Pool );
		}
		else {
			m_szLastErrorMsg = "CreateIndexBuffer";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "CreateIndexBuffer";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return CreateIndexBuffer( nLength, Usage, Pool );
		}
#endif
	}

	ADD_D3D_RES( pBuffer );

	return pBuffer;
}

void CEtDevice::SetStreamSource( int nStreamNum, EtVertexBuffer *pBuffer, int nStride, int nStartIndex )
{
	if( pBuffer == NULL )
	{
		ASSERT( 0 && "Invalid VertexBuffer" );
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetStreamSource( nStreamNum, pBuffer, nStartIndex, nStride );
}

void CEtDevice::SetIndexBuffer( EtIndexBuffer *pBuffer )
{
	if( pBuffer == NULL )
	{
		ASSERT( 0 && "Invalid IndexBuffer" );
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetIndices( pBuffer );
}

EtBaseTexture *CEtDevice::CreateTexture( int nWidth, int nHeight, EtFormat Format, EtUsage Usage, EtPool Pool, int nLevel )
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	if( nWidth < 0 )
	{
		nWidth = Width() / abs( nWidth );
	}
	if( nHeight < 0 )
	{
		nHeight = Height() / abs( nHeight );
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateTexture( m_pDevice, nWidth, nHeight, nLevel, Usage, ( D3DFORMAT )Format, ( D3DPOOL )Pool, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return CreateTexture( nWidth, nHeight, Format, Usage, Pool, nLevel );
		}
		else {
			m_szLastErrorMsg = "CreateTexture";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "CreateTexture";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return CreateTexture( nWidth, nHeight, Format, Usage, Pool, nLevel );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

EtBaseTexture *CEtDevice::CreateRenderTargetTexture( int nWidth, int nHeight, EtFormat Format, EtUsage Usage, EtPool Pool )
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	if( nWidth < 0 )
	{
		nWidth = Width() / abs( nWidth );
	}
	if( nHeight < 0 )
	{
		nHeight = Height() / abs( nHeight );
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateTexture( m_pDevice, nWidth, nHeight, 1, D3DUSAGE_RENDERTARGET | Usage, ( D3DFORMAT )Format, ( D3DPOOL )Pool, &pTexture );

	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return CreateRenderTargetTexture( nWidth, nHeight, Format, Usage, Pool );
		}
		else {
			m_szLastErrorMsg = "CreateRenderTargetTexture";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "CreateRenderTargetTexture";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return CreateRenderTargetTexture( nWidth, nHeight, Format, Usage, Pool );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

EtSurface *CEtDevice::CreateRenderTarget( int nWidth, int nHeight, EtFormat Format, EtPool Pool, EtMultiSampleType MultiSampleType )
{
	if( nWidth < 0 )
	{
		nWidth = Width() / abs( nWidth );
	}
	if( nHeight < 0 )
	{
		nHeight = Height() / abs( nHeight );
	}

	EtSurface *pSurface = NULL;
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = m_pDevice->CreateRenderTarget( nWidth, nHeight, (D3DFORMAT)Format, (D3DMULTISAMPLE_TYPE)MultiSampleType, 0, FALSE, &pSurface, NULL);

	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return CreateRenderTarget( nWidth, nHeight, Format, Pool, MultiSampleType );
		}
		else {
			m_szLastErrorMsg = "CreateRenderTarget";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "CreateRenderTarget";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return CreateRenderTarget( nWidth, nHeight, Format, Pool, MultiSampleType );
		}
#endif
	}

	if( !pSurface && MultiSampleType != MULTISAMPLE_NONE ) {	// 안티알리아싱 실패하는 경우...
		OutputDebug(" Fail Create MultiSample RenderTarget\n");
		m_pDevice->CreateRenderTarget( nWidth, nHeight,  (D3DFORMAT)Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pSurface, NULL);
	}

	ADD_D3D_RES( pSurface );

	return pSurface;
}

EtSurface *CEtDevice::CreateDepthStencil( int nWidth, int nHeight, EtFormat Format, EtMultiSampleType MultiSampleType )
{
	EtSurface *pSurface = NULL;	

	if( nWidth < 0 )
	{
		nWidth = Width() / abs( nWidth );
	}
	if( nHeight < 0 )
	{
		nHeight = Height() / abs( nHeight );
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = m_pDevice->CreateDepthStencilSurface( nWidth, nHeight, ( D3DFORMAT )Format, (D3DMULTISAMPLE_TYPE) MultiSampleType, 0, 
		TRUE, &pSurface, NULL );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return CreateDepthStencil( nWidth, nHeight, Format, MultiSampleType );
		}
		else {
			m_szLastErrorMsg = "CreateDepthStencil";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "CreateDepthStencil";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return CreateDepthStencil( nWidth, nHeight, Format, MultiSampleType );
		}
#endif
	}

	ADD_D3D_RES( pSurface );
	return pSurface;
}

EtBaseTexture *CEtDevice::LoadTextureFromMemory( char *pSource, int nSrcSize, bool bPow2, UINT nWidth, UINT nHeight, int nMipLevel )
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	if( nWidth == 0 && nHeight == 0 )
	{
		if( bPow2 )
		{
			nWidth = D3DX_DEFAULT;
			nHeight = D3DX_DEFAULT;
		}
		else
		{
			nWidth = D3DX_DEFAULT_NONPOW2;
			nHeight = D3DX_DEFAULT_NONPOW2;
		}
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx( m_pDevice, pSource, nSrcSize, nWidth, nHeight, nMipLevel, 0, 
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, g_dwTextureColorKey, NULL, NULL, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return LoadTextureFromMemory( pSource, nSrcSize, bPow2 );
		}
		else {
			m_szLastErrorMsg = "LoadTextureFromMemory";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "LoadTextureFromMemory";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return LoadTextureFromMemory( pSource, nSrcSize, bPow2 );
		}
#endif
	}


	ADD_D3D_RES( pTexture );
	return pTexture;
} 

EtBaseTexture *CEtDevice::LoadVolumeTextureFromMemory( char *pSource, int nSrcSize, bool bPow2, UINT nWidth, UINT nHeight )
{
	LPDIRECT3DVOLUMETEXTURE9 pTexture = NULL;

	if( nWidth == 0 && nHeight == 0 )
	{
		if( bPow2 )
		{
			nWidth = D3DX_DEFAULT;
			nHeight = D3DX_DEFAULT;
		}
		else
		{
			nWidth = D3DX_DEFAULT_NONPOW2;
			nHeight = D3DX_DEFAULT_NONPOW2;
		}
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateVolumeTextureFromFileInMemoryEx( m_pDevice, pSource, nSrcSize, nWidth, nHeight, D3DX_DEFAULT, 
		1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, g_dwTextureColorKey, NULL, NULL, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return LoadVolumeTextureFromMemory( pSource, nSrcSize, bPow2 );
		}
		else {
			m_szLastErrorMsg = "LoadVolumeTextureFromMemory";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "LoadVolumeTextureFromMemory";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return LoadVolumeTextureFromMemory( pSource, nSrcSize, bPow2 );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

EtBaseTexture *CEtDevice::LoadCubeTextureFromMemory( char *pSource, int nSrcSize, bool bPow2, UINT nWidth, UINT nHeight )
{
	LPDIRECT3DCUBETEXTURE9 pTexture = NULL;

	if( nWidth == 0 && nHeight == 0 )
	{
		if( bPow2 )
		{
			nWidth = D3DX_DEFAULT;
			nHeight = D3DX_DEFAULT;
		}
		else
		{
			nWidth = D3DX_DEFAULT_NONPOW2;
			nHeight = D3DX_DEFAULT_NONPOW2;
		}
	}

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateCubeTextureFromFileInMemoryEx( m_pDevice, pSource, nSrcSize, nWidth, nHeight, 0, D3DFMT_UNKNOWN, 
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, g_dwTextureColorKey, NULL, NULL, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return LoadCubeTextureFromMemory( pSource, nSrcSize, bPow2 );
		}
		else {
			m_szLastErrorMsg = "LoadCubeTextureFromMemory";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "LoadCubeTextureFromMemory";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return LoadCubeTextureFromMemory( pSource, nSrcSize, bPow2 );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

EtBaseTexture *CEtDevice::LoadTextureFromFile( const char *pFileName, bool bPow2 )
{
	LPDIRECT3DTEXTURE9 pTexture = NULL;

	UINT nSize;
	if( bPow2 )
	{
		nSize = D3DX_DEFAULT;
	}
	else
	{
		nSize = D3DX_DEFAULT_NONPOW2;
	}
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateTextureFromFileEx( m_pDevice, pFileName, nSize, nSize, nSize, 0, D3DFMT_UNKNOWN, 
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, g_dwTextureColorKey, NULL, NULL, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return LoadTextureFromFile( pFileName, bPow2 );
		}
		else {
			m_szLastErrorMsg = "LoadTextureFromFile";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "LoadTextureFromFile";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return LoadTextureFromFile( pFileName, bPow2 );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

EtBaseTexture *CEtDevice::LoadVolumeTextureFromFile( const char *pFileName, bool bPow2 )
{
	LPDIRECT3DVOLUMETEXTURE9 pTexture = NULL;

	UINT nSize;
	if( bPow2 )
	{
		nSize = D3DX_DEFAULT;
	}
	else
	{
		nSize = D3DX_DEFAULT_NONPOW2;
	}
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateVolumeTextureFromFileEx( m_pDevice, pFileName, nSize, nSize, D3DX_DEFAULT, 1, 0,
		D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, g_dwTextureColorKey, NULL, NULL, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return LoadVolumeTextureFromFile( pFileName, bPow2 );
		}
		else {
			m_szLastErrorMsg = "LoadVolumeTextureFromFile";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "LoadVolumeTextureFromFile";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return LoadVolumeTextureFromFile( pFileName, bPow2 );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

EtBaseTexture *CEtDevice::LoadCubeTextureFromFile( const char *pFileName, bool bPow2 )
{
	LPDIRECT3DCUBETEXTURE9 pTexture = NULL;

	UINT nSize;
	if( bPow2 )
	{
		nSize = D3DX_DEFAULT;
	}
	else
	{
		nSize = D3DX_DEFAULT_NONPOW2;
	}
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	HRESULT hr = D3DXCreateCubeTextureFromFileEx( m_pDevice, pFileName, nSize, nSize, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		D3DX_DEFAULT, D3DX_DEFAULT, g_dwTextureColorKey, NULL, NULL, &pTexture );
	if( hr == E_OUTOFMEMORY ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CheckFlushWaitDelete() ) {
			return LoadCubeTextureFromFile( pFileName, bPow2 );
		}
		else {
			m_szLastErrorMsg = "LoadCubeTextureFromFile";
			OnOutOfMemory();
			return NULL;
		}
#else
		if( CEtResource::IsEmptyWaitDelete() ) {
			m_szLastErrorMsg = "LoadCubeTextureFromFile";
			OnOutOfMemory();
			return NULL;
		}
		else {
			CEtResource::FlushWaitDelete();
			return LoadCubeTextureFromFile( pFileName, bPow2 );
		}
#endif
	}

	ADD_D3D_RES( pTexture );
	return pTexture;
}

void CEtDevice::SetTexture( int nSampler, EtBaseTexture *pTexture )
{
	if( GetEtStateManager()->IsEnable() ) {
		GetEtStateManager()->SetTexture( nSampler, pTexture );
	}
	else {
		ScopeLock< CSyncLock > Lock( m_DeviceLock );
		m_pDevice->SetTexture( nSampler, pTexture );
	}
}

void CEtDevice::ClearEffectMacro()
{
	D3DXMACRO Macro;
	int i;

	for( i = 0; i < ( int )m_EffectMacroString.size(); i++ )
	{
		if( m_EffectMacroString[ i ] )
		{
			delete [] m_EffectMacroString[ i ];
		}
	}
	m_EffectMacroString.clear();
	m_EffectMacro.clear();
	memset( &Macro, 0, sizeof( D3DXMACRO ) );
	m_EffectMacro.push_back( Macro );
}

void CEtDevice::AddEffectMacro( char *szName, char *szDefinition )
{
	D3DXMACRO Macro;
	char *pString;

	pString = new char[ strlen( szName ) + 1 ];
	strcpy( pString, szName );
	m_EffectMacroString.push_back( pString );
	Macro.Name = pString;
	pString = new char[ strlen( szDefinition ) + 1 ];
	strcpy( pString, szDefinition );
	m_EffectMacroString.push_back( pString );
	Macro.Definition = pString;
	m_EffectMacro[ m_EffectMacro.size() - 1 ] = Macro;
	memset( &Macro, 0, sizeof( D3DXMACRO ) );
	m_EffectMacro.push_back( Macro );
}

#ifdef PRE_CRASH_CHECK_BACKUP
float g_fMemoryPercent;
#endif
EtEffect *CEtDevice::LoadEffectFromMemory( char *pSource, int nSrcSize, void *pSharedEffectPool, ID3DXInclude *pInclude )
{
	EtEffect *pEffect = NULL;
	DWORD dwFlags = 0;//D3DXFX_LARGEADDRESSAWARE;

	LPD3DXBUFFER pCompileError = NULL;

#ifdef PRE_CRASH_CHECK_BACKUP
	g_fMemoryPercent = GetMemoryUsePercent();
#endif
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	D3DXCreateEffect( m_pDevice, pSource, nSrcSize, &m_EffectMacro[ 0 ], pInclude, dwFlags, ( LPD3DXEFFECTPOOL )pSharedEffectPool, &pEffect, &pCompileError );

	if( pCompileError ) {
		OutputDebug("%s\n", pCompileError->GetBufferPointer());
	}

	ADD_D3D_RES( pEffect );
	return pEffect;
}

EtEffect *CEtDevice::LoadEffectFromFile( const char *pFileName, void *pSharedEffectPool )
{
	EtEffect *pEffect = NULL;
	DWORD dwFlags;

	dwFlags = 0;
	if( m_bShaderDebug )
	{
		dwFlags |= D3DXSHADER_DEBUG;
		dwFlags |= D3DXSHADER_SKIPOPTIMIZATION;
		dwFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
	}

	char szPath[ _MAX_PATH ], szCurDir[ _MAX_PATH ];
	GetCurrentDirectory( _MAX_PATH, szCurDir );
	_GetPath( szPath, _countof(szPath), pFileName );
	_chdir( szPath );

	LPD3DXBUFFER pCompileError = NULL;

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	D3DXCreateEffectFromFile( m_pDevice, pFileName, &m_EffectMacro[ 0 ], NULL, dwFlags, ( LPD3DXEFFECTPOOL )pSharedEffectPool, &pEffect, &pCompileError );

	if( pCompileError ) {
		OutputDebug("%s\n", pCompileError->GetBufferPointer());
	}

	SetCurrentDirectory( szCurDir );

	ADD_D3D_RES( pEffect );
	return pEffect;
}

void* CEtDevice::CreateFont( const char *pFontName )
{
	ID3DXFont *pFont = NULL;
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	D3DXCreateFont( m_pDevice, 12, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, pFontName, &pFont ) ;
	ADD_D3D_RES( pFont );	
	return pFont;
}

void* CEtDevice::CreateSystemFont( const char *pFontName )
{
	CEtSystemFont *pFont = CEtSystemFont::Create( 12 , pFontName );
	return pFont;
}

void *CEtDevice::CreateSprite()
{
	ID3DXSprite *pSprite = NULL;

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	D3DXCreateSprite( m_pDevice, &pSprite );

	ADD_D3D_RES( pSprite );
	return pSprite;
}

void CEtDevice::SetCullMode( CullMode Mode )
{
	SetRenderState( D3DRS_CULLMODE, Mode );
}

void CEtDevice::SetWireframe( bool bEnable )
{
	SetRenderState( D3DRS_FILLMODE, bEnable ? D3DFILL_WIREFRAME : D3DFILL_SOLID );
}

bool CEtDevice::EnableAlphaBlend( bool bEnable )
{
	DWORD dwValue;
	bool bRet = false;
	GetRenderState( D3DRS_ALPHABLENDENABLE, &dwValue );
	if( dwValue )
	{
		bRet = true;
	}

	SetRenderState( D3DRS_ALPHABLENDENABLE, bEnable );
	if( bEnable )
	{
		SetRenderState( D3DRS_ALPHAREF, 0x0 );		
		SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
		SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );	
	}
	else
	{
		SetRenderState( D3DRS_ALPHAREF, ALPHA_TEST_VALUE );
	}

	return bRet;
}

bool CEtDevice::EnableAlphaTest( bool bEnable )
{
	DWORD dwValue;
	bool bRet = false;
	GetRenderState( D3DRS_ALPHATESTENABLE, &dwValue );
	if( dwValue )
	{
		bRet = true;
	}

	SetRenderState( D3DRS_ALPHATESTENABLE, bEnable );

	return bRet;
}

bool CEtDevice::IsEnableAlphaTest()
{
	DWORD dwValue;
	GetRenderState( D3DRS_ALPHATESTENABLE, &dwValue );
	return dwValue != FALSE;
}

DWORD CEtDevice::SetAlphaRef( DWORD dwAlphaRef )
{
	DWORD dwRet = GetAlphaRef();
	SetRenderState( D3DRS_ALPHAREF, dwAlphaRef );
	return dwRet;
}

DWORD CEtDevice::GetAlphaRef()
{
	DWORD dwAlphaRef;
	GetRenderState( D3DRS_ALPHAREF, &dwAlphaRef );
	return dwAlphaRef;
}

bool CEtDevice::IsEnableAlphaBlend()
{
	DWORD dwValue;
	GetRenderState( D3DRS_ALPHABLENDENABLE, &dwValue );
	return dwValue != FALSE;
}

bool CEtDevice::EnableZ( bool bEnable )
{
	DWORD bRet = TRUE;
	GetRenderState( D3DRS_ZENABLE, &bRet );
	SetRenderState( D3DRS_ZWRITEENABLE, bEnable );
	SetRenderState( D3DRS_ZENABLE, bEnable ? D3DZB_TRUE : D3DZB_FALSE );
	return (bRet != FALSE);
}

bool CEtDevice::EnableZTest( bool bZTest )
{
	bool bRet = IsEnableZTest();
	SetRenderState( D3DRS_ZENABLE, bZTest ? D3DZB_TRUE : D3DZB_FALSE );
	return bRet;
}

bool CEtDevice::IsEnableZTest()
{
	DWORD dwValue;
	GetRenderState( D3DRS_ZENABLE, &dwValue );
	if( dwValue )
	{
		return true;
	}
	else
	{
		return false;
	}
}

float CEtDevice::SetDepthBias( float fBias )
{
	float fRet;

	GetRenderState( D3DRS_DEPTHBIAS, (DWORD*)&fRet );
	SetRenderState( D3DRS_DEPTHBIAS, *(DWORD*)&fBias );

	return fRet;
}

bool CEtDevice::EnableZWrite( bool bEnable )
{
	bool bRet = IsEnableZWrite();
	SetRenderState( D3DRS_ZWRITEENABLE, bEnable );
	return bRet;
}

EtCmpFunc CEtDevice::SetZFunc( EtCmpFunc ZFunc )
{
	DWORD dwRet;
	GetRenderState( D3DRS_ZFUNC, &dwRet );
	SetRenderState( D3DRS_ZFUNC, ZFunc );
	return ( EtCmpFunc )dwRet;
}

bool CEtDevice::IsEnableZWrite()
{
	DWORD dwValue;
	GetRenderState( D3DRS_ZWRITEENABLE, &dwValue );
	if( dwValue )
	{
		return true;
	}
	else
	{
		return false;
	}
}

EtBlendOP CEtDevice::SetBlendOP( EtBlendOP BlendOP )
{
	DWORD dwOldBlendOP;
	GetRenderState( D3DRS_BLENDOP, &dwOldBlendOP );
	SetRenderState( D3DRS_BLENDOP, BlendOP );
	return ( EtBlendOP )dwOldBlendOP;
}

EtBlendMode CEtDevice::SetSrcBlend( EtBlendMode BlendMode )
{
	DWORD dwOldBlend;
	GetRenderState( D3DRS_SRCBLEND, &dwOldBlend );
	SetRenderState( D3DRS_SRCBLEND, BlendMode );
	return ( EtBlendMode )dwOldBlend;
}

EtBlendMode CEtDevice::SetDestBlend( EtBlendMode BlendMode )
{
	DWORD dwOldBlend;
	GetRenderState( D3DRS_DESTBLEND, &dwOldBlend );
	SetRenderState( D3DRS_DESTBLEND, BlendMode );
	return ( EtBlendMode )dwOldBlend;
}

void CEtDevice::SetWorldTransform( EtMatrix *pWorldMat )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetTransform( D3DTS_WORLD, pWorldMat );
}

void CEtDevice::SetViewTransform( EtMatrix *pViewMat )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetTransform( D3DTS_VIEW, pViewMat );
}

void CEtDevice::SetProjTransform( EtMatrix *pProjMat )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetTransform( D3DTS_PROJECTION, pProjMat );
}

void CEtDevice::GetFVF( DWORD *dwFVF )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->GetFVF( dwFVF );	
}

void CEtDevice::SetFVF( DWORD dwFVF )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetFVF( dwFVF );
}

void CEtDevice::SetVertexShader( LPDIRECT3DVERTEXSHADER9 pShader )
{	
	if( GetEtStateManager()->IsEnable() ) {
		GetEtStateManager()->SetVertexShader( (LPDIRECT3DVERTEXSHADER9)pShader );
	}
	else {
		ScopeLock< CSyncLock > Lock( m_DeviceLock );
		m_pDevice->SetVertexShader( pShader );
	}
}

void CEtDevice::SetPixelShader( LPDIRECT3DPIXELSHADER9 pShader )
{
	if( GetEtStateManager()->IsEnable() ) {
		GetEtStateManager()->SetPixelShader( (LPDIRECT3DPIXELSHADER9)pShader );
	}
	else {
		ScopeLock< CSyncLock > Lock( m_DeviceLock );
		m_pDevice->SetPixelShader( pShader );	
	}
}

//void CEtDevice::EnableLight( bool bEnable )
//{
//	SetRenderState( D3DRS_LIGHTING, bEnable );
//}

void CEtDevice::SetCursorProperties( EtSurface *pSurface, UINT nXHotSpot, UINT nYHotSpot )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetCursorProperties( nXHotSpot, nYHotSpot, pSurface );
}

void CEtDevice::ShowCursor( bool bShow )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->ShowCursor( bShow );
}

void CEtDevice::SetCursorPos( int nX, int nY )
{
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->SetCursorPosition( nX, nY, D3DCURSOR_IMMEDIATE_UPDATE );
}

void CEtDevice::SetGammaRamp( float fGamma, int nBright )
{
	if( !m_pDevice ) return;
	int i;
	D3DGAMMARAMP GammaRamp;

	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->GetGammaRamp( 0, &GammaRamp );
	for ( i = 0; i < 256; i++ )
	{
		float fValue = ( float )( 255 * pow( ( float ) i / 256, 1 / fGamma ) ) + nBright;
		if( fValue < 0 )
		{
			fValue = 0;
		}
		if( fValue > 255 )
		{
			fValue = 255;
		}
		GammaRamp.red[ i ] = ( ( WORD )fValue ) << 8;
		GammaRamp.green[ i ] = GammaRamp.red[ i ];
		GammaRamp.blue[ i ] = GammaRamp.red[ i ];
	}
	m_pDevice->SetGammaRamp( 0, D3DSGR_CALIBRATE, &GammaRamp );
}

#ifdef PRE_MOD_MEMORY_CHECK
void CEtDevice::CheckTotalLocalVideoMemory()
{
	ScopeLock<CSyncLock> Lock(m_DeviceLostLock);

	// DDraw GetAvailableVidMem 함수도 써보고,
	// D3D GetAvailableTextureMem 함수도 써보고,
	// Windows WMI에서 구하는 것도 해봤는데,
	// 가장 정확한건, 토탈은 DDraw로 구하고, 현재 사용량은 D3D로 체크하는거다.
	// WMI는 AvailMemory가 아니라, 진짜 비디오카드 메모리를 리턴해서, 게다가 일부 컴에서는 안먹어서 안쓰기로 한다.
	if( m_pDD != NULL )
	{
		DDSCAPS2 ddsCaps2; 
		DWORD dwTotal, dwFree; 
		memset( &ddsCaps2, 0, sizeof( DDSCAPS2 ) );
		ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		if( m_pDD->GetAvailableVidMem( &ddsCaps2, &dwTotal, &dwFree ) == DD_OK )
			m_dwLocalVideoMemory = dwTotal;
	}
	if( m_pDevice )
	{
		m_dwTotalAvailTextureMemory = m_pDevice->GetAvailableTextureMem();
	}
	m_dwMaxLocalVideoMemory = m_dwLocalVideoMemory / 1024 / 1024;
}
#endif

float CEtDevice::GetMemoryUsePercent()
{
	ScopeLock<CSyncLock> Lock(m_DeviceLostLock);
#ifdef PRE_MOD_MEMORY_CHECK
	m_dwUsingLocalVideoMemory = (m_dwTotalAvailTextureMemory - m_pDevice->GetAvailableTextureMem()) / 1024 / 1024;
	return (m_dwTotalAvailTextureMemory - m_pDevice->GetAvailableTextureMem() ) / ( float )m_dwLocalVideoMemory;
#else
	if( m_bUseDDrawMemCheck ) {
		if( m_pDD == NULL ) {
			return 0.0f;
		}
		DDSCAPS2 ddsCaps2; 
		DWORD dwTotal, dwFree; 
		memset( &ddsCaps2, 0, sizeof( DDSCAPS2 ) );
		ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		m_pDD->GetAvailableVidMem( &ddsCaps2, &dwTotal, &dwFree ); 
		if( dwTotal == 0 ) {
			return 0.0f;
		}
		return ( dwTotal - dwFree ) / ( float )dwTotal;
	}
	else {
		if( m_pDevice == NULL ) {
			return 0.0f;
		}
		return (m_dwTotalAvailTextureMemory - m_pDevice->GetAvailableTextureMem() ) / ( float )m_dwTotalAvailTextureMemory ;
	}
#endif
}

bool CEtDevice::CheckDeviceFormat( EtFormat CheckFormat, EtUsage Usage )
{
	if( FAILED( m_pD3D->CheckDeviceFormat( m_nAdapter, m_DevType, m_AdapterFormat, Usage, D3DRTYPE_SURFACE, ( D3DFORMAT )CheckFormat ) ) )
	{
		return false;
	}
	return true;
}

void CEtDevice::SetClipCursor( bool bClip )
{
#if !defined(_DEBUG) && !defined(_RDEBUG) // 디버깅시에는 필요할 수도 있으므로 클립 안함
	if( bClip ) {
		RECT rcWindow;
		::GetWindowRect( m_hWnd, &rcWindow );
		::ClipCursor( &rcWindow );			
	}
	else {
		::ClipCursor( NULL );
	}
#endif
}

void CEtDevice::OnOutOfMemory()
{
	if( m_pOutOfMemoryCallBack ) {
		m_bCallOutOfMemory = true;
		m_pOutOfMemoryCallBack->Run();
	}
}

void CEtDevice::ShowFrame( RECT *pTargetRect ) 
{
	if( m_pPrePresentCallback ) m_pPrePresentCallback();
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->Present( NULL, pTargetRect, NULL, NULL ); 
	m_nPolyCountPerFrame = m_nCurrentPolyCount;
	m_nCurrentPolyCount = 0;
}

void CEtDevice::GetRenderState( D3DRENDERSTATETYPE State, DWORD *pdwValue ) 
{ 
	GetEtStateManager()->GetRenderStateDefered( State, pdwValue); 
}

void CEtDevice::SetRenderState( D3DRENDERSTATETYPE State, DWORD dwValue ) 
{ 
	GetEtStateManager()->SetRenderStateDefered( State, dwValue); 
}

void CEtDevice::GetSamplerState( DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD *pdwValue ) 
{ 
	GetEtStateManager()->GetSamplerStateDefered( Sampler, State, pdwValue); 
}

void CEtDevice::SetSamplerState( DWORD Sampler, D3DSAMPLERSTATETYPE State, DWORD Value ) 
{ 
	GetEtStateManager()->SetSamplerStateDefered( Sampler, State, Value); 
}

void CEtDevice::GetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	GetEtStateManager()->GetTextureStageStateDefered( Stage, Type, pValue); 
}

void CEtDevice::SetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	GetEtStateManager()->SetTextureStageStateDefered( Stage, Type, Value); 
}

DWORD CEtDevice::SetColorWriteEnable( DWORD dwEnable )
{ 
	DWORD dwCur;
	GetRenderState( D3DRS_COLORWRITEENABLE, &dwCur );
	SetRenderState( D3DRS_COLORWRITEENABLE, dwEnable ); 
	return dwCur;
}

void CEtDevice::DrawPrimitive( PrimitiveType nPrimitiveType, int nStartVertex, int nPrimitiveCount )
{
	GetEtStateManager()->FlushDeferedStates();
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->DrawPrimitive( ( D3DPRIMITIVETYPE )nPrimitiveType, nStartVertex, nPrimitiveCount );
	m_nCurrentPolyCount += nPrimitiveCount;
}

void CEtDevice::DrawIndexedPrimitive( PrimitiveType nPrimitiveType, int nStartVertex, int nVertexCount, int nStartIndex, int nPrimitiveCount )
{
	GetEtStateManager()->FlushDeferedStates();
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->DrawIndexedPrimitive( ( D3DPRIMITIVETYPE )nPrimitiveType, nStartVertex, 0, nVertexCount, nStartIndex, nPrimitiveCount );
	m_nCurrentPolyCount += nPrimitiveCount;
}

void CEtDevice::DrawPrimitiveUP( PrimitiveType nPrimitiveType, int nPrimitiveCount, void *pVertexData, int nStride )
{
	GetEtStateManager()->FlushDeferedStates();
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->DrawPrimitiveUP( ( D3DPRIMITIVETYPE )nPrimitiveType, nPrimitiveCount, pVertexData, nStride );
	m_nCurrentPolyCount += nPrimitiveCount;
}

void CEtDevice::DrawIndexedPrimitiveUP( PrimitiveType nPrimitiveType, int nMinVertexIndex, int nNumVertices, int nPrimitiveCount, void *pIndexData, EtFormat IndexDataFormat, void *pVertexData, int nStride )
{
	GetEtStateManager()->FlushDeferedStates();
	ScopeLock< CSyncLock > Lock( m_DeviceLock );
	m_pDevice->DrawIndexedPrimitiveUP( ( D3DPRIMITIVETYPE )nPrimitiveType, nMinVertexIndex, nNumVertices, nPrimitiveCount, pIndexData, (D3DFORMAT)IndexDataFormat,  pVertexData, nStride);
	m_nCurrentPolyCount += nPrimitiveCount;
}
