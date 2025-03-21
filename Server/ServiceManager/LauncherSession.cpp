
#include "stdafx.h"
#include "LauncherSession.h"
#include "DNServerPacket.h"
#include "ServiceManager.h"
#include "PatchWatcher.h"
#include "Log.h"

extern TServiceManagerConfig g_Config;

CLauncherSession::CLauncherSession()
{
	Init(1024 * 100, 1024 * 100);
	m_bProcessFlag = false;
	m_nLauncherPingTick = 0;
	m_nCreateTick = timeGetTime();
	m_bBatchFlag = true;
}

CLauncherSession::~CLauncherSession()
{
}

int CLauncherSession::GetCmdSize(const WCHAR * pKey)
{
	int nCnt = 0;
	
	return nCnt;
}

int CLauncherSession::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	switch (iMainCmd)
	{
		case SERVICE_LAUNCHER_CREATED_PROCESS:
		{
			int nSID;
			memcpy(&nSID, pData, iLen);

			if (g_pServiceManager->CreateServiceServer(nSID) == false)
			{
				g_Log.Log(LogType::_FILELOG, L"UnManged service Create Report SID[%d]\n", nSID);
			}
			return ERROR_NONE;
		}

		case SERVICE_LAUNCHER_PROCESS_TERMINATED:
		{
			int nSID;
			memcpy(&nSID, pData, iLen);			

			if (g_pServiceManager->ReportTerminateService(nSID) == false)
			{
				g_Log.Log(LogType::_FILELOG, L"UnManged NetLauncher Terminated NID[%d]\n", nSID);
			}
			return ERROR_NONE;
		}

		case SERVICE_LAUNCHER_VERSION:
		{
			TServiceLauncherVersion * pPacket = (TServiceLauncherVersion*)pData;

			std::string strCmd;
			strCmd = pPacket->szVersion;
			ToLowerA(strCmd);

			std::vector<std::string> tokens;
			TokenizeA(strCmd, tokens, ".");

			int nMajorVersion = atoi(tokens[0].c_str());
			int nMinorVersion = atoi(tokens[1].c_str());

			if (nMajorVersion != g_pServiceManager->m_nMajorVersion)
			{
				//g_pServiceManager->StopEachLauncher(GetSessionID());
				g_Log.Log(LogType::_FILELOG, L"NetLauncher Version MisMatch ServiceManager[%S] NetLauncher[%S] IP[%S]\n", g_Config.szVersion, pPacket->szVersion, GetIp());
				//g_pServiceManager->SendPacketToGsm(_T("LauncherVersionMisMatch,%d,%d.%d"),GetSessionID(), nMajorVersion, nMinorVersion);
			}
			return ERROR_NONE;
		}

		case SERVICE_LAUNCHER_PROCESS_STATE:
		{
			TServerProcessState * pPacket = (TServerProcessState*)pData;

			if (!strstr(pPacket->szState, "PatchStart"))
			{
				if (strstr(pPacket->szState, "fail") || strstr(pPacket->szState, "Fail"))
					g_Log.Log(LogType::_ERROR, L"IP[%S],%S\n", GetIp(), pPacket->szState);
				else
					g_Log.Log(LogType::_FILELOG, L"IP[%S],%S\n", GetIp(), pPacket->szState);
			}

			if (pPacket->cPatch == 1)
				m_bProcessFlag = !strcmp(pPacket->szState, "PatchStart") ? true : false;

			return ERROR_NONE;
		}

		case SERVICE_LAUNCHER_PATCH_STATE:
		{
			TPatchState * pPacket = (TPatchState*)pData;

			if (pPacket->cLenKey + pPacket->cLenState >= 128) return ERROR_NONE;

			char szState[128];
			char szKey[64];
			_strcpy(szState, pPacket->szState, pPacket->cLenState);
			_strcpy(szKey, pPacket->szState + pPacket->cLenState, pPacket->cLenKey);

			WCHAR wszKey[64];
			MultiByteToWideChar(CP_ACP, 0, szKey, -1, wszKey, 64);

			WCHAR wszState[128];
			MultiByteToWideChar(CP_ACP, 0, szState, -1, wszState, 128);

			CPatchWatcher * pWatcher = g_pServiceManager->GetPatchWatcher();
			bool bComplete = false;
#if defined(_SERVICEMANAGER_EX)
			bool bNIDComplete = false;
			pWatcher->DelWatchContent(pPacket->nPatchID, GetSessionID(), wszKey, bComplete, bNIDComplete);
#else
			pWatcher->DelWatchContent(pPacket->nPatchID, GetSessionID(), wszKey, bComplete);
#endif

			if (pPacket->bFlag)
			{ 
			}
			else
			{
#if defined (_SERVICEMANAGER_EX)
				g_pServiceManager->SetPatchComplete(false);
				g_pServiceManager->OnPatchFail(GetSessionID(), wszState);				
#endif 
				g_Log.Log(LogType::_ERROR, L"msg=PatchFail,%S,IP[%S]\n", szState, GetIp());			
			}
#if defined (_SERVICEMANAGER_EX)
			if( bNIDComplete )
			{
				g_pServiceManager->OnPatchCompleted(GetSessionID());
			}
#endif

			if (wcsstr(wszKey, L"Url"))
			{
				if (g_pServiceManager->CompleteLivePatchItem(pPacket->nPatchID) == false)
					g_Log.Log(LogType::_NORMAL, L"Url Patched\n");
			}
			else if (bComplete)
			{
				g_Log.Log(LogType::_NORMAL, L"Patch Completed\n");
#if defined (_SERVICEMANAGER_EX)
				g_pServiceManager->OnPatchEnd();
#endif // #if defined (_SERVICEMANAGER_EX)
			}
			
			return ERROR_NONE;
		}
		case SERVICE_LAUNCHER_ESM_STATE:
			{
				TServerEsmState * pPacket = (TServerEsmState*)pData;
				if (strstr(pPacket->szState, "fail") || strstr(pPacket->szState, "Fail"))
					g_Log.Log(LogType::_ERROR, L"IP[%S],%S\n", GetIp(), pPacket->szState);
				else
					g_Log.Log(LogType::_FILELOG, L"IP[%S],%S\n", GetIp(), pPacket->szState);

				return ERROR_NONE;
			}
#if defined (_SERVICEMANAGER_EX)
		case SERVICE_LAUNCHER_PATCH_PROGRESS:
			{
				TPatchProgress* pPacket = (TPatchProgress*)pData;

				wchar_t key[32];
				::memset(key, 0, sizeof(key));
				::wcsncpy(key, pPacket->key, pPacket->keyLen);

				g_pServiceManager->OnPatchProgress(GetSessionID(), key, pPacket->progress, pPacket->progressMax);
				return ERROR_NONE;
			}

		case SERVICE_LAUNCHER_UNZIP_PROGRESS:
			{
				TUnzipProgress* pPacket = (TUnzipProgress*)pData;

				wchar_t filename[MAX_PATH + 1];
				::memset(filename, 0, sizeof(filename));
				::wcsncpy(filename, pPacket->filename, pPacket->len);

				g_pServiceManager->OnUnzipProgress(GetSessionID(), filename, pPacket->progress, pPacket->progressMax);
				return ERROR_NONE;
			}
#endif // #if defined (_SERVICEMANAGER_EX)

		case SERVICE_LAUNCHER_PING:
		{
			TSendPing* pPacket = (TSendPing*)pData;
			CDataManager::GetInstance()->SetLauncherPublicIP(GetSessionID(), pPacket->szPublicIP);
			m_nLauncherPingTick = timeGetTime();
			return ERROR_NONE;
		}

		case SERVICE_LAUNCHER_RETURN_PING:
		{
			TReturnPing * pPacket = (TReturnPing*)pData;
			g_pServiceManager->LauncherReturn(GetSessionID(), pPacket->nIdx, pPacket->nTick);
			return ERROR_NONE;
		}
		case SERVICE_LAUNCHER_RUNBATCH_STATE:
		{
			TRunBatchState * pPacket = (TRunBatchState*) pData;

			CRunBatchWatcher * pWatcher = g_pServiceManager->GetRunBatchWatcher();
			bool bComplete = false;
		
			if (pPacket->nRet == 0)
			{
				SetBatch (true);		// 배치성공
				pWatcher->DelWatchSpec(pPacket->nBatchID, GetSessionID(), L"RunBat", bComplete);
			}
			else if (pPacket->nRet == 1)
				g_Log.Log(LogType::_ERROR, L"IP[%S], RunBat Fail to Exec\n", GetIp());
						
			if (bComplete)
				g_Log.Log(LogType::_NORMAL, L"All Batch Completed\n");
			
			return ERROR_NONE;
		}

		case SERVICE_LAUNCHER_STOPBATCH_STATE:
			{
				TStopBatchState * pPacket = (TStopBatchState*) pData;

				CStopBatchWatcher * pWatcher = g_pServiceManager->GetStopBatchWatcher();
				bool bComplete = false;

				pWatcher->DelWatchSpec(pPacket->nBatchID, GetSessionID(), L"StopBat", bComplete);
				SetBatch (false);

				if (bComplete)
					g_Log.Log(LogType::_NORMAL, L"All Stop Batch Completed\n");

				return ERROR_NONE;
			}

		case SERVICE_LAUNCHER_LIVEEXTCOPY:
			{
				TLiveExtCopy * pPacket = (TLiveExtCopy*)pData;				
				g_pServiceManager->CompleteLiveExtCopy(pPacket->bCopyRet);
			}
	}
	return ERROR_GENERIC_UNKNOWNERROR;
}

