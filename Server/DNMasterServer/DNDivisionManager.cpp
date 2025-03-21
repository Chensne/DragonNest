
//note -2hogi
//MasterServer의 기본 Thread모델(처리에 대한것)은 Process Thread에 의한 SingleThread호출 방식 인터날아이들프로세스는 Process Thread에 의한 호출로
//내부 처리루틴에 대한 동기화를 필요치 않는다. 그런것들을 넣어주시고 성능문제발생시 또는 외부쓰레드이용시(예 idleprocess 같은거)에는
//Division객체의 내부 객체의 동기화를 필요로 하게된다. 

#include "stdafx.h"
#include "DNDivisionManager.h"
#include "DNGameConnection.h"
#include "DNVillageConnection.h"
#include "DNLoginConnection.h"
#include "DNserviceConnection.h"
#include "DNExtManager.h"
#include "DNUser.h"
//#include "DNParty.h"
#include "DNPvP.h"
#include "IniFile.h"
#include "util.h"
#include "log.h"
#include "DNWaitUserManager.h"
#include "DNLogConnection.h"
#include "DNFarm.h"
#include "TimeSet.h"
#include "DNGuildWarManager.h"
#ifdef PRE_ADD_DOORS
#include "DNDoorsConnection.h"
#endif		//#ifdef PRE_ADD_DOORS

#if defined(_KR)
#include "DNNexonAuth.h"
#elif defined(_CH)
#include "DNShandaFCM.h"
#elif defined (_JP) && defined (WIN64)
#include "DNNHNNetCafe.h"
#elif defined(_TW)
#include ".\\TW\\DNGamaniaAuth.h"
#elif defined(_US)
#include "DNNexonPI.h"
#elif defined(_TH)
#include "DNAsiaSoftPCCafe.h"
#endif	// _KR _CH _JP _TW

extern TMasterConfig g_Config;

CDivisionManager * g_pDivisionManager = NULL;

CDivisionManager::CDivisionManager()
{
	_ASSERT( g_Config.nWorldSetID > 0 );

	::memset(m_dwTick, 0, sizeof(m_dwTick));

	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		if (0 < nIndex && !g_Config.LoginInfo[nIndex].nPort) {
			// 최소 1개의 로그인 서버 세션은 생성하고 연결정보가 없는 나머지 세션들은 넘김
			m_pLoginConnectionList[nIndex] = NULL;
			continue;
		}

		m_pLoginConnectionList[nIndex] = new CDNLoginConnection;
		DN_ASSERT(NULL != m_pLoginConnectionList[nIndex],	"Invalid!");

		m_pLoginConnectionList[nIndex]->SetIp(g_Config.LoginInfo[nIndex].szIP);
		m_pLoginConnectionList[nIndex]->SetPort(g_Config.LoginInfo[nIndex].nPort);

		// P.S.> LO 의 IP / PORT 정보가 없으면 생성 않함 ??? ↔ 예외상황 방지 위해 일단 모두 생성함 ???
	}

	m_pLoginConnection = m_pLoginConnectionList[0];
	DN_ASSERT(NULL != m_pLoginConnection,	"Invalid!");

	m_GameServerConList.clear();
	m_VillageServerConList.clear();
	m_AccountDBIDList.clear();
	m_SessionIDList.clear();
	m_UserNameList.clear();
	m_CharacterDBIDList.clear();

	// PvP
	for( UINT i=1 ; i<=PvPCommon::Common::MaxRoomCount ; ++i )
		m_listPvPIndex.push_back( i );

	m_bDestroyFarm = false;
	m_nFarmGameindex = -1;
	m_cPvPLobbyVillageID	= 0;
	m_unPvPLobbyChannelID	= 0;
	m_nPvPLobbyMapIndex = 0;

	m_dwCheckVillageChannelTick = timeGetTime();
#if defined(PRE_FIX_DELAY_MASTER_CHECKVILLAGECHANNEL)
	m_dwCheckVillageChannelStartTick = m_dwCheckVillageChannelTick;
#endif

	m_nProcessCount = 0;

#ifdef PRE_MOD_OPERATINGFARM
	m_dwFarmDataCreateTick = 0;
#endif		//#ifdef PRE_MOD_OPERATINGFARM
#ifdef PRE_ADD_STEAM_USERCOUNT
	m_nSteamUserCount = 0;
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT
#if defined( PRE_PVP_GAMBLEROOM )
	m_bGambleRoomFlag = false;
	m_dwGambleRoomSendTick = 0;
#endif
}

CDivisionManager::~CDivisionManager()
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator ig;
	for (ig = m_GameServerConList.begin(); ig != m_GameServerConList.end(); ig++)
		SAFE_DELETE((*ig));

	std::vector <CDNVillageConnection*>::iterator iv;
	for (iv = m_VillageServerConList.begin(); iv != m_VillageServerConList.end(); iv++)
		SAFE_DELETE((*iv));

	std::map <UINT, CDNUser*>::iterator iu;
	for (iu = m_AccountDBIDList.begin(); iu != m_AccountDBIDList.end(); iu++)
		SAFE_DELETE((*iu).second);

	// PvP
	for( _TPvPMap::iterator itor=m_mPvP.begin() ; itor!=m_mPvP.end() ; ++itor )
		SAFE_DELETE( (*itor).second );
	m_mPvP.clear();

	std::map <UINT, CDNFarm*>::iterator ifa;
	for (ifa = m_mFarmList.begin(); ifa != m_mFarmList.end(); ifa++)
		SAFE_DELETE((*ifa).second);
	m_mFarmList.clear();

	m_GameServerConList.clear();
	m_VillageServerConList.clear();
	m_AccountDBIDList.clear();
	m_SessionIDList.clear();
	m_CharacterDBIDList.clear();
	m_UserNameList.clear();

	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		SAFE_DELETE(m_pLoginConnectionList[nIndex]);
	}
	m_pLoginConnection = NULL;
}

void CDivisionManager::CheckVillageChannel( DWORD dwCurTick )
{
	const int CHECK_PERCENTAGE_VALUE = 70;
#if defined(PRE_FIX_DELAY_MASTER_CHECKVILLAGECHANNEL)
	if( dwCurTick - m_dwCheckVillageChannelStartTick <= 1000*60*3 )	//처음 시작후 3분간은 포화상태 체크 안함
		return;
#endif
	if( dwCurTick - m_dwCheckVillageChannelTick <= 1000*30 )
		return;
	m_dwCheckVillageChannelTick = dwCurTick;

	struct TTempChannel
	{
		int		iChannelID;
		int		iMeritID;
		int		iPercentage;
		bool	bVisibility;
	};

	std::map<std::pair<int,int>,std::vector<TTempChannel>> mList;
#if defined(PRE_ADD_CHANNELNAME)
	std::multimap<std::pair<int,int>,std::wstring> mPassList;
#else
	std::map<std::pair<int,int>,int> mPassList;
#endif //#if defined(PRE_ADD_CHANNELNAME)

	for( UINT i=0 ; i<m_VillageServerConList.size() ; ++i )
	{
		CDNVillageConnection* pVillageCon = m_VillageServerConList[i];
		if( pVillageCon == NULL || pVillageCon->GetConnectionCompleted() == false )
			continue;
		const TVillageInfo* pVillageInfo = pVillageCon->GetVillageChannelInfo();
		if( pVillageInfo == NULL )
			continue;

		for( UINT j=0 ; j<pVillageInfo->vOwnedChannelList.size() ; ++j )
		{
			TTempChannel Temp;
			Temp.iChannelID		= pVillageInfo->vOwnedChannelList[j].nChannelID;
			Temp.iMeritID		= pVillageInfo->vOwnedChannelList[j].nMeritBonusID;
			Temp.iPercentage	= pVillageInfo->vOwnedChannelList[j].GetUserPercentage();
			Temp.bVisibility	= pVillageInfo->vOwnedChannelList[j].bVisibility;
			
			std::pair<int,int> KeyPair = std::make_pair(pVillageInfo->vOwnedChannelList[j].nMapIdx,Temp.iMeritID);
#if defined(PRE_ADD_CHANNELNAME)
			bool bContinueFlag = false;
			for (std::multimap<std::pair<int,int>,std::wstring>::iterator iter = mPassList.lower_bound(KeyPair); iter != mPassList.upper_bound(KeyPair); iter++)
			{
				if( iter != mPassList.end() && _wcsicmp(pVillageInfo->vOwnedChannelList[j].wszLanguageName, iter->second.c_str() ) == 0 )
				{
					bContinueFlag = true;
					break;
				}
			}
			if (bContinueFlag)
				continue;
#else
			if( mPassList.find( KeyPair ) != mPassList.end() )
				continue;
#endif //#if defined(PRE_ADD_CHANNELNAME)

			if( Temp.bVisibility == true && Temp.iPercentage < CHECK_PERCENTAGE_VALUE )
			{
				mList.erase( KeyPair );
#if defined(PRE_ADD_CHANNELNAME)
				mPassList.insert( std::make_pair(KeyPair,pVillageInfo->vOwnedChannelList[j].wszLanguageName) );
#else
				mPassList.insert( std::make_pair(KeyPair,1) );
#endif
				continue;
			}

			std::map<std::pair<int,int>,std::vector<TTempChannel>>::iterator itor = mList.find( KeyPair );
			if( itor == mList.end() )
			{
				std::vector<TTempChannel> vTemp;
				vTemp.push_back( Temp );

				mList.insert( std::make_pair(KeyPair,vTemp) );
			}
			else
			{
				(*itor).second.push_back( Temp );
			}
		}
	}
	
	for( std::map<std::pair<int,int>,std::vector<TTempChannel>>::iterator itor=mList.begin() ; itor!=mList.end() ; ++itor )
	{
		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			if( (*itor).second[i].bVisibility == false )
			{
#if defined( _WORK )
				std::cout << "채널 포화상태여서 추가로 ChannelID=" << (*itor).first.first << " 오픈함" << std::endl;
#endif // #if defined( _WORK )
				if( ChannelControl( (*itor).second[i].iChannelID, true, 0 ) == true )
					SendVillageInfo();
				break;
			}
		}
	}
}

#ifdef _WORK
void CDivisionManager::ReloadExt()
{
	std::vector <CDNGameConnection*>::iterator igame;
	for (igame = m_GameServerConList.begin(); igame != m_GameServerConList.end(); igame++)
		(*igame)->SendReloadExt();

	std::vector <CDNVillageConnection*>::iterator ivill;
	for (ivill = m_VillageServerConList.begin(); ivill != m_VillageServerConList.end(); ivill++)
		(*ivill)->SendReloadExt();
}

void CDivisionManager::ReloadAct()
{
	std::vector <CDNGameConnection*>::iterator igame;
	for (igame = m_GameServerConList.begin(); igame != m_GameServerConList.end(); igame++)
		(*igame)->SendReloadAct();
}
#endif		//#ifdef _WORK

void CDivisionManager::InternalIdleProcess(ULONG nCurTick)
{
	//여기는 내부쓰레드에서 호출입니다(process thread는 내부로 봄) 동기화 관련없이 자유롭게 작성해주세요
	static UINT s_nIdleCount = 0;
	//CheckIntervalDeletePartyMember(nCurTick);
	SendVillageInfo();
	CheckProcessFarm();
	SendFarmInfo();
#ifndef _WORK
	if ((s_nIdleCount%5) == 0)
		CheckZombie(nCurTick);
#endif
#if defined(PRE_ADD_GUILDWAR_ONSCHEDULE)
	if( g_pGuildWarManager )
		g_pGuildWarManager->DoUpdate(nCurTick);
#endif
	CheckVillageChannel( nCurTick );
#ifdef PRE_ADD_STEAM_USERCOUNT
	if ((s_nIdleCount%120) == 0)
		g_Log.Log(LogType::_STEAMUSERCOUNT, L"Users[%d]Steam[%d]\n", (int)m_AccountDBIDList.size(), m_nSteamUserCount);
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

	s_nIdleCount++;
}

void CDivisionManager::ExternalIdleProcess(ULONG nCurTick)
{
	//process Thread가 아닌 외부 Thread에서의 호출입니다. 아이들링 작업시 사용하는 객체가 동기가 걸필요 있는지 필히 확인후 작성해주세요 -2hogi
	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection *pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon) {
			continue;
		}

		pLoginCon->Reconnect(nCurTick);
	}
	if (g_pServiceConnection)
		g_pServiceConnection->Reconnect(nCurTick);
	if( g_pLogConnection )
		g_pLogConnection->Reconnect( nCurTick );

	if (LOGINSERVERLISTSORTTERM < GetTickTerm(GetTick(eTickSortLoginServerList), nCurTick)) {
		SortLoginConnectionList();
		SetTick(eTickSortLoginServerList, nCurTick);
	}
}

// InternalIdleProcess 와 같은 Timing 입니다. 
// InternalIdleProcess 가 Interval 이 너무 길어 따로 함수 분류하였습니다.

void CDivisionManager::UpdatePvPRoom( const UINT uiCurTick )
{
	CTimeSet CurTime;

#if defined( PRE_PVP_GAMBLEROOM )
	int nGambleRoomCount = 0;
	int nTotalGambleRoomCount = 0;
#endif
	for( _TPvPMap::iterator itor=m_mPvP.begin() ; itor!=m_mPvP.end() ;  )
	{
		CDNPvP* pPvPRoom = (*itor).second;

		pPvPRoom->Update( uiCurTick );

		if( pPvPRoom->GetEventRoomIndex() > 0 && pPvPRoom->GetEventDataPtr()->_tEndTime != -1)
		{
			if( CurTime.GetTimeT32_LC() >= pPvPRoom->GetEventDataPtr()->_tEndTime )
			{
				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID( pPvPRoom->GetVillageID() );

				(*itor).second->bLeaveAll( pVillageCon, PvPCommon::LeaveType::DestroyRoom );
				if( pVillageCon )
					pVillageCon->SendPvPDestroyRoom( pPvPRoom->GetVillageChannelID(), pPvPRoom->GetIndex() );

				delete pPvPRoom;
				m_mPvP.erase( itor++ );
				continue;
			}
		}
#if defined( PRE_PVP_GAMBLEROOM )
		if( pPvPRoom->GetGambleRoomType() > 0 )
		{
			if( pPvPRoom->GetRoomState() == PvPCommon::RoomState::None || pPvPRoom->GetRoomState() == PvPCommon::RoomState::Starting )
				nGambleRoomCount++;
			nTotalGambleRoomCount++;
		}
#endif
		++itor;
	}

#if defined( PRE_PVP_GAMBLEROOM )
	if( m_bGambleRoomFlag && nGambleRoomCount <= 0 )
	{
		if( m_dwGambleRoomSendTick > uiCurTick )
			return;

		m_dwGambleRoomSendTick = uiCurTick + SENDGAMBLEROOMWAITTICK;
		CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
		if( pVillageConnection )
		{			
			pVillageConnection->SendGambleRoomCreate( nTotalGambleRoomCount );
		}
	}
#endif
}

bool CDivisionManager::AddSelectJoin(int nSID, const WCHAR * pwszCharacterName)
{
	int nTemp = 0, nTemp2 = 0;
	if (GetGameConnectionByManagedID(nSID) == NULL)
		return false;

	if (IsSelectJoin(pwszCharacterName, nTemp))
	{
		ScopeLock<CSyncLock> Lock(m_Sync);

		std::vector <TSelectJoin>::iterator ii;
		for (ii = m_SelectJoinList.begin(); ii != m_SelectJoinList.end(); ii++)
		{
			if (!__wcsicmp_l((*ii).wszCharacterName, pwszCharacterName))
			{
				(*ii).nSID = nSID;
				return true;
			}
		}
	}
	else
	{
		TSelectJoin Join;
		memset(&Join, 0, sizeof(TSelectJoin));

		Join.nSID = nSID;
		_wcscpy(Join.wszCharacterName, _countof(Join.wszCharacterName), pwszCharacterName, (int)wcslen(pwszCharacterName));

		ScopeLock<CSyncLock> Lock(m_Sync);
		m_SelectJoinList.push_back(Join);
	}	
	return true;
}

void CDivisionManager::ClearSelectJoin()
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	m_SelectJoinList.clear();
}

bool CDivisionManager::IsSelectJoin(const WCHAR * pwszCharacterName, int &nSID)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	if (m_SelectJoinList.empty()) return false;
	
	std::vector <TSelectJoin>::iterator ii;
	for (ii = m_SelectJoinList.begin(); ii != m_SelectJoinList.end(); ii++)
	{
		if (!__wcsicmp_l((*ii).wszCharacterName, pwszCharacterName))
		{
			nSID = (*ii).nSID;
			return true;
		}
	}
	return false;
}

DWORD CDivisionManager::GetTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	return(m_dwTick[pTick]);
}

void CDivisionManager::SetTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	m_dwTick[pTick] = ::GetTickCount();
	m_dwTick[pTick] = (m_dwTick[pTick])?(m_dwTick[pTick]):(1);
}

void CDivisionManager::SetTick(eTick pTick, DWORD pTick32) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");
	DN_ASSERT(0 != pTick32,						"Invalid!");

	m_dwTick[pTick] = (pTick32)?(pTick32):(1);
}

void CDivisionManager::ResetTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	m_dwTick[pTick] = 0;
}

BOOL CDivisionManager::IsTick(eTick pTick) const
{
	DN_ASSERT(CHECK_LIMIT(pTick, eTickCnt),	"Invalid!");

	return(0 != m_dwTick[pTick]);
}

bool CDivisionManager::DelConnection(CConnection *pCon, eConnectionKey eConType)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	if (eConType == CONNECTIONKEY_LOGIN)
	{
		CDNLoginConnection * pLoginCon = reinterpret_cast<CDNLoginConnection*>(pCon);
		DetachUserloginID(pLoginCon->GetServerID());
		return true;
	}
	else if (eConType == CONNECTIONKEY_VILLAGE)
	{
		CDNVillageConnection * pVillCon = reinterpret_cast<CDNVillageConnection*>(pCon);
		// 빌리지쪽 애들 다 끊어버리기
		DetachUserByVillageID( pVillCon->GetVillageID() );

		// 해당서버 PvP방 모두 정리
		for( _TPvPMap::iterator itor=m_mPvP.begin() ; itor!=m_mPvP.end() ; )
		{
			CDNPvP* pPvPRoom = (*itor).second;

			bool bDestroy = false;
			if( pPvPRoom->GetVillageID() == pVillCon->GetVillageID() )
				bDestroy = pPvPRoom->bDisconnectServer( eConType );

			if( bDestroy )
			{
				delete pPvPRoom;
				itor = m_mPvP.erase( itor );
			}
			else
				++itor;
		}

		bool bCheck = false;
		int nVillageID = -1;
		std::vector <CDNVillageConnection*>::iterator ii;
		for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		{
			if ((*ii) == (CDNVillageConnection*)pCon)
			{
				CDNVillageConnection * pVillageCon = (*ii);
				nVillageID = pVillageCon->GetVillageID();
				m_VillageServerConList.erase(ii);
				g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[VillageID:%d] DelConnection", pVillageCon->GetVillageID());
				bCheck = true;
				break;
			}
		}

		//빌라지가 끊겨따아~ 로그인이랑 빌리지에게 채널리스트변경을 알리자아~
		if (bCheck)
		{
			SendAllLoginServerVillageInfoDelete(nVillageID);

			for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
				(*ii)->SendVillageInfoDelete(nVillageID);
			return true;
		}
	}
	else if (eConType == CONNECTIONKEY_GAME)
	{
		CDNGameConnection * pGCon = reinterpret_cast<CDNGameConnection*>(pCon);
		// 게임쪽 애들 다 끊어버리기
		DetachUserByGameID( pGCon->GetGameID() );

		// 해당서버 PvP방 모두 정리
		for( _TPvPMap::iterator itor=m_mPvP.begin() ; itor!=m_mPvP.end() ; )
		{
			CDNPvP* pPvPRoom = (*itor).second;

			bool bDestroy = false;
			if( pPvPRoom->GetGameServerID() == pGCon->GetGameID() )
				bDestroy = pPvPRoom->bDisconnectServer( eConType );

			if( bDestroy )
			{
				//시스템 길드전 예외처리
				if (g_pGuildWarManager && pPvPRoom->GetIsGuildWarSystem())
					g_pGuildWarManager->SetDropTournament(pPvPRoom->GetIndex());

				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID( (*itor).second->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendPvPDestroyRoom( (*itor).second->GetVillageChannelID(), (*itor).second->GetIndex() );

				if (pPvPRoom->GetEventRoomIndex() > 0)
					g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"DelConnection-Game [Index:%d][Room:%d][Event:%d] \r\n", pPvPRoom->GetIndex(), pPvPRoom->GetGameServerRoomIndex(), pPvPRoom->GetEventRoomIndex());

				delete pPvPRoom;
				itor = m_mPvP.erase( itor );
			}
			else
				++itor;
		}

		std::map <UINT, CDNFarm*>::iterator ifarm;
		for (ifarm = m_mFarmList.begin(); ifarm != m_mFarmList.end(); ifarm++)
		{
			if ((*ifarm).second->GetAssignedServerID() == pGCon->GetGameID())
			{
				(*ifarm).second->DestroyFarm();
				m_bDestroyFarm = true;
				m_nFarmGameindex = -1;
			}
		}

		if (m_bDestroyFarm)
			g_Log.Log(LogType::_FARM, L"FarmGameConnection Destroy GameCon[%d]\n", pGCon->GetGameID());

		std::vector <CDNGameConnection*>::iterator ii;
		for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
		{
			if ((*ii) == (CDNGameConnection*)pCon)
			{
				CDNGameConnection * pGameCon = (*ii);
				m_GameServerConList.erase(ii);
				g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, 0, 0, 0, L"[GameID:%d] DelConnection", pGameCon->GetGameID());
				return true;
			}
		}
	}
	else if (eConType == CONNECTIONKEY_LOG || eConType == CONNECTIONKEY_SERVICEMANAGER )
	{
		return true;
	}

	_DANGER_POINT();
	return false;
}

CDNLoginConnection * CDivisionManager::FindLoginConnection(const char* pIp, const int nPort)
{
	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection *pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon) {
			continue;
		}

		if (!strncmp(pLoginCon->GetIp(), pIp, IPLENMAX) && pLoginCon->GetPort() == nPort) {
			return pLoginCon;
		}
	}

	return NULL;
}

void CDivisionManager::SortLoginConnectionList() const
{
	std::vector<CDNLoginConnection*> List1;
	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection *pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon) {
			continue;
		}

		List1.push_back(pLoginCon);
	}
	if (List1.empty()) {
		return;
	}

	std::sort(List1.begin(), List1.end(), CDNLoginConnection::CSortLoginServerSession());

	std::vector<CDNLoginConnection*> List2;
	std::remove_copy_if(List1.begin(), List1.end(), std::back_inserter(List2), CDNLoginConnection::CRemoveCopyIfLoginServerSession());

	if (1 < List2.size()) {
		std::random_shuffle(List2.begin(), List2.end());	// 주의 !!! - ::srand() 로 랜덤시드 값이 미리 지정되어야 함
	}

	if (List2.empty()) {
		::InterlockedExchangePointer(reinterpret_cast<void**>(&m_pLoginConnection), List1[0]);	// m_pLoginConnection 의 주소가 운영체제 타입에 따라 32/64 비트로 정렬 (aligned) 되어 있지 않으면 실패 !!!
	}
	else {
		::InterlockedExchangePointer(reinterpret_cast<void**>(&m_pLoginConnection), List2[0]);	// m_pLoginConnection 의 주소가 운영체제 타입에 따라 32/64 비트로 정렬 (aligned) 되어 있지 않으면 실패 !!!
	}
	DN_ASSERT(NULL != m_pLoginConnection,	"Invalid!");
}

CDNGameConnection * CDivisionManager::GameServerConnected(const char * pIP, USHORT nPort)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	
	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetPort() == nPort)
		{
			_DANGER_POINT();
			return NULL;
		}
	}

	CDNGameConnection * pCon = new CDNGameConnection;
	if (pCon == NULL)	
		return NULL;

	pCon->SetIp(pIP);
	pCon->SetPort(nPort);
	pCon->SetSessionID(g_IDGenerator.GetGameConnectionID());
	pCon->SetGameID(pCon->GetSessionID());

#if !defined(PRE_FIX_LIVE_CONNECT)
	m_GameServerConList.push_back(pCon);
#endif //#if !defined(PRE_FIX_LIVE_CONNECT)

	return pCon;
}

#if defined(PRE_FIX_LIVE_CONNECT)
void CDivisionManager::AddGameServerConnection(CDNGameConnection* pCon)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	m_GameServerConList.push_back(pCon);
}

void CDivisionManager::AddVillageServerConnection(CDNVillageConnection* pCon)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	m_VillageServerConList.push_back(pCon);
}
#endif

void CDivisionManager::GameServerRegist(CDNGameConnection * pCon)
{
	CDNUser * pUser = NULL;
	int nLocation = _LOCATION_NONE;
	int nChannelID = -1;
	int nMapIdx = -1;

	std::map <UINT, CDNUser*>::iterator ii;
	for (ii = m_AccountDBIDList.begin(); ii != m_AccountDBIDList.end(); ii++)
	{
		pUser = (*ii).second;
		if (pUser->GetUserState() == STATE_NONE) continue;
		nLocation = _LOCATION_NONE;
		nChannelID = -1;
		nMapIdx = -1;

		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_CHECKGAME)
			nLocation = _LOCATION_MOVE;
		else if (pUser->GetUserState() == STATE_VILLAGE)
		{
			nLocation = _LOCATION_VILLAGE;
			nChannelID = pUser->GetChannelID();
		}
		else if (pUser->GetUserState() == STATE_GAME)
		{
			nLocation = _LOCATION_GAME;
			nMapIdx = pUser->m_nMapIndex;
		}

		pCon->SendAddUserState((*ii).second->GetCharacterName(), (*ii).second->GetCharacterDBID(), nLocation, -1, nChannelID, nMapIdx );
	}
}

CDNVillageConnection * CDivisionManager::VillageServerConnected(const char * pIP, USHORT nPort)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->GetPort() == nPort)
		{
			_DANGER_POINT();
			return NULL;
		}
	}

	CDNVillageConnection * pCon = new CDNVillageConnection;
	if (pCon == NULL)
		return NULL;

	pCon->SetIp(pIP);
	pCon->SetPort(nPort);
	pCon->SetSessionID(g_IDGenerator.GetVillageConnectionID());

#if !defined(PRE_FIX_LIVE_CONNECT)
	m_VillageServerConList.push_back(pCon);
#endif //#if !defined(PRE_FIX_LIVE_CONNECT)

	return pCon;
}

void CDivisionManager::SendWorldUserStateList(CDNVillageConnection * pCon)
{
	CDNUser * pUser = NULL;
	int nLocation = _LOCATION_NONE;
	int nChannelID = -1;
	int nMapIdx = -1;

	std::map <UINT, CDNUser*>::iterator ii;
	for (ii = m_AccountDBIDList.begin(); ii != m_AccountDBIDList.end(); ii++)
	{
		pUser = (*ii).second;
		if (pUser->GetUserState() == STATE_NONE) continue;
		nLocation = _LOCATION_NONE;
		nChannelID = -1;
		nMapIdx = -1;

		if (pUser->GetUserState() == STATE_LOGIN)
			nLocation = _LOCATION_LOGIN;
		else if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_CHECKGAME)
			nLocation = _LOCATION_MOVE;
		else if (pUser->GetUserState() == STATE_VILLAGE)
		{
			nLocation = _LOCATION_VILLAGE;
			nChannelID = pUser->GetChannelID();
		}
		else if (pUser->GetUserState() == STATE_GAME)
		{
			nLocation = _LOCATION_GAME;
			nMapIdx = pUser->m_nMapIndex;
		}

		pCon->SendAddUserState((*ii).second->GetCharacterName(), (*ii).second->GetCharacterDBID(), nLocation, -1, nChannelID, nMapIdx );
	}
}

CDNVillageConnection * CDivisionManager::GetFirstVillageServer()
{
	if( m_VillageServerConList.size() > 0 )
	{
		return m_VillageServerConList[0];
	}
	return NULL;
}

CDNVillageConnection* CDivisionManager::GetFirstEnableVillageServer()
{
	if (m_VillageServerConList.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->GetActive() && (*ii)->GetConnectionCompleted())
			return (*ii);
	}
	return NULL;
}


int CDivisionManager::GetGameConnectionCount()
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	int nCount = 0;
	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetActive() && (*ii)->GetConnectionCompleted())
			nCount++;
	}
	return nCount;
}

CDNGameConnection * CDivisionManager::GetFreeGameConnection(int * pExpServerID, int nReqRoomType)
{
	if (m_GameServerConList.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_Sync);

	UINT nRoomCnt = 0;
	CDNGameConnection * pCon = NULL;
	
	static unsigned int nCnt = 0;
	int nTemp = 0;
	bool bCheck;
	int nCheckLevel;
	while (1)
	{
		bCheck = true;
		nCheckLevel = -1;
		pCon = m_GameServerConList[nCnt++%m_GameServerConList.size()];

#ifdef _FINAL_BUILD
		//파이날빌드에서는 농장서버는 농장만 할당되도록 한다.
		if (m_nFarmGameindex != -1 && pCon->GetGameID() == m_nFarmGameindex && m_GameServerConList.size() > 1)
			continue;
#endif

		if (pCon == NULL) continue;
		if ((nReqRoomType == REQINFO_TYPE_SINGLE || nReqRoomType == REQINFO_TYPE_PARTY || nReqRoomType == REQINFO_TYPE_FARM) && \
			(pCon->GetAffinityType() != _GAMESERVER_AFFINITYTYPE_NORMAL && pCon->GetAffinityType() != _GAMESERVER_AFFINITYTYPE_HYBRYD))
		{
			bCheck = false;
			nCheckLevel = 1;
		}
		else if ((nReqRoomType == REQINFO_TYPE_PVP || nReqRoomType == REQINFO_TYPE_PVP_BREAKINTO || nReqRoomType == REQINFO_TYPE_LADDER ) && (pCon->GetAffinityType() != _GAMESERVER_AFFINITYTYPE_PVP && pCon->GetAffinityType() != _GAMESERVER_AFFINITYTYPE_HYBRYD))
		{
			bCheck = false;
			nCheckLevel = 3;
		}
		if (pCon->GetConnectionCompleted() == false || pCon->GetZeroPopulation())
			bCheck = false;

		if(bCheck && pCon && pCon->GetHasMargin())
			break;

		//오픈초기에 특정 게임컨넥션에만 몰리는 경우가 있어서 로그추가
		if (m_GameServerConList.size() > 1 && nReqRoomType != REQINFO_TYPE_FARM)
			g_Log.Log(LogType::_ERROR, L"PassFreeConReason SID[%d] ReqType[%d] Zero[%s] Margin[%s] CL[%d] CC[%s]\n", pCon->GetManagedID(), nReqRoomType, pCon->GetZeroPopulation() == true ? L"True" : L"False", pCon->GetHasMargin() == true ? L"True" : L"False", nCheckLevel, pCon->GetConnectionCompleted() ? L"True" : L"False");

		if(nTemp++ == 100)
		{
			if (bCheck && pCon->GetZeroPopulation())
			{
				//check가 true상태에서 100번 확인이 끝나 그냥 검사부분이 무시되어지고 포인터가 넘어갈 수 있어 로그 추가
				g_Log.Log(LogType::_ZEROPOPULATION, L"GetFreeGameConnection if(nTemp++ == 100) SID[%d] Zero[%s] Margin[%s]\n", pCon->GetManagedID(), pCon->GetZeroPopulation() == true ? L"True" : L"False", pCon->GetHasMargin() == true ? L"True" : L"False");
				return NULL;
			}
			break;
		}

	}

	if( pCon )
	{
		pCon->SetLastReqEnterGameTick();
	}

	return pCon;
}

CDNGameConnection * CDivisionManager::GetGameConnectionByGameID(int nGameID)
{
	if (m_GameServerConList.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetGameID() == nGameID && (*ii)->GetConnectionCompleted())
			return (*ii);
	}

	return NULL;
}

CDNGameConnection * CDivisionManager::GetGameConnectionByManagedID(int nSID )
{
	if (m_GameServerConList.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetConnectionCompleted() && (*ii)->GetManagedID() == nSID)
			return (*ii);
	}

	return NULL;
}

CDNVillageConnection * CDivisionManager::GetVillageConnectionByVillageID(int nVillageID)
{
	if (m_VillageServerConList.empty()) return NULL;

	ScopeLock<CSyncLock> Lock(m_Sync);
	
	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->GetVillageID() == nVillageID && (*ii)->GetConnectionCompleted())
			return (*ii);
	}
	return NULL;
}

#ifdef PRE_MOD_OPERATINGFARM
bool CDivisionManager::IsFarmGameConnectionAvailable(DWORD dwCurTick)
{
	if (g_Config.nFarmServerID > 0 && IsFarmConnectionWatingTime(dwCurTick))
	{
		ScopeLock<CSyncLock> Lock(m_Sync);

		std::vector <CDNGameConnection*>::iterator ii;
		for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
		{
			if ((*ii)->GetActive() && (*ii)->GetConnectionCompleted() && (*ii)->GetAffinityType() == _GAMESERVER_AFFINITYTYPE_FARM)
				return true;
		}		
		return false;
	}
	else
	{
		return GetGameConnectionCount() <= 0 ? false : true;
	}
}

CDNGameConnection * CDivisionManager::GetFarmGameConnection(int * pExpServerID)
{
	//GetFreeGameConnection안쪽 만지기가 살짝 무서워서 새로 땀 ㅋ
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetActive() && (*ii)->GetConnectionCompleted() && ((*ii)->GetAffinityType() == _GAMESERVER_AFFINITYTYPE_FARM || (*ii)->GetManagedID() == g_Config.nFarmServerID))
			return (*ii);
	}		
	return NULL;
}
#endif		//#ifdef PRE_MOD_OPERATINGFARM

