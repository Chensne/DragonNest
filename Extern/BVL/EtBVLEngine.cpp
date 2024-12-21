#include "StdAfx.h"
#include "EtBVLEngine.h"
#include "EtEngine.h"
#include "EtPrimitiveDraw.h"
#include "EtTextDraw.h"
#include "EtResourceMng.h"
#include "Stream.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtBVLEngine::CEtBVLEngine()
{
	m_pBVL = NULL;
	m_pBVT = NULL;
	m_pBVS = NULL;
	m_pEtDevice = NULL;
	m_bRepeat = false;
	m_bOnLostDevice = false;

	UINT WM_BVNOTIFY_EOF = ::RegisterWindowMessage(_T("WM_BVNOTIFY_EOF"));
	UINT WM_BVNOTIFY_OK = ::RegisterWindowMessage(_T("WM_BVNOTIFY_OK"));
}

CEtBVLEngine::~CEtBVLEngine()
{
	Finalize();
}

bool CEtBVLEngine::Initialize( HWND hWnd, CEtDevice *pDev, EtCameraHandle hCamera )
{
	if( Initialize( hWnd, pDev ) == false )
		return false;
	m_hCamera = hCamera;
	return true;
}

bool CEtBVLEngine::Initialize( HWND hWnd, CEtDevice *pDev )
{
	m_pBVS = new CEtBandiVideoStream();
	m_pBVL = new CBandiVideoLibrary;
	if( FAILED( m_pBVL->Create( BANDIVIDEO_RELEASE_DLL_FILE_NAME, m_pBVS ) ) ) {
		SAFE_DELETE( m_pBVS );
		SAFE_DELETE( m_pBVL );
		return false;
	}

	m_pBVL->Verify( "EYEDENTITYGAMES_DRAGONNEST_20090420", "270b0232" );

	m_pEtDevice = pDev;
	return true;
}

void CEtBVLEngine::Finalize()
{
	if( m_pBVL == NULL ) return;

	BVL_STATUS Status;
	m_pBVL->GetStatus( Status );
	if( Status != BVL_STATUS_CLOSED ) m_pBVL->Close();

	m_pBVL->Destroy();

	SAFE_DELETE( m_pBVT );
	SAFE_DELETE( m_pBVL );
	SAFE_DELETE( m_pBVS );
}

void CEtBVLEngine::Process( LOCAL_TIME LocalTime, float fDelta, int nValue )
{
	if( !m_pBVL ) return;

	if( g_pEtRenderLock ) g_pEtRenderLock->Lock();

	EtTexture *pCurTexture = NULL;
	if( GetVideoTexture( &pCurTexture ) )
	{
		EternityEngine::ClearScreen( 0xFFFFFFFF, 1.0f, 0 );
		m_pEtDevice->BeginScene();

		EtVector4 Vertices[ 4 ];
		EtVector2 TexCoord[ 4 ];
		int nWidth, nHeight;

		nWidth = CEtDevice::GetInstance().Width();
		nHeight = CEtDevice::GetInstance().Height();

		float fTemp = ( nHeight - ( ( nWidth / 16.f ) * 9.f ) ) / 2.f;

		Vertices[0] = EtVector4( 0.f, fTemp, 0.0f, 0.0f );
		Vertices[1] = EtVector4( (float)nWidth, fTemp, 0.0f, 0.f );
		Vertices[2] = EtVector4( (float)nWidth, (float)nHeight - fTemp, 0.0f, 0.f );
		Vertices[3] = EtVector4( 0.f, (float)nHeight - fTemp, 0.0f, 0.f );

		TexCoord[0] = EtVector2( 0.0f, 0.0f );
		TexCoord[1] = EtVector2( 1.0f, 0.0f );
		TexCoord[2] = EtVector2( 1.0f, 1.0f );
		TexCoord[3] = EtVector2( 0.0f, 1.0f );
		for( int i=0; i<4; i++ ) {
			Vertices[i].x -= 0.5f;
			Vertices[i].y -= 0.5f;
		}
		EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, pCurTexture );

		if( nValue != -1 )
		{
			float fXValue = nValue / 100.0f;
			float fYValue = (float)(int)( CEtDevice::GetInstance().Height() / 100.0f );
			EtVector2 vTemp[4] = { EtVector2( 0.f, 0.f ), EtVector2( 1.f, 0.f ), EtVector2( 1.f, 1.f ), EtVector2( 0.f, 1.f ) };
			vTemp[0].x = 0.0f;
			vTemp[0].y = ( 1.f / CEtDevice::GetInstance().Height() ) * ( CEtDevice::GetInstance().Height() - fYValue );
			vTemp[1].x = fXValue;
			vTemp[1].y = ( 1.f / CEtDevice::GetInstance().Height() ) * ( CEtDevice::GetInstance().Height() - fYValue );
			vTemp[2].x = fXValue;
			vTemp[2].y = 1.0f;
			vTemp[3].x = 0.0f;
			vTemp[3].y = 1.0f;
			EternityEngine::DrawTriangle2D( vTemp[0], vTemp[1], vTemp[2], 0xFFFF9B00 );
			EternityEngine::DrawTriangle2D( vTemp[0], vTemp[2], vTemp[3], 0xFFFF9B00 );

			//char szLoading[64] = {0,};
			//sprintf_s( szLoading, _countof(szLoading), "%d percent", nValue );
			//EtVector2 vPos = EtVector2( ( 1.f / CEtDevice::GetInstance().Width() ) * 10.f, ( 1.f / CEtDevice::GetInstance().Height() ) * ( CEtDevice::GetInstance().Height() - 20.f ) );
			//EternityEngine::DrawText2D( vPos, szLoading, 0xff888888 );
			//vPos = EtVector2( ( 1.f / CEtDevice::GetInstance().Width() ) * 9.f, ( 1.f / CEtDevice::GetInstance().Height() ) * ( CEtDevice::GetInstance().Height() - 21.f ) );
			//EternityEngine::DrawText2D( vPos, szLoading, 0xff000000 );
		}

		CEtPrimitiveDraw::GetInstance().Render( m_hCamera );
		//CEtTextDraw::GetInstance().Render();
		m_pEtDevice->EndScene();
		m_pEtDevice->ShowFrame( NULL );
	}

	if( g_pEtRenderLock ) g_pEtRenderLock->UnLock();
}

bool CEtBVLEngine::GetVideoTexture( EtTexture **ppTexture )
{
	if( !m_pBVL ) return false;
	if( m_pBVL->IsCreated() == FALSE ) return false;

	if( m_bRepeat )
	{
		BVL_STATUS status;
		m_pBVL->GetStatus( status );
		if( status == BVL_STATUS_PLAYEND )
		{
			m_pBVL->Seek( 0, BVL_SEEK_TIME );
			m_pBVL->Play();
		}
	}

	if( m_pBVL->IsNextFrame() )
	{
		if( m_pBVT == NULL && m_pEtDevice )
		{
			BV_DEVICE_DX9 bvd;
			bvd.m_d3d9_device = (LPDIRECT3DDEVICE9)m_pEtDevice->GetDevicePtr();
			bvd.m_d3d9_device->GetDirect3D( &bvd.m_d3d9 );
			bvd.m_hwnd_parent = m_pEtDevice->GetHWnd();
			m_pBVT = new CBandiVideoTexture_DX9( &bvd );
			if( !m_pBVT || FAILED( m_pBVT->Open( m_VideoInfo.width , m_VideoInfo.height ) ) )
			{
				m_pBVL->Stop();
				SAFE_DELETE( m_pBVT );
				return false;
			}
		}

		INT pitch;
		BYTE *buf = m_pBVT->Lock( pitch );
		if( buf )
		{
			// Get frame
			BVL_FRAME frame;
			frame.frame_buf = buf;
			frame.frame_buf_size = m_VideoInfo.height*pitch;
			frame.pitch = pitch;
			frame.width = m_VideoInfo.width;
			frame.height = m_VideoInfo.height;
			frame.pixel_format = m_pBVT->GetFormat();

			if( FAILED( m_pBVL->GetFrame( frame, TRUE ) ) ) {
				*ppTexture = NULL;
				m_pBVT->Unlock();
				return false;
			}
			*ppTexture = (LPDIRECT3DTEXTURE9)m_pBVT->GetObject();

			m_pBVT->Unlock();
		}
		return true;
	}

	return false;
}

