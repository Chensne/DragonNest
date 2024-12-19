
#pragma once

struct TEventListInfo: public TBoostMemoryPool<TEventListInfo>
{
	int nEventID;
	int nWorldID;

	time_t _tBeginTime;
	time_t _tEndTime;

#if defined(PRE_ADD_WORLD_EVENT)
#else
	std::vector <int> vApplyMapIdxList;
#endif //#if defined(PRE_ADD_WORLD_EVENT)
	
	union 
	{
		struct
		{
			int nEventType1;
			int nEventType2;
			int nEventType3;
		};
		int nEventType[3];
	};

	union
	{
		struct
		{
			int nAtt1;
			int nAtt2;
			int nAtt3;
			int nAtt4;
			int nAtt5;
		};
		int nAtt[5];
	};
};

class CDNEvent
{
public:
	CDNEvent();
	~CDNEvent();

	void InitWorld(char cWorldID);
	bool IsInitWorld(char cWorldID);
	void AddWorldEvent(char cWorld, int nCount, TEventInfo * pInfoList);

	void GetEvent(char cWorldID, int nMapIdx, DNVector(TEvent) * pvList);
	void GetEventByType(char cWorldID, int nMapIdx, int nType, DNVector(TEvent) * pvList);
#if defined(PRE_ADD_WORLD_EVENT)
	int GetExtendFatigue(char cClassID);
#else
	bool GetEventQuestList(char cWorldID, std::vector <int> &vList);
	bool GetPeriodQuestList (char cWorldID, std::vector <TEventListInfo> &vList);
	int GetExtendFatigue();
#endif
	bool GetUpkeepBonus(char cWorldID, int &nRateOffSet, int &nMaximumRate);
	bool GetFriendPartyBonus(char cWorldID, int &nRate);	
#if defined(PRE_ADD_WORLD_EVENT)
#else
#if defined(PRE_ADD_REBIRTH_EVENT)
	bool IsRebirthEvent(char cWorldID, int nMapID);
#endif //#if defined(PRE_ADD_REBIRTH_EVENT)
#endif //#if defined(PRE_ADD_WORLD_EVENT)
#if defined(PRE_ADD_WORLD_EVENT)
	const TEventListInfo* GetEventByType(char cWorldID, int nType, char cClassID);
#endif //#if defined(PRE_ADD_WORLD_EVENT)

private:
	std::multimap <int, TEventListInfo*> m_WorldEventList;
	std::vector <int> m_WorldInit;
	CSyncLock m_Sync;

	bool IsExistAsync(int nWorldID, int nEventID);
	void GetApplyMapIdx(std::vector <int> * pvList, const WCHAR * pMapIdx);
};

extern CDNEvent * g_pEvent;