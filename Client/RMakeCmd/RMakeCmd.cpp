// RMakeCmd.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <Shlwapi.h>
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include "DNTableFile.h"
#include "SundriesFunc.h"
#include "MiniDump.h"
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <direct.h>
#include "zip.h"
#include "iowin32.h"
#include "Zipper.h"

bool EmptyTargetPath( const char *szPath );
bool CopyFolder( const char *szPath, const char *szTargetPath, bool bSubFolder = true );
bool CopyGrid( const char *szPath, const char *szTargetPath, char *szGridName );
void LoadConfig( const char *szFileName );
void GetDepthPath( const char *szPath, char *szBuf, int nDepth );
int GetDepthPathCount( const char *szPath );
void _GetLastPathName( char *szBuf, const char *szPath );
int MakeDir( char *szBuf );
void PrintError( const char *szString, ... );

std::string s_szNation;
std::map<std::string, std::string> s_szMapIgnoreFileList;
char s_cMakeType = -1;

uLong filetime(
			   const char *f,                /* name of file to get info on */
			   tm_zip *tmzip,             /* return value: access, modific. and creation times */
			   uLong *dt              /* dostime */
			   )
{
	int ret = 0;
	{
		FILETIME ftLocal;
		HANDLE hFind;
		WIN32_FIND_DATA  ff32;

		hFind = FindFirstFile(f,&ff32);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
			FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
			FindClose(hFind);
			ret = 1;
		}
	}
	return ret;
}
char g_szErrorFileName[512];
int Run( int argc, _TCHAR* argv[] )
{	
	if( argc < 4 ) {
		printf( "Argument\nRMakeCmd [Original Resource Path] [Target Resource Path] [/C/S(L,M,V,G)] [Nation(null)] [/zip(/unzip)] /revision:[Number]\n" );
		return -1;
	}
	////////////////////////
	char szResultFile[512] = { 0, };
	sprintf_s( szResultFile, "%s\\rmakeresult.txt", argv[2] );
	SetFileAttributes( szResultFile, FILE_ATTRIBUTE_NORMAL );
	DeleteFile( szResultFile );

	sprintf_s( g_szErrorFileName, "%s\\rmakeerror.txt", argv[2] );
	SetFileAttributes( g_szErrorFileName, FILE_ATTRIBUTE_NORMAL );
	DeleteFile( g_szErrorFileName );
	///////////////////////

	if( argc == 4 ) {
		s_szNation = "";
	}
	else {
		s_szNation = argv[4];
		if( _stricmp( argv[4], "null" ) == NULL ) s_szNation = "";
	}

	if( _stricmp( argv[3], "/c" ) == NULL ) s_cMakeType = 0;
	else if( _stricmp( argv[3], "/sl" ) == NULL ) s_cMakeType = 1;
	else if( _stricmp( argv[3], "/sm" ) == NULL ) s_cMakeType = 2;
	else if( _stricmp( argv[3], "/sv" ) == NULL ) s_cMakeType = 3;
	else if( _stricmp( argv[3], "/sg" ) == NULL ) s_cMakeType = 4;
	else {
		PrintError( "Invalid Make Resource Type\n" );
		return -1;
	}

	// 압축 채크.
	bool bCompress = false;
	char szCompressFileName[512] = { 0, };
	if( argc >= 6 ) {
		if( _stricmp( argv[5], "/zip" ) == NULL ) {
			bCompress = true;
			switch( s_cMakeType ) {
				case 0: sprintf_s( szCompressFileName, "DragonNest.zip" ); break;
				case 1: sprintf_s( szCompressFileName, "LoginRes.zip" ); break;
				case 2: sprintf_s( szCompressFileName, "MasterRes.zip" ); break;
				case 3: sprintf_s( szCompressFileName, "VillageRes.zip" ); break;
				case 4: sprintf_s( szCompressFileName, "GameRes.zip" ); break;
			}
		}
	}
	bool bMakeRevision = false;
	int nRevision = 0;
	if( argc >= 7 ) {
		char szTemp[512] = { 0, };
		sprintf_s( szTemp, argv[6] );
		_strlwr_s( szTemp );
		if( strstr( szTemp, "/revision:" ) ) {
			bMakeRevision = true;
			char *pPtr = _GetParsePipe( 1, szTemp, ':' );
			nRevision = atoi( pPtr );
		}
	}

	// Original 체크
	if( PathIsDirectory( argv[1] ) == FALSE ) {
		PrintError( "Invalid Original Resource Path\n" );
		return -1;
	}

	// Target 체크
	bool bCreateTargetPath = false;
	BOOL bResult = PathIsDirectory( argv[2] );
	if( PathIsDirectory( argv[2] ) == FALSE ) {
		/*
		if( _mkdir( argv[2] ) != 0 ) {
		PrintError( "Invalid Target Resource Path\n" );
		return -1;
		}
		else bCreateTargetPath = true;
		*/
		char szBuf[512];
		int nDepth = GetDepthPathCount( argv[2] );
		for( int i=nDepth; i>=0; i-- ) {
			memset( szBuf, 0, sizeof(szBuf) );
			GetDepthPath( argv[2], szBuf, i );
			if( PathIsDirectory( szBuf ) == FALSE ) {
				if( MakeDir( szBuf ) != 0 ) {
					PrintError( "Invalid Target Resource Path\n" );
					return -1;
				}
			}
		}
		bCreateTargetPath = true;
	}

	// Target Resource Path 삭제
	if( !bCreateTargetPath ) {
		bool bResult = EmptyTargetPath( argv[2] );
		if( !bResult ) {
			PrintError( "Can't Empty Target Path(1)\n" );
			return -1;
		}
		if( PathIsDirectory( argv[2] ) == FALSE ) {
			if( MakeDir( argv[2] ) != 0 ) { 
				PrintError( "Can't Create Target Path(2)\n" );
				return -1;
			}
		}
	}

	// 무시해야 할 글로벌 파일 등록

	char szMouleName[512] = { 0, };
	char szConfigFileName[512] = { 0, };

	GetModuleFileName( GetModuleHandle( NULL ), szMouleName, 512 );
	_GetPath( szConfigFileName, szMouleName );
	sprintf_s( szMouleName, "%srmakecmd.cfg", szConfigFileName );

	LoadConfig( szMouleName );
	/*
	s_szMapIgnoreFileList.insert( make_pair( "Thumbs.db", "" ) );
	s_szMapIgnoreFileList.insert( make_pair( "lpk.dll", "" ) );
	s_szMapIgnoreFileList.insert( make_pair( "midimap.dll", "" ) );
	s_szMapIgnoreFileList.insert( make_pair( ".svn", "" ) );
	*/

	// 일단 Resource 쪽은 걸러내지 말고 통째로 복사하기로 한다.
	// 나중에 코드 내에서 이름 박아서 사용하는 Resource 파일들 없에고 따로 태이블 두게 되면
	// 그때 나머지 태이블들 파싱해서 복사하도록 하자.( 이것두 하면 꽤 많이 걸러질 듯 함 )
	char szOrig[512];
	char szTarget[512];
	sprintf_s( szOrig, "%s\\Resource", argv[1] );
	sprintf_s( szTarget, "%s\\Resource", argv[2] );
	if( CopyFolder( szOrig, szTarget ) == false ) {
		PrintError( "Copy Error - Resource\n" );
		return -1;
	}
	// 국가가 있으면 복사
	if( !s_szNation.empty() ) {
		sprintf_s( szOrig, "%s\\Resource_%s", argv[1], s_szNation.c_str() );
		if( PathIsDirectory( szOrig ) != FALSE ) {
			if( CopyFolder( szOrig, szTarget ) == false ) {
				PrintError( "Copy Error - Resource_Nation\n" );
				return -1;
			}
		}
	}


	if( s_szMapIgnoreFileList.find( "/mapdata" ) == s_szMapIgnoreFileList.end() ) {
		// MapData 복사
		sprintf_s( szOrig, "%s\\MapData", argv[1] );
		sprintf_s( szTarget, "%s\\MapData", argv[2] );
		if( PathFileExists( szOrig ) ) {
			if( CopyFolder( szOrig, szTarget, false ) == false ) {
				PrintError( "Copy Error - Map Infomation\n" );
				return -1;
			}
		}
		// Envi
		sprintf_s( szOrig, "%s\\MapData\\Resource\\Envi", argv[1] );
		sprintf_s( szTarget, "%s\\MapData\\Resource\\Envi", argv[2] );
		if( PathFileExists( szOrig ) ) {
			if( CopyFolder( szOrig, szTarget ) == false ) {
				PrintError( "Copy Error - MapData_Envi\n" );
				return -1;
			}
		}
		// Prop
		sprintf_s( szOrig, "%s\\MapData\\Resource\\Prop", argv[1] );
		sprintf_s( szTarget, "%s\\MapData\\Resource\\Prop", argv[2] );
		if( PathFileExists( szOrig ) ) {
			if( CopyFolder( szOrig, szTarget ) == false ) {
				PrintError( "Copy Error - MapData_Prop\n" );
				return -1;
			}
		}
		// Sound
		sprintf_s( szOrig, "%s\\MapData\\Resource\\Sound", argv[1] );
		sprintf_s( szTarget, "%s\\MapData\\Resource\\Sound", argv[2] );
		if( PathFileExists( szOrig ) ) {
			if( CopyFolder( szOrig, szTarget ) == false ) {
				PrintError( "Copy Error - MapData_Sound\n" );
				return -1;
			}
		}
		// Tile
		sprintf_s( szOrig, "%s\\MapData\\Resource\\Tile", argv[1] );
		sprintf_s( szTarget, "%s\\MapData\\Resource\\Tile", argv[2] );
		if( PathFileExists( szOrig ) ) {
			if( CopyFolder( szOrig, szTarget ) == false ) {
				PrintError( "Copy Error - MapData_Tile\n" );
				return -1;
			}
		}
		// Trigger
		sprintf_s( szOrig, "%s\\MapData\\Resource\\Trigger", argv[1] );
		sprintf_s( szTarget, "%s\\MapData\\Resource\\Trigger", argv[2] );
		if( PathFileExists( szOrig ) ) {
			if( CopyFolder( szOrig, szTarget ) == false ) {
				PrintError( "Copy Error - MapData_Trigger\n" );
				return -1;
			}
		}
		// 국가별 리소스 덮어쒸운다.
		if( !s_szNation.empty() ) {
			// Envi
			sprintf_s( szOrig, "%s\\MapData_%s\\Resource\\Envi", argv[1], s_szNation.c_str() );
			if( PathIsDirectory( szOrig ) != FALSE ) {
				sprintf_s( szTarget, "%s\\MapData\\Resource\\Envi", argv[2] );
				if( CopyFolder( szOrig, szTarget ) == false ) {
					PrintError( "Copy Error - MapData_Envi_Nation\n" );
					return -1;
				}
			}
			// Prop
			sprintf_s( szOrig, "%s\\MapData_%s\\Resource\\Prop", argv[1], s_szNation.c_str() );
			if( PathIsDirectory( szOrig ) != FALSE ) {
				sprintf_s( szTarget, "%s\\MapData\\Resource\\Prop", argv[2] );
				if( CopyFolder( szOrig, szTarget ) == false ) {
					PrintError( "Copy Error - MapData_Prop_Nation\n" );
					return -1;
				}
			}
			// Sound
			sprintf_s( szOrig, "%s\\MapData_%s\\Resource\\Sound", argv[1], s_szNation.c_str() );
			if( PathIsDirectory( szOrig ) != FALSE ) {
				sprintf_s( szTarget, "%s\\MapData\\Resource\\Sound", argv[2] );
				if( CopyFolder( szOrig, szTarget ) == false ) {
					PrintError( "Copy Error - MapData_Sound_Nation\n" );
					return -1;
				}
			}
			// Tile
			sprintf_s( szOrig, "%s\\MapData_%s\\Resource\\Tile", argv[1], s_szNation.c_str() );
			if( PathIsDirectory( szOrig ) != FALSE ) {
				sprintf_s( szTarget, "%s\\MapData\\Resource\\Tile", argv[2] );
				if( CopyFolder( szOrig, szTarget ) == false ) {
					PrintError( "Copy Error - MapData_Tile_Nation\n" );
					return -1;
				}
			}
			// Trigger
			sprintf_s( szOrig, "%s\\MapData_%s\\Resource\\Trigger", argv[1], s_szNation.c_str() );
			if( PathIsDirectory( szOrig ) != FALSE ) {
				sprintf_s( szTarget, "%s\\MapData\\Resource\\Trigger", argv[2] );
				if( CopyFolder( szOrig, szTarget ) == false ) {
					PrintError( "Copy Error - MapData_Trigger_Nation\n" );
					return -1;
				}
			}
		}


		// Grid 폴더 만들구..
		if( s_szMapIgnoreFileList.find( "/grid" ) == s_szMapIgnoreFileList.end() ) {
			sprintf_s( szTarget, "%s\\MapData\\Grid", argv[2] );
			if( PathIsDirectory( szTarget ) == FALSE ) {
				if( MakeDir( szTarget ) != 0 ) {
					PrintError( "Copy Error - Grid\n" );
					return -1;
				}
			}
			// SOX 얻어온다.
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
					if( pSox->GetFieldFromLablePtr( nItemID, "_IncludeBuild" )->GetInteger() == 0 ) continue;
					if( !szMapName.empty() ) {
						bool bExistNation = false;
						if( !s_szNation.empty() ) {
							sprintf_s( szOrig, "%s\\MapData_%s\\Grid\\%s", argv[1], s_szNation.c_str(), szMapName.c_str() );
							if( PathIsDirectory( szOrig ) != FALSE ) {
								bExistNation = true;
								sprintf_s( szOrig, "%s\\MapData_%s\\Grid", argv[1], s_szNation.c_str() );
								sprintf_s( szTarget, "%s\\MapData\\Grid", argv[2] );
								if( CopyGrid( szOrig, szTarget, (char*)szMapName.c_str() ) == false ) {
									PrintError( "Copy Error - Grid( %s )\n", szMapName.c_str() );
									return -1;
								}
							}
						}
						if( !bExistNation ) {
							sprintf_s( szOrig, "%s\\MapData\\Grid\\%s", argv[1], szMapName.c_str() );
							if( PathIsDirectory( szOrig ) != FALSE ) {
								sprintf_s( szOrig, "%s\\MapData\\Grid", argv[1] );
								sprintf_s( szTarget, "%s\\MapData\\Grid", argv[2] );
								if( CopyGrid( szOrig, szTarget, (char*)szMapName.c_str() ) == false ) {
									PrintError( "Copy Error - Grid( %s )\n", szMapName.c_str() );
									return -1;
								}
							}
						}
					}
				}
			}
		}
	}

	char szTemp[512] = { 0, };
	sprintf_s( szTemp, "%s\\ResourceRevision.txt", argv[2] );
	if( bMakeRevision ) {
		FILE *fp;
		fopen_s( &fp, szTemp, "wt" );
		if( fp ) {
			fprintf_s( fp, "Last Changed Rev: %d\n", nRevision );
			fclose(fp);
		}
	}

	if( bCompress ) {
		printf( "\n\nCompress Resource................\n" );
		CZipper Zip;
		char szTargetCompPath[512] = { 0, };
		sprintf_s( szTargetCompPath, "%s\\%s", argv[2], szCompressFileName );
		DeleteFile( szTargetCompPath );
		if( Zip.OpenZip( szTargetCompPath, false) == false ) {
			PrintError( "Compress Error!!\n" );
			return -1;
		}

		char szTemp[512] = { 0, };
		sprintf_s( szTemp, "%s\\Resource", argv[2] );
		if( PathFileExists( szTemp ) ) {
			if( Zip.AddFolderToZip(szTemp, false) == false ) {
				PrintError( "Compress Error!!\n" );
				return -1;
			}
		}
		sprintf_s( szTemp, "%s\\MapData", argv[2] );
		if( PathFileExists( szTemp ) ) {
			if( Zip.AddFolderToZip(szTemp, false) == false ) {
				PrintError( "Compress Error!!\n" );
				return -1;
			}
		}
		sprintf_s( szTemp, "%s\\ResourceRevision.txt", argv[2] );
		if( PathFileExists( szTemp ) ) {
			if( Zip.AddFileToZip( szTemp, false ) == false ) {
				PrintError( "Compress Error!!\n" );
				return -1;
			}
		}

		/*
		if( CZipper::ZipFolder( argv[2], false ) == false ) {
			PrintError( "Compress Error!!\n" );
			return -1;
		}
		*/
	}

	////////////////////////
	FILE *fp;
	fopen_s( &fp, szResultFile, "w" );
	fclose(fp);
	///////////////////////
}

