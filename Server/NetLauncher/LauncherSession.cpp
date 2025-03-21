
#include "stdafx.h"
#include "launchersession.h"
#include "ProcessManager.h"
#include "ftpupdater.h"

#if defined(PRE_UNZIP_CHANGE)
#include "UnzipProcess.h"
#else
#include "unzipper.h"
#endif
#include "IniFile.h"
#include "urlmon.h"
#include "StringUtil.h"
#include "Version.h"
#include "Directory.h"
#include "BatchManager.h"
#include "PatchStatusChecker.h"

extern void TextOut(const TCHAR * format, ...);
extern TNetLauncherConfig g_Config;

CLauncherSession::CLauncherSession()
{
	m_hThread = 0;
	m_bTerminated = false;
	m_bIsNowPatch = false;
	m_bIsPatched = false;
	m_bOtherPath = false;

	MakePatchPath();

	m_pPatchStatusChecker = new CPatchStatusChecker(this);

	m_hEventParse = CreateEvent(NULL, false, false, NULL);
}


CLauncherSession::~CLauncherSession()
{
	if (m_hThread != 0)
		WaitForSingleObject(m_hThread, INFINITE);

	if (m_hThreadParse != 0)
		WaitForSingleObject(m_hThreadParse, INFINITE);

	CloseHandle(m_hEventParse);
	m_hEventParse = NULL;

	delete m_pPatchStatusChecker;
}

bool CLauncherSession::Start(const TCHAR *ip, unsigned short port)
{
	if (_InitSession() == false)
		return false;
	
	_tcscpy_s(m_szIP, ip);
	m_iPort = port;

	_GetHostIPAddress();

	DWORD threadid;
	TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Success"), ip, port);
	m_hThread = CreateThread(NULL, 0, _threadmain, (void*)this, 0, &threadid);

	m_hThreadParse = CreateThread(NULL, 0, _threadparse, (void*)this, 0, &threadid);
	return true;
}


void CLauncherSession::Stop()
{
	m_bTerminated = true;
	CSimpleClientSession::Stop();
}


void CLauncherSession::ThreadMain()
{
	if (CSimpleClientSession::Start(m_szIP, m_iPort) == false)
		TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Failed"), m_szIP, m_iPort);

	while (m_bTerminated == false)
	{
		Sleep(1000);
		if (CSimpleClientSession::Start(m_szIP, m_iPort) == false)
			TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Failed"), m_szIP, m_iPort);
		else
			TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Success"), m_szIP, m_iPort);
	}
	
	m_hThread = 0;
}


DWORD WINAPI CLauncherSession::_threadmain(void* param)
{
	((CLauncherSession*)param)->ThreadMain();
	return 0;
}


CLauncherSession * CLauncherSession::GetInstance()
{
	static CLauncherSession s;
	return &s;
}

void CLauncherSession::SendVersion(const char * pVersion)
{
	TServiceLauncherVersion packet;
	memset(&packet, 0, sizeof(TServiceLauncherVersion));

	if (pVersion)
		strcpy_s(packet.szVersion, pVersion);

	SendPacket(SERVICE_LAUNCHER_VERSION, &packet, sizeof(packet));
}

void CLauncherSession::SendCrashEvent(int nSID)
{
	SendPacket(SERVICE_LAUNCHER_PROCESS_TERMINATED, &nSID, sizeof(nSID));
}

void CLauncherSession::SendRunBatchState(int nRet, int nBatchID)
{
	TRunBatchState packet;

	packet.nRet = nRet;
	packet.nBatchID = nBatchID;

	SendPacket(SERVICE_LAUNCHER_RUNBATCH_STATE, &packet, sizeof(packet));

}

void CLauncherSession::SendStopBatchState(int nBatchID)
{
	TStopBatchState packet;
	packet.nBatchID = nBatchID;

	SendPacket(SERVICE_LAUNCHER_STOPBATCH_STATE, &packet, sizeof(packet));

}

void CLauncherSession::SendLiveExtCopy(bool bRet)
{
	TLiveExtCopy packet;
	packet.bCopyRet = bRet;

	SendPacket(SERVICE_LAUNCHER_LIVEEXTCOPY, &packet, sizeof(packet));
}

void CLauncherSession::SendPatchState(int nPatchID, const char * pKey, const char * pState, bool bFlag)
{
	if (pState == NULL || pKey == NULL) return;
	TPatchState	packet;
	memset(&packet, 0, sizeof(packet));
	
	packet.bFlag = bFlag;
	packet.cLenState = (BYTE)strlen(pState);
	packet.cLenKey = (BYTE)strlen(pKey);

	if (packet.cLenKey + packet.cLenState >= sizeof(packet.szState))
		return;

	strcpy_s(packet.szState, pState);
	_strcpy(packet.szState + packet.cLenState, pKey, packet.cLenKey);
	packet.nPatchID = nPatchID;
	
	SendPacket(SERVICE_LAUNCHER_PATCH_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (packet.cLenState + packet.cLenKey));
}

void CLauncherSession::SendPatchProgress(const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	TPatchProgress packet;
	memset(&packet, 0, sizeof(packet));

	::wcscpy_s(packet.key, key);
	packet.keyLen = (BYTE)::wcslen(packet.key);
	packet.progress = progress;
	packet.progressMax = progressMax;

	SendPacket(SERVICE_LAUNCHER_PATCH_PROGRESS, &packet, sizeof(packet) - sizeof(packet.key) + (packet.keyLen * sizeof(wchar_t)));
}

void CLauncherSession::SendUnzipProgress(const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	TUnzipProgress packet;
	memset(&packet, 0, sizeof(packet));

	::wcscpy_s(packet.filename, filename);
	packet.len = (short)::wcslen(packet.filename);
	packet.progress = progress;
	packet.progressMax = progressMax;

	SendPacket(SERVICE_LAUNCHER_UNZIP_PROGRESS, &packet, sizeof(packet) - sizeof(packet.filename) + (packet.len * sizeof(wchar_t)));
	
}

