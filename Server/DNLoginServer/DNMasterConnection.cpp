#include "StdAfx.h"
#include "DNMasterConnection.h"
#include "DNMasterConnectionManager.h"
#include "DNUserConnection.h"
#include "DNUserConnectionManager.h"
#include "DNIocpManager.h"
#include "DNExtManager.h"
#include "Log.h"
#include "Util.h"
#include "DNAuthManager.h"
#include "DNLogConnection.h"
#include "DNSQLMembershipManager.h"
#include "DNSQLWorldManager.h"

extern TLoginConfig g_Config;

CDNMasterConnection::CDNMasterConnection(void): CConnection()
{
#if defined(_FINAL_BUILD)
	Init(1024 * 1024, 1024 * 1024);
#else	// #if defined(_FINAL_BUILD)
	Init(1024 * 100, 1024 * 100);
#endif	// #if defined(_FINAL_BUILD)
	m_bStarted = false;
	m_nWorldMaxUser = 0;
	m_nWorldCurUser = 0;
	::memset(m_dwTick, 0, sizeof(m_dwTick));
}

CDNMasterConnection::~CDNMasterConnection(void)
{
}

bool CDNMasterConnection::AddWaitUser(UINT nAccountDBID, USHORT nWaitTicketNum, USHORT &nEstimateTime)
{
	ScopeLock <CSyncLock> sync(m_Lock);

	std::list <_WAITUSER>::iterator ii;
	for (ii = m_WaitUserList.begin(); ii != m_WaitUserList.end(); ii++)
		if ((*ii).nAccountDBID == nAccountDBID)
			return false;

	_WAITUSER wait;
	memset(&wait, 0, sizeof(_WAITUSER));

	wait.nAccountDBID = nAccountDBID;
	wait.nWaitTicketNum = nWaitTicketNum;
	wait.nAddTime = timeGetTime();

	m_WaitUserList.push_back(wait);
	return true;
}

bool CDNMasterConnection::DelWaitUser(UINT nAccountDBID)
{
	ScopeLock <CSyncLock> sync(m_Lock);

	USHORT nDelTicketNum = 0;
	std::list <_WAITUSER>::iterator ii;
	for (ii = m_WaitUserList.begin(); ii != m_WaitUserList.end(); ii++)
	{
		if ((*ii).nAccountDBID == nAccountDBID)
		{
			nDelTicketNum = (*ii).nWaitTicketNum;
			m_WaitUserList.erase(ii);
			break;
		}
	}

	if (nDelTicketNum > 0)
	{
		//update ticketnum
		for (ii = m_WaitUserList.begin(); ii != m_WaitUserList.end(); ii++)
		{
			if ((*ii).nWaitTicketNum > nDelTicketNum)
				(*ii).nWaitTicketNum--;
		}
	}
	else
		return false;
	return true;
}

void CDNMasterConnection::UpdateWaitTicketNum()
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif
	
	{
		ScopeLock <CSyncLock> sync(m_Lock);

		std::list <_WAITUSER>::iterator ii;

		for (ii = m_WaitUserList.begin(); ii != m_WaitUserList.end(); ii++)
		{
			CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnectionByAccountDBID((*ii).nAccountDBID));
			if( !pUserCon ) continue;
			pUserCon->SendWaitUser(pUserCon->GetWorldID(), (*ii).nWaitTicketNum, 0);
		}
	}
}

