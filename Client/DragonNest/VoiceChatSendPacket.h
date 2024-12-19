#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"

inline void SendVoiceChatAvailable(bool bAvailable)
{
	CSVoiceChatAvailable packet;
	memset(&packet, 0, sizeof(packet));

	packet.cAvailable = bAvailable == true ? 1 : 0;

	CClientSessionManager::GetInstance().SendPacket(CS_VOICECHAT, eVoiceChat::CS_VOICEAVAILABLE, (char*)&packet, sizeof(packet));
}

inline void SendVoiceMute(UINT nSessionID, bool bMuteFlag)
{
	CSVoiceMute packet;
	memset(&packet, 0, sizeof(CSVoiceMute));

	packet.cMute = bMuteFlag == true ? 1 : 0;
	packet.nSessionID = nSessionID;

	CClientSessionManager::GetInstance().SendPacket(CS_VOICECHAT, eVoiceChat::CS_VOICEMUTE, (char*)&packet, sizeof(packet));
}

inline void SendVoiceComplaint(UINT nSessionID)
{
	CSVoiceComplaintReq packet;
	memset(&packet, 0, sizeof(CSVoiceComplaintReq));

	packet.nSessionID = nSessionID;
	sprintf_s(packet.szCategory, _countof(packet.szCategory), "test_category");
	sprintf_s(packet.szSubject, _countof(packet.szSubject), "test_subject");
	sprintf_s(packet.szMsg, _countof(packet.szMsg), "test_msg");

	CClientSessionManager::GetInstance().SendPacket(CS_VOICECHAT, eVoiceChat::CS_VOICECOMPLAINTREQ, (char*)&packet, sizeof(packet));
}