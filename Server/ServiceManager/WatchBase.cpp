
#include "Stdafx.h"
#include "WatcherBase.h"
#include "Log.h"

CWatcherBase::CWatcherBase()
{
}

CWatcherBase::~CWatcherBase()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	m_WatchList.clear();
}

bool CWatcherBase::AddWatchSpec(int nVersionID, int nNID, const WCHAR * pType)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	_WATCH_SPEC spec;
	std::list <_WATCH_OBJECT>::iterator ii;
	for (ii = m_WatchList.begin(); ii != m_WatchList.end(); ii++)
	{
		if (nVersionID == (*ii).nVersionID)
		{
			std::list <_WATCH_SPEC>::iterator ih;
			for (ih = (*ii).SpecList.begin(); ih != (*ii).SpecList.end(); ih++)
			{
				if (!wcsicmp((*ih).wszType, pType) && (*ih).nNID == nNID)
				{
					g_Log.Log(LogType::_FILELOG, L"AddWatchContent Add Fail\n");
					return false;
				}
			}

			memset(&spec, 0, sizeof(_WATCH_SPEC));

			spec.nNID = nNID;
			wcscpy_s(spec.wszType, pType);
			spec.nInsertTick = timeGetTime();

			(*ii).SpecList.push_back(spec);
			return true;
		}
	}

	_WATCH_OBJECT object;
	object.nVersionID = nVersionID;

	memset(&spec, 0, sizeof(_WATCH_SPEC));

	spec.nNID = nNID;
	wcscpy_s(spec.wszType, pType);
	spec.nInsertTick = timeGetTime();

	object.SpecList.push_back(spec);
	m_WatchList.push_back(object);
	return true;
}

bool CWatcherBase::DelWatchSpec(int nVersionID, int nNID, const WCHAR * pType, bool &bComplete)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::list <_WATCH_OBJECT>::iterator ii;
	for (ii = m_WatchList.begin(); ii != m_WatchList.end(); ii++)
	{
		if ((*ii).nVersionID == nVersionID)
		{
			std::list <_WATCH_SPEC>::iterator ih;
			for (ih = (*ii).SpecList.begin(); ih != (*ii).SpecList.end(); )
			{
				if ((*ih).nNID == nNID && !wcsicmp((*ih).wszType, pType))
				{
					ih = (*ii).SpecList.erase(ih);
				}
				else
					ih++;
			}

			if ((*ii).SpecList.size() <= 0)
			{
				m_WatchList.erase(ii);
				bComplete = true;
			}

			return true;
		}
	}
	return false;
}

void CWatcherBase::ClearWatchingSpec()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::list <_WATCH_OBJECT>::iterator ii;
	for (ii = m_WatchList.begin(); ii != m_WatchList.end(); ii++)
	{
		(*ii).SpecList.clear();
	}

	m_WatchList.clear();
	g_Log.Log(LogType::_NORMAL, L"All Watching Spec Deleted\n");
}

bool CWatcherBase::HasWatchingSpec()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (m_WatchList.size() > 0)
	{
		std::list <_WATCH_OBJECT>::iterator ii;
		for (ii = m_WatchList.begin(); ii != m_WatchList.end(); ii++)
		{
			std::list <_WATCH_SPEC>::iterator ih;
			for (ih = (*ii).SpecList.begin(); ih != (*ii).SpecList.end(); ih++)
			{
				g_Log.Log(LogType::_ERROR, L"Still Running [NID:%d][Type:%s]\n", (*ih).nNID, (*ih).wszType);
			}
		}
		return true;
	}
	return false;
}

void CWatcherBase::DelWatchSpec(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::list <_WATCH_OBJECT>::iterator ii;
	for (ii = m_WatchList.begin(); ii != m_WatchList.end();)
	{
		std::list <_WATCH_SPEC>::iterator ih;
		for (ih = (*ii).SpecList.begin(); ih != (*ii).SpecList.end(); )
		{
			if ((*ih).nNID == nNID)
			{
				ih = (*ii).SpecList.erase(ih);
			}
			else
				ih++;
		}

		if ((*ii).SpecList.size() <= 0)
			ii = m_WatchList.erase(ii);
		else
			ii++;
	}
}

