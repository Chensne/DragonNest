
#pragma once

class CSessionWatcher
{
public:
	CSessionWatcher();
	~CSessionWatcher();

	bool CheckWatchingItems();
	bool AddWatchingItem();
	bool DelWatchingItem();
	
private:
	std::list <ULONG> m_WatchingList;
	ULONG m_nLastAddingTime;
};