int _tmain(int argc, _TCHAR* argv[])
{
	return Run( argc, argv );
}


bool EmptyTargetPath( const char *szPath )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szFindFile[512];
	char szTemp[512];
	char szTemp2[512];
	bool bResult = true;

	sprintf_s( szFindFile, "%s\\*.*", szPath );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
			sprintf_s( szTemp, "%s\\%s", szPath, FindFileData.cFileName );
			SetFileAttributes( szTemp, FILE_ATTRIBUTE_NORMAL );
			sprintf_s( szTemp2, "%s", szTemp );
			_strlwr_s( szTemp2 );
			if( strstr( szTemp2, "resourcerevision.txt" ) == NULL ) {
				if( DeleteFile( szTemp ) == FALSE ) bResult = false;
			}
		}
		if( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
			if( strcmp( FindFileData.cFileName, "." ) && strcmp( FindFileData.cFileName, ".." ) && strcmp( FindFileData.cFileName, ".svn" ) && strcmp( FindFileData.cFileName, "_svn" ) ) {
				if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ) {
					sprintf_s( szTemp, "%s\\%s", szPath, FindFileData.cFileName );
					if( EmptyTargetPath( szTemp ) == false ) bResult = false;
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;

	}
	/*
	if( strstr( szPath, ".svn" ) == NULL && strstr( szPath, "_svn" ) == NULL ) {
		if( RemoveDirectory( szPath ) == FALSE ) bResult = false;
	}
	*/

	FindClose( hFind );
	return bResult;
}

