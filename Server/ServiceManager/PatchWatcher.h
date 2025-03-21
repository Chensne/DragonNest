

#pragma once

struct _CONTENT
{
	int nNID;
	WCHAR wszType[64];
	ULONG nInsertTick;
};

struct _PATCH
{
	int nPatchId;
	std::list <_CONTENT> ContentList;
};

class CPatchWatcher
{
public:
	CPatchWatcher();
	~CPatchWatcher();

	bool AddWatchContent(int nPatchId, int nNID, const WCHAR * pType);
#if defined (_SERVICEMANAGER_EX)
	bool DelWatchContent(int nPatchId, int nNID, const WCHAR * pType, bool &bComplete, bool &bNIDComplete);
#else
	bool DelWatchContent(int nPatchId, int nNID, const WCHAR * pType, bool &bComplete);
#endif
	bool HasWatchingContent();

	void DelWatchContent(int nNID);
	void ClearWatchingContent();

private:
	std::list <_PATCH> m_PatchList;
	CSyncLock m_Sync;
};