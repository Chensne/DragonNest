
#pragma once

#include <vector>
#include "windows.h"
#include "Tlhelp32.h"

struct sProcessItem
{
	int nID;
	bool bRestartFlag;
	char szExcutePath[MAX_PATH];
	char szExeName[MAX_PATH];
	char szWorkingPath[MAX_PATH];
	char szCmdParam[EXCUTELENMAX + EXCUTECMDMAX];
};

struct sProcessInfo
{
	int nID;
	DWORD dwProcessID;
	HANDLE hProcess;
	HANDLE hThread;
	TCHAR szProcessName[MAX_PATH];
	ULONG nCreateTick;
	bool bRestart;
};

struct sSnapShotItem
{
	DWORD nProcessID;
	TCHAR szProcessName[MAX_PATH];
};

class CProcessManager
{
public:
	CProcessManager();
	virtual ~CProcessManager();

	static CProcessManager * GetInstance();

	void Start();
	void Stop();

	void ClearManagingItem() { m_ManagingItems.clear(); }
	bool AddManagingItem(int nID, const char * pszExcutePath, const char * pszExe, const char * pszCmdParam, const char * pszWorkingPath);
	void MakeWatchingProcess();

	bool RunProcess(int nID, bool bRestart = false);
	bool Terminate(int nID);
	void TerminateAll();

	bool IsRunProcess(int nID);
	bool IsRunProcess(const char * szpProcessName);
	bool GetExeName(int nID, char * pszNameOut);

	//Process SnapShot
	bool SnapShotProcessList();

	bool CheckOwnProcess();
	bool GetSnapshotProcessName(DWORD nProcessID, TCHAR * pName);
	int FindSnapShotProcessCount(const TCHAR * szName);
	void GetExutePath(const char * pExe, char * pExcutePath);
	void GetExutePath(char * pExcutePath);
	bool RunByName (char* command);
	bool TerminateByName (char* strProcessName);

private:
	CSyncLock m_Sync;

	std::map <int, sProcessItem> m_ManagingItems;
	const sProcessItem * GetManagingItemAsync(int nID);

	std::vector <sProcessInfo> m_WatchingItems;
	void AddWatchingProcessAsync(HANDLE hProcess, HANDLE hThread, DWORD dwProcessID, int nID, const TCHAR * pExeName, ULONG nCreateTick, bool bRestart=false);
	bool IsWatchingProcessAsync(int nID);
	HANDLE GetProcessHandle(DWORD nProcessID);

	//SnapShot
	std::vector <sSnapShotItem> m_SnapShotList;

	//Thread
	bool m_bForceStop;
	void ThreadMain();
	HANDLE m_hThread;
	static DWORD WINAPI _threadmain(void* param);
};