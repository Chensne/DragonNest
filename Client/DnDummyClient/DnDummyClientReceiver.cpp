#include "stdafx.h"
#include "DNSecure.h"
#include "DnDummyClientReceiver.h"
#include "DNCommonDef.h"
#include "DnProtocol.h"
#include "DnPacket.h"
//#include "ServerPacket.h"
#include "NetCommon.h"
#include "DnDummyClient.h"
#include "DnServerApp.h"
#include "DnDummySession.h"
#include "DnDummySettingPropertyPanel.h"
#include "DnPacket.h"
#include "EngineUtil.h"
#include "Stream.h"
#include "DnDummyClientMgr.h"

void
DnDummyClientTCPReceiver::OnPacket(NxPacket& Packet)
{
	//DNTPacket *pHeader = (DNTPacket*)Packet.GetPacketBuffer();

	DNEncryptPacketSeq *pHeader = (DNEncryptPacketSeq*)Packet.GetPacketBuffer();
	int nLen = pHeader->nLen - sizeof(BYTE) - sizeof(USHORT);
	CDNSecure::GetInstance().Tea_decrypt( reinterpret_cast<char*>(&pHeader->Packet), nLen );

	_OnPacket(pHeader->Packet.cMainCmd, pHeader->Packet.cSubCmd, pHeader->Packet.buf, pHeader->Packet.iLen);
	
}


