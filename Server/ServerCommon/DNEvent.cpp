#include "Stdafx.h"
#include "DNEvent.h"

CDNEvent * g_pEvent = NULL;

CDNEvent::CDNEvent()
{
}

CDNEvent::~CDNEvent()
{
	m_Sync.Lock();

	std::multimap <int, TEventListInfo*>::iterator ii;
	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); )
	{
		TEventListInfo * pDel = (*ii).second;
		SAFE_DELETE(pDel);
		ii = m_WorldEventList.erase(ii);
	}
	m_WorldEventList.clear();

	m_Sync.UnLock();
}

void CDNEvent::InitWorld(char cWorldID)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	std::vector <int>::iterator ii;
	for (ii = m_WorldInit.begin(); ii != m_WorldInit.end(); ii++)
	{
		if ((*ii) == cWorldID)
		{
			m_WorldInit.erase(ii);
			return;
		}
	}	
}

bool CDNEvent::IsInitWorld(char cWorldID)
{
	m_Sync.Lock();

	std::vector <int>::iterator ii;
	for (ii = m_WorldInit.begin(); ii != m_WorldInit.end(); ii++)
	{
		if ((*ii) == cWorldID)
		{
			m_Sync.UnLock();
			return true;
		}
	}	

	m_Sync.UnLock();
	return false;
}

void CDNEvent::AddWorldEvent(char cWorld, int nCount, TEventInfo * pInfoList)
{
	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);	

	std::multimap <int, TEventListInfo*>::iterator ii;
	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); )
	{
		TEventListInfo * pDel = (*ii).second;
		SAFE_DELETE(pDel);
		ii = m_WorldEventList.erase(ii);
	}
	m_WorldEventList.clear();

	TEventListInfo * pInfo;
	bool bAdd;
	for (int i = 0; i < nCount; i++)
	{
		if (Time > pInfoList[i]._tEndTime) continue;		//Event was expired
	
		pInfo = NULL;
		bAdd = false;
		for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
		{
			if ((*ii).second->nEventID == pInfoList[i].EventID)
			{
				pInfo = (*ii).second;
				break;
			}
		}

		if (pInfo == NULL)
		{
			pInfo = new TEventListInfo;
			bAdd = true;
		}

		pInfo->nEventID = pInfoList[i].EventID;
		pInfo->nWorldID = pInfoList[i].WroldID;

		pInfo->_tBeginTime = pInfoList[i]._tBeginTime;
		pInfo->_tEndTime = pInfoList[i]._tEndTime;

#if defined(PRE_ADD_WORLD_EVENT)
#else
		GetApplyMapIdx(&pInfo->vApplyMapIdxList, pInfoList[i].wszMapIndex);
#endif //#if defined(PRE_ADD_WORLD_EVENT)

		memcpy(pInfo->nEventType, pInfoList[i].EventType, sizeof(pInfo->nEventType));
		memcpy(pInfo->nAtt, pInfoList[i].Att, sizeof(pInfo->nAtt));

		if (bAdd)
			m_WorldEventList.insert(std::make_pair(pInfo->nWorldID, pInfo));
	}

	std::vector <int>::iterator iw;
	iw = std::find(m_WorldInit.begin(), m_WorldInit.end(), cWorld);
	if (m_WorldInit.end() == iw)
		m_WorldInit.push_back(cWorld);
}

