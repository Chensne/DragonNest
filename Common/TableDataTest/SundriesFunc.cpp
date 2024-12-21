#include "StdAfx.h"
#include "SundriesFunc.h"
#include "Stream.h"
#include <stdarg.h>
#include <Windows.h>
#include <./boost/algorithm/string.hpp>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef _CHECK_D3D_LEAK
struct ResStruct {
	std::map<void*, std::string > resourceList;
	~ResStruct() {
		_ASSERT( resourceList.empty() );
	}
}resStruct;

void __ADD_D3D_RES( void *pPtr , char *szFile, int nLine )
{
	static std::map<std::string, int > mapLineCounter;
	
	char szStr[4096]={0,}, szOut[4096]={0,};
	sprintf(szStr, "%s -(%d)", szFile, nLine );
	int nCount = mapLineCounter[ szStr ];
	mapLineCounter[ szStr ]++;

	int nFindKey = GetHashCode(szStr)^nCount;

	//if( nFindKey == 70356109 || 
	//	nFindKey == 254840137 ||
	//	nFindKey == 120819530 ||
	//	nFindKey == 120819529 
	//	) 
	//{
	//	int df = 3;
	//}

	sprintf(szOut, "%s - [%d]", szStr, nFindKey);

	ASSERT ( resStruct.resourceList.count(pPtr ) == 0 );
	resStruct.resourceList.insert( std::make_pair(pPtr, szOut) );
	//OutputDebug("%X Res Add %s %d \n", pPtr, szOut, resStruct.resourceList.size() );
	
}


void REMOVE_D3D_RES( void *pPtr )
{
	ASSERT ( resStruct.resourceList.count(pPtr ) != 0 );
	std::string szTypeName = "Null";
	if( resStruct.resourceList.count(pPtr ) != 0 ) {
		szTypeName = resStruct.resourceList[ pPtr ];
	}
	resStruct.resourceList.erase( pPtr );
	//OutputDebug("%X Res Remove %s %d\n", pPtr, szTypeName.c_str(), resStruct.resourceList.size() );
}
#endif

const char* _GetPath(char* pBuffer, const char* pPath)
{
	strcpy_s( pBuffer, (int)strlen(pPath) + 1, pPath );

	int i = (int ) strlen(pPath) - 1;

	while (i >= 0) {
		if (pPath[i] == '\\' || pPath[i] == '/') {
			pBuffer[i + 1] = 0;
			return pBuffer;
		}
		i--;
	}

	pBuffer[ 0 ] = 0;

	return pBuffer;
}


const char *_GetFullFileName( char *pBuffer, const char *pString )
{
	int i = (int ) strlen(pString) - 1;

	int nSize = i;
	while (i >= 0) {
		if ( pString[i] == '\\' || pString[i] == '/') {
			memcpy( pBuffer, pString + i + 1, nSize - i );
			pBuffer[nSize - i] = 0;
			return pBuffer;
		}

		i--;
	}
	strcpy_s( pBuffer, (int)strlen(pString)+1, pString );

	return pBuffer;

}

const char *_GetExt( char *pBuffer, const char *pString )
{
	int i = (int ) strlen(pString) - 1;

	int nSize = i;
	while (i >= 0) {
		if ( pString[i] == '.' ) {
			memcpy( pBuffer, pString + i + 1, nSize - i );
			pBuffer[nSize - i] = 0;
			return pBuffer;
		}

		i--;
	}
	pBuffer[0] = 0;

	return pBuffer;
}

const char *_GetFileName( char *pBuffer, const char *pString )
{
	int i = (int ) strlen(pString) - 1;

	int nSize = i;
	int nExtPos = -1;
	while (i >= 0) {
		if ( pString[i] == '\\' || pString[i] == '/') {
			memcpy( pBuffer, pString + i + 1, nExtPos - i - 1 );
			pBuffer[nExtPos - i - 1] = 0;
			return pBuffer;
		}
		else if( pString[i] == '.' && nExtPos == -1 ) {
			nExtPos = i;
		}

		i--;
	}
	if( nExtPos > 0 ) {
		memcpy( pBuffer, pString, nExtPos );
		pBuffer[nExtPos] = 0;
	}
	else pBuffer[0] = 0;
	return pBuffer;
}

