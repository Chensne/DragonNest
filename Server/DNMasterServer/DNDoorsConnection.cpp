
#include "Stdafx.h"
#include "DNDoorsConnection.h"
#include "DNIocpManager.h"
#include "Log.h"
#include "Util.h"
#include "DNServerPacket.h"
#include "DNDoorsPacket.h"
#include "DNUser.h"
#include "DNDivisionManager.h"

#if defined(PRE_ADD_DOORS)

extern TMasterConfig g_Config;
CDNDoorsConnection * g_pDoorsConnection = NULL;

CDNDoorsConnection::CDNDoorsConnection() : m_dwReconnectTick(0)
{
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)
}

CDNDoorsConnection::~CDNDoorsConnection()
{
}

int CDNDoorsConnection::MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	switch (iMainCmd)
	{
	case DoorsProtocol::DDN_CHAT:
		{
			TDoorsChat * pPacket = (TDoorsChat*)pData;

			if (pPacket->cNameCount >= NAMELENMAX || pPacket->nMessageCount >= CHATLENMAX)
			{
				_DANGER_POINT_MSG(L"DoorsChat string overflow");
				return ERROR_NONE;
			}

			WCHAR wszName[NAMELENMAX];
			WCHAR wszMessage[CHATLENMAX];
			memset(wszName, 0, sizeof(wszName));
			memset(wszMessage, 0, sizeof(wszMessage));

			_wcscpy(wszName, NAMELENMAX, pPacket->wszMessage, pPacket->cNameCount);
			_wcscpy(wszMessage, CHATLENMAX, pPacket->wszMessage + pPacket->cNameCount, pPacket->nMessageCount);

			if (g_pDivisionManager)
			{
				if (g_pDivisionManager->DoorsChat(wszName, pPacket->biDestCharacterID, wszMessage, pPacket->nMessageCount))
					_DANGER_POINT();
			}
			return ERROR_NONE;
		}		

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	case DoorsProtocol::DDN_GUILDCHAT:
		{
			TDoorsGuildChat * pPacket = (TDoorsGuildChat*)pData;

			if (pPacket->cNameCount >= NAMELENMAX || pPacket->nMessageCount >= CHATLENMAX)
			{
				_DANGER_POINT_MSG(L"DoorsChat string overflow");
				return ERROR_NONE;
			}

			WCHAR wszMessage[CHATLENMAX];
			memset(wszMessage, 0, sizeof(wszMessage));
			_wcscpy(wszMessage, CHATLENMAX, pPacket->wszMessage + pPacket->cNameCount, pPacket->nMessageCount);

			if (g_pDivisionManager)
			{
				if (g_pDivisionManager->DoorsGuildChat(pPacket->biSrcCharacterID, pPacket->nGuildID, wszMessage, pPacket->nMessageCount))
					_DANGER_POINT();
			}

			return ERROR_NONE;
		}	
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

#ifdef PRE_ADD_DOORS_SELFKICK
	case DoorsProtocol::DDN_SELFKICK:
		{
			TDoorsSelfKick * pPacket = (TDoorsSelfKick*)pData;

			if (pPacket->nAccountID <= 0)
			{
				g_Log.Log(LogType::_DOORSLOG, L"Doors DetachUser Invalid Request [ADBID:%d][CHDBID:%I64d\n", pPacket->nAccountID, pPacket->biCharacterID);
				return ERROR_NONE;
			}

			if (g_pDivisionManager)
			{
				CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountID);
				if (pUser)
					g_pDivisionManager->SendDetachUser(pPacket->nAccountID);

				//변경되어진 캐선창기준으로 캐선창 타이밍에 해당유저가 마스터에 들어있지않음 무조건 날린다.
				g_pDivisionManager->SendAllLoginServerDetachUser(pPacket->nAccountID);
				g_Log.Log(LogType::_DOORSLOG, pUser, L"Doors DetachUser Request [ADBID:%d]\n", pPacket->nAccountID);
				return ERROR_NONE;
			}

			_DANGER_POINT_MSG(L"Doors SelfKick User Not Found");
			return ERROR_NONE;
		}
#endif		//#ifdef PRE_ADD_DOORS_SELFKICK
	}
	return ERROR_UNKNOWN_HEADER;
}

