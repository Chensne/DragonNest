
#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DnCommonUtil.h"

inline void SendIsolateListReq()
{
	CClientSessionManager::GetInstance().SendPacket(SC_ISOLATE, eIsolate::CS_ISOLATE_GET, NULL, 0);
}

inline void SendIsolateAdd(const WCHAR * pIsolateName)
{
	CSIsolateAdd packet;
	memset(&packet, 0, sizeof(packet));
	int nameLen = int(wcslen(pIsolateName));
	if (CommonUtil::IsValidCharacterNameLen(nameLen) == false)
		return;

	_wcscpy(packet.wszIsolateName, _countof(packet.wszIsolateName), pIsolateName, (int)wcslen(pIsolateName));

	CClientSessionManager::GetInstance().SendPacket(SC_ISOLATE, eIsolate::CS_ISOLATE_ADD, (char*)&packet, sizeof(packet));
}

inline void SendIsolateDel(const WCHAR * pIsolateName)
{
	CSIsoLateDelete packet;
	memset(&packet, 0, sizeof(packet));
	int nameLen = int(wcslen(pIsolateName));
	if (CommonUtil::IsValidCharacterNameLen(nameLen) == false)
		return;

	_wcscpy(packet.wszIsolateName, _countof(packet.wszIsolateName), pIsolateName, (int)wcslen(pIsolateName));

	CClientSessionManager::GetInstance().SendPacket(SC_ISOLATE, eIsolate::CS_ISOLATE_DELETE, (char*)&packet, sizeof(packet));
}