char *_CopyString( const char *szStr )
{
	if( szStr == NULL ) return NULL;
	int nLength = (int)strlen(szStr);
	char *pResult = new char[nLength+1];
	memcpy( pResult, szStr, nLength );
	pResult[nLength] = 0;

	return pResult;
}

std::string _GetParsePipeSafe( int nCount, char *szSor, char cCharacter )
{
	// Parsing
	std::string					strValue(szSor);
	std::vector<std::string>	vSplit;
	char seps[] = { cCharacter, 0 };

	boost::algorithm::split( vSplit, strValue, boost::algorithm::is_any_of(seps) );

	if( static_cast<int>(vSplit.size()) <= nCount )
		return std::string("");

	return vSplit[nCount];
}


char *_GetParsePipe( int nCount, char *szSor, char cCharacter )
{
	char szStr[65535];
	strcpy( szStr, szSor );
	static char szResult[65535];
	int nSize = (int)strlen( szStr );

	int nCurCount = 0;
	int nStrCount = 0;
	BOOL bFlag = FALSE;
	memset( szResult, 0, sizeof(szResult) );
	for( int i=0; i<=nSize; i++ ) {
		if( szStr[i] == cCharacter || i == nSize ) {
			if( nCurCount == nCount ) {
				szResult[nStrCount] = 0;
				bFlag = TRUE;
				break;
			}
			else {
				nStrCount = 0;
				nCurCount++;
				continue;
			}
		}
		szResult[nStrCount] = szStr[i];
		nStrCount++;
	}
	if( bFlag == TRUE ) 
		return szResult;

	return NULL;
}

int _GetParsePipeCount( char *szSor, char cCharacter )
{
	char szStr[65535];
	strcpy( szStr, szSor );
	int nSize = (int)strlen( szStr );

	int nCurCount = 0;
	for( int i=0; i<=nSize; i++ ) {
		if( szStr[i] == cCharacter ) {
			nCurCount++;
		}
	}
	return nCurCount;
}


const char *_GetOriginalRandomBaseName( char *pBuffer, const char *pString )
{
	char szTempName[256] = { 0, };
	int nLength = (int)strlen(pString);
	bool bModifyName = false;
	for( int i=nLength - 1; i>=0; i-- ) {
		if( pString[i] == '.' ) {
			if( pString[i-2] == '_' && 
				( ( pString[i-1] >= 'a' && pString[i-1] <= 'z' ) || 
				( pString[i-1] >= 'A' && pString[i-1] <= 'Z' ) ) ) {
					memcpy( szTempName, pString, i - 2 );
					memcpy( szTempName + ( i - 2 ), pString + i, nLength - i );
					szTempName[ ( i - 2 ) + nLength - i ] = 0;
					bModifyName = true;
					break;
			}
		}
	}
	if( !bModifyName ) sprintf_s( pBuffer, nLength + 1, pString );
	else sprintf_s( pBuffer, strlen(szTempName) + 1, szTempName );

	return pBuffer;
}

void OutputDebugFunc( const char *szString, ... )
{
	if( !s_pDefaultOutputDebugFunc ) return;
	va_list list;
	char szTemp[65535];

	va_start( list, szString );
	vsprintf_s( szTemp, szString, list );
	s_pDefaultOutputDebugFunc( szTemp );
	va_end( list );
}

void _OutputDebug( const char *szString, ... )
{
	va_list list;
	static char szTemp[65535];

	va_start( list, szString );
	vsprintf_s( szTemp, szString, list );
	OutputDebugString( szTemp );
	va_end( list );
}


