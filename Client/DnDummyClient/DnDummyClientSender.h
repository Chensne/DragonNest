#pragma once

#include "NxPacket.h"
class DnDummyClient;
class DnDummyClientTCPSender
{
public:
	DnDummyClientTCPSender() { m_PacketSequence = 0; m_pDummyClient = NULL; }
	virtual ~DnDummyClientTCPSender() { }

	void	SetDummyClient(DnDummyClient* p) { m_pDummyClient = p; }
	void	_SendPacket(NxPacket& packet);
	void	SendPacket(unsigned char cMainCmd, unsigned char cSubCmd, void* pData, int nPacketSize);
public:
	void	SendCheckVersion( BYTE cNation, BYTE cVersion );
	void	SendLogin(const WCHAR *pId, const WCHAR *pPwd, const WCHAR *pIp, USHORT wPort);
#ifdef _KOR_NEXON
	void	SendKorCheckLogin(WCHAR *pPassPort);
#endif // _KOR_NEXON

	
	void	SendCharList();
	void	SendServerList();
	void	SendSelectedServer( BYTE cServerIndex );
	void SendCreateChar(int * pEquipArr, const WCHAR * pName, DWORD dwHair, BYTE cClass);
	void	SendSelectChar(BYTE cCharIndex);
	void	SendSelectedChannel( int nChannelID, bool bSkipTutorial );

	void	SendConnectVillage(UINT nSessionID, UINT nAccountDBID, INT64 biCertifiedKey, const WCHAR * pVirtualIP);


	void	SendVillageReady(bool bFirst);
	void	SendEnter();
	void SendCompleteLoad();

	void	SendReqPartyListInfo(int nPage);
	void	SendCreateParty();
	void	SendJoinParty(int nPartyIndex);
	void SendPartyOut();

	void	SendStartStage(int nMapIdx);
	void	SendChatMsg( eChatType eType, LPCWSTR wszChatMsg );

	void	SendGameTcpConnect(UINT nSessionID);
	void SendActorMsg(UINT nUID, USHORT nProtocol, BYTE * pBuf, int nSize);
	void SendChatMsg(const WCHAR * pMsg);

	

private:
	UINT	_GetPacketSequence();

	DnDummyClient*	m_pDummyClient;
	UINT			m_PacketSequence;
};

class DnDummyClientUDPSender
{
public:
	
	DnDummyClientUDPSender() { m_pDummyClient = NULL; }
	virtual ~DnDummyClientUDPSender() { }

	void	SetDummyClient(DnDummyClient* p) { m_pDummyClient = p; }
	void	SendPacket(int iMainCmd, int iSubCmd, void * pData, int iSize, int iPrior);
public:

	void	SendReady2Receive(UINT nUID);
	void	SendGameSyncWait(UINT nUID);

	void	SendGameActorMsg(UINT nUID, USHORT nProtocol, BYTE *pBuf, int nSize, int iPrior);


	void	SendCmdStop();
	void	SendCmdMove(int nDir, char cMovePushKeyFlag, int nLoopCnt, float fBlendFrame);
	void	SendCmdAction(int nActionIdx, int nLoopCnt, float fBlendFrame);
	void	SendToggleBattle(bool b);
	void SendStartStage(int nMapIdx);
	void	SendChatMsg( eChatType eType, LPCWSTR wszChatMsg );


private:
	DnDummyClient*	m_pDummyClient;
};