void CLauncherSession::SendPing()
{
	TSendPing packet;
	memset(&packet, 0, sizeof(packet));
	
	strcpy_s (packet.szPublicIP, IPLENMAX, m_strPublicIP.c_str());
	SendPacket(SERVICE_LAUNCHER_PING, &packet, sizeof(packet));
}

bool CLauncherSession::AddPacket(const DNTPacket * packet)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	char * pData = static_cast<char*>(m_Mem._Alloc(packet->iLen));
	if (pData)
	{
		memcpy_s(pData, packet->iLen, packet, packet->iLen);
		m_lData.push_back(pData);

		SetEvent(m_hEventParse);
		return true;
	}
	return false;
}

bool CLauncherSession::ParsePacket(unsigned char main, unsigned char sub,const void * ptr, int len)
{
	switch (main)
	{
		case NETSERVICE_STARTOF_MANAGING:
		{
			TServiceStartManage * pPacket = (TServiceStartManage*)ptr;
			m_bOtherPath = pPacket->bOther;
			CProcessManager::GetInstance()->ClearManagingItem();
			return true;
		}

		case NETSERVICE_MANAGING:
		{
			TServiceManaging * pPacket = (TServiceManaging*)ptr;

			if (pPacket->nCmdSize >= EXCUTECMDMAX + EXCUTELENMAX)
			{
				_ASSERT_EXPR(0, L"NETSERVICE_MANAGING - pPacket->nCmdSize >= EXCUTECMDMAX + EXCUTELENMAX");
				return false;
			}

			USES_CONVERSION;

			std::string strCmd;
			strCmd.append(pPacket->szCmd, pPacket->nCmdSize);

			strCmd.append("/sip=");
			strCmd.append(m_szIP);

			strCmd.append("/sp=");
			strCmd.append(I2A(m_iPort));

			std::string strExe;
			GetFirstRightValue("exe", strCmd, strExe);

			std::string strExcutePath;
			GetFirstRightValue("epath", strCmd, strExcutePath);
			
			CProcessManager::GetInstance()->AddManagingItem(pPacket->nSID, strExcutePath.c_str(), strExe.c_str(), strCmd.c_str(), strlen(g_Config.szExcutePath) > 0 ? g_Config.szExcutePath : NULL);
			return true;
		}

		case NETSERVICE_ENDOF_MANAGING:
		{
			CProcessManager::GetInstance()->MakeWatchingProcess();
			return true;
		}

		case NETSERVICE_RESULT:
		{
			TNetResult * pPacket = (TNetResult*)ptr;

			m_vManagedTypeList.clear();
				
			for (int i = 0; i < MANAGEDMAX; i++)
			{
				if (pPacket->cAssingedType[i] < 0) continue;
				m_vManagedTypeList.push_back(pPacket->cAssingedType[i]);
			}
			wcscpy_s(m_wszBaseURL, pPacket->szBaseUrl);
			wcscpy_s(m_wszPatchURL, pPacket->szPatchUrl);

			std_str strerr;
			bool bRet = m_UrlUpdater.UpdaterInit(m_wszBaseURL, m_wszPatchURL, &strerr);
			if (bRet == false)
			{
				TServerProcessState packet;
				memset(&packet, 0, sizeof(packet));
				sprintf_s(packet.szState, "DownLoad Fail %s", strerr.c_str());
				SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
			}

			//SendVersion
			SendVersion(g_Config.szVersion);
			return true;
		}

		case NETSERVICE_RUN_PROCESS:
		{
			TRunProcess * pPacket = (TRunProcess*)ptr;

			char szExeName[MAX_PATH];
			memset(szExeName, 0, sizeof(szExeName));

			int nErrorCode = 0;
			bool bExcute = true;
			DWORD dwLastError = 0;
			if (CProcessManager::GetInstance()->GetExeName(pPacket->nSID, szExeName) == false)
			{
				TextOut(_T("UnManaging Item [%s]\n"), szExeName);
				nErrorCode = -1;
				bExcute = false;
			}
			else
			{
				char szCopyName[MAX_PATH];
				sprintf(szCopyName, "%s/Exe/%s", "PatchFolder", szExeName);
				if (_access(szCopyName, 0) != -1)
				{
					if (CProcessManager::GetInstance()->IsRunProcess(pPacket->nSID) == false)
					{
						if (CProcessManager::GetInstance()->IsRunProcess(szExeName) == false)
						{
							char szOtherPath[512];
							memset(szOtherPath, 0, sizeof(szOtherPath));

							CProcessManager::GetInstance()->GetExutePath(szExeName, szOtherPath);
							if (strlen(szOtherPath) > 0)
							{
								std::string strTempPath;
								strTempPath = szOtherPath;
								strTempPath.append("\\");
								strTempPath.append(szExeName);

								if (0 != remove(strTempPath.c_str()))
								{
									nErrorCode = -6;

									time_t timer;
									timer = time(NULL);

									char szReExeName[MAX_PATH];
									sprintf(szReExeName, "%s_%d", strTempPath.c_str(), timer);

									if (0 != rename (strTempPath.c_str(), szReExeName))
										nErrorCode = -7;
								}

								if (CopyFile(szCopyName, strTempPath.c_str(), false) == NULL)
								{
									dwLastError = GetLastError();
									TextOut(_T("Copy Failed [%s][ErrNum:%d]\n"), szExeName, dwLastError);
									nErrorCode = -4;
									bExcute = false;
								}
							}
							else
							{
								if (_access(szExeName, 0) != -1)
								{
									if (0 != remove(szExeName))
									{
										nErrorCode = -6;

										time_t timer;
										timer = time(NULL);

										char szReExeName[MAX_PATH];
										sprintf(szReExeName, "%s_%d", szExeName, timer);

										if (0 != rename (szExeName, szReExeName))
											nErrorCode = -7;
									}

									if (CopyFile(szCopyName, szExeName, false) == NULL)
									{
										dwLastError = GetLastError();
										TextOut(_T("Copy Failed [%s][ErrNum:%d]\n"), szExeName, dwLastError);
										nErrorCode = -4;
										bExcute = false;
									}
								}
								else
								{
									if (CopyFile(szCopyName, szExeName, false) == NULL)
									{
										dwLastError = GetLastError();
										TextOut(_T("Copy Failed [%s][ErrNum:%d]\n"), szExeName, dwLastError);
										nErrorCode = -4;
										bExcute = false;
									}
								}

							}
						}
					}
					else
					{
						nErrorCode = -2;
						bExcute = false;
					}
				}
				else
					nErrorCode = -3;
			}

			if (bExcute)
			{
				if (CProcessManager::GetInstance()->IsRunProcess(pPacket->nSID) == false)
				{
					if (CProcessManager::GetInstance()->RunProcess(pPacket->nSID) == true)
						SendPacket(SERVICE_LAUNCHER_CREATED_PROCESS, &pPacket->nSID, sizeof(pPacket->nSID));
					else
						nErrorCode = -5;
				}
				else
					nErrorCode = -2;
			}

			if (nErrorCode < 0)
			{
				TServerProcessState packet;
				memset(&packet, 0, sizeof(packet));	
				switch (nErrorCode)
				{
				case -1:
					sprintf(packet.szState, "UnManaging Item [%s]", szExeName);
					break;
				case -2:
					sprintf(packet.szState, "Already Run Process [%s]", szExeName);
					break;
				case -3:
					sprintf(packet.szState, "PatchFolder AccessFailed [%s]", szExeName);
					break;
				case -4:
					sprintf(packet.szState, "CopyFailed [%s] [ErrorNum:%d], But the version of server could be wrong", szExeName, dwLastError);
					break;
				case -5:
					sprintf(packet.szState, "ExcuteFailed [%s]", szExeName);
					break;
				case -6:
					sprintf(packet.szState, "RemoveFailed [%s]", szExeName);
					break;
				case -7:
					sprintf(packet.szState, "RenameFailed [%s]", szExeName);
					break;
				default:
					return true;
				}
				SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
			}
			return true;
		}

		case NETSERVICE_TERMINATE_ALLPROCESS:
		{
			CProcessManager::GetInstance()->TerminateAll();
			return true;
		}

		case NETSERVICE_TERMINATE_PROCESS:
		{
			TTerminateProcess * pPacket = (TTerminateProcess*)ptr;

			if (CProcessManager::GetInstance()->Terminate(pPacket->nSID) == false)
			{
				TServerProcessState packet;
				memset(&packet, 0, sizeof(packet));	
				sprintf(packet.szState, "TerminateFailed [%d]", pPacket->nSID);
				SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
			}
			else
			{
				if (!pPacket->bRestart)
					return true;

				if (!CProcessManager::GetInstance()->IsRunProcess(pPacket->nSID))
				{
					if (CProcessManager::GetInstance()->RunProcess(pPacket->nSID, pPacket->bRestart) == true)
						SendPacket(SERVICE_LAUNCHER_CREATED_PROCESS, &pPacket->nSID, sizeof(pPacket->nSID));
				}
			}
			return true;
		}

		case NETSERVICE_PATCHSTART:
		{
			m_bIsNowPatch = true;
			return true;
		}

		case NETSERVICE_PATCHEND:
		{
			m_bIsNowPatch = false;
			return true;
		}

		case NETSERVICE_PATCH:
		{
			const TPatch * pPacket = (TPatch*)ptr;

			TServerProcessState packet;
			memset(&packet, 0, sizeof(packet));	

			packet.cPatch = 1;
			strcpy_s(packet.szState, "PatchStart");
			SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);

			char szKey[32];
			WideCharToMultiByte(CP_ACP, 0, pPacket->szKey, -1, szKey, 32, NULL, NULL);

			TCHAR szCurPath[256];
			if (GetCurrentDirectory(sizeof(szCurPath), szCurPath) == false) _ASSERT(0);	

			m_pPatchStatusChecker->SetKey(pPacket->szKey);

			std_str strerr;
			if (!wcscmp(pPacket->szKey, L"Info"))
			{
				if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"PatchFolder", true, false, false, &strerr, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("Info PatchFail %s"), strerr.c_str());
					//SendPatchState(pPacket->nPatchID, szKey, "I", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
					return true;
				}				

				TextOut(_T("Info Patched"));
				SendPatchState(pPacket->nPatchID, szKey, "I", true);
			}
			else if (!wcscmp(pPacket->szKey, L"Config"))
			{
				if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"PatchFolder", true, false, false, &strerr, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("Config PatchFail %s"), strerr.c_str());
					//SendPatchState(pPacket->nPatchID, szKey, "C", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
					return true;
				}

				TextOut(_T("Config Patched"));
				SendPatchState(pPacket->nPatchID, szKey, "C", true);
			}
			else if (!wcscmp(pPacket->szKey, L"System"))
			{
				if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"PatchFolder", true, false, false, &strerr, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("System PatchFail %s"), strerr.c_str());
					//SendPatchState(pPacket->nPatchID, szKey, "S", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
					return true;
				}

				TextOut(_T("System Patched"));
				SendPatchState(pPacket->nPatchID, szKey, "S", true);
			}
			else if (!wcscmp(pPacket->szKey, L"Exe"))
			{
				if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"PatchFolder", true, false, false, &strerr, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("Exe PatchFail %s"), strerr.c_str());
					//SendPatchState(pPacket->nPatchID, szKey, "E", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
					return true;
				}

				TextOut(_T("Exe Patched"));
				SendPatchState(pPacket->nPatchID, szKey, "E", true);
			}
			else if (!wcscmp(pPacket->szKey, L"ESM"))
			{
				// Bat 디렉토리 생성
				if (_access("PatchFolder/Bat", 0) == -1)
					_tmkdir(_T("./PatchFolder/Bat"));

				// 현재 디렉토리를 찾는다.
				WCHAR szCurPath[256];
				memset(szCurPath, 0, sizeof(szCurPath));
				GetCurrentDirectoryW(_countof(szCurPath), szCurPath);


				// 다운받을 경로를 지정한다.
				std::wstring wDestPath;
				wDestPath += L"PatchFolder/Bat/ESM.exe";

				WCHAR wszUrl[1024];
				MultiByteToWideChar(CP_ACP, 0, "Bat/ESM.exe", -1, wszUrl, 1024);

				// 다운로드 성공여부를 SM에 보고
				if (m_UrlUpdater.UrlUpdateEach(wszUrl, wDestPath.c_str(), false, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("Patch ESM Fail"));
					//SendPatchState(pPacket->nPatchID, szKey, "B", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
				}
				else
				{
					TextOut(_T("Patch ESM OK"));
					SendPatchState(pPacket->nPatchID, szKey, "B", true);
				}
			}
			else if (!wcscmp(pPacket->szKey, L"Bat"))
			{
				// Bat 디렉토리 생성
				if (_access("PatchFolder/Bat", 0) == -1)
					_tmkdir(_T("./PatchFolder/Bat"));

				// 현재 디렉토리를 찾는다.
				WCHAR szCurPath[256];
				memset(szCurPath, 0, sizeof(szCurPath));
				GetCurrentDirectoryW(_countof(szCurPath), szCurPath);


				// 다운받을 경로를 지정한다.
				std::wstring wDestPath;
				wDestPath += L"PatchFolder/Bat/dnbatch.bat";

				WCHAR wszUrl[1024];
				MultiByteToWideChar(CP_ACP, 0, "Bat/dnbatch.bat", -1, wszUrl, 1024);

				// 다운로드 성공여부를 SM에 보고
				if (m_UrlUpdater.UrlUpdateEach(wszUrl, wDestPath.c_str(), false, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("Patch Batch Fail"));
					//SendPatchState(pPacket->nPatchID, szKey, "B", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
				}
				else
				{
					TextOut(_T("Patch Batch OK"));
					SendPatchState(pPacket->nPatchID, szKey, "B", true);
				}
				
				return true;
				
			}
			else if (!wcscmp(pPacket->szKey, L"DynCodeBin"))
			{
				
				if (_access("./DynCodeBin64", 0) == -1)
				{
					_tmkdir(_T("./DynCodeBin64"));
					_tmkdir(_T("./DynCodeBin64/Client"));
					_tmkdir(_T("./DynCodeBin64/Server"));
				}

				if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"DynCodeBin64", false, true, false, &strerr, m_pPatchStatusChecker) == false)
				{
					TextOut(_T("DynCodeBin PatchFail %s"), strerr.c_str());
					//SendPatchState(pPacket->nPatchID, szKey, "D", false);
					SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
					return true;
				}

				TextOut(_T("DynCodeBin Patched"));
				SendPatchState(pPacket->nPatchID, szKey, "D", true);
			}
			else if (!wcscmp(pPacket->szKey, L"Res"))
			{
				//이하 필요한 것만 끌어가자!
				if (pPacket->nPatchResLevel > 0)
				{
					TCHAR szExtract[MAX_PATH];
					if (pPacket->nPatchResLevel == 1)
					{
						if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"PatchFolder", true, false, false, &strerr, m_pPatchStatusChecker) == false)
						{
							TextOut(_T("Res PatchFail %s"), strerr.c_str());
							//SendPatchState(pPacket->nPatchID, szKey, "R", false);
							SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
							return true;
						}

						_stprintf(szExtract, _T("%s"), _T("PatchFolder\\Res\\VillageRes.zip"));
					}
					else
					{
						if (m_UrlUpdater.UrlUpdate(m_vManagedTypeList, pPacket->szKey, L"PatchFolder", true, false, false, &strerr, m_pPatchStatusChecker) == false)
						{
							TextOut(_T("Res PatchFail %s"), strerr.c_str());
							//SendPatchState(pPacket->nPatchID, szKey, "R", false);
							SendPatchState(pPacket->nPatchID, szKey, strerr.c_str(), false);
							return true;
						}

						_stprintf(szExtract, _T("%s"), _T("PatchFolder\\Res\\GameRes.zip"));
					}
