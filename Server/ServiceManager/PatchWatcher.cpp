
#include "Stdafx.h"
#include "PatchWatcher.h"
#include "Log.h"

CPatchWatcher::CPatchWatcher()
{
}

CPatchWatcher::~CPatchWatcher()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	m_PatchList.clear();
}

bool CPatchWatcher::AddWatchContent(int nPatchId, int nNID, const WCHAR * pType)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	_CONTENT content;
	std::list <_PATCH>::iterator ii;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
	{
		if (nPatchId == (*ii).nPatchId)
		{
			std::list <_CONTENT>::iterator ih;
			for (ih = (*ii).ContentList.begin(); ih != (*ii).ContentList.end(); ih++)
			{
				if (!wcsicmp((*ih).wszType, pType) && (*ih).nNID == nNID)
				{
					g_Log.Log(LogType::_FILELOG, L"AddWatchContent Add Fail\n");
					return false;
				}
			}

			memset(&content, 0, sizeof(_CONTENT));

			content.nNID = nNID;
			wcscpy_s(content.wszType, pType);
			content.nInsertTick = timeGetTime();

			(*ii).ContentList.push_back(content);
			return true;
		}
	}

	_PATCH patch;
	patch.nPatchId = nPatchId;

	memset(&content, 0, sizeof(_CONTENT));

	content.nNID = nNID;
	wcscpy_s(content.wszType, pType);
	content.nInsertTick = timeGetTime();

	patch.ContentList.push_back(content);
	m_PatchList.push_back(patch);
	return true;
}

#if defined(_SERVICEMANAGER_EX)
bool CPatchWatcher::DelWatchContent(int nPatchId, int nNID, const WCHAR * pType, bool &bComplete, bool &bNIDComplete)
#else
bool CPatchWatcher::DelWatchContent(int nPatchId, int nNID, const WCHAR * pType, bool &bComplete)
#endif
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::list <_PATCH>::iterator ii;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
	{
		if ((*ii).nPatchId == nPatchId)
		{
#if defined(_SERVICEMANAGER_EX)
			bNIDComplete = true;
#endif
			std::list <_CONTENT>::iterator ih;
			for (ih = (*ii).ContentList.begin(); ih != (*ii).ContentList.end(); )
			{
				if ((*ih).nNID == nNID && !wcsicmp((*ih).wszType, pType))
				{
					ih = (*ii).ContentList.erase(ih);
				}
#if defined(_SERVICEMANAGER_EX)
				else if((*ih).nNID == nNID)
				{
					bNIDComplete = false;
					ih++;
				}
#endif
				else
					ih++;
			}

			if ((*ii).ContentList.size() <= 0)
			{
				m_PatchList.erase(ii);
				bComplete = true;
			}

			return true;
		}
	}
	return false;
}

void CPatchWatcher::ClearWatchingContent()
{
	ScopeLock <CSyncLock> sync(m_Sync);
	std::list <_PATCH>::iterator ii;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
	{
		(*ii).ContentList.clear();
	}
	
	m_PatchList.clear();
	g_Log.Log(LogType::_NORMAL, L"All Patch Watching Content Deleted\n");
}

bool CPatchWatcher::HasWatchingContent()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	if (m_PatchList.size() > 0)
	{
		std::list <_PATCH>::iterator ii;
		for (ii = m_PatchList.begin(); ii != m_PatchList.end(); ii++)
		{
			std::list <_CONTENT>::iterator ih;
			for (ih = (*ii).ContentList.begin(); ih != (*ii).ContentList.end(); ih++)
			{
				g_Log.Log(LogType::_ERROR, L"Patching [NID:%d][Type:%s]\n", (*ih).nNID, (*ih).wszType);
			}
		}
		return true;
	}
	return false;
}

void CPatchWatcher::DelWatchContent(int nNID)
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::list <_PATCH>::iterator ii;
	for (ii = m_PatchList.begin(); ii != m_PatchList.end();)
	{
		std::list <_CONTENT>::iterator ih;
		for (ih = (*ii).ContentList.begin(); ih != (*ii).ContentList.end(); )
		{
			if ((*ih).nNID == nNID)
			{
				ih = (*ii).ContentList.erase(ih);
			}
			else
				ih++;
		}

		if ((*ii).ContentList.size() <= 0)
			ii = m_PatchList.erase(ii);
		else
			ii++;
	}
}

