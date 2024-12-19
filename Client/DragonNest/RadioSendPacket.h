
#pragma once

#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include "DnCommonUtil.h"

inline void SendUseRadio(USHORT nRadioID)
{
	CSUseRadio packet;
	memset(&packet, 0 ,sizeof(packet));

	packet.nID = nRadioID;

	CClientSessionManager::GetInstance().SendPacket(CS_RADIO, eRadio::CS_USERADIO, (char*)&packet, sizeof(CSUseRadio));
}