
#include "Stdafx.h"
#include "Log.h"
#include "DNFarm.h"
#include "DNDivisionManager.h"


#if defined( PRE_ADD_FARM_DOWNSCALE )
CDNFarm::CDNFarm( const TFarmItemFromDB& Farm )
{
	m_nFarmDBID = Farm.iFarmDBID;
	m_eFarmStatus = FARMSTATUS_NONE;
	m_nMapID = Farm.iFarmMapID;
	m_nCreateTick = timeGetTime();
	m_nFarmMaxUser = 0;
	m_bStartFarm = Farm.bStartActivate;
	m_iAttr = Farm.iAttr;
	m_nManagedID = 0;

	ResetData();
}
#elif defined( PRE_ADD_VIP_FARM )
CDNFarm::CDNFarm( const TFarmItemFromDB& Farm )
{
	m_nFarmDBID = Farm.iFarmDBID;
	m_eFarmStatus = FARMSTATUS_NONE;
	m_nMapID = Farm.iFarmMapID;
	m_nCreateTick = timeGetTime();
	m_nFarmMaxUser = 0;
	m_bStartFarm = Farm.bStartActivate;
	m_Attr = Farm.Attr;
	m_nManagedID = 0;

	ResetData();
}
#else
CDNFarm::CDNFarm(UINT nFarmDBID, int nMapID, bool bStart)
{
	m_nFarmDBID = nFarmDBID;
	m_eFarmStatus = FARMSTATUS_NONE;
	m_nMapID = nMapID;
	m_nCreateTick = timeGetTime();
	m_nFarmMaxUser = 0;
	m_bStartFarm = bStart;

	ResetData();
}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

CDNFarm::~CDNFarm()
{
	m_vWaitingUser.clear();
}

void CDNFarm::DestroyFarm()
{
	if (m_eFarmStatus == FARMSTATUS_DESTROY)
		return;

	ResetData();
	m_eFarmStatus = FARMSTATUS_DESTROY;
}

bool CDNFarm::SetAssignedServerID(int nGameServerID)
{
	if (m_eFarmStatus != FARMSTATUS_NONE && m_eFarmStatus != FARMSTATUS_DESTROY)
	{
		g_Log.Log(LogType::_FARM, L"FarmID[%d] SetAssignedServerID Fail Reason[StateMisMatch]\n", GetFarmDBID());
		return false;
	}

	m_nAssignedGameID = nGameServerID;
	m_eFarmStatus = FARMSTATUS_READY;
	return true;
}

bool CDNFarm::SetAssignedFarmData(int nGameServerID, int nGameThreadIdx, UINT nRoomID, int nMapID, int nFarmMaxUser)
{
	//게임서버에서 로드끝나고 유저 받을 수 있다고 하는 타이밍이다.
	if (m_eFarmStatus != FARMSTATUS_READY)
	{
		g_Log.Log(LogType::_FARM, L"FarmID[%d] SetAssignedFarmData Fail Reason[StateMisMatch]\n", GetFarmDBID());
		return false;		//세팅은 최초 한번만 가능하다 이렇게 데면 잘못만든거임!
	}

	if ((m_nAssignedGameID != nGameServerID) || m_nMapID != nMapID)
	{
		//이건뭐야!!!!!!!!!!!!!
		g_Log.Log(LogType::_FARM, L"FarmID[%d] SetFarmDataLoaded Fail Reason[DataMisMatch]\n", GetFarmDBID());
		return false;
	}

	m_nAssignedThreadIdx = nGameThreadIdx;
	m_nAssignedRoomID = nRoomID;
	m_nMapID = nMapID;
	m_nPlayTick = timeGetTime();
	m_eFarmStatus = FARMSTATUS_PLAY;
	m_nFarmMaxUser = nFarmMaxUser;
	
	//플레이 상태가되면 대기중인 녀석들을 밀어 넣어준다.
	ProcessFarmWaiting();
	return true;
}
void CDNFarm::SetFarmCurUserCount(int nFarmCurUserCount, bool bStarted, int nManagedID)
{
	m_nFarmCurUser = nFarmCurUserCount;
	m_bStartFarm = bStarted;
	m_nManagedID = nManagedID;
}

bool CDNFarm::VerifyWaitAndPush(const VIMAReqGameID * pPacket)
{
	if (m_eFarmStatus == FARMSTATUS_READY || m_eFarmStatus == FARMSTATUS_NONE)
	{
		std::vector <VIMAReqGameID>::iterator ii;
		for (ii = m_vWaitingUser.begin(); ii != m_vWaitingUser.end(); ii++)
		{
			if ((*ii).InstanceID == pPacket->InstanceID)
			{
				_DANGER_POINT();	//이미 있다! 이럼 안데지!
				return false;
			}
		}

		VIMAReqGameID req;
		memcpy(&req, pPacket, sizeof(VIMAReqGameID));

		m_vWaitingUser.push_back(req);		//대기 유저들을 밀어 넣어주자.
		return true;
	}
	return false;
}

bool CDNFarm::GetAssignedServerInfo(int &nGameServerID, int &nGameServerThreaIdx, UINT &nGameRoomID)
{
	if (m_eFarmStatus == FARMSTATUS_NONE)			//생성만 되어진 상태에서는 룸정보가 없다.
		return false;

	nGameServerID = m_nAssignedGameID;
	nGameServerThreaIdx = m_nAssignedThreadIdx;
	nGameRoomID = m_nAssignedRoomID;
	return true;
}

void CDNFarm::ResetData()
{
	m_nPlayTick = 0;
	m_nAssignedGameID = -1;
	m_nAssignedThreadIdx = -1;
	m_nAssignedRoomID = 0;
	m_nFarmCurUser = 0;

	m_vWaitingUser.clear();
}

void CDNFarm::ProcessFarmWaiting()
{
	//방초기 또는 생성도중 진입요청이 있는 친구들을 밀어 넣어 준다. 일단은 그냥 밀어 넣지만 스테이트 보호를 위해
	//몇가지 스테이트 검사를 추가해야겠다.
	std::vector <VIMAReqGameID>::iterator ii;
	for (ii = m_vWaitingUser.begin(); ii != m_vWaitingUser.end(); ii++)
	{
		if (g_pDivisionManager->RequestGameRoom(&(*ii)) == false)
		{
			//진입실패시 처리
		}
	}
	m_vWaitingUser.clear();
}