#if defined( PRE_FIX_WORLDCOMBINEPARTY )
CDNGameConnection * CDivisionManager::GetWorldCombineGameConnection()
{
	CDNGameConnection * pCon = NULL;

	for( int i=0;i<m_GameServerConList.size();i++)
	{
		pCon = m_GameServerConList[i];
		if( pCon->GetWorldCombineGameServer() )
			return pCon;
	}	
	return NULL;
}
#endif

void CDivisionManager::SendLoginServer(const int iMainCmd, const int iSubCmd, const char *cBuf, const unsigned short uLen)
{
	GetLoginConnection()->AddSendData(iMainCmd, iSubCmd, const_cast<char*>(cBuf), uLen);
}

void CDivisionManager::SendAllLoginServer(const int iMainCmd, const int iSubCmd, const char *cBuf, const unsigned short uLen)
{
	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection * pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon || !pLoginCon->GetActive()) {
			continue;
		}

		pLoginCon->AddSendData(iMainCmd, iSubCmd, const_cast<char*>(cBuf), uLen);
	}
}

void CDivisionManager::SendAllVillageServer(const int iMainCmd, const int iSubCmd, const char *cBuf, const unsigned short uLen)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	
	std::vector <CDNVillageConnection*>::iterator itor = m_VillageServerConList.begin();
	for (; itor != m_VillageServerConList.end() ; ++itor) {
		CDNVillageConnection* pVillageConnection = (*itor);
		if (pVillageConnection)
			pVillageConnection->AddSendData(iMainCmd, iSubCmd, const_cast<char*>(cBuf), uLen);
	}
}

void CDivisionManager::SendAllGameServer(const int iMainCmd, const int iSubCmd, const char *cBuf, const unsigned short uLen)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator itor = m_GameServerConList.begin();
	for (; itor != m_GameServerConList.end(); ++itor) {
		CDNGameConnection* pGameConnection = (*itor);
		if (pGameConnection)
			pGameConnection->AddSendData(iMainCmd, iSubCmd, const_cast<char*>(cBuf), uLen);
	}
}

CDNLoginConnection* CDivisionManager::GetFirstEnableLoginServer()
{
	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection * pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon || !pLoginCon->GetActive()) {
			continue;
		}
		return pLoginCon;
	}
	return NULL;
}

void CDivisionManager::SendAllLoginServerDetachUser(UINT nAccountDBID)
{
	MALODetachUser packet;
	memset(&packet, 0, sizeof(MALODetachUser));
	packet.nAccountDBID = nAccountDBID;

	SendAllLoginServer(MALO_DETACHUSER, 0, (char*)&packet, sizeof(packet));
}

void CDivisionManager::SendAllLoginServerVillageInfo(UINT nUserCnt, UINT nMaxUserCount, std::vector<TVillageInfo> * vList)
{
	std::vector <TVillageInfo>::iterator ii;
	for (ii = vList->begin(); ii != vList->end(); ii++)
	{
		MALOVillageInfo packet;
		memset(&packet, 0, sizeof(packet));

		packet.nVillageID = (*ii).cVillageID;
		packet.nWorldUserCount = nUserCnt;
		packet.nWorldMaxUserCount = nMaxUserCount;

		for (int j = 0; j < (int)(*ii).vOwnedChannelList.size(); j++)
		{
			packet.Info[packet.cCount].nChannelID = (*ii).vOwnedChannelList[j].nChannelID;
			packet.Info[packet.cCount].nChannelIdx = (*ii).vOwnedChannelList[j].nChannelIdx;
			packet.Info[packet.cCount].nMapIdx = (*ii).vOwnedChannelList[j].nMapIdx;
			packet.Info[packet.cCount].nCurrentUserCount = (*ii).vOwnedChannelList[j].nCurrentUser;
			packet.Info[packet.cCount].nMaxUserCount = (*ii).vOwnedChannelList[j].nChannelMaxUser;
			packet.Info[packet.cCount].nChannelAttribute = (*ii).vOwnedChannelList[j].nAttribute;
			packet.Info[packet.cCount].nMeritBonusID = (*ii).vOwnedChannelList[j].nMeritBonusID;
			if (packet.Info[packet.cCount].nMeritBonusID > 0)
			{
				const TMeritInfo * pInfo = g_pExtManager->GetMeritInfo(packet.Info[packet.cCount].nMeritBonusID);
				if (pInfo)
				{
					packet.Info[packet.cCount].cMinLevel = pInfo->nMinLevel;
					packet.Info[packet.cCount].cMaxLevel = pInfo->nMaxLevel;\
				}
				else
					_DANGER_POINT();
			}
			packet.Info[packet.cCount].bVisibility = (*ii).vOwnedChannelList[j].bVisibility;
			_strcpy(packet.Info[packet.cCount].szIP, _countof(packet.Info[packet.cCount].szIP), (*ii).szIP, (int)strlen((*ii).szIP));
			packet.Info[packet.cCount].nPort = (*ii).nPort;
			packet.Info[packet.cCount].nLimitLevel = (*ii).vOwnedChannelList[j].nLimitLevel;
			packet.Info[packet.cCount].bShow = (*ii).vOwnedChannelList[j].bShow;
			packet.Info[packet.cCount].nDependentMapID = (*ii).vOwnedChannelList[j].nDependentMapID;
#if defined(PRE_ADD_CHANNELNAME)
			_wcscpy(packet.Info[packet.cCount].wszLanguageName, _countof(packet.Info[packet.cCount].wszLanguageName), (*ii).vOwnedChannelList[j].wszLanguageName, (int)wcslen((*ii).vOwnedChannelList[j].wszLanguageName));
#endif //#if defined(PRE_ADD_CHANNELNAME)
			packet.cCount++;
		}
		SendAllLoginServer(MALO_VILLAGEINFO, 0, (char*)&packet, sizeof(MALOVillageInfo) - sizeof(packet.Info) + (sizeof(sChannelInfo) * packet.cCount));
	}
}

void CDivisionManager::SendAllLoginServerVillageInfoDelete(int nVillageID)
{
	MALOVillageInfoDelete packet;
	memset(&packet, 0, sizeof(packet));
	packet.nVillageID = nVillageID;

	SendAllLoginServer(MALO_VILLAGEINFODEL, 0, (char*)&packet, sizeof(packet));
}

bool CDivisionManager::GetVillageServerInfo(std::vector <TVillageInfo> * vList)
{
	if (m_VillageServerConList.empty()) return false;

	ScopeLock<CSyncLock> Lock(m_Sync);

	TVillageInfo Info;
	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->GetConnectionCompleted() == false)
			continue;
		TVillageInfo * pInfo = (*ii)->GetVillageChannelInfo();
		if (pInfo != NULL)
		{
			Info.nManagedID = pInfo->nManagedID;
			Info.cVillageID = pInfo->cVillageID;
			Info.nPort = pInfo->nPort;
			_strcpy(Info.szIP, _countof(Info.szIP), pInfo->szIP, (int)strlen(pInfo->szIP));
			Info.vOwnedChannelList = pInfo->vOwnedChannelList;
			vList->push_back(Info);
		}
	}

	return true;
}

bool CDivisionManager::GetVillageServerInfo(int nVillageID, TVillageInfo * pVillageInfo, const char * pszIP, short nPort)
{
	if (!pVillageInfo) return false;

	const TServerInfo * pInfo = g_pExtManager->GetServerInfoByWorldSetID(g_Config.nWorldSetID);
	if (pInfo != NULL)
	{
		for (int i = 0; i < (int)pInfo->vOwnedVillageList.size(); i++)
		{
			if (pInfo->vOwnedVillageList[i].cVillageID == nVillageID)
			{
				//같은게 이미 접속되어 있는지 한번 확인하자
				if (GetVillageConnectionByVillageID(nVillageID) == NULL)
				{
					pVillageInfo->cVillageID = pInfo->vOwnedVillageList[i].cVillageID;
#if defined (_FINAL_BUILD)		//파이날빌드까지 묶는이유는 21번때문에 ㅎㅎㅎㅎㅎ
					strcpy(pVillageInfo->szIP, pszIP);
					pVillageInfo->nPort = nPort;
#else
					strcpy(pVillageInfo->szIP, pInfo->vOwnedVillageList[i].szIP);
					pVillageInfo->nPort = pInfo->vOwnedVillageList[i].nPort;
#endif
					pVillageInfo->vOwnedChannelList = pInfo->vOwnedVillageList[i].vOwnedChannelList;
					return true;
				}
				else
					return false;
			}
		}
	}
	_DANGER_POINT();
	return false;
}

const TChannelInfo* CDivisionManager::GetChannelInfo( const int nVillageID, const int nChannelID )
{
	TVillageInfo * pInfo = NULL;
	std::vector <TChannelInfo>::iterator ih;
	CDNVillageConnection * pCon = GetVillageConnectionByVillageID(nVillageID);
	if (pCon)
	{
		pInfo = pCon->GetVillageChannelInfo();
		for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
			if ((*ih).nChannelID == nChannelID)
				return &(*ih);
	}
	return NULL;
}

const TChannelInfo * CDivisionManager::GetChannelInfo(int nChannelID)
{
	//sync버전입니다. 중복해서 싱크를 잡지 않도록 주의 요망!!!!
	TVillageInfo * pInfo = NULL;
	std::vector <CDNVillageConnection*>::iterator ii;
	std::vector <TChannelInfo>::iterator ih;

	ScopeLock<CSyncLock> Lock(m_Sync);

	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->GetConnectionCompleted() == false) continue;
		pInfo = (*ii)->GetVillageChannelInfo();
		for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
		{
			if ((*ih).nChannelID == nChannelID)
				return &(*ih);
		}
	}

	return NULL;
}

bool CDivisionManager::ChannelControl(int nChannelID, bool bVisibility, int nChannelPopulation)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		TVillageInfo * pInfo = (*ii)->GetVillageChannelInfo();
		if (pInfo)
		{
			std::vector <TChannelInfo>::iterator ih;
			for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
			{
				if ((*ih).nChannelID == nChannelID)
				{
					if (nChannelPopulation > 0)
						(*ih).nChannelMaxUser = nChannelPopulation;
					(*ih).bVisibility = bVisibility;
					return true;
				}
			}
		}
	}

	return false;
}

bool CDivisionManager::PopulationControl(int nServerType, int nManagedID, bool bZeroPopulation)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	if (nServerType == MANAGED_TYPE_VILLAGE)
	{
		std::vector <CDNVillageConnection*>::iterator iv;
		for (iv = m_VillageServerConList.begin(); iv != m_VillageServerConList.end(); iv++)
		{
			if ((*iv)->GetManagedID() > 0 && (*iv)->GetManagedID() == nManagedID)
			{
				TVillageInfo * pInfo = (*iv)->GetVillageChannelInfo();
				if (pInfo)
				{
					std::vector <TChannelInfo>::iterator io;
					for (io = pInfo->vOwnedChannelList.begin(); io != pInfo->vOwnedChannelList.end(); io++)
						(*io).bVisibility = false;

					return true;
				}
				break;
			}
		}
	}
	else if (nServerType == MANAGED_TYPE_GAME)
	{
		std::vector <CDNGameConnection*>::iterator ig;
		for (ig = m_GameServerConList.begin(); ig != m_GameServerConList.end(); ig++)
		{
			if ((*ig)->GetManagedID() == nManagedID)
			{
				(*ig)->SetZeroPopulation(bZeroPopulation);
				(*ig)->SendPopulationZero(bZeroPopulation);

				return true;
			}
		}
	}

	return false;
}

void CDivisionManager::SetGameConAffinityType(int nManagedID, BYTE cAffinityType)
{
	if (cAffinityType > 2) return;				//타입이 넘어가믄 안데
	if (m_GameServerConList.empty()) return;
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetManagedID() > 0 && (*ii)->GetManagedID() == nManagedID)
		{
			(*ii)->SetAffinityType(cAffinityType);
			return;
		}
	}
}

void CDivisionManager::UpdateUserRestraint(UINT nAccountDBID)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser)
	{
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendUserRestraint(nAccountDBID);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendUserRestraint(nAccountDBID);
		}
	}
}

bool CDivisionManager::GetGameInfoByID(int nGameID, int nGameServerID, char * pIP, USHORT * pPort, USHORT * pTcpPort)
{
	if (m_GameServerConList.empty()) return false;

	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if ((*ii)->GetGameID() == nGameID && (*ii)->GetConnectionCompleted())
		{
			if ((*ii)->GetGameServerInfoByID(nGameServerID, pPort, pIP, pTcpPort) == true)
				return true;
		}
	}

	return false;
}

bool CDivisionManager::GetVillageInfo(int nMapIdx, int nOriginChannelId, int &nChannelId, BYTE &cVillageID, char * pIP, USHORT &pPort)
{
	if (m_VillageServerConList.empty()) return false;

	const TChannelInfo * pChInfo = GetChannelInfo(nOriginChannelId);
	if (pChInfo == NULL) return false;
	
	ScopeLock<CSyncLock> Lock(m_Sync);

	TVillageInfo * pInfo = NULL;
	std::vector <CDNVillageConnection*>::iterator ii;
	std::vector <TChannelInfo>::iterator ih;

	if (pChInfo->nMapIdx == nMapIdx)
	{
		//원래 있던 곳으로 돌아가는 것
		for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		{
			if ((*ii)->GetConnectionCompleted() == false) continue;
			pInfo = (*ii)->GetVillageChannelInfo();
			for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
			{
				if ((*ih).nMapIdx == nMapIdx && (*ih).nChannelID == nOriginChannelId && (*ih).nChannelMaxUser > (*ih).nCurrentUser && (*ih).bVisibility)
				{
					nChannelId = (*ih).nChannelID;
					strcpy(pIP, (*ii)->GetVillageChannelInfo()->szIP);
					pPort = (*ii)->GetVillageChannelInfo()->nPort;
					cVillageID = (*ii)->GetVillageID();

					return true;
				}
			}
		}

		//원래 채널이 가득차서 못들어 가는 경우이다. 충족요건을 찾는다.
		for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		{
			if ((*ii)->GetConnectionCompleted() == false) continue;
			pInfo = (*ii)->GetVillageChannelInfo();
			for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
			{
				if ((*ih).nMapIdx == nMapIdx && (*ih).nChannelMaxUser >= (*ih).nCurrentUser && (*ih).bVisibility)
				{
					if (pChInfo->nAttribute == GlobalEnum::CHANNEL_ATT_MERITBONUS)
					{
						if ((*ih).nAttribute == pChInfo->nAttribute && (*ih).nMeritBonusID == pChInfo->nMeritBonusID)
						{
							nChannelId = (*ih).nChannelID;
							strcpy(pIP, (*ii)->GetVillageChannelInfo()->szIP);
							pPort = (*ii)->GetVillageChannelInfo()->nPort;
							cVillageID = (*ii)->GetVillageID();

							return true;
						}
					}
					else if ((*ih).nAttribute == pChInfo->nAttribute)
					{
						nChannelId = (*ih).nChannelID;
						strcpy(pIP, (*ii)->GetVillageChannelInfo()->szIP);
						pPort = (*ii)->GetVillageChannelInfo()->nPort;
						cVillageID = (*ii)->GetVillageID();

						return true;
					}
				}
			}
		}
	}
	else if (pChInfo->nAttribute == GlobalEnum::CHANNEL_ATT_MERITBONUS)
	{
		//전에 있던 채널이 메리트가 있던 곳이 었으면 메리트 조건으로 찾아본다
		const TMeritInfo * pMerit = g_pExtManager->GetMeritInfo(pChInfo->nMeritBonusID);
		if (pMerit)
		{
			int nLevel = (pMerit->nMaxLevel + pMerit->nMinLevel) / 2;
			int nLastLevel = 1000;
			CDNVillageConnection * pPickedCon = NULL;
			TChannelInfo * pPickedChInfo = NULL;

			for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
			{
				if ((*ii)->GetConnectionCompleted() == false) continue;
				pInfo = (*ii)->GetVillageChannelInfo();
				for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
				{
					if ((*ih).nAttribute == GlobalEnum::CHANNEL_ATT_MERITBONUS && (*ih).nMapIdx == nMapIdx && (*ih).nChannelMaxUser > (*ih).nCurrentUser && (*ih).bVisibility)
					{
						pMerit = g_pExtManager->GetMeritInfo((*ih).nMeritBonusID);
						if (pMerit)
						{
							int nAvrLv = (pMerit->nMinLevel + pMerit->nMaxLevel) / 2;
							int nGap = nAvrLv > nLevel ? nAvrLv - nLevel : nLevel - nAvrLv;

							if (nLastLevel > nGap)
							{
								nLastLevel = nGap;
								pPickedCon = (*ii);
								pPickedChInfo = &(*ih);
							}
						}
					}
				}
			}

			if (pPickedCon && pPickedChInfo)
			{
				nChannelId = pPickedChInfo->nChannelID;
				strcpy(pIP, pPickedCon->GetVillageChannelInfo()->szIP);
				pPort = pPickedCon->GetVillageChannelInfo()->nPort;
				cVillageID = pPickedCon->GetVillageID();

				return true;
			}
		}
	}

	//이도 저도 못찾았습니다. 적당히 남는데 넣어 줍니다.
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->GetConnectionCompleted() == false) continue;
		pInfo = (*ii)->GetVillageChannelInfo();
		for (ih = pInfo->vOwnedChannelList.begin(); ih != pInfo->vOwnedChannelList.end(); ih++)
		{
			if ((*ih).nMapIdx == nMapIdx && (*ih).nChannelMaxUser > (*ih).nCurrentUser && (*ih).bVisibility)
			{
				nChannelId = (*ih).nChannelID;
				strcpy(pIP, (*ii)->GetVillageChannelInfo()->szIP);
				pPort = (*ii)->GetVillageChannelInfo()->nPort;
				cVillageID = (*ii)->GetVillageID();

				return true;
			}
		}
	}

	return false;
}

void CDivisionManager::CheckPvPLobbyChannel( CDNVillageConnection* pVillageCon )
{
	for( UINT i=0 ; i<pVillageCon->GetVillageChannelInfo()->vOwnedChannelList.size() ; ++i )
	{
		if( pVillageCon->GetVillageChannelInfo()->vOwnedChannelList[i].nAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY )
		{
			m_cPvPLobbyVillageID	= pVillageCon->GetVillageID();
			m_unPvPLobbyChannelID	= pVillageCon->GetVillageChannelInfo()->vOwnedChannelList[i].nChannelID;
			m_nPvPLobbyMapIndex = pVillageCon->GetVillageChannelInfo()->vOwnedChannelList[i].nMapIdx;
			break;
		}
	}
}

void CDivisionManager::SendWaitProcess(int nServerID, MALOWaitUserProcess * pProcess)
{
	if (pProcess == NULL)
	{
		_DANGER_POINT();
		return;
	}

	ScopeLock<CSyncLock> Lock(m_Sync);

	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection *pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon)	continue;

		if (pLoginCon->GetServerID() == nServerID)
		{
			pLoginCon->SendWaitProcess(pProcess);
			return;
		}
	}
}

//UserState
void CDivisionManager::AddUserState(const WCHAR * pName, INT64 biChracterDBID)
{
	if (pName == NULL) return;
	if (wcslen(pName) <= 0 || pName[0] == '\0') return;

	ScopeLock<CSyncLock> Lock(m_Sync);
	
	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		(*ii)->SendAddUserState(pName, biChracterDBID);

	std::vector <CDNGameConnection*>::iterator ig;
	for (ig = m_GameServerConList.begin(); ig != m_GameServerConList.end(); ig++)
		(*ig)->SendAddUserState(pName, biChracterDBID);
}

void CDivisionManager::DelUserState(const WCHAR * pName, INT64 biChracterDBID)
{
	if (pName == NULL) return;
	if (wcslen(pName) <= 0) return;

	ScopeLock<CSyncLock> Lock(m_Sync);
	
	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		(*ii)->SendDelUserState(pName, biChracterDBID);

	std::vector <CDNGameConnection*>::iterator ig;
	for (ig = m_GameServerConList.begin(); ig != m_GameServerConList.end(); ig++)
		(*ig)->SendDelUserState(pName, biChracterDBID);
}

void CDivisionManager::UpdateUserState(const WCHAR * pName, INT64 biChracterDBID, int nLocationState, int nCommunityState, int nChannelID, int nMapIdx, CDNGameConnection * pCon)
{
	if (pName == NULL) return;
	if (wcslen(pName) <= 0 || pName[0] == '\0') return;

	//loginserver 제외된 master에 연결된 모든 서버에 전달
	ScopeLock<CSyncLock> Lock(m_Sync);
	
	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		(*ii)->SendUpdateUserState(pName, biChracterDBID, nLocationState, nCommunityState, nChannelID, nMapIdx);

	std::vector <CDNGameConnection*>::iterator ig;
	for (ig = m_GameServerConList.begin(); ig != m_GameServerConList.end(); ig++)
		if (!(pCon && pCon == (*ig)))
			(*ig)->SendUpdateUserState(pName, biChracterDBID, nLocationState, nCommunityState, nChannelID, nMapIdx);
}

UINT CDivisionManager::GetCurUserCount()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	return (UINT)m_AccountDBIDList.size();
}

#if defined(PRE_MOD_SELECT_CHAR)
void CDivisionManager::_AddUserList(CDNUser *pUser)
{
	m_AccountDBIDList[pUser->GetAccountDBID()] = pUser;
	m_SessionIDList[pUser->GetSessionID()] = pUser;
	m_CharacterDBIDList[pUser->GetCharacterDBID()]	= pUser;
	m_UserNameList[pUser->GetCharacterName()] = pUser;
}

int CDivisionManager::AddUser(LOMAAddUser *pPacket, int nServerID)
{
	if (IsExistUser(pPacket->nAccountDBID))
		return ERROR_GENERIC_DUPLICATEUSER;

	if (pPacket->cAccountLevel <= 0)		//일반유저가 아니라면 대기열과 상관없이 진입가능
	{
		if(!pPacket->bForce && (g_pWaitUserManager->GetWorldMaxUser() <= m_AccountDBIDList.size() || g_pWaitUserManager->GetWaitUserCount() > 0)) 
			return ERROR_GENERIC_WORLDFULLY_CAPACITY;
	}

	CDNUser *pUser = new CDNUser;
	if (!pUser){
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	if (!pUser->InitUser(pPacket, nServerID))
	{
		SAFE_DELETE(pUser);
		return ERROR_GENERIC_UNKNOWNERROR;
	}

	AddUserState(pPacket->wszCharacterName, pPacket->biCharacterDBID);	//유저스테이트추가합니다 (모든서버 로긴제외)
	UpdateUserState(pPacket->wszCharacterName, pPacket->biCharacterDBID, _LOCATION_MOVE, _COMMUNITY_NONE);

	_AddUserList(pUser);

#if !defined( STRESS_TEST )
	g_Log.Log(LogType::_NORMAL, pUser, L"[AddUser:%d] ADBID:%u, CDBID:%I64d, SID:%u (%s:%s) Adult:%d\r\n", 
		m_SessionIDList.size(), pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID, pPacket->wszAccountName, pPacket->wszCharacterName, pPacket->bAdult);
#endif

	return ERROR_NONE;
}

bool CDivisionManager::AddUser(GAMAAddUserList *pPacket, int nGameID, bool bIsForceAdd/* = false*/)
{
	if (IsExistUser(pPacket->nAccountDBID))
		return false;

	CDNUser *pUser = new CDNUser;
	if (!pUser){
		_DANGER_POINT();
		return false;
	}

	if (!pUser->InitUser(pPacket))
		return false;

	AddUserState(pPacket->wszCharacterName, pPacket->biCharacterDBID);	//유저스테이트추가합니다 (모든서버 로긴제외)	

	if (bIsForceAdd == false)
	{
#if defined( STRESS_TEST )
		_AddUserList(pUser);
		return true;
#endif // #if defined( STRESS_TEST )

		_ASSERT(0);
		SAFE_DELETE(pUser);
		return false;
	}
	else
	{
		if (nGameID > 0)
		{
			pUser->SetGameInfo(nGameID, pPacket->nThreadIdx, true);
			pUser->m_VillageCheckPartyID = pPacket->PartyID;
			UpdateUserState(pPacket->wszCharacterName, pPacket->biCharacterDBID, _LOCATION_GAME, _COMMUNITY_NONE, -1, pUser->m_nMapIndex);
		}
	}

	_AddUserList(pUser);

#if !defined( STRESS_TEST )
	g_Log.Log(LogType::_NORMAL, pUser, L"[AddUser:%d] ADBID:%u, CDBID:%I64d, SID:%u (%s:%s) Adult:%d\r\n",
		m_SessionIDList.size(), pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID, pPacket->wszAccountName, pPacket->wszCharacterName, pPacket->bAdult);
#endif

	return true;
}

bool CDivisionManager::AddUser(VIMAAddUserList *pPacket, int nVillageID, bool bIsForceAdd/* = false*/)
{
	if (IsExistUser(pPacket->nAccountDBID))
		return false;

	CDNUser *pUser = new CDNUser;
	if (!pUser){
		_DANGER_POINT();
		return false;
	}

	if (!pUser->InitUser(pPacket))
		return false;

	AddUserState(pPacket->wszCharacterName, pPacket->biCharacterDBID);	//유저스테이트추가합니다 (모든서버 로긴제외)	

	if (bIsForceAdd == false)
	{
#if defined( STRESS_TEST )
		_AddUserList(pUser);
		return true;
#endif // #if defined( STRESS_TEST )

		_ASSERT(0);
		SAFE_DELETE(pUser);
		return false;
	}
	else
	{
		if (nVillageID > 0)
		{
			pUser->SetVillageInfo(nVillageID, pPacket->wChannelID, true);

			const TChannelInfo * pChannelInfo = GetChannelInfo(pPacket->wChannelID);
			if (pChannelInfo)
				UpdateUserState(pPacket->wszCharacterName, pPacket->biCharacterDBID, _LOCATION_VILLAGE, _COMMUNITY_NONE, pChannelInfo->nChannelIdx, pChannelInfo->nMapIdx);
		}
	}

	_AddUserList(pUser);

#if !defined( STRESS_TEST )
	g_Log.Log(LogType::_NORMAL, pUser, L"[AddUser:%d] ADBID:%u, CDBID:%I64d, SID:%u (%s:%s) Adult:%d\r\n",
		m_SessionIDList.size(), pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->nSessionID, pPacket->wszAccountName, pPacket->wszCharacterName, pPacket->bAdult);
#endif

	return true;
}

#else	// #if defined(PRE_MOD_SELECT_CHAR)
int CDivisionManager::AddUser(int nServerID, UINT nAccountDBID, const BYTE * pMachineID, DWORD dwGRC, const char * pszIP, char cSelectedLanguage, BYTE cAccountLevel, bool bForce/*=false*/ )
{
	if (IsExistUser(nAccountDBID)) return ERROR_GENERIC_DUPLICATEUSER;

	if (cAccountLevel <= 0)		//일반유저가 아니라면 대기열과 상관없이 진입가능
	{
		if( !bForce && (g_pWaitUserManager->GetWorldMaxUser() <= m_AccountDBIDList.size() || g_pWaitUserManager->GetWaitUserCount() > 0)) 
			return ERROR_GENERIC_WORLDFULLY_CAPACITY;
	}

	CDNUser * pUser = new CDNUser;
	if (!pUser){
		_DANGER_POINT();
		return ERROR_GENERIC_UNKNOWNERROR;
	}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	pUser->PreInit(nServerID, nAccountDBID, cAccountLevel, pszIP);
#else		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	pUser->PreInit(nServerID, nAccountDBID, cAccountLevel, NULL);
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#ifdef PRE_ADD_MULTILANGUAGE
	pUser->m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

#if defined (_KR)
	if (pMachineID) memcpy(pUser->m_szMID, pMachineID, sizeof(pUser->m_szMID));
	pUser->m_dwGRC = dwGRC;
#endif

	m_AccountDBIDList[nAccountDBID] = pUser;
	g_Log.Log(LogType::_NORMAL, pUser, L"[AddUserList:%d] ADBID(%d)\n", (UINT)m_AccountDBIDList.size(), nAccountDBID);

	return ERROR_NONE;
}

bool CDivisionManager::SetUserInfo(LOMAUserInfo *pUserInfo, int nGameID, int nGameThreadIdx, BYTE * pMachineID, DWORD &dwGRC)
{
	CDNUser * pUser = GetUserByAccountDBID(pUserInfo->nAccountDBID);
	if (pUser == NULL)
	{
		_DANGER_POINT();
		return false;
	}

	pUser->Init(pUserInfo->nAccountDBID, pUserInfo->nSessionID, pUserInfo->biCharacterDBID, pUserInfo->wszCharacterName, pUserInfo->wszAccountName, pUserInfo->nChannelID, pUserInfo->bAdult, pUserInfo->szIP, pUserInfo->szVirtualIp);
	pUser->SetPCGrade(pUserInfo->cPCBangGrade);
#if defined(_US)
	pUser->m_nNexonUserNo = pUserInfo->nNexonUserNo;
#endif	// _US
#if defined (_KR)
	memcpy(pMachineID, pUser->m_szMID, sizeof(pUser->m_szMID));
	dwGRC = pUser->m_dwGRC;
#endif

	bool bMoveServerCheck = false;
	if (pUserInfo->cVillageID > 0)
		bMoveServerCheck = pUser->SetCheckVillageInfo(pUserInfo->cVillageID, pUserInfo->nChannelID);
	else if (nGameID > 0)
		bMoveServerCheck = pUser->SetCheckGameInfo(nGameID, nGameThreadIdx);

	if (bMoveServerCheck == false)
		return false;

	AddUserState(pUserInfo->wszCharacterName, pUserInfo->biCharacterDBID);	//유저스테이트추가합니다 (모든서버 로긴제외)
	UpdateUserState(pUserInfo->wszCharacterName, pUserInfo->biCharacterDBID, _LOCATION_MOVE, _COMMUNITY_NONE);
	
	m_SessionIDList[pUserInfo->nSessionID] = pUser;
	m_UserNameList[pUser->GetCharacterName()] = pUser;
	m_CharacterDBIDList[pUser->GetCharacterDBID()]	= pUser;

#if defined(_CH) && defined(_FINAL_BUILD)
	pUser->FCMIDOnline();	// 피로도 online 
#endif	// _CH

#if defined(_ID)
	_strcpy(pUser->m_szMacAddress, _countof(pUser->m_szMacAddress),  pUserInfo->szMacAddress, (int)strlen( pUserInfo->szMacAddress));
	_strcpy(pUser->m_szKey, _countof(pUser->m_szKey),  pUserInfo->szKey, (int)strlen( pUserInfo->szKey));
	pUser->m_dwKreonCN = pUserInfo->dwKreonCN;
#endif //#if defined(_ID)

#if defined(PRE_ADD_DWC)
	pUser->SetCharacterAccountLevel(pUserInfo->cCharacterAccountLevel);
#endif

#if !defined( STRESS_TEST )
	g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, pUserInfo->nAccountDBID, pUserInfo->biCharacterDBID, pUserInfo->nSessionID, 
		L"[AddUser:%d] ADBID:%u, CDBID:%I64d, SID:%u (%s:%s) Adult:%d\r\n", m_SessionIDList.size(), pUserInfo->nAccountDBID, pUserInfo->biCharacterDBID, pUserInfo->nSessionID, pUserInfo->wszAccountName, pUserInfo->wszCharacterName, pUserInfo->bAdult);
#endif

	return true;
}

bool CDivisionManager::AddUser(int nVillageID, int nChannelID, int nGameID, int nGameThreadIdx, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, WCHAR *pwszCharacterName, WCHAR *pwszAccountName,
#if defined(PRE_ADD_DWC)
							   BYTE cAccountLevel,
#endif
							   bool bAdult, char *pszIp, char *pszVirtualIp, TPARTYID PartyID, const BYTE * pMachineID, DWORD dwGRC, BYTE cPcBangGrade, char cSelectedLanguage, bool bIsForceAdd)
{
	if (IsExistUser(nAccountDBID)) return false;

	CDNUser * pUser = new CDNUser;
	if (!pUser){
		_DANGER_POINT();
		return false;
	}

	pUser->Init(nAccountDBID, nSessionID, biCharacterDBID, pwszCharacterName, pwszAccountName, nChannelID, bAdult, pszIp, pszVirtualIp);
#if defined (_KR)
	if (pMachineID) memcpy(pUser->m_szMID, pMachineID, sizeof(pUser->m_szMID));
	pUser->m_dwGRC = dwGRC;
#endif
	pUser->SetPCGrade(cPcBangGrade);
#ifdef PRE_ADD_MULTILANGUAGE
	pUser->m_eSelectedLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(cSelectedLanguage);
#endif		//#ifdef PRE_ADD_MULTILANGUAGE

	AddUserState(pwszCharacterName, biCharacterDBID);	//유저스테이트추가합니다 (모든서버 로긴제외)	
	if (bIsForceAdd == false)
	{
#if defined( STRESS_TEST )
		m_AccountDBIDList[nAccountDBID] = pUser;
		m_SessionIDList[nSessionID] = pUser;
		m_CharacterDBIDList[biCharacterDBID] = pUser;
		m_UserNameList[pUser->GetCharacterName()] = pUser;
		return true;
#endif // #if defined( STRESS_TEST )
		_ASSERT(0);
		SAFE_DELETE( pUser );
		return false;
	}
	else
	{
		if (nVillageID > 0)
		{
			pUser->SetVillageInfo(nVillageID, nChannelID, true);

			const TChannelInfo * pChannelInfo = GetChannelInfo(nChannelID);
			if (pChannelInfo)
				UpdateUserState(pwszCharacterName, biCharacterDBID, _LOCATION_VILLAGE, _COMMUNITY_NONE, pChannelInfo->nChannelIdx, pChannelInfo->nMapIdx);
		}
		else if (nGameID > 0)
		{
			pUser->SetGameInfo(nGameID, nGameThreadIdx, true);
			pUser->m_VillageCheckPartyID = PartyID;
			UpdateUserState(pwszCharacterName, biCharacterDBID, _LOCATION_GAME, _COMMUNITY_NONE, -1, pUser->m_nMapIndex);
		}
	}

	m_AccountDBIDList[nAccountDBID] = pUser;
	m_SessionIDList[nSessionID] = pUser;
	m_CharacterDBIDList[biCharacterDBID] = pUser;
	m_UserNameList[pUser->GetCharacterName()] = pUser;

#if defined(_CH) && defined(_FINAL_BUILD)
	pUser->FCMIDOnline();	// 피로도 online 
#endif	// _CH

#if !defined( STRESS_TEST )
	g_Log.Log(LogType::_NORMAL, g_Config.nWorldSetID, nAccountDBID, biCharacterDBID, nSessionID, L"[AddUser:%d] ADBID:%u, CDBID:%I64d, SID:%u (%s:%s) Adult:%d\r\n", m_SessionIDList.size(), nAccountDBID, biCharacterDBID, nSessionID, pwszAccountName, pwszCharacterName, bAdult);
#endif

	return true;
}

