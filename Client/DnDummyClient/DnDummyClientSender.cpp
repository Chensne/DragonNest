#include "stdafx.h"
#include "DNSecure.h"
#include "DnDummySession.h"
#include "DnDummyClientSender.h"
#include "DNCommonDef.h"
#include "DnProtocol.h"
//#include "ServerPacket.h"
#include "NetCommon.h"
#include "DnDummyClient.h"
#include "DnPacket.h"
#include "NetSocketRUDP.h"
#include "RUDPCommonDef.h"
#include "DnDummyClientMgr.h"
#include "EngineUtil.h"
#include "Stream.h"
#include "DnDummySettingPropertyPanel.h"
#include "DNSecure.h"



void
DnDummyClientTCPSender::_SendPacket(NxPacket& packet)
{
	if ( !m_pDummyClient )
		return;

	if ( m_pDummyClient->m_pSession == NULL )
		return;

	m_pDummyClient->m_pSession->SendPacket(packet);
	m_pDummyClient->OnCalcResponseTimeSend();
}



UINT DnDummyClientTCPSender::_GetPacketSequence()
{
	m_PacketSequence++;
	if (m_PacketSequence > 4000000000) m_PacketSequence = 0;
	return m_PacketSequence;
}

void 
DnDummyClientTCPSender::SendPacket(unsigned char cMainCmd, unsigned char cSubCmd, void* pData, int nPacketSize)
{
	DNEncryptPacketSeq EnPacket;
	memset(&EnPacket, 0, sizeof(EnPacket));

	EnPacket.Packet.iLen = static_cast<unsigned short>(sizeof(DNTPacketHeader)+nPacketSize);
	EnPacket.Packet.cMainCmd = static_cast<unsigned char>(cMainCmd);
	EnPacket.Packet.cSubCmd = static_cast<unsigned char>(cSubCmd);
	memcpy(&EnPacket.Packet.buf, pData, nPacketSize);

	EnPacket.nLen = EnPacket.Packet.iLen + sizeof(BYTE) + sizeof(USHORT);
	CDNSecure::GetInstance().Tea_encrypt( reinterpret_cast<char*>(&EnPacket.Packet), EnPacket.Packet.iLen );


	//	EnPacket.nLen = (USHORT)(SecureEncrypt((BYTE*)&EnPacket.Packet, EnPacket.Packet.iLen) + sizeof(ClientSA) + sizeof(USHORT));


	/*DNTPacket tpacket;
	tpacket.iLen = nPacketSize + (int)(sizeof(tpacket) - sizeof(tpacket.buf));
	tpacket.cMainCmd = cMainCmd;
	tpacket.cSubCmd = cSubCmd;
	if ( pData != NULL && nPacketSize > 0 )
	memcpy(tpacket.buf, pData, nPacketSize);*/

	// 패킷구조체 사이즈 + 패킷길이 + 메인커맨드 + 서브커맨드
	NxPacket packet;
	memcpy(packet.GetPacketBuffer(), &EnPacket, EnPacket.nLen);
	_SendPacket(packet);

}

void	
DnDummyClientTCPSender::SendCheckVersion( BYTE cNation, BYTE cVersion )
{
	FUNC_USER_LOG();
	CSCheckVersion Check;
	memset(&Check, 0, sizeof(CSCheckVersion));

	Check.cNation = cNation;
	Check.cVersion = cVersion;
	Check.bCheck = true;

	SendPacket( CS_LOGIN, eLogin::CS_CHECKVERSION, &Check, sizeof(CSCheckVersion));
}

void	
DnDummyClientTCPSender::SendLogin(const WCHAR *pId, const WCHAR *pPwd, const WCHAR *pIp, USHORT wPort)
{
	FUNC_USER_LOG();
	CSCheckLogin Login;
	memset(&Login, 0, sizeof(CSCheckLogin));

	memcpy( Login.wszUserId, pId, sizeof(WCHAR)*wcslen(pId) );
	memcpy( Login.wszPassword, pPwd, sizeof(WCHAR)*wcslen(pPwd) );
	memcpy( Login.wszVirtualIp, pIp, sizeof(WCHAR)*wcslen(pIp) );
	//Login.wVirtualPort = wPort;

	SendPacket( CS_LOGIN, eLogin::CS_CHECKLOGIN, &Login, sizeof(CSCheckLogin));
}

