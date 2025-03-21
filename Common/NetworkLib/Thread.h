#pragma once

class Thread
{
public:
	Thread(const char* szThreadNick);
	virtual ~Thread();

	bool		Start();
	bool		Terminate( DWORD nExitCode );
	bool		WaitForTerminate( DWORD nTimeout = INFINITE );
	DWORD_PTR		SetThreadAffinityMask( DWORD dwMask );

protected:
	virtual	void Run() = 0;

	HANDLE		m_hThreadHandle;
	unsigned int		m_nThreadID;
	bool		m_bRunning;
	char		m_szThreadNick[128];

private:
	
	static	unsigned int	WINAPI	_Runner( void * pParam );
	static  void SetThreadName( DWORD dwThreadID, const char* szThreadName);

private:

	// 
	typedef struct tagTHREADNAME_INFO 
	{ 
		DWORD dwType;		// must be 0x1000 
		LPCSTR szName;		// pointer to name (in user addr space) 
		DWORD dwThreadID;	// thread ID (-1=caller thread) 
		DWORD dwFlags;		// reserved for future use, must be zero 
	} THREADNAME_INFO; 

};