void SetOutputDebugFuncPtr( void (__stdcall *Func)(const char*, ...) )
{
	s_pDefaultOutputDebugFunc = Func;
}

int NavigateDirectory( const char *pDir, const char *pFileName, char *pTargetBuffer )
{
	char szFindFile[ _MAX_PATH ], szReturn[ _MAX_PATH ];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	strcpy( szFindFile, pDir );
	strcat( szFindFile, pFileName );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		if( pTargetBuffer ) {
			strcpy( pTargetBuffer, pDir );
			strcat( pTargetBuffer, FindFileData.cFileName );
		}
		FindClose( hFind );
		return 1;
	}

	strcpy( szReturn, pDir );
	strcat( szReturn, "*.*" );
	hFind = FindFirstFile( szReturn, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE )
	{
		if( ( FindFileData.cFileName[ 0 ] != '.' ) && ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			strcpy( szReturn, pDir );
			strcat( szReturn, FindFileData.cFileName );
			strcat( szReturn, "\\" );
			if( NavigateDirectory( szReturn, pFileName, pTargetBuffer ) )
			{
				return 1;
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) )
		{
			break;
		}
	}
	if( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
	}

	return 0;
}


int FindFileInDirectory( const char *pFindDir, const char *pFileName, char *pTargetBuffer )
{
	const char *pFindPtr;

	pFindPtr = strrchr( pFileName, '\\' );
	if( pFindPtr == NULL )
	{
		pFindPtr = pFileName;
	}
	else
	{
		pFindPtr++;
	}

	return NavigateDirectory( pFindDir, pFindPtr, pTargetBuffer );
}

LONG GetRegistryNumber(HKEY hKey, char *pSubKey, char *pValueName, DWORD &dwValue)
{
	HKEY hReg;
	DWORD dwType=REG_DWORD, dwOption=REG_CREATED_NEW_KEY;
	LONG lRet;
	DWORD dwSize=sizeof(DWORD);

	lRet=RegOpenKeyEx(hKey, pSubKey, NULL, KEY_ALL_ACCESS, &hReg);
	if(lRet!=ERROR_SUCCESS){
		lRet=RegCreateKeyEx(hKey, pSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hReg, &dwOption);
		if(lRet!=ERROR_SUCCESS){
			return lRet;
		}
	}
	lRet=RegQueryValueEx(hReg, pValueName, NULL, &dwType, (unsigned char*)&dwValue, &dwSize);
	RegCloseKey(hReg);

	return lRet;
}