void
DnDummyClientTCPReceiver::_OnPacket(int iMainCmd, int iSubCmd, char *pData, int nSize)
{
	switch( iMainCmd)
	{
	case SC_ACTOR:	OnTRecvActorMsg(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_ACTORBUNDLE: OnTRecvActorBundleMessage(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_LOGIN:	OnTRecvLoginMsg(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_CHAR:	OnTRecvCharMsg(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_PARTY:	OnTRecvPartyMsg(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_SYSTEM: OnTRecvSystemMsg(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_ROOM:	OnTRecvRoomMsg(iMainCmd, iSubCmd, pData, nSize);	break;
	case SC_CHAT: 
		{
			SCChat * pPacket = (SCChat*)pData;			
			m_pDummyClient->Answer(_wtoi(pPacket->wszChatMsg), iMainCmd, iSubCmd, timeGetTime());
		}
	case SC_ITEM:
		OnTRecvItemMsg( iMainCmd, iSubCmd, pData, nSize);
		break;
	}
}



void
DnDummyClientTCPReceiver::OnTRecvSystemMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{

	switch(iSubCmd)
	{
	case eSystem::SC_VILLAGEINFO:		OnRecvSystemVillageInfo( pData ); break;
	case eSystem::SC_CONNECTVILLAGE:	OnRecvSystemConnectVillage( pData ); break;
	case eSystem::SC_GAMEINFO:			OnRecvSystemGameInfo( pData ); break;
	case eSystem::SC_CONNECTED_RESULT:	OnRecvSystemConnectedResult( pData ); break;
	case eSystem::SC_RECONNECT_REQ:		OnRecvSystemReconnectReq(); break;
	case eSystem::SC_TCP_CONNECT_REQ:	OnRecvSystemGameTcpConnectReq(); break;
	case eSystem::SC_RECONNECTLOGIN:	OnRecvSystemReconnectLogin( pData ); break;
		
	}

}

void	
DnDummyClientTCPReceiver::OnTRecvLoginMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{

	switch(iSubCmd){
		case eLogin::SC_CHECKVERSION: 
			{
				FUNC_USER_LOG();
				SCCheckVersion *pVersion = (SCCheckVersion *)pData;

				if (pVersion->nRet != 0)
					_ASSERT(0);

				/*!!!
				if( pVersion->cRet != 0 ) 
				{ 
					m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
					m_pDummyClient->AddUserLog(_T("버전이 맞지 않습니다."));
					return;
				}
				*/
				tstring szIP;
				WORD nPort;
				g_ServerApp.GetLocalIpPort(m_pDummyClient->m_pSession->GetUID(), szIP, nPort);
#ifdef _KOR_NEXON
				m_pDummyClient->m_TCPSender.SendKorCheckLogin((WCHAR*)m_pDummyClient->m_CreateInfo.szExtra.c_str());
#else  //_KOR_NEXON
				m_pDummyClient->m_TCPSender.SendLogin(m_pDummyClient->m_CreateInfo.szUserID.c_str(), m_pDummyClient->m_CreateInfo.szPasswd.c_str(), szIP.c_str(), nPort);
#endif //_KOR_NEXON
			}
			break;
		case eLogin::SC_CHECKLOGIN: 
			{
				

				OnTRecvLoginCheckLogin( pData );
			}
			break;
		case eLogin::SC_SERVERLIST:
			{
				OnTRecvLoginServerList(pData);
			}
			break;

		case eLogin::SC_CHARLIST:
			{
				OnTRecvLoginCharList( pData );
			}
			break;

		case eLogin::SC_CREATECHAR:
			{
				OnTRecvLoginCharCreate( pData );
			}
			break;

		case eLogin::SC_DELETECHAR:
			{
				SCDeleteChar *pCharDelete = (SCDeleteChar *)pData;

				OnTRecvLoginCharDelete( pData );

				if( pCharDelete->nRet == 0 )
				{
					//m_pDummyClient->m_TCPSender.SendCharList();
				}
			}
			break;
		case eLogin::SC_CHANNELLIST:
			{
				OnTRecvLoginChannelList(pData);

			}
			break;
	}
}



void 
DnDummyClientTCPReceiver::OnTRecvCharMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{

	switch(iSubCmd) 
	{
		case eChar::SC_MAPINFO:	OnTRecvVillageMapInfo( pData );		break;
		case eChar::SC_ENTER:		OnTRecvVillageEntered( pData );		break;
		case eChar::SC_ENTERUSER:	OnTRecvVillageUserEntered( pData );	break;
//		case eChar::SC_ACTORMSG:	OnTRecvVillageActorMsg( pData );		break;

		default:
			break;
	}
}

void 
DnDummyClientTCPReceiver::OnTRecvPartyMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{


	switch(iSubCmd)
	{
		case eParty::SC_CREATEPARTY: OnTRecvPartyCreateParty(pData); break;
//		case SC_DESTROYPARTY: OnTRecvPartyDestroyedParty(pPacket); break;
		case eParty::SC_JOINPARTY: OnTRecvPartyJoinParty(pData); break;

		case eParty::SC_REFRESHPARTY: OnTRecvPartyRefresh(pData); break;
//		case SC_PARTYOUT: OnTRecvPartyOutParty( pPacket ); break;
		//!!!case eParty::SC_PARTYLIST: OnTRecvPartyList( pData ); break;
		case eParty::SC_PARTYLISTINFO: OnTRecvPartyListInfo( pData ); break;
		case eParty::SC_PARTYINFO_ERROR: OnTRecvPartyListInfoError( pData ); break;

//		case SC_PUSHPARTYLIST: OnTRecvPartyListPush( pPacket ); break;
//		case SC_ERASEPARTYLIST: OnTRecvPartyListErase( pPacket ); break;

		case eParty::SC_REFRESHGATEINFO: OnTRecvPartyRefreshGateInfo( pData ); break;
		case eParty::SC_GATEINFO: OnTRecvPartyReadyGate( pData ); break;
		case eParty::SC_STARTSTAGE: 
		case eParty::SC_STARTSTAGEDIRECT:
			{
				OnTRecvPartyStageStart( pData );	
			}
			break;

		case eParty::SC_CANCELSTAGE: OnTRecvPartyStageCancel( pData ); break;
		case eParty::SC_EQUIPDATA: OnRecvPartyEquipData( pData ); break;
	}
}


void 
DnDummyClientTCPReceiver::OnTRecvRoomMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{
	switch( iSubCmd )
	{
		case eRoom::SC_SYNC_WAIT:  OnRecvRoomSyncWait( pData );break;
		case eRoom::SC_SYNC_START: OnRecvRoomSyncStart( pData ); break;
	}

}

void DnDummyClientTCPReceiver::OnTRecvItemMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{
	switch( iSubCmd )
	{
		case eItem::SC_INVENLIST: OnRecvInvenList( pData ); break;
	}
}

void DnDummyClientTCPReceiver::OnRecvInvenList( char *pPacket )
{
	m_InvenList = *( SCInvenList * )pPacket;
}

void
DnDummyClientTCPReceiver::OnRecvRoomSyncWait( char *pPacket )
{
	FUNC_USER_LOG();
	m_pDummyClient->AddUserLog(_T("동기화대기중"));
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_WAIT_GAME_START;


}

void 
DnDummyClientTCPReceiver::OnRecvRoomSyncStart( char *pPacket )
{
	FUNC_USER_LOG();
	m_pDummyClient->AddUserLog(_T("게임 시작!!! "));
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_GAME_PLAY;
	m_pDummyClient->GetProperty().m_vPos = m_pDummyClient->GetNewPos();

	m_pDummyClient->m_UDPSender.SendToggleBattle(true);

	m_pDummyClient->m_UDPSender.SendStartStage(1);

	m_pDummyClient->SetRemainPlayTime( g_SettingPropertyPanel->GetSettingPropertyData().nBasePlayTime + rand()%(g_SettingPropertyPanel->GetSettingPropertyData().nRandomPlayTime) );

}

void 
DnDummyClientTCPReceiver::OnRecvSystemVillageInfo( char *pData )
{
	FUNC_USER_LOG();
	// 로그인서버랑 접속 끊고
	// 빌리지 서버로 접속한다. 
	
	if ( m_pDummyClient->GetProperty().nCurPosition != CurrentServerInfo::_LOGIN_SERVER )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("로그인 상태가 아님."));
		m_pDummyClient->Disconnect();
		return;
	}
	m_pDummyClient->Disconnect();
	m_pDummyClient->Update();


	SCVillageInfo* pPacket = (SCVillageInfo*)pData;
	std::wstring wszIP;
	ToWideString(pPacket->szVillageIp, wszIP);
	m_pDummyClient->m_biCertifyingKey = pPacket->biCertifyingKey;
	m_pDummyClient->m_nAccountDBID = pPacket->nAccountDBID;
	m_pDummyClient->ConnectToVillageServer(wszIP.c_str(), pPacket->wVillagePort);
}

void 
DnDummyClientTCPReceiver::OnRecvSystemConnectVillage( char *pData )
{
	FUNC_USER_LOG();
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_VILLAGE_LOGIN;
	m_pDummyClient->AddUserLog(_T("빌리지서버에 입장 시도"));

	m_pDummyClient->m_TCPSender.SendVillageReady(true);

}

void
DnDummyClientTCPReceiver::OnRecvSystemGameInfo( char *pData )
{
	FUNC_USER_LOG();
	if ( m_pDummyClient->GetProperty().nCurPosition != CurrentServerInfo::_VILLAGE_SERVER )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("빌리지 상태가 아님."));
		m_pDummyClient->Disconnect();
		return;
	}

	// 빌리지와서 접속도 끊는다.
	m_pDummyClient->Disconnect();
	m_pDummyClient->Update();


	SCGameInfo* pPacket = (SCGameInfo*)pData;
	m_pDummyClient->ConnectToGameServer(pPacket->nGameServerIP, pPacket->nGameServerPort, pPacket->nGameServerTcpPort);

}

void DnDummyClientTCPReceiver::OnRecvSystemReconnectReq()
{
	FUNC_USER_LOG();
}

void DnDummyClientTCPReceiver::OnRecvSystemGameTcpConnectReq()
{
	FUNC_USER_LOG();
	m_pDummyClient->m_TCPSender.SendGameTcpConnect(m_pDummyClient->m_nUniqueID);

}

void 
DnDummyClientTCPReceiver::OnRecvSystemReconnectLogin( char* pData )
{
	FUNC_USER_LOG();
	SCReconnectLogin* pPacket = (SCReconnectLogin *)pData;
	

}

void
DnDummyClientTCPReceiver::OnRecvSystemConnectedResult( char *pData )
{
	FUNC_USER_LOG();
	m_pDummyClient->m_UDPSender.SendReady2Receive(m_pDummyClient->m_nUniqueID);

}

void 
DnDummyClientTCPReceiver::OnTRecvLoginCheckLogin( char *pPacket )
{
	FUNC_USER_LOG();
	SCCheckLogin *pLogin = (SCCheckLogin*)pPacket;

	if( pLogin->nRet != 0 ) 
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		std::wstring logStr;
		if (pLogin->nRet == 4)
			logStr = L"중복로그인";
		else
			logStr = L"로그인 실패";
		m_pDummyClient->AddUserLog(logStr.c_str());
		m_pDummyClient->Disconnect();
		return;
	}

	m_pDummyClient->m_TCPSender.SendServerList();
	m_pDummyClient->m_nUniqueID = pLogin->nSessionID;
}

void 
DnDummyClientTCPReceiver::OnTRecvLoginServerList( char *pPacket )
{
	FUNC_USER_LOG();

	SCServerList *pServerList = (SCServerList*)pPacket;

	if ( pServerList->cServerCount > 0 )
	{
		for ( int i = 0 ; i < WORLDCOUNTMAX ; i++ )
		{
			if ( g_SettingPropertyPanel->GetSettingPropertyData().szServerName == pServerList->ServerListData[i].wszServerName )
			{
				m_nSelectedServerIdx = pServerList->ServerListData[i].cServerIndex;
				return m_pDummyClient->m_TCPSender.SendSelectedServer((BYTE)m_nSelectedServerIdx);

			}
		}
	}
	else
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("서버리스트가 없음 "));
		m_pDummyClient->Disconnect();
	}

	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
	m_pDummyClient->AddUserLog(_T("서버 이름에 맞는 서버를 찾을수 없음 "));
	m_pDummyClient->Disconnect();
}

