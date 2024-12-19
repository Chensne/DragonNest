
#include "StdAfx.h"
#include "DNPvPRoomManager.h"
#include "DNPvPRoom.h"
#include "DNUserSession.h"
#include "DnPvPRoomSortObject.h"
#include "DNField.h"
#include "DNEvent.h"
#include "DNMasterConnection.h"
#include "DNGameDataManager.h"
#include "EtUIXML.h"
#include "DNGameDataManager.h"

#if defined( PRE_WORLDCOMBINE_PVP )
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#endif

extern TVillageConfig g_Config;

CDNPvPRoomManager::CDNPvPRoomManager()
{
	m_dwUpdateTick = timeGetTime();
#if defined( PRE_WORLDCOMBINE_PVP )
	m_dwGetWorldPvPRoomTick = 0;
	m_nWorldPvPChannelID = 0;
	m_nGMWorldPvPRoomIndex = WorldPvPMissionRoom::Common::GMWorldPvPRoomStartIndex;
	m_mapWorldPvPRoom.clear();
	m_listWorldPvpReqRoom.clear();
	m_pDBCon = NULL;
	m_bApplyWorldPvPRoom = false;
#endif
	m_nMasterDisconnected = false;
#if defined( PRE_PVP_GAMBLEROOM )
	m_bCreateGambleRoomFromDolis = false;
	memset( &m_GambleRoomData, 0, sizeof(m_GambleRoomData) );
	m_nPvPLobbyChannelID = 0;
#endif
}

CDNPvPRoomManager::~CDNPvPRoomManager()
{
	for( std::map<std::pair<USHORT,UINT>,CDNPvPRoom*>::iterator itor=m_mPvPRoom.begin() ; itor!=m_mPvPRoom.end() ; ++itor )
		SAFE_DELETE( (*itor).second );
}

bool CDNPvPRoomManager::bIsExist( const USHORT unVillageChannelID, const UINT uiPvPIndex )
{
	return (GetPvPRoom( unVillageChannelID, uiPvPIndex )) ? true : false;
}

CDNPvPRoom*	CDNPvPRoomManager::GetPvPRoom( const USHORT unVillageChannelID, const UINT uiPvPIndex )
{
	std::map<std::pair<USHORT,UINT>,CDNPvPRoom*>::iterator itor = m_mPvPRoom.find( std::make_pair(unVillageChannelID,uiPvPIndex) );
	if( itor == m_mPvPRoom.end() )
		return NULL;

	return (*itor).second;
}

short CDNPvPRoomManager::CreatePvPRoom( CDNUserSession* pUserObj, const MAVIPVP_CREATEROOM* pPacket )
{
	if( bIsExist( pPacket->unVillageChannelID, pPacket->uiPvPIndex ) )
		return ERROR_NONE;

	CDNPvPRoom* pRoom = new (std::nothrow) CDNPvPRoom;
	if( !pRoom )
		return ERROR_NONE;

	short nRet = pRoom->Create( pPacket );
	if( nRet == ERROR_NONE )
	{
		m_mPvPRoom.insert( std::make_pair( std::make_pair(pPacket->unVillageChannelID,pPacket->uiPvPIndex), pRoom ) );
#if defined( PRE_WORLDCOMBINE_PVP )
		if( pRoom->GetWorldPvPRoomType() )
		{
			pRoom->SetWorldPvPRoomMemberInfo( pPacket->cWorldPvPRoomCurMemberCount );
		}
#endif
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		std::map<USHORT,std::vector<std::list<CDNPvPRoom*>>>::iterator itor = m_listPvPRoom.find( pPacket->unVillageChannelID );
		if( itor == m_listPvPRoom.end() )
		{
			std::vector<std::list<CDNPvPRoom*>> pushvec;
			for (int i = 0; i < PvPCommon::RoomType::max; i++)
			{
				std::list<CDNPvPRoom*> list;
				if (i == pPacket->sCSPVP_CREATEROOM.cRoomType)
					list.push_back( pRoom );

				pushvec.push_back(list);
			}
			m_listPvPRoom.insert( std::make_pair( pPacket->unVillageChannelID, pushvec) );
		}
		else
		{
			if (pPacket->sCSPVP_CREATEROOM.cRoomType < PvPCommon::RoomType::max)
			{
				(*itor).second[pPacket->sCSPVP_CREATEROOM.cRoomType].push_back( pRoom );
			}
		}
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		std::map<USHORT,std::list<CDNPvPRoom*>>::iterator itor = m_listPvPRoom.find( pPacket->unVillageChannelID );
		if( itor == m_listPvPRoom.end() )
		{
			std::list<CDNPvPRoom*> list;
			list.push_back( pRoom );
			m_listPvPRoom.insert( std::make_pair( pPacket->unVillageChannelID, list) );
		}
		else
		{
			(*itor).second.push_back( pRoom );
		}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		if( pRoom->GetEventRoomIndex() > 0 )		
		{
			std::map<USHORT,std::list<CDNPvPRoom*>>::iterator EventItor = m_listPvPEventRoom.find( pRoom->GetEventPtr()->nEventID );
			if( EventItor == m_listPvPEventRoom.end() )
			{
				std::list<CDNPvPRoom*> list;
				list.push_back( pRoom );
				m_listPvPEventRoom.insert( std::make_pair(pRoom->GetEventPtr()->nEventID, list ) );
			}
			else
			{
				(*EventItor).second.push_back( pRoom );
			}

			g_Log.Log(LogType::_NORMAL, L"[Type:%d Group:%d ID:%d] PvPEventRoom Success!! \r\n", pRoom->GetEventPtr()->nEventType1, pRoom->GetEventPtr()->nEventType2, pRoom->GetEventPtr()->nEventID);
		}
		if( pUserObj )
		{
			pUserObj->SendPvPCreateRoom( ERROR_NONE, pPacket->uiPvPIndex, &pPacket->sCSPVP_CREATEROOM );
		}
		return ERROR_NONE;
	}

	delete pRoom;
	return nRet;
}

