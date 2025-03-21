
#pragma once

#include "DNConfig.h"

bool GetFirstRightValue(const WCHAR * pwszKey, std::wstring &wstrIn, std::wstring &wstrOut, bool bExtendOut = false);
bool GetFirstRightValue(const WCHAR * pwszKey, const WCHAR * pwszIn, std::wstring &wstrOut, bool bExtendOut = false);
bool GetFirstRightIntValue(const WCHAR * pwszKey, const WCHAR * pwszIn, int &nOut);
bool GetFirstRightValue(const char * pszKey, std::string &wstrIn, std::string &strOut, bool bExtendOut = false);
bool GetFirstRightValue(const char * pszKey, const char * pszIn, std::string &strOut, bool bExtendOut = false);

bool GetValueCount(const WCHAR * pwszKey, const std::wstring &wstrIn, int &nCount);
bool GetRightValueByIndex(const WCHAR * pwszKey, const std::wstring &wstrIn, int nIndex, std::wstring &wstrOut, bool bExtendOut = false);
bool GetRightValue(const WCHAR * pwszKey, std::wstring &wstrIn, std::vector<std::wstring> &vwstrOut);

bool IsUseCmd(const WCHAR * pwszArgv);

void GetDefaultInfo(const WCHAR * pwszArgv, int &nSID, std::string &strResOut, char * pszRevision, char * pszIP, int &nPort);
void GetMemberShipInfo(const WCHAR * pwszArgv, TDBName * pDBInfo);
void GetWorldInfo(const WCHAR * pwszArgv, TDBName * pDBInfo, int nArrCount);
void GetLogdbInfo(const WCHAR * pwszArgv, TDBName * pDBInfo);
void GetDefaultConInfo(const WCHAR * pwszArgv, const WCHAR * pwszIdent, TConnectionInfo * pConInfo, int nConMax);
void GetDolbyAxonInfo(const WCHAR * pwszArgv, char * szPrivate, char * szPublicIP, int &nAPort, int &nCPort);

bool FolderCheckAndCreate(const char * pPath);
bool FolderCheckAndCreate(const WCHAR * pPath);