#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined (_JP) && defined (WIN64)
bool CDivisionManager::SetNHNNetCafeInfo(UINT nAccountDBID, bool bNetCafe, const char * pszNetCafeCode, const char * pszProdectCode)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);

	if (pUser)
	{
		pUser->SetPCBang(bNetCafe);
		memcpy_s(pUser->m_szNHNNetCafeCode, sizeof(pUser->m_szNHNNetCafeCode), pszNetCafeCode, NHNNETCAFECODEMAX);
		memcpy_s(pUser->m_szNHNProdectCode, sizeof(pUser->m_szNHNProdectCode), pszProdectCode, NHNNETCAFECODEMAX);
		return true;
	}	

	return false;
}
#endif

bool CDivisionManager::DelUser(UINT nAccountDBID, bool bIsReconnectLogin, UINT nSessionID)
{
#if defined( STRESS_TEST )
	return true;
#endif

	std::map <UINT, CDNUser*>::iterator ii = m_AccountDBIDList.find(nAccountDBID);
	if (ii != m_AccountDBIDList.end())
	{
		CDNUser * pUser = (*ii).second;
		if( nSessionID > 0 && nSessionID != pUser->GetSessionID()) // SessionID 체크
		{
			g_Log.Log(LogType::_NORMAL, pUser, L"DelUser SessionID not Match!! SessionID:%u, DelSessionID:%u\r\n", pUser->GetSessionID(), nSessionID );
			return false;
		}
		// PvP 처리
		LeavePvPRoom( nAccountDBID, false );
		LeaveLadderSystem( nAccountDBID );

#if defined(_KR) && defined(_FINAL_BUILD)	// pc bang
		g_pNexonAuth->SendLogout(pUser);

#elif defined(_US) && defined(_FINAL_BUILD)
		g_pNexonPI->RequestLogoff(pUser->m_nNexonUserNo, pUser->GetIp());

#elif defined(_CH) && defined(_FINAL_BUILD)
		pUser->FCMIDOffline();	// 피로도 offline 

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
		g_pNHNNetCafe->NetCafe_UserLogOut(pUser->GetAccountNameA());

#elif defined (_TW) && defined(_FINAL_BUILD)
		if( !bIsReconnectLogin && strlen(pUser->GetAccountNameA()) != 0 ) // 캐릭터 선택창으로 돌아가는 경우는 LogOut 날리지 않기.
		{
			ScopeLock<CSyncLock> Lock(pUser->m_SendLogOutLock);
			if( !pUser->m_bSendLogOut )
			{
				g_pGamaniaAuthLogOut->SendLogout(pUser->GetAccountNameA(), pUser->GetIp());
				pUser->m_bSendLogOut = true;
				g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut DelUser %s, %d, State:%d\r\n", pUser->GetAccountName(), pUser->GetSessionID(), pUser->GetUserState() );
			}
		}
#elif defined(_TH) && defined(_FINAL_BUILD)
		if( pUser )		
			g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 2, pUser->GetAccountDBID());
#endif	// _KR _CH _JP _TW
#ifdef PRE_ADD_STEAM_USERCOUNT
		if (pUser && pUser->m_bSteamUser)
			DecreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

		eUserState State = pUser->GetUserState();
		if (State != STATE_NONE)
		{
			DelUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID());
		}
		EraseUserListExceptAccountDBList(pUser);
		m_AccountDBIDList.erase(ii);
		if (bIsReconnectLogin == false)
		{
			g_pWaitUserManager->UpdateCurCount((UINT)m_AccountDBIDList.size());
		}
		return true;
	}

#if !defined( STRESS_TEST )
	_DANGER_POINT();
#endif

	return false;
}

bool CDivisionManager::IsExistUser(UINT nAccountDBID)
{
	if (m_AccountDBIDList.empty()) return false;
	std::map <UINT, CDNUser*>::iterator ii = m_AccountDBIDList.find(nAccountDBID);
	if (ii != m_AccountDBIDList.end())
	{
		//test
		ULONG CreateTick = (*ii).second->m_nCreateTick;
		ULONG nCUrTick = timeGetTime();
		return true;
	}
	return false;
}

CDNUser * CDivisionManager::GetUserByAccountDBID(UINT nAccountDBID)
{
	if (m_AccountDBIDList.empty()) return NULL;

	std::map <UINT, CDNUser*>::iterator ii = m_AccountDBIDList.find(nAccountDBID);
	if (ii != m_AccountDBIDList.end())
		return (*ii).second;
	//g_Log.Log(LogType::_ERROR, g_Config.nWorldSetID, nAccountDBID, 0, 0, L"GetUserByAccountDBID Failed ADBID[%d]\n", nAccountDBID);
	return NULL;
}

CDNUser * CDivisionManager::GetUserBySessionID(UINT nSessionID)
{
	if (m_SessionIDList.empty()) return NULL;

	std::map <UINT, CDNUser*>::iterator ii = m_SessionIDList.find(nSessionID);
	if (ii != m_SessionIDList.end())
		return (*ii).second;
	return NULL;
}

CDNUser * CDivisionManager::GetUserByCharacterDBID( INT64 biCharacterDBID )
{
	if (m_CharacterDBIDList.empty()) return NULL;

	std::map <INT64, CDNUser*>::iterator ii = m_CharacterDBIDList.find(biCharacterDBID);
	if (ii != m_CharacterDBIDList.end())
		return (*ii).second;
	return NULL;
}

CDNUser * CDivisionManager::GetUserByName(const WCHAR * pName)
{
	if (m_UserNameList.empty()) return NULL;

	std::map <std::wstring, CDNUser*>::iterator ii = m_UserNameList.find(pName);
	if (ii != m_UserNameList.end())
		return (*ii).second;
	return NULL;
}

CDNUser *CDivisionManager::GetUserByAccountName(const WCHAR *pAccountName)
{
	if (m_SessionIDList.empty()) return NULL;

	std::map <UINT, CDNUser*>::iterator iter;
	for (iter = m_SessionIDList.begin(); iter != m_SessionIDList.end(); ++iter){
		if (__wcsicmp_l(iter->second->GetAccountName(), pAccountName) == 0)
			return iter->second;
	}

	return NULL;
}

#if defined(_KR)
CDNUser *CDivisionManager::GetUserByNexonSessionNo(INT64 biSessionNo)
{
	if (m_SessionIDList.empty()) return NULL;

	std::map <UINT, CDNUser*>::iterator iter;
	for (iter = m_SessionIDList.begin(); iter != m_SessionIDList.end(); ++iter){
		if (iter->second->m_biNexonSessionNo == biSessionNo)
			return iter->second;
	}

	return NULL;
}
#endif	// #if defined(_KR)

void CDivisionManager::DetachUserloginID(int nServerID)
{
	if (m_AccountDBIDList.empty()) return;

	UINT nSessionID = 0;
	WCHAR wszCharName[NAMELENMAX] = { 0, };

	map <UINT, CDNUser*>::iterator iter;
	CDNUser *pUser = NULL;
	for (iter = m_AccountDBIDList.begin(); iter != m_AccountDBIDList.end(); ){
		if ((iter->second->GetUserState() == STATE_LOGIN || iter->second->GetUserState() == STATE_NONE) && (iter->second->GetLoginServerID() == nServerID)){
			pUser = iter->second;

#if defined(_KR) && defined(_FINAL_BUILD)	// pc bang
			g_pNexonAuth->SendLogout(pUser);

#elif defined(_CH) && defined(_FINAL_BUILD)
			pUser->FCMIDOffline();	// 피로도 offline 

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
			g_pNHNNetCafe->NetCafe_UserLogOut(pUser->GetAccountNameA());
#elif defined(_TH) && defined(_FINAL_BUILD)
			if( pUser )	
				g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 2, pUser->GetAccountDBID());
#endif	// _KR _CH _JP 
#ifdef PRE_ADD_STEAM_USERCOUNT
			if (pUser && pUser->m_bSteamUser)
				DecreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

			DelUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID());
			EraseUserListExceptAccountDBList(pUser);
			m_AccountDBIDList.erase(iter++);
		}
		else 
			++iter;
	}
}

void CDivisionManager::DetachUserByVillageID(BYTE cVillageID)
{
	if (m_AccountDBIDList.empty()) return;

	UINT nSessionID = 0;
	WCHAR wszCharName[NAMELENMAX] = { 0, };

	map <UINT, CDNUser*>::iterator iter;

	CDNUser *pUser = NULL;
	for (iter = m_AccountDBIDList.begin(); iter != m_AccountDBIDList.end(); ){
		if ((iter->second->GetUserState() == STATE_VILLAGE || iter->second->GetUserState() == STATE_CHECKVILLAGE) && (iter->second->GetVillageID() == cVillageID)){
			pUser = iter->second;

#if defined(_KR) && defined(_FINAL_BUILD)	// pc bang
			g_pNexonAuth->SendLogout(pUser);

#elif defined(_CH) && defined(_FINAL_BUILD)
			pUser->FCMIDOffline();	// 피로도 offline 

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
			g_pNHNNetCafe->NetCafe_UserLogOut(pUser->GetAccountNameA());

#elif defined (_TW) && defined(_FINAL_BUILD)
			if( pUser->GetAccountDBID() != 0 && strlen(pUser->GetAccountNameA()) !=0 )
			{
				ScopeLock<CSyncLock> Lock(pUser->m_SendLogOutLock);
				if( !pUser->m_bSendLogOut )
				{
					g_pGamaniaAuthLogOut->SendLogout(pUser->GetAccountNameA(), pUser->GetIp());
					pUser->m_bSendLogOut = true;
					g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut DetachUserByVillageID %s, %d\r\n", pUser->GetAccountName(), pUser->GetSessionID() );
				}
			}				
#elif defined(_TH) && defined(_FINAL_BUILD)
			if( pUser )		
				g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 2, pUser->GetAccountDBID());
#endif	// _KR _CH _JP _TW
#ifdef PRE_ADD_STEAM_USERCOUNT
			if (pUser && pUser->m_bSteamUser)
				DecreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

			DelUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID());
			EraseUserListExceptAccountDBList(pUser);
			m_AccountDBIDList.erase(iter++);
		}
		else 
			++iter;
	}
}

void CDivisionManager::DetachUserByGameID(USHORT wGameID)
{
	if (m_AccountDBIDList.empty()) return;

	UINT nSessionID = 0;
	WCHAR wszCharName[NAMELENMAX] = { 0, };

	map <UINT, CDNUser*>::iterator iter;

	CDNUser *pUser = NULL;
	for (iter = m_AccountDBIDList.begin(); iter != m_AccountDBIDList.end(); ){
		if ((iter->second->GetUserState() == STATE_GAME || iter->second->GetUserState() == STATE_CHECKGAME) && (iter->second->GetGameID() == wGameID)){
			pUser = iter->second;

#if defined(_KR) && defined(_FINAL_BUILD)	// pc bang
			g_pNexonAuth->SendLogout(pUser);

#elif defined(_CH) && defined(_FINAL_BUILD)
			pUser->FCMIDOffline();	// 피로도 offline 

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)

			g_pNHNNetCafe->NetCafe_UserLogOut(pUser->GetAccountNameA());

#elif defined (_TW) && defined(_FINAL_BUILD)
			if( pUser->GetAccountDBID() != 0 && strlen(pUser->GetAccountNameA()) !=0 )
			{
				ScopeLock<CSyncLock> Lock(pUser->m_SendLogOutLock);
				if( !pUser->m_bSendLogOut )
				{
					g_pGamaniaAuthLogOut->SendLogout(pUser->GetAccountNameA(), pUser->GetIp());
					pUser->m_bSendLogOut = true;
					g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut DetachUserByGameID %s, %d\r\n", pUser->GetAccountName(), pUser->GetSessionID() );
				}
			}	
#elif defined(_TH) && defined(_FINAL_BUILD)
			if( pUser )		
				g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 2, pUser->GetAccountDBID());
#endif	// _KR _CH _JP _TW
#ifdef PRE_ADD_STEAM_USERCOUNT
			if (pUser && pUser->m_bSteamUser)
				DecreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

			DelUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID());
			EraseUserListExceptAccountDBList(pUser);
			m_AccountDBIDList.erase(iter++);
		}
		else 
			++iter;
	}
}

void CDivisionManager::EraseUserListExceptAccountDBList(CDNUser *pUser)
{
	LeavePvPRoom( pUser->GetAccountDBID(), false );

	map<UINT, CDNUser*>::iterator ii = m_SessionIDList.find(pUser->GetSessionID());
	if (ii != m_SessionIDList.end())
		m_SessionIDList.erase(ii);

	std::map <std::wstring, CDNUser*>::iterator iter = m_UserNameList.find(pUser->GetCharacterName());
	if (iter != m_UserNameList.end())
		m_UserNameList.erase(iter);

	map<INT64, CDNUser*>::iterator itor = m_CharacterDBIDList.find( pUser->GetCharacterDBID() );
	if( itor != m_CharacterDBIDList.end() )
		m_CharacterDBIDList.erase( itor );

	g_Log.Log(LogType::_NORMAL, pUser, L"[EraseUserList:%d] A(%d, %s) C(%I64d, %s)\r\n", m_SessionIDList.size(), pUser->GetAccountDBID(), pUser->GetAccountName(), pUser->GetCharacterDBID(), pUser->GetCharacterName());

	SAFE_DELETE(pUser);
}

void CDivisionManager::ReplaceCharacterName(CDNUser* pUser, WCHAR* wszCharacterName)
{
	std::map <std::wstring, CDNUser*>::iterator iter = m_UserNameList.find(pUser->GetCharacterName());
	if (iter != m_UserNameList.end())
	{
		m_UserNameList.erase(iter);

		pUser->SetCharacterName (wszCharacterName);

		m_UserNameList[pUser->GetCharacterName()] = pUser;
	}
}

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
bool CDivisionManager::DetachUserByIP(const char * pszIP)
{
	if (pszIP == NULL)
		return false;

	g_Log.Log(LogType::_RESTRICTIP, L"DetachUserByIP Detect IP[%S]\n", pszIP);

	std::vector <UINT> vList;
	std::map <UINT, CDNUser*>::iterator iu;
	for (iu = m_AccountDBIDList.begin(); iu != m_AccountDBIDList.end(); iu++)
	{
		if (!stricmp((*iu).second->GetIp(), pszIP))
			vList.push_back((*iu).second->GetAccountDBID());
	}

	for (std::vector <UINT>::iterator vlistItor = vList.begin(); vlistItor != vList.end(); vlistItor++)
	{
		SendDetachUser((*vlistItor));
	}
	return true;
}
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#ifdef PRE_ADD_STEAM_USERCOUNT
void CDivisionManager::IncreaseSteamUser()
{
	m_nSteamUserCount++;
}

void CDivisionManager::DecreaseSteamUser()
{
	m_nSteamUserCount--;
}
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

bool CDivisionManager::VillageCheckUser(CDNVillageConnection * pCon, UINT nSessionID)
{
	CDNUser * pUser = GetUserBySessionID(nSessionID);
	if (pUser != NULL)
	{
		if (pCon->GetVillageID() != pUser->GetVillageID() || pUser->GetUserState() != STATE_CHECKVILLAGE || pUser->GetChannelID() <= 0)
		{
			g_Log.Log(LogType::_NORMAL, L"VillageCheckUser StateFail [%d] VID[%d : %d] UserState[%d] UserChannelID[%d]\n", nSessionID, pCon->GetVillageID(), pUser->GetVillageID(), pUser->GetUserState(), pUser->GetChannelID());
		}

		pCon->SendCheckUser(pUser, 0, ERROR_NONE);
		return true;
	}

	pCon->SendCheckUser(NULL, nSessionID, ERROR_GENERIC_USER_NOT_FOUND);
	g_Log.Log(LogType::_NORMAL, L"VillageCheckUser Fail UserNotFound[%d]\n", nSessionID);
	return false;
}

void CDivisionManager::CheckZombie(ULONG nCurTick)
{
	if (m_AccountDBIDList.empty()) return;

	std::map <UINT, CDNUser*>::iterator ii;
	for (ii = m_AccountDBIDList.begin(); ii != m_AccountDBIDList.end();)
	{
		CDNUser * pUser = (*ii).second;
		if (pUser->GetCheckStateTick() > 0 && \
			(pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_CHECKVILLAGE || \
			pUser->GetUserState() == STATE_NONE || pUser->GetUserState() == STATE_CHECKRECONNECTLOGIN) &&
			((pUser->GetCheckStateTick() + CHECK_ZOMBIE_DISCONNECT_TICK) < nCurTick))
		{
			//어디에 남아있을 지 모르므로 일단 전부 날려준다.
			CDNVillageConnection * pVilCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVilCon){
				pVilCon->SendDetachUser(pUser->GetAccountDBID());
				g_Log.Log(LogType::_NORMAL, pUser, L"[ADBID:%u CDBID:%I64d SID:%u] SendDetachUser(CheckZombie):%d (VillageID:%d)\r\n", pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), pUser->GetUserState(), pUser->GetVillageID());
			}

			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon){
				pGameCon->SendDetachUser(pUser->GetAccountDBID());
				g_Log.Log(LogType::_NORMAL, pUser, L"[ADBID:%u CDBID:%I64d SID:%u] SendDetachUser(CheckZombie):%d (GameID:%d)\r\n", pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), pUser->GetUserState(), pUser->GetGameID());
			}

#if defined(_KR) && defined(_FINAL_BUILD)	// pc bang
			g_pNexonAuth->SendLogout(pUser);

#elif defined(_CH) && defined(_FINAL_BUILD)
			pUser->FCMIDOffline();	// 피로도 offline 

#elif defined (_JP) && defined(_FINAL_BUILD) && defined (WIN64)
			g_pNHNNetCafe->NetCafe_UserLogOut(pUser->GetAccountNameA());

#elif defined (_TW) && defined(_FINAL_BUILD)
			if( pUser->GetAccountDBID() != 0 && strlen(pUser->GetAccountNameA()) !=0 )
			{
				ScopeLock<CSyncLock> Lock(pUser->m_SendLogOutLock);
				if( !pUser->m_bSendLogOut )
				{
					g_pGamaniaAuthLogOut->SendLogout(pUser->GetAccountNameA(), pUser->GetIp());
					pUser->m_bSendLogOut = true;
					g_Log.Log(LogType::_FILELOG, L"[GASH] LogOut CheckZombie %s, %d\r\n", pUser->GetAccountName(), pUser->GetSessionID() );
				}
			}				
#elif defined(_TH) && defined(_FINAL_BUILD)
			if( pUser )		
				g_pAsiaSoftPCCafe->SendCheckIPBonus(pUser->GetAccountNameA(), pUser->GetIp(), 2, pUser->GetAccountDBID());
#endif	// _KR _CH _JP _TW
#ifdef PRE_ADD_STEAM_USERCOUNT
			if (pUser && pUser->m_bSteamUser)
				DecreaseSteamUser();
#endif		//#ifdef PRE_ADD_STEAM_USERCOUNT

			LeavePvPRoom( pUser->GetAccountDBID(), false );
			DelUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID());
			EraseUserListExceptAccountDBList(pUser);

			ii = m_AccountDBIDList.erase(ii);
		}
		else
			ii++;
	}
}

int CDivisionManager::_CheckVillageToPvPVillage( CDNVillageConnection* pVillageCon, CDNUser* pUser, char cReqType, const BYTE cNextVillageID, const int nNextVillageChannelID )
{
	const TChannelInfo* pChannelInfo = GetChannelInfo( cNextVillageID, nNextVillageChannelID );
	if( !pChannelInfo )
		return ERROR_GENERIC_VILLAGECON_NOT_FOUND;

	// 이동할 맵이 PvPVillage 인지 검사
#if defined(PRE_ADD_DWC)
	if( pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVP || pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_DWC )
#else
	if( pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVP )
#endif
	{
		// 파티상태라면 에러
		if (cReqType == REQINFO_TYPE_PARTY)
			return ERROR_PVP_CANTMOVECHANNEL_PARTYSTATE;
	}

	return ERROR_NONE;
}

void CDivisionManager::_CheckPvPVillageToPvPLobby( VIMAVillageToVillage* pPacket, CDNUser* pUser, const BYTE cNextVillageID, const int nNextChannelIndex )
{
	// 현재채널이 PvP마을인지 검사
	const TChannelInfo* pChannelInfo = GetChannelInfo( pUser->GetVillageID(), pUser->GetChannelID() );
	if( !pChannelInfo )
		return;

#if defined(PRE_ADD_DWC)
	if( !(pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVP) && !(pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_DWC)  )
#else
	if( !(pChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVP) )
#endif
		return;

	// 타겟채널이 PvP로비인지 검사
	const TChannelInfo* pNextChannelInfo = GetChannelInfo( cNextVillageID, nNextChannelIndex );
	if( !pNextChannelInfo )
		return;

	if( !(pNextChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY) )
		return;

	// 현재 PvP마을 정보 기억
	pUser->SetPvPVillageInfo();
}

void CDivisionManager::_CheckVillageToPvPLobby( VIMAVillageToVillage* pPacket, CDNUser* pUser, const BYTE cNextVillageID, const int nNextChannelIndex )
{
	// 타겟채널이 PvP로비인지 검사
	const TChannelInfo* pNextChannelInfo = GetChannelInfo( cNextVillageID, nNextChannelIndex );
	if( !pNextChannelInfo )
		return;

	if( !(pNextChannelInfo->nAttribute&GlobalEnum::CHANNEL_ATT_PVPLOBBY) )
		return;

	// 현재 PvP마을 정보 기억
	pUser->SetPvPVillageInfo();
}

void CDivisionManager::MovePvPLobbyToVillage( CDNVillageConnection* pCon, VIMAPVP_MOVELOBBYTOVILLAGE* pPacket )
{
	if( !pCon )
	{
		_DANGER_POINT();
		return;
	}

	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( !pUser )
	{
		_DANGER_POINT();
		return;
	}
	
	// 현재 유저가 로비상태에 있는지 검사
	if( !pUser->bIsPvPLobby() )
	{
		_DANGER_POINT();
		return;
	}

	// PvP방에 있는지 검사
	if( pUser->GetPvPIndex() > 0 )
	{
		_DANGER_POINT();
		return;
	}

	const TChannelInfo* pChannelInfo = GetChannelInfo( pUser->GetPvPVillageID(), pUser->GetPvPVillageChannelID() );
	if( !pChannelInfo )
	{
		_DANGER_POINT();
		return;
	}

	char	szIP[IPLENMAX]	= { 0, };
	USHORT	nPort			= 0;
	int		nNextChannelIdx = 0;
	BYTE	cNextVillageID	= 0;

	if( GetVillageInfo( pChannelInfo->nMapIdx, pChannelInfo->nChannelID, nNextChannelIdx, cNextVillageID, szIP, nPort) == false )
	{
		_DANGER_POINT();
		return;
	}

	int nNextGateIdx	= PvPCommon::Common::PvPVillageStartPositionGateNo;
	int nNextMapIdx		= pChannelInfo->nMapIdx;

	if( VillageToVillage(pCon, pUser->GetAccountDBID(), cNextVillageID, nNextChannelIdx, 0) )
		pCon->SendTargetVillageInfo(pUser->GetAccountDBID(), cNextVillageID, nNextChannelIdx, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_NONE);
	else
		pCon->SendTargetVillageInfo(pUser->GetAccountDBID(), cNextVillageID, nNextChannelIdx, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
}

void CDivisionManager::MoveVillageToVillage(CDNVillageConnection * pCon, VIMAVillageToVillage * pPacket)
{
	char szIP[IPLENMAX] = { 0, };
	unsigned short nPort = 0;
	BYTE cNextVillageID = 0;
	int nNextChannelID = 0;
	int nNextGateIdx = -1;
	int nNextMapIdx = -1;

	if (pCon == NULL)
	{
		_DANGER_POINT();
		return;
	}
	
	UINT nAccountDBID = 0;
	switch (pPacket->cReqType)
	{
	case REQINFO_TYPE_SINGLE: 
	case REQINFO_TYPE_SINGLE_SAMEDEST:
	case REQINFO_TYPE_PVP:
	case REQINFO_TYPE_PVP_BREAKINTO:
	{
#if defined( PRE_PARTY_DB )
		nAccountDBID = static_cast<UINT>(pPacket->biID);
#else
		nAccountDBID = pPacket->nID;
#endif // #if defined( PRE_PARTY_DB )
		break;
	}
	case REQINFO_TYPE_PARTY: nAccountDBID = pPacket->PartyData.nLeaderAccountDBID; break;
	default: _ASSERT(0);
	}

	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser == NULL)
	{
		_DANGER_POINT();
		return;
	}
	
	int nOriginChannel = pUser->GetChannelID();
	if (pPacket->nMoveChannel > 0)			//채널이동일경우
	{
		if (GetVillageInfo(pPacket->nMapIndex, pPacket->nMoveChannel, nNextChannelID, cNextVillageID, szIP, nPort) == false)
		{
			_DANGER_POINT();
			return;
		}

		int nCheckRet = _CheckVillageToPvPVillage( pCon, pUser, pPacket->cReqType, cNextVillageID, nNextChannelID );
		if( nCheckRet != ERROR_NONE )
		{
			pCon->SendTargetVillageInfo( pUser->GetAccountDBID(), cNextVillageID, nNextChannelID, nNextMapIdx, nNextGateIdx, szIP, nPort, nCheckRet );
			return;
		}

		nNextGateIdx = pPacket->nEnteredGateIndex;
		nNextMapIdx = pPacket->nMapIndex;
		_CheckVillageToPvPLobby(pPacket, pUser, cNextVillageID, nNextChannelID);
	}
	else if (pPacket->nEnteredGateIndex > 0)	// 마을에서 마을로 이동할때
	{
		nNextGateIdx = g_pExtManager->GetGateNoByGateNo( pPacket->nMapIndex, pPacket->nEnteredGateIndex );
		nNextMapIdx = g_pExtManager->GetMapIndexByGateNo( pPacket->nMapIndex, pPacket->nEnteredGateIndex );

		if (GetVillageInfo(nNextMapIdx, nOriginChannel, nNextChannelID, cNextVillageID, szIP, nPort) == false)
		{
			_DANGER_POINT();
			return;
		}

		_CheckPvPVillageToPvPLobby( pPacket, pUser, cNextVillageID, nNextChannelID );
	}
	else			// 치트용
	{
		if (GetVillageInfo(pPacket->nMapIndex, nOriginChannel, nNextChannelID, cNextVillageID, szIP, nPort) == false)
		{
			_DANGER_POINT();
			return;
		}
		nNextMapIdx = pPacket->nMapIndex;
	}

	if (pPacket->cReqType == REQINFO_TYPE_PARTY)
	{
#if defined( PRE_PARTY_DB )
		for (int i = 0; i < pPacket->PartyData.PartyData.iCurMemberCount; i++)
#else
		for (int i = 0; i < pPacket->PartyData.cMemberCount; i++)
#endif
		{
#if defined( PRE_PARTY_DB )
			if (VillageToVillage(pCon, pPacket->PartyData.MemberInfo[i].nAccountDBID, cNextVillageID, nNextChannelID, pPacket->biID) == false)
#else
			if (VillageToVillage(pCon, pPacket->PartyData.MemberInfo[i].nAccountDBID, cNextVillageID, nNextChannelID, pPacket->nID) == false)
#endif // #if defined( PRE_PARTY_DB )
			{
				_DANGER_POINT();
				SendDetachUser(pPacket->PartyData.MemberInfo[i].nAccountDBID);
			}
		}

		if (cNextVillageID != pCon->GetVillageID())	//물리적으로 다른 마을로 이동이다. 그쪽에 파티를 푸쉬해준다
		{
			CDNVillageConnection * pVillageCon = GetVillageConnectionByVillageID(cNextVillageID);
			if (pVillageCon)
			{
#if defined( PRE_PARTY_DB )
				pVillageCon->SendPushParty(nAccountDBID, pPacket->biID, nNextChannelID, pUser->m_nRandomSeed, &pPacket->PartyData);
#else
				pVillageCon->SendPushParty(nAccountDBID, pPacket->nID, nNextChannelID, pUser->m_nRandomSeed, &pPacket->PartyData);
#endif // #if defined( PRE_PARTY_DB )
				pCon->SendTargetVillageInfo(pUser->GetAccountDBID(), cNextVillageID, nNextChannelID, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_NONE);
			}
			else
				pCon->SendTargetVillageInfo(pUser->GetAccountDBID(), cNextVillageID, nNextChannelID, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_GENERIC_UNKNOWNERROR);
		}
		else
			pCon->SendTargetVillageInfo(pUser->GetAccountDBID(), cNextVillageID, nNextChannelID, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_NONE);
	}
	else
	{
		if (VillageToVillage(pCon, pUser->GetAccountDBID(), cNextVillageID, nNextChannelID, 0))
		{
			pCon->SendTargetVillageInfo(pUser->GetAccountDBID(), cNextVillageID, nNextChannelID, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_NONE, pPacket->cReqType == REQINFO_TYPE_SINGLE_SAMEDEST ? true : false, pPacket->nItemSerial);
		}
		else
		{
			_DANGER_POINT();
			SendDetachUser(pUser->GetAccountDBID());
		}
	}
}

void CDivisionManager::MoveGameToVillage(CDNGameConnection * pCon, GAMARequestNextVillageInfo * pPacket)
{
	CDNUser * pUser = NULL;
	UINT nAccountDBID = 0;
	char szIP[IPLENMAX] = { 0, };
	USHORT nPort = 0;
	BYTE cVillageID = 0;
	int nChannelID = 0;

	switch (pPacket->cReqGameIDType)
	{
		case REQINFO_TYPE_SINGLE: 
		{
			nAccountDBID = static_cast<UINT>(pPacket->InstanceID);
			break;
		}
		case REQINFO_TYPE_PVP:
		case REQINFO_TYPE_PVP_BREAKINTO:
		case REQINFO_TYPE_PARTY: nAccountDBID = pPacket->PartyData.nLeaderAccountDBID; break;
		default: _ASSERT(0);
	}

	pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser == NULL || pCon == NULL)
	{
		_DANGER_POINT();
		if (pCon)
			pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, 0, 0, szIP, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
		return;
	}
	
	int nOriginChannel = pUser->GetChannelID();
	if (pPacket->cReturnVillage == 1)
	{
		if (GetVillageInfo(pPacket->nMapIndex, nOriginChannel,nChannelID, cVillageID, szIP, nPort) == false)
		{
			_DANGER_POINT();
			pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, 0, 0, szIP, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
			return;
		}

		if (pPacket->cReqGameIDType == REQINFO_TYPE_PARTY)
		{
#if defined( PRE_PARTY_DB )
			for (int i = 0; i < pPacket->PartyData.PartyData.iCurMemberCount; i++)
#else
			for (int i = 0; i < pPacket->PartyData.cMemberCount; i++)
#endif
			{
				if (GameToVillage(pCon, pPacket->PartyData.MemberInfo[i].nAccountDBID, cVillageID, nChannelID) == false)
				{
					_DANGER_POINT();
					SendDetachUser(pPacket->PartyData.MemberInfo[i].nAccountDBID);	//스테이트미스매치
				}
			}
			
			CDNVillageConnection * pVillageCon = GetVillageConnectionByVillageID(cVillageID);
			if (pVillageCon)
			{
				pVillageCon->SendPushParty(nAccountDBID, pPacket->InstanceID, nChannelID, pUser->m_nRandomSeed, &pPacket->PartyData);
				pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, 0, 0, szIP, nPort, ERROR_NONE);
			}
			else
				pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, 0, 0, szIP, nPort, ERROR_GENERIC_UNKNOWNERROR);
		}
		else
		{
			if (GameToVillage(pCon, nAccountDBID, cVillageID, nChannelID))
			{
				pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, 0, 0, szIP, nPort, ERROR_NONE, pPacket->nItemSerial);
			}
			else
			{
				_DANGER_POINT();
				SendDetachUser(nAccountDBID);
			}
		}
	}
	else
	{
		int nNextGateIdx = 0, nNextMapIdx = 0;

		if (pPacket->nEnteredGateIndex > 0){	// mapindex, gate다 있는경우
			nNextGateIdx = g_pExtManager->GetGateNoByGateNo( pPacket->nMapIndex, pPacket->nEnteredGateIndex );
			nNextMapIdx = g_pExtManager->GetMapIndexByGateNo( pPacket->nMapIndex, pPacket->nEnteredGateIndex );
		}
		else{	// gateno를 모르는 경우
			nNextGateIdx = -1;
			nNextMapIdx = pPacket->nMapIndex;
		}

		switch( g_pExtManager->GetMapType( nNextMapIdx ) )
		{
			case GlobalEnum::MAP_VILLAGE:
			{
				char szIP[IPLENMAX] = { 0, };
				unsigned short nPort = 0;
				BYTE cNextVillageID = 0;
				int nNextChannelID = 0;

				//항상 채널은 재분배될 수 있습니다.
				if (GetVillageInfo(nNextMapIdx, nOriginChannel, nNextChannelID, cNextVillageID, szIP, nPort) == false)				{
					pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_GENERIC_VILLAGECON_NOT_FOUND);
					break;
				}				
				
				if (pPacket->cReqGameIDType == REQINFO_TYPE_PARTY)
				{
#if defined( PRE_PARTY_DB )
					for (int i = 0; i < pPacket->PartyData.PartyData.iCurMemberCount; i++)
#else
					for (int i = 0; i < pPacket->PartyData.cMemberCount; i++)
#endif
					{
						if (GameToVillage(pCon, pPacket->PartyData.MemberInfo[i].nAccountDBID, cNextVillageID, nNextChannelID) == false)
						{
							_DANGER_POINT();
							SendDetachUser(pPacket->PartyData.MemberInfo[i].nAccountDBID);
						}
					}

					CDNVillageConnection * pVillageCon = GetVillageConnectionByVillageID(cNextVillageID);
					if (pVillageCon)
					{
						pVillageCon->SendPushParty(nAccountDBID, pPacket->InstanceID, nNextChannelID, pUser->m_nRandomSeed, &pPacket->PartyData);
						pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_NONE);
					}
					else
						pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, 0, 0, szIP, nPort, ERROR_GENERIC_UNKNOWNERROR);
				}
				else
				{
					if (GameToVillage(pCon, nAccountDBID, cNextVillageID, nNextChannelID))
						pCon->SendNextVillageServerInfo(nAccountDBID, pPacket->nMapIndex, nNextMapIdx, nNextGateIdx, szIP, nPort, ERROR_NONE);
					else
						SendDetachUser(nAccountDBID);
				}
				break;
			}
			default:
				_DANGER_POINT();
		}
	}
}