bool CDNPvPRoomManager::bDestroyPvPRoom( const USHORT unVillageChannelID, const UINT unPvPIndex )
{
	std::map<std::pair<USHORT,UINT>,CDNPvPRoom*>::iterator itor = m_mPvPRoom.find( std::make_pair(unVillageChannelID,unPvPIndex) );
	if( itor == m_mPvPRoom.end() )
	{
		_DANGER_POINT();
		return false;
	}

	CDNPvPRoom* pPvPRoom = (*itor).second;
	if( !pPvPRoom->bIsEmpty() )
	{
		_DANGER_POINT();
		return false;
	}

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	std::map<USHORT,std::vector<std::list<CDNPvPRoom*>>>::iterator itor2 = m_listPvPRoom.find( unVillageChannelID );
	if( itor2 != m_listPvPRoom.end() )
	{
		for (int i = 0; i < PvPCommon::RoomType::max; i++)
			(*itor2).second[i].remove(pPvPRoom);
	}
	m_mPvPRoom.erase( itor );
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	std::map<USHORT,std::list<CDNPvPRoom*>>::iterator itor2 = m_listPvPRoom.find( unVillageChannelID );
	if( itor2 != m_listPvPRoom.end() )
		(*itor2).second.remove( pPvPRoom );
	m_mPvPRoom.erase( itor );
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	if( pPvPRoom->GetEventRoomIndex() > 0 )
	{
		std::map<USHORT,std::list<CDNPvPRoom*>>::iterator EventItor = m_listPvPEventRoom.find( pPvPRoom->GetEventPtr()->nEventID );
		if( EventItor != m_listPvPEventRoom.end() )
			(*EventItor).second.remove( pPvPRoom );
	}
	delete pPvPRoom;

	return true;
}