void CLauncherSession::SendManagingExeItem(std::vector <TServerExcuteData> * vExeList, bool bOtherPath)
{
	TServiceStartManage Manage;
	memset(&Manage, 0, sizeof(TServiceStartManage));
	Manage.bOther = bOtherPath;
	AddSendData(NETSERVICE_STARTOF_MANAGING, 0, (char*)&Manage, sizeof(TServiceStartManage));

	TServiceManaging packet;
	char szTemp[EXCUTELENMAX + EXCUTECMDMAX];
	std::vector <TServerExcuteData>::iterator ii;
	for (ii = vExeList->begin(); ii != vExeList->end(); ii++)
	{
		memset(&packet, 0, sizeof(TServiceManaging));
		memset(szTemp, 0, sizeof(szTemp));

		packet.nSID = (*ii).nSID;
		packet.cCreateCount = (BYTE)(*ii).nCreateCount;
		packet.cCreateIndex = (BYTE)(*ii).nCreateIndex;

		WideCharToMultiByte(CP_ACP, 0, (*ii).wstrExcuteData.c_str(), -1, szTemp, EXCUTELENMAX + EXCUTECMDMAX, NULL, NULL);
		packet.nCmdSize = (USHORT)strlen(szTemp);
		if (packet.nCmdSize >= EXCUTELENMAX + EXCUTECMDMAX)
		{
			_ASSERT_EXPR(0, L"[SendManagingExeItem] packet.nCmdSize >= EXCUTELENMAX + EXCUTECMDMAX");
			continue;
		}
		strcpy_s(packet.szCmd, szTemp);

		AddSendData(NETSERVICE_MANAGING, 0, (char*)&packet, sizeof(packet) - sizeof(packet.szCmd) + ((packet.nCmdSize)));
	}
	AddSendData(NETSERVICE_ENDOF_MANAGING, 0, NULL, 0);
}