bool CDivisionManager::VillageToVillage(CDNVillageConnection * pCon, UINT nAccountDBID, int nVillageID, int nChannelID, TPARTYID PartyID)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser && pUser->SetCheckVillageInfo(nVillageID, nChannelID))
	{
		pUser->m_VillageCheckPartyID = PartyID;
		return true;
	}
	return false;
}

bool CDivisionManager::GameToVillage(CDNGameConnection * pCon, UINT nAccountDBID, int nVillageID, int nChannelID)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser && pUser->SetCheckVillageInfo(nVillageID, nChannelID))
		return true;
	return false;
}

bool CDivisionManager::VillageToGame(CDNVillageConnection * pCon, UINT nAccountDBID, int nGameID, int nRoomID, int nServerIdx)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser && pUser->SetCheckGameInfo(nGameID, nServerIdx))
	{
		pUser->m_nRoomID = nRoomID;
		return true;
	}
	return false;
}

bool CDivisionManager::EnterVillage(UINT nAccountDBID, int nVillageID, int nChannelID)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser)
	{//최종검증한번하고 트레이스한다
		if (pUser->GetVillageID() != nVillageID || pUser->GetChannelID() != nChannelID)
		{
			_DANGER_POINT();			//꼬였다! 일
			return false;
		}

		const TChannelInfo * pChannelInfo = GetChannelInfo(nChannelID);
		if (pChannelInfo)
		{
			if (pUser->SetVillageInfo(nVillageID, nChannelID, false) == false)
				return false;

			UpdateUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID(), _LOCATION_VILLAGE, _COMMUNITY_NONE, pChannelInfo->nChannelIdx, pChannelInfo->nMapIdx);	//스테이트업데이트 즐기고

#if !defined (_KR)
			SendPCBangResult(pUser);
#endif
		}
		else
		{
			_DANGER_POINT();
			return false;
		}

#if defined(_CH) && defined(_FINAL_BUILD)
		CDNVillageConnection *pVillageCon = pUser->GetCurrentVillageConnection();
		if (pVillageCon)
			pVillageCon->SendFCMState(pUser->GetAccountDBID(), pUser->GetFCMOnlineMin(), false);
#endif
		return true;
	}
	_DANGER_POINT();
	return false;
}

bool CDivisionManager::EnterGame(UINT nAccountDBID, int nGameID, int nRoomID, int nServerIdx)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser)
	{//최종검증한번하고 트레이스한다
		if (pUser->GetGameID() != nGameID || pUser->m_nRoomID != nRoomID || pUser->GetGameServerIdx() != nServerIdx)
		{
			_DANGER_POINT();			//꼬였다!
			return false;
		}

		if (pUser->SetGameInfo(nGameID, nServerIdx, false) == false)
			return false;
		
		// 캐스팅을 해줘서 원하는 함수 콜이 이루어지게 함
		UpdateUserState(pUser->GetCharacterName(), pUser->GetCharacterDBID(), _LOCATION_GAME, _COMMUNITY_NONE, -1, pUser->m_nMapIndex );

#if defined(_CH) && defined(_FINAL_BUILD)
		CDNGameConnection *pGameCon = pUser->GetCurrentGameConnection();
		if (pGameCon)
			pGameCon->SendFCMState(pUser->GetAccountDBID(), pUser->GetFCMOnlineMin(), false);
#endif	// #if defined(_CH)

#if !defined (_KR)
		SendPCBangResult(pUser);
#endif	// #if !defined (_KR)
		return true;
	}
	return false;
}

bool CDivisionManager::ReconnectLogin(UINT nAccountDBID)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser)
	{
		//게임서버 또는 빌리지서버의 파이날 유저가 호출되면서 발생합니다.(캐릭터선택창으로 이동시
		pUser->SetCheckReconnectLogin();
		return true;
	}
	return false;
}

CDNPvP* CDivisionManager::GetPvPRoomByIdx( UINT uiIndex )
{
	_TPvPMap::iterator itor = m_mPvP.find(uiIndex);
	if( itor == m_mPvP.end() )
		return NULL;
	return (*itor).second;
}

bool CDivisionManager::SendInviteGuildMember(MAInviteGuildMember* pPacket)
{
	// 길드초대할 유저를 찾는다
	CDNUser * pUser = GetUserByName(pPacket->wszToCharacterName);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				// 해당 빌리지 서버에 있는 유저일 경우 길드초대 알림
				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID (pUser->GetVillageID());
				if( pVillageCon )
				{
					pVillageCon->SendInviteGuildMember(pPacket);
					return true;
				}
			}
			break;

		default:
			break;
		}
	}
	else // 유저존재 하지 않으므로 에러처리
		return false;
	
	return false;
}

#if defined (PRE_ADD_BESTFRIEND)
bool CDivisionManager::SendSearchBestFriend(MASearchBestFriend* pPacket)
{

	CDNUser * pUser = NULL;
	if (!pPacket->bAck)
		pUser = GetUserByName(pPacket->wszName);
	else
		pUser = GetUserByAccountDBID(pPacket->nAccountDBID);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID (pUser->GetVillageID());
				if( pVillageCon )
				{
					pVillageCon->SendSearchBestFriend(pPacket);
					return true;
				}
			}
			break;

		default:
			break;
		}
	}
	else
		return false;

	return false;
}

bool CDivisionManager::SendRegistBestFriend(MARegistBestFriend* pPacket)
{
	CDNUser * pUser = GetUserByName(pPacket->wszToName);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID (pUser->GetVillageID());
				if( pVillageCon )
				{
					pVillageCon->SendRegistBestFriend(pPacket);
					return true;
				}
			}
			break;

		default:
			break;
		}
	}
	else
		return false;

	return false;
}

void CDivisionManager::SendRegistBestFriendResult(MARegistBestFriendResult* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendRegistBestFriendResult(pPacket);
			}
			break;

		case STATE_GAME:
			{
				// 게임서버에 존재할 경우 에러처리
			}
			break;
		}
	}
}

void CDivisionManager::SendCompleteBestFriend(MACompleteBestFriend* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nToAccountDBID);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendCompleteBestFriend(pPacket);
			}
			break;

		case STATE_GAME:
			{
				// 게임서버에 존재할 경우 에러처리
			}
			break;
		}
	}
}

void CDivisionManager::SendEditBestFriendMemo(MAEditBestFriendMemo* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByCharacterDBID(pPacket->biToCharacterDBID);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
				{
					pPacket->nToAccountDBID = pUser->GetAccountDBID();
					pVillageCon->SendEditBestFriendMemo(pPacket);
				}
			}
			break;

		case STATE_GAME:
			{
				// 게임서버에 존재할 경우 에러처리
			}
			break;
		}
	}
}

void CDivisionManager::SendCancelBestFriend(MACancelBestFriend* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByName(pPacket->wszToName);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendCancelBestFriend(pPacket);
			}
			break;

		case STATE_GAME:
			{
				CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
				if( pGameCon )
				{
					pPacket->nAccountDBID = pUser->GetAccountDBID();
					pGameCon->SendCancelBestFriend( pPacket );
				}
				break;
			}
			break;
		}
	}
}

void CDivisionManager::SendCloseBestFriend(MACloseBestFriend* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByName(pPacket->wszToName);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendCloseBestFriend(pPacket);
			}
			break;

		case STATE_GAME:
			{
				CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
				if( pGameCon )
				{
					pPacket->nAccountDBID = pUser->GetAccountDBID();
					pGameCon->SendCloseBestFriend( pPacket );
				}
				break;
			}
			break;
		}
	}
}

void CDivisionManager::SendLevelBestFriend(MALevelUpBestFriend* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByName(pPacket->wszName);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendLevelBestFriend(pPacket);
			}
			break;

		case STATE_GAME:
			{
				CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
				if( pGameCon )
				{
					pPacket->nAccountDBID = pUser->GetAccountDBID();
					pGameCon->SendLevelBestFriend( pPacket );
				}
				break;
			}
			break;
		}
	}
}


#endif // #if defined (PRE_ADD_BESTFRIEND)


void CDivisionManager::SendInviteGuildMemberResult(VIMAInviteGuildMemberResult* pPacket)
{
	// 길드원 초대 요청한 유저를 찾는다.
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nInviterDBID);

	if( pUser )
	{
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendInviteGuildMemberResult(pPacket->nInviterDBID, pPacket->nRetCode, pPacket->bAck, pPacket->wszInvitedName);
			}
			break;

		case STATE_GAME:
			{
				// 게임서버에 존재할 경우 에러처리
			}
			break;
		}
	}
}

void CDivisionManager::SendGuildWareInfo(MAGuildWareInfo* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		if (0 != (*itor)->GetManagedID() && pPacket->nManagedID == (*itor)->GetManagedID()) 
			continue;

		(*itor)->SendGuildWareInfo(pPacket);
	}
}

void CDivisionManager::SendGuildWareInfoResult(MAGuildWareInfoResult* pPacket)
{
	// 요청한 VI서버에 반송
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		if (0 != (*itor)->GetManagedID() && pPacket->nFromManagedID == (*itor)->GetManagedID()) 
		{
			(*itor)->SendGuildWareInfoResult(pPacket);
			return;
		}
	}
}

void CDivisionManager::SendGuildMemberLevelUp(MAGuildMemberLevelUp* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID()) {
			continue;
		}
		(*itorv)->SendGuildMemberLevelUp(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID()) {
			continue;
		}
		(*itorg)->SendGuildMemberLevelUp(pPacket);
	}
}

void CDivisionManager::SendRefreshGuildItem(MARefreshGuildItem* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		if (0 != (*itor)->GetManagedID() && pPacket->nManagedID == (*itor)->GetManagedID()) 
			continue;

		(*itor)->SendRefreshGuildItem(pPacket);
	}
}

void CDivisionManager::SendRefreshGuildCoin(MARefreshGuildCoin* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		if (0 != (*itor)->GetManagedID() && pPacket->nManagedID == (*itor)->GetManagedID()) 
			continue;

		(*itor)->SendRefreshGuildCoin(pPacket);
	}
}

void CDivisionManager::SendExtendGuildWareSize(MAExtendGuildWare* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		if (0 != (*itor)->GetManagedID() && pPacket->nManagedID == (*itor)->GetManagedID()) 
			continue;

		(*itor)->SendExtendGuildWareSize(pPacket);
	}
}

void CDivisionManager::SendDismissGuild(MADismissGuild* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendDismissGuild(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator iterg;
	for (iterg = m_GameServerConList.begin() ; m_GameServerConList.end() != iterg ; ++iterg) {
		if (0 != (*iterg)->GetManagedID() && pPacket->nManagedID == (*iterg)->GetManagedID()) {
			continue;
		}
		(*iterg)->SendDismissGuild(pPacket);
	}
}

void CDivisionManager::SendAddGuildMember(MAAddGuildMember* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID()) {
			continue;
		}
		(*itorv)->SendAddGuildMember(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID()) {
			continue;
		}
		(*itorg)->SendAddGuildMember(pPacket);
	}
}

void CDivisionManager::SendDelGuildMember(MADelGuildMember* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID()) {
			continue;
		}
		(*itorv)->SendDelGuildMember(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID()) {
			continue;
		}
		(*itorg)->SendDelGuildMember(pPacket);
	}
}

void CDivisionManager::SendChangeGuildInfo(MAChangeGuildInfo* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID()) {
			continue;
		}
		(*itorv)->SendChangeGuildInfo(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID()) {
			continue;
		}
		(*itorg)->SendChangeGuildInfo(pPacket);
	}
}

void CDivisionManager::SendChangeGuildMemberInfo(MAChangeGuildMemberInfo* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID()) {
			continue;
		}
		(*itorv)->SendChangeGuildMemberInfo(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID()) {
			continue;
		}
		(*itorg)->SendChangeGuildMemberInfo(pPacket);
	}
}

void CDivisionManager::SendGuildChat(MAGuildChat* pPacket)
{
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	if (g_pDoorsConnection && g_pDoorsConnection->GetActive())
	{
		CDNUser * pUser = GetUserByAccountDBID(pPacket->nAccountDBID);
		if (pUser)
			g_pDoorsConnection->SendGuildChatToDoors(pUser, pPacket->GuildUID.nDBID, pPacket->wszChatMsg, pPacket->nLen);
	}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		(*itorv)->SendGuildChat(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		(*itorg)->SendGuildChat(pPacket);
	}
}

void CDivisionManager::SendChangeGuildName(MAGuildChangeName* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		(*itorv)->SendChangeGuildName(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		(*itorg)->SendChangeGuildName(pPacket);
	}
}

void CDivisionManager::SendChangeGuildMark(MAGuildChangeMark* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv)
		(*itorv)->SendChangeGuildMark(pPacket);

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg)
		(*itorg)->SendChangeGuildMark(pPacket);
}

void CDivisionManager::SendUpdateGuildExp(MAUpdateGuildExp* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv)
		(*itorv)->SendUpdateGuildExp(pPacket);

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg)
		(*itorg)->SendUpdateGuildExp(pPacket);
}

void CDivisionManager::SendEnrollGuildWar(MAEnrollGuildWar* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID())
			continue;

		(*itorv)->SendEnrollGuildWar(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID())
			continue;

		(*itorg)->SendEnrollGuildWar(pPacket);
	}
}
void CDivisionManager::SendChangeGuildWarStep(MAChangeGuildWarStep* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{			
		(*itorv)->SendChangeGuildWarStep(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{		
		(*itorg)->SendChangeGuildWarStep(pPacket);
	}
}

void CDivisionManager::SendSetGuildWarPoint(MASetGuildWarPoint* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv)
	{
		(*itorv)->SendSetGuildWarPoint(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg)
	{
		(*itorg)->SendSetGuildWarPoint(pPacket);
	}
}

void CDivisionManager::SendSetGuildWarPreWinGuild(MAGuildWarPreWinGuild* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv)
	{
		(*itorv)->SendSetGuildWarPreWinGuild(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg)
	{
		(*itorg)->SendSetGuildWarPreWinGuild(pPacket);
	}
}

#if defined(PRE_FIX_75807)
void CDivisionManager::SendSetGuildWarPreWinGuildGameServer(MAGuildWarPreWinGuild* pPacket)
{
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg)
	{
		(*itorg)->SendSetGuildWarPreWinGuild(pPacket);
	}
}
#endif //#if defined(PRE_FIX_75807)


void CDivisionManager::SendAddGuildWarPoint(MAAddGuildWarPoint* pPacket)
{
	// 전체 VI 에 송신, GA는 필요없음.
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		(*itor)->SendAddGuildWarPoint(pPacket);
	}
}

void CDivisionManager::SendGetGuildWarTrialStats()
{
	// 전체 VI 에 송신, GA는 필요없음.
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		(*itor)->SendGetGuildWarTrialStats();
	}
}

void CDivisionManager::SendSetGuildWarTournamentInfo(SGuildTournamentInfo* pGuildTournamentInfo)
{
	// 전체 VI 에 송신, GA는 필요없음.
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		(*itor)->SendSetGuildWarTournamentInfo(pGuildTournamentInfo);
	}
}

void CDivisionManager::SendSetGuildwarFinalProcess(char cGuildFinalPart, __time64_t tBeginTime)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{
		(*itorv)->SendSetGuildwarFinalProcess(cGuildFinalPart, tBeginTime);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg)
	{
		(*itorg)->SendSetGuildwarFinalProcess(cGuildFinalPart, tBeginTime);
	}
}

void CDivisionManager::SendGuildWarInfoToVillage(CDNVillageConnection* pCon)
{
	char cStepIndex = g_pGuildWarManager->GetStepIndex();
#if defined(PRE_FIX_75807)
	// 본선이 아니거나 차수가 진행중이 아니고 지난대회 우승팀이 있으면 보냄
	if( (cStepIndex != GUILDWAR_STEP_REWARD || !g_pGuildWarManager->GetFinalStart())  && g_pGuildWarManager->GetPreWinGuild().IsSet() )
	{
		if( g_pGuildWarManager->GetFinalWinGuildReward() )	
		{
			MAGuildWarPreWinGuild GuildWarPreWinGuild;
			GuildWarPreWinGuild.GuildUID = g_pGuildWarManager->GetPreWinGuild();
			GuildWarPreWinGuild.bPreWin = true;
			pCon->SendSetGuildWarPreWinGuild(&GuildWarPreWinGuild);

			if( g_pGuildWarManager->GetPreWinSKillCoolTime() > 0 )
			{
				MAGuildWarPreWinSkillCoolTime GuildWarPreWinSkillCoolTime = {0,};
				GuildWarPreWinSkillCoolTime.dwSkillCoolTime = g_pGuildWarManager->GetPreWinSKillCoolTime();

				pCon->SendSetGuildWarPreWinSkillCoolTime(&GuildWarPreWinSkillCoolTime);
			}		
		}
	}
#endif //#if defined(PRE_FIX_75807)

	if( g_pGuildWarManager->GetGuildWarSettingStep() == REQ_ALL_COMPLETE )
	{
		// 전체 스케쥴 보내주기
		MASetGuildWarEventTime SetGuildWarEventTime;
		memset(&SetGuildWarEventTime, 0, sizeof(SetGuildWarEventTime));
		memcpy(SetGuildWarEventTime.sGuildWarTime, g_pGuildWarManager->GetGuildWarSchedule(), sizeof(SetGuildWarEventTime.sGuildWarTime));
		memcpy(SetGuildWarEventTime.sFinalPartTime, g_pGuildWarManager->GetGuildWarFinalSchedule(), sizeof(SetGuildWarEventTime.sFinalPartTime));

		SetGuildWarEventTime.bFinalProgress = g_pGuildWarManager->GetFinalProgress();
		pCon->SendSetGuildWarSchedule(&SetGuildWarEventTime);
	}
	if( cStepIndex == GUILDWAR_STEP_NONE 
		|| !(g_pGuildWarManager->GetWarEvent(cStepIndex))->IsValidPeriod() )
		return;

	MAChangeGuildWarStep ChangeGuildWarStep;
	memset(&ChangeGuildWarStep, 0, sizeof(ChangeGuildWarStep));

	ChangeGuildWarStep.wScheduleID = g_pGuildWarManager->GetScheduleID();
	ChangeGuildWarStep.cEventType = GUILDWAR_EVENT_START;
	ChangeGuildWarStep.cEventStep = cStepIndex;
	ChangeGuildWarStep.wWinersWeightRate = g_pGuildWarManager->GetWinersWeightRate();
	pCon->SendChangeGuildWarStep(&ChangeGuildWarStep);
	
	// 예선전이거나 보상기간이면 각 팀 점수보내주기..
	if( cStepIndex == GUILDWAR_STEP_TRIAL || cStepIndex == GUILDWAR_STEP_REWARD)
	{
		MASetGuildWarPoint SetGuildWarPoint = {0,};
		SetGuildWarPoint.nBlueTeamPoint = g_pGuildWarManager->GetBlueTeamPoint();
		SetGuildWarPoint.nRedTeamPoint = g_pGuildWarManager->GetRedTeamPoint();
		pCon->SendSetGuildWarPoint(&SetGuildWarPoint);

		if( cStepIndex == GUILDWAR_STEP_TRIAL )
		{
			//24위 까지 정보 보내주기..
			pCon->SendSetGuildWarPointRunning((MASetGuildWarPointRunningTotal*)g_pGuildWarManager->GetGuildWarPointTrialRanking());
		}
	}
	// 본선이고 예선집계가 가능하면 대진표 정보랑 집계하라고 보냄
	if( cStepIndex == GUILDWAR_STEP_REWARD )	
	{
		if( g_pGuildWarManager->IsTrialStats() )
		{
			pCon->SendSetGuildWarTournamentInfo(g_pGuildWarManager->GetGuildTournamentInfo());
			pCon->SendGetGuildWarTrialStats();
		}
		pCon->SendSetGuildwarFinalProcess(g_pGuildWarManager->GetCurFinalPart(), g_pGuildWarManager->GetCurFinalPartBeginTime());
	}	
#if defined(PRE_FIX_75807)
#else
	// 본선이 아니거나 차수가 진행중이 아니고 지난대회 우승팀이 있으면 보냄
	if( (cStepIndex != GUILDWAR_STEP_REWARD || !g_pGuildWarManager->GetFinalStart())  && g_pGuildWarManager->GetPreWinGuild().IsSet() )
	{
		if( g_pGuildWarManager->GetFinalWinGuildReward() )	
		{
			MAGuildWarPreWinGuild GuildWarPreWinGuild;
			GuildWarPreWinGuild.GuildUID = g_pGuildWarManager->GetPreWinGuild();
			GuildWarPreWinGuild.bPreWin = true;
			pCon->SendSetGuildWarPreWinGuild(&GuildWarPreWinGuild);

			if( g_pGuildWarManager->GetPreWinSKillCoolTime() > 0 )
			{
				MAGuildWarPreWinSkillCoolTime GuildWarPreWinSkillCoolTime = {0,};
				GuildWarPreWinSkillCoolTime.dwSkillCoolTime = g_pGuildWarManager->GetPreWinSKillCoolTime();

				pCon->SendSetGuildWarPreWinSkillCoolTime(&GuildWarPreWinSkillCoolTime);
			}		
		}
	}
#endif //#if defined(PRE_FIX_75807)

	BYTE cSecretTeam = g_pGuildWarManager->GetSecretTeam();
	if (cSecretTeam > 0)
	{
		MASetGuildWarSecretMission SecretMission;
		SecretMission.cTeamCode = cSecretTeam;
		SecretMission.nRandomSeed = g_pGuildWarManager->GetSecretRandomSeed();
		pCon->SendSetGuildWarSecretMission(&SecretMission);
	}
}

void CDivisionManager::SendGuildWarInfoToGame(CDNGameConnection* pCon)
{
	char cStepIndex = g_pGuildWarManager->GetStepIndex();
#if defined(PRE_FIX_75807)
	// 본선이 아니거나 차수가 진행중이 아니고 지난대회 우승팀이 있으면 보냄
	if( (cStepIndex != GUILDWAR_STEP_REWARD || !g_pGuildWarManager->GetFinalStart() )  && g_pGuildWarManager->GetPreWinGuild().IsSet() )
	{
		if( g_pGuildWarManager->GetFinalWinGuildReward() ) // 보상이 지급되었는지 확인하고
		{
			MAGuildWarPreWinGuild GuildWarPreWinGuild;
			GuildWarPreWinGuild.GuildUID = g_pGuildWarManager->GetPreWinGuild();
			GuildWarPreWinGuild.bPreWin = true;
			pCon->SendSetGuildWarPreWinGuild(&GuildWarPreWinGuild);
		}		
	}
#endif //#if defined(PRE_FIX_75807)
	if( cStepIndex == GUILDWAR_STEP_NONE 
		|| !(g_pGuildWarManager->GetWarEvent(cStepIndex))->IsValidPeriod() )
		return;

	if( g_pGuildWarManager->GetGuildWarSettingStep() == REQ_ALL_COMPLETE )
	{
		// 전체 스케쥴 보내주기
		MASetGuildWarEventTime SetGuildWarEventTime;
		memset(&SetGuildWarEventTime, 0, sizeof(SetGuildWarEventTime));
		memcpy(SetGuildWarEventTime.sGuildWarTime, g_pGuildWarManager->GetGuildWarSchedule(), sizeof(SetGuildWarEventTime.sGuildWarTime));
		memcpy(SetGuildWarEventTime.sFinalPartTime, g_pGuildWarManager->GetGuildWarFinalSchedule(), sizeof(SetGuildWarEventTime.sFinalPartTime));

		SetGuildWarEventTime.bFinalProgress = g_pGuildWarManager->GetFinalProgress();
		pCon->SendSetGuildWarSchedule(&SetGuildWarEventTime);
	}

	MAChangeGuildWarStep ChangeGuildWarStep;
	memset(&ChangeGuildWarStep, 0, sizeof(ChangeGuildWarStep));

	ChangeGuildWarStep.wScheduleID = g_pGuildWarManager->GetScheduleID();
	ChangeGuildWarStep.cEventType = GUILDWAR_EVENT_START;
	ChangeGuildWarStep.cEventStep = cStepIndex;
	ChangeGuildWarStep.wWinersWeightRate = g_pGuildWarManager->GetWinersWeightRate();
	pCon->SendChangeGuildWarStep(&ChangeGuildWarStep);

	// 본선이면 본선정보 보내줌.
	if( cStepIndex == GUILDWAR_STEP_REWARD )	
	{		
		pCon->SendSetGuildwarFinalProcess(g_pGuildWarManager->GetCurFinalPart(), g_pGuildWarManager->GetCurFinalPartBeginTime());
	}

#if defined(PRE_FIX_75807)
#else
	// 본선이 아니거나 차수가 진행중이 아니고 지난대회 우승팀이 있으면 보냄
	if( (cStepIndex != GUILDWAR_STEP_REWARD || !g_pGuildWarManager->GetFinalStart() )  && g_pGuildWarManager->GetPreWinGuild().IsSet() )
	{
		if( g_pGuildWarManager->GetFinalWinGuildReward() ) // 보상이 지급되었는지 확인하고
		{
			MAGuildWarPreWinGuild GuildWarPreWinGuild;
			GuildWarPreWinGuild.GuildUID = g_pGuildWarManager->GetPreWinGuild();
			GuildWarPreWinGuild.bPreWin = true;
			pCon->SendSetGuildWarPreWinGuild(&GuildWarPreWinGuild);
		}		
	}
#endif //#if defined(PRE_FIX_75807)

	BYTE cSecretTeam = g_pGuildWarManager->GetSecretTeam();
	if (cSecretTeam > 0)
	{
		MASetGuildWarSecretMission SecretMission;
		SecretMission.cTeamCode = cSecretTeam;
		SecretMission.nRandomSeed = g_pGuildWarManager->GetSecretRandomSeed();
		pCon->SendSetGuildWarSecretMission(&SecretMission);
	}
}

void CDivisionManager::SendSetGuildWarSecretMission(MASetGuildWarSecretMission * pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{			
		(*itorv)->SendSetGuildWarSecretMission(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{		
		(*itorg)->SendSetGuildWarSecretMission(pPacket);
	}
}

void CDivisionManager::SendSetGuildWarFinalResult(MAVISetGuildWarFinalResult* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorg;
	for (itorg = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorg ; ++itorg) 
	{			
		(*itorg)->SendSetGuildWarFinalResult(pPacket);
	}
}

void CDivisionManager::SendSetGuildWarPreWinSkillCoolTime(MAGuildWarPreWinSkillCoolTime* pPacket)
{
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{
		if (0 != (*itor)->GetManagedID() && pPacket->nManagedID == (*itor)->GetManagedID())
			continue;

		(*itor)->SendSetGuildWarPreWinSkillCoolTime(pPacket);
	}
}

void CDivisionManager::SendSetGuildWarSchedule(MASetGuildWarEventTime* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itor;
	for (itor = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itor ; ++itor) 
	{			
		(*itor)->SendSetGuildWarSchedule(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{		
		(*itorg)->SendSetGuildWarSchedule(pPacket);
	}
}

void CDivisionManager::SendSetGuildWarTournamentWin(MASetGuildWarTournamentWin* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{			
		(*itorv)->SendSetGuildWarTournamentWin(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{		
		(*itorg)->SendSetGuildWarTournamentWin(pPacket);
	}
}

void CDivisionManager::SendResetGuildWarBuyedItemCount()
{
	for each (CDNVillageConnection* pConnection in m_VillageServerConList)
	{
		pConnection->SendResetGuildWarBuyedItemCount();
	}
}

void CDivisionManager::SendGuildWarAllStop()
{
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{		
		(*itorg)->SendGuildWarAllStop();
	}
}

void CDivisionManager::SendGuildRecruitMember(MAGuildRecruitMember* pPacket)
{
	// 길드초대할 유저를 찾는다
	CDNUser * pUser = GetUserByCharacterDBID(pPacket->biCharacterDBID);

	if( pUser )
	{
		pPacket->uiAccountDBID = pUser->GetAccountDBID();
		switch( pUser->GetUserState() )
		{
		case STATE_VILLAGE:
			{
				// 해당 빌리지 서버에 있는 유저일 경우 길드가입 알림
				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID (pUser->GetVillageID());
				if( pVillageCon )
				{
					pVillageCon->SendGuildRecruitMember(pPacket);
					return;
				}
			}
			break;
		case STATE_GAME:
			{
				// 해당 게임 서버에 있는 유저일 경우 길드가입 알림
				CDNGameConnection* pGameCon = GetGameConnectionByGameID (pUser->GetGameID());
				if( pGameCon )
				{
					pGameCon->SendGuildRecruitMember(pPacket);
					return;
				}
			}
			break;
		default:
			break;
		}
	}
	else 
		return;

	return;
}

void CDivisionManager::SendAddGuildRewardItem(MAGuildRewardItem* pPacket)
{	
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID())
			continue;

		(*itorv)->SendAddGuildRewardItem(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID())
			continue;

		(*itorg)->SendAddGuildRewardItem(pPacket);
	}	
}
void CDivisionManager::SendExtendGuildSize(MAExtendGuildSize* pPacket)
{	
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID())
			continue;

		(*itorv)->SendExtendGuildSize(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID())
			continue;

		(*itorg)->SendExtendGuildSize(pPacket);
	}	
}

void CDivisionManager::SendSetGuildWarTrialRanking(MASetGuildWarPointRunningTotal* pPacket)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{			
		(*itorv)->SendSetGuildWarPointRunning(pPacket);
	}
}
void CDivisionManager::SendGuildWarRefreshGuildPoint(MAGuildWarRefreshGuildPoint* pPacket)
{
	// 일단 전체 VI 에만 송신..게임쪽은 필요없을듯..
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{			
		(*itorv)->SendGuildWarRefreshGuildPoint(pPacket);
	}
}

void CDivisionManager::SendUpdateGuildWare(int nGuildID)
{
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{
		(*itorv)->SendUpdateGuildWare(nGuildID);
	}
	//// 전체 GA 에 송신
	//vector<CDNGameConnection*>::iterator itorg;
	//for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	//{
	//	(*itorg)->SendUpdateGuildWare(nGuildID);
	//}	
}

void CDivisionManager::SendChangeCharacterName(MAChangeCharacterName* pPacket)
{
	CDNUser *pSender = GetUserByAccountDBID(pPacket->uiAccountDBID);
	if (pSender)
		ReplaceCharacterName(pSender, pPacket->wszCharacterName);
	
#if defined (PRE_ADD_BESTFRIEND)
	CDNUser *pBestFriend = GetUserByCharacterDBID(pPacket->biBFCharacterDBID);
	if (pBestFriend)
		pPacket->uiBFAccountDBID = pBestFriend->GetAccountDBID();
#endif // #if defined (PRE_ADD_BESTFRIEND)
	
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) 
	{
		if (0 != (*itorv)->GetManagedID() && pPacket->nManagedID == (*itorv)->GetManagedID())
			continue;

		(*itorv)->SendChangeCharacterName(pPacket);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) 
	{
		if (0 != (*itorg)->GetManagedID() && pPacket->nManagedID == (*itorg)->GetManagedID())
			continue;
		
		(*itorg)->SendChangeCharacterName(pPacket);
	}
}

void CDivisionManager::SendFriendAddNotice(UINT nAddedAccountDBID, const WCHAR * pAddName)
{
	CDNUser * pUser = GetUserByAccountDBID(nAddedAccountDBID);
	if (pUser)
	{
		//이동 중일 경우와 오프라인은 무시 한다.
		if (pUser->GetUserState() == STATE_GAME || pUser->GetUserState() == STATE_VILLAGE)
		{
			ScopeLock<CSyncLock> Lock(m_Sync);

			if (pUser->GetUserState() == STATE_GAME)
			{
				CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
				if (pGameCon)
					pGameCon->SendFriendAddNotice(pUser->GetAccountDBID(), pAddName);
			}
			else if (pUser->GetUserState() == STATE_VILLAGE)
			{
				CDNVillageConnection * pVillageCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
				if (pVillageCon)
					pVillageCon->SendFriendAddNotice(pUser->GetAccountDBID(), pAddName);
			}
		}
	}
}