#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
void CDNPvPRoomManager::SendPvPRoomList( CDNUserSession* pUserObj, const MAVIPVP_ROOMLIST* pPacket, BYTE cVillageID, UINT uiAccountDBID )
#else
void CDNPvPRoomManager::SendPvPRoomList( CDNUserSession* pUserObj, const MAVIPVP_ROOMLIST* pPacket )
#endif
{
	std::vector<CDNPvPRoom*> vPvPRoom;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	std::map<USHORT,std::vector<std::list<CDNPvPRoom*>>>::iterator itor = m_listPvPRoom.find( pPacket->unVillageChannelID );
	if(itor == m_listPvPRoom.end() || pPacket->sCSPVP_ROOMLIST.uiPage*PvPCommon::Common::RoomPerPage >= (*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].size())
	{
		if( pUserObj )
			pUserObj->SendPvPRoomList( ERROR_NONE, vPvPRoom, pPacket->sCSPVP_ROOMLIST.uiPage );
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		else if( cVillageID > 0 && uiAccountDBID )
			g_pMasterConnection->SendPvPRoomListRelay(cVillageID, uiAccountDBID, ERROR_NONE, vPvPRoom, pPacket->sCSPVP_ROOMLIST.uiPage);
#endif
		return;
	}
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	std::map<USHORT,std::list<CDNPvPRoom*>>::iterator itor = m_listPvPRoom.find( pPacket->unVillageChannelID );	
	if( itor == m_listPvPRoom.end() || pPacket->sCSPVP_ROOMLIST.uiPage*PvPCommon::Common::RoomPerPage >= (*itor).second.size() )
	{
		if( pUserObj )
			pUserObj->SendPvPRoomList( ERROR_NONE, vPvPRoom, pPacket->sCSPVP_ROOMLIST.uiPage );
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		else if( cVillageID > 0 && uiAccountDBID )
			g_pMasterConnection->SendPvPRoomListRelay(cVillageID, uiAccountDBID, ERROR_NONE, vPvPRoom, pPacket->sCSPVP_ROOMLIST.uiPage);
#endif
		return;
	}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	// Sort
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	switch( pPacket->sCSPVP_ROOMLIST.cRoomSortType )
	{
	case PvPCommon::RoomSortType::Index:
		{
			if( pPacket->sCSPVP_ROOMLIST.cIsAscend != 0)	
				(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].sort( PvPRoomSort::RoomIndexOrderByAsc() );
			else
				(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].sort( PvPRoomSort::RoomIndexOrderByDesc() );
			break;
		}
	case PvPCommon::RoomSortType::GameMode:
		{
			(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].sort( PvPRoomSort::GameModeOrder(pPacket->sCSPVP_ROOMLIST.uiSortData) );
			break;
		}
	case PvPCommon::RoomSortType::MapIndex:
		{
			(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].sort( PvPRoomSort::MapOrder(pPacket->sCSPVP_ROOMLIST.uiSortData) );
			break;
		}
	case PvPCommon::RoomSortType::NumOfPlayer:
		{
			if( pPacket->sCSPVP_ROOMLIST.cIsAscend != 0)	
				(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].sort( PvPRoomSort::NumOfPlayerOrderByAsc() );
			else
				(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].sort( PvPRoomSort::NumOfPlayerOrderByDesc() );
			break;
		}
	default:
		{
			if( pUserObj )
				pUserObj->SendPvPRoomList( ERROR_GENERIC_INVALIDREQUEST );
			return;
		}
	}
	//



	std::list<CDNPvPRoom*>::iterator itor2 = (*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].begin();
	std::advance( itor2, pPacket->sCSPVP_ROOMLIST.uiPage*PvPCommon::Common::RoomPerPage );


	for( ; itor2!=(*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].end() ; ++itor2 )
	{
		vPvPRoom.push_back( *itor2 );
		if( vPvPRoom.size() >= PvPCommon::Common::RoomPerPage )
			break;
	}
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	switch( pPacket->sCSPVP_ROOMLIST.cRoomSortType )
	{
		case PvPCommon::RoomSortType::Index:
		{
			if( pPacket->sCSPVP_ROOMLIST.cIsAscend != 0)	
				(*itor).second.sort( PvPRoomSort::RoomIndexOrderByAsc() );
			else
				(*itor).second.sort( PvPRoomSort::RoomIndexOrderByDesc() );
			break;
		}
		case PvPCommon::RoomSortType::GameMode:
		{
			(*itor).second.sort( PvPRoomSort::GameModeOrder(pPacket->sCSPVP_ROOMLIST.uiSortData) );
			break;
		}
		case PvPCommon::RoomSortType::MapIndex:
		{
			(*itor).second.sort( PvPRoomSort::MapOrder(pPacket->sCSPVP_ROOMLIST.uiSortData) );
			break;
		}
		case PvPCommon::RoomSortType::NumOfPlayer:
		{
			if( pPacket->sCSPVP_ROOMLIST.cIsAscend != 0)	
				(*itor).second.sort( PvPRoomSort::NumOfPlayerOrderByAsc() );
			else
				(*itor).second.sort( PvPRoomSort::NumOfPlayerOrderByDesc() );
			break;
		}
		default:
		{
			if( pUserObj )
				pUserObj->SendPvPRoomList( ERROR_GENERIC_INVALIDREQUEST );
			return;
		}
	}
	//

	std::list<CDNPvPRoom*>::iterator itor2 = (*itor).second.begin();
	std::advance( itor2, pPacket->sCSPVP_ROOMLIST.uiPage*PvPCommon::Common::RoomPerPage );

	for( ; itor2!=(*itor).second.end() ; ++itor2 )
	{
		vPvPRoom.push_back( *itor2 );
		if( vPvPRoom.size() >= PvPCommon::Common::RoomPerPage )
			break;
	}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	// 최대 페이지 계산
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	UINT uiRoomCount	= static_cast<UINT>((*itor).second[pPacket->sCSPVP_ROOMLIST.cChannelType].size());
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	UINT uiRoomCount	= static_cast<UINT>((*itor).second.size());
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	UINT uiMaxPage		= uiRoomCount/PvPCommon::Common::RoomPerPage;
	if( uiMaxPage > 0 && uiRoomCount%PvPCommon::Common::RoomPerPage == 0 )
		--uiMaxPage;

	if( pUserObj )
		pUserObj->SendPvPRoomList( ERROR_NONE, vPvPRoom, uiMaxPage );
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
	else if( cVillageID > 0 && uiAccountDBID )
		g_pMasterConnection->SendPvPRoomListRelay(cVillageID, uiAccountDBID, ERROR_NONE, vPvPRoom, uiMaxPage);
#endif
}

