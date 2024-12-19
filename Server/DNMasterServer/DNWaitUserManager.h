
#pragma once

const int DISTRIBUTE_OUT_TABLE = 6;

struct _WAITUSER
{
	int nServerID;				//대기중으로 연결되어 있는 로그인서버의 아이디
	UINT nAccountDBID;			//대기하는 놈의 어카운트디비아이디
	ULONG nAddTimeTick;			//대기시작한 시간

	//need update
	USHORT nWaitTicketNum;		//티켓넘버 당신이 몇번째 선수인지

	_WAITUSER()
	{
		nServerID = 0;
		nAccountDBID = 0;
		nAddTimeTick = 0;
		nWaitTicketNum = 0;
	}
};

class CDNWaitUserManager
{
public:
	CDNWaitUserManager();
	~CDNWaitUserManager();

	bool Initialize(int nMaxuser);
	void UpdateCurCount(UINT nCurCount);

	bool IsWaitUser(UINT nAccountDBID);
	int AddWaitUser(int nServerID, UINT nAccountDBID, USHORT &nLeftTicketNum, USHORT &nLastEstimateCalcTime);
	void DelWaitUser(int nServerID, UINT nAccountDBID);

	//Calc Estimate Waiting Time
	UINT GetWaitUserCount();
	UINT GetWorldMaxUser() { return m_nWorldMaxUser; }
	USHORT CalcEstimateTime(UINT nLeftWaitUser, UINT nWorldUserCount);

private:
	CSyncLock m_Sync;
	std::list <_WAITUSER> m_WaitUserList;

	UINT m_nWorldMaxUser;
	UINT m_nWorldPreCurUser;
	UINT m_nWorldCurUser;

	void WaitUserProcessAsync(UINT nCurCnt);

	std::vector <std::pair<int, int>> m_OutList;		//first index, second outcount;
	void UpdateProcessIndexAsync(int nIndex, int nCount);
};

extern CDNWaitUserManager * g_pWaitUserManager;
