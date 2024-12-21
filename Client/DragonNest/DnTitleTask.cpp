#include "StdAfx.h"
#include "DnTitleTask.h"
#include "DnInterfaceCamera.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnMainFrame.h"
#include "EtPrimitiveDraw.h"
#include "DnLoadingTask.h"
#include "DnServiceSetup.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

char *CDnTitleTask::s_szMovieName[] = {
	"Publisher.avi",
	"Eyedentity.avi",
	"Grade.avi",
};
/*
#if defined(_KR)
char *CDnTitleTask::s_szMovieName[] = {
"Publisher.avi",
"Eyedentity.avi",
"Grade.avi",
};

#elif defined(_CH) || defined(_JP)
char *CDnTitleTask::s_szMovieName[] = {
"Publisher.avi",
"Eyedentity.avi",
};

#else
char *CDnTitleTask::s_szMovieName[] = {
"Eyedentity.avi",
};

#endif
*/


CDnTitleTask::CDnTitleTask()
: CInputReceiver( true )
, m_nLogoStep( -1 )
, m_bPlayMovie( false )
, m_bFinishPreoadLogin( false )
, m_pLoginTask( NULL )
, m_nHoundLoading( 0 )
, m_nForceAddHoundLoadingValue( 0 )
, m_dwForceAddTime( 0 )
, m_fForceAddTimeDelta( 0.0f )
{
}

CDnTitleTask::~CDnTitleTask()
{
	SAFE_RELEASE_SPTR( m_hCamera );
	GetInterface().Finalize( CDnInterface::Title );
}

bool CDnTitleTask::Initialize()
{
	m_hCamera = (new CDnInterfaceCamera)->GetMySmartPtr();
	CDnCamera::SetActiveCamera( m_hCamera );

	m_pBVLEngine.reset(new CEtBVLEngine);
	if (m_pBVLEngine->Initialize( CDnMainFrame::GetInstance().GetHWnd(), &CEtDevice::GetInstance() ) == false)
		m_pBVLEngine.reset();

	if( CDnLoadingTask::IsActive() ) {
		CDnLoadingTask::GetInstance().EnableBackgroundLoading( true );
		CDnLoadingTask::GetInstance().InsertLoadObject( OnLoadLoginTasktCallback, NULL, NULL, NULL, (void*)this, NULL, 0, 0 );
	}

	if ( g_pServiceSetup )
		g_pServiceSetup->OnEvent(1, "WiseLogEvent");

	return true;
}

void CDnTitleTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnMouseCursor::GetInstance().ShowCursor( CGameOption::GetInstance().m_bWindow, true );
	int nCount = sizeof(s_szMovieName) / sizeof(char *);

#ifndef _FINAL_BUILD
	static DWORD dwTest1;
	static DWORD dwTest2;
#endif

	if( !m_bPlayMovie ) {
		if( m_nLogoStep < nCount - 1 ) {
			m_nLogoStep++;
#ifndef _FINAL_BUILD
			if( m_nLogoStep == 0 ) dwTest1 = GetTickCount();
#endif

			if( m_pBVLEngine ) {
				m_pBVLEngine->Close();
				if( m_pBVLEngine->Open( CEtResourceMng::GetInstance().GetFullName( s_szMovieName[m_nLogoStep] ).c_str() ) ) {
					m_pBVLEngine->Play();
					m_bPlayMovie = true;
				}
			}
		}
		else if( m_nLogoStep == nCount - 1 ) {
			if( m_pBVLEngine ) {
				m_pBVLEngine->Close();
				if( m_pBVLEngine->Open( CEtResourceMng::GetInstance().GetFullName( "TitleLoading.avi" ).c_str() ) ) {
					m_pBVLEngine->Play( true );
					m_bPlayMovie = true;
				}
			}
			m_nLogoStep++;
		}
	}
	else {
		m_bPlayMovie = ( m_pBVLEngine ) ? m_pBVLEngine->IsPlaying() : false;
	}

	if( m_nLogoStep == nCount )
		ProcessHoundLoading( LocalTime, fDelta );
	if( m_pBVLEngine ) m_pBVLEngine->Process( LocalTime, fDelta, m_nHoundLoading );

#ifndef _FINAL_BUILD
	if( GetAsyncKeyState( VK_LSHIFT ) )
	{
		CGlobalValue::GetInstance().m_bMessageBoxWithMsgID = true;
	}
