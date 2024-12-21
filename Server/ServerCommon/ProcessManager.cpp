
#include "stdafx.h"
#include "ProcessManager.h"

#if defined(_LAUNCHER) || defined(_ESM)
#ifdef _LAUNCHER
#include "LauncherSession.h"
#include "ServiceUtil.h"
#elif _ESM
#include "LauncherPatcherSession.h"
#endif
extern void TextOut(const TCHAR * format, ...);
#endif
#include "psapi.h"

CProcessManager::CProcessManager()
{
	m_hThread = NULL;
	m_bForceStop = false;
}

CProcessManager::~CProcessManager()
{
	Stop();
}

CProcessManager * CProcessManager::GetInstance()
{
	static CProcessManager s;
	return &s;
}

void CProcessManager::Start()
{
	DWORD threadid;
	m_bForceStop = false;
	m_hThread = CreateThread(NULL, 0, _threadmain, (void*)this, 0, &threadid);
}

void CProcessManager::Stop()
{
	if (m_hThread != NULL)
	{
		m_bForceStop = true;
		WaitForSingleObject(m_hThread, INFINITE);

		ScopeLock <CSyncLock> sync(m_Sync);
		std::vector <sProcessInfo>::iterator ii;
		for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
		{
			DWORD dwExit;
			BOOL bRet = GetExitCodeProcess((*ii).hProcess, &dwExit);			
			TerminateProcess((*ii).hProcess, bRet == TRUE ? dwExit : false);
			CloseHandle((*ii).hProcess);
			if ((*ii).hThread > 0)
				CloseHandle((*ii).hThread);
		}
		m_WatchingItems.clear();
	}
}

bool CProcessManager::AddManagingItem(int nID, const char * pszExcutePath, const char * pszExe, const char * pszCmdParam, const char * pszWorkingPath)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::map <int, sProcessItem>::iterator ii = m_ManagingItems.find(nID);
	if (ii == m_ManagingItems.end())
	{
		sProcessItem Item;
		memset(&Item, 0, sizeof(sProcessItem));

		Item.nID = nID;
		if (pszExcutePath)
		{
			strcpy_s(Item.szExcutePath, pszExcutePath);
			FolderCheckAndCreate(pszExcutePath);
		}
		strcpy_s(Item.szExeName, pszExe);
		if (pszCmdParam)
			strcpy_s(Item.szCmdParam, pszCmdParam);
		if (pszWorkingPath)		
			strcpy_s(Item.szWorkingPath, pszWorkingPath);
		
		Item.bRestartFlag = true;

		std::pair<std::map <int, sProcessItem>::iterator, bool> Ret = m_ManagingItems.insert(std::make_pair(nID, Item));
		if (Ret.second == false)
			return false;
		return true;
	}
	return false; 
}

void CProcessManager::MakeWatchingProcess()
{
	if (SnapShotProcessList())
	{
		ScopeLock <CSyncLock> sync(m_Sync);
		std::map <int, sProcessItem>::iterator ii;
		for (ii = m_ManagingItems.begin(); ii != m_ManagingItems.end(); ii++)
		{
			std::vector <sSnapShotItem>::iterator ih;
			for (ih = m_SnapShotList.begin(); ih != m_SnapShotList.end(); ih++)
			{
				if (!stricmp((*ii).second.szExeName, (*ih).szProcessName))
				{
					HANDLE hProcess = GetProcessHandle((*ih).nProcessID);
					AddWatchingProcessAsync(hProcess, 0, (*ih).nProcessID, (*ii).second.nID, (*ii).second.szExeName, 0);
				}
			}
		}
	}
	else
		_ASSERT_EXPR(0, L"!SnapShotProcessList()");
}