void 
DnDummyClientTCPReceiver::OnTRecvLoginChannelList( char *pPacket )
{
	FUNC_USER_LOG();
	SCChannelList *pData = (SCChannelList*)pPacket;
	
	int nChannelIdx = g_SettingPropertyPanel->GetSettingPropertyData().nChannelIndex;
	m_pDummyClient->m_TCPSender.SendSelectedChannel(nChannelIdx, false);
	m_pDummyClient->AddUserLog( _T("채널에 접속: [%d]번 채널"), nChannelIdx);

	//m_pDummyClient->m_TCPSender.SendSelectedChannel(rand()%pData->cCount);

}

static int s_Equip[] = { 295001, 296003, 0, 291003, 292003, 293001, 294003, 0, 0, 0, 0, 0, 0, 0, 0 };
static DWORD s_Hair = 235087017;
static BYTE s_Class = 1;


void 
DnDummyClientTCPReceiver::OnTRecvLoginCharList( char *pPacket )
{

	FUNC_USER_LOG();
	SCCharList *pCharList = (SCCharList*)pPacket;

	if (pCharList->nRet != ERROR_NONE)
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("캐릭터리스트 에러"));
		m_pDummyClient->Disconnect();
		return;
	}

	int nCharSlotCount = pCharList->cCharCount;
	std::vector<int>	IndexList;
	for ( int i = 0 ; i < nCharSlotCount ; i++ )
	{
		IndexList.push_back((int)pCharList->CharListData[i].cCharIndex);
	}

	if ( IndexList.size() < 1 )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("캐릭터없음"));
		m_pDummyClient->Disconnect();
		/*WCHAR wszCharName[NAMELENMAX];
		std::vector<std::wstring> tokens;
		TokenizeW(m_pDummyClient->GetProperty().szUserID, tokens, std::wstring(L"_") );
		_swprintf(wszCharName, L"%s%d", tokens[0].c_str(), s_Class);
		m_pDummyClient->m_TCPSender.SendCreateChar(s_Equip, wszCharName, s_Hair, s_Class);
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_CREATECHAR;*/
		return;
	}

	//int nSelected = IndexList[rand()%IndexList.size()];
	int nSelected = IndexList[ 0 ];
	m_pDummyClient->GetProperty().cClass = pCharList->CharListData[nSelected].cJob;

	m_pDummyClient->AddUserLog(_T("%d"), nSelected);

	switch(m_pDummyClient->GetProperty().cClass)
	{
	case CLASS_WARRIER:
		m_pDummyClient->GetProperty().szCharacter = _T("워리어");
		break;
	case CLASS_ARCHER:
		m_pDummyClient->GetProperty().szCharacter = _T("아쳐");
		break;
	case CLASS_SOCERESS:
		m_pDummyClient->GetProperty().szCharacter = _T("소서리스");
		break;
	case CLASS_CLERIC:
		m_pDummyClient->GetProperty().szCharacter = _T("클레릭");
		break;
	}

	m_pDummyClient->m_TCPSender.SendSelectChar((BYTE)nSelected);

}