bool CDivisionManager::RequestGameRoom( BYTE cReqGameIDType, UINT uiIndex, int nRandomSeed, int nMapIndex, char cGateNo, char cStageConstructionLevel, bool bDirectConnect, bool bTutorial, int nServerID /* 튜토리얼일 경우만 LO ID 입력 */)
{
	//튜토리얼과 PvP만이걸로
	int nGameServerID = 0;
	CDNGameConnection* pGameCon = GetFreeGameConnection(&nGameServerID, cReqGameIDType);
	if( !pGameCon ) 
		return false;

	if( cReqGameIDType == REQINFO_TYPE_SINGLE )
	{
		CDNUser* pUser = GetUserByAccountDBID(uiIndex);
		if( pUser )
		{
			if (pUser->SetCheckGameInfo( pGameCon->GetGameID(), -1 ) == false)
				return false;

			pUser->m_nRandomSeed	= nRandomSeed;
			pUser->m_nMapIndex		= nMapIndex;
			pUser->m_cGateNo		= cGateNo;

			if (bTutorial)	{ // login에서 보낸거
				pGameCon->SendReqTutorialRoomID(pUser, nServerID);
			}
			else
			{
#if defined( STRESS_TEST )
				pGameCon->SendReqRoomID( GameTaskType::Normal, pUser, cStageConstructionLevel, 0, true );
				return true;
#endif
				_DANGER_POINT();
				return false;
			}
			return true;
		}
	}
	else if( cReqGameIDType == REQINFO_TYPE_PVP || cReqGameIDType == REQINFO_TYPE_PVP_BREAKINTO)
	{
		CDNPvP* pPvPRoom = GetPvPRoomByIdx( uiIndex );
		if( pPvPRoom )
		{
			pPvPRoom->SetAllGameID( pGameCon->GetGameID(), nRandomSeed, nMapIndex, cGateNo );
			if (pGameCon->SendReqRoomID( pPvPRoom, bDirectConnect ) == false)
				return false;

			return true;
		}
		_DANGER_POINT();
	}
	else
		_DANGER_POINT();

	return false;
}

bool CDivisionManager::RequestGameRoom(VIMAReqGameID * pPacket)
{
	int nGameServerID = 0;
#if defined( PRE_FIX_WORLDCOMBINEPARTY )
	CDNGameConnection* pGameCon = NULL;
	if( Party::bIsWorldCombineParty(pPacket->PartyData.PartyData.Type) )
	{
		 pGameCon = GetWorldCombineGameConnection();
	}
	else
		pGameCon = GetFreeGameConnection(&nGameServerID, pPacket->cReqGameIDType);
#else	// #if defined( PRE_FIX_WORLDCOMBINEPARTY )
	CDNGameConnection* pGameCon = GetFreeGameConnection(&nGameServerID, pPacket->cReqGameIDType);
#endif	// #if defined( PRE_FIX_WORLDCOMBINEPARTY )
	if( !pGameCon ) 
		return false;

	if( pPacket->cReqGameIDType == REQINFO_TYPE_SINGLE )
	{
		CDNUser* pUser = GetUserByAccountDBID(static_cast<UINT>(pPacket->InstanceID));
		if( pUser )
		{
			//강제할당처리 검사
			int nSID = 0;
			if (IsSelectJoin(pUser->GetCharacterName(), nSID))
			{
				pGameCon = GetGameConnectionByManagedID(nSID);
				if( !pGameCon ) 
					return false;
			}

			if (pUser->SetCheckGameInfo( pGameCon->GetGameID(), -1 ) == false)
				return false;

			pUser->m_nRandomSeed	= pPacket->nRandomSeed;
			pUser->m_nMapIndex		= pPacket->nMapIndex;
			pUser->m_cGateNo		= pPacket->cGateNo;
			pUser->m_cGateSelect	= pPacket->cGateSelect;

			const TChannelInfo * pInfo = GetChannelInfo(pUser->GetVillageID(), pUser->GetChannelID());
			if (pInfo)
			{
				bool bRet = pGameCon->SendReqRoomID( pPacket->GameTaskType, pUser, pPacket->StageDifficulty, pInfo->nMeritBonusID, pPacket->bDirectConnect );
				if( !bRet )
				{
					g_Log.Log( LogType::_GAMECONNECTLOG, pUser, L"SendReqRoomID() Failed!!\n" );
				}
				return bRet;
			}
			else
			{
#if defined( STRESS_TEST )
				pGameCon->SendReqRoomID( GameTaskType::Normal, pUser, 0, 0, true );
				return true;
#endif
				_DANGER_POINT();
				return false;
			}
		}
		_DANGER_POINT();
	}
	else if( pPacket->cReqGameIDType == REQINFO_TYPE_PARTY || pPacket->cReqGameIDType == REQINFO_TYPE_PARTY_BREAKINTO )
	{
		CDNUser * pUser = NULL;
#if defined( PRE_PARTY_DB )
		for (int i = 0; i < pPacket->PartyData.PartyData.iCurMemberCount; i++)
#else
		for (int i = 0; i < pPacket->PartyData.cMemberCount; i++)
#endif
		{
			pUser = GetUserByAccountDBID(pPacket->PartyData.MemberInfo[i].nAccountDBID);
			if (pUser)
			{
				if (pUser->SetCheckGameInfo( pGameCon->GetGameID(), -1 ) == false)
					continue;

				pUser->m_VillageCheckPartyID = pPacket->InstanceID;
				pUser->m_nRandomSeed	= pPacket->nRandomSeed;
				pUser->m_nMapIndex		= pPacket->nMapIndex;
				pUser->m_cGateNo		= pPacket->cGateNo;
				pUser->m_cGateSelect	= pPacket->cGateSelect;
			}
			else
				_DANGER_POINT();
		}		
#if defined( PRE_WORLDCOMBINE_PARTY )
		if( Party::bIsWorldCombineParty(pPacket->PartyData.PartyData.Type) )
		{
			const TChannelInfo * pInfo = GetChannelInfo( m_cPvPLobbyVillageID, m_unPvPLobbyChannelID );
			if (pInfo)
				pGameCon->SendReqRoomID(pPacket, pInfo->nMeritBonusID);
			else
			{
				_DANGER_POINT();
				return false;
			}
		}
		else
		{
			if (pUser == NULL)
			{
				_DANGER_POINT();
				return false;
			}

			const TChannelInfo * pInfo = GetChannelInfo(pUser->GetVillageID(), pUser->GetChannelID());
			if (pInfo)
				pGameCon->SendReqRoomID(pPacket, pInfo->nMeritBonusID);
			else
			{
				_DANGER_POINT();
				return false;
			}
		}
#else
		if (pUser == NULL)
		{
			_DANGER_POINT();
			return false;
		}

		const TChannelInfo * pInfo = GetChannelInfo(pUser->GetVillageID(), pUser->GetChannelID());
		if (pInfo)
			pGameCon->SendReqRoomID(pPacket, pInfo->nMeritBonusID);
		else
		{
			_DANGER_POINT();
			return false;
		}
#endif
		return true;
	}
	else if( pPacket->cReqGameIDType == REQINFO_TYPE_PVP )
	{
		CDNPvP* pPvPRoom = GetPvPRoomByIdx( static_cast<UINT>(pPacket->InstanceID) );
		if( pPvPRoom )
		{
			pPvPRoom->SetAllGameID( pGameCon->GetGameID(), pPacket->nRandomSeed, pPacket->nMapIndex, pPacket->cGateNo );
			pGameCon->SendReqRoomID( pPvPRoom, pPacket->bDirectConnect );
			return true;
		}
		_DANGER_POINT();
	}
	else if (pPacket->cReqGameIDType == REQINFO_TYPE_FARM)
	{
		CDNUser * pFarmUser = GetUserByAccountDBID(static_cast<UINT>(pPacket->InstanceID));
		if (pFarmUser == NULL)
		{
			_DANGER_POINT();		//이럼 안데시어요.
			return false;
		}

		//농장의 생성은 유저의 요청에서는 없다!
		bool bAlreadyCreated = false;
		CDNFarm * pFarm = GetFarm(pPacket->nFarmDBID);
		if (pFarm)
		{
			if (pFarm->GetFarmStatus() == FARMSTATUS_PLAY)
			{
				int nFarmGameServerID = -1;
				int nFarmGameServerThreadIdx = -1;
				UINT nFarmRoomID;
				pFarm->GetAssignedServerInfo(nFarmGameServerID, nFarmGameServerThreadIdx, nFarmRoomID);

				pGameCon = GetGameConnectionByGameID(nFarmGameServerID);
				if (pGameCon)
				{
					pGameCon->SendBreakintoRoom( nFarmRoomID, pFarmUser, BreakInto::Type::None );
					return true;
				}
			}
			else
			{
				if (pFarm->GetFarmStatus() != FARMSTATUS_DESTROY)
				{
					if (pFarm->VerifyWaitAndPush(const_cast<const VIMAReqGameID*>(pPacket)))
						return true;
					else
						_DANGER_POINT();		//이런경우는 나오면 안덴다.
				}

				return false;
			}
		}
		_DANGER_POINT();		//헐... 이러면 생성도중 또는 등등의 상황에 서버가 죽었을 경우이다.
		return false;
	}
	else
		_DANGER_POINT();
	return false;
}

bool CDivisionManager::RequestGameRoom( VIMALadderReqGameID* pPacket )
{
	int nGameServerID = 0;
	CDNGameConnection* pGameCon = GetFreeGameConnection(&nGameServerID, REQINFO_TYPE_LADDER );
	if( !pGameCon ) 
		return false;

	// AllUser 검사
	for( int i=0 ; i<pPacket->LadderData.cMemberCount ; ++i )
	{
		CDNUser* pUser = GetUserByAccountDBID( pPacket->LadderData.MemberInfoArr[i].uiAccountDBID );
		if( pUser == NULL )
		{
			_DANGER_POINT();
			return false;
		}
	}

	for( int i=0 ; i<pPacket->LadderData.cMemberCount ; ++i )
	{
		CDNUser* pUser = GetUserByAccountDBID( pPacket->LadderData.MemberInfoArr[i].uiAccountDBID );
		if (pUser->SetCheckGameInfo( pGameCon->GetGameID(), -1 ) == false)
			continue;

		pUser->m_nRandomSeed	= pPacket->iRandomSeed;
		pUser->m_nMapIndex		= pPacket->iMapIndex;
	}
	
	pGameCon->SendLadderReqRoomID( pPacket );
	return true;
}

#if defined( PRE_WORLDCOMBINE_PVP )

bool CDivisionManager::RequestGameRoom( VIMACreateWorldPvPRoom* pPacket, BYTE cVillageID )
{
	int nGameServerID = 0;
	CDNGameConnection* pGameCon = GetWorldCombineGameConnection();
	if( !pGameCon ) 
		return false;
	
	pGameCon->SendWorldPvPReqRoomID( pPacket, cVillageID );
	return true;
}

#endif

#if defined( PRE_ADD_FARM_DOWNSCALE )
bool CDivisionManager::RequestFarmGameRoom(UINT nFarmDBID, int nMapID, int nMaxUser, bool bStart, int &nServerID, int iAttr )
#elif defined( PRE_ADD_VIP_FARM )
bool CDivisionManager::RequestFarmGameRoom(UINT nFarmDBID, int nMapID, int nMaxUser, bool bStart, int &nServerID, Farm::Attr::eType Attr )
#else
bool CDivisionManager::RequestFarmGameRoom(UINT nFarmDBID, int nMapID, int nMaxUser, bool bStart, int &nServerID)
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

{
	CDNGameConnection* pGameCon = NULL;
	if (m_nFarmGameindex == -1)
	{
#ifdef PRE_MOD_OPERATINGFARM
		if (IsFarmConnectionWatingTime(timeGetTime()) && g_Config.nFarmServerID > 0)
			pGameCon = GetFarmGameConnection(&nServerID);
		else
			pGameCon = GetFreeGameConnection(&nServerID, REQINFO_TYPE_FARM);
#else		//#ifdef PRE_MOD_OPERATINGFARM
		pGameCon = GetFreeGameConnection(&nServerID, REQINFO_TYPE_FARM);
#endif		//#ifdef PRE_MOD_OPERATINGFARM
	}
	else
		pGameCon = GetGameConnectionByGameID(m_nFarmGameindex);

	if( !pGameCon ) 
	{
		if (m_nFarmGameindex != -1)
			m_nFarmGameindex = -1;
			return false;
	}

	m_nFarmGameindex = nServerID = pGameCon->GetGameID();
#if defined( PRE_ADD_FARM_DOWNSCALE )
	return pGameCon->SendReqFarmRoomID(nFarmDBID, nMapID, nMaxUser, bStart, iAttr );
#elif defined( PRE_ADD_VIP_FARM )
	return pGameCon->SendReqFarmRoomID(nFarmDBID, nMapID, nMaxUser, bStart, Attr );
#else
	return pGameCon->SendReqFarmRoomID(nFarmDBID, nMapID, nMaxUser, bStart);
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

}

void CDivisionManager::ExceptionPvPRoom(GAMASetRoomID* pMsg, USHORT& unVillageChannelID)
{
	if (pMsg->cReqGameIDType != REQINFO_TYPE_PVP)
		return;

	CDNPvP* pPvPRoom = GetPvPRoomByIdx( static_cast<UINT>(pMsg->InstanceID) );
	if (pPvPRoom)
	{
		unVillageChannelID = pPvPRoom->GetVillageChannelID();
		UINT uiRoomState = pPvPRoom->GetRoomState();
		UINT uiNewRoomState = uiRoomState&PvPCommon::RoomState::Password;
		pPvPRoom->SetRoomState( uiNewRoomState );
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"ExceptionRoom [Index:%d][Room:%d][Event:%d] \r\n", pPvPRoom->GetIndex(), pPvPRoom->GetGameServerRoomIndex(), pPvPRoom->GetEventRoomIndex());
	}
	else
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"ExceptionRoom can't find PvPRoom [ID:%d] \r\n", pMsg->InstanceID);
}

bool CDivisionManager::SetGameRoom( CDNGameConnection* pGameCon, GAMASetRoomID* pMsg )
{
	CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID(pMsg->cVillageID);
	if( pVillageCon )
	{
		if (pMsg->iRoomID <= 0)
		{
			USHORT	unVillageChannelID = 0;
			ExceptionPvPRoom(pMsg, unVillageChannelID);

			//게임서버에서 방생성 실패했삼!
#if defined( PRE_WORLDCOMBINE_PARTY )
#if defined( PRE_WORLDCOMBINE_PVP )
			pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, 0, 0, 0, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, unVillageChannelID, pMsg->cVillageID, -1, pMsg->Type, pMsg->eWorldReqType );
#else // #if defined( PRE_WORLDCOMBINE_PVP )
			pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, 0, 0, 0, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, unVillageChannelID, pMsg->cVillageID, -1, pMsg->Type );
#endif // #if defined( PRE_WORLDCOMBINE_PVP )
#else // #if defined( PRE_WORLDCOMBINE_PARTY )
			pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, 0, 0, 0, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, unVillageChannelID, pMsg->cVillageID, -1 );
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
			return false;
		}

		if( pMsg->cReqGameIDType == REQINFO_TYPE_PARTY || pMsg->cReqGameIDType == REQINFO_TYPE_LADDER )
		{
			for( int i=0 ; i<_countof(pMsg->nRoomMember) ; ++i )
			{
				if( pMsg->nRoomMember[i] <= 0 ) 
					continue;
				if( VillageToGame( pVillageCon, pMsg->nRoomMember[i], pGameCon->GetGameID(), pMsg->iRoomID, pMsg->cServerIdx ) == false )
					_DANGER_POINT();		//어어어어어 이거봐라...
			}

			char szIP[IPLENMAX] = {0,};
			USHORT nPort, nTcpPort;
			nPort = nTcpPort = 0;
			if( GetGameInfoByID( pMsg->cGameID, pMsg->cServerIdx, szIP, &nPort, &nTcpPort ) == true )
			{
				if( pMsg->cReqGameIDType == REQINFO_TYPE_PARTY )
				{				
#if defined( PRE_WORLDCOMBINE_PARTY )
					pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, _inet_addr(szIP), nPort, nTcpPort, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, 0, pMsg->cVillageID, ERROR_NONE, pMsg->Type );
#else
					pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, _inet_addr(szIP), nPort, nTcpPort, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, 0, pMsg->cVillageID, ERROR_NONE );
#endif
				}
				else	// REQINFO_TYPE_LADDER
				{
					const TPvPGameModeTable* pPvPGameModeTable = g_pExtManager->GetPvPGameModeTableByMatchType( pMsg->MatchType );
					if( pPvPGameModeTable )
					{
						pGameCon->SendPvPLadderGameMode( pMsg, pPvPGameModeTable );
						pVillageCon->SendLadderSetGameID( _inet_addr(szIP), nPort, nTcpPort, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, 0, pMsg->cVillageID, ERROR_NONE, pMsg, pPvPGameModeTable->nItemID );
					}
					else
					{
						GAMASetRoomID TempPacket;
						memset( &TempPacket, 0, sizeof(TempPacket) );

						for( int i=0 ; i<_countof(pMsg->nRoomMember) ; ++i )
						{
							if( pMsg->nRoomMember[i] <= 0 ) 
								continue;
							TempPacket.nRoomMember[i] = pMsg->nRoomMember[i];
						}

						pVillageCon->SendLadderSetGameID( 0, 0, 0, 0, 0, 0, 0, 0, -1, &TempPacket );
					}
				}
				return true;
			}
		}
		else if( pMsg->cReqGameIDType == REQINFO_TYPE_PVP )
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( pMsg->eWorldReqType > WorldPvPMissionRoom::Common::NoneType )
			{
				return true;
			}
#endif
			CDNPvP* pPvPRoom = GetPvPRoomByIdx( static_cast<UINT>(pMsg->InstanceID) );
			if( pPvPRoom )
			{
				if (pPvPRoom->GetEventRoomIndex() > 0)
					g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"SetGameRoom [Index:%d][Room:%d][Event:%d] \r\n", pPvPRoom->GetIndex(), pPvPRoom->GetGameServerRoomIndex(), pPvPRoom->GetEventRoomIndex());

				pPvPRoom->SetAllRoomID( pVillageCon, pGameCon, pMsg->iRoomID, pMsg->cServerIdx );

				char szIP[IPLENMAX] = {0,};
				USHORT nPort, nTcpPort;
				nPort = nTcpPort = 0;
				if( GetGameInfoByID( pMsg->cGameID, pMsg->cServerIdx, szIP, &nPort, &nTcpPort ) == true )
				{
					pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, _inet_addr(szIP), nPort, nTcpPort, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, pPvPRoom->GetVillageChannelID(), pMsg->cVillageID, ERROR_NONE );
					return true;
				}
			}
			else
				_DANGER_POINT();
		}
		else if( pMsg->cReqGameIDType == REQINFO_TYPE_SINGLE || pMsg->cReqGameIDType == REQINFO_TYPE_PVP_BREAKINTO || pMsg->cReqGameIDType == REQINFO_TYPE_PARTY_BREAKINTO || pMsg->cReqGameIDType == REQINFO_TYPE_LADDER_OBSERVER )
		{
			if( VillageToGame( pVillageCon, static_cast<UINT>(pMsg->InstanceID), pGameCon->GetGameID(), pMsg->iRoomID, pMsg->cServerIdx ) == true )
			{
				if (pMsg->cReqGameIDType == REQINFO_TYPE_PARTY_BREAKINTO)
				{
					CDNUser * pBreakUser = GetUserByAccountDBID(static_cast<UINT>(pMsg->InstanceID));
					if (pBreakUser)
					{
						pBreakUser->m_VillageCheckPartyID = pMsg->PartyIDForBreakInto;
					}
					else
						_DANGER_POINT();		//여이가 유저가 없으면 문제가 있다!!!! 게임서버에는 정상 진입되어지지만 마을로 돌아갈경우(파티전체가)파티에 속하지 않는 것처럼 보임
				}

				char szIP[IPLENMAX] = {0,};
				USHORT nPort, nTcpPort;
				nPort = nTcpPort = 0;
				if( GetGameInfoByID( pMsg->cGameID, pMsg->cServerIdx, szIP, &nPort, &nTcpPort ) == true )
				{
					pVillageCon->SendSetGameID( pMsg->GameTaskType, pMsg->cReqGameIDType, pMsg->InstanceID, _inet_addr(szIP), nPort, nTcpPort, pMsg->cServerIdx, pGameCon->GetGameID(), pMsg->iRoomID, 0, pMsg->cVillageID, ERROR_NONE );
					return true;
				}
			}
		}
		else
			_DANGER_POINT();
	}

#if !defined( STRESS_TEST )
	_DANGER_POINT();
#endif

	return false;
}

bool CDivisionManager::SetTutorialGameRoom(int nLocalGameID, UINT nAccountDBID, int nGameID, int nServerIdx, int nRoomID, int nLoginServerID)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (!pUser) return false;
	
	if (pUser->SetCheckGameInfo(nLocalGameID, nServerIdx) == false)
		return false;
	pUser->m_nRoomID = nRoomID;

	char szIP[IPLENMAX] = {0,};
	USHORT nPort, nTcpPort;
	nPort = nTcpPort = 0;
	if (GetGameInfoByID(nGameID, nServerIdx, szIP, &nPort, &nTcpPort) == true)
	{
		for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
			CDNLoginConnection * pLoginCon = m_pLoginConnectionList[nIndex];
			if (!pLoginCon) {
				continue;
			}

			if (pLoginCon->GetServerID() == nLoginServerID) {
				pLoginCon->SendSetTutorialGameID(pUser->GetSessionID(), _inet_addr(szIP), nPort, nTcpPort, nServerIdx, nLocalGameID, nRoomID, ERROR_NONE);
//				break;	// P.S.> 만약 ServerID 가 0 이라면 전부 찾아서 보내야 될 수도 있음 ???
			}
		}
		return true;
	}

	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) {
		CDNLoginConnection * pLoginCon = m_pLoginConnectionList[nIndex];
		if (!pLoginCon) {
			continue;
		}

		if (pLoginCon->GetServerID() == nLoginServerID) {
			pLoginCon->SendSetTutorialGameID(pUser->GetSessionID(), _inet_addr(szIP), nPort, nTcpPort, nServerIdx, nLocalGameID, nRoomID, ERROR_GENERIC_GAMECON_NOT_FOUND);
//			break;	// P.S.> 만약 ServerID 가 0 이라면 전부 찾아서 보내야 될 수도 있음 ???
		}
	}
	return false;
}

#ifdef PRE_ADD_DOORS
int CDivisionManager::PrivateChat(UINT nSenderAccountDBID, const WCHAR * pReceiverName, char cType, const WCHAR * pChatMsg, int nLen, INT64 biDoorDestCharacterDBID)
#else		//#ifdef PRE_ADD_DOORS
int CDivisionManager::PrivateChat(UINT nSenderAccountDBID, const WCHAR * pReceiverName, char cType, const WCHAR * pChatMsg, int nLen)
#endif		//#ifdef PRE_ADD_DOORS
{
	CDNUser * pSender = GetUserByAccountDBID(nSenderAccountDBID);
	if (pSender == NULL) return ERROR_CHAT_USERNOTFOUND;

	CDNUser * pReceiver = GetUserByName(pReceiverName);
	if (pReceiver == NULL)
	{
#ifdef PRE_ADD_DOORS
		if (biDoorDestCharacterDBID > 0 && g_pDoorsConnection && g_pDoorsConnection->GetActive())
		{
			g_pDoorsConnection->SendChatToDoors(pSender, biDoorDestCharacterDBID, pChatMsg, nLen);
			return ERROR_CHAT_MOBILESENDED;
		}
#endif		//#ifdef PRE_ADD_DOORS
		return ERROR_CHAT_USERNOTFOUND;
	}

#if defined(PRE_ADD_DWC)
	if (pReceiver->GetCharacterAccountLevel() >= AccountLevel_New && pReceiver->GetCharacterAccountLevel() <= AccountLevel_QA)
		return ERROR_CHAT_USERNOTFOUND;
#else
	if (pReceiver->GetAccountLevel() >= AccountLevel_New && pReceiver->GetAccountLevel() <= AccountLevel_QA)
		return ERROR_CHAT_USERNOTFOUND;
#endif

#if defined(PRE_ADD_DWC)
	//같은 타입끼리만 대화 가능
	if( (pReceiver->GetCharacterAccountLevel() == AccountLevel_DWC) ^ (cType == CHATTYPE_DWC_PRIVATE) )
		return ERROR_CHAT_USERNOTFOUND;
	if(cType == CHATTYPE_DWC_PRIVATE)
		cType = CHATTYPE_PRIVATE;	//귓속말로 전환해서 쏜다.
#endif

	if (pReceiver->GetUserState() == STATE_VILLAGE)
	{
		CDNVillageConnection * pVillageCon = pReceiver->GetCurrentVillageConnection();
		if (pVillageCon != NULL)
		{
			pVillageCon->SendPrivateChat(pReceiver->GetAccountDBID(), pSender->GetCharacterName(), cType, pChatMsg, nLen);
			return ERROR_NONE;
		}
	}
	else if (pReceiver->GetUserState() == STATE_GAME)
	{
		CDNGameConnection * pGameCon = pReceiver->GetCurrentGameConnection();
		if (pGameCon != NULL)
		{
			pGameCon->SendPrivateChat(pReceiver->GetAccountDBID(), pSender->GetCharacterName(), cType, pChatMsg, nLen);
			return ERROR_NONE;
		}
	}
	return ERROR_CHAT;
}

int CDivisionManager::ZoneChat(UINT nSenderAccountDBID, const WCHAR * pChatMsg, int nLen, int nMapIdx)
{
	CDNUser * pSender = GetUserByAccountDBID(nSenderAccountDBID);
	if (pSender == NULL || nMapIdx == -1) return ERROR_CHAT_USERNOTFOUND;
	
	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if ((*ii)->HasManagedMapIdx(nMapIdx))		//맞으면 쏘세요~
			(*ii)->SendZoneChat(nMapIdx, pSender->GetCharacterName(), pChatMsg, nLen);
	}

	return ERROR_CHAT;
}

#if defined(PRE_ADD_WORLD_MSG_RED)
int CDivisionManager::WorldChat(UINT nSenderAccountDBID, const WCHAR * pChatMsg, int nLen, char cChatType /*= CHATTYPE_WORLD */)
#else	// #if defined(PRE_ADD_WORLD_MSG_RED)
int CDivisionManager::WorldChat(UINT nSenderAccountDBID, const WCHAR * pChatMsg, int nLen)
#endif	// // #if defined(PRE_ADD_WORLD_MSG_RED)
{
	//빌리지만 해당합니다 각 빌리지에 쏴악 쏩니다.
	CDNUser * pSender = GetUserByAccountDBID(nSenderAccountDBID);
	if (pSender == NULL) return ERROR_CHAT_USERNOTFOUND;

	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
#if defined(PRE_ADD_WORLD_MSG_RED)
		(*ii)->SendChat(cChatType, nSenderAccountDBID, pSender->GetCharacterName(), pChatMsg, nLen);
#else	// #if defined(PRE_ADD_WORLD_MSG_RED)
		(*ii)->SendChat(CHATTYPE_WORLD, nSenderAccountDBID, pSender->GetCharacterName(), pChatMsg, nLen);
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)

	vector<CDNGameConnection*>::iterator it;
	for (it = m_GameServerConList.begin(); it != m_GameServerConList.end(); ++it)
#if defined(PRE_ADD_WORLD_MSG_RED)
		(*it)->SendChat(cChatType, nSenderAccountDBID, pSender->GetCharacterName(), pChatMsg, nLen);
#else	// #if defined(PRE_ADD_WORLD_MSG_RED)
		(*it)->SendChat(CHATTYPE_WORLD, nSenderAccountDBID, pSender->GetCharacterName(), pChatMsg, nLen);
#endif	// #if defined(PRE_ADD_WORLD_MSG_RED)
	

	return ERROR_NONE;
}

int CDivisionManager::WorldSystemMsg(UINT nSenderAccountDBID, char cType, int nID, int nValue, WCHAR* pwszToCharacterName)
{
	CDNUser *pSender = GetUserByAccountDBID(nSenderAccountDBID);
	if (!pSender) return ERROR_CHAT_USERNOTFOUND;

	vector<CDNVillageConnection*>::iterator iter;
	for (iter = m_VillageServerConList.begin(); iter != m_VillageServerConList.end(); ++iter){
		(*iter)->SendWorldSystemMsg(pSender->GetCharacterName(), cType, nID, nValue, pwszToCharacterName);
	}

	vector<CDNGameConnection*>::iterator it;
	for (it = m_GameServerConList.begin(); it != m_GameServerConList.end(); ++it){
		(*it)->SendWorldSystemMsg(pSender->GetCharacterName(), cType, nID, nValue, pwszToCharacterName);
	}

	return ERROR_NONE;
}

#ifdef PRE_ADD_DOORS
int CDivisionManager::DoorsChat(const WCHAR * pSenderName, INT64 biCharacterID, const WCHAR * pMessage, int nLen)
{
	CDNUser *pReceiver = GetUserByCharacterDBID(biCharacterID);
	if (!pReceiver) return ERROR_CHAT_USERNOTFOUND;

	if (pReceiver->GetAccountLevel() >= AccountLevel_New && pReceiver->GetAccountLevel() <= AccountLevel_QA)
		return ERROR_CHAT_USERNOTFOUND;

	if (pReceiver->GetUserState() == STATE_VILLAGE)
	{
		CDNVillageConnection * pVillageCon = pReceiver->GetCurrentVillageConnection();
		if (pVillageCon != NULL)
		{
			pVillageCon->SendPrivateChat(pReceiver->GetAccountDBID(), pSenderName, static_cast<char>(CHATTYPE_PRIVATE_MOBILE), pMessage, nLen);
			return ERROR_NONE;
		}
	}
	else if (pReceiver->GetUserState() == STATE_GAME)
	{
		CDNGameConnection * pGameCon = pReceiver->GetCurrentGameConnection();
		if (pGameCon != NULL)
		{
			pGameCon->SendPrivateChat(pReceiver->GetAccountDBID(), pSenderName, static_cast<char>(CHATTYPE_PRIVATE_MOBILE), pMessage, nLen);
			return ERROR_NONE;
		}
	}

	return ERROR_NONE;
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
int CDivisionManager::DoorsGuildChat(INT64 biCharacterID, int nGuildID, const WCHAR * pMessage, int nLen)
{
	if (pMessage == NULL || biCharacterID <= 0) return ERROR_DB;

	MADoorsGuildChat packet;
	memset(&packet, 0, sizeof(MADoorsGuildChat));

	packet.nManagedID = g_Config.nManagedID;
	packet.GuildUID.Set(g_Config.nWorldSetID, static_cast<UINT>(nGuildID));
	packet.nCharacterDBID = biCharacterID;

	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv) {
		(*itorv)->SendDoorsGuildChat(&packet);
	}
	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator itorg;
	for (itorg = m_GameServerConList.begin() ; m_GameServerConList.end() != itorg ; ++itorg) {
		(*itorg)->SendDoorsGuildChat(&packet);
	}
	return ERROR_NONE;
}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
#endif		//#ifdef PRE_ADD_DOORS

#if defined( PRE_PRIVATECHAT_CHANNEL )
int CDivisionManager::PrivateChannelChat(UINT nSenderAccountDBID, const WCHAR * pChatMsg, int nLen, INT64 nChannelID)
{	
	CDNUser * pSender = GetUserByAccountDBID(nSenderAccountDBID);
	if (pSender == NULL) return ERROR_CHAT_USERNOTFOUND;

	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
		(*ii)->SendPrivateChannelChat(  pSender->GetCharacterName(), pChatMsg, nLen, nChannelID );

	vector<CDNGameConnection*>::iterator it;
	for (it = m_GameServerConList.begin(); it != m_GameServerConList.end(); ++it){
		(*it)->SendPrivateChannelChat( pSender->GetCharacterName(), pChatMsg, nLen, nChannelID );
	}

	return ERROR_NONE;
}
#endif

CDNPvP*	CDivisionManager::GetPvPRoom( const UINT uiAccountDBID )
{
	CDNUser* pUser = GetUserByAccountDBID( uiAccountDBID );
	if( pUser && pUser->GetPvPIndex() )
	{
		_TPvPMap::iterator itor = m_mPvP.find( pUser->GetPvPIndex() );
		if( itor != m_mPvP.end() )
			return (*itor).second;
	}
	return NULL;
}

short CDivisionManager::CreatePvPRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_CREATEROOM* pPacket, UINT* nOutPvPIndex )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiCreateAccountDBID );
#if defined( PRE_PVP_GAMBLEROOM )
	if( pUser || pPacket->nEventID > 0 || pPacket->nGuildDBID[0] > 0 || nOutPvPIndex != NULL || pPacket->cGambleType )
#else
	if( pUser || pPacket->nEventID > 0 || pPacket->nGuildDBID[0] > 0 || nOutPvPIndex != NULL )
