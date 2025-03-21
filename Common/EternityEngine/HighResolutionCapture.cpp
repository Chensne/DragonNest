#include "StdAfx.h"
#include "HighResolutionCapture.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include "shlobj.h"
#include <time.h>
#include <Direct.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CHighResolutionCapture::CHighResolutionCapture()
: m_bCapture( false )
, m_pColorAdjFilter( NULL )
, m_dwClearColor( 0xFF000000 )
, m_fTempAspectRatio( 0.0f )
{
}

CHighResolutionCapture::~CHighResolutionCapture()
{
	SAFE_DELETE( m_pColorAdjFilter );
}

void CHighResolutionCapture::Capture( const char *szFilePrefix, int nWidth, int nHeight )
{
	if( !m_pColorAdjFilter )
	{
		// 세인트 헤이븐꺼 가져와 쓴다.
		m_pColorAdjFilter = (CEtColorAdjustTexFilter*)EternityEngine::CreateFilter( SF_COLORADJUSTTEX );
		m_pColorAdjFilter->SetParam( EtVector4(-0.071f, -0.107f, -0.107f, 0.0f), EtVector4(0.25f, 0.286f, 0.357f, 0.0f), EtVector4(0.357f, 0.357f, 0.536f, 0.0f), 0.9f );
		m_pColorAdjFilter->Enable( false );
	}

	if( m_bCapture )
		return;

	if( nWidth <= 0 || nWidth == INT_MAX ) nWidth = GetEtDevice()->GetMaxTextureWidth();
	if( nHeight <= 0 || nHeight == INT_MAX ) nHeight = GetEtDevice()->GetMaxTextureHeight();

	// 비디오카드 메모리 문제인가.. 텍스처 생성은 문제없는데 Save할때 아웃오브메모리떠서 최대치 제한 걸어둔다.
	if( nWidth > 7200 ) nWidth = 7200;
	if( nHeight > 7200 ) nHeight = 7200;

	m_hRenderTarget = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, FMT_X8R8G8B8 );
	m_hDepthTarget = CEtDepth::CreateDepthStencil( nWidth, nHeight, FMT_D24S8 );
	m_hTempTarget = CEtTexture::CreateRenderTargetTexture( nWidth, nHeight, FMT_X8R8G8B8 );
	m_szFilePrefix = szFilePrefix;

	// 백그라운드 컬러에 0x0으로 클리어해서 알파판에 렌더해줄 수 있냐고 요청이 왔었다.
	// 그래서 FMT_A8R8G8B8로 타겟 설정하고 렌더루틴을 기본 그대로 타게 했더니,
	// 알파렌더할때 버퍼에 알파 기록하는 부분에서 a값이 이상하게 기록되었다.(나중에 그려지는 a를 그대로 버퍼에 기록)
	// 그래서 불투명위에 찍는 알파부분의 a값이 255보다 낮아지는 현상이 발생했다.
	// 아무래도 렌더루틴이 X8R8G8B8에 그려지는걸 기반으로 작성되서 그런듯..
	// 그렇다고 쉐이더를 뜯을수도 없고,
	// D3DRS_SEPARATEALPHABLENDENABLE사용하더라도 누적해서 알파블렌딩 할 경우 깔끔하게 그릴 방법이 없어서, 불투명 판에다만 그리도록 했다.
	// 사실, 엔진의 렌더루틴을 그대로 사용하면서 알파백버퍼에 그리려던거 자체가 잘못된거 같았다.

	m_bCapture = true;
}

void CHighResolutionCapture::BeginCapture()
{
	if( !m_bCapture ) return;

	EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
	if( hCamera )
	{
		m_fTempAspectRatio = hCamera->GetAspectRatio();
		hCamera->SetAspectRatio( (float)m_hRenderTarget->Width() / (float)m_hRenderTarget->Height() );
	}

	GetEtDevice()->SetRenderTarget( m_hRenderTarget->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( m_hDepthTarget->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( m_dwClearColor, 1.0f, 0 );
}

void CHighResolutionCapture::EndCapture()
{
	if( !m_bCapture ) return;

	GetEtDevice()->RestoreDepthStencil();
	GetEtDevice()->RestoreRenderTarget();

	EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
	if( hCamera )
	{
		hCamera->SetAspectRatio( m_fTempAspectRatio );
	}

	if( m_pColorAdjFilter && 0 )
	{
		// 임시텍스처에 현재 RTT 내용을 복사한 후
		m_hTempTarget->Copy( m_hRenderTarget );

		// 프레리에서 쓰는걸로 통일해서 사용한다.
		float fSat = 0.f;
		fSat = m_pColorAdjFilter->GetSceneSaturation();
		m_pColorAdjFilter->SetSceneSaturation( 1.0f );
		bool bEnableZ = GetEtDevice()->EnableZ( false );	
		bool bAlphaEnable = GetEtDevice()->EnableAlphaBlend( true );
		GetEtDevice()->SetDepthStencilSurface( NULL );
		m_pColorAdjFilter->SetTexture( m_hTempTarget->GetMyIndex() );
		m_pColorAdjFilter->DrawFilter( m_hRenderTarget, 1, EtVector2(0, 0), EtVector2(1, 1) , true );
		GetEtDevice()->RestoreRenderTarget();
		GetEtDevice()->RestoreDepthStencil();
		GetEtDevice()->EnableZ( bEnableZ );
		GetEtDevice()->EnableAlphaBlend( bAlphaEnable );
		m_pColorAdjFilter->SetSceneSaturation( fSat );
	}

	char szPath[MAX_PATH]={0,};
	SHGetSpecialFolderPath(GetEtDevice()->GetHWnd(), szPath, CSIDL_PERSONAL, 0);

	__time64_t ltime;
	char szBuf[256] = { 0, }, szFile[256] = { 0, };
	struct	tm *tm_ptr;
	time(&ltime);
	tm_ptr = localtime(&ltime);

	strftime(szBuf, 256, "%Y-%m-%d %H-%M-%S %a", tm_ptr);
	sprintf(szFile, "%s %s.tga", m_szFilePrefix.c_str(), szBuf);

	char szResultName[MAX_PATH] = { 0, };

	sprintf( szResultName, "%s\\ToolScreenshot", szPath );
	if( !PathFileExists( szResultName ) ) _mkdir( szResultName );

	sprintf( szResultName, "%s\\ToolScreenshot\\%s", szPath, szFile );

	HRESULT hr = D3DXSaveSurfaceToFile( szResultName, D3DXIFF_TGA, m_hRenderTarget->GetSurfaceLevel(), NULL, NULL );

	SAFE_RELEASE_SPTR( m_hRenderTarget );
	SAFE_RELEASE_SPTR( m_hDepthTarget );
	SAFE_RELEASE_SPTR( m_hTempTarget );

	m_bCapture = false;
}