bool CEtBVLEngine::Open( const char *szFileName, BVL_OPEN_OPT option )
{
	if( !m_pBVL ) return false;
	Close();

	if( FAILED( m_pBVL->Open( szFileName, FALSE ) ) )
		return false;

	if( FAILED( m_pBVL->GetVideoInfo( m_VideoInfo ) ) )
		return false;

	if( option == BVL_OPEN_OPT_AUTOPLAY || option == BVL_OPEN_OPT_AUTOPLAY_REPEAT )
		m_pBVL->Play();

	m_bRepeat = (option == BVL_OPEN_OPT_AUTOPLAY_REPEAT) ? true : false;
	m_bOnLostDevice = false;

	return true;
}

void CEtBVLEngine::SetVolume( float fValue )
{
	if( !m_pBVL ) return;
	m_pBVL->SetVolume( (int)(fValue * 255.f ) );
}

void CEtBVLEngine::Close()
{
	if( !m_pBVL ) return;
	if( IsPlaying() ) Stop();
	SAFE_DELETE( m_pBVT );
	m_pBVL->Close();
}

HRESULT CEtBVLEngine::Play( bool bRepeat )
{
	if( !m_pBVL ) return E_FAIL;
	m_bRepeat = bRepeat;
	m_bOnLostDevice = false;
	return m_pBVL->Play();
}

void CEtBVLEngine::Stop()
{
	if( !m_pBVL ) return;
	m_bOnLostDevice = false;
	m_pBVL->Stop();
}

void CEtBVLEngine::Pause()
{
	if( !m_pBVL ) return;
	BVL_STATUS status;
	if( SUCCEEDED( m_pBVL->GetStatus( status ) ) )
	{
		if( status == BVL_STATUS_PLAYING )
			m_pBVL->Pause( true );
	}
}

void CEtBVLEngine::Resume()
{
	if( !m_pBVL ) return;
	BVL_STATUS status;
	if( SUCCEEDED( m_pBVL->GetStatus( status ) ) )
	{
		if( status == BVL_STATUS_PAUSED )
			m_pBVL->Pause( false );
	}
}

bool CEtBVLEngine::IsPlaying()
{
	if( !m_pBVL ) return false;

	BVL_STATUS status;
	if( SUCCEEDED( m_pBVL->GetStatus( status ) ) )
	{
		if( status == BVL_STATUS_PLAYING )
			return true;
	}

	return false;
}

HRESULT CEtBVLEngine::GetVideoInfo(BVL_VIDEO_INFO& info)
{
	if (m_pBVL)
		return m_pBVL->GetVideoInfo(info);

	return E_FAIL;
}

void CEtBVLEngine::OnLostDevice()
{
	SAFE_DELETE( m_pBVT );
	m_bOnLostDevice = true;
}

void CEtBVLEngine::OnResetDevice()
{
	if( CEtDevice::IsActive() )
		m_pEtDevice = &CEtDevice::GetInstance();
}

CEtBandiVideoStream::CEtBandiVideoStream()
{
}

HRESULT CEtBandiVideoStream::Open(LPCSTR path_name)
{
	if( CResMngStream::Open( path_name ) ) return BV_OK;
	return BVERR_OPEN_FAILED;
}

void CEtBandiVideoStream::Close()
{
	CResMngStream::Close();
}

INT	CEtBandiVideoStream::Read(BYTE* pBuf, INT nBytesToRead)
{
	int nValue = m_pStream->Tell();
	m_pStream->Read( pBuf, nBytesToRead );
	return m_pStream->Tell() - nValue;
}

INT64 CEtBandiVideoStream::SetPosition(INT64 nPos)
{
	m_pStream->Seek( (int)nPos, SEEK_SET );
	return m_pStream->Tell();
}

INT64 CEtBandiVideoStream::GetPosition()
{
	return (int)m_pStream->Tell();
}

INT64 CEtBandiVideoStream::GetFileSize()
{
	return m_pStream->Size();
}