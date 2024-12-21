// RCleanerCmd.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <conio.h>
#include <Shlwapi.h>
#include <windows.h>
#include <stdio.h>
#include "DNTableFile.h"
#include "SundriesFunc.h"

#include <string>
#include <map>
using namespace std;

void PrintError( const char *szString, ... );
void DeleteFolder( const char *pFolder, bool bDeleteReadOnly );
void CheckGridFolder( const char *pGridFolder );

std::string s_szNation;
std::map<string, string> s_mapIncludeBuildMapName;

char g_szErrorFileName[512];
int Run( int argc, _TCHAR* argv[] )
{	
	if( argc < 2 ) {
		printf( "Argument\nRCleanerCmd [Resource Path] [Nation(null)]\n" );
		return -1;
	}
	////////////////////////
	char szResultFile[512] = { 0, };
	sprintf_s( szResultFile, "RCleanerCmd_Completed.txt" );
	SetFileAttributes( szResultFile, FILE_ATTRIBUTE_NORMAL );
	DeleteFile( szResultFile );
	////////////////////////

	if( argc == 2 ) {
		s_szNation = "";
	}
	else {
		s_szNation = argv[2];
		if( _stricmp( argv[2], "null" ) == NULL ) s_szNation = "";
	}


	// Original 체크
	if( PathIsDirectory( argv[1] ) == FALSE ) {
		PrintError( "Invalid Resource Path\n" );
		return -1;
	}

	// SOX 얻어온다.
	char szOrig[512];
	DNTableFileFormat *pSox = new DNTableFileFormat;
	bool bValidNation = false;
	if( !s_szNation.empty() ) {
		sprintf_s( szOrig, "%s\\Resource_%s\\Ext\\MapTable.ext", argv[1], s_szNation.c_str() );
		if( PathFileExists( szOrig ) == TRUE ) {
			bValidNation = true;
			if( pSox->Load( szOrig ) == false ) {
				PrintError( "Ext Open Error - Nation\n" );
				return -1;
			}
		}
	}
	if( !bValidNation ) {
		sprintf_s( szOrig, "%s\\Resource\\Ext\\MapTable.ext", argv[1], s_szNation.c_str() );
		if( pSox->Load( szOrig ) == false ) {
			PrintError( "Ext Open Error - Local\n" );
			return -1;
		}
	}

	char szLabel[64];
	std::string szMapName;
	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		int nItemID = pSox->GetItemID(i);
		for( int j=0; j<10; j++ ) {
			sprintf_s( szLabel, "_ToolName%d", j + 1 );
			szMapName = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetString();
			if( pSox->GetFieldFromLablePtr( nItemID, "_IncludeBuild" )->GetInteger() == 0 ) {
				bool bEnable = true;
				if( szMapName.empty() ) bEnable = false;
				if( (int)szMapName.size() == 1 && szMapName[0] == '.' ) bEnable = false;
				if( (int)szMapName.size() == 2 && szMapName[0] == '.' && szMapName[1] == '.' ) bEnable = false;
				if( bEnable ) {
					// Nation쪽도 지워주고,
					if( !s_szNation.empty() ) {
						sprintf_s( szOrig, "%s\\MapData_%s\\Grid\\%s", argv[1], s_szNation.c_str(), szMapName.c_str() );
						if( PathIsDirectory( szOrig ) != FALSE ) {
							DeleteFolder( szOrig, true );
						}
					}
					// 기본폴더것도 지워준다.
					sprintf_s( szOrig, "%s\\MapData\\Grid\\%s", argv[1], szMapName.c_str() );
					if( PathIsDirectory( szOrig ) != FALSE ) {
						DeleteFolder( szOrig, true );
					}
				}
			}
			else {
				if( !szMapName.empty() )
					// IncludeBuild에 포함되는 맵 이름을 쭉 모은다.
					s_mapIncludeBuildMapName[szMapName] = "";
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Grid 폴더를 열어서 하위폴더들을 쭉 뒤져보면서, IncludeBuild에 포함안된 맵들을 다 지운다.
	// Nation쪽도 체크하고,
	if( !s_szNation.empty() ) {
		sprintf_s( szOrig, "%s\\MapData_%s\\Grid", argv[1], s_szNation.c_str() );
		if( PathIsDirectory( szOrig ) != FALSE ) {
			CheckGridFolder( szOrig );
		}
	}
	// 기본폴더도 체크한다.
	sprintf_s( szOrig, "%s\\MapData\\Grid", argv[1] );
	if( PathIsDirectory( szOrig ) != FALSE ) {
		CheckGridFolder( szOrig );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////

	FILE *fp = NULL;
	fp = fopen( szResultFile, "wt" );
	fprintf( fp, "Completed!" );
	fclose( fp );

	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	return Run( argc, argv );
}


void PrintError( const char *szString, ... )
{
	va_list list;
	char szTemp[65535];

	va_start( list, szString );
	vsprintf_s( szTemp, szString, list );
	printf( szTemp );
	FILE *fp;
	fopen_s( &fp, g_szErrorFileName, "at" );
	if( fp ) {
		fprintf_s( fp, szTemp );
		fclose(fp);
	}
	s_pDefaultOutputDebugFunc( szTemp );
	va_end( list );
}

void DeleteFolder( const char *pFolder, bool bDeleteReadOnly )
{
	char szFindFile[ _MAX_PATH ];
	WIN32_FIND_DATA FindData;
	HANDLE hFind;

	strcpy_s( szFindFile, _MAX_PATH, pFolder );
	strcat_s( szFindFile, _MAX_PATH, "\\*.*" );
	hFind = FindFirstFile( szFindFile, &FindData );
	while( hFind != INVALID_HANDLE_VALUE )
	{
		// svn폴더 검사.
		bool bSvnFolder = false;
		if( strlen(FindData.cFileName) == 4 && FindData.cFileName[ 0 ] == '.' && FindData.cFileName[ 1 ] == 's' && FindData.cFileName[ 2 ] == 'v' && FindData.cFileName[ 3 ] == 'n' )
			bSvnFolder = true;

		if( FindData.cFileName[ 0 ] != '.' || bSvnFolder )
		{
			char szFullName[ _MAX_PATH ];
			strcpy_s( szFullName, _MAX_PATH, pFolder );
			strcat_s( szFullName, _MAX_PATH, "\\" );
			strcat_s( szFullName, _MAX_PATH, FindData.cFileName );
			if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				DeleteFolder( szFullName, bDeleteReadOnly );
			}
			else
			{
				if( bDeleteReadOnly ) SetFileAttributes( szFullName, FILE_ATTRIBUTE_NORMAL );
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

void CheckGridFolder( const char *pFolder )
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
			if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( s_mapIncludeBuildMapName.find( FindData.cFileName ) == s_mapIncludeBuildMapName.end() )
				{
					char szFullName[ _MAX_PATH ];
					strcpy_s( szFullName, _MAX_PATH, pFolder );
					strcat_s( szFullName, _MAX_PATH, "\\" );
					strcat_s( szFullName, _MAX_PATH, FindData.cFileName );
					DeleteFolder( szFullName, true );
				}
			}
		}
		if( !FindNextFile( hFind, &FindData ) )
		{
			FindClose( hFind );
			break;
		}
	}
}