void GetDepthPath( const char *szPath, char *szBuf, int nDepth )
{
	if( nDepth == 0 ) {
		strcpy( szBuf, szPath );
		return;
	}

	int nLength = (int)strlen(szPath);
	int nCount = 0;
	for( int i=nLength-1; i>=0; i-- ) {
		if( szPath[i] == '\\' ) {
			nCount++;
			if( nCount == nDepth ) {
				strncpy( szBuf, szPath, i );
				szBuf[i] = 0;
				return;
			}
		}
	}
}

int GetDepthPathCount( const char *szPath )
{
	int nLength = (int)strlen(szPath);
	int nCount = 0;
	for( int i=nLength-1; i>=0; i-- ) {
		if( szPath[i] == '\\' ) {
			nCount++;
		}
	}
	return nCount;
}

bool CopyFolder( const char *szPath, const char *szTargetPath, bool bSubFolder )
{
	if( PathIsDirectory( szPath ) == FALSE ) return false;
	
	char szBuf[512];
	char szFindFile[512];
	char szTemp[512];
	char szTemp2[512];

	memset( szBuf, 0, sizeof(szBuf) );
	memset( szTemp, 0, sizeof(szTemp) );
	_GetLastPathName( szBuf, szPath );
	_strlwr_s( szBuf );
	sprintf_s( szTemp, "/%s", szBuf );
	if( s_szMapIgnoreFileList.find( szTemp ) != s_szMapIgnoreFileList.end() ) return true;


	int nDepth = GetDepthPathCount( szTargetPath );
	for( int i=nDepth; i>=0; i-- ) {
		memset( szBuf, 0, sizeof(szBuf) );
		GetDepthPath( szTargetPath, szBuf, i );
		if( PathIsDirectory( szBuf ) == FALSE ) {
			if( MakeDir( szBuf ) != 0 ) return false;
			printf( "Make Folder : %s\n", szBuf );
		}
	}

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	bool bResult = true;

	sprintf_s( szFindFile, "%s\\*.*", szPath );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ) {
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
				memset( szTemp2, 0, sizeof(szTemp2) );
				sprintf_s( szTemp2, FindFileData.cFileName );
				_strlwr_s( szTemp2 );
				if( s_szMapIgnoreFileList.find( szTemp2 ) == s_szMapIgnoreFileList.end() ) {

					memset( szTemp, 0, sizeof(szTemp) );
					_GetExt( szTemp, szTemp2 );
					bool bValid = true;
					if( strlen(szTemp) > 0 ) {
						sprintf_s( szTemp2, "*.%s", szTemp );
						if( s_szMapIgnoreFileList.find( szTemp2 ) != s_szMapIgnoreFileList.end() ) bValid = false;
					}

					if( bValid ) {
						sprintf_s( szTemp, "%s\\%s", szPath, FindFileData.cFileName );
						sprintf_s( szTemp2, "%s\\%s", szTargetPath, FindFileData.cFileName );
						_strlwr_s( szTemp2 );
						CopyFile( szTemp, szTemp2, FALSE );
						printf( "Copy File : %s -> %s\n", szTemp, szTemp2 );
						SetFileAttributes( szTemp2, FILE_ATTRIBUTE_NORMAL );
					}
				}
			}
			if( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) {
				memset( szTemp2, 0, sizeof(szTemp2) );
				sprintf_s( szTemp2, "/%s", FindFileData.cFileName );
				_strlwr_s( szTemp2 );
				if( s_szMapIgnoreFileList.find( szTemp2 ) == s_szMapIgnoreFileList.end() ) {
					if( strcmp( FindFileData.cFileName, "." ) && strcmp( FindFileData.cFileName, ".." ) ) {
						if( bSubFolder ) {
							sprintf_s( szTemp, "%s\\%s", szPath, FindFileData.cFileName );
							sprintf_s( szTemp2, "%s\\%s", szTargetPath, FindFileData.cFileName );
							if( PathIsDirectory( szTemp2 ) == FALSE ) {
								if( MakeDir( szTemp2 ) != 0 ) bResult = false;
								else {
									CopyFolder( szTemp, szTemp2 );
								}
							}
							else CopyFolder( szTemp, szTemp2 );
						}
					}
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;

	}

	FindClose( hFind );
	return bResult;
}