void 
DnDummyClientTCPReceiver::OnTRecvLoginCharCreate( char *pData )
{
	FUNC_USER_LOG();
	SCCreateChar * pPacket = (SCCreateChar*)pData;

	if (pPacket->nRet == ERROR_NONE)
		m_pDummyClient->m_TCPSender.SendSelectedServer((BYTE)m_nSelectedServerIdx);
	else
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->Disconnect();
	}
}

void 
DnDummyClientTCPReceiver::OnTRecvLoginCharDelete( char *pPacket )
{
	FUNC_USER_LOG();

}

void
DnDummyClientTCPReceiver::OnTRecvLoginEnterVillage( char *pPacket )
{
	FUNC_USER_LOG();
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_VILLAGE_LOGIN;
	m_pDummyClient->AddUserLog(_T("빌리지서버에 입장 시도"));

	m_pDummyClient->m_TCPSender.SendVillageReady(true);

}

void
DnDummyClientTCPReceiver::OnTRecvVillageActorMsg( char *pPacket)
{
	FUNC_USER_LOG();

}

void 
DnDummyClientTCPReceiver::OnTRecvVillageMapInfo( char *pPacket)
{
	FUNC_USER_LOG();
	SCMapInfo *pStruct = (SCMapInfo*)pPacket;

	int m_nVillageMapIndex = pStruct->nMapIndex;
	int m_nVillageMapArrayIndex = pStruct->cMapArrayIndex;
	int m_nVillageMapEnviIndex = pStruct->cEnviIndex;
	int m_nVillageMapEnviArrayIndex = pStruct->cEnviArrayIndex;

	m_pDummyClient->m_TCPSender.SendEnter();
}

void 
DnDummyClientTCPReceiver::OnTRecvVillageEntered( char *pPacket )
{
	FUNC_USER_LOG();

	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_COMPLETE_VLOGIN;
	m_pDummyClient->AddUserLog(_T("빌리지서버에 입장 완료"));

	//if ( g_DummyClientMgr.GetMode() == DnDummyClientMgr::VillageMode )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_IDLE;
		m_pDummyClient->AddUserLog(_T("유저아이들"));

	}


	// 파티 리스트 요청을 보낸다.
	//m_pDummyClient->m_TCPSender.SendReqPartyListInfo(m_pDummyClient->m_nReqPartyPage);
	//m_pDummyClient->m_nPartyReqStartIndex = 0;

	SCEnter * pEnter = (SCEnter*)pPacket;
	m_pDummyClient->m_Property.m_vNewPos = pEnter->Position;

	m_pDummyClient->m_TCPSender.SendCompleteLoad();
}

void
DnDummyClientTCPReceiver::OnTRecvVillageUserEntered( char *pPacket)
{
	//FUNC_USER_LOG();
}