void CLauncherSession::SendConnectedResult(std::list <char> * pList, const WCHAR * pBaseUrl, const WCHAR * pPatchUrl)
{
	TNetResult packet;
	memset(&packet, 0, sizeof(packet));

	if (MANAGEDMAX < pList->size()) 
		_ASSERT_EXPR(0, L"[SendConnectedResult] MANAGEDMAX < pList->size()");
	for (int i = 0; i < MANAGEDMAX; i++)
		packet.cAssingedType[i] = -1;
	int cnt;
	std::list <char>::iterator ii;
	for (ii = pList->begin(), cnt = 0; ii != pList->end(); ii++, cnt++)
		packet.cAssingedType[cnt] = (*ii);
	wcscpy_s(packet.szBaseUrl, pBaseUrl);
	wcscpy_s(packet.szPatchUrl, pPatchUrl);
	
	AddSendData(NETSERVICE_RESULT, 0, (char*)&packet, sizeof(packet));
}

void CLauncherSession::SendRunProcess(int nSID, int nCreateCount, int nCreateIndex, const WCHAR * pType, const WCHAR * pCmd, bool bForcePatch)
{
	TRunProcess packet;
	memset(&packet, 0, sizeof(packet));

	packet.nSID = nSID;

	AddSendData(NETSERVICE_RUN_PROCESS, 0, (char*)&packet, sizeof(packet));
}