#ifdef _KOR_NEXON
void	
DnDummyClientTCPSender::SendKorCheckLogin(WCHAR *pPassPort)
{
	//pPassPort = L"NP11:auth08:1996548449:ho2MedodZ_Wx6KrdkY1qb2EPw~5L4pp4MN8OnsvXKE41AXFK3NlWAmneH4oue67CpPqlbsJ_Ft1vXdbFGseZvu~uHvaKr7LkNfpASNwA8TA20jmO81ECZ4qyqWaErJmi7GkCCtO4GtesSRGBAu6dXNnyTkpJNUsLJk8TTjvQz~j8yp~UtPbPihU1a3bOgR0fks~DE9PMIxbPchjsRBDkExF_lpkE7i";
#ifdef _KOR_NEXON
	CSKorCheckLogin CheckLogin = { 0, };
	size_t nLen = wcslen(pPassPort);
	if ( nLen > 1024 )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("SendKorCheckLogin 패스포트 길이 너무 큼 "));
		return;
	}
	wcsncpy( CheckLogin.wszPassPort, pPassPort, nLen );
	SendPacket( CS_LOGIN, eLogin::CS_KOR_CHECKLOGIN, &CheckLogin, sizeof(CSKorCheckLogin));
#endif
}

#endif // _KOR_NEXON
void
DnDummyClientTCPSender::SendCharList()
{

	FUNC_USER_LOG();
	/*
	TPacketHeader Header;
	memset(&Header, 0, sizeof(TPacketHeader));

	Header.wLen = sizeof(TPacketHeader);
	Header.cCtrl = CONTROL_CLLO;
	Header.cMainCmd = CS_LOGIN;
	Header.cSubCmd = CS_CHARLIST;
	Header.nUniqueID = m_pDummyClient->m_nUniqueID;
	Header.wSeq = _GetPacketSequence();

	NxPacket packet;
	memcpy(packet.GetPacketBuffer(), &Header, sizeof(TPacketHeader));
	SendPacket(packet);
	*/
}

void	
DnDummyClientTCPSender::SendServerList()
{
	FUNC_USER_LOG();

	SendPacket( CS_LOGIN, eLogin::CS_SERVERLIST, NULL, 0);

}


void
DnDummyClientTCPSender::SendSelectedServer( BYTE cServerIndex )
{
	FUNC_USER_LOG();
	CSSelectServer selectServer;
	memset(&selectServer, 0, sizeof(CSSelectServer));
	selectServer.cServerIndex = cServerIndex;

	SendPacket( CS_LOGIN, eLogin::CS_SELECTSERVER, &selectServer, sizeof(CSSelectServer));

}

void DnDummyClientTCPSender::SendCreateChar(int * pEquipArr, const WCHAR * pName, DWORD dwHair, BYTE cClass)
{
	CSCreateChar Create;
	memset(&Create, 0, sizeof(CSCreateChar));

	_tcsncpy_s(Create.wszCharacterName, pName, NAMELENMAX);
	for (int i = 0; i < EQUIPMAX; i++)
		Create.nEquipArray[i] = pEquipArr[i];
	Create.cClass = cClass;
	Create.dwHairColor = dwHair;

	SendPacket(CS_LOGIN, eLogin::CS_CREATECHAR, (char*)&Create, sizeof(CSCreateChar));
}

void 
DnDummyClientTCPSender::SendSelectedChannel( int nChannelID, bool bSkipTutorial )
{
	FUNC_USER_LOG();
	CSSelectChannel selectChannel;
	memset(&selectChannel, 0, sizeof(CSSelectChannel));

	selectChannel.nChannelID = nChannelID;
#if defined(PRE_SKIPTUTORIAL)
	selectChannel.bSkipTutorial = bSkipTutorial;
#endif	// #if defined(PRE_SKIPTUTORIAL)

	SendPacket( CS_LOGIN, eLogin::CS_SELECTCHANNEL,  &selectChannel, sizeof(CSSelectChannel));
}

void
DnDummyClientTCPSender::SendSelectChar(BYTE cCharIndex)
{
	FUNC_USER_LOG();
	CSSelectChar Select;
	memset(&Select, 0, sizeof(CSSelectChar));

	Select.cCharIndex = cCharIndex;
	SendPacket( CS_LOGIN, eLogin::CS_SELECTCHAR,  &Select, sizeof(CSSelectChar));
}

