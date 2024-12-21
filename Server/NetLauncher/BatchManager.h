
#pragma once

struct sBatchInfo
{
	DWORD dwProcessID;
	HANDLE hProcess;
	HANDLE hThread;
	int	nBatchID;
};

class CBatchManager
{
public:
	CBatchManager();
	virtual ~CBatchManager();

	static CBatchManager * GetInstance();

	bool RunBatch(char* szPath, int nBatchID);
	void TerminateAll();

	void Start();
	void Stop();

private:
	CSyncLock m_Sync;
	
	std::vector <sBatchInfo> m_BatchList;

	bool	m_bForceStop;
	HANDLE	m_hThread;

private:
	static DWORD WINAPI _threadmain(void* param);
	
	void ThreadMain();
};