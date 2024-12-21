#include "StdAfx.h"
#include "EtPackingShaderUtility.h"

std::vector<std::string> GetShaderFileNameList( CString szPath )
{
	char				wildName[MAX_PATH];
	sprintf(wildName, "%s\\*.fx", szPath.GetBuffer());

	std::vector<std::string> szResult;

	WIN32_FIND_DATA		hFinder;
	HANDLE hFindFile;
	if( (hFindFile = FindFirstFile(wildName, &hFinder)) == INVALID_HANDLE_VALUE) {
		return szResult;
	}

	do {
		if(hFinder.cFileName[0] == '.'  || (hFinder.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ||
													(hFinder.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		char name[MAX_PATH];
		sprintf(name, "%s\\%s", szPath.GetBuffer(), hFinder.cFileName);

		szResult.push_back( name );

	}while( FindNextFile(hFindFile, &hFinder) );

	FindClose(hFindFile);

	return szResult;
}

char *StrDup( char *szStr ) 
{
	int len = (int)strlen(szStr);
	char *szResult = new char[len+1];
	strcpy(szResult, szStr);
	szResult[len] = '\0';
	return szResult;
}

std::vector< D3DXMACRO > GetStringToMacro(CString str )
{
	char *szStr = str.GetBuffer();
	char szName[255], szDefinition[255];

	std::vector< D3DXMACRO > ResultMacro;

	char *ps = strtok( szStr, "\r\n");
	while( ps != NULL )	 {
		if( 2 == sscanf(ps, "%s %s", szName, szDefinition) ) {
			D3DXMACRO Macro;
			Macro.Name = StrDup(szName);
			Macro.Definition = StrDup( szDefinition);
			ResultMacro.push_back( Macro );
		}
		ps = strtok( NULL, "\r\n");
	}

	D3DXMACRO Macro;
	memset(&Macro, 0, sizeof(D3DXMACRO));
	ResultMacro.push_back( Macro );

	return ResultMacro;
}


LONG GetRegistryString(HKEY hKey, char *pSubKey, char *pValueName, char *pValue, int nMaxLength )
{
	HKEY hReg;
	DWORD dwType=REG_SZ, dwOption=REG_CREATED_NEW_KEY;
	LONG lRet;
	DWORD dwSize = nMaxLength ? nMaxLength : 512;

	lRet=RegOpenKeyEx(hKey, pSubKey, NULL, KEY_ALL_ACCESS, &hReg);
	if(lRet!=ERROR_SUCCESS){
		lRet=RegCreateKeyEx(hKey, pSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hReg, &dwOption);
		if(lRet!=ERROR_SUCCESS){
			return lRet;
		}
	}
	lRet=RegQueryValueEx(hReg, pValueName, NULL, &dwType, (unsigned char*)pValue, &dwSize);
	RegCloseKey(hReg);

	return lRet;
}

LONG SetRegistryString(HKEY hKey, char *pSubKey, char *pValueName, char *pValue )
{
	HKEY hReg;
	DWORD dwType=REG_SZ, dwOption=REG_CREATED_NEW_KEY;
	LONG lRet;
	DWORD dwSize = (DWORD)strlen( pValue );

	lRet=RegOpenKeyEx(hKey, pSubKey, NULL, KEY_ALL_ACCESS, &hReg);
	if(lRet!=ERROR_SUCCESS){
		lRet=RegCreateKeyEx(hKey, pSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hReg, &dwOption);
		if(lRet!=ERROR_SUCCESS){
			return lRet;
		}
	}
	lRet=RegSetValueEx(hReg, pValueName, NULL, dwType, (unsigned char*)pValue, dwSize);
	RegCloseKey(hReg);

	return lRet;
}