void CLauncherSession::SendTerminateProcess()
{
	AddSendData(NETSERVICE_TERMINATE_ALLPROCESS, 0, NULL, 0);
}

void CLauncherSession::SendTerminateProcess(int nSID, bool bRestart/*=false*/)
{
	TTerminateProcess packet;
	memset(&packet, 0, sizeof(packet));

	packet.nSID = nSID;
	packet.bRestart = bRestart;

	AddSendData(NETSERVICE_TERMINATE_PROCESS, 0, (char*)&packet, sizeof(packet));

}

void CLauncherSession::SendStartESM()
{
	AddSendData(NETSERVICE_STARTESM, 0, NULL, 0);
}

void CLauncherSession::SendStopESM()
{
	AddSendData(NETSERVICE_STOPESM, 0, NULL, 0);
}

bool CLauncherSession::SendPatch(const TCHAR * pKey, int nPatchResLevel, int nPatchID)
{
	TPatch packet;
	memset(&packet, 0, sizeof(packet));

	_tcscpy_s(packet.szKey, pKey);
	packet.nPatchResLevel = nPatchResLevel;
	packet.nPatchID = nPatchID;

	if (AddSendData(NETSERVICE_PATCH, 0, (char*)&packet, sizeof(packet)) < 0)
		return false;
	return true;
}

bool CLauncherSession::SendPatchByUrl(int nPatchID, const char * pUrl, const WCHAR * pKey, bool bOnlyUseURL, const WCHAR * pDest)
{
	if (pUrl == NULL) return false;
	if (strlen(pUrl) >= 1024) return false;

	TPatchUrl packet;
	memset(&packet, 0, sizeof(packet));

	if (bOnlyUseURL)
	{
		packet.bOnlyUseURL = true;
		if (pDest == NULL)
			return false;
		STRCPYW(packet.szDest, 64, pDest);
	}

	wcscpy_s(packet.szKey, pKey);
	packet.nPatchID = nPatchID;
	strcpy_s(packet.szUrl, pUrl);
	packet.nLen = (short)strlen(pUrl);

	if (AddSendData(NETSERVICE_PATCH_BYURL, 0, (char*)&packet, sizeof(packet) - sizeof(packet.szUrl) + packet.nLen) < 0)
		return false;
	return true;
}

void CLauncherSession::SendPatchStart()
{
	AddSendData(NETSERVICE_PATCHSTART, 0, NULL, 0);
}

void CLauncherSession::SendPatchEnd()
{
	AddSendData(NETSERVICE_PATCHEND, 0, NULL, 0);
}

void CLauncherSession::SendPatchApply()
{
	AddSendData(NETSERVICE_PATCHAPPLY, 0, NULL, 0);
}

void CLauncherSession::SendInfoCopy(bool bNeedOtherPath)
{
	TInfoCopy packet;
	memset(&packet, 0, sizeof(TInfoCopy));

	packet.bNeed = bNeedOtherPath;

	AddSendData(NETSERVICE_INFOCOPY, 0, (char*)&packet, sizeof(TInfoCopy));
}
bool CLauncherSession::SendReturnPing(int nIdx)
{
	TReturnPing Ping;
	Ping.nIdx = nIdx;
	Ping.nTick = timeGetTime();
	if (AddSendData(NETSERVICE_RETURN_PING, 0, (char*)&Ping, sizeof(TReturnPing)) < 0)
		return false;
	return true;
}

void CLauncherSession::SendBatchRun(int nBatchID)
{
	TRunBatch packet;
	packet.nBatchID = nBatchID;

	AddSendData(NETSERVICE_RUNBATCH, 0, (char*)&packet, sizeof(packet));
}

void CLauncherSession::SendBatchStop(int nBatchID)
{
	TStopBatch packet;
	packet.nBatchID = nBatchID;
	
	AddSendData(NETSERVICE_STOPBATCH, 0, (char*)&packet, sizeof(packet));
}

void CLauncherSession::SendLiveExtDel()
{
	//여러가지 종류가되면 종류별로 목록을 보내서 따로따로 처리 가능토록 하자
	AddSendData(NETSERVICE_LIVEEXTDEL, 0, NULL, 0);
}

void CLauncherSession::SendLiveExtCopy()
{
	//여러가지 종류가되면 종류별로 목록을 보내서 따로따로 처리 가능토록 하자
	AddSendData(NETSERVICE_LIVEEXTCOPY, 0, NULL, 0);
}