void CDNPvPRoomManager::SendPvPWaitUserList( CDNUserSession* pUserSession, const MAVIPVP_WAITUSERLIST* pPacket )
{
	if( !pUserSession )
		return;

	CDNField* pField = pUserSession->GetField();
	if( !pField || !pField->bIsPvPLobby() )
		return;

	std::vector<CDNUserSession*>	SendList;
	std::list<CDNUserSession*>		UserList;

	int iLimitLevel = -1;
#if defined(PRE_ADD_DWC)	//필요 없으면 제거하기
	if( pUserSession->bIsLadderUser() && pUserSession->GetPvPChannelType() != PvPCommon::RoomType::dwc )
#else
	if( pUserSession->bIsLadderUser() )
#endif
		iLimitLevel = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_LimitLevel ));
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	pField->GetWaitUserList( UserList, iLimitLevel, pUserSession->bIsLadderUser() ? static_cast<BYTE>(PvPCommon::RoomType::max) : pUserSession->GetPvPChannelType() );
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	pField->GetWaitUserList( UserList, iLimitLevel );
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

	if( UserList.empty() || static_cast<size_t>(pPacket->sCSPVP_WAITUSERLIST.unPage*PvPCommon::WaitUserList::Common::MaxPerPage) >= UserList.size() )
	{
		pUserSession->SendPvPWaitUserList( ERROR_NONE, SendList, pPacket->sCSPVP_WAITUSERLIST.unPage );
		return;
	}

	SendList.reserve( PvPCommon::WaitUserList::Common::MaxPerPage );

	switch( pPacket->sCSPVP_WAITUSERLIST.SortType )
	{
		case PvPCommon::WaitUserList::SortType::PvPLevel:
		{
			if( pPacket->sCSPVP_WAITUSERLIST.bIsAscend )
				UserList.sort( PvPUserSort::OrderByPvPLevelAsc() );
			else
				UserList.sort( PvPUserSort::OrderByPvPLevelDesc() );
			break;
		}
		case PvPCommon::WaitUserList::SortType::Job:
		{
			if( pPacket->sCSPVP_WAITUSERLIST.bIsAscend )
				UserList.sort( PvPUserSort::OrderByJobAsc() );
			else
				UserList.sort( PvPUserSort::OrderByJobDesc() );
			break;
		}
		case PvPCommon::WaitUserList::SortType::Level:
		{
			if( pPacket->sCSPVP_WAITUSERLIST.bIsAscend )
				UserList.sort( PvPUserSort::OrderByLevelAsc() );
			else
				UserList.sort( PvPUserSort::OrderByLevelDesc() );
			break;
		}
		case PvPCommon::WaitUserList::SortType::Name:
		{
			if( pPacket->sCSPVP_WAITUSERLIST.bIsAscend )
				UserList.sort( PvPUserSort::OrderByCharNameAsc() );
			else
				UserList.sort( PvPUserSort::OrderByCharNameDesc() );
			break;
		}
	}

	std::list<CDNUserSession*>::iterator itor = UserList.begin();
	std::advance( itor, pPacket->sCSPVP_WAITUSERLIST.unPage*PvPCommon::WaitUserList::Common::MaxPerPage );

	for( ; itor!=UserList.end() ; ++itor )
	{
		SendList.push_back( *itor );
		if( SendList.size() >= PvPCommon::WaitUserList::Common::MaxPerPage )
			break;
	}


	// 최대 페이지 계산
	UINT uiUserCount	= static_cast<UINT>(UserList.size());
	UINT uiMaxPage		= uiUserCount/PvPCommon::WaitUserList::Common::MaxPerPage;
	if( uiMaxPage > 0 && uiUserCount%PvPCommon::WaitUserList::Common::MaxPerPage == 0 )
		--uiMaxPage;

	if( pUserSession )
		pUserSession->SendPvPWaitUserList( ERROR_NONE, SendList, uiMaxPage );
}

void CDNPvPRoomManager::OnDisconnected( const UINT uiConnectionKey )
{
	switch( uiConnectionKey )
	{
		case CONNECTIONKEY_MASTER:
		{
			InterlockedExchange(&m_nMasterDisconnected, 1);
		}
	}
}


