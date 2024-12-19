
#include "stdafx.h"
#include "Wininet.h"
#include "URLUpdater.h"
#include "IniFile.h"
#include "Log.h"
#include "XMLParser.h"
#include "comdef.h"
#ifdef _LAUNCHER
#include "ProcessManager.h"
#include "ServiceUtil.h"
#include "HttpClient.h"
#endif

#if defined(_LAUNCHER)
//#define PRE_FIX_DOWNLOADFILE // Http �ٿ�ε� �ͽ��÷ξ� ������ ����..
#endif //#if defined(_LAUNCHER)

#if defined (_SERVICEMANAGER_EX)
extern CLog g_Log;
#endif

CURLUpdater::CURLUpdater()
{
	memset(m_szBaseURL, 0, sizeof(m_szBaseURL));
	memset(m_szPatchURL, 0, sizeof(m_szPatchURL));
}

CURLUpdater::~CURLUpdater()
{
	ClearPatchList();
}

int CURLUpdater::GetPatchServerType(const WCHAR * pType)
{
	if (!wcsicmp(pType, L"db"))
		return PATCHTYPE_DB;
	else if (!wcsicmp(pType, L"log"))
		return PATCHTYPE_LOG;
	else if (!wcsicmp(pType, L"login"))
		return PATCHTYPE_LOGIN;
	else if (!wcsicmp(pType, L"master"))
		return PATCHTYPE_MASTER;
	else if (!wcsicmp(pType, L"village"))
		return PATCHTYPE_VILLAGE;
	else if (!wcsicmp(pType, L"game"))
		return PATCHTYPE_GAME;
	else if (!wcsicmp(pType, L"patcher"))
		return PATCHTYPE_PATCHER;
	else if (!wcsicmp(pType, L"cash"))
		return PATCHTYPE_CASH;
	else
		_ASSERT_EXPR(0, L"[GetPatchServerType] invalid type" );
	return -1;
}

bool CURLUpdater::UpdaterInit(const WCHAR * pBaseURL, const WCHAR * pPatchURL, std_str * pString, int nTryCount)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	wcscpy_s(m_szBaseURL, pBaseURL);
	wcscpy_s(m_szPatchURL, pPatchURL);
	
	if (_waccess(L"PatchList.xml", 0) != -1)
		DeleteFileW(L"PatchList.xml");
			
	if (URLDownLoad(L"PatchList.xml", L"PatchList.xml", false, pString, nTryCount))
	{//�����о �α׸� ����!
		if (LoadPatchList())
			return true;
	}	
	return false;
}

#if defined(_LAUNCHER)
extern void TextOut(const TCHAR * format, ...);
#endif

bool CURLUpdater::UrlUpdate(std::vector <int> vList, const WCHAR * pKey, const WCHAR * pDestFolder, bool bIsWithFolder, bool bWithDestFolder, bool bBaseAccess, std_str * pstring, LPBINDSTATUSCALLBACK callback)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
		
	int nType = GetNeedResType(&vList);
	for (int i = 0; i < (int)vList.size(); i++)
	{
		std::map <int, UpdateSubject*>::iterator ii = m_UpdateSubject.find(vList[i]);
		if (ii != m_UpdateSubject.end())
		{
			std::vector <UpdateItem*>::iterator ih;
			for (ih = (*ii).second->ItemList.begin(); ih != (*ii).second->ItemList.end(); ih++)
			{
				if (wcsicmp(pKey, (*ih)->wszFolder)) continue;
				if (!wcsicmp(pKey, L"res"))
				{
					WCHAR wszName[MAX_PATH];
					memset(wszName, 0, sizeof(wszName));
					switch (nType)
					{
					case NEED_NONE: break;
					case NEED_VILLAGERES:
						{
							if (!wcsicmp((*ih)->wszFile, L"villageres.zip"))
								wcscpy_s(wszName, ((*ih)->wszFile));
							break;
						}
					case NEED_GAMERES: 
						{
							if (!wcsicmp((*ih)->wszFile, L"gameres.zip"))
								wcscpy_s(wszName, ((*ih)->wszFile));
							break;
						}
					}

					if (wcslen(wszName) <= 0) continue;
				}

				WCHAR szDestPath[256];
				if (bIsWithFolder)
					swprintf(szDestPath, L"%s/%s", pDestFolder, (*ih)->wszWhole);
				else
					swprintf(szDestPath, L"%s/%s", pDestFolder, (*ih)->wszFile);

				for (int h = 0; h < 256 && szDestPath[h] != NULL && szDestPath[h] != '\0'; h++)
				{
					if (szDestPath[h] == '\\')
						szDestPath[h] = '/';
				}

				if (bWithDestFolder == false)
				{
					std::vector<std::wstring> tokens;
					TokenizeW((*ih)->wszFile, tokens, L"/");
					if (tokens.size() > 1)
					{
						if (!wcsstr(tokens[0].c_str(), L"x86"))
						{
							swprintf(szDestPath, L"%s/%s/%s", pDestFolder, (*ih)->wszFolder, tokens[tokens.size() - 1].c_str());
						}
						else
						{
							FolderCheckAndCreate(szDestPath);
						}
					}
				}

				if (_waccess(szDestPath, 0) != -1)
				{
					if (DeleteFileW(szDestPath) == 0)
					{
#if defined(_LAUNCHER)
						TextOut(_T("Delete Error Name[%S]"), (*ih)->wszFile);
#endif
						return false;
					}
				}

				int tryCount = 2;
				if (!wcsicmp(pKey, L"res"))
				{
					if (URLDownLoad((*ih)->wszWhole, szDestPath, bBaseAccess, pstring, tryCount, callback) == false)
					{
#if defined(_LAUNCHER)
						TextOut(_T("URLDown Error Name[%S]"), (*ih)->wszFile);
#endif
						return false;
					}
					else
					{
						return true;
					}
				}
				else
				{
					if (URLDownLoad((*ih)->wszWhole, szDestPath, bBaseAccess, pstring, tryCount, callback) == false)
					{
#if defined(_LAUNCHER)
						TextOut(_T("URLDown Error Name[%S]"), (*ih)->wszFile);
#endif
						return false;
					}
				}
			}
		}
	}
	return true;
}

bool CURLUpdater::UrlUpdate(const WCHAR * pKey, const WCHAR * pDestFolder, bool bIsWithFolder, bool bBaseAccess, std_str * pstring, LPBINDSTATUSCALLBACK callback)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	map_wstr <UpdateSubject*>::iterator ii = m_wUpdateSubject.find(pKey);
	if (ii != m_wUpdateSubject.end())
	{
		std::vector <UpdateItem*>::iterator ih;
		for (ih = (*ii).second->ItemList.begin(); ih != (*ii).second->ItemList.end(); ih++)
		{
			if (!wcsicmp(pKey, L"Config") || !wcsicmp(pKey, L"exe") || !wcsicmp(pKey, L"res") || !wcsicmp(pKey, L"info") || !wcsicmp(pKey, L"system"))
				if (wcsicmp(pKey, (*ih)->wszFolder)) continue;

			bool bFolder = bIsWithFolder;
			if (!wcsicmp(pKey, L"Patcher"))
			{
				if (wcsicmp((*ih)->wszFolder, L"exe"))
					bFolder = true;
			}

			WCHAR szDestPath[256];
			if (bFolder)
				swprintf(szDestPath, L"%s/%s", pDestFolder, (*ih)->wszWhole);
			else
				swprintf(szDestPath, L"%s/%s", pDestFolder, (*ih)->wszFile);

			if (_waccess(szDestPath, 0) != -1)
			{
				if (DeleteFileW(szDestPath) == 0)
					return false;
			}

			int tryCount = 2;
			if (URLDownLoad((*ih)->wszWhole, szDestPath, bBaseAccess, pstring, tryCount, callback) == false)
				return false;
		}
	}
	return true;
}

bool CURLUpdater::UrlUpdateEach(const WCHAR * pUrl, const WCHAR * pDest, bool bBaseAccess, LPBINDSTATUSCALLBACK callback)
{
	ScopeLock<CSyncLock> Lock(m_Sync);
	int tryCount = 2;
	return URLDownLoad(pUrl, pDest, bBaseAccess, NULL, tryCount, callback);
}