void CDNEvent::GetEvent(char cWorldID, int nMapIdx, DNVector(TEvent) * pvList)
{	
	TEvent _event;
	memset(&_event, 0, sizeof(TEvent));
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	m_Sync.Lock();

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		//기간이 시작전이거나 끝난 이벤트가 아니라면
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
#if defined(PRE_ADD_WORLD_EVENT)
		_event.nEventID = (*ii).second->nEventID;
		_event._tBeginTime = (*ii).second->_tBeginTime;
		_event._tEndTime = (*ii).second->_tEndTime;
		memcpy(_event.nEventType, (*ii).second->nEventType, sizeof(_event.nEventType));
		memcpy(_event.nAtt, (*ii).second->nAtt, sizeof(_event.nAtt));
		pvList->push_back(_event);
#else
		for (int i = 0; i < (int)(*ii).second->vApplyMapIdxList.size(); i++)
		{
			if ((*ii).second->vApplyMapIdxList[i] == nMapIdx || (*ii).second->vApplyMapIdxList[i] == 0)
			{
				_event.nEventID = (*ii).second->nEventID;
				_event._tBeginTime = (*ii).second->_tBeginTime;
				_event._tEndTime = (*ii).second->_tEndTime;
				memcpy(_event.nEventType, (*ii).second->nEventType, sizeof(_event.nEventType));
				memcpy(_event.nAtt, (*ii).second->nAtt, sizeof(_event.nAtt));

				pvList->push_back(_event);
				break;
			}
		}
#endif //#if defined(PRE_ADD_WORLD_EVENT)
	}

	m_Sync.UnLock();
}

void CDNEvent::GetEventByType(char cWorldID, int nMapIdx, int nType, DNVector(TEvent) * pvList)
{
	DNVector(TEvent) vTemp;
	GetEvent( cWorldID, nMapIdx, &vTemp );

	for( UINT i=0 ; i<vTemp.size() ; ++i )
	{
		if( vTemp[i].nEventType1 == nType )
			pvList->push_back( vTemp[i] );
	}
}

#if defined(PRE_ADD_WORLD_EVENT)
const TEventListInfo* CDNEvent::GetEventByType(char cWorldID, int nType, char cClassID)
{
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time >= (*ii).second->_tBeginTime && Time <= (*ii).second->_tEndTime)
		{
			if ((*ii).second->nEventType2 == nType)
			{
				if( (*ii).second->nEventType1 == 0 || (*ii).second->nEventType1 == cClassID )
					return (*ii).second;
			}
		}
	}
	return NULL;
}
#endif //#if defined(PRE_ADD_WORLD_EVENT)

#if defined(PRE_ADD_WORLD_EVENT)
int CDNEvent::GetExtendFatigue(char cClassID)
{
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time >= (*ii).second->_tBeginTime && Time <= (*ii).second->_tEndTime)
		{
			if ((*ii).second->nEventType2 == WorldEvent::EVENT6 )
			{
				if( (*ii).second->nEventType1 == 0 || (*ii).second->nEventType1 == cClassID )
					return (*ii).second->nAtt1;
			}				
		}
	}
	return 0;
}
#else //#if defined(PRE_ADD_WORLD_EVENT)
int CDNEvent::GetExtendFatigue()
{
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time >= (*ii).second->_tBeginTime && Time <= (*ii).second->_tEndTime)
		{
			if ((*ii).second->nEventType1 == _EVENT_1_FATIGUE && (*ii).second->nEventType2 == _EVENT_2_EXTEND_FATIGUE)
				return (*ii).second->nAtt1;
		}
	}
	return 0;
}

bool CDNEvent::GetEventQuestList(char cWorldID, std::vector <int> &vList)
{
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType1 == _EVENT_1_QUEST && (*ii).second->nEventType2 == _EVENT_2_NONE && \
			(*ii).second->nEventType3 == _EVENT_3_NONE)
		{
			vList.push_back((*ii).second->nAtt1);
			return true;
		}
	}
	return false;
}

bool CDNEvent::GetPeriodQuestList (char cWorldID, std::vector <TEventListInfo> &vList)
{
	TEventListInfo _event;
	memset(&_event, 0, sizeof(TEventListInfo));
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType1 == _EVENT_1_QUEST && (*ii).second->nEventType2 == _EVENT_2_NONE && \
			(*ii).second->nEventType3 == _EVENT_3_NONE)
		{
			_event.nEventID		= (*ii).second->nEventID;
			_event.nWorldID		= (*ii).second->nWorldID;
			_event._tBeginTime	= (*ii).second->_tBeginTime;
			_event._tEndTime	= (*ii).second->_tEndTime;
			memcpy(_event.nEventType, (*ii).second->nEventType, sizeof(_event.nEventType));
			memcpy(_event.nAtt, (*ii).second->nAtt, sizeof(_event.nAtt));
			vList.push_back(_event);
			return true;
		}
	}
	return false;
}
#endif //#if defined(PRE_ADD_WORLD_EVENT)