#if defined(PRE_UNZIP_CHANGE)
					//GameRes를 버전별로 프로세스 실행시 어느것을 읽을지 선택하게 풀어놓을 폴더 설정 및 생성
					TCHAR szExtractFolder[MAX_PATH];
					_stprintf(szExtractFolder, _T("%s%s"), szCurPath, _T("\\GameRes"));

					CUnZipProcess zip = CUnZipProcess(this, szExtractFolder, szExtract);
					if(!zip.OpenZip())
					{
						TextOut(_T("ZipOpen Failed"));
						strcpy_s(packet.szState, "Patch ZipOpen Failed");
						//SendPatchState(pPacket->nPatchID, szKey, "R", false);
						SendPatchState(pPacket->nPatchID, szKey, "Patch ZipOpen Failed", false);
						SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
						return true;
					}

					if (_access(szExtractFolder, 0) == -1)
						_tmkdir(szExtractFolder);
					else
						EmptyDirectory(szExtractFolder);

					TextOut(_T("Unzip Start"));
					if(zip.UnZip())
					{
						TextOut(_T("Unzip Finish"));
						SendPatchState(pPacket->nPatchID, szKey, "R", true);
					}
					else
					{
						TextOut(_T("Unzip Failed"));
						strcpy_s(packet.szState, "Patch Unzip Failed");
						SendPatchState(pPacket->nPatchID, szKey, "Patch Unzip Failed", false);
						SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
						return true;
					}
