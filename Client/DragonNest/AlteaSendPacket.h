#pragma once

#if defined( PRE_ALTEIAWORLD_EXPLORE )

#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include <strsafe.h>

inline void SendAlteaInfo( const AlteiaWorld::Info::eInfoType eType )
{
	AlteiaWorld::CSAlteiaWorldInfo packet;
	memset(&packet, 0, sizeof(AlteiaWorld::CSAlteiaWorldInfo));

	packet.eType = eType;

	CClientSessionManager::GetInstance().SendPacket(CS_ALTEIAWORLD, eAlteiaWorld::CS_ALTEIAWORLD_INFO, (char*)&packet, sizeof(packet));
}

inline void SendAlteaJoin()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ALTEIAWORLD, eAlteiaWorld::CS_ALTEIAWORLD_JOIN, NULL, 0);
}

inline void SendAlteaSendTicket( const WCHAR * wszCharacterName )
{
	AlteiaWorld::CSAlteiaWorldSendTicket packet;
	memset(&packet, 0, sizeof(AlteiaWorld::CSAlteiaWorldSendTicket));

	_wcscpy(packet.wszCharacterName, _countof(packet.wszCharacterName), wszCharacterName, (int)wcslen(wszCharacterName));

	CClientSessionManager::GetInstance().SendPacket(CS_ALTEIAWORLD, eAlteiaWorld::CS_ALTEIAWORLD_SENDTICKET, (char*)&packet, sizeof(packet));
}

inline void SendAlteaDice()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ALTEIAWORLD, eAlteiaWorld::CS_ALTEIAWORLD_DICE, NULL, 0);
}

inline void SendAlteaQuitInfo()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ALTEIAWORLD, eAlteiaWorld::CS_ALTEIAWORLD_QUITINFO, NULL, 0);
}

inline void SendAlteaNextMap()
{
	CClientSessionManager::GetInstance().SendPacket(CS_ALTEIAWORLD, eAlteiaWorld::CS_ALTEIAWORLD_NEXTMAP, NULL, 0);
}

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )