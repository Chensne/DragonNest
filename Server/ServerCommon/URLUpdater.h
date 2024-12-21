
#pragma once

#include "urlmon.h"
#include "CriticalSection.h"
#if defined(_LAUNCHER) || defined(_ESM)
#include "strdef.h"
#endif

template <class _Tx> struct less_str : std::binary_function <_Tx, _Tx, bool>
{
public:
	bool operator() (const _Tx &a, const _Tx &b) const { return strcmp(a, b) < 0 ? true : false; }
};

template <class _Tx>
class map_str : public std::map <const char*, _Tx, less_str <const char*> >
{
};

template <class _Tx> struct less_wstr : std::binary_function <_Tx, _Tx, bool>
{
public:
	bool operator() (const _Tx &a, const _Tx &b) const { return wcscmp(a, b) < 0 ? true : false; }
};

template <class _Tx>
class map_wstr : public std::map <const WCHAR*, _Tx, less_wstr <const WCHAR*> >
{
};


enum eNeedRes
{
	NEED_NONE = 0,
	NEED_GAMERES,
	NEED_VILLAGERES,
};

class CURLUpdater
{
public:
	CURLUpdater();
	~CURLUpdater();

	int GetPatchServerType(const WCHAR * pType);
	bool UpdaterInit(const WCHAR * pBaseURL, const WCHAR * pPatchURL, std_str * pString = NULL, int nTryCount = 10);
	bool UrlUpdate(std::vector <int> vList, const WCHAR * pKey, const WCHAR * pDestFolder, bool bIsWithFolder, bool bWithDestFolder = false, bool bBaseAccess = false, std_str * pstring = NULL, LPBINDSTATUSCALLBACK callback = NULL);
	bool UrlUpdate(const WCHAR * pKey, const WCHAR * pDestFolder, bool bIsWithFolder, bool bBaseAccess = false, std_str * pstring = NULL, LPBINDSTATUSCALLBACK callback = NULL);
	bool UrlUpdateEach(const WCHAR * pUrl, const WCHAR * pDest, bool bBaseAccess, LPBINDSTATUSCALLBACK callback = NULL);
	bool UrlUpdateUrl(const WCHAR * pUrl, const WCHAR * pDest, std_str * pString = NULL, int DefaultTryCount = 10, LPBINDSTATUSCALLBACK callback = NULL);

private:
	CSyncLock m_Sync;
	WCHAR m_szBaseURL[MAX_PATH];
	WCHAR m_szPatchURL[MAX_PATH];

	struct UpdateItem
	{
		WCHAR wszFolder[MAX_PATH];
		WCHAR wszFile[MAX_PATH];
		WCHAR wszWhole[MAX_PATH];
#ifdef _LAUNCHER
		char szExcutePath[512];
#endif
	};

	struct UpdateSubject
	{
		int nAssignedType;
		WCHAR wszAssignedType[32];
		std::vector <UpdateItem*> ItemList;
	};
	std::map <int, UpdateSubject*> m_UpdateSubject;
	map_wstr <UpdateSubject*> m_wUpdateSubject;

	bool URLDownLoad(const WCHAR * pURL, const WCHAR * pDest, bool bBaseAccess = false, std_str * pString = NULL, int DefaultTryCount = 2, LPBINDSTATUSCALLBACK callback = NULL);
	bool LoadPatchList();
	void ClearPatchList();

	int GetNeedResType(std::vector <int> * pList);

	//File Func
	WCHAR * FileLoad(const WCHAR * pFileName);
	void GetFile(const WCHAR * pPath, WCHAR * pFolder, WCHAR * pFile);

	const TCHAR* GetErrorMessage(HRESULT hResult);
};