void
DnDummyClientTCPSender::SendConnectVillage(UINT nSessionID, UINT nAccountDBID, INT64 biCertifiedKey, const WCHAR * pVirtualIP)
{
	FUNC_USER_LOG();
	CSConnectVillage Connect;
	memset(&Connect, 0, sizeof(CSConnectVillage));

	Connect.nSessionID = nSessionID;
	Connect.nAccountDBID = nAccountDBID;
	Connect.biCertifyingKey = biCertifiedKey;
	wcscpy_s(Connect.wszVirtualIp, pVirtualIP);

	SendPacket( CS_SYSTEM, eSystem::CS_CONNECTVILLAGE,  &Connect, sizeof(CSConnectVillage));
}

void	
DnDummyClientTCPSender::SendVillageReady(bool bFirst)
{
	FUNC_USER_LOG();
	CSVillageReady Ready;
	memset(&Ready, 0, sizeof(CSVillageReady));

	Ready.boFirst = bFirst;


	SendPacket( CS_SYSTEM, eSystem::CS_VILLAGEREADY,  &Ready, sizeof(CSVillageReady));
}

void	
DnDummyClientTCPSender::SendEnter()
{
	FUNC_USER_LOG();

	SendPacket( CS_CHAR, eChar::CS_ENTER,  NULL, 0 );

}

void DnDummyClientTCPSender::SendCompleteLoad()
{
	FUNC_USER_LOG();

	SendPacket( CS_CHAR, eChar::CS_COMPLETELOADING,  NULL, 0 );
}


void	
DnDummyClientTCPSender::SendReqPartyListInfo(int nPage)
{
	FUNC_USER_LOG();

	CSPartyListInfo PartyInfo;
	memset(&PartyInfo, 0, sizeof(CSPartyListInfo));

	PartyInfo.cOffSetCnt = PARTYLISTOFFSET;
	PartyInfo.cGetPage = nPage;

	memset(&PartyInfo.nSortMapIdx, 0, sizeof(PartyInfo.nSortMapIdx));

	PartyInfo.cDifficulty = 0;
	PartyInfo.cIsRefresh = 0;

	SendPacket( CS_PARTY, eParty::CS_PARTYLISTINFO, (char*)&PartyInfo, sizeof(CSPartyListInfo) );
}


void	
DnDummyClientTCPSender::SendCreateParty()
{
	FUNC_USER_LOG();
	CSCreateParty CreateParty;
	memset(&CreateParty, 0, sizeof(CSCreateParty));

	CreateParty.cPartyMemberMax = 4;
	CreateParty.cItemLootRule = 0;	//ePartyItemLootRule 참조
	CreateParty.cItemRank = 0;
	CreateParty.cUserLvLimitMin = 1;
	CreateParty.cUserLvLimitMax = 100;
	CreateParty.nTargetMapIdx = 0;
	CreateParty.cDifficulty = 0;

	static int partyCount = 0;
	std::wstring str;
	str = FormatW(L"더미가 만든 파티%d", partyCount++);
	_tcsncpy_s(CreateParty.wszBuf, str.c_str() , 256);
	//_tcsncpy_s(CreateParty.wszBuf, L"더미가 만든 파티에요." , 256); commented by kalliste
	CreateParty.cNameLen = wcslen(CreateParty.wszBuf);

	SendPacket( CS_PARTY, eParty::CS_CREATEPARTY,  (char*)&CreateParty, sizeof(CSCreateParty) - \
		sizeof(CreateParty.wszBuf) + ((CreateParty.cNameLen + CreateParty.cPassWordLen) * sizeof(WCHAR)));
}


void 
DnDummyClientTCPSender::SendJoinParty(int nPartyIndex)
{
	FUNC_USER_LOG();
	CSJoinParty JoinParty;
	memset(&JoinParty, 0, sizeof(CSJoinParty));

	JoinParty.nPartyID = nPartyIndex;


	SendPacket( CS_PARTY, eParty::CS_JOINPARTY,  &JoinParty, sizeof(CSJoinParty)- \
		sizeof(JoinParty.wszBuf) + (JoinParty.cPartyPasswordSize * sizeof(WCHAR)) );
}

void DnDummyClientTCPSender::SendPartyOut()
{
	SendPacket(CS_PARTY, eParty::CS_PARTYOUT, NULL, 0);
}

void	
DnDummyClientTCPSender::SendStartStage(int nMapIdx)
{
	FUNC_USER_LOG();

	//int nMapIndex = g_SettingPropertyPanel->GetSettingPropertyData().nMapIndex;

	std::wstring wszCheat = FormatW(L"/go %d", nMapIdx);
	SendChatMsg(CHATTYPE_NORMAL, wszCheat.c_str() );
}