void CDNPvPRoomManager::DoUpdate( DWORD dwCurTick )
{
	if (InterlockedCompareExchange(&m_nMasterDisconnected, 0, 1) == 1)
	{
		MasterDisconnectProcess();
		return;
	}

	// 업데이트 주기 체크
	if( timeGetTime()-m_dwUpdateTick < 5000 )
		return;
	m_dwUpdateTick = dwCurTick;	

#if defined( PRE_WORLDCOMBINE_PVP )	
	CDNDBConnection* pDBCon = GetDBConnection();
	if( g_Config.nCombinePartyWorld > 0 )
	{
		if( timeGetTime()-m_dwGetWorldPvPRoomTick < WorldPvPMissionRoom::Common::GetWorldPvPRoomTick )
			return;
		m_dwGetWorldPvPRoomTick = dwCurTick;

		if( pDBCon && pDBCon->GetActive() )
			pDBCon->QueryGetListWorldPvPRoom();
	}	
#endif

	// PvPLobby 채널 검사
	if( !g_pMasterConnection )
		return;

	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
	if( !pChannelInfo )
		return;
	if( pChannelInfo->cVillageID != g_Config.nVillageID )
		return;
	
	if (!g_pDataManager) 
		return;

	if (!g_pMasterConnection || !g_pMasterConnection->GetActive())
		return;

	TVecPvPMissionRoom& vPvPMissionRoom = g_pDataManager->GetVecPvPMissionRoom();

	// 복사본을 만들고 셔플
	srand((UINT)time(NULL));
	
	TVecPvPMissionRoom vRandomPvPMissionRoom;
	std::copy (vPvPMissionRoom.begin(), vPvPMissionRoom.end(), std::back_inserter(vRandomPvPMissionRoom));
	random_shuffle(vRandomPvPMissionRoom.begin(), vRandomPvPMissionRoom.end());	

	for( TVecPvPMissionRoom::iterator iter = vRandomPvPMissionRoom.begin(); iter != vRandomPvPMissionRoom.end(); iter++ )
	{
		TPvPMissionRoom* pMissonRoom = &(*iter);
		if (!pMissonRoom)
			continue;
	
		bool bDuplicatedGroup = false;
		std::map<USHORT,std::list<CDNPvPRoom*>>::iterator itor = m_listPvPEventRoom.begin();
		while (itor != m_listPvPEventRoom.end())
		{
			std::list<CDNPvPRoom*>& list_rooms = itor->second;

			std::list<CDNPvPRoom*>::iterator itor2 = list_rooms.begin();
			while (itor2 != list_rooms.end())
			{
				CDNPvPRoom* pRoom = *itor2;
				
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				if (pRoom->GetEventPtr()->nEventType1 == pMissonRoom->nChannelType && pRoom->GetEventPtr()->nEventType2 == pMissonRoom->nGroupID)
				{
					bDuplicatedGroup = true;
					break;
				}
#else // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
				if (pRoom->GetEventPtr()->nEventType2 == pMissonRoom->nGroupID)
				{
					bDuplicatedGroup = true;
					break;
				}
#endif
				itor2++;
			}

			itor++;
		}

		if (bDuplicatedGroup) 
			continue;

		// pMissonRoom -> TEvent 데이터 변환
		TEvent PvPEvent;
		memset( &PvPEvent, 0, sizeof(PvPEvent) );
		PvPEvent._tEndTime = -1; // 이벤트 룸을 계속 유지한다.
		PvPEvent.nEventID = pMissonRoom->nItemID;
		PvPEvent.nAtt1	= pMissonRoom->nMapID;
		PvPEvent.nAtt2	= pMissonRoom->nModeID;
		PvPEvent.nAtt3	= pMissonRoom->nWinCondition;
		PvPEvent.nAtt4	= pMissonRoom->nPlayTime;
		PvPEvent.nAtt5	= pMissonRoom->nMaxPlayers;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		PvPEvent.nEventType1 = pMissonRoom->nChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		PvPEvent.nEventType2 = pMissonRoom->nGroupID;
		
		CSPVP_CREATEROOM CreateRoom;
		memset( &CreateRoom, 0, sizeof(CreateRoom) );

		CreateRoom.uiEventItemID		= pMissonRoom->nItemID;
		CreateRoom.uiMapIndex			= PvPEvent.nAtt1;
		CreateRoom.uiGameModeTableID	= PvPEvent.nAtt2;
		CreateRoom.uiSelectWinCondition	= PvPEvent.nAtt3;
		CreateRoom.uiSelectPlayTimeSec	= PvPEvent.nAtt4;
		CreateRoom.cMaxUser				= PvPEvent.nAtt5;
		CreateRoom.cMinUser				= pMissonRoom->nStartPlayers;
		CreateRoom.cMinLevel			= 1;
		CreateRoom.cMaxLevel			= CHARLEVELMAX;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		CreateRoom.cRoomType = PvPEvent.nEventType1;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		CreateRoom.unRoomOptionBit		= (PvPCommon::RoomOption::BreakInto);
		if (pMissonRoom->bDropItem)
			CreateRoom.unRoomOptionBit	|= PvPCommon::RoomOption::DropItem;
#ifdef PRE_MOD_PVPOBSERVER
		CreateRoom.unRoomOptionBit |= PvPCommon::RoomOption::AllowObserver;
#endif		//#ifdef PRE_MOD_PVPOBSERVER
	
#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pMissonRoom->nRoomNameIndex, MultiLanguage::eDefaultLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pMissonRoom->nRoomNameIndex );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		_wcscpy( CreateRoom.wszBuf, PvPCommon::TxtMax::RoomName, wszName.c_str(), static_cast<int>(wszName.size()) );
		CreateRoom.cRoomNameLen			= static_cast<BYTE>(wcslen( CreateRoom.wszBuf ));

		UINT uiPvPGameMode;
		if( CDNUserSession::bIsCheckPvPCreateRoom( NULL, &CreateRoom, CreateRoom.uiMapIndex, uiPvPGameMode ) == false )
		{
			g_Log.Log(LogType::_ERROR, L"[Type:%d Group:%d ID:%d] PvPEventRoom Fail!! \r\n", PvPEvent.nEventType1, PvPEvent.nEventType2, PvPEvent.nEventID);
			continue;
		}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( uiPvPGameMode == PvPCommon::GameMode::PvP_Tournament )
			CreateRoom.unRoomOptionBit |= PvPCommon::RoomOption::RandomTeam;
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
		g_pMasterConnection->SendPvPCreateRoom( 0, static_cast<BYTE>(uiPvPGameMode), pChannelInfo->nChannelID, pChannelInfo->nMapIdx, &CreateRoom, &PvPEvent );

	}
}

#if defined( PRE_WORLDCOMBINE_PVP )

CDNDBConnection* CDNPvPRoomManager::GetDBConnection()
{
	if( m_pDBCon == NULL )
	{
		BYTE cThreadID;
		m_pDBCon = g_pDBConnectionManager->GetDBConnection( g_Config.nWorldSetID, cThreadID );
	}

	return m_pDBCon;
}

