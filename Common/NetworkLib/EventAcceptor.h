#pragma once

#if defined(_SERVER) && !defined(_USE_ACCEPTEX)

#include "Thread.h"

class CIocpManager;
class CListenSocket;
class EventAcceptor : public Thread
{
public:
	EventAcceptor(CIocpManager* pIOCP, const char* szThreadNick = "EventAcceptor" );
	virtual	~EventAcceptor();

	int				Open(const int nKey, u_short nPortNo, int nBackLogCount = SOMAXCONN );
	void			Close();
	virtual	void	Run();

private:
	CIocpManager*	m_pIOCP;	

	HANDLE						m_hAddListenPort;	
	HANDLE						m_hEndThread;	
	CSyncLock					m_Lock;

	std::list<CListenSocket*>	m_listListenSocket;	

	void InitListenEvent(HANDLE* hEvents, int& nEventCount );
	void ProcessEvent(HANDLE* hEvents, int nEventCount);

private:
	EventAcceptor();
};

inline void TraceGetLastError()
{
	int nResult = WSAGetLastError();
	TCHAR Buf[512] = {NULL};
	::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, nResult, LANG_SYSTEM_DEFAULT, Buf, 512, NULL );
	OutputDebugString(Buf);
}

#endif	// #if defined(_SERVER) && !defined(_USE_ACCEPTEX)
