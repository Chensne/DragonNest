#include "StdAfx.h"
#include "EtBCLEngine.h"
#include <Shlobj.h>
#include <shlwapi.h>
#include <time.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


void (__stdcall *CEtBCLEngine::s_pCaptureInfoCallback)() = NULL;

CEtBCLEngine::CEtBCLEngine()
{
	m_pBCL = NULL;
	m_pEtDevice = NULL;
	m_hWnd = NULL;
	m_szDefaultPath = L".";
	m_pPrevPrePresentCallback = NULL;

	SetOption( Default );
}

CEtBCLEngine::~CEtBCLEngine()
{
	Finalize();
}


bool CEtBCLEngine::Initialize( HWND hWnd, CEtDevice *pDev, WCHAR *wszDefaultPath )
{
	m_pBCL = new CBandiCaptureLibrary;

	if( FAILED( m_pBCL->Create( BANDICAP_RELEASE_DLL_FILE_NAME ) ) ) {
		SAFE_DELETE( m_pBCL );
		return false;
	}
	if( m_pBCL->GetVersion() < BCAP_VERSION ) {
		SAFE_DELETE( m_pBCL );
		return false;
	}

	m_pBCL->Verify( "EYEDENTITYGAMES_DRAGONNEST_20091102", "e740e5a4" );
	m_pEtDevice = pDev;
	m_hWnd = hWnd;

	if( wszDefaultPath ) {
		m_szDefaultPath = wszDefaultPath;
	}

	return true;
}

void CEtBCLEngine::Finalize()
{
	if( m_pBCL == NULL ) return;
	m_pBCL->Destroy();
	SAFE_DELETE( m_pBCL );
}

void CEtBCLEngine::SetOption( CaptureOptionEnum Option )
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	BCapConfigPreset( &m_Config, (BCAP_PRESET)Option );
	m_Config.IncludeCursor = TRUE;
	m_pBCL->CheckConfig( &m_Config );
	m_pBCL->SetConfig( &m_Config );
}

void CEtBCLEngine::RefreshConfig()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	m_pBCL->CheckConfig( &m_Config );
	m_pBCL->SetConfig( &m_Config );
}

void CEtBCLEngine::Start( TCHAR *szFileName )
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	TCHAR szResultName[256] = { 0, };

	if( szFileName == NULL ) {
		__time64_t ltime;
		TCHAR szBuf[256] = { 0, };
		struct	tm tm_ptr;
		time(&ltime);
		_localtime64_s( &tm_ptr, &ltime );

		wcsftime( szBuf, 256, L"%Y-%m-%d %H-%M-%S %a", &tm_ptr );

		wsprintf( szResultName, L"DN %s.avi", szBuf );
		m_szSaveFileName = szResultName;
		wsprintf( szResultName, L"%s\\DN %s.avi", m_szDefaultPath.c_str(), szBuf );
	}
	else {
		m_szSaveFileName = szFileName;
		if( !( szFileName[1] == L':' || szFileName[1] == L'\\' ) ) {
			wsprintf( szResultName, L"%s\\%s", m_szDefaultPath.c_str(), szFileName );
		}
	}

	m_pBCL->Start( szResultName, NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)m_pEtDevice->GetDevicePtr() );
	m_pPrevPrePresentCallback = ( void ( __stdcall * )() )m_pEtDevice->SetPrePresentCallback( PrePresentCallback );
}

void CEtBCLEngine::Stop()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	m_pEtDevice->SetPrePresentCallback( m_pPrevPrePresentCallback );
	m_pBCL->Stop();
}

bool CEtBCLEngine::IsCapturing()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return false;

	return ( m_pBCL->IsCapturing() == TRUE ) ? true : false;
}

int CEtBCLEngine::GetCaptureTime()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return 0;

	return m_pBCL->GetCaptureTime();
}

INT64 CEtBCLEngine::GetCaptureFileSize()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return 0;

	return m_pBCL->GetCaptureFileSize();
}

void CEtBCLEngine::SetPriority( int nPriority )
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	m_pBCL->SetPriority( nPriority );
}

int CEtBCLEngine::GetPriority()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return 0;

	return m_pBCL->GetPriority();
}

void CEtBCLEngine::SetMinMaxFPS( int nMin, int nMax )
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	m_pBCL->SetMinMaxFPS( nMin, nMax );
}

void CEtBCLEngine::GetMinMaxFPS( int &nMin, int &nMax )
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	m_pBCL->GetMinMaxFPS( &nMin, &nMax );
}

void __stdcall CEtBCLEngine::PrePresentCallback()
{
	if( CEtBCLEngine::GetInstance().m_pPrevPrePresentCallback )
	{
		CEtBCLEngine::GetInstance().m_pPrevPrePresentCallback();
	}
	if( CEtBCLEngine::IsActive() && CEtBCLEngine::GetInstance().IsCapturing() ) {
		CEtBCLEngine::GetInstance().Work();

		if( CEtBCLEngine::s_pCaptureInfoCallback ) CEtBCLEngine::s_pCaptureInfoCallback();
	}
}

void CEtBCLEngine::Work()
{
	if( !m_pBCL || !m_pBCL->IsCreated() ) return;

	HRESULT hResult = m_pBCL->Work( (LONG_PTR)m_pEtDevice->GetDevicePtr() );
	if( FAILED( hResult ) ) {
		CaptureErrorEnum ErrorCode = ErrorUnknown;
		switch( hResult ) {
			case BCERR_VIDEO_CAPTURE: ErrorCode = ErrorVideo; break;
			case BCERR_AUDIO_CAPTURE: ErrorCode = ErrorAudio; break;
			case BCERR_FILE_CREATE: ErrorCode = ErrorCreateFile; break;
			case BCERR_FILE_WRITE: ErrorCode = ErrorWriteFile; break;
			case BCERR_NOT_ENOUGH_DISKSPACE: ErrorCode = ErrorNotEnoughDiskSpace; break;
		}
		SendMessage( m_hWnd, UM_CAPTURE_ERROR, ErrorCode, 0 );
	}
}