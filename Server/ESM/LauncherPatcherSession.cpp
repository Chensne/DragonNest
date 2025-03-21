
#include "Stdafx.h"
#include "IniFile.h"
#include "LauncherPatcherSession.h"
#include "ProcessManager.h"
#include "DNConfig.h"
#include "DNServerPacket.h"
#include <io.h>
#include <direct.h>

extern TNetLauncherPatcherConfig g_Config;
#if defined(_LAUNCHER)
extern void TextOut(const TCHAR * format, ...);
#endif

bool CLauncherPatcherSession::Start(const TCHAR *ip, unsigned short port)
{
	if (_InitSession() == false)
		return false;

	_tcscpy_s(m_szIP, ip);
	m_iPort = port;

	DWORD threadid;
#if defined(_LAUNCHER)
	TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Success"), ip, port);
#endif
	m_hThread = CreateThread(NULL, 0, _threadmain, (void*)this, 0, &threadid);

	m_hThreadParse = CreateThread(NULL, 0, _threadparse, (void*)this, 0, &threadid);
	return true;
}

void CLauncherPatcherSession::Stop()
{
	m_bTerminated = true;
	CSimpleClientSession::Stop();
}

void CLauncherPatcherSession::SendPatchState(bool bPatch)
{
	TPatcherSrevicePatchState packet;
	memset(&packet, 0, sizeof(TPatcherSrevicePatchState));

	packet.cState = bPatch == true ? 1 : 0;

	SendPacket(PATCHERSERVICE_PATCHSTATE, &packet, sizeof(packet));
}

void CLauncherPatcherSession::SendPing()
{
	SendPacket(PATCHERSERVICE_PING, NULL, 0);
}

void CLauncherPatcherSession::ThreadMain()
{
	if (CSimpleClientSession::Start(m_szIP, m_iPort) == false)
	{
#if defined(_LAUNCHER)
		TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Failed"), m_szIP, m_iPort);
#endif
	}

	while (m_bTerminated == false)
	{
		Sleep(1000);
		if (CSimpleClientSession::Start(m_szIP, m_iPort) == false)
		{
#if defined(_LAUNCHER)
			TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Failed"), m_szIP, m_iPort);
#endif
		}
		else
		{
#if defined(_LAUNCHER)
			TextOut(_T("Connect ServiceManager [IP:%s][Port:%d] Success"), m_szIP, m_iPort);
#endif
		}
	}

	m_hThread = 0;
}


DWORD WINAPI CLauncherPatcherSession::_threadmain(void* param)
{
	((CLauncherPatcherSession*)param)->ThreadMain();
	return 0;
}


CLauncherPatcherSession * CLauncherPatcherSession::GetInstance()
{
	static CLauncherPatcherSession s;
	return &s;
}

bool CLauncherPatcherSession::AddPacket(const DNTPacket * packet)
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

bool CLauncherPatcherSession::ParsePacket(unsigned char main, unsigned char sub,const void * ptr, int len)
{
	switch (main)
	{
	case PATCHERSERVICE_RESULT:
		{
			TPatcherResult * pPacket = (TPatcherResult*)ptr;

			wcscpy_s(m_wszBaseURL, pPacket->szBaseUrl);
			wcscpy_s(m_wszPatchURL, pPacket->szPatchUrl);

			m_UrlUpdater.UpdaterInit(m_wszBaseURL, m_wszPatchURL);
			CProcessManager::GetInstance()->AddManagingItem(0, NULL, g_Config.szNetLauncherName, NULL, g_Config.szNetLauncherLocation);
			CProcessManager::GetInstance()->MakeWatchingProcess();
			return true;
		}

	case PATCHERSERVICE_STARTLAUNCHER:
		{
			CProcessManager::GetInstance()->RunProcess(0);
			return true;
		}

	case PATCHERSERVICE_STOPLAUNCHER:
		{
			CProcessManager::GetInstance()->Terminate(0);
			return true;
		}

	case PATCHERSERVICE_PATCHLAUNCHER:
		{
			CProcessManager::GetInstance()->Terminate(0);

			WCHAR wszLocation[MAX_PATH];
			memset(wszLocation, 0, sizeof(wszLocation));
			MultiByteToWideChar(CP_ACP, 0, g_Config.szNetLauncherLocation, -1, wszLocation, MAX_PATH);

			bool bRet = m_UrlUpdater.UrlUpdate(L"Patcher", wszLocation, false);
			SendPatchState(bRet);
			return true;
		}
	}
	return false;
}

void CLauncherPatcherSession::Connect()
{
}

void CLauncherPatcherSession::Destroy()
{
}

void CLauncherPatcherSession::TimeEvent()
{
	static int nCnt = 0;
	if (nCnt%10 == 0)
		SendPing();
	nCnt++;
}

CLauncherPatcherSession::CLauncherPatcherSession()
{
	m_hThread = 0;
	m_bTerminated = false;
}

CLauncherPatcherSession::~CLauncherPatcherSession()
{
	if (m_hThread != 0)
		WaitForSingleObject(m_hThread, INFINITE);
}

void CLauncherPatcherSession::ThreadParse()
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
				{
#ifdef _LAUNCHER
					TextOut(_T("Unknown Header"));
#endif		//#ifdef _LAUNCHER
				}
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

DWORD WINAPI CLauncherPatcherSession::_threadparse(void* param)
{
	((CLauncherPatcherSession*)param)->ThreadParse();
	return 0;
}

bool CLauncherPatcherSession::_InitSession()
{
	return m_Mem.InitPool();
}