int CDNMasterConnection::MessageProcess(int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	switch(nMainCmd)
	{
	case MALO_REGIST:
		{
			MALORegist *pRegist = (MALORegist*)pData;

			m_nSessionID = pRegist->cWorldSetID;
			DN_ASSERT(0 != m_nSessionID,	"Invalid!");	// 월드 ID 가 0 이 되는 경우는 없다고 가정
			m_nWorldMaxUser = pRegist->nWorldMaxUser;
			m_bStarted = true;

			g_Log.Log(LogType::_FILEDBLOG, m_nSessionID, 0, 0, 0, L"PRE-[MALO_REGIST] %x WorldID:%d\r\n", this, m_nSessionID);

			if (g_pMasterConnectionManager->PushConnection(this) == ERROR_GENERIC_DUPLICATESESSIONID){
				g_Log.Log(LogType::_FILEDBLOG, m_nSessionID, 0, 0, 0, L"ERR-[MALO_REGIST] WORLDID를 확인해주세요!!! (WorldID:%d)가 맞나요?\r\n", m_nSessionID);
				DetachConnection(L"Duplicate Server");
				return ERROR_NONE;
			}

			g_pExtManager->SetOnOffServerInfo(pRegist->cWorldSetID, true);
			g_Log.Log(LogType::_FILEDBLOG, m_nSessionID, 0, 0, 0, L"OK-[MALO_REGIST] %x WorldID:%d\r\n", this, m_nSessionID);
			return ERROR_NONE;
		}
		break;	

	case MALO_VILLAGEINFO:
		{
			MALOVillageInfo * pPacket = (MALOVillageInfo*)pData;

			{
#ifdef _USE_ACCEPTEX
				ScopeLock<CSyncLock> lock(m_Lock);
#endif
				bool bFlag = false;
				for (int i = 0; i < pPacket->cCount; i++)
				{
					std::vector < std::pair <int, sChannelInfo> >::iterator ii;
					for (ii = m_vChannelList.begin(); ii != m_vChannelList.end(); ii++)
					{
						if ((*ii).second.nChannelID == pPacket->Info[i].nChannelID)
						{
							memcpy(&(*ii).second, &pPacket->Info[i], sizeof(sChannelInfo));
							bFlag = true;
						}
					}
				}

				if (m_vChannelList.size() <= 0 || bFlag == false)
				{
					for (int i = 0; i < pPacket->cCount; i++)
						m_vChannelList.push_back(std::make_pair(pPacket->nVillageID, pPacket->Info[i]));
				}
			}

			UpdateWaitTicketNum();
			m_nWorldCurUser = pPacket->nWorldUserCount;
			m_nWorldMaxUser = pPacket->nWorldMaxUserCount;
			return ERROR_NONE;
		}
		break;

	case MALO_UPDATECHANNELSHOWINFO:
	{
#ifdef _USE_ACCEPTEX
		ScopeLock<CSyncLock> lock(m_Lock);
#endif
		
		MALOUpdateChannelShowInfo* pPacket = reinterpret_cast<MALOUpdateChannelShowInfo*>(pData);

		for( UINT i=0 ; i<m_vChannelList.size() ; ++i )
		{
			if( m_vChannelList[i].second.nChannelID == pPacket->unChannelID )
			{
				m_vChannelList[i].second.bShow = pPacket->bShow;
				break;
			}
		}

		return ERROR_NONE;
	}

	case MALO_VILLAGEINFODEL:
		{
			MALOVillageInfoDelete * pPacket = (MALOVillageInfoDelete*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> lock(m_Lock);
#endif
			std::vector < std::pair <int, sChannelInfo> >::iterator ii;
			for (ii = m_vChannelList.begin(); ii != m_vChannelList.end();)
			{
				if ((*ii).first == pPacket->nVillageID)
					ii = m_vChannelList.erase(ii);
				else
					ii++;
			}
		}
		break;

#if defined(PRE_MOD_SELECT_CHAR)
	case MALO_ADDUSER:
		{
			MALOAddUser *pPacket = (MALOAddUser*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif

			CDNUserConnection *pUserCon = g_pUserConnectionManager->GetConnectionByAccountDBID(pPacket->nAccountDBID);
			if( !pUserCon ) 
				return ERROR_GENERIC_UNKNOWNERROR;

			switch (pPacket->nRet)
			{
			case ERROR_NONE:
				{
					// 캐릭명 길이 체크
					if( pUserCon->GetAccountLevel() == 0 && !pUserCon->CheckAbuserCharLength() )					
						pUserCon->QueryAddRestraint(L"CheckAbuserCharLength");

					pUserCon->m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();

					int nResult = g_pAuthManager->QueryStoreAuth(SERVERTYPE_LOGIN, pUserCon);
					if (ERROR_NONE != nResult){
						pUserCon->DetachConnection(L"QUERY_STOREAUTH_LO error");		// 기존유저를 끊을것이냐.. 지금 유저를 끊을 것이냐... (일단은 지금 유저를 끊는다)
						break;
					}

					pUserCon->CheckVillageGameInfo();
				}
				break;

			case ERROR_GENERIC_WORLDFULLY_CAPACITY:
				{
					pUserCon->m_cUserState = STATE_WAITUSER;
					pUserCon->SetWaitWorldIDBySelectCharacter();	//대기월드 번호를 세팅합니다.

					SendAddWaitUser(g_pAuthManager->GetServerID(), pUserCon->GetAccountDBID());
				}
				break;

			case ERROR_GENERIC_GAMECON_NOT_FOUND:
				{
					pUserCon->m_cUserState = STATE_CHARLIST;
					pUserCon->SendGameInfo(0, 0, 0, pPacket->nRet, 0, 0);
				}
				break;

			default:
				{
					g_Log.Log(LogType::_ERROR, pUserCon, L"[MALO_ADDUSER] ADBID:%u, SID:%u, Ret:%d\r\n", pPacket->nAccountDBID, pPacket->nSessionID, pPacket->nRet);
					pUserCon->DetachConnection(L"MALO_ADDUSER Err");		//이게 나오면 내부문제.
					return ERROR_NONE;
				}
				break;
			}
			return ERROR_NONE;
		}
		break;

#else	// #if defined(PRE_MOD_SELECT_CHAR)

	case MALO_ADDUSER:
		{
			MALOAddUser * pPacket = (MALOAddUser*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif

			CDNUserConnection * pUserCon = g_pUserConnectionManager->GetConnectionByAccountDBID(pPacket->nAccountDBID);
			if (pUserCon)
			{
				if (pPacket->nRet == ERROR_NONE)
				{
					//정상진입
					pUserCon->SetWorldID(pPacket->cWorldID);
					pUserCon->SetWorldSetID(GetWorldSetID());

					pUserCon->SendCharacterList(false);

					pUserCon->m_bMasterAddUser = true;

					// 캐릭명 길이 체크
					if( pUserCon->GetAccountLevel() == 0 && !pUserCon->CheckAbuserCharLength() )					
						pUserCon->QueryAddRestraint(L"CheckAbuserCharLength");
				}
				else if (pPacket->nRet == ERROR_GENERIC_WORLDFULLY_CAPACITY)
				{
					pUserCon->m_cUserState = STATE_WAITUSER;
					pUserCon->SetWaitWorldID(pPacket->cWorldID);	//대기월드 번호를 세팅합니다.
					pUserCon->SetWaitWorldSetID(GetWorldSetID());	//대기월드 번호를 세팅합니다.

					SendAddWaitUser(g_pAuthManager->GetServerID(), pUserCon->GetAccountDBID());
				}
				else
				{
					_DANGER_POINT();
					pUserCon->DetachConnection(L"MALO_ADDUSER Err");		//이게 나오면 내부문제.
					return ERROR_NONE;
				}
			}
			return ERROR_NONE;
		}

	case MALO_USERINFO:
		{
			MALOUserInfo *pUserInfo = (MALOUserInfo*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif
			CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(pUserInfo->nSessionID));
			if( !pUserCon ) 
				return ERROR_GENERIC_UNKNOWNERROR;

			switch (pUserInfo->nRet){
			case ERROR_NONE:
				{
					pUserCon->m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();

					int nResult = g_pAuthManager->QueryStoreAuth(SERVERTYPE_LOGIN, pUserCon);
					if (ERROR_NONE != nResult){
						pUserCon->DetachConnection(L"QUERY_STOREAUTH_LO error");		// 기존유저를 끊을것이냐.. 지금 유저를 끊을 것이냐... (일단은 지금 유저를 끊는다)
						break;
					}

					pUserCon->CheckVillageGameInfo();
				}
				break;

			case ERROR_GENERIC_GAMECON_NOT_FOUND:
				pUserCon->m_cUserState = STATE_CHARLIST;
				pUserCon->SendGameInfo(0, 0, 0, pUserInfo->nRet, 0, 0);
				break;

			default:
				g_Log.Log(LogType::_ERROR, m_nSessionID, pUserInfo->nAccountDBID, 0, pUserInfo->nSessionID, L"[MALO_USERINFO] ADBID:%u, SID:%u, Ret:%d\r\n", pUserInfo->nAccountDBID, pUserInfo->nSessionID, pUserInfo->nRet);

				pUserCon->DetachConnection(L"UserInfo Error");				
			}

			return ERROR_NONE;
		}
		break;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	case MALO_DETACHUSER:
		{
			MALODetachUser * pPacket = (MALODetachUser*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif
			CDNUserConnection * pCon = g_pUserConnectionManager->GetConnectionByAccountDBID(pPacket->nAccountDBID);
			if (pCon)
				pCon->DetachConnection(L"MALODetachUser");

			return ERROR_NONE;
		}
		break;

	case MALO_SETTUTORIALGAMEID:
		{
			MALOSetTutorialGameID *pGameID = (MALOSetTutorialGameID*)pData;
			
#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif
			CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnection(pGameID->nSessionID));
			if( !pUserCon ) 
				return ERROR_GENERIC_UNKNOWNERROR;

			if (pGameID->nRet == ERROR_NONE){
				pUserCon->SendTutorialMapInfo(pUserCon->m_nTutorialMapIndex, pUserCon->m_cTutorialGateNo);
				pUserCon->m_cUserState = STATE_CONNECTGAME;
			}
			else{
				pUserCon->m_cUserState = STATE_CHARLIST;
				g_Log.Log(LogType::_ERROR, pUserCon, L"[MALO_SETTUTORIALGAMEID] ADBID:%u, CDBID:%lld, SID:%u, Ret:%d\r\n", pUserCon->GetAccountDBID(), pUserCon->GetSelectCharacterDBID(), pUserCon->GetSessionID(), pGameID->nRet);
			}

			pUserCon->SendGameInfo(pGameID->nIP, pGameID->nPort, pGameID->nTcpPort, pGameID->nRet, pUserCon->GetAccountDBID(), pUserCon->GetCertifyingKey());
			return ERROR_NONE;
		}
		break;

	case MALO_ADDWAITUSER:
		{
			MALOAddWaitUser * pPacket = (MALOAddWaitUser*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif
			CDNUserConnection *pUserCon = static_cast<CDNUserConnection*>(g_pUserConnectionManager->GetConnectionByAccountDBID(pPacket->nAccountDBID));
			if( !pUserCon ) return ERROR_GENERIC_UNKNOWNERROR;

			if (pPacket->nRetCode == ERROR_NONE)
			{
				USHORT nEstimateTime = 0;
				bool bRet = AddWaitUser(pUserCon->GetAccountDBID(), pPacket->nTicketNum, nEstimateTime);
				if (bRet)
				{
					pUserCon->SendWaitUser(pUserCon->GetWaitWorldID(), pPacket->nTicketNum, nEstimateTime);
					return ERROR_NONE;
				}
			}
			
			_DANGER_POINT();
			pUserCon->DetachConnection(L"MALO_ADDWAITUSER Err");
			return ERROR_NONE;
		}
		break;

	case MALO_WAITUSERPROCESS:
		{
			MALOWaitUserProcess * pPacket = (MALOWaitUserProcess*)pData;

#ifdef _USE_ACCEPTEX
			ScopeLock<CSyncLock> Lock( g_pUserConnectionManager->m_ConSync );
#endif

			for (int i = 0; i < pPacket->nCount; i++)
			{
				CDNUserConnection * pUserCon = (CDNUserConnection*)g_pUserConnectionManager->GetConnectionByAccountDBID(pPacket->nAccountArr[i]);
				if (pUserCon)
				{
					// MasterConnectionManager Lock 용 Scope
					{
#ifdef _USE_ACCEPTEX
						ScopeLock<CSyncLock> Lock( g_pMasterConnectionManager->m_ConSync );
#endif // #ifdef _USE_ACCEPTEX
						CDNMasterConnection *pMasterCon = static_cast<CDNMasterConnection*>(g_pMasterConnectionManager->GetConnection(pUserCon->GetWaitWorldSetID(), false));
						if( pMasterCon )
#if defined(PRE_MOD_SELECT_CHAR)
							pMasterCon->SendAddUser( pUserCon, true );
#else	// #if defined(PRE_MOD_SELECT_CHAR)
							pMasterCon->SendAddUser( pUserCon, pUserCon->GetWaitWorldID(), true );
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
						else
						{
							pUserCon->DetachConnection( L"WaitUser|MasterConNotFound" );
							continue;
						}
					}
				}

				if (DelWaitUser(pPacket->nAccountArr[i]) == false)
					_DANGER_POINT();
			}
			return ERROR_NONE;
		}
		break;

	case MALO_DUPLICATE_LOGIN :
		{
			MALODuplicateLogin* packet = (MALODuplicateLogin*)pData;
			// 요건 현재 서버에 존재하지 않는 다는 거임..ResetAuth 쳐불자.
			g_pAuthManager->QueryResetAuth(0, packet->nAccountDBID, packet->nSessionID);
			g_Log.Log(LogType::_ERROR, m_nSessionID, packet->nAccountDBID, 0, packet->nSessionID, L"[MALO_DUPLICATE_LOGIN] QueryResetAuth !!\r\n");
			return ERROR_NONE;
		}
		break;
	}

	return ERROR_UNKNOWN_HEADER;
}

bool CDNMasterConnection::_bCheckMapIndexFromAttr( const int iMapIndex, const UINT uiAttr )
{
	for( UINT i=0 ; i<m_vChannelList.size() ; ++i )
	{
		if( m_vChannelList[i].second.nMapIdx == iMapIndex )
		{
			if( m_vChannelList[i].second.nChannelAttribute&uiAttr )
				return true;
		}
	}

	return false;
}

bool CDNMasterConnection::bIsUseLastMapIndex( const int iMapIndex )
{
	return bIsPvPMapIndex( iMapIndex ) || bIsGMMapIndex( iMapIndex ) || bIsDarkLairMapIndex( iMapIndex ) || bIsFarmMapIndex(iMapIndex);
}

bool CDNMasterConnection::bIsPvPMapIndex( const int iMapIndex )
{
	return _bCheckMapIndexFromAttr( iMapIndex, (GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_PVPLOBBY) );
}

bool CDNMasterConnection::bIsGMMapIndex( const int iMapIndex )
{
	return _bCheckMapIndexFromAttr( iMapIndex, (GlobalEnum::CHANNEL_ATT_GM) );
}

bool CDNMasterConnection::bIsDarkLairMapIndex( const int iMapIndex )
{
	return _bCheckMapIndexFromAttr( iMapIndex, (GlobalEnum::CHANNEL_ATT_DARKLAIR) );
}

bool CDNMasterConnection::bIsFarmMapIndex(int nMapIndex)
{
	return _bCheckMapIndexFromAttr( nMapIndex, (GlobalEnum::CHANNEL_ATT_FARMTOWN) );
}

void CDNMasterConnection::GetChannelListByMapIdx(TDBListCharData* pSelectCharData, sChannelInfo *ChannelArray, BYTE &cCount, TMapChannel *ChannelList)
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> lock(m_Lock);
#endif

	int nMapID = pSelectCharData->nMapID;
	int nLastVillageMapID = pSelectCharData->nLastVillageMapID;
	int nCheckLevel = pSelectCharData->cLevel;

	int nSearchMapIndex = nMapID;
	if( nMapID == 0 ) 
		nSearchMapIndex = 1;
	else if( g_pExtManager->GetMapType(nMapID) != GlobalEnum::MAP_VILLAGE || bIsUseLastMapIndex( nMapID ) )
		nSearchMapIndex = nLastVillageMapID;

	bool bVillageMap = false;

//-----------------------------------------------------------------
	//[debug]
	if ((int)m_vChannelList.size() == 0)  
	{
		printf("[Err]: m_vChannelList.size() == 0 \n");
	}
	else
	{
		//printf("窟쨌斤口袒긍쯤: 董珂퓻齡鞫刻뒤寧몸窟쨌 \n");
		//m_vChannelList[0].second.bShow = true;  //董珂퓻齡鞫刻뒤寧몸窟쨌
	}
//-----------------------------------------------------------------


	for (int i = 0; i < (int)m_vChannelList.size(); i++)
	{
		if (nSearchMapIndex == m_vChannelList[i].second.nMapIdx && !(m_vChannelList[i].second.nChannelAttribute&GlobalEnum::CHANNEL_ATT_USELASTMAPINDEX))
		{
			bVillageMap = true;
			break;
		}
	}

	if (bVillageMap == false && m_vChannelList.size() > 0)
	{
		g_Log.Log(LogType::_ERROR, L"CDBID[%lld] SearchMapIndex Invalid VillageIndex MapIdx[%d] LastMapIdx[%d]\n", pSelectCharData->biCharacterDBID, nMapID, nLastVillageMapID);
		nSearchMapIndex = 1;
	}

	cCount = 0;
	TChannelInfoEx ChannelInfo;
	ChannelList->clear();

	for (int i = 0; i < (int)m_vChannelList.size(); i++)
	{
		if (m_vChannelList[i].second.bVisibility == false) 
			continue;

		if( m_vChannelList[i].second.bShow == false )
			continue;

		if( m_vChannelList[i].second.nLimitLevel > nCheckLevel )
			continue;

		bool bShowChannel = false;
		// 튜토리얼 상태 예외처리.PvP,다크레어,마을 채널 보여주지 않는다.	//DWC 채널은 보여주지 않는다.
		if( nLastVillageMapID > 0 && m_vChannelList[i].second.nChannelAttribute&(GlobalEnum::CHANNEL_ATT_PVP|GlobalEnum::CHANNEL_ATT_DARKLAIR|GlobalEnum::CHANNEL_ATT_FARMTOWN) )
			bShowChannel = true;

		// PvP로비는 항상 보이지 않는다.
		if( m_vChannelList[i].second.nChannelAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
			continue;

		//농장 마을인경우 디펜던시 체크를한다
		if (m_vChannelList[i].second.nChannelAttribute&(GlobalEnum::CHANNEL_ATT_FARMTOWN) && \
			nSearchMapIndex != m_vChannelList[i].second.nDependentMapID)
			continue;

		if( !bShowChannel && nSearchMapIndex != m_vChannelList[i].second.nMapIdx ) 
			continue;

		memset(&ChannelInfo, 0, sizeof(TChannelInfoEx));

		ChannelInfo.Channel.nAttribute = m_vChannelList[i].second.nChannelAttribute;
		ChannelInfo.Channel.nChannelID = m_vChannelList[i].second.nChannelID;
		ChannelInfo.Channel.nChannelIdx = m_vChannelList[i].second.nChannelIdx;
		ChannelInfo.Channel.nChannelMaxUser = m_vChannelList[i].second.nMaxUserCount;
		ChannelInfo.Channel.nCurrentUser = m_vChannelList[i].second.nCurrentUserCount;
		ChannelInfo.Channel.nMapIdx = m_vChannelList[i].second.nMapIdx;
		MultiByteToWideChar(CP_ACP, 0, g_pExtManager->GetMapName(m_vChannelList[i].second.nMapIdx), -1, ChannelInfo.Channel.wszMapName, 64 );

		ChannelInfo.cVillageID = m_vChannelList[i].first;
		memcpy(ChannelInfo.szIP, m_vChannelList[i].second.szIP, IPLENMAX);
		ChannelInfo.nPort = m_vChannelList[i].second.nPort;
#if defined(PRE_ADD_CHANNELNAME)
		memcpy(ChannelInfo.Channel.wszLanguageName, m_vChannelList[i].second.wszLanguageName, sizeof(ChannelInfo.Channel.wszLanguageName));
#endif //#if defined(PRE_ADD_CHANNELNAME)
		ChannelList->insert(make_pair(ChannelInfo.Channel.nChannelID, ChannelInfo));

		memcpy(&ChannelArray[cCount], &m_vChannelList[i].second, sizeof(sChannelInfo));
		cCount++;		
	}
}

#if defined(PRE_ADD_DWC)
void CDNMasterConnection::GetDWCChannelList(TDBListCharData* pSelectCharData, sChannelInfo *ChannelArray, BYTE &cCount, TMapChannel *ChannelList)
{
#ifdef _USE_ACCEPTEX
	ScopeLock<CSyncLock> lock(m_Lock);
#endif

	int nCheckLevel = pSelectCharData->cLevel;

	cCount = 0;
	TChannelInfoEx ChannelInfo;
	ChannelList->clear();

	for (int i = 0; i < (int)m_vChannelList.size(); i++)
	{
		//DWC 채널만 보여준다.
		if( !(m_vChannelList[i].second.nChannelAttribute&GlobalEnum::CHANNEL_ATT_DWC) )
			continue;

		if (m_vChannelList[i].second.bVisibility == false) 
			continue;

		if( m_vChannelList[i].second.bShow == false )
			continue;

		if( m_vChannelList[i].second.nLimitLevel > nCheckLevel )
			continue;

		memset(&ChannelInfo, 0, sizeof(TChannelInfoEx));

		ChannelInfo.Channel.nAttribute = m_vChannelList[i].second.nChannelAttribute;
		ChannelInfo.Channel.nChannelID = m_vChannelList[i].second.nChannelID;
		ChannelInfo.Channel.nChannelIdx = m_vChannelList[i].second.nChannelIdx;
		ChannelInfo.Channel.nChannelMaxUser = m_vChannelList[i].second.nMaxUserCount;
		ChannelInfo.Channel.nCurrentUser = m_vChannelList[i].second.nCurrentUserCount;
		ChannelInfo.Channel.nMapIdx = m_vChannelList[i].second.nMapIdx;
		MultiByteToWideChar(CP_ACP, 0, g_pExtManager->GetMapName(m_vChannelList[i].second.nMapIdx), -1, ChannelInfo.Channel.wszMapName, 64 );

		ChannelInfo.cVillageID = m_vChannelList[i].first;
		memcpy(ChannelInfo.szIP, m_vChannelList[i].second.szIP, IPLENMAX);
		ChannelInfo.nPort = m_vChannelList[i].second.nPort;
		ChannelList->insert(make_pair(ChannelInfo.Channel.nChannelID, ChannelInfo));

		memcpy(&ChannelArray[cCount], &m_vChannelList[i].second, sizeof(sChannelInfo));
		cCount++;		
	}
}
#endif

DWORD CDNMasterConnection::GetTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	return(m_dwTick[pTick]);
}

void CDNMasterConnection::SetTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	m_dwTick[pTick] = ::GetTickCount();
	m_dwTick[pTick] = (m_dwTick[pTick])?(m_dwTick[pTick]):(1);
}

void CDNMasterConnection::SetTick(eTick pTick, DWORD pTick32) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");
	DN_ASSERT(0 != pTick32,						"Invalid!");

	m_dwTick[pTick] = (pTick32)?(pTick32):(1);
}

void CDNMasterConnection::ResetTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	m_dwTick[pTick] = 0;
}