LONG SetRegistryNumber(HKEY hKey, char *pSubKey, char *pValueName, DWORD dwValue)
{
	HKEY hReg;
	DWORD dwType=REG_DWORD, dwOption=REG_CREATED_NEW_KEY;
	LONG lRet;
	DWORD dwSize=sizeof(DWORD);

	lRet=RegOpenKeyEx(hKey, pSubKey, NULL, KEY_ALL_ACCESS, &hReg);
	if(lRet!=ERROR_SUCCESS){
		lRet=RegCreateKeyEx(hKey, pSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hReg, &dwOption);
		if(lRet!=ERROR_SUCCESS){
			return lRet;
		}
	}
	lRet=RegSetValueEx(hReg, pValueName, NULL, dwType, (unsigned char*)&dwValue, dwSize);
	RegCloseKey(hReg);

	return lRet;
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

LONG GetRegistryBinary(HKEY hKey, char *pSubKey, char *pValueName, void *pValue, int &nSize )
{
	HKEY hReg;
	DWORD dwType=REG_BINARY, dwOption=REG_CREATED_NEW_KEY;
	LONG lRet;

	lRet=RegOpenKeyEx(hKey, pSubKey, NULL, KEY_ALL_ACCESS, &hReg);
	if(lRet!=ERROR_SUCCESS){
		lRet=RegCreateKeyEx(hKey, pSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hReg, &dwOption);
		if(lRet!=ERROR_SUCCESS){
			return lRet;
		}
	}
	lRet=RegQueryValueEx(hReg, pValueName, NULL, &dwType, (unsigned char*)pValue, (DWORD*)&nSize);
	RegCloseKey(hReg);

	return lRet;
}

LONG SetRegistryBinary(HKEY hKey, char *pSubKey, char *pValueName, void *pValue, int nSize )
{
	HKEY hReg;
	DWORD dwType=REG_BINARY, dwOption=REG_CREATED_NEW_KEY;
	LONG lRet;

	lRet=RegOpenKeyEx(hKey, pSubKey, NULL, KEY_ALL_ACCESS, &hReg);
	if(lRet!=ERROR_SUCCESS){
		lRet=RegCreateKeyEx(hKey, pSubKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hReg, &dwOption);
		if(lRet!=ERROR_SUCCESS){
			return lRet;
		}
	}
	lRet=RegSetValueEx(hReg, pValueName, NULL, dwType, (unsigned char*)pValue, nSize);
	RegCloseKey(hReg);

	return lRet;
}

LONG DeleteRegistry( HKEY hKey, char *pSubKey )
{
	HKEY newKey;
	char newSubKey[MAX_PATH];
	LONG Result;
	DWORD Size;
	FILETIME FileTime;

	RegOpenKeyEx(hKey, pSubKey, 0, KEY_ALL_ACCESS, &newKey);
	Result = ERROR_SUCCESS;
	while(TRUE) {
		Size = MAX_PATH;
		Result = RegEnumKeyEx(newKey, 0, newSubKey, &Size, NULL, NULL, NULL, &FileTime);
		if (Result != ERROR_SUCCESS) break;
		Result = DeleteRegistry(newKey, newSubKey);
		if (Result  != ERROR_SUCCESS) break;

	}
	RegCloseKey(newKey);
	return RegDeleteKey(hKey, pSubKey);
}

LONG AddRegistryKey( HKEY hKey, char *pSubKey, char *pValueName )
{
	HKEY newKey;
	char newSubKey[MAX_PATH];
	DWORD dwDisp;
	sprintf_s( newSubKey, "%s\\%s", pSubKey, pValueName );
	LONG Result = RegCreateKeyEx( hKey, newSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &newKey, &dwDisp );

	return Result;
}

void FindFileListInDirectory( const char *szFolderName, const char *szExt, std::vector<CFileNameString> &szVecResult, bool bIncludeFolder, bool bSearchSubDir, bool bIncludeFolderName )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szFindFile[512];

	sprintf_s( szFindFile, "%s\\%s", szFolderName, szExt );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ) {
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) || bIncludeFolder == true ) {
				if( bIncludeFolderName )
				{
					char szFullName[ _MAX_PATH ];
					sprintf_s( szFullName, "%s\\%s", szFolderName, FindFileData.cFileName );
					szVecResult.push_back( szFullName );
				}
				else
				{
					szVecResult.push_back( CFileNameString( FindFileData.cFileName ) );
				}
			}
			if( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && bSearchSubDir == true ) {
				if( strcmp( FindFileData.cFileName, "." ) && strcmp( FindFileData.cFileName, ".." ) ) {
					char szFolder[512] = { 0, };
					sprintf_s( szFolder, "%s\\%s", szFolderName, FindFileData.cFileName );
					FindFileListInDirectory( szFolder, szExt, szVecResult, bIncludeFolder, bSearchSubDir, bIncludeFolderName );
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;

	}

	FindClose( hFind );
}