#else	// #if defined(PRE_UNZIP_CHANGE)
					CUnzipper zip;
					if (zip.OpenZip(szExtract) == false)
					{
						TextOut(_T("ZipOpen Failed"));
						strcpy_s(packet.szState, "Patch ZipOpen Failed");
						//SendPatchState(pPacket->nPatchID, szKey, "R", false);
						SendPatchState(pPacket->nPatchID, szKey, "Patch ZipOpen Failed", false);
						SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
						return true;
					}

					//GameRes를 버전별로 프로세스 실행시 어느것을 읽을지 선택하게 풀어놓을 폴더 설정 및 생성
					TCHAR szExtractFolder[MAX_PATH];
					_stprintf(szExtractFolder, _T("%s%s"), szCurPath, _T("\\GameRes"));

					if (_access(szExtractFolder, 0) == -1)
						_tmkdir(szExtractFolder);
					else
						EmptyDirectory(szExtractFolder);

					if (zip.SetOutputFolder(szExtractFolder) == true)
					{
						TextOut(_T("Unzip Start"));
						if (zip.Unzip(this) == false)
						{
							TextOut(_T("Unzip Failed"));
							//SendPatchState(pPacket->nPatchID, szKey, "R", false);							
							strcpy_s(packet.szState, "Patch Unzip Failed");
							SendPatchState(pPacket->nPatchID, szKey, "Patch Unzip Failed", false);
							SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
							return true;
						}
						TextOut(_T("Unzip Finish"));
						
						SendPatchState(pPacket->nPatchID, szKey, "R", true);
					}
					else
					{
						TextOut(_T("Unzip SetFolder Failed"));
						//SendPatchState(pPacket->nPatchID, szKey, "R", false);
						SendPatchState(pPacket->nPatchID, szKey, "Unzip SetFolder Failed", false);
						strcpy_s(packet.szState, "Patch Unzip SetFolder Failed");
						SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
						return true;
					}