bool CURLUpdater::UrlUpdateUrl(const WCHAR * pUrl, const WCHAR * pDest, std_str * pString, int DefaultTryCount, LPBINDSTATUSCALLBACK callback)
{
	ScopeLock<CSyncLock> Lock(m_Sync);

	bool bFlag = false;
	for (int nCount = 0; nCount < DefaultTryCount && bFlag == false; nCount++)
	{
		HRESULT hRet = URLDownloadToFileW(NULL, pUrl, pDest, 0, callback);		

		bFlag = hRet == S_OK ? true : false;
		if (hRet != S_OK)
		{
			//_com_error err = (hRet);
			//std_str strerr = err.ErrorMessage();
			std_str strerr = GetErrorMessage(hRet);			

			if (pString)
			{
				*pString = _T("[Err:");
				*pString += strerr;
				*pString += _T("] [Url:");
	#ifndef _UNICODE
				std_str tempstr;
				ToMultiString(const_cast<WCHAR*>(pUrl), tempstr);
				*pString += tempstr;
	#else
				*pString += pUrl;
	#endif
				*pString += _T("]");
			}

	#if defined(_LAUNCHER)
			TextOut("URLDownLoadToFile [Err:%s][Url:%S]", strerr.c_str(), pUrl);
	#else
			printf("URLDownLoadToFile [Err:%s][Url:%S][Dest:%S]", strerr.c_str(), pUrl, pDest);
	#endif
		}
		DeleteUrlCacheEntryW(pUrl);
		if (bFlag == false)
			Sleep(100);
	}
	return bFlag;
}

bool CURLUpdater::URLDownLoad(const WCHAR * pURL, const WCHAR * pDest, bool bBaseAccess, std_str * pString, int DefaultTryCount, LPBINDSTATUSCALLBACK callback)
{
	WCHAR szSrc[256];
#if defined(PRE_FIX_DOWNLOADFILE)
	char szBaseURL[256] = {0,};
	char* szAddress = NULL;
	char* szForder = NULL;
	char szContent[256] = {0,};
	WideCharToMultiByte(CP_ACP, 0, m_szBaseURL, -1, szBaseURL, 256, NULL, NULL);
	char* token = strtok(szBaseURL, "/");

	// ���� �ּҿ� �ڿ� ������ �и�
	int nCount = 0;
	while(token != NULL)
	{
		if( nCount == 1)
			szAddress = token;
		else if (nCount == 2)
			szForder = token;
		token = strtok(NULL, "/");
		++nCount;
	}
	if( bBaseAccess)
	{	
		if( szForder )
			swprintf(szSrc, L"/%S/%s", szForder, pURL);
		else
			swprintf(szSrc, L"/%s", pURL);

		WideCharToMultiByte(CP_ACP, 0, szSrc, -1, szContent, 256, NULL, NULL);
	}
	else
	{
		if( szForder )
			swprintf(szSrc, L"/%S/%s/%s", szForder, m_szPatchURL, pURL);		
		else
			swprintf(szSrc, L"/%s/%s", m_szPatchURL, pURL);

		WideCharToMultiByte(CP_ACP, 0, szSrc, -1, szContent, 256, NULL, NULL);
	}
#else
	if (bBaseAccess)
		swprintf(szSrc, L"%s/%s", m_szBaseURL, pURL);
	else
		swprintf(szSrc, L"%s/%s/%s", m_szBaseURL, m_szPatchURL, pURL);
#endif
	
	bool bFlag = false;
	for (int nCount = 0; nCount < DefaultTryCount && bFlag == false; nCount++)
	{
#if defined(PRE_FIX_DOWNLOADFILE)
		CHttpClient HttpDown;
		HRESULT hRet = S_OK;
		if( HttpDown.Open(szAddress, CHttpClient::EV_HTTP_CONNECTION_KEEPALIVE) == FALSE )
			hRet = INET_E_RESOURCE_NOT_FOUND;
		else
		{
			if( HttpDown.SendRequestGet(szContent) == -1 )
				hRet = INET_E_OBJECT_NOT_FOUND;
			else
			{
				char szDestFileName[256] = {0,};
				WideCharToMultiByte(CP_ACP, 0, pDest, -1, szDestFileName, 256, NULL, NULL);

				if( HttpDown.RecvDownFile(szDestFileName, callback) == -1 )
					hRet = INET_E_DOWNLOAD_FAILURE;
			}
		}	
#else
		HRESULT hRet = URLDownloadToFileW(NULL, szSrc, pDest, 0, callback);
#endif //#if defined(PRE_FIX_DOWNLOADFILE)
		bFlag = hRet == S_OK ? true : false;
		if (hRet != S_OK)
		{
			//_com_error err = (hRet);
			//std_str strerr = err.ErrorMessage();
			std_str strerr = GetErrorMessage(hRet);

			if (pString)
			{
				*pString = _T("[Err:");
				*pString += strerr;
				*pString += _T("] [Url:");
	#ifndef _UNICODE
				std_str tempstr;
				ToMultiString(const_cast<WCHAR*>(pURL), tempstr);
				*pString += tempstr;
	#else
				*pString += pURL;
	#endif
				*pString += _T("]");
			}
#if defined(_LAUNCHER)
			TextOut("URLDownLoadToFile [Err:%s][Url:%S]", strerr.c_str(), pURL);
#else
#if defined (_SERVICEMANAGER_EX)
			g_Log.Log(LogType::_NORMAL, L"URLDownLoadToFile [Err:%s][Url:%S][Dest:%S]", strerr.c_str(), pURL, pDest);
#else
			printf("URLDownLoadToFile [Err:%s][Url:%S][Dest:%S]", strerr.c_str(), pURL, pDest);
#endif // #if defined (_SERVICEMANAGER_EX)
#endif
		}
		DeleteUrlCacheEntryW(szSrc);
		if (bFlag)
		{
#if defined (_SERVICEMANAGER_EX)
			g_Log.Log(LogType::_FILELOG, L"DownLoad [%s]\n", szSrc);
#elif defined(_LAUNCHER)
			TextOut("DownLoad [%S]", szSrc);
#else
			wprintf(L"DownLoad [%s]\n", szSrc);
#endif
		}
		else
		{
#if defined (_SERVICEMANAGER_EX)
			g_Log.Log(LogType::_FILELOG, L"DownLoad Fail [%s]\n", szSrc);
#elif defined(_LAUNCHER)
			TextOut("DownLoad Fail [%S]", szSrc);
#else
			wprintf(L"DownLoad Fail [%s]\n", szSrc);
#endif
			Sleep(100);
		}
	}
	return bFlag;
}

bool CURLUpdater::LoadPatchList()
{
	CXMLParser parser;
	if (parser.Open("./PatchList.xml") == false)
	{
#ifdef _SERVICEMANAGER
		g_Log.Log(LogType::_FILELOG, L"ServerStruct.xml failed\r\n");
#endif
		return false;
	}

	ClearPatchList();

	ScopeLock <CSyncLock> sync(m_Sync);

	if (parser.FirstChildElement("PatchList", true))
	{	//Parse Struct
		if (parser.FirstChildElement("Subject", true))
		{
			do {
				UpdateSubject * pInfo = new UpdateSubject;
				memset(pInfo, 0, sizeof(UpdateSubject));

				wcscpy_s(pInfo->wszAssignedType, parser.GetAttribute("Type"));
				if (!wcsicmp(pInfo->wszAssignedType, L"serverstruct"))
					pInfo->nAssignedType = -1;
				else
					pInfo->nAssignedType = GetPatchServerType(pInfo->wszAssignedType);

				if (m_UpdateSubject.find(pInfo->nAssignedType) != m_UpdateSubject.end())
				{
					SAFE_DELETE(pInfo);
					return false;
				}

				if (parser.FirstChildElement("Item"))
				{
					do {
						UpdateItem * pItem = new UpdateItem;
						memset(pItem, 0, sizeof(UpdateItem));

						wcscpy_s(pItem->wszFolder, parser.GetAttribute("Type"));
						wcscpy_s(pItem->wszFile, parser.GetAttribute("Name"));

						std::wstring tempstr;
						if (wcslen(pItem->wszFolder) > 0)
						{
							tempstr += pItem->wszFolder;
							tempstr += L"/";
						}
						tempstr += pItem->wszFile;

						wcscpy_s(pItem->wszWhole, tempstr.c_str());

#ifdef _LAUNCHER
						if (wcsstr(pItem->wszFile, L".exe"))
						{
							char szExcuteTemp[512];
							WideCharToMultiByte(CP_ACP, 0, pItem->wszFile, (int)wcslen(pItem->wszFile) + 1, szExcuteTemp, 512, NULL, NULL);
							CProcessManager::GetInstance()->GetExutePath(szExcuteTemp, pItem->szExcutePath);
						}
#endif

						pInfo->ItemList.push_back(pItem);
					} while (parser.NextSiblingElement("Item"));
				}

				m_UpdateSubject[pInfo->nAssignedType] = pInfo;
				m_wUpdateSubject[pInfo->wszAssignedType] = pInfo;
			} while (parser.NextSiblingElement("Subject"));
		}
	}

	return true;
}

void CURLUpdater::ClearPatchList()
{
	ScopeLock <CSyncLock> sync(m_Sync);

	std::map <int, UpdateSubject*>::iterator ii;
	for (ii = m_UpdateSubject.begin(); ii != m_UpdateSubject.end(); ii++)
	{
		for (int h = 0; h < (int)(*ii).second->ItemList.size(); h++)
			SAFE_DELETE((*ii).second->ItemList[h]);
		SAFE_DELETE((*ii).second);
	}
	m_UpdateSubject.clear();
	m_wUpdateSubject.clear();
}

int CURLUpdater::GetNeedResType(std::vector <int> * pList)
{
	int nRet = NEED_NONE;
	std::vector <int>::iterator ii;
	for (ii = pList->begin(); ii != pList->end(); ii++)
	{
		if ((*ii) == PATCHTYPE_GAME)
			nRet = NEED_GAMERES;

		if ((*ii) == PATCHTYPE_LOGIN || (*ii) == PATCHTYPE_VILLAGE || (*ii) == PATCHTYPE_MASTER || (*ii) == PATCHTYPE_DB)
		{
			if (nRet == NEED_NONE)
				nRet = NEED_VILLAGERES;
		}
	}
	return nRet;
}

//void CURLUpdater::LoadPatchList(WCHAR * pList)
//{
	/*map_wstr <std::vector<UpdateInfo*>>::iterator ii;
	for (ii = m_UpdateList.begin(); ii != m_UpdateList.end(); ii++)
	{
		for (int i = 0; i < (int)(*ii).second.size(); i++)
			delete (*ii).second[i];
		(*ii).second.clear();
	}
	m_UpdateList.clear();

	WCHAR szFile[1024];
	memset(szFile, 0, sizeof(szFile));
	int nCnt = 0;
	int nLen = (int)wcslen(pList) + 1;
	for (int i = 0; i < nLen; i++)
	{
		if (pList[i] == '\n' || pList[i] == '\r' || pList[i] == '\0')
		{
			int len = (int)wcslen(szFile);
			if (len <= 0)	continue;

			UpdateInfo * info = new UpdateInfo;
			memset(info, 0, sizeof(UpdateInfo));
			
			wcscpy_s(info->wszWhole, szFile);
			info->wszWhole[len] = '\0';
			info->wszWhole[len+1] = '\0';

			GetFile(szFile, info->wszFolder, info->wszFile);

			map_wstr <std::vector<UpdateInfo*>>::iterator ii = m_UpdateList.find(info->wszFolder);
			if (ii == m_UpdateList.end())
			{
				std::vector <UpdateInfo*> vList;
				vList.push_back(info);
				m_UpdateList[info->wszFolder] = vList;
			}
			else
				(*ii).second.push_back(info);
			
			memset(szFile, 0, sizeof(szFile));
			nCnt = 0;
		}
		else
			szFile[nCnt++] = pList[i];
	}*/
//}

