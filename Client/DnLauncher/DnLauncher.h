// DnLauncher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
class CDnLauncherDlg;

// CDnLauncherApp:
// See DnLauncher.cpp for the implementation of this class
//
class CDnLauncherApp : public CWinApp
{
public:
	CDnLauncherApp();
	~CDnLauncherApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
	BOOL InitLauncher();			// 런처 초기화
	BOOL InitFail();				// 초기화 실패시 에러메세지 및 로그 기록
	BOOL CheckDirectXVersion();		// DirectX 버전 체크
	void DeleteOldLogFile();		// 오래된 로그파일 삭제
	void UseLogFile();				// 로그파일 사용 할 경우 초기화
	void UseLogWindow();			// 로그윈도우 사용 할 경우 초기화
	BOOL RemoveUsedFile();			// 런처 구동 시 사용 했던 파일 및 런처 업데이트 시 생성된 tmp 파일 삭제
	void ParseCommandLine();		// 커맨드 라인 파싱
	
#ifdef _FIRST_PATCH
	BOOL InitFirstPatchThread();	// FirstPatch 스레드 생성.
	void TerminateFirstPatchThread();	// FirstPatch 스레드 종료.
#endif

#ifdef _USE_MULTILANGUAGE
	void SetLanguageParam();
#endif // _USE_MULTILANGUAGE
};

extern CDnLauncherApp theApp;

enum emInitErrorCode
{
	INIT_ERROR_CLIENT_ALEADY_RUN = 1,
	INIT_ERROR_DX_VER,
	INIT_ERROR_SERVICE_MODULE,
	INIT_ERROR_REMOVE_FILE,
	INIT_ERROR_PATCHCONFIGLIST_DOWNLOAD,
	INIT_ERROR_PATCHCONFIGLIST_PARSING,
	INIT_ERROR_INTERFACE,
	INIT_ERROR_WEBPAGE,
	INIT_ERROR_SET_PATCHINFO,
	INIT_ERROR_SET_PARAMETER,
	INIT_ERROR_COMMAND_LINE,
	INIT_ERROR_FIRSTPATCH_THREAD_CREATE_FAILED
};