void CDNPvPRoomManager::OnRecvWorldPvPRoomList( const TAGetListWorldPvPRoom *pPacket )
{
	if( m_nWorldPvPChannelID == 0 )
	{
		const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
		if( !pChannelInfo )
			return;
		m_nWorldPvPChannelID = pChannelInfo->nChannelID;
	}
	

	TVecWorldPvPMissionRoom& vWorldPvPMissionRoom = g_pDataManager->GetWorldVecPvPMissionRoom();

	std::list<int> listDestroyWorldPvPRoomIndex;
	listDestroyWorldPvPRoomIndex.clear();

	for( std::map<int, TWorldPvPRoomDBData>::iterator ii = m_mapWorldPvPRoom.begin();ii != m_mapWorldPvPRoom.end();ii++ )
	{
		listDestroyWorldPvPRoomIndex.push_back(ii->first);
	}

	m_mapWorldPvPRoom.clear();
	
	for( int i=0;i<pPacket->nCount;i++ )
	{
		int nRoomIndex = WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex;
		int nWinCondition = 0;
		int nPlaytime = 0;
		int nWorldPvPRoomType = WorldPvPMissionRoom::Common::MissionRoom;
		TVecWorldPvPMissionRoom::iterator iter;
		for( iter = vWorldPvPMissionRoom.begin(); iter != vWorldPvPMissionRoom.end(); iter++ )
		{
			if( pPacket->WorldPvPRoomData[i].cModeID == iter->cModeID && pPacket->WorldPvPRoomData[i].cChannelType == iter->nChannelType )
			{
				nRoomIndex += iter->nItemID;				
				break;
			}
		}

		if( iter == vWorldPvPMissionRoom.end() )
		{
			// 운영자가 생성한 방			
			nRoomIndex = ( pPacket->WorldPvPRoomData[i].nDBRoomID%100) + WorldPvPMissionRoom::Common::GMWorldPvPRoomStartIndex;
			nWorldPvPRoomType = WorldPvPMissionRoom::Common::GMRoom;
		}

		bool bCreatePvPRoom = false;

		CDNPvPRoom* pPvPRoom = GetPvPRoom( m_nWorldPvPChannelID, static_cast<UINT>(nRoomIndex) );

		if( pPvPRoom )
		{
			if( pPvPRoom->GetWorldPvPRoomDBIndex() != pPacket->WorldPvPRoomData[i].nDBRoomID )
			{
				bCreatePvPRoom = true;
				bDestroyPvPRoom(m_nWorldPvPChannelID, nRoomIndex);
			}
			else
			{
				pPvPRoom->SetWorldPvPRoomMemberInfo(pPacket->WorldPvPRoomData[i].nRoomMemberCount);
			}
		}
		else
			bCreatePvPRoom = true;
		
		m_mapWorldPvPRoom.insert( std::make_pair(nRoomIndex, pPacket->WorldPvPRoomData[i]) );

		if(bCreatePvPRoom)
		{				
			MAVIPVP_CREATEROOM Data;
			memset( &Data, 0, sizeof(Data));
			Data.cGameMode = pPacket->WorldPvPRoomData[i].cModeID;
			Data.sCSPVP_CREATEROOM.uiGameModeTableID = pPacket->WorldPvPRoomData[i].cModeID;
			Data.uiPvPIndex = nRoomIndex;
			Data.unVillageChannelID = m_nWorldPvPChannelID;
			Data.sCSPVP_CREATEROOM.uiSelectWinCondition = pPacket->WorldPvPRoomData[i].nWinCondition;
			Data.sCSPVP_CREATEROOM.uiSelectPlayTimeSec = pPacket->WorldPvPRoomData[i].nPlayTimeSec;		
			Data.sCSPVP_CREATEROOM.cMinLevel = pPacket->WorldPvPRoomData[i].cMinLevel;
			Data.sCSPVP_CREATEROOM.cMaxLevel = pPacket->WorldPvPRoomData[i].cMaxLevel;
			Data.sCSPVP_CREATEROOM.cMaxUser = pPacket->WorldPvPRoomData[i].cMaxPlayers;
			Data.sCSPVP_CREATEROOM.cRoomNameLen = (BYTE)wcslen(pPacket->WorldPvPRoomData[i].wszRoomName);
			Data.sCSPVP_CREATEROOM.uiMapIndex = pPacket->WorldPvPRoomData[i].nMapID;
#if defined( PRE_ADD_COLOSSEUM_BEGINNER )
			Data.sCSPVP_CREATEROOM.cRoomType = pPacket->WorldPvPRoomData[i].cChannelType;
#endif
			Data.sCSPVP_CREATEROOM.nWorldPvPRoomType = nWorldPvPRoomType;
			Data.cWorldPvPRoomCurMemberCount = (BYTE)(pPacket->WorldPvPRoomData[i].nRoomMemberCount);
			_wcscpy( Data.sCSPVP_CREATEROOM.wszBuf, _countof(Data.sCSPVP_CREATEROOM.wszBuf), pPacket->WorldPvPRoomData[i].wszRoomName, Data.sCSPVP_CREATEROOM.cRoomNameLen );
			
			if( CreatePvPRoom(NULL, &Data) == ERROR_NONE )
			{
				CDNPvPRoom* pPvPRoom = GetPvPRoom( m_nWorldPvPChannelID, static_cast<UINT>(nRoomIndex) );
				if( pPvPRoom )
				{	
					pPvPRoom->SetWorldPvPRoomDBIndex( pPacket->WorldPvPRoomData[i].nDBRoomID );

					if( pPacket->WorldPvPRoomData[i].nPassWord > 0 )
					{
						pPvPRoom->SetWorldPvPRoomPw(pPacket->WorldPvPRoomData[i].nPassWord);					
						UINT uiRoomState = pPvPRoom->GetRoomState();
						uiRoomState |= PvPCommon::RoomState::Password;
						pPvPRoom->SetRoomState(uiRoomState);
					}					
				}
			}
		}		
	}

	for( std::list<int>::iterator Delitor = listDestroyWorldPvPRoomIndex.begin();Delitor != listDestroyWorldPvPRoomIndex.end();Delitor++ )
	{
		if( m_mapWorldPvPRoom.end() == m_mapWorldPvPRoom.find(*Delitor) )
		{
			bDestroyPvPRoom(m_nWorldPvPChannelID, *Delitor);
		}
	}

	if( g_Config.nWorldSetID == g_Config.nCombinePartyWorld )
		CreateWorldPvPRoom();
}

