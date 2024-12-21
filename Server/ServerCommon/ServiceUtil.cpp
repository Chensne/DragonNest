
#include "Stdafx.h"
#include <io.h>
#include <direct.h>
#include "SafeStringMacros.h"
#include "ServiceUtil.h"
#include "CommonMacros.h"


bool GetFirstRightValue(const WCHAR * pwszKey, std::wstring &wstrIn, std::wstring &wstrOut, bool bExtendOut)
{
	std::vector<std::wstring> tokens;
	TokenizeW(wstrIn.c_str(), tokens, L"/");

	std::vector<std::wstring>::iterator ii;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		std::vector<std::wstring> tokens2;
		TokenizeW((*ii).c_str(), tokens2, L"=");

		if (!_wcsicmp(tokens2[0].c_str(), pwszKey))
		{
			if (bExtendOut)
				wstrOut += tokens2[1];
			else
				wstrOut = tokens2[1];
			return true;
		}
	}
	return false;
}

bool GetFirstRightValue(const WCHAR * pwszKey, const WCHAR * pwszIn, std::wstring &wstrOut, bool bExtendOut)
{
	std::wstring wstrTempIn;
	wstrTempIn = pwszIn;

	return GetFirstRightValue(pwszKey, wstrTempIn, wstrOut, bExtendOut);
}

bool GetFirstRightIntValue(const WCHAR * pwszKey, const WCHAR * pwszIn, int &nOut)
{
	std::wstring wstrTempIn, wstrOut;
	wstrTempIn = pwszIn;

	bool bRet = GetFirstRightValue(pwszKey, wstrTempIn, wstrOut, false);
	if (bRet)
		nOut = _wtoi(wstrOut.c_str());
	return bRet;
}

bool GetFirstRightValue(const char * pszKey, std::string &strIn, std::string &strOut, bool bExtendOut)
{
	std::vector<std::string> tokens;
	TokenizeA(strIn.c_str(), tokens, "/");

	std::vector<std::string>::iterator ii;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		std::vector<std::string> tokens2;
		TokenizeA((*ii).c_str(), tokens2, "=");

		if (!stricmp(tokens2[0].c_str(), pszKey))
		{
			if (bExtendOut)
				strOut += tokens2[1];
			else
				strOut = tokens2[1];
			return true;
		}
	}
	return false;
}

bool GetFirstRightValue(const char * pszKey, const char * pszIn, std::string &strOut, bool bExtendOut)
{
	std::string strTempIn;
	strTempIn = pszIn;

	return GetFirstRightValue(pszKey, strTempIn, strOut, bExtendOut);
}

bool GetValueCount(const WCHAR * pwszKey, const std::wstring &wstrIn, int &nCount)
{
	nCount = 0;
	std::vector<std::wstring> tokens;
	TokenizeW(wstrIn.c_str(), tokens, L"/");
	
	std::vector<std::wstring>::iterator ii;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		std::vector<std::wstring> tokens2;
		TokenizeW((*ii).c_str(), tokens2, L"=");

		if (!_wcsicmp(tokens2[0].c_str(), pwszKey))
			nCount++;
	}
	return nCount <= 0 ? false : true;
}

bool GetRightValueByIndex(const WCHAR * pwszKey, const std::wstring &wstrIn, int nIndex, std::wstring &wstrOut, bool bExtendOut)
{
	int nCount = 0;
	std::vector<std::wstring> tokens;
	TokenizeW(wstrIn.c_str(), tokens, L"/");

	std::vector<std::wstring>::iterator ii;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		std::vector<std::wstring> tokens2;
		TokenizeW((*ii).c_str(), tokens2, L"=");

		if (!_wcsicmp(tokens2[0].c_str(), pwszKey))
		{
			if (nCount == nIndex)
			{
				if (bExtendOut)
					wstrOut += tokens2[1];
				else
					wstrOut = tokens2[1];
				return true;
			}
			else
				nCount++;
		}
	}
	return false;
}

bool GetRightValue(const WCHAR * pwszKey, std::wstring &wstrIn, std::vector<std::wstring> &vwstrOut)
{
	std::vector<std::wstring> tokens;
	TokenizeW(wstrIn.c_str(), tokens, L"/");

	bool bRet = false;
	std::vector<std::wstring>::iterator ii;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		std::vector<std::wstring> tokens2;
		TokenizeW((*ii).c_str(), tokens2, L"=");

		if (!_wcsicmp(tokens2[0].c_str(), pwszKey))
		{
			std::wstring wstrOut;
			wstrOut = tokens2[1];
			vwstrOut.push_back(wstrOut);
			bRet = true;
		}
	}
	return bRet;
}

bool IsUseCmd(const WCHAR * pwszArgv)
{
	std::wstring wstrIn;
	wstrIn = pwszArgv;

	std::vector<std::wstring> tokens;
	TokenizeW(wstrIn.c_str(), tokens, L"/");

	std::vector<std::wstring>::iterator ii;
	for (ii = tokens.begin(); ii != tokens.end(); ii++)
	{
		std::vector<std::wstring> tokens2;
		TokenizeW((*ii).c_str(), tokens2, L"=");

		if (!_wcsicmp(tokens2[0].c_str(), L"usecmd"))
			return true;
	}
	return false;
}

