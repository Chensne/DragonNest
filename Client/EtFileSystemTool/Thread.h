#pragma once

class Thread
{
public:
	Thread();
	virtual ~Thread();

	bool		Start();
	bool		Terminate( DWORD nExitCode );


	bool		WaitForTerminate( DWORD nTimeout = INFINITE );


protected:
	virtual	void Run() = 0;

	HANDLE		m_hHandleThread;
	DWORD		m_nThreadID;
	bool		m_bThreadLoop;

private:

	static	DWORD	WINAPI	_Runner( LPVOID pParam );
};