WCHAR * CURLUpdater::FileLoad(const WCHAR * pFileName)
{	
	FILE * fp = _wfopen(pFileName, L"rb");
	if (fp)
	{
		int len;
		char * ptr;
		WCHAR * tp;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		if(len < 0)
		{
			fclose(fp);
			return NULL;
		}
		fseek(fp, 0, SEEK_SET);
		ptr = new char [len+2];
		tp = new WCHAR [len+2];
		fread(ptr, 1, len, fp);
		fclose(fp);
		ptr[len] = '\0';
		ptr[len+1] = '\0';

		MultiByteToWideChar(CP_ACP, 0, ptr, -1, tp, len+2);
		delete [] ptr;
		
		return tp;
	}
	return NULL;
}

void CURLUpdater::GetFile(const WCHAR * pPath, WCHAR * pFolder, WCHAR * pFile)
{
	bool bFlag = false;
	int filecnt = 0, foldercnt = 0;
	for (int i = 0; pPath[i] != '\0'; i++)
	{
		if (bFlag) pFile[filecnt++] = pPath[i];
		else pFolder[foldercnt++] = pPath[i];

		if (pPath[i] == '/')	bFlag = true;
	}
	pFile[filecnt] = '\0';
	pFolder[foldercnt-1] = '\0';
}

const TCHAR* CURLUpdater::GetErrorMessage(HRESULT hResult)
{
	switch(hResult)
	{
	case INET_E_INVALID_URL : return _T("INVALID_URL");
	case INET_E_NO_SESSION: return _T("NO_SESSION");
	case INET_E_CANNOT_CONNECT: return _T("CANNOT_CONNECT");
	case INET_E_RESOURCE_NOT_FOUND: return _T("Can't connect Patch Server"); // �̰� ������ ������ ���� ������ �ΰ� ����..(���� �ּ� �߸� �־�����.)
	case INET_E_OBJECT_NOT_FOUND: return _T("Patch File Not Found"); // ������ ���� ���� ������...
	case INET_E_DATA_NOT_AVAILABLE: return _T("DATA_NOT_AVAILABLEL");
	case INET_E_DOWNLOAD_FAILURE: return _T("DOWNLOAD_FAILURE");	        
	case INET_E_AUTHENTICATION_REQUIRED: return _T("AUTHENTICATION_REQUIRED");
	case INET_E_NO_VALID_MEDIA: return _T("NO_VALID_MEDIA");
	case INET_E_CONNECTION_TIMEOUT: return _T("CONNECTION_TIMEOUT");
	case INET_E_INVALID_REQUEST: return _T("INVALID_REQUEST");
	case INET_E_UNKNOWN_PROTOCOL: return _T("UNKNOWN_PROTOCOL");
	case INET_E_SECURITY_PROBLEM: return _T("SECURITY_PROBLEM");
	case INET_E_CANNOT_LOAD_DATA: return _T("CANNOT_LOAD_DATA");
	case INET_E_CANNOT_INSTANTIATE_OBJECT: return _T("CANNOT_INSTANTIATE_OBJECT");
	case INET_E_REDIRECT_FAILED: return _T("REDIRECT_FAILED");
	case INET_E_REDIRECT_TO_DIR: return _T("REDIRECT_TO_DIR");
	case INET_E_CANNOT_LOCK_REQUEST: return _T("CANNOT_LOCK_REQUEST");
	case INET_E_USE_EXTEND_BINDING: return _T("USE_EXTEND_BINDING");
	case INET_E_TERMINATED_BIND: return _T("TERMINATED_BIND");	
	case INET_E_CODE_DOWNLOAD_DECLINED: return _T("CODE_DOWNLOAD_DECLINED");
	case INET_E_RESULT_DISPATCHED: return _T("RESULT_DISPATCHED");
	case INET_E_CANNOT_REPLACE_SFP_FILE: return _T("CANNOT_REPLACE_SFP_FILE");
	//TODO(Cussrro): 
    //case INET_E_CODE_INSTALL_SUPPRESSED: return _T("CODE_INSTALL_SUPPRESSED");
	}
	return _T("Unknown Error");
}