void 
DnDummyClientTCPReceiver::OnTRecvPartyList( char *pPacket )
{
	FUNC_USER_LOG();

	/*!!!
	SCPartyList *pData = (SCPartyList *)pPacket;

	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
	m_pDummyClient->AddUserLog(_T("파티 갯수 수신 받음 [현재 파티 : %d 개]"), pData->nPartyVecSize );

	// 파티리스트를 다 지우고
	m_pDummyClient->m_PartyList.clear();
	m_pDummyClient->m_nPartyReqStartIndex = 0;

	// 갯수만큼 빈 파티 구조체를 추가한다 -_-++++++++++++++++++
	for ( int i = 0 ; i < pData->nPartyVecSize ; i++ )
	{
		m_pDummyClient->m_PartyList.push_back(PartyListStruct());
	}

	
	m_pDummyClient->SetNextCmdTime();
	*/
}

void 
DnDummyClientTCPReceiver::OnTRecvPartyListInfoError( char *pData )
{
	FUNC_USER_LOG();
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
	m_pDummyClient->SetNextCmdTime();
}

void 
DnDummyClientTCPReceiver::OnTRecvPartyListInfo( char *packet )
{
	FUNC_USER_LOG();

	SCPartyListInfo *pPacket = (SCPartyListInfo*)packet;
	m_pDummyClient->m_mapPartyListInfo.clear();

	if( pPacket->cCount > PARTYLISTOFFSET )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
		m_pDummyClient->AddUserLog(_T("pPacket->cCount >= PARTYLISTOFFSET"));
		m_pDummyClient->Disconnect();
		return;
	}

	for( int i=0; i<pPacket->cCount; i++ )
	{
		PartyListStruct Struct;
		Struct.nCurCount = pPacket->PartyListInfo[i].cCurrentCount;
		Struct.nMaxCount = pPacket->PartyListInfo[i].cMaxCount;
		Struct.nPartyIndex = pPacket->PartyListInfo[i].nPartyID;
		Struct.nActLvLimit = 0;
		Struct.nUserLvLimit = pPacket->PartyListInfo[i].cUserLvLimitMax;
		Struct.bSecret = pPacket->PartyListInfo[i].cIsPrivateRoom == 1 ? true : false;
		Struct.bEmpty = false;
		Struct.szPartyName.assign( pPacket->PartyListInfo[i].wszBuf, pPacket->PartyListInfo[i].cPartyNameLen );

		int nPartyIndex = pPacket->PartyListInfo[i].nPartyID;
		m_pDummyClient->m_mapPartyListInfo.insert( std::make_pair( nPartyIndex, Struct ));
		
	}

	m_pDummyClient->m_nPartyListCount = pPacket->nWholeCount;

#ifdef _TEST_CODE_KAL
#else
	int nPartyIdx = m_pDummyClient->GetPossibleJoinPartyIndex();
	if ( nPartyIdx > -1 )
	{
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_JOIN_REQ_PARTY;
		m_pDummyClient->AddUserLog(_T("파티 참가 신청 : %d "), nPartyIdx );
		m_pDummyClient->m_TCPSender.SendJoinParty(nPartyIdx);
		return;
	}
#endif

	//참가할 파티가 없으면 목록을 다시 요청 하거나 좀더 고민해본다.
	if ( rand () % 2 )
	{
		m_pDummyClient->AddUserLog(_T("파티 생성 요청"));
		m_pDummyClient->m_TCPSender.SendCreateParty();
		m_pDummyClient->SetNextCmdTime();
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_CREATE_REQ_PARTY;
		return;
	}
	else
	{
		m_pDummyClient->AddUserLog(_T("파티 목록 다시 요청후 대기"));
		// 아니면 파티 갯수를 다시 요청하고 기다린다.
		if (pPacket->nWholeCount > PARTYLISTOFFSET && m_pDummyClient->m_nReqPartyPage == 0)
			m_pDummyClient->m_nReqPartyPage = m_pDummyClient->m_nReqPartyPage+1;
		else
			m_pDummyClient->m_nReqPartyPage = 0;
		m_pDummyClient->m_TCPSender.SendReqPartyListInfo(m_pDummyClient->m_nReqPartyPage);
		//m_pDummyClient->m_nPartyReqStartIndex = 0;
		m_pDummyClient->SetNextCmdTime();
		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_PARTYLIST_REQ;

	}
}