void
DnDummyClientTCPSender::SendChatMsg( eChatType eType, LPCWSTR wszChatMsg )
{
	m_pDummyClient->AddUserLog(_T("%s"),wszChatMsg);
	FUNC_USER_LOG();
	CSChat ChatMsg;
	memset(&ChatMsg, 0, sizeof(CSChat));

	ChatMsg.eType = eType;
	memcpy( ChatMsg.wszChatMsg, wszChatMsg, min( sizeof(WCHAR)*wcslen(wszChatMsg), sizeof(WCHAR)*(CHATLENMAX-1)) );
	ChatMsg.nLen = wcslen(wszChatMsg);

	SendPacket(CS_CHAT, eChat::CS_CHATMSG, (char*)&ChatMsg, \
		sizeof(CSChat) - sizeof(ChatMsg.wszChatMsg) + (ChatMsg.nLen * 2));
}

void	
DnDummyClientTCPSender::SendGameTcpConnect(UINT nSessionID)
{
	FUNC_USER_LOG();
	CSConnectGame packet;
	memset(&packet, 0, sizeof(packet));
	packet.nSessionID = nSessionID;
	SendPacket(CS_SYSTEM, eSystem::CS_CONNECTGAME, (char*)&packet, sizeof(packet));
}

void DnDummyClientTCPSender::SendActorMsg(UINT nUID, USHORT nProtocol, BYTE * pBuf, int nSize)
{
	CSActorMessage ActorMsg;
	memset( &ActorMsg, 0, sizeof(CSActorMessage) );
	memcpy( ActorMsg.cBuf, pBuf, nSize );

	SendPacket(CS_ACTOR, nProtocol, (char*)&ActorMsg, sizeof(CSActorMessage) - (USHORT)(128 - nSize));
}

void DnDummyClientTCPSender::SendChatMsg(const WCHAR * wszChatMsg)
{
	CSChat ChatMsg;
	memset(&ChatMsg, 0, sizeof(CSChat));

	ChatMsg.eType = CHATTYPE_NORMAL;
	ChatMsg.nLen = (short)wcslen( wszChatMsg );
	memcpy( ChatMsg.wszChatMsg, wszChatMsg, min( sizeof(WCHAR)*wcslen(wszChatMsg), sizeof(WCHAR)*(CHATLENMAX-1)) );

	SendPacket(CS_CHAT, eChat::CS_CHATMSG, (char*)&ChatMsg, sizeof(CSChat) - sizeof(ChatMsg.wszChatMsg) + (ChatMsg.nLen * 2));
}






//--------------------------------------------------------------------------------------------------------

void	
DnDummyClientUDPSender::SendPacket(int iMainCmd, int iSubCmd, void * pData, int iSize, int iPrior)
{
	if ( m_pDummyClient && m_pDummyClient->m_pRUDPSocket )
	{
		m_pDummyClient->m_pRUDPSocket->Send(iMainCmd, iSubCmd, pData, iSize, iPrior, 0);
		g_DummyClientMgr.AddSendBytes(iSize);
		g_DummyClientMgr.AddSendCount();

		m_pDummyClient->OnCalcResponseTimeSend();
	}
}

void
DnDummyClientUDPSender::SendReady2Receive(UINT nUID)
{
	FUNC_USER_LOG();
	CSReady2Recieve packet;
	packet.nSessionID = nUID;

	SendPacket(CS_SYSTEM, eSystem::CS_READY_2_RECIEVE, (char*)&packet, sizeof(packet), _RELIABLE);

}

void	
DnDummyClientUDPSender::SendGameSyncWait(UINT nUID)
{
	FUNC_USER_LOG();
	CSSyncWait packet;
	packet.nSessionID = nUID;

	SendPacket(CS_ROOM, eRoom::CS_SYNC_WAIT, (char*)&packet, sizeof(packet), _RELIABLE);
	/*
	DNPACKET_DATA_SYNC_WAIT packet;
	packet.uid = nUID;
	SendPacket(DNPACKET_CLIENT_SYNC_WAIT, &packet, sizeof(packet), iPrior);
	*/
}

void
DnDummyClientUDPSender::SendGameActorMsg(UINT nUID, USHORT nProtocol, BYTE *pBuf, int nSize, int iPrior)
{

	CSActorMessage packet;
	memcpy(packet.cBuf, pBuf, nSize);

	packet.nSessionID = m_pDummyClient->m_nUniqueID;


	g_DummyClientMgr.AddSendBytes(nSize);
	g_DummyClientMgr.AddSendCount();

	SendPacket(CS_ACTOR, nProtocol, (char*)&packet,	sizeof(packet) - sizeof(packet.cBuf) + nSize, iPrior);




}