#endif	// #if defined(PRE_UNZIP_CHANGE)
				}
				else
				{
					//리소스패치할게 없습니다.
					SendPatchState(pPacket->nPatchID, szKey, "R", true);
				}
			}

			TextOut(_T("Patch OK"));
			m_bIsPatched = true;
			return true;
		}

		case NETSERVICE_PATCH_BYURL:
		{
			TPatchUrl * pPacket = (TPatchUrl*)ptr;

			if (pPacket->nLen >= 1024) return false;

			char szUrl[1024];
			_strcpy(szUrl, pPacket->szUrl, pPacket->nLen);

			char szKey[32];
			WideCharToMultiByte(CP_ACP, 0, pPacket->szKey, -1, szKey, 32, NULL, NULL);

			WCHAR wszUrl[1024];
			MultiByteToWideChar(CP_ACP, 0, szUrl, -1, wszUrl, 1024);

			std::vector<std::wstring> tokens;
			TokenizeW(wszUrl, tokens, L"/");

			if (tokens.size() <= 0)
				return true;

			WCHAR szCurPath[256];
			memset(szCurPath, 0, sizeof(szCurPath));
			GetCurrentDirectoryW(_countof(szCurPath), szCurPath);

			std::wstring wDestPath;
			if (pPacket->bOnlyUseURL)
			{
				wDestPath = L"PatchFolder/";
				wDestPath += pPacket->szDest;

				if (_waccess(wDestPath.c_str(), 0) == -1)
					_wmkdir(wDestPath.c_str());
				
				wDestPath = szCurPath;
				wDestPath += L"\\";
				wDestPath += L"PatchFolder\\";
				wDestPath += pPacket->szDest;
				wDestPath += L"\\";
				wDestPath += tokens[tokens.size() - 1];
				
				if (m_UrlUpdater.UrlUpdateUrl(wszUrl, wDestPath.c_str(), false) == false)
				{
					TextOut(_T("Each PatchFail"));
					SendPatchState(pPacket->nPatchID, szKey, "Live", false);
					return true;
				}
			}
			else
			{
				wDestPath = szCurPath;
				wDestPath += L"\\";
				wDestPath += tokens[tokens.size() - 1];

				if (m_UrlUpdater.UrlUpdateEach(wszUrl, wDestPath.c_str(), false) == false)
				{
					TextOut(_T("Each PatchFail"));
					SendPatchState(pPacket->nPatchID, szKey, "Each", false);
					return true;
				}
			}
			
			SendPatchState(pPacket->nPatchID, szKey, "Each", true);
			return true;
		}

		case NETSERVICE_PATCHAPPLY:
		{
			TServerProcessState packet;
			memset(&packet, 0, sizeof(packet));

			if (m_bIsPatched)
			{
				m_bIsPatched = false;
				strcpy_s(packet.szState, "PatchApply OK");
			}
			else
				strcpy_s(packet.szState, "PatchApply Fail");
			SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
			
			return true;
		}

		case NETSERVICE_INFOCOPY:
		{
			TInfoCopy * pPacket = (TInfoCopy*)ptr;

			TServerProcessState packet;
			memset(&packet, 0, sizeof(packet));

			char szOtherExcutePath[512];
			memset(szOtherExcutePath, 0, sizeof(szOtherExcutePath));
			if (pPacket->bNeed)
				CProcessManager::GetInstance()->GetExutePath(szOtherExcutePath);

			bool bInfoRet = false, bSystemRet = false, bConfigRet = false;
			TCHAR szCurPath[256];
			memset(szCurPath, 0, sizeof(szCurPath));
			GetCurrentDirectory(sizeof(szCurPath), szCurPath);

			//타입과 관계 없이 전부 실행폴더로 카피한다.
			std::string deststr, srcstr = szCurPath;
			srcstr += "\\PatchFolder\\Info\\*.*";
			deststr = szCurPath;
			deststr += "\\";
			bInfoRet = CopyFolder(srcstr, deststr);

			if (pPacket->bNeed && strlen(szOtherExcutePath) > 0)
			{
				srcstr = szCurPath;
				srcstr += "\\PatchFolder\\Info\\*.*";
				deststr = szOtherExcutePath;
				deststr += "\\";
				bInfoRet = CopyFolder(srcstr, deststr);

				//일단 외부 실행위치에 파일을 모조리 카퓌 x86용만 덮어씌우기
				srcstr = szCurPath;
				srcstr += "\\PatchFolder\\Info\\x86\\*.*";
				deststr = szOtherExcutePath;
				deststr += "\\";
				bInfoRet = CopyFolder(srcstr, deststr);
			}

			srcstr.clear();
			deststr.clear();
			srcstr = szCurPath;
			srcstr += "\\PatchFolder\\Config\\*.*";
			deststr = szCurPath;
			deststr += "\\Config\\";
			bConfigRet = CopyFolder(srcstr, deststr);

			srcstr.clear();
			deststr.clear();
			srcstr = szCurPath;
			srcstr += "\\PatchFolder\\System\\*.*";
			deststr = szCurPath;
			deststr += "\\System\\";
			bSystemRet = CopyFolder(srcstr, deststr);
			
			std::string strSrc;
			strSrc = szCurPath;
			strSrc += "\\PatchFolder\\Info\\UserAuthen.xml";

			std::string strDest;
			strDest = szCurPath;
			strDest += "\\services\\UserAuthen.xml";

			if (_access(strSrc.c_str(), 0) != -1)
			{
				//이녀석은 특별대우
				if (_access("services", 0) == -1)
				{
					_tmkdir(_T("services"));
				}

				if (CopyFile(strSrc.c_str(), strDest.c_str(), false) == false)
				{
					DWORD dwLastError = GetLastError();
					TextOut(_T("Copy Failed [%s][ErrNum:%d]\n"), L"UserAuthen.xml", dwLastError);
					bInfoRet = false;
				}
				else
				{
					//정상처리가 되었을 경우 템프를 지운다.
					strSrc = szCurPath;
					strSrc += "\\UserAuthen.xml";
					DeleteFile(strSrc.c_str());
				}
			}

			std::string strState;
			strState = "CopyInfo : ";
			strState += bInfoRet == true ? "(Info)[OK]" : "(Info)[Fail]";
			strState += bConfigRet == true ? "(Config)[OK]" : "(Config)[Fail]";
			strState += bSystemRet == true ? "(System)[OK]" : "(System)[Fail]";
			strcpy_s(packet.szState, strState.c_str());			
			
			SendPacket(SERVICE_LAUNCHER_PROCESS_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
			return true;
		}

		case NETSERVICE_RETURN_PING:
		{
			TReturnPing * pPacket = (TReturnPing*)ptr;

			TReturnPing packet;
			memcpy(&packet, ptr, sizeof(TReturnPing));

			SendPacket(SERVICE_LAUNCHER_RETURN_PING, &packet, sizeof(TReturnPing));
			return true;
		}

		case NETSERVICE_RUNBATCH:
		{
			char szBatchPath[MAX_PATH];
			sprintf(szBatchPath, "%s/PatchFolder/Bat/dnbatch.bat", g_Config.szExcutePath);

			TRunBatch * pPacket = (TRunBatch*)ptr;

			// 배치파일을 실행한다 
			int nRet = -1;
			
			if (!CBatchManager::GetInstance()->RunBatch (szBatchPath, pPacket->nBatchID))
			{
				nRet = 1;
				TextOut(_T("BatchRun Fail to Exec"));
			}

			// 실행결과를 SM에 보고한다.
			SendRunBatchState (nRet, pPacket->nBatchID);
			
			return true;
		}

		case NETSERVICE_STOPBATCH:
		{
			TRunBatch * pPacket = (TRunBatch*)ptr;

			CBatchManager::GetInstance()->TerminateAll();

			// 실행결과를 SM에 보고한다.
			SendStopBatchState (pPacket->nBatchID);

			return true;
		}

		case NETSERVICE_LIVEEXTDEL:
		{
			TCHAR szCurPath[256];
			memset(szCurPath, 0, sizeof(szCurPath));
			GetCurrentDirectory(sizeof(szCurPath), szCurPath);

			std::string strPath;
			strPath = szCurPath;
			strPath += "\\PatchFolder\\ext";

			if (_access(strPath.c_str(), 0) != -1)
			{
				CDirectory directory;
				directory.Remove(strPath.c_str(), FALSE);
			}
			
			return true;
		}

		case NETSERVICE_LIVEEXTCOPY:
		{
			TCHAR szCurPath[256];
			memset(szCurPath, 0, sizeof(szCurPath));
			GetCurrentDirectory(sizeof(szCurPath), szCurPath);

			std::string strPath;
			strPath = szCurPath;
			strPath += "\\PatchFolder\\ext\\*.*";

			std::string strDestPath;
			strDestPath = szCurPath;
			strDestPath += "\\GameRes\\Resource\\Ext";

			//일단은 테스트용도로 일반적인 폴더카피를 실행		
			//실버전에서는 정확한 패치가 이루어지는 것이 보장되어야 하므로 따로 처리하자
			bool bRet = false;
			for (int i = 0; i < 10 && bRet == false; i++)
				bRet = CopyFolder(strPath, strDestPath);

			if (bRet == false)
				TextOut(_T("ExtLive CopyFail Check Files\n"));
			SendLiveExtCopy(bRet);
			return true;
		}
		case NETSERVICE_STARTESM:
			{
				std::string szExecuteName = "ESM.exe";
				std::string szCopyName = "PatchFolder/Bat/";
				szCopyName += szExecuteName;
				std::string szError;

				if (_access(szCopyName.c_str(), 0) != -1)
				{
					if (0 != remove(szExecuteName.c_str()))
						CProcessManager::GetInstance()->TerminateByName((char*)szExecuteName.c_str());
					
					if (CopyFile(szCopyName.c_str(), szExecuteName.c_str(), false) == NULL)
						szError = "Copy Failed";
					else
					{
						if (!CProcessManager::GetInstance()->RunByName((char*)szExecuteName.c_str()))
							szError = "Execute Failed due to duplication";
						else
							szError = "";
					}
				}
				else
				{
					if (!CProcessManager::GetInstance()->RunByName((char*)szExecuteName.c_str()))
						szError = "Execute Failed due to access violation";
					else
						szError = "";
				}
					
								
				TServerEsmState packet;
				memset(&packet, 0, sizeof(packet));

				if (szError.empty())
					sprintf(packet.szState, "Success to start ESM");
				else
					sprintf(packet.szState, "Fail to start ESM [%s]", szError.c_str());
					

				SendPacket(SERVICE_LAUNCHER_ESM_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);
				
				return true;
			}

		case NETSERVICE_STOPESM:
			{
				TServerEsmState packet;
				memset(&packet, 0, sizeof(packet));
				if (CProcessManager::GetInstance()->TerminateByName("ESM.exe"))
					sprintf(packet.szState, "Success to stop ESM");
				else
					sprintf(packet.szState, "Fail to stop ESM");
				SendPacket(SERVICE_LAUNCHER_ESM_STATE, &packet, sizeof(packet) - sizeof(packet.szState) + (int)strlen(packet.szState) + 1);

				return true;
			}
	}
	return false;
}


