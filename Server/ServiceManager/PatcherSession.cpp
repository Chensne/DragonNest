
#include "stdafx.h"
#include "LauncherSession.h"
#include "DNServerPacket.h"
#include "ServiceManager.h"
#include "Log.h"

CPatcherSession::CPatcherSession()
{
	Init(1024 * 10, 1024 * 10);
	m_nPSID = 0;
}

CPatcherSession::~CPatcherSession()
{
}

int CPatcherSession::MessageProcess(int iMainCmd, int iSubCmd, char *pData, int iLen)
{
	switch (iMainCmd)
	{
	case PATCHERSERVICE_PATCHSTATE:
		{
			TPatcherSrevicePatchState * pPacket = (TPatcherSrevicePatchState*)pData;

			if (pPacket->cState == 0)
			{
				g_Log.Log(LogType::_ERROR, L"NetLauncher Patch Failed [ID:%d][IP:%S]\n", GetSessionID(), GetIp());
			}
			else
			{
			}
			g_pServiceManager->PatchedLauncher(GetSessionID());
			break;
		}

	case PATCHERSERVICE_PING:
		{
			int i = 0;
			break;
		}
	}
	return ERROR_NONE;
}

void CPatcherSession::SendConnectedResult(const WCHAR * pBaseUrl, const WCHAR * pPatchUrl)
{
	TPatcherResult packet;
	memset(&packet, 0, sizeof(packet));
	
	wcscpy_s(packet.szBaseUrl, pBaseUrl);
	wcscpy_s(packet.szPatchUrl, pPatchUrl);

	AddSendData(PATCHERSERVICE_RESULT, 0, (char*)&packet, sizeof(packet));
}

void CPatcherSession::SendStartNetLauncher()
{
	AddSendData(PATCHERSERVICE_STARTLAUNCHER, 0, NULL, 0);
}

void CPatcherSession::SendStopNetLauncher()
{
	AddSendData(PATCHERSERVICE_STOPLAUNCHER, 0, NULL, 0);
}

bool CPatcherSession::SendNetLauncherPatch()
{
	if (AddSendData(PATCHERSERVICE_PATCHLAUNCHER, 0, NULL, 0) < 0)
		return false;
	return true;
}