bool CProcessManager::RunProcess(int nID, bool bRestart/*=false*/)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (IsWatchingProcessAsync(nID)) return true;
	const sProcessItem * pItem = GetManagingItemAsync(nID);
	if (pItem == NULL) return false;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	TCHAR command[4096];
	TCHAR title[512];

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_NORMAL;

	std::vector<std::string> tokens;
	TokenizeA(pItem->szExeName, tokens, ".");

	_stprintf_s(title, _T("%s_%d"), tokens[0].c_str(), nID);
	si.lpTitle = title;
	ZeroMemory( &pi, sizeof(pi) );

	if (strlen(pItem->szExcutePath) > 0)
		_stprintf_s(command, _T("\"%s\\%s\" %s"), pItem->szExcutePath, pItem->szExeName, pItem->szCmdParam);
	else
		_stprintf_s(command, _T("\"%s\" %s"), pItem->szExeName, pItem->szCmdParam);

	if (!CreateProcess(NULL,	// No module name (use command line). 
		command,			// Command line. 
		NULL,				// Process handle not inheritable. 
		NULL,				// Thread handle not inheritable. 
		FALSE,				// Set handle inheritance to FALSE. 
		0,					// No creation flags. 
		NULL,				// Use parent's environment block. 
#ifdef _ESM
		NULL,
#else		//#ifdef _ESM
		strlen(pItem->szWorkingPath) > 0 ? pItem->szWorkingPath : NULL,				// Use parent's starting directory. 
#endif		//#ifdef _ESM
		&si,				// Pointer to STARTUPINFO structure.
		&pi)				// Pointer to PROCESS_INFORMATION structure.
		) 
	{
		DWORD Ret = GetLastError();
#if defined(_LAUNCHER)
		TextOut(_T("Execute Failed '%s %s' ErrCode%d"), pItem->szExeName, pItem->szCmdParam, Ret);
#endif
		return false;
	}

#if defined(_LAUNCHER)
	TextOut(_T("Execute Success (%s %s):%d"), pItem->szExeName, pItem->szCmdParam, pItem->nID);
#endif
	

	AddWatchingProcessAsync(pi.hProcess, pi.hThread, pi.dwProcessId, pItem->nID, pItem->szExeName, timeGetTime(), bRestart);
	return true;
}


bool CProcessManager::Terminate(int nID)
{
	{
		ScopeLock <CSyncLock> sync(m_Sync);

		std::vector <sProcessInfo>::iterator ii;
		for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
		{
			if ((*ii).nID == nID)
			{
				DWORD dwExit;
				BOOL bRet = GetExitCodeProcess((*ii).hProcess, &dwExit);			
				TerminateProcess((*ii).hProcess, bRet == TRUE ? dwExit : 0);
				CloseHandle((*ii).hProcess);
				if ((*ii).hThread > 0)
					CloseHandle((*ii).hThread);
				m_WatchingItems.erase(ii);
				return true;
			}
		}
	}
	return true;
}

void CProcessManager::TerminateAll()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::vector <sProcessInfo>::iterator ii;
	for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
	{
		DWORD dwExit;
		BOOL bRet = GetExitCodeProcess((*ii).hProcess, &dwExit);			
		TerminateProcess((*ii).hProcess, bRet == TRUE ? dwExit : false);
		CloseHandle((*ii).hProcess);
		if ((*ii).hThread > 0)
			CloseHandle((*ii).hThread);
	}
	m_WatchingItems.clear();
}

bool CProcessManager::IsRunProcess(int nID)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector <sProcessInfo>::iterator ii;
	for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
	{
		if ((*ii).nID == nID)
			return true;
	}
	return false;
}

bool CProcessManager::IsRunProcess(const char * szpProcessName)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector <sProcessInfo>::iterator ii;
	for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
	{
		if (!stricmp((*ii).szProcessName, szpProcessName))
			return true;
	}
	return false;
}

bool CProcessManager::GetExeName(int nID, char * pszNameOut)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::map <int, sProcessItem>::iterator ii = m_ManagingItems.find(nID);
	if (ii != m_ManagingItems.end())
	{
		strcpy_s(pszNameOut, MAX_PATH, (*ii).second.szExeName);
		return true;
	}
	return false;
}