BOOL CDNMasterConnection::IsTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	return(0 != m_dwTick[pTick]);
}

bool CDNMasterConnection::SendRegist(int nServerID)
{
	DN_ASSERT(0 != nServerID,	"Invalid!");

	LOMARegist Regist;
	Regist.nServerID = nServerID;

	int nRet = AddSendData(LOMA_REGIST, 0, (char*)&Regist, sizeof(Regist));
	if (nRet < 0)
		g_Log.Log(LogType::_FILEDBLOG, L"SendRegist(#d) Failed!!!\n", nServerID);

	return (nRet == 0);
}

void CDNMasterConnection::SendConCountInfo(int nServerID, int nCurConCount, int nMaxConCount)
{
	DN_ASSERT(0 != nServerID,		"Invalid!");
	DN_ASSERT(0 <= nCurConCount,	"Invalid!");
	DN_ASSERT(0 < nMaxConCount,		"Invalid!");

	LOMAConCountInfo ConCountInfo;
	ConCountInfo.Set(nServerID, nCurConCount, nMaxConCount);
	
	AddSendData(LOMA_CONCOUNTINFO, 0, (char*)&ConCountInfo, sizeof(ConCountInfo));
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDNMasterConnection::SendAddUser(CDNUserConnection *pUserCon, bool bForce/* = false*/)
{
	LOMAAddUser AddUser = {0,};

	AddUser.nAccountDBID = pUserCon->GetAccountDBID();
	AddUser.nSessionID = pUserCon->GetSessionID();
	AddUser.biCharacterDBID = pUserCon->GetSelectCharacterDBID();
	_wcscpy(AddUser.wszAccountName, IDLENMAX, pUserCon->GetAccountName(), (int)wcslen(pUserCon->GetAccountName()));
	_wcscpy(AddUser.wszCharacterName, NAMELENMAX, pUserCon->GetSelectCharacterName(), (int)wcslen(pUserCon->GetSelectCharacterName()));
	AddUser.bForce = bForce;
	AddUser.cAccountLevel = pUserCon->m_cAccountLevel;
	AddUser.cVillageID = pUserCon->m_cVillageID;
	AddUser.wChannelID = pUserCon->m_nChannelID;
	AddUser.bAdult = pUserCon->m_bAdult;
	_strcpy(AddUser.szIP, IPLENMAX, pUserCon->GetIp(), (int)strlen(pUserCon->GetIp()));
	_strcpy(AddUser.szVirtualIp, IPLENMAX, pUserCon->GetVirtualIp(), (int)strlen(pUserCon->GetVirtualIp()));
	AddUser.bTutorial = (pUserCon->GetSelectCharacterLastMapIndex() == 0) ? true : false;
#if defined(PRE_ADD_DWC)
	AddUser.cCharacterAccountLevel = pUserCon->GetSelectCharacterAccountLevel();
	if(AccountLevel_DWC == AddUser.cCharacterAccountLevel)
		AddUser.bTutorial = false;
#endif
	AddUser.cPCBangGrade = pUserCon->GetPCBangGrade();	// 피씨방 등급 (0: 피씨방 아님, 나머지: ePCBangGrade)
#ifdef PRE_ADD_MULTILANGUAGE
	AddUser.cSelectedLanguage = pUserCon->m_eSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_COMEBACK
	AddUser.bComebackUser = pUserCon->m_bComebackUser;
#endif		//PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	AddUser.bReConnectNewbieReward = pUserCon->m_bReConnectNewbieReward;
	AddUser.eUserGameQuitReward = pUserCon->m_eUserGameQuitReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef _KR
	memcpy(AddUser.szMID, pUserCon->m_szMID, sizeof(AddUser.szMID));
	AddUser.dwGRC = pUserCon->m_dwGRC;
#endif //#ifdef _KR
#if defined(_US)
	AddUser.nNexonUserNo = pUserCon->GetNexonUserNo();
#endif	// _US
#if defined(_ID)
	_strcpy(AddUser.szMacAddress, _countof(AddUser.szMacAddress), pUserCon->GetMacAddress(), (int)strlen(pUserCon->GetMacAddress()));
	_strcpy(AddUser.szKey, _countof(AddUser.szKey), pUserCon->GetKey(), (int)strlen(pUserCon->GetKey()) );
	AddUser.dwKreonCN = pUserCon->GetKreonCN();
#endif // #if defined(_ID)
#if defined(_KRAZ)
	AddUser.ShutdownData = pUserCon->m_ShutdownData;
#endif	// #if defined(_KRAZ)
#ifdef PRE_ADD_STEAM_USERCOUNT
	AddUser.bSteamUser = pUserCon->m_bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

	AddSendData(LOMA_ADDUSER, 0, (char*)&AddUser, sizeof(LOMAAddUser));
}

#else	// #if defined(PRE_MOD_SELECT_CHAR)

void CDNMasterConnection::SendAddUser(CDNUserConnection *pUserCon, int nWorldID, bool bForce/*=false*/ )
{
	LOMAAddUser packet;
	memset(&packet, 0, sizeof(LOMAAddUser));

	packet.nAccountDBID = pUserCon->GetAccountDBID();
	packet.nSessionID = pUserCon->GetSessionID();
	_wcscpy(packet.wszAccountName, IDLENMAX, pUserCon->GetAccountName(), (int)wcslen(pUserCon->GetAccountName()));
	packet.bForce = bForce;
#if defined(_KR)
	memcpy(packet.szMID, pUserCon->m_szMID, sizeof(packet.szMID));
	packet.dwGRC = pUserCon->m_dwGRC;
#endif
	packet.cAccountLevel = pUserCon->m_cAccountLevel;
	packet.cWorldID = nWorldID;
#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	_strcpy(packet.szIP, IPLENMAX, pUserCon->GetIp(), IPLENMAX);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#ifdef PRE_ADD_MULTILANGUAGE
	packet.cSelectedLanguage = pUserCon->m_eSelectedLanguage;
#endif		//#ifdef PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_COMEBACK
	packet.bComebackUser = pUserCon->m_bComebackUser;
#endif		//#ifdef PRE_ADD_COMEBACK
#if defined(PRE_ADD_GAMEQUIT_REWARD)
	packet.bReConnectNewbieReward = pUserCon->m_bReConnectNewbieReward;
	packet.eUserGameQuitReward = pUserCon->m_eUserGameQuitReward;
#endif	// #if defined(PRE_ADD_GAMEQUIT_REWARD)
#ifdef PRE_ADD_STEAM_USERCOUNT
	packet.bSteamUser = pUserCon->m_bSteamUser;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

	AddSendData(LOMA_ADDUSER, 0, (char*)&packet, sizeof(LOMAAddUser));
}

void CDNMasterConnection::SendUserInfo(CDNUserConnection *pUserCon, int nSelectedChannelID, bool bAdult, char *pVirtualIp, char *pIp, bool bTutorial)
{
	LOMAUserInfo UserInfo;
	memset(&UserInfo, 0, sizeof(LOMAUserInfo));

	UserInfo.nAccountDBID = pUserCon->GetAccountDBID();

#if defined(PRE_MOD_SELECT_CHAR)
	UserInfo.biCharacterDBID = pUserCon->GetSelectCharacterDBID();
	_wcscpy(UserInfo.wszCharacterName, NAMELENMAX, pUserCon->GetSelectCharacterName(), NAMELENMAX);
	
#else	// #if defined(PRE_MOD_SELECT_CHAR)
	TDBListCharData *pCharData = pUserCon->SelectCharData();
	if (pCharData){
		UserInfo.biCharacterDBID = pCharData->biCharacterDBID;
		wcsncpy(UserInfo.wszCharacterName, pCharData->wszCharacterName, NAMELENMAX);
	}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

	wcsncpy(UserInfo.wszAccountName, pUserCon->GetAccountName(), IDLENMAX);
	UserInfo.nSessionID = pUserCon->GetSessionID();
	UserInfo.cVillageID = pUserCon->m_cVillageID;
	UserInfo.nChannelID = (USHORT)nSelectedChannelID;

	UserInfo.bAdult = bAdult;
	_strcpy(UserInfo.szVirtualIp, IPLENMAX, pVirtualIp, (int)strlen(pVirtualIp));
	_strcpy(UserInfo.szIP, IPLENMAX, pIp, (int)strlen(pIp));
	UserInfo.bTutorial = bTutorial;
	UserInfo.cPCBangGrade = pUserCon->GetPCBangGrade();
#if defined(_US)
	UserInfo.nNexonUserNo = pUserCon->GetNexonUserNo();
#endif	// _US
#if defined(_ID)
	_strcpy(UserInfo.szMacAddress, _countof(UserInfo.szMacAddress), pUserCon->GetMacAddress(), (int)strlen(pUserCon->GetMacAddress()));	
	_strcpy(UserInfo.szKey, _countof(UserInfo.szKey), pUserCon->GetKey(), (int)strlen(pUserCon->GetKey()));		
	UserInfo.dwKreonCN = pUserCon->GetKreonCN();
#endif
#if defined(PRE_ADD_DWC)
	UserInfo.cCharacterAccountLevel = pUserCon->GetSelectCharacterAccountLevel();
	if( AccountLevel_DWC == UserInfo.cCharacterAccountLevel )
		UserInfo.bTutorial = false;
#endif

	AddSendData(LOMA_USERINFO, 0, (char*)&UserInfo, sizeof(LOMAUserInfo));
}

#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined(PRE_MOD_SELECT_CHAR)
void CDNMasterConnection::SendDetachUser(UINT nAccountDBID, bool bIsReconnectLogin/* = false*/, bool bIsDuplicate/* = false*/, UINT nSessionID/* = 0*/)
#else	// #if defined(PRE_MOD_SELECT_CHAR)
void CDNMasterConnection::SendDetachUser(UINT nAccountDBID, BYTE cAccountLevel, bool bIsReconnectLogin/* = false*/, const BYTE * pMachineID/* = NULL*/, DWORD dwGRC/* = 0*/, bool bIsDuplicate/* = false*/,
										 UINT nSessionID/*=0*/, const char * pszIP/* = NULL*/)
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	LOMADetachUser Detach;
	memset(&Detach, 0, sizeof(LOMADetachUser));

	Detach.nAccountDBID = nAccountDBID;
	Detach.cIsReconnectLogin = bIsReconnectLogin == true ? 1 : 0;
	Detach.bIsDuplicate = bIsDuplicate;
	Detach.nSessionID = nSessionID;

#if !defined(PRE_MOD_SELECT_CHAR)
#if defined(_KR)
	if (pMachineID)
	{
		memcpy(Detach.szMID, pMachineID, sizeof(Detach.szMID));
		Detach.dwGRC = dwGRC;
	}
#endif

	Detach.cAccountLevel = cAccountLevel;

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	if (pszIP != NULL)
		_strcpy(Detach.szIP, IPLENMAX, pszIP, IPLENMAX);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
#endif	// #if !defined(PRE_MOD_SELECT_CHAR)

	AddSendData(LOMA_DETACHUSER, 0, (char*)&Detach, sizeof(LOMADetachUser));
}

