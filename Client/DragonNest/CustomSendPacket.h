#pragma once


#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include <strsafe.h>

//rebirth system
inline void SendDoRebirth()
{
	CClientSessionManager::GetInstance().SendPacket(CS_CHAR, eChar::CS_DOREBIRTH, NULL, 0);
}


inline void SendSpecializePacket(int nSelectedClass)
{
	CSSpecialize packet;
	memset(&packet, 0, sizeof(CSSpecialize));

	packet.nSelectedClass = nSelectedClass;

	CClientSessionManager::GetInstance().SendPacket(CS_CHAR, eChar::CS_DOSPECIALIZE, (char*)&packet, sizeof(packet));
}