#endif
	{
		if( pUser && pPacket->nEventID > 0 )
			return ERROR_PVP_CREATEROOM_FAILED;
		// 위에서 한번 거르는데 여기 들어올 일은 없을듯..
		//if( pUser == NULL && pPacket->nEventID <= 0 )
		//	return ERROR_PVP_CREATEROOM_FAILED;

		// 유저가 이미 PvP 방에 있음
		if( pUser )
		{
			if( pUser->GetPvPIndex() )
				return ERROR_PVP_ALREADY_PVPROOM;

#if !defined(PRE_ADD_PVP_VILLAGE_ACCESS)
			// PvP로비검사
			if( !pUser->bIsPvPLobby() )
				return ERROR_PVP_CREATEROOM_FAILED;
#endif // #if !defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		}

		// 더이상 만들 수 없음
		if( m_listPvPIndex.empty() )
			return ERROR_PVP_CREATEROOM_FAILED;

		// 이벤트방 중복생성 검사( 이벤트방은 이벤트때에만 호출되므로 또 Repository 만들기보단 걍 쌩으로 루프 검사한다. )
		if( pPacket->nEventID > 0 )
		{
			for( std::map<UINT,CDNPvP*>::iterator itor=m_mPvP.begin() ; itor!=m_mPvP.end() ; ++itor )
			{
				if( (*itor).second->GetEventRoomIndex() <= 0 )
					continue;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				if( (*itor).second->GetEventDataPtr()->nEventType1 == pPacket->EventData.nEventType1
					&& (*itor).second->GetEventDataPtr()->nEventType2 == pPacket->EventData.nEventType2)
					return ERROR_PVP_CREATEROOM_FAILED;
#else
				if( (*itor).second->GetEventDataPtr()->nEventType2 == pPacket->EventData.nEventType2 )
					return ERROR_PVP_CREATEROOM_FAILED;
#endif
			}
		}

		UINT unPvPIndex = m_listPvPIndex.front();
		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( unPvPIndex );
		if( itor != m_mPvP.end() )
			return ERROR_PVP_CREATEROOM_FAILED;

		CDNPvP* pPvPRoom = new (std::nothrow) CDNPvP();
		INT64 biRoomCreateCharacterDBID = 0;
		if( pUser )
			biRoomCreateCharacterDBID = pUser->GetCharacterDBID();

#ifdef PRE_MOD_PVPOBSERVER
		bool bExtendObserver = false;
		if( pUser )
		{
			if (pUser->GetAccountLevel() >= AccountLevel_New && pUser->GetAccountLevel() <= AccountLevel_QA)
			{
				if(pPacket->sCSPVP_CREATEROOM.cRoomPWLen > 0)
					bExtendObserver = true;
			}
		}

		if( pPvPRoom && pPvPRoom->bCreate( pVillageCon->GetVillageID(), unPvPIndex, pPacket, biRoomCreateCharacterDBID, bExtendObserver ) )
#else		//#ifdef PRE_MOD_PVPOBSERVER
		if( pPvPRoom && pPvPRoom->bCreate( pVillageCon->GetVillageID(), unPvPIndex, pPacket, biRoomCreateCharacterDBID, false ) )
#endif		//#ifdef PRE_MOD_PVPOBSERVER
		{
			// PvP방 생성 성공
			m_listPvPIndex.pop_front();
			m_mPvP.insert( std::make_pair( unPvPIndex, pPvPRoom ) );
			
			pVillageCon->SendPvPCreateRoom( ERROR_NONE, pPvPRoom, pPacket );
			// 비밀방인경우
			if( pPvPRoom->bIsPWRoom() )
				pPvPRoom->SetRoomState( pPvPRoom->GetRoomState()|PvPCommon::RoomState::Password );
			else
				pPvPRoom->SetRoomState( pPvPRoom->GetRoomState() );

			// 방 만든사람 조인
			if( pUser )
			{
				int iUserState = PvPCommon::UserState::Captain;
				if( pPvPRoom->GetGameMode() == PvPCommon::GameMode::PvP_AllKill )
					iUserState |= PvPCommon::UserState::GroupCaptain;
#ifdef PRE_MOD_PVPOBSERVER
				pPvPRoom->Join( pVillageCon, pUser, iUserState, bExtendObserver );
#else		//#ifdef PRE_MOD_PVPOBSERVER
				pPvPRoom->Join( pVillageCon, pUser, iUserState );
#endif		//#ifdef PRE_MOD_PVPOBSERVER
#if defined(PRE_ADD_PVP_TOURNAMENT)
				pUser->SetUserJob(pPacket->cCreateUserJob);
#endif
			}
			if( nOutPvPIndex != NULL )
				*nOutPvPIndex = unPvPIndex;
			return ERROR_NONE;
		}
		else
			delete pPvPRoom;
	}
	else
		_DANGER_POINT();
	
	return ERROR_PVP_CREATEROOM_FAILED;
}

bool CDivisionManager::DestroyPvPGameRoom(const UINT uiPvPIndex)
{
	_TPvPMap::iterator itor = m_mPvP.find(uiPvPIndex);
	if (itor == m_mPvP.end())
		return false;

	CDNPvP * pPvPRoom = (*itor).second;
	CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID( pPvPRoom->GetVillageID() );

	pPvPRoom->bLeaveAll( pVillageCon, PvPCommon::LeaveType::DestroyRoom );
	if( pVillageCon )
		pVillageCon->SendPvPDestroyRoom( pPvPRoom->GetVillageChannelID(), pPvPRoom->GetIndex() );

	if (pPvPRoom->GetEventRoomIndex() > 0)
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"DestroyPvPGameRoom [Index:%d][Room:%d][Event:%d] \r\n", pPvPRoom->GetIndex(), pPvPRoom->GetGameServerRoomIndex(), pPvPRoom->GetEventRoomIndex());

	delete pPvPRoom;
	m_mPvP.erase(itor);
	return true;
}

short CDivisionManager::ModifyPvPRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_MODIFYROOM* pPacket )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		// PvP방에 있는지 검사
		if( uiPvPIndex == 0 )
			return ERROR_PVP_MODIFYROOM_FAILED;
		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
		if( itor == m_mPvP.end() )
			return ERROR_PVP_MODIFYROOM_FAILED;

		CDNPvP* pPvPRoom = (*itor).second;

		//시스템에서 생성된 길드전은 편집할 수 없다
		if (pPvPRoom->GetIsGuildWarSystem())
			return ERROR_PVP_MODIFYROOM_FAILED;

		// Event방은 편집 할 수 없다.
		if( pPvPRoom->GetEventRoomIndex() > 0)
			return ERROR_PVP_MODIFYROOM_FAILED;

#if defined( PRE_PVP_GAMBLEROOM )
		if( pPvPRoom->GetGambleRoomType() > 0)
			return ERROR_PVP_MODIFYROOM_FAILED;
#endif

		// 방장인지 검사
		if( !pPvPRoom->bIsCaptain( pPacket->uiAccountDBID) )
			return ERROR_PVP_MODIFYROOM_FAILED;

		// RoomState 검사
		if( pPvPRoom->GetRoomState()&PvPCommon::RoomState::CantModifyRoom )
			return ERROR_PVP_MODIFYROOM_FAILED;

		pPvPRoom->ModifyRoom( pVillageCon, pPacket );
		return ERROR_NONE;
	}
	else
		_DANGER_POINT();

	return ERROR_PVP_MODIFYROOM_FAILED;
}

void CDivisionManager::LeaveLadderSystem( UINT uiAccountDBID )
{
	CDNUser* pUser = GetUserByAccountDBID( uiAccountDBID );
	if( pUser == NULL )
		return;

	CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID( m_cPvPLobbyVillageID );
	if( pVillageCon == NULL || pVillageCon->GetActive() == false )
		return;

	pVillageCon->SendLadderSystemDelUser( pUser );
}

void CDivisionManager::LeavePvPRoom( const UINT unAccountDBID, const bool bReq )
{
	CDNUser* pUser = GetUserByAccountDBID( unAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex == 0 )
			return;

		// bRet == true : 정상적인 경우에는 유저의 State 를 체크한다.
		if( bReq )
		{
			UINT uiPvPUserState = pUser->GetPvPUserState();
			if( uiPvPUserState&PvPCommon::UserState::CantLeaveMask )
				return;
		}

		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
		if( itor == m_mPvP.end() )
		{
			_DANGER_POINT();
			return;
		}

		CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID(GetPvPLobbyVillageID());
		(*itor).second->bLeave( pVillageCon, pUser );
		
		// Event방은 유저Leave 에 의해 파괴되지 않는다.
		if( (*itor).second->GetEventRoomIndex() > 0)
			return;

		// 시스템에의해 생성된 길드전 방은 유저Leave 에 의해 파괴되지 않는다.
		if ((*itor).second->GetIsGuildWarSystem())
			return;

#if defined( PRE_PVP_GAMBLEROOM )
		if ( (*itor).second->GetGambleRoomType() > 0 )
			return;
#endif

		// PvP방 파괴
		if( (*itor).second->bIsEmpty(true) )
		{
			(*itor).second->bLeaveAll( pVillageCon, PvPCommon::LeaveType::DestroyRoom );
			if( pVillageCon )
				pVillageCon->SendPvPDestroyRoom( (*itor).second->GetVillageChannelID(), (*itor).second->GetIndex() );

			delete (*itor).second;
			m_mPvP.erase( itor );
		}
	}
	else
		_DANGER_POINT();
}

short CDivisionManager::JoinPvPRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_JOINROOM* pPacket )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex )
			return ERROR_PVP_ALREADY_PVPROOM;

		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( pPacket->sCSPVP_JOINROOM.uiPvPIndex );
		if( itor == m_mPvP.end() )
			return ERROR_PVP_JOINROOM_NOTFOUNDROOM;
#if !defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		// PvP로비검사
		if( !pUser->bIsPvPLobby() )
			return ERROR_PVP_JOINROOM_FAILED;
#endif //#if !defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		
		CDNPvP* pPvPRoom = (*itor).second;
		// 방최대인원 검사
		if( !pPvPRoom->bIsEmptySlot( pPacket->sCSPVP_JOINROOM.bIsObserver ) )
			return ERROR_PVP_JOINROOM_MAXPLAYER;

		// 추방유저 검사
		if( pPvPRoom->bIsBanUser( pUser->GetSessionID()) )
			return ERROR_PVP_JOINROOM_BANUSER;

		// 관전 모드 일 경우에는 레벨제한 검사를 하지 않는다.
		if( pPacket->sCSPVP_JOINROOM.bIsObserver == false )
		{
			if( !pPvPRoom->bIsAllowLevel( pPacket->cUserLevel) )
				return ERROR_PVP_JOINROOM_LEVELLIMIT;
		}
		else
		{
#ifdef PRE_MOD_PVPOBSERVER
			if (pPvPRoom->bIsAllowObserver() == false)
				return ERROR_PVP_JOINROOM_DONTALLOW_EVENTROOM_OBSERVER;
#else		//PRE_MOD_PVPOBSERVER
			// 이벤트방은 관전모드로 참관할 수 없다.
			if( pPvPRoom->GetEventRoomIndex() > 0 )
				return ERROR_PVP_JOINROOM_DONTALLOW_EVENTROOM_OBSERVER;
#endif		//PRE_MOD_PVPOBSERVER
		}

		// 비밀번호 검사
		if( pPvPRoom->bIsPWRoom() )
		{
			bool bCheck = true;
#ifdef PRE_MOD_PVPOBSERVER
			if (pPvPRoom->bIsExtendObserver() && pPacket->sCSPVP_JOINROOM.bIsObserver)
				bCheck = false;
#endif		//#ifdef PRE_MOD_PVPOBSERVER

			if( bCheck )
			{
				if( pPacket->sCSPVP_JOINROOM.cRoomPWLen == 0 || pPacket->sCSPVP_JOINROOM.cRoomPWLen > PvPCommon::TxtMax::RoomPW )
					return ERROR_PVP_JOINROOM_INVALIDPW;

				WCHAR wszPW[PvPCommon::TxtMax::RoomPW+1];
				memset( wszPW, 0, (PvPCommon::TxtMax::RoomPW+1)*sizeof(WCHAR) );
				wcsncpy( wszPW, pPacket->sCSPVP_JOINROOM.wszRoomPW, pPacket->sCSPVP_JOINROOM.cRoomPWLen );

				if( !pPvPRoom->bIsCheckPW( wszPW ) )
					return ERROR_PVP_JOINROOM_INVALIDPW;
			}
		}

		if (pPvPRoom->GetGameMode() == PvPCommon::GameMode::PvP_GuildWar)
		{
			if (pPvPRoom->GetIsGuildWarSystem())
			{
				//시스템에서 생성되어진 길드전일경우 관전입장은 운영자만 가능하다
				if (pPacket->sCSPVP_JOINROOM.bIsObserver && pUser->GetAccountLevel() < AccountLevel_New)
					return ERROR_PVP_JOINROOM_FAILED;
			}
		}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		pUser->SetUserJob(pPacket->cUserJob);
#endif
		return pPvPRoom->Join( pVillageCon, pUser, PvPCommon::UserState::None, pPacket->sCSPVP_JOINROOM.bIsObserver, pPacket->sCSPVP_JOINROOM.nGuildDBID );
	}
	else
		_DANGER_POINT();

	return ERROR_NONE;
}

short CDivisionManager::RandomJoinPvPRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_RANDOMJOINROOM* pPacket )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex )
			return ERROR_PVP_ALREADY_PVPROOM;
#if !defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		// PvP로비검사
		if( !pUser->bIsPvPLobby() )
			return ERROR_PVP_JOINROOM_FAILED;
#endif
		// 0.PvP방 없는 경우 예외처리
		if( m_mPvP.empty() )
			return ERROR_PVP_JOINROOM_FAILED;

		// 1.랜덤하게 방하나 뽑아보기
		size_t AdvanceOffset = rand()%m_mPvP.size();

		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.begin();
		std::advance( itor, AdvanceOffset );

		CDNPvP* pPvPRoom = (*itor).second;
		
		// 빈자리가 있고 추방유저가 아니고 비밀방이 아니라면 조인시켜준다.
		if( pPvPRoom->bIsEmptySlot() && !pPvPRoom->bIsPWRoom() && !pPvPRoom->bIsBanUser( pUser->GetSessionID() ) )
		{
#if defined( PRE_PVP_GAMBLEROOM )
			if( pPvPRoom->bIsAllowLevel( pPacket->cUserLevel) && pPvPRoom->GetEventRoomIndex() <= 0 && pPvPRoom->GetGambleRoomType() <= 0 )
#else
			if( pPvPRoom->bIsAllowLevel( pPacket->cUserLevel) && pPvPRoom->GetEventRoomIndex() <= 0 )
#endif
				return pPvPRoom->Join( pVillageCon, pUser, PvPCommon::UserState::None );
		}

		// 2.순차적으로 방검사
		for( itor=m_mPvP.begin() ; itor!=m_mPvP.end() ; ++itor )
		{
			CDNPvP* pPvPRoom = (*itor).second;

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
			if( pPvPRoom->GetGameMode() == PvPCommon::GameMode::PvP_ComboExercise )
			{
				continue;
			}
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

			// 빈자리가 있고 추방유저가 아니고 비밀방이 아니라면 조인시켜준다.
			if( pPvPRoom->bIsEmptySlot() && !pPvPRoom->bIsPWRoom() && !pPvPRoom->bIsBanUser( pUser->GetSessionID() ) )
			{
#if defined( PRE_PVP_GAMBLEROOM )
				if( pPvPRoom->bIsAllowLevel( pPacket->cUserLevel) && pPvPRoom->GetEventRoomIndex() <= 0 && pPvPRoom->GetGambleRoomType() <= 0 )
#else
				if( pPvPRoom->bIsAllowLevel( pPacket->cUserLevel) && pPvPRoom->GetEventRoomIndex() <= 0 )
#endif
					return pPvPRoom->Join( pVillageCon, pUser, PvPCommon::UserState::None );
			}
		}
	}
	else
		_DANGER_POINT();

	return ERROR_PVP_JOINROOM_FAILED;
}

void CDivisionManager::ChangePvPTeam( CDNVillageConnection* pVillageCon, const VIMAPVP_CHANGETEAM* pPacket )
{
	// RoomState검사
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex == 0 )
			return;

		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
		if( itor == m_mPvP.end() )
			return;

		CDNPvP* pPvPRoom = (*itor).second;

		if( pPvPRoom->GetEventRoomIndex() > 0 )
			return;

#if defined( PRE_PVP_GAMBLEROOM )
		if( pPvPRoom->GetGambleRoomType() > 0 )
			return;
#endif
		// 길드전(점령전)일 경우 무조건 팀변경 불가능으로 변경되었다. 34361이슈
		if (pPvPRoom->GetGameMode() == PvPCommon::GameMode::PvP_GuildWar)
		{
			if (pPvPRoom->GetIsGuildWarSystem())
				return;
		}
#ifdef PRE_MOD_PVPOBSERVER
		if (pPvPRoom->bIsExtendObserver())
		{
			if( pUser->bIsObserver() || (pUser->bIsObserver() == false && pPacket->sCSPVP_CHANGETEAM.usTeam == PvPCommon::Team::Observer) )
				return;
		}
#endif		//#ifdef PRE_MOD_PVPOBSERVER

		switch( pPvPRoom->GetGameMode() )
		{
			// 개인전은 팀변경이 되지 않는다.
			case PvPCommon::GameMode::PvP_IndividualRespawn:
#if defined(PRE_ADD_PVP_TOURNAMENT)
			case PvPCommon::GameMode::PvP_Tournament:
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
			case PvPCommon::GameMode::PvP_ComboExercise:
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
			{
				// 옵셔버 <-> 팀간 이동은 가능
				if( pUser->bIsObserver() || (pUser->bIsObserver()==false && pPacket->sCSPVP_CHANGETEAM.usTeam == PvPCommon::Team::Observer) )
					break;
				return;
			}
		}

		// 레뒤상태에서는 팀이동 불가
		if( pUser->GetPvPUserState()&PvPCommon::UserState::Ready )
			return;

		// PvPCommon::UserState::Starting|PvPCommon::UserState::Syncing|PvPCommon::UserState::Playing 에서 팀이동 불가
		if( pUser->GetPvPUserState()&(PvPCommon::UserState::Starting|PvPCommon::UserState::Syncing|PvPCommon::UserState::Playing) )
			return;

		// 팀 값 확인
		if( !PvPCommon::CheckTeam( pPacket->sCSPVP_CHANGETEAM.usTeam ) || pUser->GetPvPTeam() == pPacket->sCSPVP_CHANGETEAM.usTeam )
			return;
	
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( pPvPRoom->IsMode( PvPCommon::GameMode::PvP_Tournament) == true )
		{
			if( pPacket->sCSPVP_CHANGETEAM.usTeam == PvPCommon::Team::Observer ) // 옵저버 일때만 검사..다른넘은 밑에서 검사.
			{
				// 옮길 팀 여유 슬롯 있는지 검사
				if( !pPvPRoom->bIsEmptyTeamSlot( pPacket->sCSPVP_CHANGETEAM.usTeam ) )
				{
					pVillageCon->SendPvPChangeTeam( ERROR_PVP_CANTCHANGETEAM_NOSLOT, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pPacket );
					return;
				}
			}
		}
		else
#endif
		{		
			// 옮길 팀 여유 슬롯 있는지 검사
			if( !pPvPRoom->bIsEmptyTeamSlot( pPacket->sCSPVP_CHANGETEAM.usTeam ) )
			{
				pVillageCon->SendPvPChangeTeam( ERROR_PVP_CANTCHANGETEAM_NOSLOT, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pPacket );
				return;
			}
		}

		// 관전모드에서 게임참여로 옮길경우 레벨 제한 체크
		if( pUser->GetPvPTeam() == PvPCommon::Team::Observer && pPacket->sCSPVP_CHANGETEAM.usTeam <= PvPCommon::Team::B )
		{
			if( pPvPRoom->bIsAllowLevel( pPacket->cUserLevel) == false )
			{
				pVillageCon->SendPvPChangeTeam( ERROR_PVP_CANTCHANGETEAM_LEVELLIMIT, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pPacket );
				return;
			}
		}

		// Observer 슬롯으로 변경하는 거고 방장이라면...
		if( pPacket->sCSPVP_CHANGETEAM.usTeam == PvPCommon::Team::Observer && pPvPRoom->bIsCaptain( pPacket->uiAccountDBID ) )
		{
			// 새로운 방장으로 변경
			UINT uiNewCaptain = pPvPRoom->FindNewCaptain();
			if( uiNewCaptain == 0 )
			{
				pVillageCon->SendPvPChangeTeam( ERROR_PVP_CANTCHANGETEAM_CHANGECAPTAIN_FAILED, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pPacket );
				return;
			}
			pPvPRoom->ChangeCaptain( pVillageCon, uiNewCaptain );
		}
		char cTeamSlotIndex = -1;
		if( pPvPRoom->IsMode( PvPCommon::GameMode::PvP_GuildWar) == true )
		{
			if(pPvPRoom->bIsGuildWarGrade(pPacket->uiAccountDBID, PvPCommon::UserState::GuildWarCaptain|PvPCommon::UserState::GuildWarSedcondCaptain))
			{
				UINT uiPvPState = pUser->GetPvPUserState();
				uiPvPState &= ~PvPCommon::UserState::GuildWarCaptain;
				uiPvPState &= ~PvPCommon::UserState::GuildWarSedcondCaptain;
				pPvPRoom->SetPvPUserState(pUser, uiPvPState);
			}

			pPvPRoom->SetPvPMemberIndex(pUser->GetPvPTeam(), pUser->GetAccountDBID());
			if (pPacket->sCSPVP_CHANGETEAM.usTeam != PvPCommon::Team::Observer)
			{
				pPvPRoom->GetAndSetRemainTeamIndex(pPacket->sCSPVP_CHANGETEAM.usTeam, pUser->GetAccountDBID(), cTeamSlotIndex);
			}
		}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( pPvPRoom->IsMode( PvPCommon::GameMode::PvP_Tournament) == true )
		{
			if (pPacket->sCSPVP_CHANGETEAM.usTeam == PvPCommon::Team::Observer)			
				pPvPRoom->DelTournamentIndex(pPacket->uiAccountDBID);
			else			
			{
				cTeamSlotIndex = pPvPRoom->GetTournamentIndex(pPacket->uiAccountDBID);
				if( cTeamSlotIndex != -1)
					pUser->SetPvPTeam(cTeamSlotIndex%2==0? PvPCommon::Team::A : PvPCommon::Team::B);
				else
				{
					pVillageCon->SendPvPChangeTeam( ERROR_PVP_CANTCHANGETEAM_NOSLOT, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pPacket );
					return;
				}
			}
		}
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( pPvPRoom->IsMode( PvPCommon::GameMode::PvP_AllKill ) == true )
		{
			if( pUser->GetPvPUserState()&PvPCommon::UserState::GroupCaptain )
				pPvPRoom->SetPvPUserState( pUser, pUser->GetPvPUserState()&~PvPCommon::UserState::GroupCaptain );
		}
		// 팀변경
		pUser->SetPvPTeam( pPacket->sCSPVP_CHANGETEAM.usTeam );

		pPvPRoom->CheckAndSetGroupCaptain();
		if( pPvPRoom->IsMode( PvPCommon::GameMode::PvP_GuildWar ) == true )
		{
			if (pUser->GetPvPTeam() != PvPCommon::Team::Observer)
			{
				for (int i = 0; i < PvPCommon::TeamIndex::Max; i++)
				{
					if (pPvPRoom->GetUserCountByState(i == 0 ? PvPCommon::Team::A : PvPCommon::Team::B, PvPCommon::UserState::GuildWarCaptain) <= 0)
					{
						UINT nNewGuildWarCaptain = pPvPRoom->SelectNewGuildWarCaptain(i == 0 ? PvPCommon::Team::A : PvPCommon::Team::B);
						pPvPRoom->SetPvPGuildWarMemberGrade(i == 0 ? PvPCommon::Team::A : PvPCommon::Team::B, true, PvPCommon::UserState::GuildWarCaptain, nNewGuildWarCaptain);
					}
				}
			}
		}
		// 패킷 전송
		pVillageCon->SendPvPChangeTeam( ERROR_NONE, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pPacket, cTeamSlotIndex );
	}
}

void CDivisionManager::BanPvPRoom( CDNVillageConnection* pVillageCon, const VIMAPVP_BAN* pPacket )
{
	CDNPvP* pPvPRoom = GetPvPRoom( pPacket->uiAccountDBID );
	if( !pPvPRoom )
		return;

	pPvPRoom->BanPvPRoom( pVillageCon, pPacket );
}

void CDivisionManager::EnterLobby( CDNVillageConnection* pVillageCon, const VIMAPVP_ENTERLOBBY* pPacket )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
#if defined(PRE_ADD_PVP_VILLAGE_ACCESS)
		if( pUser->GetLadderMatchType() != LadderSystem::MatchType::None)
		{
			VIMALadderEnterChannel LadderEnterChannel;
			memset(&LadderEnterChannel, 0, sizeof(LadderEnterChannel));

			LadderEnterChannel.MatchType = pUser->GetLadderMatchType();
			LadderEnterChannel.uiAccountDBID = pPacket->uiAccountDBID;
			pVillageCon->AddSendData( MAVI_LADDERSYSTEM_ENTERCHANNEL, 0, (char*)&LadderEnterChannel, sizeof(LadderEnterChannel));
			pUser->SetLadderMatchType(LadderSystem::MatchType::None);
			return;
		}
#endif
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex == 0 )
		{
			VIMAPVP_ROOMLIST RoomList;
			memset( &RoomList, 0, sizeof(RoomList) );

			RoomList.uiAccountDBID				= pPacket->uiAccountDBID;
			RoomList.unVillageChannelID			= pPacket->unVillageChannelID;
			RoomList.sCSPVP_ROOMLIST.uiPage		= 0;
			RoomList.sCSPVP_ROOMLIST.cIsAscend	= 1;
			pVillageCon->SendPvPRoomList( &RoomList );
			return;
		}

#if defined( PRE_WORLDCOMBINE_PVP )
		if( uiPvPIndex > WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex )
		{
			pVillageCon->SendPvPJoinRoom(ERROR_NONE, pPacket->uiAccountDBID, pPacket->unVillageChannelID, uiPvPIndex, pUser->GetPvPTeam(), pUser->GetPvPUserState(), -1);
			return;
		}
#endif
		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
		if( itor == m_mPvP.end() )
			return;

		CDNPvP* pPvPRoom = (*itor).second;

		if( pPvPRoom->bIsCheckUser( pPacket->uiAccountDBID ) )
		{
			char cIndex = -1;
#if defined(PRE_ADD_PVP_TOURNAMENT)
			if( pPvPRoom->IsMode(PvPCommon::GameMode::PvP_Tournament) == true )
				cIndex = pPvPRoom->FindTournamentIndex(pPacket->uiAccountDBID);
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
			if( pPvPRoom->IsMode(PvPCommon::GameMode::PvP_GuildWar) == true )
				cIndex = pPvPRoom->GetPvPTeamIndex(pUser);

			pVillageCon->SendPvPJoinRoom(ERROR_NONE, pPacket->uiAccountDBID, pPvPRoom->GetVillageChannelID(), uiPvPIndex, pUser->GetPvPTeam(), pUser->GetPvPUserState(), cIndex);
			// 방장이양
			UINT uiRoomState = pPvPRoom->GetRoomState();

			if( uiRoomState&PvPCommon::RoomState::Playing )	
			{
				// 게임모드가 끝났으면 방장이양하지 않는다.
				if( uiRoomState&PvPCommon::RoomState::Finished )
					return;

				// 로비로 돌아온 캐릭터가 방장인지 검사
				if( pPvPRoom->bIsCaptain( pPacket->uiAccountDBID ) )
				{
#ifdef PRE_MOD_PVPOBSERVER
					if (pPvPRoom->bIsExtendObserver() == false)
					{
						UINT uiNewCaptain = pPvPRoom->FindNewCaptain();
						pPvPRoom->ChangeCaptain( pVillageCon, uiNewCaptain );
					}
#else		//#ifdef PRE_MOD_PVPOBSERVER
					UINT uiNewCaptain = pPvPRoom->FindNewCaptain();
					pPvPRoom->ChangeCaptain( pVillageCon, uiNewCaptain );
#endif		//#ifdef PRE_MOD_PVPOBSERVER
				}
				//로비로 돌아온 캐릭터가 길드전 대장 또는 부대장인지 검사
				if (pPvPRoom->bIsGuildWarGrade(pPacket->uiAccountDBID, PvPCommon::UserState::GuildWarCaptain))
				{
					if (pPvPRoom->GetUserCountByTeam(pUser->GetPvPTeam()) > 1)
					{
						UINT nPvPState = pUser->GetPvPUserState();
						nPvPState &= ~PvPCommon::UserState::GuildWarCaptain;
						
						UINT nNewGuildWarCaptain = pPvPRoom->SelectNewGuildWarCaptain(pUser->GetPvPTeam(), pUser->GetSessionID());
						pPvPRoom->SetPvPGuildWarMemberGrade(pUser->GetPvPTeam(), true, PvPCommon::UserState::GuildWarCaptain, nNewGuildWarCaptain);

						if (g_pDivisionManager)
						{
							CDNUser * pNewGuildWarCaptain = g_pDivisionManager->GetUserBySessionID(nNewGuildWarCaptain);
							if (pNewGuildWarCaptain)
							{
								CDNGameConnection * pGameCon = g_pDivisionManager->GetGameConnectionByGameID(pNewGuildWarCaptain->GetGameID());
								if (pGameCon)
								{
									//변경을 알린다.
									pGameCon->SendPvPMemberGrade(pNewGuildWarCaptain->GetAccountDBID(), pNewGuildWarCaptain->GetPvPTeam(), pNewGuildWarCaptain->GetPvPUserState(), nNewGuildWarCaptain, ERROR_NONE);
								}
								else
									_DANGER_POINT();
							}
							else
								_DANGER_POINT();
						}

						pPvPRoom->SetPvPUserState(pUser, nPvPState);
					}
				}

				if (pPvPRoom->bIsGuildWarGrade(pPacket->uiAccountDBID, PvPCommon::UserState::GuildWarSedcondCaptain))
				{
					UINT nPvPState = pUser->GetPvPUserState();
					nPvPState &= ~PvPCommon::UserState::GuildWarSedcondCaptain;

					pPvPRoom->SetPvPUserState(pUser, nPvPState);
				}
			}
		}
	}
	else
		_DANGER_POINT();
}

void CDivisionManager::ChangePvPRoomCaptain( CDNVillageConnection* pVillageCon, const VIMAPVP_CHANGECAPTAIN* pPacket )
{
	if( pPacket->sCSPVP_CHANGECAPTAIN.Type == PvPCommon::CaptainType::Captain )
	{
		CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
		if( pUser )
		{
			UINT uiPvPIndex = pUser->GetPvPIndex();
			if( uiPvPIndex == 0 )
				return;

			std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
			if( itor == m_mPvP.end() )
				return;

			CDNPvP* pPvPRoom = (*itor).second;

			if( !pPvPRoom->bIsCaptain( pPacket->uiAccountDBID ) )
				return;

			CDNUser* pNewUser = pPvPRoom->GetUserBySessionID( pPacket->sCSPVP_CHANGECAPTAIN.uiNewCaptainSessionID );
			if( !pNewUser || pNewUser->GetAccountDBID() == pPacket->uiAccountDBID )
				return;
			if( pNewUser->GetPvPTeam() == PvPCommon::Team::Observer )
				return;

			pPvPRoom->ChangeCaptain( pVillageCon, pNewUser->GetAccountDBID() );
		}
	}
	else if( pPacket->sCSPVP_CHANGECAPTAIN.Type == PvPCommon::CaptainType::GroupCaptain )
	{
		CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
		if( pUser )
		{
			USHORT nTeam = pUser->GetPvPTeam();

			UINT uiPvPIndex = pUser->GetPvPIndex();
			if( uiPvPIndex == 0 )
				return;

			std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
			if( itor == m_mPvP.end() )
				return;

			CDNPvP* pPvPRoom = (*itor).second;

			if( pPvPRoom->IsMode(PvPCommon::GameMode::PvP_AllKill) == false )
				return;

			if( !pPvPRoom->bIsGroupCaptain( pPacket->uiAccountDBID ) )
				return;

			CDNUser* pNewUser = pPvPRoom->GetUserBySessionID( pPacket->sCSPVP_CHANGECAPTAIN.uiNewCaptainSessionID );
			if( !pNewUser || pNewUser->GetAccountDBID() == pPacket->uiAccountDBID )
				return;
			if( pNewUser->GetPvPTeam() == PvPCommon::Team::Observer )
				return;
			if( pNewUser->GetPvPTeam() != nTeam )
				return;

			pPvPRoom->ChangeGroupCaptain( pNewUser->GetAccountDBID(), nTeam );
		}
	}
}

void CDivisionManager::ReadyPvP( CDNVillageConnection* pVillageCon, VIMAPVP_READY* pPacket )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex == 0 )
			return;

		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
		if( itor == m_mPvP.end() )
			return;

		CDNPvP* pPvPRoom = (*itor).second;

		pPvPRoom->ReadyUser( pVillageCon, pPacket );
	}
	else
		_DANGER_POINT();
}

short CDivisionManager::StartPvP( CDNVillageConnection* pVillageCon, VIMAPVP_START* pPacket, bool bFromUser/* = false*/ )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
	{
		UINT uiPvPIndex = pUser->GetPvPIndex();
		if( uiPvPIndex == 0 )
			return ERROR_PVP_STARTPVP_FAILED;

		std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
		if( itor == m_mPvP.end() )
			return ERROR_PVP_STARTPVP_FAILED;

		CDNPvP* pPvPRoom = (*itor).second;
		if (pPvPRoom->GetIsGuildWarSystem() && bFromUser && !(pPvPRoom->GetRoomState()&PvPCommon::RoomState::Playing) )
			return ERROR_PVP_STARTPVP_FAILED;					//시스템에서 생성한 피빕은 시스템에서만 시작할 수 있다
		return pPvPRoom->StartPvP( pVillageCon, pPacket );
	}
	else
		_DANGER_POINT();

	return ERROR_PVP_STARTPVP_FAILED;
}