void FindFileListInDirectory( const char *szFolderName, const char *szExt, std::vector<std::string> &szVecResult, bool bIncludeFolder, bool bSearchSubDir, bool bIncludeFolderName )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szFindFile[512];

	sprintf_s( szFindFile, "%s\\%s", szFolderName, szExt );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ) {
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) || bIncludeFolder == true ) {
				if( bIncludeFolderName )
				{
					char szFullName[ _MAX_PATH ];
					sprintf_s( szFullName, "%s\\%s", szFolderName, FindFileData.cFileName );
					szVecResult.push_back( szFullName );
				}
				else
				{
					szVecResult.push_back( std::string( FindFileData.cFileName ) );
				}
			}
			if( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && bSearchSubDir == true ) {
				if( strcmp( FindFileData.cFileName, "." ) && strcmp( FindFileData.cFileName, ".." ) ) {
					char szFolder[512] = { 0, };
					sprintf_s( szFolder, "%s\\%s", szFolderName, FindFileData.cFileName );
					FindFileListInDirectory( szFolder, szExt, szVecResult, bIncludeFolder, bSearchSubDir, bIncludeFolderName );
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;

	}

	FindClose( hFind );
}

void _FindFolder( const char *szFolderName, std::vector<CFileNameString> &szVecResult, bool bSearchSubDir, char *szRootPathName ) 
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szFindFile[512];

	sprintf_s( szFindFile, "%s\\*.*", szFolderName );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) && strcmp( ".", FindFileData.cFileName ) && strcmp( "..", FindFileData.cFileName ) ) {
				if( bSearchSubDir && szRootPathName && strcmp( szFolderName, szRootPathName ) ) {
					char szTemp[512];
					int nOffset = (int)strlen( szRootPathName ) + 1;
					sprintf_s( szTemp, "%s\\%s", szFolderName + nOffset, FindFileData.cFileName );
					szVecResult.push_back( szTemp );
				}
				else szVecResult.push_back( FindFileData.cFileName );

				if( bSearchSubDir ) {
					char szTemp[512];
					sprintf_s( szTemp, "%s\\%s", szFolderName, FindFileData.cFileName );
					_FindFolder( szTemp, szVecResult, bSearchSubDir, szRootPathName );
				}
			}
		}

		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}

	FindClose( hFind );
}

void _FindFolder( const char *szFolderName, std::vector<std::string> &szVecResult, bool bSearchSubDir, char *szRootPathName ) 
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szFindFile[512];

	sprintf_s( szFindFile, "%s\\*.*", szFolderName );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) && strcmp( ".", FindFileData.cFileName ) && strcmp( "..", FindFileData.cFileName ) ) {
				if( bSearchSubDir && szRootPathName && strcmp( szFolderName, szRootPathName ) ) {
					char szTemp[512];
					int nOffset = (int)strlen( szRootPathName ) + 1;
					sprintf_s( szTemp, "%s\\%s", szFolderName + nOffset, FindFileData.cFileName );
					szVecResult.push_back( szTemp );
				}
				else szVecResult.push_back( FindFileData.cFileName );

				if( bSearchSubDir ) {
					char szTemp[512];
					sprintf_s( szTemp, "%s\\%s", szFolderName, FindFileData.cFileName );
					_FindFolder( szTemp, szVecResult, bSearchSubDir, szRootPathName );
				}
			}
		}

		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}

	FindClose( hFind );
}

bool _CopyFile( const char *pSource, const char *pTarget, bool bExistOverwrite, bool bMoveFile )
{
	char szSorPath[512] = { 0, };
	char szSorExt[256] = { 0, };

	_GetPath( szSorPath, pSource );
	_GetFullFileName( szSorExt, pSource );

	std::vector<CFileNameString> szVecList;
	FindFileListInDirectory( szSorPath, szSorExt, szVecList, false );

	char szStr[512] = { 0, }; 
	char szStr2[512] = { 0, };
	bool bResult = true;
	for( DWORD i=0; i<szVecList.size(); i++ ) {
		sprintf_s( szStr, "%s%s", szSorPath, szVecList[i].c_str() );
		sprintf_s( szStr2, "%s\\%s", pTarget, szVecList[i].c_str() );
		if( CopyFile( szStr, szStr2, !bExistOverwrite ) == false ) bResult = false;
		else {
			if( bMoveFile ) DeleteFile( szStr );
		}
	}
	return bResult;
}

