#pragma once

class NxThread
{
public:
	NxThread(const char* szThreadNick);
	virtual ~NxThread();

	bool		Start();
	bool		Terminate( DWORD nExitCode );


	bool		WaitForTerminate( DWORD nTimeout = INFINITE );

protected:
	virtual	void Run() = 0;

	HANDLE		m_hThreadHandle;
	DWORD		m_nThreadID;
	bool		m_bRunning;

	char		m_szThreadNick[128];

private:

	static	DWORD	WINAPI	_Runner( LPVOID pParam );
	static  void SetThreadName( DWORD dwThreadID, const char* szThreadName);

private:

	// 
	typedef struct tagTHREADNAME_INFO 
	{ 
		DWORD dwType;  // must be 0x1000 
		LPCSTR szName; // pointer to name (in user addr space) 
		DWORD dwThreadID; // thread ID (-1=caller thread) 
		DWORD dwFlags; // reserved for future use, must be zero 
	} THREADNAME_INFO; 

};