void	
DnDummyClientUDPSender::SendCmdMove(int nActionIndex, char cMovePushKeyFlag, int nLoopCnt, float fBlendFrame)
{

	FUNC_USER_LOG();
	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );


	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &(m_pDummyClient->GetProperty().m_vPos), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &(m_pDummyClient->GetProperty().m_vDir), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &(m_pDummyClient->GetProperty().m_vDir), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &cMovePushKeyFlag, sizeof(char) );

	SendGameActorMsg( m_pDummyClient->m_nUniqueID, (USHORT)eActor::CS_CMDMOVE, (BYTE*)Stream.GetBuffer(), Stream.Tell(), _RELIABLE );

	EtVector3& vPos = m_pDummyClient->GetProperty().m_vPos;
	EtVector3 vXVec;
	EtVector2 vZVec;
	vZVec = ( m_pDummyClient->GetProperty().m_vDir );

	EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

	float fXSpeed = 0.f, fZSpeed = 0.f;
	if( cMovePushKeyFlag & 0x01 ) fXSpeed = -100000.f;
	if( cMovePushKeyFlag & 0x02 ) fXSpeed = 100000.f;
	if( cMovePushKeyFlag & 0x04 ) fZSpeed = 100000.f;
	if( cMovePushKeyFlag & 0x08 ) fZSpeed = -100000.f;
	vPos += ( vXVec * fXSpeed );
	vPos += ( EtVec2toVec3( m_pDummyClient->GetProperty().m_vDir ) * fZSpeed );


}

void
DnDummyClientUDPSender::SendCmdAction(int nActionIdx, int nLoopCnt, float fBlendFrame)
{

	FUNC_USER_LOG();
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	char m_cMovePushKeyFlag = 0;

	Stream.Write( &nActionIdx, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &nLoopCnt, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
	Stream.Write( &(m_pDummyClient->GetProperty().m_vPos), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &(m_pDummyClient->GetProperty().m_vDir), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &(m_pDummyClient->GetProperty().m_vDir), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

	Stream.Write( &m_cMovePushKeyFlag, sizeof(char) );

	SendGameActorMsg( m_pDummyClient->m_nUniqueID, (USHORT)eActor::CS_CMDACTION, (BYTE*)Stream.GetBuffer(), Stream.Tell(), _RELIABLE );

}

void
DnDummyClientUDPSender::SendCmdStop()
{
	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &(m_pDummyClient->GetProperty().m_vPos), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

	SendGameActorMsg( m_pDummyClient->m_nUniqueID, (USHORT)eActor::CS_CMDSTOP, (BYTE*)Stream.GetBuffer(), Stream.Tell(), _RELIABLE );

}
void	
DnDummyClientUDPSender::SendToggleBattle(bool b)
{

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );	

	Stream.Write( &b, sizeof(bool) );

	SendGameActorMsg( m_pDummyClient->m_nUniqueID, (USHORT)eActor::CS_CMDTOGGLEBATTLE, (BYTE*)Stream.GetBuffer(), Stream.Tell(), _RELIABLE );

}

void DnDummyClientUDPSender::SendStartStage(int nMapIdx)
{
	FUNC_USER_LOG();

	//int nMapIndex = g_SettingPropertyPanel->GetSettingPropertyData().nMapIndex;

	std::wstring wszCheat = FormatW(L"/changemap %d", nMapIdx);
	SendChatMsg(CHATTYPE_NORMAL, wszCheat.c_str() );
}

void	DnDummyClientUDPSender::SendChatMsg( eChatType eType, LPCWSTR wszChatMsg )
{
	FUNC_USER_LOG();
	CSChat ChatMsg;
	memset(&ChatMsg, 0, sizeof(CSChat));

	ChatMsg.eType = eType;
	memcpy( ChatMsg.wszChatMsg, wszChatMsg, min( sizeof(WCHAR)*wcslen(wszChatMsg), sizeof(WCHAR)*(CHATLENMAX-1)) );
	ChatMsg.nLen = wcslen(wszChatMsg);

	SendPacket(CS_CHAT, eChat::CS_CHATMSG, (char*)&ChatMsg, \
		(int)(sizeof(eChatType)+(sizeof(WCHAR)*wcslen(ChatMsg.wszChatMsg))), _RELIABLE);
}