void DeleteFolder( const char *pFolder )
{
	char szFindFile[ _MAX_PATH ];
	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	strcpy_s( szFindFile, _MAX_PATH, pFolder );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );
	hFind = FindFirstFile( szFindFile, &FindData );
	while( hFind != INVALID_HANDLE_VALUE )
	{
		if( FindData.cFileName[ 0 ] != '.' ) 
		{
			char szFullName[ _MAX_PATH ];
			strcpy_s( szFullName, _MAX_PATH, pFolder );
			strcat_s( szFullName, _MAX_PATH, "\\" );
			strcat_s( szFullName, _MAX_PATH, FindData.cFileName );
			if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				DeleteFolder( szFullName );
			}
			else
			{
				DeleteFile( szFullName );
			}
		}
		if( !FindNextFile( hFind, &FindData ) )
		{
			FindClose( hFind );
			RemoveDirectory( pFolder );
			break;
		}
	}
}

void CreateFolder( const char *pFolder )
{
	char szPath[ _MAX_PATH ], *pFindPtr;

	strcpy_s( szPath, _MAX_PATH, pFolder );
	if( szPath[ strlen( szPath ) -1 ] != '\\' )
	{
		strcat_s( szPath, _MAX_PATH, "\\" );
	}
	pFindPtr = szPath;
	while( 1 )
	{
		pFindPtr = strchr( pFindPtr, '\\' );
		if( pFindPtr == NULL )
		{
			break;
		}
		if( *( pFindPtr - 1 ) != ':' )
		{
			*pFindPtr = 0;
			CreateDirectory( szPath, NULL );
			*pFindPtr = '\\';
		}
		pFindPtr++;
	}
}

void GetFullPath( const char *pSource, char *pTarget )
{
	strcpy( pTarget, pSource );
	if( pTarget[ strlen( pTarget ) - 1 ] != '\\' )
	{
		strcat( pTarget, "\\" );
	}
}

const char *FindFileName( const char *pFullName )
{
	const char *pFindPtr;

	pFindPtr = strrchr( pFullName, '\\' );
	if( pFindPtr )
	{
		return pFindPtr + 1;
	}
	else
	{
		return pFullName;
	}
}

bool IsFullName( const char *pFileName )
{
	if( pFileName[ 1 ] == ':' )
	{
		return true;
	}
	else if( pFileName[0] == '\\' && pFileName[1] == '\\' ) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

#ifndef WIN64
void SSE_memcpy(void *pDest,void *pSrc,long nCnt)
{
	__asm
	{
		mov  ecx , nCnt
			shr  ecx , 4

			mov  edi , pDest
			mov  esi , pSrc

Loop_S2 :
		movdqu xmm0 , [esi]
		movdqu [edi] , xmm0
			add  edi,16
			add  esi,16
			loop Loop_S2

			// 나머지 처리
			mov ecx, nCnt
			and ecx, 15
			rep movsb
	}
}
#endif

void ReadStdString( std::string &szStr, CStream *pStream )
{
	int nSize;
	char szBuf[4096] = {0,};
	pStream->Read( &nSize, sizeof(int) );
	if( nSize )
	{
		if( nSize >= 4096 ) nSize = 4095;
		pStream->Read( szBuf, nSize );
		szBuf[ nSize ] = '\0';
	}

	szStr.clear();
	szStr = szBuf;
}

void WriteStdString( std::string &szStr, CStream *pStream )
{
	int nSize;

	nSize = ( int )szStr.size();
	if( nSize )
	{
		nSize++;
		pStream->Write( &nSize, sizeof( int ) );
		pStream->Write( szStr.c_str(), nSize );
	}
	else
	{
		pStream->Write( &nSize, sizeof( int ) );
	}
}


int GetOSVersionType()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx;
	if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
			return -1;
	}

	UINT nOSVersion(-1);

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		if (osvi.dwMajorVersion == 4 )
		{
			switch( osvi.dwMinorVersion )
			{
			case 0:		nOSVersion = 1;		break;
			case 10:	nOSVersion = 2;		break;
			case 90:	nOSVersion = 3;		break;
			}
		}
		break;
	case VER_PLATFORM_WIN32_NT:
		{
			if( osvi.dwMajorVersion <= 4 )
			{
				nOSVersion = 4;
			}
			else if( osvi.dwMajorVersion == 5 )
			{
				switch( osvi.dwMinorVersion )
				{
				case 0:	nOSVersion = 5;		break;
				case 1:	nOSVersion = 6;		break;
				case 2:	nOSVersion = 7;		break;
				}
			}
			else if( osvi.dwMajorVersion == 6 )
			{
				switch(osvi.dwMinorVersion)
				{
				case 0:	nOSVersion = 8;		break;
				case 1:	nOSVersion = 9;		break;
				}
			}
		}
		break;
	}

	return nOSVersion;
}