bool CDivisionManager::ForceStopPvP(const UINT uiPvPIndex, const WCHAR * pGuildName/* = NULL*/)
{
	CDNPvP * pPvP = GetPvPRoomByIdx(uiPvPIndex);
	if (pPvP)
	{
		bool bRet = false;
		int nGameID = pPvP->GetGameServerID();
		int nGameRoomID = pPvP->GetGameServerRoomIndex();
		UINT nForceWinGuildDBID = 0;
		if (nGameID > 0 && nGameRoomID > 0)
		{
			if (pGuildName && wcslen(pGuildName) > 0)
			{
				nForceWinGuildDBID = g_pGuildWarManager->GetGuildDBIDWithFinal(uiPvPIndex, pGuildName);
				if (nForceWinGuildDBID > 0)
					bRet = true;
			}
			else
				bRet = true;

			if (bRet)
			{
				CDNGameConnection * pGameCon = GetGameConnectionByGameID(nGameID);
				if (pGameCon)				
				{
					//게임서버에 강제파괴를 알리고
					bRet = pGameCon->SendForceStopPvP(nGameRoomID, nForceWinGuildDBID);
					if (bRet == false)
					{
						//error
					}
				}
			}
		}

		if (bRet)
		{
			//PvPRoom을 정리한다.
			SetDestroyPvPGameRoom(uiPvPIndex, nGameRoomID);
			//만약 삭제처리하는 PvPRoom이 Guildwarsystem방이라면 SetDestroyPvPGameRoom이후에는 pPvPRoom은 유효하지 않다. 주의!!
			return true;
		}
	}
	return false;
}

void CDivisionManager::SetPvPFatigueOption(const VIMAPVP_FATIGUE_OPTION* pPacket )
{
	CDNUser* pUser = GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )			
		pUser->SetPvPFatigue(pPacket->bFatigueOption);
	else
		_DANGER_POINT();
}

bool CDivisionManager::SetGuildWarMemberGrade(UINT nAccountDBID, bool bAsign, USHORT nType, UINT nTargetSessionID, USHORT &nOutUserState, USHORT &nTeam)
{
	bool bRet = false;
	// 유저 검사
	CDNUser* pUser = GetUserByAccountDBID(nAccountDBID);
	if( !pUser )
		return bRet;

	// PvP 방에 없음
	UINT uiPvPIndex = pUser->GetPvPIndex();
	if( uiPvPIndex == 0 )
		return bRet;

	CDNPvP* pPvPRoom = GetPvPRoomByIdx(uiPvPIndex);
	if (pPvPRoom == NULL)
		return bRet;

	if (!(pUser->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain))
		return bRet;

	nTeam = pUser->GetPvPTeam();
	int nRet = pPvPRoom->SetPvPGuildWarMemberGrade(pUser->GetPvPTeam(), bAsign, nType, nTargetSessionID);
	CDNUser * pTargetUser = NULL;
	if (nRet == ERROR_NONE)
	{
		pTargetUser = GetUserBySessionID(nTargetSessionID);
		if (pTargetUser)
			nOutUserState = pTargetUser->GetPvPUserState();
		else
			return false;
	}
	return nRet == ERROR_NONE ? true : false;
}

bool CDivisionManager::SetPvPMemberIndex(UINT nAccountDBID, BYTE cCount, const TSwapMemberIndex * pIndex, USHORT &nTeam)
{
	bool bRet = false;
	// 유저 검사
	CDNUser* pUser = GetUserByAccountDBID(nAccountDBID);
	if( !pUser )
		return bRet;

	// PvP 방에 없음
	UINT uiPvPIndex = pUser->GetPvPIndex();
	if( uiPvPIndex == 0 )
		return bRet;

	CDNPvP* pPvPRoom = GetPvPRoomByIdx(uiPvPIndex);
	if (pPvPRoom == NULL)
		return bRet;

	if (!(pUser->GetPvPUserState()&PvPCommon::UserState::GuildWarCaptain))
		return bRet;

	nTeam = pUser->GetPvPTeam();
	bRet = pPvPRoom->SwapPvPMemberIndex(pUser->GetPvPTeam(), cCount, pIndex);
	return bRet;
}

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
void CDivisionManager::SetPvPChangeChannel(CDNVillageConnection * pVillageCon, const PVP_CHANGECHANNEL * pChange)
{
	if (pVillageCon == NULL) return;
	CDNUser * pUser = GetUserByAccountDBID(pChange->nAccountDBID);
	if (pUser)
	{
#if defined(PRE_ADD_DWC)
		if( pUser->GetCharacterAccountLevel() == AccountLevel_DWC )
		{
			if( pChange->cType == PvPCommon::RoomType::dwc )
			{
				pUser->SetPvPChannelType(pChange->cType);
				if (pChange->bSend)
					pVillageCon->SendPvPChangeChannelResult(pChange->nAccountDBID, static_cast<BYTE>(pUser->GetPvPChannelType()), ERROR_NONE);
				return;
			}
			_DANGER_POINT();
		}
		else{
			if( pChange->cType == PvPCommon::RoomType::dwc )	// 일반계정은 dwc 채널로 가면 안됨
			{
				_DANGER_POINT();
				return;
			}
#endif
		if (pChange->cPvPLevel < pChange->cSeperateLevel)
		{
			//초짜인 친구 어디로가든 오케이!
			pUser->SetPvPChannelType(pChange->cType);
			if (pChange->bSend)
				pVillageCon->SendPvPChangeChannelResult(pChange->nAccountDBID, static_cast<BYTE>(pUser->GetPvPChannelType()), ERROR_NONE);
			return;
		}
		else if (pChange->cPvPLevel >= pChange->cSeperateLevel)
		{
			//상급자!는 일반채널만 가능
			if (static_cast<PvPCommon::RoomType::eRoomType>(pChange->cPvPLevel) == PvPCommon::RoomType::regular)
			{
				pUser->SetPvPChannelType(pChange->cType);
				if (pChange->bSend)
					pVillageCon->SendPvPChangeChannelResult(pChange->nAccountDBID, static_cast<BYTE>(pUser->GetPvPChannelType()), ERROR_NONE);
				return;
			}
			_DANGER_POINT();
		}
#if defined(PRE_ADD_DWC)
		}
#endif
		_DANGER_POINT();
	}
	_DANGER_POINT();
}
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER

#if defined(PRE_ADD_PVP_TOURNAMENT)
bool CDivisionManager::SwapTournamentIndex(UINT nAccountDBID, char cSourceIndex, char cDestIndex)
{
	bool bRet = false;
	// 유저 검사
	CDNUser* pUser = GetUserByAccountDBID(nAccountDBID);
	if( !pUser )
		return bRet;

	// PvP 방에 없음
	UINT uiPvPIndex = pUser->GetPvPIndex();
	if( uiPvPIndex == 0 )
		return bRet;

	CDNPvP* pPvPRoom = GetPvPRoomByIdx(uiPvPIndex);
	if (pPvPRoom == NULL)
		return bRet;

	if (!(pUser->GetPvPUserState()&PvPCommon::UserState::Captain))
		return bRet;
	bRet = pPvPRoom->SwapPvPTournamentIndex(cSourceIndex, cDestIndex);
	if( bRet )
	{
		UINT uiSourceAccountDBID=pPvPRoom->GetPvPTournamentAccountDBID(cSourceIndex);
		if( uiSourceAccountDBID > 0 )
		{
			CDNUser* pSourceUser = GetUserByAccountDBID(uiSourceAccountDBID);
			if( pSourceUser )			
				pSourceUser->SetPvPTeam(cSourceIndex%2==0 ? PvPCommon::Team::A : PvPCommon::Team::B);
		}
		UINT uiDestAccountDBID=pPvPRoom->GetPvPTournamentAccountDBID(cDestIndex);
		if( uiDestAccountDBID > 0 )
		{
			CDNUser* pDestUser = GetUserByAccountDBID(uiDestAccountDBID);
			if( pDestUser )
				pDestUser->SetPvPTeam(cDestIndex%2==0 ? PvPCommon::Team::A : PvPCommon::Team::B);
		}
	}
	return bRet;
}
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)

void CDivisionManager::SetPvPRoomState( const UINT uiPvPIndex, const UINT uiRoomState )
{
	std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
	if( itor == m_mPvP.end() )
	{
		_DANGER_POINT();
		return;
	}

	(*itor).second->SetRoomState( uiRoomState );
}

void CDivisionManager::SetPvPRoomSyncOK( const UINT uiPvPIndex )
{
	std::map<UINT, CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
	if( itor == m_mPvP.end() )
	{
		_DANGER_POINT();
		return;
	}

	UINT uiRoomState = (*itor).second->GetRoomState();

	uiRoomState &= ~PvPCommon::RoomState::Syncing;
	uiRoomState |= PvPCommon::RoomState::Playing;
	
	(*itor).second->SetRoomState( uiRoomState );
}

void CDivisionManager::SetPvPBreakIntoOK( const GAMAPVP_BREAKINTOOK* pPacket )
{
#if defined( PRE_WORLDCOMBINE_PVP )
	if( pPacket->uiPvPIndex > WorldPvPMissionRoom::Common::WorldPvPRoomStartIndex )
	{
		return;
	}
#endif
	std::map<UINT, CDNPvP*>::iterator itor = m_mPvP.find( pPacket->uiPvPIndex );
	if( itor == m_mPvP.end() )
	{
		_DANGER_POINT();
		return;
	}

	CDNPvP* pPvPRoom = (*itor).second;

	if( !pPvPRoom->bIsCheckUser( pPacket->uiAccountDBID ) )
		_DANGER_POINT();

	CDNUser* pUser = g_pDivisionManager->GetUserByAccountDBID( pPacket->uiAccountDBID );
	if( pUser )
		pPvPRoom->AddPvPUserState( pUser, PvPCommon::UserState::Playing );
	else
		_DANGER_POINT();
}

void CDivisionManager::SetPvPFinishGameMode( const UINT uiPvPIndex, const UINT uiRoomIndex )
{
	std::map<UINT, CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
	if( itor == m_mPvP.end() )
	{
		_DANGER_POINT();
		return;
	}

	CDNPvP* pPvPRoom = (*itor).second;

	// 게임룸ID 확인
	if( pPvPRoom->GetGameServerRoomIndex() != uiRoomIndex )
		return;

	UINT uiRoomState = (*itor).second->GetRoomState();

	uiRoomState |= PvPCommon::RoomState::Finished;

	(*itor).second->SetRoomState( uiRoomState );
}

void CDivisionManager::SetPvPNoMoreBreakInto( const UINT uiPvPIndex, const UINT uiRoomIndex )
{
	std::map<UINT, CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
	if( itor == m_mPvP.end() )
	{
		_DANGER_POINT();
		return;
	}

	CDNPvP* pPvPRoom = (*itor).second;

	// 게임룸ID 확인
	if( pPvPRoom->GetGameServerRoomIndex() != uiRoomIndex )
		return;

	UINT uiRoomState = (*itor).second->GetRoomState();

	uiRoomState |= PvPCommon::RoomState::NoMoreBreakInto;

	(*itor).second->SetRoomState( uiRoomState );
}

void CDivisionManager::SetDestroyPvPGameRoom( const UINT uiPvPIndex, const UINT uiRoomIndex )
{
	std::map<UINT,CDNPvP*>::iterator itor = m_mPvP.find( uiPvPIndex );
	if( itor == m_mPvP.end() )
	{
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"Fail to find PvPRoom [Index:%d][Room:%d] \r\n", uiPvPIndex, uiRoomIndex);
		return;
	}
	
	CDNPvP* pPvPRoom = (*itor).second;

	// 게임룸ID 확인
	if( pPvPRoom->GetGameServerRoomIndex() != uiRoomIndex )
	{
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"Not Match PvPRoomID[%d!=%d] \r\n", pPvPRoom->GetGameServerRoomIndex(), uiRoomIndex);
		return;
	}

	UINT uiRoomState = pPvPRoom->GetRoomState();
	UINT uiNewRoomState = uiRoomState&PvPCommon::RoomState::Password;
	pPvPRoom->SetRoomState( uiNewRoomState );

	if (pPvPRoom->GetEventRoomIndex() > 0)
		g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"Success to destroy PvPRoom [Index:%d][Room:%d][Event:%d] \r\n", pPvPRoom->GetIndex(), pPvPRoom->GetGameServerRoomIndex(), pPvPRoom->GetEventRoomIndex());

	if (pPvPRoom->GetIsGuildWarSystem())
	{
		if (DestroyPvPGameRoom(uiPvPIndex) == false)
			_DANGER_POINT();
		//주의 이하에서 pPvPRoom은 유효하지 않음!
		return;
	}

#if defined( PRE_PVP_GAMBLEROOM )
	if( pPvPRoom->GetGambleRoomType() > 0)
	{
		if (DestroyPvPGameRoom(uiPvPIndex) == false)
			_DANGER_POINT();
		//주의 이하에서 pPvPRoom은 유효하지 않음!
		return;
	}
#endif
}

//Farm
void CDivisionManager::CheckProcessFarm()
{
	if (m_bDestroyFarm == false) return;

	g_Log.Log(LogType::_FARM, L"CheckProcessFarm\n");

	//파괴되어진 농장이 있다면 (게임서버가 죽음) 다른 서버로 세팅처리 해준다.
	m_bDestroyFarm = false;
	std::map <UINT, CDNFarm*>::iterator ii;
	for (ii = m_mFarmList.begin(); ii != m_mFarmList.end(); ii++)
	{
		if ((*ii).second->GetFarmStatus() == FARMSTATUS_DESTROY)
		{
			bool bFlag = true;
			int nAssignedGameID = -1;
#if defined( PRE_ADD_FARM_DOWNSCALE )
			if (RequestFarmGameRoom((*ii).second->GetFarmDBID(), (*ii).second->GetMapID(), (*ii).second->GetFarmMaxUser(), (*ii).second->GetFarmStart(), nAssignedGameID, (*ii).second->GetAttr() ))
#elif defined( PRE_ADD_VIP_FARM )
			if (RequestFarmGameRoom((*ii).second->GetFarmDBID(), (*ii).second->GetMapID(), (*ii).second->GetFarmMaxUser(), (*ii).second->GetFarmStart(), nAssignedGameID, (*ii).second->GetAttr() ))
#else
			if (RequestFarmGameRoom((*ii).second->GetFarmDBID(), (*ii).second->GetMapID(), (*ii).second->GetFarmMaxUser(), (*ii).second->GetFarmStart(), nAssignedGameID))
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

			{
				if ((*ii).second->SetAssignedServerID(nAssignedGameID) == false)
				{
					_DANGER_POINT();
					m_bDestroyFarm = true;		//실패한게 있다면 다시 돌게한다.
					g_Log.Log(LogType::_FARM, L"Create FarmGameRoom GameID[%d] FarmID[%d] SetAssignedServerID Fail\n", nAssignedGameID, (*ii).second->GetFarmDBID());
				}
				g_Log.Log(LogType::_FARM, L"Create FarmGameRoom GameID[%d] FarmID[%d] Success\n", nAssignedGameID, (*ii).second->GetFarmDBID());
			}
			else
			{
				m_bDestroyFarm = true;		//실패한게 있다면 다시 돌게한다.
				g_Log.Log(LogType::_FARM, L"Create FarmGameRoom GameID[%d] FarmID[%d] RequestFarmGameRoom Fail\n", nAssignedGameID, (*ii).second->GetFarmDBID());
			}
		}
	}
}

#if defined( PRE_PVP_GAMBLEROOM )
void CDivisionManager::DeletePvPGambleRoom()
{	
	for( _TPvPMap::iterator itor=m_mPvP.begin() ; itor!=m_mPvP.end() ; ++itor )
	{
		CDNPvP* pPvPRoom = (*itor).second;
		
		if( pPvPRoom->GetGambleRoomType() > 0 )
		{
			if( pPvPRoom->GetRoomState() == PvPCommon::RoomState::None )
			{
				CDNVillageConnection* pVillageCon = GetVillageConnectionByVillageID( pPvPRoom->GetVillageID() );

				pPvPRoom->bLeaveAll( pVillageCon, PvPCommon::LeaveType::DestroyRoom );
				if( pVillageCon )
					pVillageCon->SendPvPDestroyRoom( pPvPRoom->GetVillageChannelID(), pPvPRoom->GetIndex() );

				g_Log.Log(LogType::_PVPROOM, g_Config.nWorldSetID, 0, 0, 0, L"DeletePvPGambleRoom\r\n");

				delete pPvPRoom;
				m_mPvP.erase(itor);
				return;
			}
		}		
	}
}
#endif

#ifdef PRE_MOD_OPERATINGFARM
void CDivisionManager::CheckFarmSync(DWORD dwCurTick)
#else		//#ifdef PRE_MOD_OPERATINGFARM
void CDivisionManager::CheckFarmSync()
#endif		//#ifdef PRE_MOD_OPERATINGFARM
{
	if (m_vFarmCreateSync.empty())
		return;

	std::vector <TFarmItemFromDB>::iterator ii;
	for (ii = m_vFarmCreateSync.begin(); ii != m_vFarmCreateSync.end(); ii++)
	{
#ifdef PRE_MOD_OPERATINGFARM
		CreateFarm((*ii), dwCurTick);
#else		//#ifdef PRE_MOD_OPERATINGFARM
		if (CreateFarm((*ii)) == NULL)
			_DANGER_POINT();		//어헐 여까지 와서 이러심 안뎁니다.
#endif		//#ifdef PRE_MOD_OPERATINGFARM
	}
}

#ifdef PRE_MOD_OPERATINGFARM
bool CDivisionManager::IsFarmConnectionWatingTime(DWORD dwCurTick)
{
	if (m_dwFarmDataCreateTick == 0 || m_dwFarmDataCreateTick + Farm::Common::OPERATINGFARM_LIMIT > dwCurTick)
		return true;
	return false;
}

CDNFarm * CDivisionManager::CreateFarm(TFarmItemFromDB &FarmItem, DWORD dwCurTick)
{
	if (IsFarmGameConnectionAvailable(dwCurTick) == false)
	{
#else		//#ifdef PRE_MOD_OPERATINGFARM
CDNFarm * CDivisionManager::CreateFarm(TFarmItemFromDB &FarmItem)
{
	if (GetGameConnectionCount() <= 0)
	{
#endif		//#ifdef PRE_MOD_OPERATINGFARM
		//생성할 게임서버가 아직 연결 전이라면 버퍼에 담아둔다
		std::vector <TFarmItemFromDB>::iterator ih;
		for (ih = m_vFarmCreateSync.begin(); ih != m_vFarmCreateSync.end(); ih++)
		{
			if ((*ih).iFarmDBID == FarmItem.iFarmDBID)
				return NULL;
		}

		m_vFarmCreateSync.push_back(FarmItem);
#ifdef PRE_MOD_OPERATINGFARM
		SetCreateFarmDataTick(timeGetTime());
#endif		//#ifdef PRE_MOD_OPERATINGFARM
		return NULL;
	}

	CDNFarm * pFarm = GetFarm(FarmItem.iFarmDBID);
	if (pFarm != NULL)
		return pFarm;

	//자 없슈~ 생성~
#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	pFarm = new CDNFarm(FarmItem);
#else
	pFarm = new CDNFarm(FarmItem.iFarmDBID, FarmItem.iFarmMapID, FarmItem.bStartActivate);
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	if (pFarm == NULL)
		return NULL;
	
	bool bFlag = true;
	int nAssignedGameID = -1;
#if defined( PRE_ADD_FARM_DOWNSCALE )
	if (RequestFarmGameRoom(FarmItem.iFarmDBID, FarmItem.iFarmMapID, FarmItem.iFarmMaxUser, FarmItem.bStartActivate, nAssignedGameID, FarmItem.iAttr ) == false)
#elif defined( PRE_ADD_VIP_FARM )
	if (RequestFarmGameRoom(FarmItem.iFarmDBID, FarmItem.iFarmMapID, FarmItem.iFarmMaxUser, FarmItem.bStartActivate, nAssignedGameID, FarmItem.Attr ) == false)
#else
	if (RequestFarmGameRoom(FarmItem.iFarmDBID, FarmItem.iFarmMapID, FarmItem.iFarmMaxUser, FarmItem.bStartActivate, nAssignedGameID) == false)
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		bFlag = false;
	
	if (bFlag)
	{
		if (pFarm->SetAssignedServerID(nAssignedGameID) == false)
			bFlag = false;
	}

	if (bFlag)
		m_mFarmList.insert(std::make_pair(pFarm->GetFarmDBID(), pFarm));
	else
	{
		delete pFarm;
		return NULL;
	}
	return pFarm;
}

CDNFarm * CDivisionManager::GetFarm(UINT nFarmDBID)
{
	if (m_mFarmList.empty()) return NULL;

	std::map <UINT, CDNFarm*>::iterator ii = m_mFarmList.find(nFarmDBID);
	return (ii != m_mFarmList.end()) ? (*ii).second : NULL;
}

bool CDivisionManager::DestroyFarm(UINT nFarmDBID)
{
	std::map <UINT, CDNFarm*>::iterator ii = m_mFarmList.find(nFarmDBID);
	if (ii != m_mFarmList.end())
	{
		(*ii).second->DestroyFarm();
		m_bDestroyFarm = true;
		return true;
	}
	return false;
}

bool CDivisionManager::SetFarmDataLoaded(UINT nFarmDBID, int nGameServerID, int nGameServerIdx, UINT nRoomID, int nMapID, int nFarmMaxUser)
{
	CDNFarm * pFarm = GetFarm(nFarmDBID);
	if (pFarm)
	{
		if (pFarm->SetAssignedFarmData(nGameServerID, nGameServerIdx, nRoomID, nMapID, nFarmMaxUser))
		{
			SendFarmInfo();
			return true;
		}
	}

	g_Log.Log(LogType::_FARM, L"FarmID[%d] SetFarmDataLoaded Fail\n", nFarmDBID);
	return false;
}

bool CDivisionManager::SetFarmUpdateUserCount(UINT nFarmDBID, UINT nRoomID, int nCurUserCount, bool bStarted, int nManagedID)
{
	CDNFarm * pFarm = GetFarm(nFarmDBID);
	if (pFarm)
	{
		pFarm->SetFarmCurUserCount(nCurUserCount, bStarted, nManagedID);
		return true;
	}

	_DANGER_POINT();		//헐 없뎅~
	return false;
}


void CDivisionManager::Notice(const WCHAR * pMsg, const int nLen, int nShowSec)
{
	if (nLen >= CHATLENMAX) 
	{
		_DANGER_POINT();
		return;
	}
	//다 쏴버려!
	for (int i = 0; i < (int)m_GameServerConList.size(); i++)
		m_GameServerConList[i]->SendNotice(pMsg, nLen, nShowSec);

	for (int i = 0; i < (int)m_VillageServerConList.size(); i++)
		m_VillageServerConList[i]->SendNotice(pMsg, nLen, nShowSec);
}

void CDivisionManager::NoticeZone(int nMapIndex, const WCHAR * pMsg, const int nLen, int nShowSec)
{
	if (nLen >= CHATLENMAX) 
	{
		_DANGER_POINT();
		return;
	}
	//다 쏴버려!
	for (int i = 0; i < (int)m_VillageServerConList.size(); i++)
		m_VillageServerConList[i]->SendNoticeZone(nMapIndex, pMsg, nLen, nShowSec);
}

void CDivisionManager::NoticeChannel(int nChannelID, const WCHAR * pMsg, const int nLen, int nShowSec)
{
	if (nLen >= CHATLENMAX) 
	{
		_DANGER_POINT();
		return;
	}
	for (int i = 0; i < (int)m_VillageServerConList.size(); i++)
		m_VillageServerConList[i]->SendNoticeChannel(nChannelID, pMsg, nLen, nShowSec);
}

void CDivisionManager::NoticeServer(int nManagedID, const WCHAR * pMsg, const int nLen, int nShowSec)
{
	if (nLen >= CHATLENMAX) 
	{
		_DANGER_POINT();
		return;
	}
	for (int i = 0; i < (int)m_GameServerConList.size(); i++)
		m_GameServerConList[i]->SendNoticeServer(nManagedID, pMsg, nLen, nShowSec);

	for (int i = 0; i < (int)m_VillageServerConList.size(); i++)
		m_VillageServerConList[i]->SendNoticeServer(nManagedID, pMsg, nLen, nShowSec);
}

void CDivisionManager::NoticeCancel()
{
	for (int i = 0; i < (int)m_GameServerConList.size(); i++)
		m_GameServerConList[i]->SendNoticeCancel();

	for (int i = 0; i < (int)m_VillageServerConList.size(); i++)
		m_VillageServerConList[i]->SendNoticeCancel();
}

int CDivisionManager::SendInvitePartyMember(int nGameServerID, const GAMAInvitePartyMember * pInvitePartyMember)
{
	CDNUser * pDestUser = GetUserByName(pInvitePartyMember->wszInvitedName);
	if (pDestUser)
	{
		//초대할 대상이 있습니다. 기본적인 판단을 해봅니다.
		if (pDestUser->GetUserState() != STATE_VILLAGE)
			return ERROR_PARTY_INVITEFAIL;

		//파튀가 어서 왔는지 알아야겠다.
		const TChannelInfo * pChannelInfo = GetChannelInfo(pInvitePartyMember->nChannelID);	//어디서 오신 파티이신지
		const TChannelInfo * pDestChannelInfo = GetChannelInfo(pDestUser->GetChannelID());	//어디서 오신 파티이신지
		if (pChannelInfo == NULL || pDestChannelInfo == NULL)
			return ERROR_GENERIC_UNKNOWNERROR;

		if (pDestUser->bIsPvPLobby())									//안되는 이유비교
			return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;

#if defined( PRE_PARTY_DB )
#else
		if (pChannelInfo->nMapIdx != pDestChannelInfo->nMapIdx)			//초대대상의 마을과 비교
			return ERROR_PARTY_INVITEFAIL_DESTLOCATION_NOT_SAME;
#endif // #if defined( PRE_PARTY_DB )

		//더 화인해봐야 할게 있나?...일단 바로 빌리지에 쏴줍니다.
		CDNVillageConnection * pVillageCon = GetVillageConnectionByVillageID(pDestUser->GetVillageID());
		if (pVillageCon == NULL)
			return ERROR_GENERIC_VILLAGECON_NOT_FOUND;

		pVillageCon->SendInvitepartyMember(nGameServerID, pChannelInfo->nMapIdx, pDestUser->GetAccountDBID(), pInvitePartyMember, pInvitePartyMember->cPassClassIds, pInvitePartyMember->cPermitLevel);
		return ERROR_NONE;
	}
	SendInvitePartyMemberResult(nGameServerID, pInvitePartyMember->wszInviterName, pInvitePartyMember->wszInvitedName, ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND);
	return ERROR_PARTY_INVITEFAIL_DESTUSER_NOTFOUND;
}

bool CDivisionManager::SendInvitePartyMemberResult(int nGameServerID, const WCHAR * pwszInviterName, const WCHAR * pwszInvitedName, int nRetCode)
{
	CDNGameConnection * pGameCon = GetGameConnectionByGameID(nGameServerID);
	if (pGameCon)
	{
		if(pGameCon->GetZeroPopulation())
			return false;

		CDNUser * pUser = GetUserByName(pwszInviterName);
		if (pUser == NULL) return false;

		if (nRetCode == ERROR_NONE)
		{
			CDNUser * pInvitedUser = GetUserByName(pwszInvitedName);
			if (pInvitedUser)
			{
				pGameCon->SendBreakintoRoom( pUser->m_nRoomID, pInvitedUser, BreakInto::Type::WorldZoneParty );
				return true;
			}
		}
		else
		{
			pGameCon->SendInivitePartyMemberResult(pUser->GetAccountDBID(), pwszInvitedName, nRetCode);
			return true;
		}
	}
	
	return false;
}

bool CDivisionManager::SendInvitedPartyMemberReturn(UINT nAccountDBID, int nRetCode)
{
	CDNUser * pUser = GetUserByAccountDBID(nAccountDBID);
	if (pUser)
	{
		CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
		if (pVillCon)
		{
			pVillCon->SendResult(nAccountDBID, MAVI_INVITEPARTYMEMBER_RETMSG, nRetCode);
			return true;
		}
	}
	return false;
}

bool CDivisionManager::SendDetachUser(UINT nAccountDBID, bool bIsReconnectLogin, bool bIsDuplicate, UINT nSessionID)
{
	CDNUser *pUser = GetUserByAccountDBID(nAccountDBID);
	if (!pUser)
	{
		if( bIsDuplicate )		
			return false;
		return true;
	}	

	g_Log.Log( LogType::_NORMAL, g_Config.nWorldSetID, nAccountDBID, 0, pUser->GetSessionID(), L"[ADBID:%u CDBID:%I64d SID:%u] SendDetachUser (State:%d)\r\n", pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), pUser->GetUserState());
	
	bool bSend = false;
	bool bGameSend = false;

	switch(pUser->GetUserState())
	{	
	case STATE_VILLAGE:
		{
			bSend = SendDetachUserAllVillage(nAccountDBID, nSessionID, pUser->GetVillageID(), bIsDuplicate);
		}
		break;
	
	case STATE_GAME:
		{
			bSend = SendDetachUserAllGame(nAccountDBID, nSessionID, pUser->GetGameID(), bIsDuplicate);
		}
		break;

	case STATE_CHECKVILLAGE:
	case STATE_CHECKGAME:
	case STATE_CHECKRECONNECTLOGIN:
		{
			bSend = SendDetachUserAllVillage(nAccountDBID, nSessionID, pUser->GetPreVillageID(), bIsDuplicate);
			if( pUser->GetGameID() )
				bGameSend = SendDetachUserAllGame(nAccountDBID, nSessionID, pUser->GetGameID(), bIsDuplicate);
		}
		break;
	}
	// 중복로그인 끊기이고 각 서버에 보냈으면 DelUser하지 말자
	if( bIsDuplicate )
	{
		if( bSend || bGameSend)
		{
			g_Log.Log( LogType::_NORMAL, pUser, L"[ADBID:%u CDBID:%I64d SID:%u] SendDetachDuplicateUser (State:%d)\r\n", pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), pUser->GetUserState());
			return true;
		}
		else
		{
			// 서버에 못 보냈으면 지우고 짤라버리라고 함.
			DelUser(nAccountDBID, bIsReconnectLogin);
			return false;
		}
	}
	DelUser(nAccountDBID, bIsReconnectLogin);	// 서버가 없으면 걍 유저 지워준다
	return true;
}

bool CDivisionManager::SendDetachUserAllVillage(UINT nAccountDBID, INT nSessionID, BYTE cCurVillageID, bool bIsDuplicate )
{
	if (m_VillageServerConList.empty()) return false;

	ScopeLock<CSyncLock> Lock(m_Sync);
	bool bResult = false;

	std::vector <CDNVillageConnection*>::iterator ii;
	for (ii = m_VillageServerConList.begin(); ii != m_VillageServerConList.end(); ii++)
	{
		if( (*ii)->GetConnectionCompleted() )
		{
			if( (*ii)->GetVillageID() == cCurVillageID )
			{
				// 현재 Village 한테만 중복로그인 체크
				(*ii)->SendDetachUser(nAccountDBID, bIsDuplicate, nSessionID);
				bResult = true;
				g_Log.Log( LogType::_NORMAL, 0, nAccountDBID, 0, nSessionID, L"SendDetachUserVillage (Village:%d)\r\n", cCurVillageID);
			}
			else			
				(*ii)->SendDetachUser(nAccountDBID, false, nSessionID);
		}	
	}
	return bResult;
}

bool CDivisionManager::SendDetachUserAllGame(UINT nAccountDBID, INT nSessionID, INT nGameID, bool bIsDuplicate)
{
	if (m_GameServerConList.empty()) return false;

	ScopeLock<CSyncLock> Lock(m_Sync);

	bool bResult = false;

	std::vector <CDNGameConnection*>::iterator ii;
	for (ii = m_GameServerConList.begin(); ii != m_GameServerConList.end(); ii++)
	{
		if( (*ii)->GetConnectionCompleted() )
		{
			if ((*ii)->GetGameID() == nGameID )
			{
				// 현재 Game 한테만 중복로그인 체크
				(*ii)->SendDetachUser(nAccountDBID, bIsDuplicate, nSessionID);
				bResult = true;
				g_Log.Log( LogType::_NORMAL, 0, nAccountDBID, 0, nSessionID, L"SendDetachUserGame (GameID:%d)\r\n", nGameID);
			}
			else
				(*ii)->SendDetachUser(nAccountDBID, false, nSessionID);
		}
	}
	return bResult;
}

void CDivisionManager::SendNotifyMail(UINT nToAccountDBID, INT64 biToCharacterDBID, short wTotalMailCount, short wNotReadMailCount, short w7DaysLeftCount, bool bNewMail)
{
	CDNUser *pUser = GetUserByAccountDBID(nToAccountDBID);
	if (!pUser) return;

	if (pUser->GetUserState() == STATE_VILLAGE){
		CDNVillageConnection *pVillageCon = pUser->GetCurrentVillageConnection();
		if (pVillageCon)
			pVillageCon->SendNotifyMail(nToAccountDBID, biToCharacterDBID, wTotalMailCount, wNotReadMailCount, w7DaysLeftCount, bNewMail);
	}
	else if (pUser->GetUserState() == STATE_GAME){
		CDNGameConnection *pGameCon = pUser->GetCurrentGameConnection();
		if (pGameCon)
			pGameCon->SendNotifyMail(nToAccountDBID, biToCharacterDBID, wTotalMailCount, wNotReadMailCount, w7DaysLeftCount, bNewMail);
	}
}

void CDivisionManager::SendNotifyMarket(UINT nSellerAccountDBID, INT64 biSellerCharacterDBID, int nItemID, short wCalculationCount)
{
	CDNUser *pUser = GetUserByAccountDBID(nSellerAccountDBID);
	if (!pUser) return;

	if (pUser->GetUserState() == STATE_VILLAGE){
		CDNVillageConnection *pVillageCon = pUser->GetCurrentVillageConnection();
		if (pVillageCon)
			pVillageCon->SendNotifyMarket(nSellerAccountDBID, biSellerCharacterDBID, nItemID, wCalculationCount);
	}
	else if (pUser->GetUserState() == STATE_GAME){
		CDNGameConnection *pGameCon = pUser->GetCurrentGameConnection();
		if (pGameCon)
			pGameCon->SendNotifyMarket(nSellerAccountDBID, biSellerCharacterDBID, nItemID, wCalculationCount);
	}
}

