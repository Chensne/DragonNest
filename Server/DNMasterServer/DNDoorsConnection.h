
#pragma once

#include "Connection.h"

#if defined(PRE_ADD_DOORS)

class CDNUser;
class CDNDoorsConnection : public CConnection
{
public:
	CDNDoorsConnection();
	virtual ~CDNDoorsConnection();

	//NetworkMessage Process
	int MessageProcess(int iMainCmd, int iSubCmd, char * pData, int iLen);
	void Reconnect(DWORD CurTick);

	void SendRegist(char cWorldID, const char * pszIP, int nPort, const char * pszIP2, int nPort2);
	bool SendChatToDoors(CDNUser * pUser, INT64 biDestCharacterDBID, const WCHAR * pMessage, int nLen);
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	bool SendGuildChatToDoors(CDNUser * pUser, int nGuildID, const WCHAR * pMessage, int nLen);
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

private:
	DWORD m_dwReconnectTick;
};

extern CDNDoorsConnection * g_pDoorsConnection;

#endif	// #if defined(PRE_ADD_DOORS)