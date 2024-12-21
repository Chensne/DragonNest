#include "stdafx.h"
#include "BatchManager.h"
#include "LauncherSession.h"
#include <io.h>

CBatchManager::CBatchManager()
{
	m_hThread = INVALID_HANDLE_VALUE;
	m_BatchList.clear();
	m_bForceStop = false;
}

CBatchManager::~CBatchManager()
{

}

CBatchManager * CBatchManager::GetInstance()
{
	static CBatchManager s;
	return &s;
}

bool CBatchManager::RunBatch (char* szPath, int nBatchID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	STARTUPINFO si;
	PROCESS_INFORMATION pi = {0,};

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_NORMAL;

	if (_access(szPath, 0) != -1)
	{
		//if (!system(szBatchPath)) -- ��ġ���� ������ ���������ؾ� �ϴ� ��Ȳ�� ����Ͽ� CreateProcess�� ��ü�մϴ�.
		if (!CreateProcess (NULL, szPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			return false;
	}

	// ���μ������� ����
	sBatchInfo info;
	info.dwProcessID = pi.dwProcessId;
	info.hProcess = pi.hProcess;
	info.hThread = pi.hThread;
	info.nBatchID = nBatchID;

	m_BatchList.push_back(info);

	return true;
}

void CBatchManager::TerminateAll()
{

	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector <sBatchInfo>::iterator ii;
	for (ii = m_BatchList.begin(); ii != m_BatchList.end(); ii++)
	{
		DWORD dwExit;
		BOOL bRet = GetExitCodeProcess((*ii).hProcess, &dwExit);			
		TerminateProcess((*ii).hProcess, bRet == TRUE ? dwExit : false);
		CloseHandle((*ii).hProcess);
		if ((*ii).hThread > 0)
			CloseHandle((*ii).hThread);
	}

	m_BatchList.clear ();
}

DWORD WINAPI CBatchManager::_threadmain(void* param)
{
	((CBatchManager*)param)->ThreadMain();
	return 0;
}

void CBatchManager::ThreadMain ()
{
	HANDLE hList[MAXIMUM_WAIT_OBJECTS];
	unsigned int i;
	std::vector <sBatchInfo>::iterator ii;
	sBatchInfo temp;	

	while(m_bForceStop == false)
	{
		m_Sync.Lock();
		for(i = 0; i < m_BatchList.size(); i++)
			hList[i] = m_BatchList[i].hProcess;
		m_Sync.UnLock();

		if (i > 0)
		{
			DWORD ret;
			ret = WaitForMultipleObjects(i, hList, FALSE, 2*1000);
			if (ret != WAIT_TIMEOUT)
			{

				m_Sync.Lock();
				memset(&temp, 0, sizeof(sBatchInfo));
				for(ii = m_BatchList.begin(); ii != m_BatchList.end(); ii++)
				{
					if ((*ii).hProcess == hList[ret - WAIT_OBJECT_0])
					{
						temp = (*ii);
						m_BatchList.erase(ii);

						CloseHandle(temp.hProcess);
						if (temp.hThread > 0)
							CloseHandle(temp.hThread);

						
						// ��������� SM�� �����Ѵ�.
						CLauncherSession::GetInstance()->SendRunBatchState(0, temp.nBatchID);

						break;
					}
				}
				m_Sync.UnLock();
			}
		}	else
		{
			Sleep(2*1000);
		}
	}
	m_hThread = 0;
}

void CBatchManager::Start()
{
	DWORD threadid;
	m_bForceStop = false;
	m_hThread = CreateThread(NULL, 0, _threadmain, (void*)this, 0, &threadid);
}

void CBatchManager::Stop()
{
	if (m_hThread != NULL)
	{
		m_bForceStop = true;
		WaitForSingleObject(m_hThread, INFINITE);

		ScopeLock <CSyncLock> sync(m_Sync);
		std::vector <sBatchInfo>::iterator ii;
		for (ii = m_BatchList.begin(); ii != m_BatchList.end(); ii++)
		{
			DWORD dwExit;
			BOOL bRet = GetExitCodeProcess((*ii).hProcess, &dwExit);			
			TerminateProcess((*ii).hProcess, bRet == TRUE ? dwExit : false);
			CloseHandle((*ii).hProcess);
			if ((*ii).hThread > 0)
				CloseHandle((*ii).hThread);
		}
		m_BatchList.clear();
	}
}