void CDivisionManager::SendNotifyGift(UINT nToAccountDBID, INT64 biToCharacterDBID, bool bNew, int nGiftCount)
{
	CDNUser *pUser = GetUserByAccountDBID(nToAccountDBID);
	if (!pUser) return;

	if (pUser->GetUserState() == STATE_VILLAGE){
		CDNVillageConnection *pVillageCon = pUser->GetCurrentVillageConnection();
		if (pVillageCon)
			pVillageCon->SendNotifyGift(nToAccountDBID, biToCharacterDBID, bNew, nGiftCount);
	}
	else if (pUser->GetUserState() == STATE_GAME){
		CDNGameConnection *pGameCon = pUser->GetCurrentGameConnection();
		if (pGameCon)
			pGameCon->SendNotifyGift(nToAccountDBID, biToCharacterDBID, bNew, nGiftCount);
	}
}

void CDivisionManager::SendVillageInfo()
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::vector <TVillageInfo> vList;
	GetVillageServerInfo(&vList);
	if (vList.size() > 0)
	{
		SendAllLoginServerVillageInfo((UINT)m_AccountDBIDList.size(), g_pWaitUserManager->GetWorldMaxUser(), &vList);

		for (int i = 0; i < (int)m_VillageServerConList.size(); i++)
			m_VillageServerConList[i]->SendVillageInfo(&vList);
	}

	if (g_pServiceConnection)
		g_pServiceConnection->SendVillageInfo((UINT)m_AccountDBIDList.size(), &vList);
}

void CDivisionManager::SendVillageChannelShowInfo( USHORT unChannelID, bool bShow )
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	// LoginServer
	MALOUpdateChannelShowInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.unChannelID	= unChannelID;
	TxPacket.bShow			= bShow;

	SendAllLoginServer( MALO_UPDATECHANNELSHOWINFO, 0, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );

	// VillageServer
	MAVIUpdateChannelShowInfo TxPacket2;
	memset( &TxPacket2, 0, sizeof(TxPacket2) );

	TxPacket2.unChannelID	= unChannelID;
	TxPacket2.bShow			= bShow;

	SendAllVillageServer( MAVI_UPDATECHANNELSHOWINFO, 0, reinterpret_cast<char*>(&TxPacket2), sizeof(TxPacket2) );
}

void CDivisionManager::SendUpdateWorldEventCounter( MAUpdateWorldEventCounter* pPacket )
{
	// VillageServer
	SendAllVillageServer( MAVI_UPPDATE_WORLDEVENTCOUNTER, 0, reinterpret_cast<char*>(pPacket), sizeof(MAUpdateWorldEventCounter) );

	// GameServer
	SendAllGameServer( MAGA_UPPDATE_WORLDEVENTCOUNTER, 0, reinterpret_cast<char*>(pPacket), sizeof(MAUpdateWorldEventCounter) );
}

void CDivisionManager::SendFarmInfo()
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	MAVIFarmInfoUpdate packet;
	memset(&packet, 0, sizeof(MAVIFarmInfoUpdate));

	std::map <UINT, CDNFarm*>::iterator ii;
	for (ii = m_mFarmList.begin(); ii != m_mFarmList.end(); ii++)
	{
		packet.FarmInfo[packet.cFarmCount].nFarmDBID = (*ii).second->GetFarmDBID();
		packet.FarmInfo[packet.cFarmCount].nFarmCurUserCount = (*ii).second->GetFarmCurUserCount();
		packet.FarmInfo[packet.cFarmCount].bActivate = ((*ii).second->IsActivateFarm() && (*ii).second->IsStartedFarm()) ? true : false;

		packet.cFarmCount++;

		if (packet.cFarmCount >= Farm::Max::FARMCOUNT)
		{
			_DANGER_POINT();
			break;
		}
	}

	SendAllVillageServer(MAVI_FARMINFOUPDATE, 0, reinterpret_cast<char*>(&packet), sizeof(MAVIFarmInfoUpdate));
}

void CDivisionManager::GetFarmInfo(TServiceReportMaster * pData)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	std::map <UINT, CDNFarm*>::iterator ii;
	for (ii = m_mFarmList.begin(); ii != m_mFarmList.end(); ii++)
	{
		pData->FarmStatus[pData->cFarmCount].nManagedID = (*ii).second->GetManagedID();
		pData->FarmStatus[pData->cFarmCount].nFarmDBID = (*ii).second->GetFarmDBID();
		pData->FarmStatus[pData->cFarmCount].nFarmCurUserCount = (*ii).second->GetFarmCurUserCount();
		pData->FarmStatus[pData->cFarmCount].bActivate = ((*ii).second->IsActivateFarm() && (*ii).second->IsStartedFarm()) ? true : false;

		pData->cFarmCount++;

		if (pData->cFarmCount >= Farm::Max::FARMCOUNT/2)
		{
			_DANGER_POINT();
			break;
		}
	}
}



void CDivisionManager::SendPCBangResult(CDNUser *pUser)
{
	switch(pUser->GetUserState())
	{
	case STATE_CHECKVILLAGE:
	case STATE_VILLAGE:
		{
			CDNVillageConnection *pVillageCon = pUser->GetCurrentVillageConnection();
			if (pVillageCon)
				pVillageCon->SendPCBangResult(pUser->GetAccountDBID(), pUser);
			else
				DelUser(pUser->GetAccountDBID());	// 서버가 없으면 걍 유저 지워준다
		}
		break;

	case STATE_CHECKGAME:
	case STATE_GAME:
		{
			CDNGameConnection *pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if( pGameCon )
				pGameCon->SendPCBangResult(pUser->GetAccountDBID(), pUser);
			else
				DelUser(pUser->GetAccountDBID());	// 서버가 없으면 걍 유저 지워준다
		}
		break;

	default:
		break;
	}	
}

void CDivisionManager::OnDisconnectServer( const int iConnectionKey, const int iServerID, int nManagedID )
{
	//external thread call Watch!!!!

}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDivisionManager::SyncSystemMail( VIMASyncSystemMail* pMail )
{
	CDNUser* pUser = GetUserByCharacterDBID( pMail->biReceiverCharacterDBID );
	if( pUser == NULL )
		return;

	switch( pUser->GetUserState() )
	{
		case STATE_CHECKVILLAGE:
		case STATE_VILLAGE:
		{
			CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
			if( pVillageCon )
				pVillageCon->SendSyncSystemMail( pUser->GetAccountDBID(), pMail );
			break;
		}
		case STATE_CHECKGAME:
		case STATE_GAME:
		{
			CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
			if( pGameCon )
				pGameCon->SendSyncSystemMail( pUser->GetAccountDBID(), pMail );
			break;
		}
	}
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

void CDivisionManager::SyncMasterSystemSimpleInfo( MasterSystem::VIMASyncSimpleInfo* pPacket )
{
	CDNUser* pUser = g_pDivisionManager->GetUserByCharacterDBID( pPacket->biCharacterDBID );
	if( pUser == NULL )
		return;

	switch( pUser->GetUserState() )
	{
		case STATE_CHECKVILLAGE:
		case STATE_VILLAGE:
		{
			CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
			if( pVillageCon )
				pVillageCon->SendMasterSystemSyncSimpleInfo( pUser->GetAccountDBID(), pPacket->biCharacterDBID, pPacket->EventCode );
			break;
		}
		case STATE_CHECKGAME:
		case STATE_GAME:
		{
			CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
			if( pGameCon )
				pGameCon->SendMasterSystemSyncSimpleInfo( pUser->GetAccountDBID(), pPacket->biCharacterDBID, pPacket->EventCode );
			break;
		}
	}
}

void CDivisionManager::SyncMasterSystemGraduate( MasterSystem::VIMASyncGraduate* pPacket )
{
	CDNUser* pUser = GetUserByCharacterDBID( pPacket->biCharacterDBID );
	if( pUser == NULL )
		return;

	switch( pUser->GetUserState() )
	{
		case STATE_VILLAGE:
		{
			CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
			if( pVillageCon )
				pVillageCon->SendMasterSystemSyncGraduate( pUser->GetAccountDBID(), pPacket );
			break;
		}
		case STATE_GAME:
		{
			CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
			if( pGameCon )
				pGameCon->SendMasterSystemSyncGraduate( pUser->GetAccountDBID(), pPacket );
			break;
		}
	}
}

void CDivisionManager::SyncMasterSystemConnect( MasterSystem::VIMASyncConnect* pPacket )
{
	for( UINT i=0 ;i<pPacket->cCharacterDBIDCount ; ++i )
	{
		CDNUser* pUser = GetUserByCharacterDBID( pPacket->CharacterDBIDList[i] );
		if( pUser == NULL )
			continue;

		switch( pUser->GetUserState() )
		{
			case STATE_VILLAGE:
			{
				CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
				if( pVillageCon )
					pVillageCon->SendMasterSystemSyncConnect( pUser->GetAccountDBID(), pPacket->bIsConnect, pPacket->wszCharName );
				break;
			}
			case STATE_GAME:
			{
				CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
				if( pGameCon )
					pGameCon->SendMasterSystemSyncConnect( pUser->GetAccountDBID(), pPacket->bIsConnect, pPacket->wszCharName );
				break;
			}
		}
	}
}

void CDivisionManager::SyncFarm( GAMAFarmSync* pPacket )
{
	CDNUser* pUser = GetUserByCharacterDBID( pPacket->biCharacterDBID );
	if( pUser == NULL )
		return;
	
	bool bSyncGameServer	= false;
	bool bSyncVillageServer	= false;

	switch( pPacket->Type )
	{
		case Farm::ServerSyncType::FIELDCOUNT:
		case Farm::ServerSyncType::FIELDLIST:
		{
			bSyncGameServer		= true;
			break;
		}
		case Farm::ServerSyncType::WAREHOUSE_ITEMCOUNT:
		{
			bSyncGameServer		= true;
			bSyncVillageServer	= true;
			break;
		}
	}

	switch( pUser->GetUserState() )
	{
		case STATE_GAME:
		{
			CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
			if( pGameCon && bSyncGameServer )
				pGameCon->SendFarmSync( pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pPacket->Type );
			break;
		}
		case STATE_VILLAGE:
		{
			CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
			if( pVillageCon && bSyncVillageServer)
				pVillageCon->SendFarmSync( pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pPacket->Type );
			break;
		}
	}
}

void CDivisionManager::SyncFarmAddWater( GAMAFarmSyncAddWater* pPacket )
{
	CDNUser* pUser = GetUserByCharacterDBID( pPacket->biCharacterDBID );
	if( pUser == NULL )
		return;

	switch( pUser->GetUserState() )
	{
		case STATE_GAME:
		{
			CDNGameConnection* pGameCon = g_pDivisionManager->GetGameConnectionByGameID( pUser->GetGameID() );
			if( pGameCon )
				pGameCon->SendFarmSyncAddWater( pUser->GetAccountDBID(), pPacket->wszCharName, pPacket->iAddPoint );
			break;
		}
	}
}


void CDivisionManager::SendLoginUserDetach(UINT nAccountDBID, int nLoginServerID)
{
	for (int nIndex = 0 ; LOGINCOUNTMAX > nIndex ; ++nIndex) 
	{
		// 나를 제외한 다른 서버들에게 끊어달라고 요청..
		if( m_pLoginConnectionList[nIndex] && m_pLoginConnectionList[nIndex]->GetServerID() != nLoginServerID )
		{
			if( m_pLoginConnectionList[nIndex]->GetActive() )
			{
				m_pLoginConnectionList[nIndex]->SendDetachUser(nAccountDBID);
			}
		}		
	}
}

#if defined(PRE_ADD_QUICK_PVP)
int CDivisionManager::MakeQuickPvPRoom(UINT nMasterAccountDBID, UINT nSlaveAccountDBID)
{
	CDNUser* pMasterUser = g_pDivisionManager->GetUserByAccountDBID(nMasterAccountDBID);
	if( pMasterUser == NULL)
		return ERROR_PVP_CREATEROOM_FAILED;
	
	CDNUser* pSlaveUser = g_pDivisionManager->GetUserByAccountDBID(nSlaveAccountDBID);
	if( pSlaveUser == NULL)
		return ERROR_PVP_CREATEROOM_FAILED;

	if (pMasterUser->GetUserState() != STATE_VILLAGE || pSlaveUser->GetUserState() != STATE_VILLAGE)
		return ERROR_PVP_CREATEROOM_FAILED;

	CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
	if( pVillageCon == NULL || pVillageCon->GetActive() == false )
		return ERROR_PVP_CREATEROOM_FAILED;

	if( pMasterUser->GetPvPIndex() > 0 || pSlaveUser->GetPvPIndex() > 0 )
		return ERROR_PVP_CREATEROOM_FAILED;

	// MapID 얻어오기
	UINT uiMapIndex = 10003;	// 죽음의 결투장(s) 하드코딩..
	/*
	// MapID로 PvPMapTable 얻어오기	
	const TPvPMapTable* pMapTable = g_pExtManager->GetPvPMapTable(uiMapIndex);
	if( pMapTable == NULL )	
		return ERROR_PVP_CREATEROOM_FAILED;
	
	// PvPMapTable에서 GameMode 정보 가져오기
	const TPvPGameModeTable* pPvPGameModeTable = g_pExtManager->GetPvPGameModeTable( pMapTable->vGameModeTableID[0] );
	if( pPvPGameModeTable == NULL )
		return ERROR_PVP_CREATEROOM_FAILED;
		*/

	// 여기서 방만들기..
	VIMAPVP_CREATEROOM sCreateRoom;
	memset(&sCreateRoom, 0, sizeof(sCreateRoom));
	sCreateRoom.unVillageChannelID = GetPvPLobbyChannelID();
	sCreateRoom.uiVillageMapIndex = GetPvPLobbyMapIndex();
	//sCreateRoom.uiCreateAccountDBID = nMasterAccountDBID;
	sCreateRoom.cGameMode =  PvPCommon::GameMode::PvP_Round;

	
	sCreateRoom.sCSPVP_CREATEROOM.uiMapIndex = uiMapIndex;
	sCreateRoom.sCSPVP_CREATEROOM.uiGameModeTableID = 10;
	sCreateRoom.sCSPVP_CREATEROOM.uiSelectWinCondition = 5; // 5라운드
	sCreateRoom.sCSPVP_CREATEROOM.uiSelectPlayTimeSec = 180;
	sCreateRoom.sCSPVP_CREATEROOM.cMaxUser = 2;
	sCreateRoom.sCSPVP_CREATEROOM.cMinUser = 0;
	sCreateRoom.sCSPVP_CREATEROOM.cRoomPWLen = 0;
	sCreateRoom.sCSPVP_CREATEROOM.cMinLevel = 1;
	sCreateRoom.sCSPVP_CREATEROOM.cMaxLevel = 100;
	sCreateRoom.sCSPVP_CREATEROOM.unRoomOptionBit = 0;
	sCreateRoom.sCSPVP_CREATEROOM.uiEventItemID = 0;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	sCreateRoom.sCSPVP_CREATEROOM.cRoomType = static_cast<BYTE>(PvPCommon::RoomType::regular);

	pMasterUser->SetPvPChannelType(static_cast<BYTE>(PvPCommon::RoomType::regular));
	pVillageCon->SendPvPChangeChannelResult(pMasterUser->GetAccountDBID(), static_cast<BYTE>(pMasterUser->GetPvPChannelType()), ERROR_NONE);
	
	pSlaveUser->SetPvPChannelType(static_cast<BYTE>(PvPCommon::RoomType::regular));
	pVillageCon->SendPvPChangeChannelResult(pSlaveUser->GetAccountDBID(), static_cast<BYTE>(pSlaveUser->GetPvPChannelType()), ERROR_NONE);
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	wsprintf(sCreateRoom.sCSPVP_CREATEROOM.wszBuf, L"%s vs %s", pMasterUser->GetCharacterName(), pSlaveUser->GetCharacterName());

	sCreateRoom.sCSPVP_CREATEROOM.cRoomNameLen = (BYTE)wcslen(sCreateRoom.sCSPVP_CREATEROOM.wszBuf);

	UINT uiPvPIndex = 0;
	short nRetCode = g_pDivisionManager->CreatePvPRoom( pVillageCon, &sCreateRoom, &uiPvPIndex );
	// 마스터 셋팅 및 PvPIndex 셋팅
	if( nRetCode == ERROR_NONE )
	{		
		CDNPvP* pPvP = GetPvPRoomByIdx(uiPvPIndex);
		if( pPvP )
		{
			pMasterUser->SetPvPIndex(uiPvPIndex);
			pMasterUser->SetPvPTeam(PvPCommon::Team::A);
			pMasterUser->SetPvPUserState( PvPCommon::UserState::Captain );
			pPvP->SetCaptainAccountDBID(nMasterAccountDBID);
			pPvP->AddListAccountDBID(nMasterAccountDBID);

			pSlaveUser->SetPvPIndex(uiPvPIndex);
			pSlaveUser->SetPvPTeam(PvPCommon::Team::B);
			pSlaveUser->SetPvPUserState( PvPCommon::UserState::None );
			pPvP->AddListAccountDBID(nSlaveAccountDBID);
		}
	}
	return nRetCode;
}
#endif

#if defined( PRE_WORLDCOMBINE_PARTY )
int CDivisionManager::GetWorldPartyMember( MAGetWorldPartyMember* pPacket )
{
	CDNGameConnection* pGameCon = GetGameConnectionByManagedID(pPacket->iServerID);
	if(pGameCon)
	{
		pGameCon->SendGetWorldPartyMember(pPacket);
		return ERROR_NONE;
	}
	return ERROR_PARTY_JOINFAIL;
}
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )
int CDivisionManager::AddPrivateChatChannel( MAAddPrivateChannel* pPacket )
{
	// 전체 VI 에 송신
	pPacket->cWorldSetID = g_Config.nWorldSetID;
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendPrivateChatChannelAdd(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator iterg;
	for (iterg = m_GameServerConList.begin() ; m_GameServerConList.end() != iterg ; ++iterg) 
	{
		
		(*iterg)->SendPrivateChatChannelAdd(pPacket);
	}
	return ERROR_NONE;
}

int CDivisionManager::AddPrivateChatChannelMember( MAAddPrivateChannelMember* pPacket )
{
	pPacket->cWorldSetID = g_Config.nWorldSetID;
	// 전체 VI 에 송신
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendPrivateChatChannelMemberAdd(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator iterg;
	for (iterg = m_GameServerConList.begin() ; m_GameServerConList.end() != iterg ; ++iterg)
	{		
		(*iterg)->SendPrivateChatChannelMemberAdd(pPacket);
	}
	return ERROR_NONE;
}

int CDivisionManager::InvitePrivateChatChannelMember( MAInvitePrivateChannelMember* pPacket )
{	
	CDNUser* pUser = GetUserByName( pPacket->wszInviteName );

	if (pUser)
	{
#if defined(PRE_ADD_DWC)
		if( pUser->GetCharacterAccountLevel() == AccountLevel_DWC )
		{
			MAInvitePrivateChannelMemberResult InviteResult;
			memset(&InviteResult, 0, sizeof(InviteResult));
			InviteResult.cWorldSetID = pPacket->cWorldSetID;
			InviteResult.nMasterAccountDBID = pPacket->nMasterAccountDBID;
			InviteResult.nRet = ERROR_NOTEXIST_INVITEUSER;
			g_pDivisionManager->InviteResultPrivateChatChannelMember( &InviteResult );
			return ERROR_NONE;
		}
#endif
		pPacket->nInviteAccountDBID = pUser->GetAccountDBID();
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendPrivateChatChannelMemberInvite(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendPrivateChatChannelMemberInvite(pPacket);
		}
	}
	else
	{
		MAInvitePrivateChannelMemberResult InviteResult;
		memset(&InviteResult, 0, sizeof(InviteResult));
		InviteResult.cWorldSetID = pPacket->cWorldSetID;
		InviteResult.nMasterAccountDBID = pPacket->nMasterAccountDBID;
		InviteResult.nRet = ERROR_NOTEXIST_INVITEUSER;
		g_pDivisionManager->InviteResultPrivateChatChannelMember( &InviteResult );
	}
	return ERROR_NONE;
}

int CDivisionManager::InviteResultPrivateChatChannelMember( MAInvitePrivateChannelMemberResult* pPacket )
{	
	CDNUser* pUser = GetUserByAccountDBID( pPacket->nMasterAccountDBID );

	if (pUser)
	{		
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendPrivateChatChannelMemberInviteResult(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendPrivateChatChannelMemberInviteResult(pPacket);
		}
	}
	return ERROR_NONE;
}

int CDivisionManager::DelPrivateChatChannelMember( MADelPrivateChannelMember* pPacket )
{	
	pPacket->cWorldSetID = g_Config.nWorldSetID;
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendPrivateChatChannelMemberDel(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator iterg;
	for (iterg = m_GameServerConList.begin() ; m_GameServerConList.end() != iterg ; ++iterg)
	{		
		(*iterg)->SendPrivateChatChannelMemberDel(pPacket);
	}
	return ERROR_NONE;
}

int CDivisionManager::KickPrivateChatChannelMemberResult( MAKickPrivateChannelMemberResult* pPacket )
{	
	CDNUser* pUser = GetUserByCharacterDBID( pPacket->biCharacterDBID );
	
	if (pUser)
	{
		pPacket->nAccountDBID = pUser->GetAccountDBID();
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendPrivateChatChannelMemberKickResult(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendPrivateChatChannelMemberKickResult(pPacket);
		}
	}
	return ERROR_NONE;
}

int CDivisionManager::ModPrivateChatChannel( MAModPrivateChannel* pPacket )
{	
	pPacket->cWorldSetID = g_Config.nWorldSetID;
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendPrivateChatChannelMod(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator iterg;
	for (iterg = m_GameServerConList.begin() ; m_GameServerConList.end() != iterg ; ++iterg)
	{		
		(*iterg)->SendPrivateChatChannelMod(pPacket);
	}
	return ERROR_NONE;
}

int CDivisionManager::ModPrivateChatChannelMemberName( MAModPrivateChannelMemberName* pPacket )
{	
	pPacket->cWorldSetID = g_Config.nWorldSetID;
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendPrivateChatChannelModMemberName(pPacket);
	}

	// 전체 GA 에 송신
	vector<CDNGameConnection*>::iterator iterg;
	for (iterg = m_GameServerConList.begin() ; m_GameServerConList.end() != iterg ; ++iterg)
	{		
		(*iterg)->SendPrivateChatChannelModMemberName(pPacket);
	}
	return ERROR_NONE;
}
#endif

#ifdef _WORK
#include "StringUtil.h"
#include "DNWaitUserManager.h"
void CDivisionManager::Parse(const char * pMsg)
{
	std::string strCmd;
	strCmd = pMsg;
	ToLowerA(strCmd);

	std::vector<std::string> tokens;
	TokenizeA(strCmd, tokens, "|");
	
	std::wstring szWStr;
	if (tokens.size() > 0)
	{
		if (tokens[0] == "notice")
		{
			ToWideString(tokens[2], szWStr);
			Notice(szWStr.c_str(), (int)wcslen(szWStr.c_str()), atoi(tokens[1].c_str()));
		}
		if (tokens[0] == "noticezone")
		{
			ToWideString(tokens[3], szWStr);
			NoticeZone(atoi(tokens[1].c_str()), szWStr.c_str(), (int)wcslen(szWStr.c_str()), atoi(tokens[2].c_str()));
		}
		if (tokens[0] == "noticechannel")
		{
			ToWideString(tokens[3], szWStr);
			NoticeChannel(atoi(tokens[1].c_str()), szWStr.c_str(), (int)wcslen(szWStr.c_str()), atoi(tokens[2].c_str()));
		}
		if (tokens[0] == "userban")
		{
			ToWideString(tokens[1], szWStr);
			CDNUser * user = GetUserByName(szWStr.c_str());
			if (user)
				SendDetachUser(user->GetAccountDBID());
		}
		if (tokens[0] == "maxuser")
		{
			g_pWaitUserManager->Initialize(100);
		}
		if (tokens[0] == "selectjoin")
		{
			ToWideString(tokens[2], szWStr);
			g_pDivisionManager->AddSelectJoin(atoi(tokens[1].c_str()), szWStr.c_str());
		}
		if (tokens[0] == "clearselectjoin")
		{
			g_pDivisionManager->ClearSelectJoin();
		}
		if (tokens[0] == "destroypvp")
		{
			if (tokens.size() > 2)
			{
				ToWideString(tokens[2], szWStr);
				g_pDivisionManager->ForceStopPvP(atoi(tokens[1].c_str()), szWStr.c_str());
			}
			else
			{
				g_pDivisionManager->ForceStopPvP(atoi(tokens[1].c_str()));
			}
		}
		if (tokens[0] == "reloadext")
		{
			g_pDivisionManager->ReloadExt();
		}
		if (tokens[0] == "reloadact")
		{
			g_pDivisionManager->ReloadAct();
		}
		if (tokens[0] == "kick")
		{
			if (tokens.size() >= 2)
			{
				ToWideString(tokens[1], szWStr);
				CDNUser * pUser = g_pDivisionManager->GetUserByName(szWStr.c_str());
				if(pUser)
					g_pDivisionManager->SendDetachUser(pUser->GetAccountDBID());
			}
		}
		if (tokens[0] == "refreshguildware")
		{
			if (tokens.size() >= 2)
			{
				SendUpdateGuildWare(atoi(tokens[1].c_str()));
			}
		}
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
		if (tokens[0] == "guildchat")
		{
			if (tokens.size() >= 4)
			{
				if (g_pDivisionManager)
				{
					std::wstring wstrMessage;
					ToWideString(tokens[3], wstrMessage);
					if (g_pDivisionManager->DoorsGuildChat(_atoi64(tokens[1].c_str()), atoi(tokens[2].c_str()), wstrMessage.c_str(), static_cast<int>(wstrMessage.size())))
						_DANGER_POINT();
				}
			}
		}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	}
}
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )

int CDivisionManager::AlteiaSendTicket( MAAlteiaWorldSendTicket* pPacket )
{	
	CDNUser* pUser = GetUserByName( pPacket->wszRecvCharacterName );

	if (pUser)
	{
		pPacket->nAccountDBID = pUser->GetAccountDBID();
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
			{
				pVillCon->SendAlteiaWorldSendTicket(pPacket);
				return ERROR_NONE;
			}
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
			{
				pGameCon->SendAlteiaWorldSendTicket(pPacket);
				return ERROR_NONE;
			}
		}
	}
	return ERROR_ALTEIAWORLD_NOTEXISTUSER;
}

int CDivisionManager::AlteiaSendTicketResult( MAAlteiaWorldSendTicketResult* pPacket )
{	
	CDNUser* pUser = GetUserByCharacterDBID( pPacket->biSendCharacterDBID );

	if (pUser)
	{
		pPacket->nSendAccountDBID = pUser->GetAccountDBID();
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendAlteiaWorldSendTicketResult(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendAlteiaWorldSendTicketResult(pPacket);
		}
	}
	return ERROR_NONE;
}

#endif

#if defined( PRE_DRAGONBUFF )
int CDivisionManager::SendApplyWorldBuff( MAApplyWorldBuff* pPacket )
{		
	vector<CDNVillageConnection*>::iterator iterv;
	for (iterv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != iterv ; ++iterv) {
		if (0 != (*iterv)->GetManagedID() && pPacket->nManagedID == (*iterv)->GetManagedID()) {
			continue;
		}
		(*iterv)->SendApplyWorldBuff(pPacket);
	}
	
	return ERROR_NONE;
}
#endif

#if defined(PRE_ADD_DWC)
bool CDivisionManager::SendInviteDWCTeamMember(MAInviteDWCTeamMember* pPacket)
{
	// 길드초대할 유저를 찾는다
	CDNUser * pUser = GetUserByName(pPacket->wszToCharacterName);
	if( pUser && pUser->GetUserState() == STATE_VILLAGE)
	{
		CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
		if( pVillageCon )
		{
			pVillageCon->SendInviteDWCTeamMember(pPacket);
			return true;
		}
	}
	return false;
}

bool CDivisionManager::SendInviteDWCTeamMemberAck(MAInviteDWCTeamMemberAck* pPacket)
{
	// 길드원 초대 요청한 유저를 찾는다.
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nInviterDBID);
	if( pUser && pUser->GetUserState() == STATE_VILLAGE)
	{
		CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( pUser->GetVillageID() );
		if( pVillageCon )
		{
			pVillageCon->SendInviteDWCTeamMemberAck(pPacket);
			return true;
		}
	}
	return false;
}

bool CDivisionManager::SendChangeDWCTeamMemberState(MAChangeDWCTeamMemberState* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nTartgetUserAccountDBID);
	if (pUser)
	{
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendChangeDWCTeamMemberState(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendChangeDWCTeamMemberState(pPacket);
		}
	}
	return ERROR_NONE;
}

void CDivisionManager::SetDWCTeamMemberList(VIMADWCTeamMemberList *pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
	if (pUser)
	{	//빌리지 최초 접속시에만 처리
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
			pUser->SetDWCTeamInfo(pPacket);
	}
}

void CDivisionManager::SendDWCTeamChat(MADWCTeamChat *pPacket)
{
	CDNUser * pSender = g_pDivisionManager->GetUserByAccountDBID(pPacket->nAccountDBID);
	if (pSender && pSender->GetDWCTeamID() > 0 )
	{
		std::vector<Int64> vecMemberList;
		pSender->GetDWCMemberList(vecMemberList);

		for(int i = 0 ; i < vecMemberList.size() ; i++)
		{
			CDNUser * pUser = g_pDivisionManager->GetUserByCharacterDBID(vecMemberList[i]);
			if (pUser)
			{
				if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
				{
					CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
					if (pVillCon)
						pVillCon->SendChat(CHATTYPE_DWC_TEAM, pUser->GetAccountDBID(), pSender->GetCharacterName(), pPacket->wszChatMsg, pPacket->nLen);
				}
				else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
				{
					CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
					if (pGameCon)
						pGameCon->SendDWCTeamChat(CHATTYPE_DWC_TEAM, pUser->GetAccountDBID(), pSender->GetCharacterName(), pPacket->wszChatMsg, pPacket->nLen);
				}
			}
		}
	}
}

bool CDivisionManager::_RecvAddDWCTeamMember(MAAddDWCTeamMember* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nTartgetUserAccountDBID);
	if (pUser)
	{
		pUser->AddDWCMemberList(pPacket->biCharacterDBID);
		if(pPacket->bAlredySentByVillage)	//빌리지 서버에서 이미 유저에게 패킷전송했음
			return ERROR_NONE;

		return SendAddDWCTeamMember(pPacket, pUser);
	}
	return ERROR_NONE;
}

bool CDivisionManager::_RecvLeaveDWCTeamMember(MALeaveDWCTeamMember* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nTartgetUserAccountDBID);
	if (pUser)
	{
		pUser->DelDWCMemberList(pPacket->biLeaveUserCharacterDBID);
		if(pPacket->bAlredySentByVillage)	//빌리지 서버에서 이미 유저에게 패킷전송했음
			return ERROR_NONE;

		return SendLeaveDWCTeamMember(pPacket, pUser);
	}
	return ERROR_NONE;
}

bool CDivisionManager::_RecvDismissDWCTeam(MADismissDWCTeam* pPacket)
{
	CDNUser * pUser = g_pDivisionManager->GetUserByAccountDBID(pPacket->nTartgetUserAccountDBID);
	if (pUser)
	{
		pUser->ResetDWCInfo();
		if(pPacket->bAlredySentByVillage)	//빌리지 서버에서 이미 유저에게 패킷전송했음
			return ERROR_NONE;

		return SendDismissDWCTeam(pPacket, pUser);
	}
	return ERROR_NONE;
}

bool CDivisionManager::SendAddDWCTeamMember(MAAddDWCTeamMember* pPacket, CDNUser * pUser)
{
	if (pUser)
	{
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendAddDWCTeamMember(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendAddDWCTeamMember(pPacket);
		}
	}
	return ERROR_NONE;
}

bool CDivisionManager::SendLeaveDWCTeamMember(MALeaveDWCTeamMember* pPacket, CDNUser * pUser)
{
	if (pUser)
	{
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendLeaveDWCTeamMember(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendLeaveDWCTeamMember(pPacket);
		}
	}
	return ERROR_NONE;
}

bool CDivisionManager::SendDismissDWCTeam(MADismissDWCTeam* pPacket, CDNUser * pUser)
{
	if (pUser)
	{
		if (pUser->GetUserState() == STATE_CHECKVILLAGE || pUser->GetUserState() == STATE_VILLAGE)
		{
			CDNVillageConnection * pVillCon = GetVillageConnectionByVillageID(pUser->GetVillageID());
			if (pVillCon)
				pVillCon->SendDismissDWCTeam(pPacket);
		}
		else if (pUser->GetUserState() == STATE_CHECKGAME || pUser->GetUserState() == STATE_GAME)
		{
			CDNGameConnection * pGameCon = GetGameConnectionByGameID(pUser->GetGameID());
			if (pGameCon)
				pGameCon->SendDismissDWCTeam(pPacket);
		}
	}
	return ERROR_NONE;
}

void CDivisionManager::SendUpdateDWCScore(MADWCUpdateScore *pPacket)
{
	vector<CDNVillageConnection*>::iterator itorv;
	for (itorv = m_VillageServerConList.begin() ; m_VillageServerConList.end() != itorv ; ++itorv)
		(*itorv)->SendUpdateDWCScore(pPacket);
}

#endif