void
DnDummyClientTCPReceiver::OnTRecvPartyCreateParty( char *pPacket )
{
	FUNC_USER_LOG();

	// m_pDummyClient->m_PartyList.clear();
	
	SCCreateParty *pData = (SCCreateParty*)pPacket;

	if( pData->nRetCode != 0 )
	{

		m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
		m_pDummyClient->AddUserLog(_T("파티 생성 실패 : %d "), pData->nPartyID );
		m_pDummyClient->SetNextCmdTime();

		//참가할 파티가 없으면 목록을 다시 요청 하거나 좀더 고민해본다.
		if ( rand () % 10 == 2 )
		{
			m_pDummyClient->AddUserLog(_T("파티 목록 다시 요청후 대기"));
			// 아니면 파티 갯수를 다시 요청하고 기다린다.
			//m_pDummyClient->m_nReqPartyPage = m_pDummyClient->m_nReqPartyPage+1;
			m_pDummyClient->m_TCPSender.SendReqPartyListInfo(m_pDummyClient->m_nReqPartyPage);
			//m_pDummyClient->m_nPartyReqStartIndex = 0;
			m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_PARTYLIST_REQ;
		}
		else
		{
			m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
		}

		return;
	}

	m_pDummyClient->m_nPartyIndex = pData->nPartyID;
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_COMPLETE_CREATE_PARTY;
	m_pDummyClient->AddUserLog(_T("파티 생성 완료 : %d "), pData->nPartyID );
	m_pDummyClient->m_bPartyLeader = true;

	m_pDummyClient->SetNextCmdTime();

	// 조건이 충족 되면 바로 시작하도록
	m_pDummyClient->_UpdateCompleteCreateParty();
	

}

void 
DnDummyClientTCPReceiver::OnTRecvPartyJoinParty( char *pPacket )
{
	FUNC_USER_LOG();
	SCJoinParty *pData = (SCJoinParty*)pPacket;
	if( pData->nRetCode != 0 ) 
	{
		m_pDummyClient->AddUserLog(_T("파티 참가 실패 : %d "), pData->nPartyID );
		m_pDummyClient->SetNextCmdTime();

		//참가할 파티가 없으면 목록을 다시 요청 하거나 좀더 고민해본다.
		if ( rand () % 10 == 2 )
		{
			m_pDummyClient->AddUserLog(_T("파티 목록 다시 요청후 대기"));
			// 아니면 파티 갯수를 다시 요청하고 기다린다.
			//m_pDummyClient->m_nReqPartyPage = m_pDummyClient->m_nReqPartyPage+1;
			m_pDummyClient->m_TCPSender.SendReqPartyListInfo(m_pDummyClient->m_nReqPartyPage);
			//m_pDummyClient->m_nPartyReqStartIndex = 0;
			m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_PARTYLIST_REQ;
		}
		else
		{
			m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_THINKING_PARTY;
		}

		return;
	}

	m_pDummyClient->m_nPartyIndex = pData->nPartyID;
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_COMPLETE_PARTY_JOIN;
	m_pDummyClient->AddUserLog(_T("파티 참가 완료 : %d "), pData->nPartyID );
}


void
DnDummyClientTCPReceiver::OnTRecvPartyRefresh( char *pPacket )
{
	FUNC_USER_LOG();

	SCRefreshParty *pData = (SCRefreshParty*)pPacket;

	CopyMemory( &(m_pDummyClient->m_PartyMember[0]), &(pData->Member[0]), sizeof(SPartyMemberInfo)*PARTYMAX);

}



void 
DnDummyClientTCPReceiver::OnTRecvPartyRefreshGateInfo( char *pPacket )
{
	FUNC_USER_LOG();

}

void
DnDummyClientTCPReceiver::OnTRecvPartyReadyGate( char *pPacket )
{

	FUNC_USER_LOG();
}

void
DnDummyClientTCPReceiver::OnTRecvPartyStageStart( char *pPacket )
{

	FUNC_USER_LOG();

	SCStartStage *pData = (SCStartStage *)pPacket;

	m_pDummyClient->m_UDPSender.SendGameSyncWait(m_pDummyClient->m_nUniqueID);
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_START_STAGE;
}

void
DnDummyClientTCPReceiver::OnTRecvPartyStageCancel( char *pPacket )
{
	SCCancelStage *pData = (SCCancelStage *)pPacket;
	

	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_ERROR;
	m_pDummyClient->Disconnect();
	FUNC_USER_LOG();
}

void 
DnDummyClientTCPReceiver::OnRecvPartyEquipData(char * pData )
{
	FUNC_USER_LOG();
	


}



//----------------------------------------------------------------------------------------

void 
DnDummyClientUDPReceiver::OnPacket(int header, int sub_header, char * data, int size, _ADDR * addr)
{
	int a = 0;
	/*
	switch( header )
	{
	case DNPACKET_SERVER_SYNC_WAIT_SERVER: OnURecvPartySyncWait( data );break;
	case DNPACKET_SERVER_SYNC_START_SERVER: OnURecvPartySyncStart( data ); break;
	case DNPACKET_SERVER_ACTOR_MSG_SERVER : OnURecvGameActorMsg(data); break;
	}
	*/
}

void 
DnDummyClientTCPReceiver::OnURecvPartySyncWait( char *pPacket )
{
	FUNC_USER_LOG();
/*
	DNPACKET_DATA_SYNC_WAIT_SERVER * packet = (DNPACKET_DATA_SYNC_WAIT_SERVER * ) pPacket;

	m_pDummyClient->AddUserLog(_T("동기화대기중"));
	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_WAIT_GAME_START;
*/
}

