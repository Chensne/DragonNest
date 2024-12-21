
#pragma once

#include "Connection.h"
#include "CriticalSection.h"
#include "DataManager.h"

class CLauncherSession : public CConnection
{
public:
	CLauncherSession();
	~CLauncherSession();

	int GetCmdSize(const WCHAR * pKey);

	int MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen);

	void SendManagingExeItem(std::vector <TServerExcuteData> * vExeList, bool bOtherPath);
	void SendConnectedResult(std::list <char> * pList, const WCHAR * pBaseUrl, const WCHAR * pPatchUrl);
	void SendRunProcess(int nSID, int nCreateCount, int nCreateIndex, const WCHAR * pType, const WCHAR * pCmd, bool bForcePatch);
	void SendTerminateProcess();
	void SendTerminateProcess(int nSID, bool bRestart=false);
	void SendCloseService(int nSID);
	void SendStartESM();
	void SendStopESM();

	bool SendPatch(const TCHAR * pKey, int nPatchResLevel, int nPatchID);
	bool SendPatchByUrl(int nPatchID, const char * pUrl, const WCHAR * pKey, bool bOnlyUseURL = false, const WCHAR * pDest = NULL);

	void SendPatchStart();
	void SendPatchEnd();
	void SendPatchApply();
	void SendInfoCopy(bool bNeedOtherPath);
	bool SendReturnPing(int nIdx);
	void SendBatchRun(int nBatchID);
	void SendBatchStop(int nBatchID);
	void SendLiveExtDel();
	void SendLiveExtCopy();

	bool IsBatch() {return m_bBatchFlag;}
	void SetBatch (bool bFlag) {m_bBatchFlag = bFlag;}

	bool IsProcess() { return m_bProcessFlag; }
	ULONG GetPingTick() { return m_nLauncherPingTick; }
	ULONG GetCreateTick() { return m_nCreateTick; }

private:
	volatile bool m_bProcessFlag;

	ULONG m_nLauncherPingTick;
	ULONG m_nCreateTick;

	bool m_bBatchFlag;
};