unsigned long GetHashCode( const char* String )
{
	unsigned long i,len;
	unsigned long ch;
	unsigned long result;
	unsigned long XOR;

	len		   = ( long )strlen( String );
	result	 = 0;
	XOR		   = 0;
	for( i=0; i<len; i++ )
	{
		ch = (unsigned long)String[i];
		result = ( ( result << 8 ) | ch ) % 16777213UL; // 16777213UL
		XOR ^= ch;
	}

	return result | ( XOR << 24 );
}

INT64 GetHashCode64( const char* String )
{
	unsigned long i,len;
	INT64 ch;
	INT64 result;
	INT64 XOR;
	INT64 temp = 0;

	len		   = ( long )strlen( String );
	result	 = 0;
	XOR		   = 0;
	for( i=0; i<len; i++ )
	{
		ch = (INT64)( String[i] * String[i] );
		temp += ch;
		result = ( ( result << 8 ) | ch ) % 0x00fffffffffffffd; // 16777213UL
		XOR ^= ch;
	}

	return ( result | ( XOR << 48 ) ) + temp;
}

bool ExecCommand( char *pCommand, std::string *pszOutput )
{
	HANDLE hReadPipe, hWritePipe;
	SECURITY_ATTRIBUTES SecurityAttr;

	memset( &SecurityAttr, 0, sizeof( SECURITY_ATTRIBUTES ) );
	SecurityAttr.nLength = sizeof( SECURITY_ATTRIBUTES );
	SecurityAttr.bInheritHandle = TRUE;

	if( pszOutput )
	{
		CreatePipe( &hReadPipe, &hWritePipe, &SecurityAttr, 0 );
	}

	STARTUPINFO sInfo; 
	PROCESS_INFORMATION pInfo; 
	memset( &sInfo, 0, sizeof( sInfo ) );
	memset( &pInfo, 0, sizeof( pInfo ) );
	sInfo.cb = sizeof( sInfo );
	if( pszOutput )
	{
		sInfo.dwFlags=STARTF_USESTDHANDLES;
		sInfo.hStdInput=NULL; 
		sInfo.hStdOutput=hWritePipe; 
		sInfo.hStdError=hWritePipe;
	}
	DWORD dwFlags = NORMAL_PRIORITY_CLASS;
	if( pszOutput ) dwFlags |= CREATE_NO_WINDOW;
	if( CreateProcess( 0, pCommand, 0, 0, TRUE, dwFlags, 0, 0, &sInfo, &pInfo) )
	{
		if( pszOutput )
		{
			char cBuffer[ 65536 ];
			DWORD dwReadSize; 
			*pszOutput = "";
			CloseHandle( hWritePipe );
			while( 1 )
			{
				memset( cBuffer, 0, 65536 );
				::ReadFile( hReadPipe, cBuffer, 65535, &dwReadSize, 0 );
				if( dwReadSize <= 0 )
				{
					break;
				}
				*pszOutput += cBuffer;
			}
		}
		return true;
	}

	return false;
}