void 
DnDummyClientTCPReceiver::OnURecvPartySyncStart( char *pPacket )
{
	FUNC_USER_LOG();
	/*
	m_pDummyClient->AddUserLog(_T("게임 시작!!! "));

	m_pDummyClient->GetProperty().nPlayState = PlayStateInfo::PS_GAME_PLAY;

	m_pDummyClient->GetProperty().m_vPos = m_pDummyClient->GetNewPos();
	*/
}

struct SHitParam
{
	std::string szActionName;
	//DnWeaponHandle hWeapon;
	int nDamage;
	int HitType;
	EtVector3 vPosition;
	EtVector3 vVelocity;
	EtVector3 vResistance;
	EtVector3 vViewVec;
	float fStiffDelta;
	float fDownDelta;
	int nBoneIndex;
	bool bFirstHit;

	SHitParam()
		: vVelocity( 0.f, 0.f, 0.f )
		, vResistance( 0.f, 0.f, 0.f )
		, vPosition( 0.f, 0.f, 0.f )
		, vViewVec( 0.f, 0.f, 0.f )
	{
		nDamage = 0;
		HitType = 0;
		fStiffDelta = 0.f;
		fDownDelta = 0.f;
		nBoneIndex = -1;
		bFirstHit = false;
	}
};

void 
DnDummyClientTCPReceiver::OnURecvGameActorMsg( char *pPacket )
{
	/*
	DNPACKET_DATA_ACTOR_MSG_SERVER * packet = (DNPACKET_DATA_ACTOR_MSG_SERVER *)pPacket;
	DWORD dwProtocol = packet->act;
	if ( packet->uid == m_pDummyClient->m_nUniqueID )
	{

	}

	switch( dwProtocol )
	{
	case APT_CMDMOVE:
		{
			CPacketCompressStream Stream( packet->buf, 128 );

			int nActionIndex, nLoopCount;
			EtVector3 vPos, vXVec, vTargetPos;
			EtVector2 vZVec, vLook;

			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	


		}
		break;

	case APT_CMDACTION:
		{
			CPacketCompressStream Stream( packet->buf, 128 );

			int nActionIndex, nLoopCount;
			float fBlendFrame;
			EtVector3 vPos;
			EtVector2 vLook;
			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );


		}
		break;
	case APT_CMDMOVETARGET:
		{
			CPacketCompressStream Stream( packet->buf, 128 );

			int nActionIndex, nLoopCount;
			EtVector3 vXVec, vPos;
			EtVector2 vZVec, vLook;
			DWORD dwUniqueID;
			float fMinDistance;

			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &dwUniqueID, sizeof(DWORD) );
			Stream.Read( &fMinDistance, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
			Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

		}
		break;
	case APT_ONDAMAGE:
		{
			CPacketCompressStream Stream( packet->buf, 128 );

			int nSeed;
			DWORD dwUniqueID, dwWeaponUniqueID = -1;
			int nActionIndex;
			bool bSendWeaponSerialID;
			INT64 nWeaponSerialID = -1;
			SHitParam HitParam;
			EtVector3 vPos;

			Stream.Read( &nSeed, sizeof(int) );
			Stream.Read( &dwUniqueID, sizeof(DWORD) );
			Stream.Read( &bSendWeaponSerialID, sizeof(bool) );
			if( bSendWeaponSerialID ) Stream.Read( &nWeaponSerialID, sizeof(INT64) );
			else Stream.Read( &dwWeaponUniqueID, sizeof(DWORD) );
			
			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &HitParam.nDamage, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
			Stream.Read( &HitParam.HitType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &HitParam.vVelocity, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT, 10.f );
			Stream.Read( &HitParam.vResistance, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT, 10.f );
			Stream.Read( &HitParam.vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &HitParam.vViewVec, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT, 10.f );
			Stream.Read( &HitParam.bFirstHit, sizeof(bool) );
			Stream.Read( &HitParam.nBoneIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
			
			m_pDummyClient->AddUserLog(_T("m_pDummyClient->m_nUniqueID : %d , dwUniqueID : %d packet->uid : %d"), m_pDummyClient->m_nUniqueID , dwUniqueID, packet->uid );


			// 내가 때렸다.
			if ( m_pDummyClient->m_nUniqueID == dwUniqueID )
			{
				m_pDummyClient->GetProperty().nHitCnt++;
			}

			// 내가 맞았다.
			if ( m_pDummyClient->m_nUniqueID == packet->uid )
			{
				m_pDummyClient->GetProperty().nDamageCnt++;
			}
		}
	}
	*/

}

void DnDummyClientTCPReceiver::OnTRecvActorBundleMessage(int iMainCmd, int iSubCmd, char *pData, int nSize)
{
	SCActorBundleMessage *pPacket = (SCActorBundleMessage *)pData;

	BYTE cSubCmd;
	WORD nCurSize;
	int nOffset = 0;
	SCActorMessage Packet;
	ZeroMemory(&Packet, sizeof(SCActorMessage));
	for( int i=0; i<iSubCmd; i++ ) {
		memcpy( &Packet.nSessionID, pPacket->cBuf + nOffset, sizeof(DWORD) ); nOffset += sizeof(DWORD);
		memcpy( &cSubCmd, pPacket->cBuf + nOffset, sizeof(BYTE) ); nOffset += sizeof(BYTE);
		memcpy( &nCurSize, pPacket->cBuf + nOffset, sizeof(WORD) ); nOffset += sizeof(WORD);
		memcpy( Packet.cBuf, pPacket->cBuf + nOffset, nCurSize ); nOffset += nCurSize;
		OnTRecvActorMsg( iMainCmd, cSubCmd, (char*)&Packet, nCurSize );
	}
}