bool CopyGrid( const char *szPath, const char *szTargetPath, char *szGridName )
{
	char szTemp[512];
	char szTemp2[512];

	sprintf_s( szTemp, "%s\\%s", szPath, szGridName );
	sprintf_s( szTemp2, "%s\\%s", szTargetPath, szGridName );

	if( CopyFolder( szTemp, szTemp2 ) == false ) return false;
	sprintf_s( szTemp2, "%s\\%s\\GridInfo.ini", szTargetPath, szGridName );

	FILE *fp;
	fopen_s( &fp, szTemp2, "rb" );
	if( fp == NULL ) return false;
	DWORD dwGridX, dwGridY;
	fseek( fp, 64, SEEK_SET );
	fread( &dwGridX, sizeof(DWORD), 1, fp );
	fread( &dwGridY, sizeof(DWORD), 1, fp );
	fclose(fp);

	for( DWORD j=0; j<dwGridY; j++ ) {
		for( DWORD i=0; i<dwGridX; i++ ) {
			sprintf_s( szTemp2, "%s\\%s\\%d_%d\\Thumbnail.tga", szTargetPath, szGridName, i, j );
			SetFileAttributes( szTemp2, FILE_ATTRIBUTE_NORMAL );
			DeleteFile( szTemp2 );
		}
	}
	std::vector<std::string> szVecList;
	sprintf_s( szTemp2, "%s\\%s", szTargetPath, szGridName );
	FindFileListInDirectory( szTemp2, "*.dds", szVecList, false, false );

	char szName[512];
	char szTempGridName[512];
	for( DWORD i=0; i<szVecList.size(); i++ ) {
		memset( szName, 0, sizeof(szName) );
		memset( szTempGridName, 0, sizeof(szTempGridName) );

		strcpy_s( szTempGridName, szGridName );
		_GetFileName( szName, szVecList[i].c_str() );
		_strlwr_s( szName );
		_strlwr_s( szTempGridName );

		if( strstr( szName, szTempGridName ) == NULL ) {
			sprintf_s( szTemp2, "%s\\%s\\%s", szTargetPath, szGridName, szVecList[i].c_str() );
			DeleteFile( szTemp2 );
		}
	}
	return true;
}