void CDNMasterConnection::SendReqTutorialGameID(UINT nAccountDBID, UINT nSessionID, int nMapID, char cGateNo)
{
	LOMAReqTutorialGameID GameID;
	memset(&GameID, 0, sizeof(LOMAReqTutorialGameID));

	GameID.nAccountDBID = nAccountDBID;
	GameID.nSessionID = nSessionID;
	GameID.nTutorialMapIndex = nMapID;
	GameID.cTutorialGateNo = cGateNo;

	AddSendData(LOMA_REQTUTORIALGAMEID, 0, (char*)&GameID, sizeof(LOMAReqTutorialGameID));	
}

void CDNMasterConnection::SendAddWaitUser(int nServerID, UINT nAccountDBID)
{
	LOMAAddWaitUser packet;
	memset(&packet, 0, sizeof(LOMAAddWaitUser));

	packet.nServerID = nServerID;
	packet.nAccountDBID = nAccountDBID;

	AddSendData(LOMA_ADDWAITUSER, 0, (char*)&packet, sizeof(LOMAAddWaitUser));
}

void CDNMasterConnection::SendDelWaitUser(int nServerID, UINT nAccountDBID)
{
	LOMADelWaitUser packet;
	memset(&packet, 0, sizeof(LOMADelWaitUser));

	packet.nServerID = nServerID;
	packet.nAccountDBID = nAccountDBID;

	AddSendData(LOMA_DELWAITUSER, 0, (char*)&packet, sizeof(LOMADelWaitUser));
}

void CDNMasterConnection::SendDetachUserOther(UINT nAccountDBID, int nLoginServerID)
{
	LOMADetachUserOther packet;
	memset(&packet, 0, sizeof(LOMADetachUserOther));
	
	packet.nAccountDBID = nAccountDBID;
	packet.nLoginServerID = nLoginServerID;

	AddSendData(LOMA_DETACHUSEROTHER, 0, (char*)&packet, sizeof(LOMADelWaitUser));
}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
void CDNMasterConnection::SendDetachUserbyIP(const char * pszIP)
{
	LOMADetachUserbyIP packet;
	memset(&packet, 0, sizeof(LOMADetachUserbyIP));

	_strcpy(packet.szIP, IPLENMAX, pszIP, IPLENMAX);

	AddSendData(LOMA_DETACHUSER_BY_IP, 0, (char*)&packet, sizeof(LOMADetachUserbyIP));
}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#ifdef PRE_ADD_BEGINNERGUILD
void CDNMasterConnection::SendGuildMemberAdd(MAAddGuildMember * pPacket)
{
	AddSendData(LOMA_ADDGUILDMEMB, 0, (char*)pPacket, sizeof(MAAddGuildMember));
}
#endif		//#ifdef PRE_ADD_BEGINNERGUILD
