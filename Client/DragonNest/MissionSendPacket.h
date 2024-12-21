#pragma once

#if defined( PRE_ADD_CHAT_MISSION )

#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include <strsafe.h>

inline void SendChatMission( int nUIStringMid )
{
	CSChatMission packet;
	memset(&packet, 0, sizeof(CSChatMission));

	packet.nUIStringMid = nUIStringMid;

	CClientSessionManager::GetInstance().SendPacket(CS_MISSION, eMission::CS_CHAT_MISSION, (char*)&packet, sizeof(packet));
}

#endif //#if defined( PRE_ADD_CHAT_MISSION )