void CDNPvPRoomManager::CreateWorldPvPRoom()
{
	if( !m_bApplyWorldPvPRoom )
		return;

	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
	
	if( !pChannelInfo )
		return;

	if( pChannelInfo->cVillageID != g_Config.nVillageID )
		return;

	TVecWorldPvPMissionRoom& vWorldPvPMissionRoom = g_pDataManager->GetWorldVecPvPMissionRoom();
	
	for( TVecWorldPvPMissionRoom::iterator iter = vWorldPvPMissionRoom.begin(); iter != vWorldPvPMissionRoom.end(); iter++ )
	{
		TWorldPvPMissionRoom* pMissonRoom = &(*iter);
		if (!pMissonRoom)
			continue;

		bool bDuplicatedGroup = false;		

		int nRoomIndex = PvPCommon::Common::MaxRoomCount+pMissonRoom->nItemID;
		bool bDuplicated = false;

		for(std::list<int>::iterator itor1 = m_listWorldPvpReqRoom.begin(); itor1!=m_listWorldPvpReqRoom.end();itor1++)
		{
			if(*itor1 == nRoomIndex)
			{
				bDuplicated = true;
				break;
			}				
		}

		for(std::map<int, TWorldPvPRoomDBData>::iterator itor2 = m_mapWorldPvPRoom.begin(); itor2!=m_mapWorldPvPRoom.end();itor2++)
		{
			if(itor2->first < WorldPvPMissionRoom::Common::GMWorldPvPRoomStartIndex && itor2->second.cModeID == pMissonRoom->cModeID && itor2->second.cChannelType == pMissonRoom->nChannelType)
			{
				bDuplicated = true;
				break;
			}
		}

		if(bDuplicated)
			continue;		

		g_pMasterConnection->SendWorldPvPCreateRoom( nRoomIndex, *pMissonRoom );
		m_listWorldPvpReqRoom.push_back( nRoomIndex );
	}
}

void CDNPvPRoomManager::DelWorldPvPRoom( int nRoomIndex )
{	
	for(std::list<int>::iterator itor = m_listWorldPvpReqRoom.begin(); itor!=m_listWorldPvpReqRoom.end();itor++)
	{
		if(*itor == nRoomIndex)
		{
			m_listWorldPvpReqRoom.erase(itor);
			return;
		}				
	}	
}

TWorldPvPRoomDBData* CDNPvPRoomManager::GetWorldPvPRoom( UINT uiPvPIndex )
{
	std::map<int, TWorldPvPRoomDBData>::iterator itor = m_mapWorldPvPRoom.find( uiPvPIndex );
	if( itor == m_mapWorldPvPRoom.end() )
		return NULL;

	return &((*itor).second);
}

#endif

void CDNPvPRoomManager::MasterDisconnectProcess()
{
	for( std::map<std::pair<USHORT,UINT>,CDNPvPRoom*>::iterator itor=m_mPvPRoom.begin() ; itor!=m_mPvPRoom.end() ; )
	{
		// 유저종료처리
		(*itor).second->OnMasterServerDisconnected();
		// 객체파괴
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		std::map<USHORT,std::vector<std::list<CDNPvPRoom*>>>::iterator itor2 = m_listPvPRoom.find( (*itor).first.first );
		if( itor2 != m_listPvPRoom.end() )
		{
			for (int i = 0; i < PvPCommon::RoomType::max; i++)
				(*itor2).second[i].remove( (*itor).second );
		}
#else		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		std::map<USHORT,std::list<CDNPvPRoom*>>::iterator itor2 = m_listPvPRoom.find( (*itor).first.first );
		if( itor2 != m_listPvPRoom.end() )
			(*itor2).second.remove( (*itor).second );
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		delete (*itor).second;
		itor = m_mPvPRoom.erase( itor );
	}
	m_listPvPEventRoom.clear();
}

