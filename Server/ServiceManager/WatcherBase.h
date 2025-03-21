

#pragma once

// PatchWatcher를 그대로 베이스클래스로 만들어서 추후에 확장하여 사용할수 있도록 한다.

struct _WATCH_SPEC
{
	int nNID;
	WCHAR wszType[64];
	ULONG nInsertTick;
};

struct _WATCH_OBJECT
{
	int nVersionID;
	std::list <_WATCH_SPEC> SpecList;
};

class CWatcherBase
{
public:
	CWatcherBase();
	~CWatcherBase();

	virtual bool AddWatchSpec(int nVersionID, int nNID, const WCHAR * pType);
	virtual bool DelWatchSpec(int nVersionID, int nNID, const WCHAR * pType, bool &bComplete);
	virtual bool HasWatchingSpec();

	virtual void DelWatchSpec(int nNID);
	virtual void ClearWatchingSpec();

protected:
	std::list <_WATCH_OBJECT> m_WatchList;
	CSyncLock m_Sync;
};