void GetDefaultInfo(const WCHAR * pwszArgv, int &nSID, std::string &strResOut, char * pszRevision, char * pszIP, int &nPort)
{
	char szCurPath[MAX_PATH], szDestPath[MAX_PATH];
	GetCurrentDirectoryA(sizeof(szCurPath), szCurPath);

	std::wstring wstrTempArgv;
	std::string strTempArgv;

	if (GetFirstRightValue(L"sid", pwszArgv, wstrTempArgv))
		nSID = _wtoi(wstrTempArgv.c_str());

	if (GetFirstRightValue(L"sip", pwszArgv, wstrTempArgv))
	{
		if (ToMultiString(wstrTempArgv, strTempArgv) == false)
			_ASSERT_EXPR(0, L"[GetDefaultInfo]sip invalid");
		strcpy_s(pszIP, IPLENMAX, strTempArgv.c_str());
	}

	if (GetFirstRightValue(L"sp", pwszArgv, wstrTempArgv))
		nPort = _wtoi(wstrTempArgv.c_str()) + 1;

	if (GetFirstRightValue(L"res", pwszArgv, wstrTempArgv) == false)
	{
		sprintf(szDestPath, "%s\\%s", szCurPath, "GameRes");
		strResOut = szDestPath;
	}
	else
	{
		if (ToMultiString(wstrTempArgv, strTempArgv) == false)
			_ASSERT_EXPR(0, L"[GetDefaultInfo]res invalid");

		sprintf(szDestPath, "%s\\%s", szCurPath, strTempArgv.c_str());
		strResOut = szDestPath;
	}

	char szResPath[MAX_PATH];
	sprintf(szResPath, "%s\\ResourceRevision.txt", szDestPath);
	FILE * fp = fopen(szResPath, "rb");
	if (fp)
	{
		int len;
		char * ptr;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		if( len < 0) 
		{
			fclose(fp);
			return;
		}
		ptr = new char [len+2];
		fread(ptr, 1, len, fp);
		fclose(fp);

		ptr[len] = '\0';
		ptr[len+1] = '\0';

		std::vector<std::string> tokens;
		TokenizeA(ptr, tokens, ":");

		if (tokens.size() >= 2)
		{
			int nVersion = atoi(tokens[1].c_str());
			USES_CONVERSION;
			strcpy_s(pszRevision, SERVERVERSIONMAX, I2A(nVersion));
		}		
		delete [] ptr;

	}
	else
		strcpy_s(pszRevision, SERVERVERSIONMAX, "Unknown Version");
}

void GetMemberShipInfo(const WCHAR * pwszArgv, TDBName * pDBInfo)
{
	std::wstring wstrTempArgv;
	std::string strTempArgv;

	if (GetFirstRightValue(L"msdb", pwszArgv, wstrTempArgv))
	{
		std::vector<std::wstring> tokens;
		TokenizeW(wstrTempArgv.c_str(), tokens, L",");

		if (tokens.size() >= 4)
		{
			ToMultiString(tokens[0], strTempArgv);
			strcpy_s(pDBInfo->szIP, IPLENMAX, strTempArgv.c_str());
			pDBInfo->nPort = _wtoi(tokens[1].c_str());
			wcscpy_s(pDBInfo->wszDBID, IDLENMAX, tokens[2].c_str());
			wcscpy_s(pDBInfo->wszDBName, DBNAMELENMAX, tokens[3].c_str());
		}
		else
			_ASSERT_EXPR(0, L"[GetMemberShipInfo]msdb tokens size < 4");
	}
	else
		_ASSERT_EXPR(0, L"[GetMemberShipInfo]msdb invalid");
}

void GetWorldInfo(const WCHAR * pwszArgv, TDBName * pDBInfo, int nArrCount)
{
	std::wstring wstrTempIn = pwszArgv;
	std::vector <std::wstring> vTempArgv;
	std::string strTempArgv;

	if (GetRightValue(L"wdb", wstrTempIn, vTempArgv))
	{
		for (int i = 0; i < (int)vTempArgv.size(); i++)
		{
			std::vector<std::wstring> tokens;
			TokenizeW(vTempArgv[i].c_str(), tokens, L",");

			if (tokens.size() >= 4)
			{
				pDBInfo[i].nWorldSetID = _wtoi(tokens[0].c_str());
				ToMultiString(tokens[1], strTempArgv);				
				
				strcpy_s(pDBInfo[i].szIP, IPLENMAX, strTempArgv.c_str());
				pDBInfo[i].nPort = _wtoi(tokens[2].c_str());
				wcscpy_s(pDBInfo[i].wszDBID, IDLENMAX, tokens[3].c_str());
				wcscpy_s(pDBInfo[i].wszDBName, DBNAMELENMAX, tokens[4].c_str());
			}
			else
				_ASSERT_EXPR(0, L"[GetWorldInfo]wdb tokens size < 4");
		}
	}
}