void CDNDoorsConnection::Reconnect(DWORD CurTick)
{
	if (m_wPort == 0) return;
	if (m_dwReconnectTick + 5000 < CurTick){
		m_dwReconnectTick = CurTick;

		if (!GetActive() && !GetConnecting() )
		{
			SetConnecting(true);
			if (g_pIocpManager->AddConnectionEx(this, CONNECTIONKEY_DOORS, m_szIP, m_wPort) < 0)
			{
				SetConnecting(false);
				g_Log.Log(LogType::_FILEDBLOG, L"DoorsServer Fail(%S, %d)\r\n", m_szIP, m_wPort);
			}
			else {
				g_Log.Log(LogType::_FILEDBLOG, L"DoorsServer(%S, %d) Connecting..\r\n", m_szIP, m_wPort);				
			}
		}
	}
}

void CDNDoorsConnection::SendRegist(char cWorldID, const char * pszIP, int nPort, const char * pszIP2, int nPort2)
{
	TDNDRegist packet;
	memset(&packet, 0, sizeof(TDNDRegist));

	packet.cWorldID = cWorldID;
	_strcpy(packet.szIP, IPLENMAX, pszIP, (int)strlen(pszIP));
	packet.nPort = nPort;
	_strcpy(packet.szIP2, IPLENMAX, pszIP2, (int)strlen(pszIP2));
	packet.nPort2 = nPort2;

	AddSendData(DoorsProtocol::DND_REGIST, 0, reinterpret_cast<char*>(&packet), sizeof(TDNDRegist));
}

bool CDNDoorsConnection::SendChatToDoors(CDNUser * pUser, INT64 biDestCharacterDBID, const WCHAR * pMessage, int nLen)
{
	if (pMessage == NULL) return false;

	TDoorsChat packet;
	memset(&packet, 0, sizeof(TDoorsChat));

	packet.cWorldID = static_cast<char>(g_Config.nWorldSetID);
	packet.biSrcCharacterID = pUser->GetCharacterDBID();	
	packet.biDestCharacterID = biDestCharacterDBID;	
	packet.cNameCount = static_cast<unsigned char>(wcslen(pUser->GetCharacterName()));
	packet.nMessageCount = static_cast<unsigned char>(nLen);

	_wcscpy(packet.wszMessage, NAMELENMAX + CHATLENMAX, pUser->GetCharacterName(), packet.cNameCount);
	_wcscpy(packet.wszMessage + packet.cNameCount, NAMELENMAX + CHATLENMAX - packet.cNameCount, pMessage, packet.nMessageCount);
	
	return AddSendData(DoorsProtocol::DND_CHAT, 0, reinterpret_cast<char*>(&packet), sizeof(TDoorsChat) - sizeof(packet.wszMessage) + ((packet.cNameCount + packet.nMessageCount) * sizeof(wchar_t))) < 0 ? false : true;
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
bool CDNDoorsConnection::SendGuildChatToDoors(CDNUser * pUser, int nGuildID, const WCHAR * pMessage, int nLen)
{
	if (pMessage == NULL || nLen == 0) return false;

	TDoorsGuildChat packet;
	memset(&packet, 0, sizeof(TDoorsGuildChat));

	packet.cWorldID = static_cast<char>(g_Config.nWorldSetID);
	packet.nGuildID = nGuildID;
	packet.biSrcCharacterID = pUser->GetCharacterDBID();
	packet.cNameCount = static_cast<unsigned char>(wcslen(pUser->GetCharacterName()));
	packet.nMessageCount = static_cast<unsigned char>(nLen);

	_wcscpy(packet.wszMessage, NAMELENMAX + CHATLENMAX, pUser->GetCharacterName(), packet.cNameCount);
	_wcscpy(packet.wszMessage + packet.cNameCount, NAMELENMAX + CHATLENMAX - packet.cNameCount, pMessage, packet.nMessageCount);

	return AddSendData(DoorsProtocol::DND_GUILDCHAT, 0, reinterpret_cast<char*>(&packet), sizeof(TDoorsChat) - sizeof(packet.wszMessage) + ((packet.cNameCount + packet.nMessageCount) * sizeof(wchar_t))) < 0 ? false : true;
}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

#endif	// #if defined(PRE_ADD_DOORS)