bool CDNEvent::GetUpkeepBonus(char cWorldID, int &nRateOffSet, int &nMaximumRate)
{
#if defined(PRE_ADD_WORLD_EVENT)
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType2 == WorldEvent::EVENT9 )
		{
			nRateOffSet = (*ii).second->nAtt1;
			nMaximumRate = (*ii).second->nAtt2;
			return true;
		}
	}
	return false;
#else
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType1 == _EVENT_1_EXP && (*ii).second->nEventType2 == _EVENT_2_MONSTERDIE && \
			(*ii).second->nEventType3 == _EVENT_3_UPKEEPPARTY)
		{
			nRateOffSet = (*ii).second->nAtt1;
			nMaximumRate = (*ii).second->nAtt2;
			return true;
		}
	}
	return false;
#endif
}

bool CDNEvent::GetFriendPartyBonus(char cWorldID, int &nRate)
{
#if defined(PRE_ADD_WORLD_EVENT)
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType2 == WorldEvent::EVENT10 )			
		{
			nRate = (*ii).second->nAtt1;
			return true;
		}
	}
	return false;	
#else
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType1 == _EVENT_1_EXP && (*ii).second->nEventType2 == _EVENT_2_MONSTERDIE && \
			(*ii).second->nEventType3 == _EVENT_3_FRIENDPARTY)
		{
			nRate = (*ii).second->nAtt1;
			return true;
		}
	}
	return false;
#endif //#if defined(PRE_ADD_WORLD_EVENT)
}

bool CDNEvent::IsExistAsync(int nWorldID, int nEventID)
{
	std::multimap <int, TEventListInfo*>::iterator ii;
	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if ((*ii).second->nWorldID == nWorldID && (*ii).second->nEventID == nEventID)
			return true;
	}
	return false;
}

void CDNEvent::GetApplyMapIdx(std::vector <int> * pvList, const WCHAR * pMapIdx)
{
	std::vector<std::wstring> tokens;
	TokenizeW(pMapIdx, tokens, L",");

	for (int i = 0; i < (int)tokens.size(); i++)
	{
		std::vector<std::wstring> tokens2;
		TokenizeW(tokens[i].c_str(), tokens2, L"-");

		if (tokens2.size() > 1)
		{
			int nBegin, nEnd;
			nBegin = _wtoi(tokens2[0].c_str());
			nEnd = _wtoi(tokens2[1].c_str());

			for (int j = nBegin; j <= nEnd; j++)
				pvList->push_back(j);
		}
		else
			pvList->push_back(_wtoi(tokens[i].c_str()));
	}
}

#if defined(PRE_ADD_WORLD_EVENT)
#else
#if defined(PRE_ADD_REBIRTH_EVENT)
bool CDNEvent::IsRebirthEvent(char cWorldID, int nMapID)
{
	std::multimap <int, TEventListInfo*>::iterator ii;

	time_t Time;
	time(&Time);

	ScopeLock <CSyncLock> Lock(m_Sync);

	for (ii = m_WorldEventList.begin(); ii != m_WorldEventList.end(); ii++)
	{
		if (Time < (*ii).second->_tBeginTime || Time > (*ii).second->_tEndTime) continue;
		if ((*ii).second->nWorldID != cWorldID && (*ii).second->nWorldID != 0) continue;
		if ((*ii).second->nEventType1 == _EVENT_1_REBIRTH && 
			((*ii).second->nAtt1 == nMapID || (*ii).second->nAtt2 == nMapID || (*ii).second->nAtt3 == nMapID || (*ii).second->nAtt4 == nMapID || (*ii).second->nAtt5 == nMapID))
			return true;		
	}
	return false;
}
#endif
#endif //#if defined(PRE_ADD_WORLD_EVENT)
