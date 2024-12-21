#include "StdAfx.h"
#include "DnFirstPatchInfo.h"
#include "DnPatchInfo.h"
#include "EtFileSystem.h"
#include "DnPatchThread.h"

extern CDnFistPatchDownloadThread * g_pFirstPatchDownloadThread;

DnFirstPatchInfo::DnFirstPatchInfo(void) :
m_bIsChangedLauncherFile(false),
m_nLocalModuleVersion(0),
m_nServerModuleVersion(0),
m_nFirstPatchStatus(EM_FIRSTPATCH_OK),
m_nIsFirstPatchProcess(EM_FIRSTPATCH_NOT)
{
	m_vecCopyList.clear();
	m_vecDeleteList.clear();
}

DnFirstPatchInfo::~DnFirstPatchInfo(void)
{
	if(g_pFirstPatchDownloadThread)
	{
		g_pFirstPatchDownloadThread->TerminateThread();
		g_pFirstPatchDownloadThread->WaitForTerminate();

		SAFE_DELETE(g_pFirstPatchDownloadThread);
	}
}

DnFirstPatchInfo& DnFirstPatchInfo::GetInstance()
{
	static DnFirstPatchInfo global;
	return global;
}

int DnFirstPatchInfo::SetPatchInfo()
{
	LogWnd::TraceLog(_T("모듈패치 - 스레드 시작."));

	// [!] txt파싱에 성공하면, 스레드 시작.
	if( g_pFirstPatchDownloadThread == NULL )
	{
		g_pFirstPatchDownloadThread = new CDnFistPatchDownloadThread();
		if(g_pFirstPatchDownloadThread)
		{
			return g_pFirstPatchDownloadThread->Start();
		}
	}

	LogWnd::TraceLog(_T("모듈패치 실패."), FIRSTPATCHLIST_NAME);
	LogWnd::Log( LogLevel::Error, _T("g_pFirstPatchDownloadThread Create Failed") );
	return false;
}