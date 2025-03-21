
#pragma once

#include "SimpleClientSession.h"
#include "URLUpdater.h"

class CLauncherPatcherSession : public CSimpleClientSession
{
public:
	static CLauncherPatcherSession * GetInstance();
	
	bool Start(const TCHAR *ip, unsigned short port);
	void Stop();

	void SendPatchState(bool bPatch);
	void SendPing();

protected :
	bool AddPacket(const DNTPacket * packet);
	bool ParsePacket(unsigned char main, unsigned char sub,const void * ptr, int len);
	void Connect();
	void Destroy();
	void TimeEvent();

private :
	CLauncherPatcherSession();
	~CLauncherPatcherSession();

	static DWORD WINAPI _threadmain(void* param);

	void ThreadMain();
	HANDLE m_hThread;
	volatile bool m_bTerminated;

	TCHAR m_szIP[64];
	unsigned short m_iPort;

	CURLUpdater m_UrlUpdater;
	WCHAR m_wszBaseURL[256];
	WCHAR m_wszPatchURL[256];

	//Parse
	static DWORD WINAPI _threadparse(void* param);

	void ThreadParse();
	HANDLE m_hThreadParse;
	HANDLE m_hEventParse;

	CLfhHeap m_Mem;
	std::list <char *> m_lData;
	CSyncLock m_Sync;	

	bool _InitSession();
};