void GetLogdbInfo(const WCHAR * pwszArgv, TDBName * pDBInfo)
{
	std::wstring wstrTempArgv;
	std::string strTempArgv;

	if (GetFirstRightValue(L"sldb", pwszArgv, wstrTempArgv))
	{
		std::vector<std::wstring> tokens;
		TokenizeW(wstrTempArgv.c_str(), tokens, L",");

		if (tokens.size() >= 4)
		{
			ToMultiString(tokens[0], strTempArgv);			
			strcpy_s(pDBInfo->szIP, IPLENMAX, strTempArgv.c_str());
			pDBInfo->nPort = _wtoi(tokens[1].c_str());
			wcscpy_s(pDBInfo->wszDBID, IDLENMAX, tokens[2].c_str());
			wcscpy_s(pDBInfo->wszDBName, DBNAMELENMAX, tokens[3].c_str());
		}
		else
			_ASSERT_EXPR(0, L"[GetLogdbInfo]sldb tokens size < 4");
	}
	else
		_ASSERT_EXPR(0, L"[GetLogdbInfo]sldb Invalid" );
}

void GetDefaultConInfo(const WCHAR * pwszArgv, const WCHAR * pwszIdent, TConnectionInfo * pConInfo, int nConMax)
{
	std::vector <std::wstring> vwstrTempArgv;
	std::string strTempArgv;
	std::wstring wstrTempIn = pwszArgv;

	if (GetRightValue(pwszIdent, wstrTempIn, vwstrTempArgv))
	{
		for (int i = 0; i < (int)vwstrTempArgv.size() && i < nConMax; i++)
		{
			std::vector<std::wstring> tokens;
			TokenizeW(vwstrTempArgv[i].c_str(), tokens, L",");

			if (tokens.size() >= 2)
			{
				ToMultiString(tokens[0], strTempArgv);
				strcpy_s(pConInfo[i].szIP, IPLENMAX, strTempArgv.c_str());
				pConInfo[i].nPort = _wtoi(tokens[1].c_str());
			}
			else
				_ASSERT_EXPR(0, L"[GetDefaultConInfo] token size < 2" );
		}
	}
}


void GetDolbyAxonInfo(const WCHAR * pwszArgv, char * szPrivateIP, char * szPublicIP, int &nAPort, int &nCPort)
{
	std::wstring wstrTempArgv;
	std::string strTempArgv;

	if (GetFirstRightValue(L"daip", pwszArgv, wstrTempArgv))
	{
		std::vector<std::wstring> tokens;
		TokenizeW(wstrTempArgv.c_str(), tokens, L",");

		if (tokens.size() >= 4)
		{
			ToMultiString(tokens[0], strTempArgv);
			strcpy_s(szPrivateIP, IPLENMAX, strTempArgv.c_str());

			strTempArgv.clear();
			ToMultiString(tokens[1], strTempArgv);
			strcpy_s(szPublicIP, IPLENMAX, strTempArgv.c_str());
			nAPort = _wtoi(tokens[2].c_str());
			nCPort = _wtoi(tokens[3].c_str());
		}
		else
			_ASSERT_EXPR(0, L"[GetDolbyAxonInfo] token size < 4" );
	}
	else
		_ASSERT_EXPR(0, L"[GetDolbyAxonInfo] daip invalid" );
}

bool FolderCheckAndCreate(const char * pPath)
{
	if (pPath == NULL) return false;

	int nLen = (int)strlen(pPath);
	if (nLen <= 0 || nLen >= 512) return false;

	char szPath[512];
	memset(szPath, 0, sizeof(szPath));
	strcpy_s(szPath, pPath);

	for (int i = 0; i < (int)strlen(szPath); i++)
	{
		if (szPath[i] == NULL || szPath[i] == '\0')
			break;

		if (szPath[i] == '/')
			szPath[i] = '\\';
	}

	std::vector<std::string> tokens;
	TokenizeA(szPath, tokens, "\\");

	std::string strCheckPath;
	for (int i = 0; i < (int)tokens.size(); i++)
	{
		if (strstr(tokens[i].c_str(), ".")) continue;
				
		if (i == 0)
		{
			if (!strstr(tokens[i].c_str(), ":"))
			{
				char szCurPath[MAX_PATH];
				GetCurrentDirectoryA(sizeof(szCurPath), szCurPath);

				strCheckPath.append(szCurPath);
				strCheckPath.append("\\");
			}
			strCheckPath.append(tokens[i]);
		}
		else
		{
			strCheckPath.append("\\");
			strCheckPath.append(tokens[i]);
		}

		if (_access(strCheckPath.c_str(), 0) == -1)
			mkdir(strCheckPath.c_str());
	}
	return true;
}

bool FolderCheckAndCreate(const WCHAR * pPath)
{
	if (pPath == NULL) return false;

	int nLen = (int)wcslen(pPath);
	if (nLen <= 0 || nLen >= 512) return false;

	char szTemp[1024];
	WideCharToMultiByte(CP_ACP, 0, pPath, (int)wcslen(pPath) + 1, szTemp, 1024, NULL, NULL);

	return FolderCheckAndCreate(szTemp);
}

