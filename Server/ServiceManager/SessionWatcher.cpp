
#include "Stdafx.h"
#include "SessionWatcher.h"

CSessionWatcher::CSessionWatcher()
{
	m_nLastAddingTime = 0;
}

CSessionWatcher::~CSessionWatcher()
{
	m_WatchingList.clear();
}

bool CSessionWatcher::CheckWatchingItems()
{
	ULONG nCurTick = timeGetTime();
	return false;
}

bool CSessionWatcher::AddWatchingItem()
{
	ULONG nCurTick = timeGetTime();

	m_WatchingList.push_back(nCurTick);

	m_nLastAddingTime = nCurTick;
	return false;
}

bool CSessionWatcher::DelWatchingItem()
{
	if (m_WatchingList.empty())
		return false;

	m_WatchingList.pop_front();
	return true;
}

