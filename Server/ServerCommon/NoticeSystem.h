
#pragma once
//일정 영역에 일정시간동안 진입해있거나 진입하는 유저들에게 알려야 할 것이 있을 경우

struct TNoticeInfo
{
	TNoticeTypeInfo TypeInfo;	//apply region
	WCHAR wszMsg[CHATLENMAX];	//message
	ULONG nCreateTime;
	ULONG nDestroyTime;
};

class CDNNoticeSystem
{
public:
	CDNNoticeSystem();
	~CDNNoticeSystem();

	bool AddNotice(const TNoticeTypeInfo * pInfo, const WCHAR * pNoticeMsg);
	bool CancelNotice();

	bool GetNotice(int nChannelID, int nMapIdx, TNoticeInfo & Info);

protected:
	CSyncLock m_Sync;
	std::list <TNoticeInfo> m_pNoticeInfoList;

	bool CheckNotice(const TNoticeTypeInfo * pInfo);
};

extern CDNNoticeSystem * g_pNoticeSystem;