bool CProcessManager::SnapShotProcessList()
{
	m_Sync.Lock();
	m_SnapShotList.clear();
	m_Sync.UnLock();

	HANDLE hProcessSnap = NULL;
	bool bReturn = false;

	PROCESSENTRY32 pe32         = {0};

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return false;

	pe32.dwSize = sizeof(PROCESSENTRY32);


	if (Process32First(hProcessSnap, &pe32))
	{
		DWORD Code = 0;
		DWORD         dwPriorityClass;

		do
		{
			HANDLE hProcess;
			// Get the actual priority class.
			hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			dwPriorityClass = GetPriorityClass (hProcess);

			sSnapShotItem item;
			memset(&item, 0, sizeof(sSnapShotItem));

			_tcscpy_s(item.szProcessName, pe32.szExeFile);
			item.nProcessID = pe32.th32ProcessID;

			m_Sync.Lock();
			m_SnapShotList.push_back(item);
			m_Sync.UnLock();

			CloseHandle(hProcess);
		}

		while (Process32Next(hProcessSnap, &pe32));
		bReturn = true;
	}
	else
		bReturn = false;

	CloseHandle (hProcessSnap);
	return bReturn;
}

bool CProcessManager::CheckOwnProcess()
{
	int nCnt = 0;
	DWORD nProcessID = GetCurrentProcessId();

	TCHAR szName[MAX_PATH];
	bool bRet = GetSnapshotProcessName(nProcessID, szName);
	if (bRet)
	{
		int nCount = FindSnapShotProcessCount(szName);
		if (nCount >= 2)
			return true;
	}
	return false;	
}

bool CProcessManager::GetSnapshotProcessName(DWORD nProcessID, TCHAR * pName)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::vector <sSnapShotItem>::iterator ii;
	for (ii = m_SnapShotList.begin(); ii != m_SnapShotList.end(); ii++)
	{
		if ((*ii).nProcessID == nProcessID)
		{
			_tcscpy_s(pName, MAX_PATH, (*ii).szProcessName);
			return true;
		}
	}
	return false;
}

int CProcessManager::FindSnapShotProcessCount(const TCHAR * szName)
{
	ScopeLock <CSyncLock> sync(m_Sync);
	int nCount = 0;
	std::vector <sSnapShotItem>::iterator ii;
	for (ii = m_SnapShotList.begin(); ii != m_SnapShotList.end(); ii++)
	{
		if (!_tcscmp((*ii).szProcessName, szName))
			nCount++;
	}
	return nCount;
}

void CProcessManager::GetExutePath(const char * pExe, char * pExcutePath)
{
	if (pExcutePath == NULL) return;

	ScopeLock <CSyncLock> sync(m_Sync);		
	for (std::map <int, sProcessItem>::iterator ii = m_ManagingItems.begin(); ii != m_ManagingItems.end(); ii++)
	{
		if (!stricmp((*ii).second.szExeName, pExe))
		{
			if (strlen((*ii).second.szExcutePath) > 0)
				strcpy_s(pExcutePath, 512, (*ii).second.szExcutePath);
			return;
		}
	}
}

void CProcessManager::GetExutePath(char * pExcutePath)
{
	if (pExcutePath == NULL) return;

	ScopeLock <CSyncLock> sync(m_Sync);		
	for (std::map <int, sProcessItem>::iterator ii = m_ManagingItems.begin(); ii != m_ManagingItems.end(); ii++)
	{
		if (strlen((*ii).second.szExcutePath) > 0)
		{
			strcpy_s(pExcutePath, 512, (*ii).second.szExcutePath);
			return;
		}
	}
}

const sProcessItem * CProcessManager::GetManagingItemAsync(int nID)
{
	std::map <int, sProcessItem>::iterator ii = m_ManagingItems.find(nID);
	if (ii != m_ManagingItems.end())
		return &(*ii).second;
	return NULL;
}

void CProcessManager::AddWatchingProcessAsync(HANDLE hProcess, HANDLE hThread, DWORD dwProcessID, int nID, const TCHAR * pExeName, ULONG nCreateTick, bool bRestart/*=false*/)
{
	//Check
	std::vector <sProcessInfo>::iterator ii;
	for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
	{
		if ((*ii).dwProcessID == dwProcessID)
			return;
	}

	sProcessInfo Item;
	memset(&Item, 0, sizeof(sProcessInfo));

	Item.hProcess = hProcess;
	Item.hThread = hThread;
	Item.dwProcessID = dwProcessID;
	Item.nID = nID;
	_tcscpy_s(Item.szProcessName, pExeName);
	Item.nCreateTick = nCreateTick;
	Item.bRestart = bRestart;

	m_WatchingItems.push_back(Item);
}