#endif

	if( m_nLogoStep == nCount ) {
		if( m_bFinishPreoadLogin ) {
#ifndef _FINAL_BUILD
			dwTest2 = GetTickCount();
			dwTest1 = dwTest2 - dwTest1;
			char szTemp[128] = {0,};
			sprintf_s( szTemp, _countof(szTemp), "\n=============================\nTitle Loading Time = %d\n", dwTest1 );
			OutputDebug( szTemp );
#endif

			if( m_pLoginTask->Initialize( CDnLoginTask::Logo ) == false ) {
				SAFE_DELETE( m_pLoginTask );
				OutputDebug( "Error - Can't Initialize Login Task\n" );
			}
			else {
				CTaskManager::GetInstance().AddTask( m_pLoginTask, "LoginTask", -1, false );
			}

//#ifndef _FINAL_BUILD
			if( CGlobalInfo::GetInstance().m_bUITest ) {
				// PreloadLogin이 끝나면 로그인태스크가 실행되는데 이때 덤프나는지 테스트하면 된다.
				GetInterface()._TestUIDump( this );
				GetInterface().MessageBox( L"ui테스트완료! uitest.txt파일을 확인하세요.", MB_OK, MESSAGEBOX_10, CDnBridgeTask::GetInstancePtr(), true, false, false, true );
			}
//#endif	// #ifndef _FINAL_BUILD

			DestroyTask( true );
		}
		return;
	}
}

bool __stdcall CDnTitleTask::OnLoadLoginTasktCallback( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime )
{
	CDnTitleTask *pTask = (CDnTitleTask *)pThis;
	pTask->PreLoadLoginTask();
	return true;
}

void CDnTitleTask::PreLoadLoginTask()
{
	EtInterface::g_bEtUIMsgProc = false;
	if( CDnMainFrame::GetInstance().ThreadInitialize() == false ) { // 여기서 false 나는경우는 Table 을 못읽었을떄바께없어요. 무조껀 죽입니다.
		_exit(0);
		return;
	}
	m_pLoginTask = new CDnLoginTask;
	m_pLoginTask->SetTaskName( "LoginTask" );	// DnInterface에서 Task이름을 기억하기 위해 강제로 미리 넣어둡니다.
	m_pLoginTask->PreInitialize();
	m_bFinishPreoadLogin = true;

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	CDnMainFrame::GetInstance().ThreadInitializePreloadPlayer();
#endif

	EtInterface::g_bEtUIMsgProc = true;
}


bool CDnTitleTask::RenderScreen( LOCAL_TIME LocalTime, float fDelta )
{
	return !m_bFinishPreoadLogin;
}

void CDnTitleTask::CheckPointHoundLoading( int nValue )
{
	// 사실 제대로 하려면, TableDB쪽이나 GameDataManager쪽안에다가 로딩하는 테이블 및 맵 개수 확인 후 진행량을 계산해야햐는데,
	// 이렇게 하면 다른 코드들 사이사이에 지저분한 로딩체크 함수가 들어가므로, 최대한 간소화 하기로 했다.
	if( m_nHoundLoading < nValue )
		m_nHoundLoading = nValue;

	m_nForceAddHoundLoadingValue = 0;
	m_dwForceAddTime = 0;
}

void CDnTitleTask::ForceAddHoundLoading( int nValue, DWORD dwTime )
{
	// 자연스럽게 올라가는 것처럼 보이게 하려고 만든 함수.
	m_nForceAddHoundLoadingValue = nValue;
	m_dwForceAddTime = dwTime / nValue;
	m_fForceAddTimeDelta = m_dwForceAddTime / 1000.0f;
}

void CDnTitleTask::ProcessHoundLoading( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nForceAddHoundLoadingValue > 0 )
	{
		m_fForceAddTimeDelta -= fDelta;
		if( m_fForceAddTimeDelta < 0.0f )
		{
			m_nHoundLoading++;
			if( m_nHoundLoading > 99 )
				m_nHoundLoading = 99;
			m_nForceAddHoundLoadingValue--;

			if( m_nForceAddHoundLoadingValue > 0 )
				m_fForceAddTimeDelta += m_dwForceAddTime / 1000.0f;
		}
	}
}