void CLauncherSession::Connect()
{
	TextOut(_T("Connected ServiceManager"));
}


void CLauncherSession::Destroy()
{
	TextOut(_T("DisConnected ServiceManager"));
}

void CLauncherSession::TimeEvent()
{
	static int nCnt = 0;
	if (nCnt%10 == 0)
		SendPing();
	nCnt++;
}

bool CLauncherSession::CheckSrcPath(const TCHAR * szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	ZeroMemory(&FindFileData,sizeof(FindFileData));

	//find dot
	TCHAR szPathFull[256];
	bool bFlag = false;
	for (int i = (int)strlen(szPath); i > 0; i--)
	{
		if (szPath[i] != '\\')
		{
			if (	szPath[i] == '.')
				bFlag = true;
		}
		else if (szPath[i] == '\\')
			break;
	}

	if (bFlag == false)
		_sntprintf(szPathFull, sizeof(szPathFull)/sizeof(*szPathFull), _T("%s/*.*"), szPath);	
	else
		strcpy_s(szPathFull, szPath);

	hFind = FindFirstFile(szPathFull, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	
	FindClose(hFind);	
	return true;
}

bool CLauncherSession::CopyFolder(const TCHAR * srcPath, const TCHAR * destPath)
{
	SHFILEOPSTRUCT fo_struct;
	memset(&fo_struct, 0, sizeof(SHFILEOPSTRUCT));
	fo_struct.wFunc = FO_COPY;
	std::string srcStr = srcPath;
	srcStr += "/*.*";
	srcStr += '\0';
	srcStr += '\0';
	fo_struct.pFrom = srcStr.c_str();
	fo_struct.pTo = destPath;

	fo_struct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES;
	//fo_struct.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES;
	int nRet = SHFileOperation(&fo_struct);
	return nRet == 0 ? true : false;
}

bool CLauncherSession::CopyFolder(std::string strSrc, std::string strDest)
{
	SHFILEOPSTRUCT fo_struct;
	memset(&fo_struct, 0, sizeof(SHFILEOPSTRUCT));
	fo_struct.wFunc = FO_COPY;

	strSrc += '\0';
	strSrc += '\0';
	strDest += '\0';
	strDest += '\0';
	
	fo_struct.pFrom = strSrc.c_str();
	fo_struct.pTo = strDest.c_str();

	fo_struct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES;
	//fo_struct.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_MULTIDESTFILES;
	int nRet = SHFileOperation(&fo_struct);
	return nRet == 0 ? true : false;
}

void CLauncherSession::MakePatchPath()
{
	if (_access("System", 0) == -1)
		_tmkdir(_T("System"));

	if (_access("PatchFolder", 0) == -1)
	{
		_tmkdir(_T("PatchFolder"));
		_tmkdir(_T("./PatchFolder/Config"));
		_tmkdir(_T("./PatchFolder/Info"));
		_tmkdir(_T("./PatchFolder/Res"));
		_tmkdir(_T("./PatchFolder/Exe"));
		_tmkdir(_T("./PatchFolder/System"));
	}
}

void CLauncherSession::EmptyDirectory(const char * pPath)
{
	CDirectory directory;
	directory.Remove(pPath, FALSE);

	//이상하게 자꾸 뻑난다.....
	/*SHFILEOPSTRUCT fo_struct;
	memset(&fo_struct, 0, sizeof(SHFILEOPSTRUCT));
	fo_struct.wFunc = FO_DELETE;

	std::string srcStr = pPath;
	srcStr += "/*.*";

	fo_struct.pFrom = srcStr.c_str();
	fo_struct.pTo = "";
	
	fo_struct.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT | FOF_FILESONLY;
	SHFileOperation(&fo_struct);*/
}

void CLauncherSession::SwapSlash(const TCHAR * pSrc, TCHAR * pDest)
{
	int nCnt = 0;;
	for (int i = 0; pSrc[i] != NULL; i++)
	{
		if (pSrc[i] == ':')
			pDest[nCnt] = '$';
		else if (pSrc[i] == '/')		
			pDest[nCnt] = '\\';
		else
			pDest[nCnt] = pSrc[i];
		nCnt++;	
	}
	pDest[nCnt] = NULL;
}

void CLauncherSession::OnUnzip(const wchar_t* pFilename, unsigned long progress, unsigned long progressMax)
{
	static DWORD prevTick = 0;

	DWORD currentTick = GetTickCount();
	if ((currentTick - prevTick <= 200) && (progress != progressMax))
		return;

	prevTick = currentTick;

	SendUnzipProgress(pFilename, progress, progressMax);
}

void CLauncherSession::ThreadParse()
{
	while (m_bTerminated == false)
	{
		if (m_lData.empty() == false)
		{
			ScopeLock <CSyncLock> sync(m_Sync);

			DNTPacket * pTPacket;
			std::list <char *>::iterator ii;
			for (ii = m_lData.begin(); ii != m_lData.end(); ii++)
			{
				pTPacket = NULL;
				pTPacket = reinterpret_cast<DNTPacket*>((*ii));
				if (ParsePacket(pTPacket->cMainCmd, pTPacket->cSubCmd, pTPacket->buf, pTPacket->iLen) == false)
					TextOut(_T("Unknown Header"));
				m_Mem._DeAlloc((*ii));
			}

			m_lData.clear();
		}
		else
		{
			WaitForSingleObject(m_hEventParse, 500);
		}
	}

	m_hThreadParse = 0;
}

DWORD WINAPI CLauncherSession::_threadparse(void* param)
{
	((CLauncherSession*)param)->ThreadParse();
	return 0;
}

bool CLauncherSession::_InitSession()
{
	return m_Mem.InitPool();
}

#include <Iprtrmib.h>

#define CXIP_A(IP)	((IP&0xFF000000)>>24)
#define CXIP_B(IP)	((IP&0x00FF0000)>>16)
#define CXIP_C(IP)	((IP&0x0000FF00)>>8)
#define CXIP_D(IP)	(IP&0x000000FF)

void CLauncherSession::_GetHostIPAddress()
{
	DWORD dwPrivateIP		= 0;
	DWORD dwPrivateIPMask	= 0;
	DWORD dwPublicIP		= 0;
	DWORD dwPublicIPMask	= 0;

	HMODULE hIPHLP = LoadLibrary( _T("iphlpapi.dll") );
	if( hIPHLP )
	{
		typedef	BOOL (WINAPI * LPGIPT)(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize, BOOL bOrder);
		LPGIPT fnGetIpAddrTable=(LPGIPT)GetProcAddress(hIPHLP, "GetIpAddrTable");
		if( fnGetIpAddrTable )
		{
			PMIB_IPADDRTABLE pIPAddrTable;
			DWORD dwSize=0;

			pIPAddrTable=(MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
			if(!pIPAddrTable) 
			{
				FreeLibrary(hIPHLP);
				return;
			}

			if( fnGetIpAddrTable(pIPAddrTable, &dwSize, 0)==ERROR_INSUFFICIENT_BUFFER )
			{
				free(pIPAddrTable);
				pIPAddrTable=(MIB_IPADDRTABLE *)malloc(dwSize);
				if(!pIPAddrTable) 
				{
					FreeLibrary(hIPHLP);
					return;
				}
			}

			if( fnGetIpAddrTable(pIPAddrTable, &dwSize, 0) == NO_ERROR )
			{ 
				for( DWORD i=0; i<pIPAddrTable->dwNumEntries ; ++i )
				{
					DWORD	dwIP		= ntohl(pIPAddrTable->table[i].dwAddr);
					BOOL	bPrivate	= false;

					if(CXIP_A(dwIP)==127)
					{
						continue;
					}
					else if(CXIP_A(dwIP)==10)
					{
						bPrivate=true;
					}
					else if(CXIP_A(dwIP)==172)
					{
						if(CXIP_B(dwIP)>=16 && CXIP_B(dwIP)<=31) 
							bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==192)
					{
						if(CXIP_B(dwIP)==168) 
							bPrivate=TRUE;
					}

					if(bPrivate)
					{
						if( !dwPrivateIP || dwPrivateIP>dwIP )
						{
							dwPrivateIP=dwIP;
							dwPrivateIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						}
					}
					else
					{
						if( !dwPublicIP )
						{
							dwPublicIP=dwIP;
							dwPublicIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						}
					}

					if( dwPrivateIP &&  dwPublicIP) 
						break;
				}
			}
			else
			{
				FreeLibrary(hIPHLP);
				return;
			}

			BOOL bIPAdjust=FALSE;

			// Check Public IP
			if(dwPrivateIP && !dwPublicIP)
			{
				bIPAdjust=TRUE;

				for(DWORD i=0; i<pIPAddrTable->dwNumEntries; ++i)
				{
					DWORD dwIP=ntohl(pIPAddrTable->table[i].dwAddr);
					BOOL bPrivate=FALSE;

					if(CXIP_A(dwIP)==127)
					{
						continue;
					}
					else if(CXIP_A(dwIP)==10)
					{
						bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==172)
					{
						if(CXIP_B(dwIP)>=16 && CXIP_B(dwIP)<=31) 
							bPrivate=TRUE;
					}
					else if(CXIP_A(dwIP)==192)
					{
						if(CXIP_B(dwIP)==168) 
							bPrivate=TRUE;
					}

					if(bPrivate && dwPrivateIP!=dwIP)
					{
						dwPublicIP=dwIP;
						dwPublicIPMask=ntohl(pIPAddrTable->table[i].dwMask);
						break;
					}
				}
			}

			// Check Not Found Public IP
			if(!dwPublicIP)
			{
				dwPublicIP		= dwPrivateIP;
				dwPublicIPMask	= dwPrivateIPMask;
			}
			else
			{
				if( bIPAdjust && dwPrivateIP>dwPublicIP )
				{
					DWORD dwIP		= dwPrivateIP;
					DWORD dwIPMask	= dwPrivateIPMask;

					dwPrivateIP		= dwPublicIP;
					dwPrivateIPMask	= dwPublicIPMask;
					dwPublicIP		= dwIP;
					dwPublicIPMask	= dwIPMask;
				}
			}

			// Clear
			free(pIPAddrTable);
		}
		else
		{
			FreeLibrary(hIPHLP);
			return;
		}
	}
	else
	{
		return;
	}

	FreeLibrary(hIPHLP);

	// Check IP
	if(!dwPrivateIP && !dwPublicIP)
		return;

	DWORD dwNPublicIP = htonl(dwPublicIP);
	m_strPublicIP = inet_ntoa( *((in_addr*)&dwNPublicIP) );

	DWORD dwNPrivateIP = htonl(dwPrivateIP);
	m_strPrivateIP = inet_ntoa( *((in_addr*)&dwNPrivateIP) );
}