

#pragma once

#include "DNDivisionManager.h"


enum eFarmStatus
{
	FARMSTATUS_NONE,	//������ü�� ������ �Ǿ�����
	FARMSTATUS_READY,	//���Ӽ����� �Ҵ� ��û�ѻ���
	FARMSTATUS_PLAY,	//��밡���ѻ��� (��û�� ���Ӽ������� ���尡�� ��û������)
	FARMSTATUS_DESTROY,	//������ ���������� �𸣰�����
};

class CDNFarm
{
public:

#if defined( PRE_ADD_FARM_DOWNSCALE )
	CDNFarm( const TFarmItemFromDB& Farm );
#elif defined( PRE_ADD_VIP_FARM )
	CDNFarm( const TFarmItemFromDB& Farm );
#else
	CDNFarm(UINT nFarmDBID, int nMapID, bool bStart);
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

	~CDNFarm();

	//Operate
	void DestroyFarm();

	//FarmData
	bool SetAssignedServerID(int nGameServerID);
	bool SetAssignedFarmData(int nGameServerID, int nGameThreadIdx, UINT nRoomID, int nMapID, int nFarmMaxUser);
	void SetFarmCurUserCount(int nFarmCurUserCount, bool bStarted, int nManagedID);

	//FarmUser
	bool VerifyWaitAndPush(const VIMAReqGameID * pPacket);

	//Get
	bool GetAssignedServerInfo(int &nGameServerID, int &nGameServerThreaIdx, UINT &nGameRoomID);
	UINT GetFarmDBID() { return m_nFarmDBID; }
	int GetMapID() { return m_nMapID; }
	int GetFarmCurUserCount() { return m_nFarmCurUser; }
	bool IsSaturation() { return m_nFarmMaxUser <= m_nFarmCurUser ? true : false; }
	int GetAssignedServerID() { return m_nAssignedGameID; }
	int GetFarmMaxUser() { return m_nFarmMaxUser; }
	int GetManagedID () {return m_nManagedID;}

	eFarmStatus GetFarmStatus() { return m_eFarmStatus; }
	bool IsActivateFarm() { return m_eFarmStatus == FARMSTATUS_PLAY ? true : false; }
	bool IsStartedFarm() { return m_bStartFarm; }
	ULONG GetCreateTick() { return m_nCreateTick; }
	ULONG GetPlayTick() { return m_nPlayTick; }
	bool GetFarmStart() { return m_bStartFarm; }
#if defined( PRE_ADD_FARM_DOWNSCALE )
	int GetAttr(){ return m_iAttr; }
#elif defined( PRE_ADD_VIP_FARM )
	Farm::Attr::eType GetAttr(){ return m_Attr; }
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

protected:
	eFarmStatus m_eFarmStatus;
	ULONG m_nCreateTick;
	ULONG m_nPlayTick;

	std::vector <VIMAReqGameID> m_vWaitingUser;	//���������� ���� ��������� �ʿ��Ұ��(���尴ü�� ���Ӽ����� �����Ǳ����� ���ʴ�� �߰��� �ٸ� ������ ���Կ�û�� ������� ��⿡ ��)

private:
	UINT m_nFarmDBID;
	int m_nAssignedGameID;
	int m_nAssignedThreadIdx;
	UINT m_nAssignedRoomID;
	int m_nMapID;
	int m_nFarmMaxUser;
	int m_nFarmCurUser;
	bool m_bStartFarm;
	int m_nManagedID;
#if defined( PRE_ADD_FARM_DOWNSCALE )
	int m_iAttr;
#elif defined( PRE_ADD_VIP_FARM )
	Farm::Attr::eType m_Attr;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )


	void ResetData();
	void ProcessFarmWaiting();
};

