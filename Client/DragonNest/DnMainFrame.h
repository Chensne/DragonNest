#pragma once

#include "Timer.h"
#include "Singleton.h"
#ifdef PRE_ADD_STEAMWORKS
#include "../../Extern/Steamworks/public/steam/steam_api.h"
#endif // PRE_ADD_STEAMWORKS

class CInputDevice;
class CTaskManager;
class CDnTableDB;
class CTask;
class CEtSoundEngine;
class CClientSessionManager;
class CDnInterface;
class CEtUIXML;
class CDnInCodeResource;
class CDnLoadingTask;
class CDnBridgeTask;
class CDnHangCrashTask;
class CEtBCLEngine;
class CDnRecordDlg;
class CSyncTimer;
class CDnUISound;
class CDnAuthTask;
class CEtBVLEngine;
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
class CNameAutoComplete;
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#ifdef PRE_ADD_DWC
class CDnDWCTask;
#endif // PRE_ADD_DWC

class CDnMainFrame : public CSingleton<CDnMainFrame> {
public:
	CDnMainFrame( HINSTANCE hInst );
	~CDnMainFrame();

	// Window 생성 전에 할것들 Hacking tool 관련 체크등등..
	bool PreInitialize();

	// 윈도 생성 & 게임 내 초기화할 것들
	bool InitializeWindow();

	// Engine 생성
	bool PreInitializeDevice();
	bool InitializeDevice();
	bool ReInitializeDevice();
	bool FinalizeDevice();

	// 게임 생성
	bool ThreadInitialize();
	bool Initialize();
	bool Finalize();

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	bool ThreadPreloadPlayer();
#endif
#ifdef PRE_ADD_STEAMWORKS
	STEAM_CALLBACK( CDnMainFrame, OnSteamOverlayActivated, GameOverlayActivated_t, m_CallbackGameOverlayActivated );
#endif // PRE_ADD_STEAMWORKS
	// Kernel 메인 루프
	bool Execute();
	LRESULT WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	HWND GetHWnd() { return m_hWnd; }
	HINSTANCE GetHInst() { return m_hInstance; }

	static LPTOP_LEVEL_EXCEPTION_FILTER s_PrevExceptionFilter;
	static LONG	__stdcall UnhandledExceptionFilter( _EXCEPTION_POINTERS* pExceptionInfo );

	void DestroyMainFrame() { m_bDestroy = true; }
	void SaveScreenShotToFile( D3DXIMAGE_FILEFORMAT Format = D3DXIFF_JPG );

	bool IsBeginCaption() { return m_bBeginCaption; }
	CDnRecordDlg *GetRecordDlg() { return m_pRecordDlg; }

	static void	__cdecl BadAllocFilter();
	static void __stdcall DrawCaptureStateCallback();

	DWORD GetMainThreadID() { return m_dwMainThreadID; }
	void FlushWndMessage();

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	bool IsFinishPreloadPlayer() {
		ScopeLock<CSyncLock> Lock(m_PreLoadLock);
		return m_bFinishPlayerPreload; 
	}
	void SetFinishPreload( bool bFinish ) { 
		ScopeLock<CSyncLock> Lock(m_PreLoadLock);
		m_bFinishPlayerPreload = bFinish; 
	}
	void ThreadInitializePreloadPlayer();
	void FlushPreloadPlayer();
#endif

	void SaveWindowPos();
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	CNameAutoComplete* GetNameAutoComplete() { return m_pNameAutoComplete; }
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE

#ifdef PRE_ADD_RELOAD_ACTFILE
	void AddResourcePathByFolderName( CFileNameString& szPath );
	void ReLoadActionFilePath();
#endif 

protected:
	bool CheckRecordDiskSpace( bool bBegin );

	void ToggleRecord();
	void RecordBegin();
	void RecordEnd();

	bool IMEProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
#ifndef _FINAL_BUILD
	void TemporaryKeyProc();
#endif // _FINAL_BUILD

protected:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	CTaskManager *m_pTaskMng;
	CInputDevice *m_pInputDevice;
	CEtSoundEngine *m_pSoundEngine;
	CDnUISound *m_pUISound;
	CDnMouseCursor *m_pCursor;
	CDnInterface *m_pInterface;
	CDnInCodeResource *m_pInCodeRes;
	CEtUIXML *m_pUIXML;
	CEtResourceMng *m_pResMng;
	CSyncTimer *m_pSyncTimer;

	CEtBCLEngine *m_pCaptureEngine;
	CDnRecordDlg *m_pRecordDlg;
	bool m_bCheckRecordDiskSpace[4];
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	CNameAutoComplete *m_pNameAutoComplete;
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	CEtExceptionalUIXML* m_pExceptionalUIXML;
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING

	/*std::vector<DWORD> m_dwVecTaskHandle;*/	// 안쓰는거같아서 지웁니다.
	CDnTableDB *m_pTableDB;
	CClientSessionManager *m_pSessionMng;

	CDnLoadingTask *m_pLoadingTask;
	CDnBridgeTask *m_pBridgeTask;
	CDnHangCrashTask *m_pHangCrashTask;
	CDnAuthTask *m_pAuthTask; 
#ifdef PRE_ADD_DWC
	CDnDWCTask *m_pDWCTask;
#endif

	bool m_bDestroy;

	POINT m_MousePos;
	POINT m_WindowPos;
	bool m_bBeginCaption;
	bool m_bCreateWindow;
	bool m_bMaximize;
	DWORD m_dwMainThreadID;

	int	m_nWinVer;

	struct DnOutOfMemoryFunc : public FnDeviceOutOfMemory
	{
		virtual void Run();
	};

	DnOutOfMemoryFunc m_DnOutOfMemoryFunc;

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	static bool __stdcall FlushWaitDelete();
#else
	HANDLE m_hPreloadPlayerThreadHandle;
	unsigned m_dwPreloadPlayerThreadID;
	bool m_bFinishPlayerPreload;
	DnActorHandle m_hPreLoadPlayerClass[10];
	CSyncLock m_PreLoadLock;
#endif
};
 