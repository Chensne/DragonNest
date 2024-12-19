
#pragma once

#include "SimpleClientSession.h"
#include "URLUpdater.h"

class CPatchStatusChecker;

class	CLauncherSession : public CSimpleClientSession
{
public :
	static CLauncherSession * GetInstance();

	bool Start(const TCHAR *ip, unsigned short port);
	void Stop();

	void SendVersion(const char * pVersion);
	void SendCrashEvent(int nSID);
	void SendPatchState(int nPatchID, const char * pKey, const char * pState, bool bFlag);
	void SendPatchProgress(const wchar_t* key, unsigned long progress, unsigned long progressMax);
	void SendUnzipProgress(const wchar_t* filename, unsigned long progress, unsigned long progressMax);
	void SendPing();
	void SendRunBatchState(int nRet, int nBatchID);
	void SendStopBatchState(int nBatchID);
	void SendLiveExtCopy(bool bRet);

	void OnUnzip(const wchar_t* filename, unsigned long progress, unsigned long progressMax);

protected :
	bool AddPacket(const DNTPacket * packet);
	bool ParsePacket(unsigned char main, unsigned char sub,const void * ptr, int len);

	void Connect();
	void Destroy();
	void TimeEvent();

	bool CheckSrcPath(const TCHAR * szPath);
	bool CopyFolder(const TCHAR * srcPath, const TCHAR * destPath);
	bool CopyFolder(std::string strSrc, std::string strDest);

private :
	CLauncherSession();
	~CLauncherSession();

	static DWORD WINAPI _threadmain(void* param);

	void ThreadMain();
	HANDLE m_hThread;
	volatile bool m_bTerminated;

	TCHAR m_szIP[64];
	unsigned short m_iPort;

	//patch
	bool m_bIsNowPatch;			//패치중
	bool m_bIsPatched;			//패치가 되었으나 재시작 되었는지 (스타트 리스타트 시에만 체크 시작하면 다시 false로)
	
	void MakePatchPath();
	void EmptyDirectory(const char * pPath);
	void SwapSlash(const TCHAR * pSrc, TCHAR * pDest);

	CURLUpdater m_UrlUpdater;
	std::vector <int> m_vManagedTypeList;
	WCHAR m_wszBaseURL[256];
	WCHAR m_wszPatchURL[256];

	bool m_bOtherPath;

	CPatchStatusChecker* m_pPatchStatusChecker;

	//Parse
	static DWORD WINAPI _threadparse(void* param);

	void ThreadParse();
	HANDLE m_hThreadParse;
	HANDLE m_hEventParse;

	CLfhHeap m_Mem;
	std::list <char *> m_lData;
	CSyncLock m_Sync;	

	bool _InitSession();

	void	_GetHostIPAddress();
	std::string	m_strPublicIP;
	std::string	m_strPrivateIP;
} ;