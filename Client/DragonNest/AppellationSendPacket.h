#pragma once

#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include <strsafe.h>

inline void SendSelectAppellation( int nArrayIndex, int nCoverArrayIndex )
{
	CSSelectAppellation packet;
	packet.nArrayIndex = nArrayIndex;
	packet.nCoverArrayIndex = nCoverArrayIndex;

	CClientSessionManager::GetInstance().SendPacket(CS_APPELLATION, eAppellation::CS_SELECT_APPELLATION, (char*)&packet, sizeof(packet));
}

inline void SendCollectionBook(int nCollectionID)
{
	CSCollectionBook Appellation = { 0, };
	Appellation.nCollectionID = nCollectionID;
	CClientSessionManager::GetInstance().SendPacket(CS_APPELLATION, eAppellation::CS_COLLECTIONBOOK, (char*)&Appellation, sizeof(Appellation));
}