void SetBitFlag( char *pBuffer, int nArrayIndex, bool bFlag )
{
	int nBitSet = 7 - ( nArrayIndex % 8 );
	if( bFlag ) pBuffer[nArrayIndex/8] |= ( (char)1<<nBitSet );
	else pBuffer[nArrayIndex/8] &= ~( (char)1<<nBitSet );
}

bool GetBitFlag( char *pBuffer, int nArrayIndex )
{
	int nBitSet = ( nArrayIndex % 8 );
	return ( (char)( ( ( ( pBuffer[nArrayIndex/8] << nBitSet ) & 0xff ) >> 7 ) & 0xff ) == 1 ) ? true : false;
}

void (__stdcall *s_pDefaultOutputDebugFunc)(const char*, ...) = _OutputDebug;

int GetDateValue( int nYear, int nMonth, int nDay )
{
	int nResult = 0;
	for( int i=2009; i<nYear; i++ ) {
		bool bLeapMonth = false;
		//		int nValue = ( i - 1583 ) + 1;
		if( i % 4 == 0 ) bLeapMonth = true;
		if( i % 100 == 0 ) bLeapMonth = false;
		if( i % 400 == 0 ) bLeapMonth = true;

		if( bLeapMonth ) nResult += 366;
		else nResult += 365;
	}

	bool bLeapMonth = false;
	//	int nValue = ( nYear - 1583 ) + 1;
	if( nYear % 4 == 0 ) bLeapMonth = true;
	if( nYear % 100 == 0 ) bLeapMonth = false;
	if( nYear % 400 == 0 ) bLeapMonth = true;
	int nDayCount[12] = { 31, ( bLeapMonth == true ) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	for( int i=0; i<nMonth - 1; i++ ) {
		nResult += nDayCount[i];
	}
	nResult += nDay;

	return nResult;
}

bool DnLocalTime_s( struct tm * _Tm, const time_t * _Time )
{
	if( *_Time < 0 || *_Time > 0x793406fffi64 ) return false;
	return ( localtime_s( _Tm, _Time ) ) ? false : true;
}

int GetValue2ExceptionSwap( int nValue, int nSwapValueCount, ... )
{
	int nResult = nValue;
	va_list marker;

	va_start( marker, nSwapValueCount );     /* Initialize variable arguments. */
	for( int i=0; i<nSwapValueCount; i++ ) {
		int nValue1 = va_arg( marker, int );
		int nValue2 = va_arg( marker, int );

		if( nResult == nValue1 ) {
			nResult = nValue2;
			break;
		}
		else if( nResult == nValue2 ) {
			nResult = nValue1;
			break;
		}
	}
	return nResult;
}

DWORD CalcDecreaseColor( DWORD OrigColor, DWORD DescColor )
{
	int nOrigA = OrigColor >> 24;
	int nOrigR = ( OrigColor << 8 ) >> 24;
	int nOrigG = ( OrigColor << 16 ) >> 24;
	int nOrigB = ( OrigColor << 24 ) >> 24;

	int nA = DescColor >> 24;
	int nR = ( DescColor << 8 ) >> 24;
	int nG = ( DescColor << 16 ) >> 24;
	int nB = ( DescColor << 24 ) >> 24;

	nOrigA = max( nOrigA - nA, 0 );
	nOrigR = max( nOrigR - nR, 0 );
	nOrigG = max( nOrigG - nG, 0 );
	nOrigB = max( nOrigB - nB, 0 );

	return ((DWORD)((((nOrigA)&0xff)<<24)|(((nOrigG)&0xff)<<16)|(((nOrigG)&0xff)<<8)|((nOrigB)&0xff)));
}