void 
DnDummyClientTCPReceiver::OnTRecvActorMsg(int iMainCmd, int iSubCmd, char *pData, int nSize)
{
	SCActorMessage *pPacket = (SCActorMessage *)pData;
	
	
	switch(iSubCmd)
	{
	case eActor::SC_CMDMOVE: 
		{
			CPacketCompressStream Stream( pPacket->cBuf, 128 );

			int nActionIndex, nLoopCount;
			EtVector3 vPos, vXVec, vTargetPos;
			EtVector2 vZVec, vLook;

			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

			m_pDummyClient->Answer(pPacket->nSessionID, SC_ACTOR, eActor::SC_CMDMOVE, timeGetTime());
		}
		break;
	case eActor::SC_CMDSTOP: break;
	case eActor::SC_CMDACTION:
		{
			CPacketCompressStream Stream( pPacket->cBuf, 128 );

			int nActionIndex, nLoopCount;
			float fBlendFrame;
			EtVector3 vPos;
			EtVector2 vLook;
			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
		}break;
	case eActor::SC_CMDMIXEDACTION: break;
	case eActor::SC_CMDMOVETARGET:
		{
			CPacketCompressStream Stream( pPacket->cBuf, 128 );

			int nActionIndex, nLoopCount;
			EtVector3 vXVec, vPos;
			EtVector2 vZVec, vLook;
			DWORD dwUniqueID;
			float fMinDistance;

			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &dwUniqueID, sizeof(DWORD) );
			Stream.Read( &fMinDistance, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1.f );
			Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			
		}break;
	case eActor::SC_CMDLOOKTARGET: break;
	case eActor::SC_ONDAMAGE:
		{
			CPacketCompressStream Stream( pPacket->cBuf, 128 );

			int nSeed;
			DWORD dwUniqueID, dwWeaponUniqueID = -1;
			int nActionIndex;
			bool bSendWeaponSerialID;
			INT64 nWeaponSerialID = -1;
			SHitParam HitParam;
			EtVector3 vPos;

			Stream.Read( &nSeed, sizeof(int) );
			Stream.Read( &dwUniqueID, sizeof(DWORD) );
			Stream.Read( &bSendWeaponSerialID, sizeof(bool) );
			if( bSendWeaponSerialID ) Stream.Read( &nWeaponSerialID, sizeof(INT64) );
			else Stream.Read( &dwWeaponUniqueID, sizeof(DWORD) );

			Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &HitParam.nDamage, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
			Stream.Read( &HitParam.HitType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &HitParam.vVelocity, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT, 10.f );
			Stream.Read( &HitParam.vResistance, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT, 10.f );
			Stream.Read( &HitParam.vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Read( &HitParam.vViewVec, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT, 10.f );
			Stream.Read( &HitParam.bFirstHit, sizeof(bool) );
			Stream.Read( &HitParam.nBoneIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

			m_pDummyClient->AddUserLog(_T("m_pDummyClient->m_nUniqueID : %d , dwUniqueID : %d packet->uid : %d"), m_pDummyClient->m_nUniqueID , dwUniqueID, pPacket->nSessionID );


			// 내가 때렸다.
			if ( m_pDummyClient->m_nUniqueID == dwUniqueID )
			{
				m_pDummyClient->GetProperty().nHitCnt++;
			}

			// 내가 맞았다.
			if ( m_pDummyClient->m_nUniqueID == pPacket->nSessionID )
			{
				m_pDummyClient->GetProperty().nDamageCnt++;
			}
		}break;
	case eActor::SC_PROJECTILE: break;
	case eActor::SC_CMDLOOK: break;
	case eActor::SC_PLAYERPITCH: break;
	case eActor::SC_CMDENTEREDGATE: break;
	case eActor::SC_USESKILL: break;
	case eActor::SC_AGGROTARGET: break;
	case eActor::SC_VIEWSYNC: break;
	case eActor::SC_CMDTOGGLEBATTLE: break;
	case eActor::SC_CMDPASSIVESKILLACTION: break;
	case eActor::SC_CMDADDSTATEEFFECT: break;
	case eActor::SC_CMDREMOVESTATEEFFECT: break;
	case eActor::SC_ADDEXP: break;
	case eActor::SC_LEVELUP: break;
	case eActor::SC_ADDCOIN: break;
	//case eActor::SC_CHAIN: break;
	case eActor::SC_PARTYCHAIN: break;
//	case eActor::SC_REBIRTH: break;				
//	case eActor::SC_REBIRTH_COIN: break;			
	}
}