void LoadConfig( const char *szFileName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rt" );
	if( fp == NULL ) return;

	char szTemp[1024];
	char cFlag = -1;
	while( !feof(fp) ) {
		memset( szTemp, 0, sizeof(szTemp) );
		fscanf_s( fp, "%s", szTemp, 1024 );
		if( _stricmp( szTemp, "[Common]" ) == NULL ) cFlag = 0;
		else if( _stricmp( szTemp, "[Client]" ) == NULL ) cFlag = 1;
		else if( _stricmp( szTemp, "[Server]" ) == NULL ) cFlag = 2;
		else if( _stricmp( szTemp, "[Login]" ) == NULL ) cFlag = 3;
		else if( _stricmp( szTemp, "[Master]" ) == NULL ) cFlag = 4;
		else if( _stricmp( szTemp, "[Village]" ) == NULL ) cFlag = 5;
		else if( _stricmp( szTemp, "[Game]" ) == NULL ) cFlag = 6;
		else {
			if( strlen( szTemp ) == 0 ) continue;
			_strlwr_s( szTemp );
			switch( cFlag ) {
				case 0:
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				case 1:
					if( s_cMakeType != 0 ) break;
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				case 2:
					if( s_cMakeType < 1 ) break;
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				case 3:
					if( s_cMakeType != 1 ) break;
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				case 4:
					if( s_cMakeType != 2 ) break;
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				case 5:
					if( s_cMakeType != 3 ) break;
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				case 6:
					if( s_cMakeType != 4 ) break;
					s_szMapIgnoreFileList.insert( make_pair( szTemp, "" ) );
					break;
				default:
					continue;
			}
		}
	}

	fclose(fp);
}

void _GetLastPathName( char *szBuf, const char *szPath )
{
	int nLength = (int)strlen(szPath);
	int nOffset = 0;
	for( int i=nLength - 1; i>=0; i-- ) {
		if( szPath[i] == '\\' || szPath[i] == ':' ) {
			memcpy( szBuf, szPath + i + 1, nOffset );
			szBuf[nOffset] = 0;
			break;
		}
//		szBuf[nOffset] = szPath[i];
		nOffset++;
	}
}

int MakeDir( char *szBuf )
{
	char szTemp[512] = { 0, };
	sprintf_s( szTemp, szBuf );
	_strlwr_s( szTemp );
	return _mkdir( szTemp );
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