bool CProcessManager::IsWatchingProcessAsync(int nID)
{
	std::vector <sProcessInfo>::iterator ii;
	for (ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
	{
		if ((*ii).nID == nID)
			return true;
	}
	return false;
}

HANDLE CProcessManager::GetProcessHandle(DWORD nProcessID)
{
	HANDLE hProcess = 0;
	hProcess = ::OpenProcess(
		PROCESS_ALL_ACCESS, // Specifies all possible access flags
		FALSE,
		nProcessID//pProcess->Get_ProcessId()
		);
	if( !hProcess )
		return 0;
	else
		return hProcess;
}

DWORD WINAPI CProcessManager::_threadmain(void* param)
{
	((CProcessManager*)param)->ThreadMain();
	return 0;
}

void CProcessManager::ThreadMain()
{
	HANDLE hList[MAXIMUM_WAIT_OBJECTS];
	unsigned int i;
	std::vector <sProcessInfo>::iterator ii;
	sProcessInfo temp;	

	while(m_bForceStop == false)
	{
		m_Sync.Lock();
		for(i = 0; i < m_WatchingItems.size(); i++)
			hList[i] = m_WatchingItems[i].hProcess;
		m_Sync.UnLock();

		if (i > 0)
		{
			DWORD ret;
			ret = WaitForMultipleObjects(i, hList, FALSE, 2*1000);
			if (ret != WAIT_TIMEOUT)
			{
				m_Sync.Lock();
				memset(&temp, 0, sizeof(sProcessInfo));
				for(ii = m_WatchingItems.begin(); ii != m_WatchingItems.end(); ii++)
				{
					if ((*ii).hProcess == hList[ret - WAIT_OBJECT_0])
					{
						temp = (*ii);
						if (temp.bRestart)
						{
							(*ii).bRestart = false;
							continue;
						}
						m_WatchingItems.erase(ii);

						CloseHandle(temp.hProcess);
						if (temp.hThread > 0)
							CloseHandle(temp.hThread);

#if defined(_LAUNCHER)
						TextOut(_T("Application Crash! (%s):%d\n"), temp.szProcessName, temp.nID);
						if (temp.nID != 0)
						{
							CLauncherSession::GetInstance()->SendCrashEvent(temp.nID);
						}
#endif
						break;
					}
				}
				m_Sync.UnLock();

				if (temp.nID > 0)
				{
					const sProcessItem * pItem = GetManagingItemAsync(temp.nID);
					if (pItem)
					{
						if (pItem->bRestartFlag && (temp.nCreateTick > 0 && temp.nCreateTick + 5000 < timeGetTime()))
						{
							Sleep(3*1000);
							RunProcess(temp.nID);
						}
					}
				}
			}
		}	else
		{
			Sleep(2*1000);
			//CLauncherSession::GetInstance()->SendPing();
		}
	}
	m_hThread = 0;
}

bool CProcessManager::RunByName (char* command)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	ZeroMemory( &pi, sizeof(pi) );

	if (!CreateProcess(NULL, command, NULL,	NULL, FALSE, 0, NULL, NULL, &si, &pi)) 
		return false;
	
	return true;
}
bool CProcessManager::TerminateByName (char* strProcessName)
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ))
		return false;

	cProcesses = cbNeeded / sizeof(DWORD);

	for ( i = 0; i < cProcesses; i++ )
	{
		char szProcessName[MAX_PATH] = "unknown";

		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE, aProcesses[i] );

		if (NULL != hProcess )
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
			{
				GetModuleBaseNameA( hProcess, hMod, szProcessName, sizeof(szProcessName) );
			}
		}
		
		if(strcmp(szProcessName, strProcessName)==0)
		{
			HANDLE hSameProcess = NULL;

			hSameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcesses[i]);
			if(hSameProcess != NULL)
			{
				DWORD    ExitCode = 0;
				BOOL bRet = GetExitCodeProcess(hSameProcess, &ExitCode);
				if (bRet == FALSE)
					ExitCode = 0;
				bRet = ::TerminateProcess(hSameProcess, ExitCode );
				if( bRet )
				{
					::WaitForSingleObject(hSameProcess, INFINITE);
				} 

				::CloseHandle(hSameProcess);
				return true;
			}
		}
		CloseHandle( hProcess );
	}

	return false;
}