#if defined( PRE_PVP_GAMBLEROOM )
void CDNPvPRoomManager::SetGambleRoomData( int nRoomIndex, bool bRegulation, BYTE cGambleType, int nPrice, bool bFlag )
{
	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
	if( !pChannelInfo )
		return;

	if( pChannelInfo->cVillageID != g_Config.nVillageID )
		return;

	if (!g_pMasterConnection || !g_pMasterConnection->GetActive())
		return;

	g_Log.Log(LogType::_NORMAL, L"SetGambleRoomData!! \r\n");

	m_GambleRoomData.nRoomIndex = nRoomIndex;
	m_GambleRoomData.bRegulation = bRegulation;
	m_GambleRoomData.cGambleType = cGambleType;
	m_GambleRoomData.nPrice = nPrice;

	g_pMasterConnection->SendCreateGambleRoomFlag( bFlag );
}

void CDNPvPRoomManager::CreateGambleRoom( int nTotalGambleRoomCount )
{	

	if( nTotalGambleRoomCount > (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PvPGambleRoomMaxCount) )
		return;

	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
	if( !pChannelInfo )
		return;

	if( pChannelInfo->cVillageID != g_Config.nVillageID )
		return;

	if (!g_pDataManager) 
		return;

	if (!g_pMasterConnection || !g_pMasterConnection->GetActive())
		return;
	

		TPvPMissionRoom* pGambleRoom = g_pDataManager->GetPvPGambleRoom(m_GambleRoomData.nRoomIndex);
		if (!pGambleRoom)
		{
			g_Log.Log(LogType::_ERROR, L"[RoomIndex:%d]GambleRoom Data Fail!! \r\n", m_GambleRoomData.nRoomIndex);
			return;
		}
		
		CSPVP_CREATEROOM CreateRoom;
		memset( &CreateRoom, 0, sizeof(CreateRoom) );
		
		CreateRoom.uiMapIndex			= pGambleRoom->nMapID;
		CreateRoom.uiGameModeTableID	= pGambleRoom->nModeID;
		CreateRoom.uiSelectWinCondition	= pGambleRoom->nWinCondition;
		CreateRoom.uiSelectPlayTimeSec	= pGambleRoom->nPlayTime;
		CreateRoom.cMaxUser				= pGambleRoom->nMaxPlayers;
		CreateRoom.cMinUser				= pGambleRoom->nStartPlayers;
		CreateRoom.cMinLevel			= 1;
		CreateRoom.cMaxLevel			= CHARLEVELMAX;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		CreateRoom.cRoomType = pGambleRoom->nChannelType;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		if( !m_GambleRoomData.bRegulation )
			CreateRoom.unRoomOptionBit |= PvPCommon::RoomOption::NoRegulation;		
		CreateRoom.unRoomOptionBit |= PvPCommon::RoomOption::RandomTeam;
		CreateRoom.unRoomOptionBit |= PvPCommon::RoomOption::AllKill_RandomOrder;

#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pGambleRoom->nRoomNameIndex, MultiLanguage::eDefaultLanguage );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
		std::wstring wszName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pGambleRoom->nRoomNameIndex );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		_wcscpy( CreateRoom.wszBuf, PvPCommon::TxtMax::RoomName, wszName.c_str(), static_cast<int>(wszName.size()) );
		CreateRoom.cRoomNameLen			= static_cast<BYTE>(wcslen( CreateRoom.wszBuf ));

		UINT uiPvPGameMode;
		if( CDNUserSession::bIsCheckPvPCreateRoom( NULL, &CreateRoom, CreateRoom.uiMapIndex, uiPvPGameMode ) == false )
		{
			g_Log.Log(LogType::_ERROR, L"[RoomIndex:%d]PvPGambleRoom Create Fail!! \r\n", m_GambleRoomData.nRoomIndex);
			return;
		}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( uiPvPGameMode == PvPCommon::GameMode::PvP_Tournament )
			CreateRoom.unRoomOptionBit |= PvPCommon::RoomOption::RandomTeam;
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
		g_pMasterConnection->SendPvPCreateRoom( 0, static_cast<BYTE>(uiPvPGameMode), pChannelInfo->nChannelID, pChannelInfo->nMapIdx, &CreateRoom, NULL, 0, &m_GambleRoomData );
}

int CDNPvPRoomManager::GetPvPLobbyChannelID()
{
	if( m_nPvPLobbyChannelID == 0 )
	{
		const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
		if( !pChannelInfo )
			return 0;
		m_nPvPLobbyChannelID = pChannelInfo->nChannelID;
	}
	return m_nPvPLobbyChannelID;
}

void CDNPvPRoomManager::DelGambleRoom()
{
	const sChannelInfo* pChannelInfo = g_pMasterConnection->GetChannelInfoFromAttr( GlobalEnum::CHANNEL_ATT_PVPLOBBY );
	if( !pChannelInfo )
		return;

	if( pChannelInfo->cVillageID != g_Config.nVillageID )
		return;

	if (!g_pMasterConnection || !g_pMasterConnection->GetActive())
		return;
	
	g_pMasterConnection->SendDelGambleRoom();
}

#endif