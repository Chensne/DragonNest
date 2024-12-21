#include "stdafx.h"

#include <zlib.h>
#include <utility>
#include <algorithm>
#include "EtFileSystem.h"
#include "SundriesFunc.h"
#include "StringUtil.h"
#ifdef _x86
#include <lz4.h> //lz4!!
#endif
#include "Crypt.h"
/*
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
*/

bool g_bUseCompress= true;

using namespace std;
typedef list<CEtFileHandle>		 		CValueFileHandleList;
typedef list<CEtFileHandle>::iterator	 CValueFileHandleList_It;


//-------------------------------------------------------------------------------------
const DWORD ET_SEED_CNT = 50;
const DWORD ET_ENCRYPT_SEED[ET_SEED_CNT] =
{
	0x67184865, 0x722c3d4a, 0x6d165f69, 0xa126415a, 0x15122e0b, 
	0xf39f397e, 0x49443001, 0x54154d4d, 0x4d072d39, 0x26426664, 

	0x767a5d70, 0x1e3b1245, 0x6b631a6e, 0x324e7f7f, 0xd36b2622, 
	0x57560b30, 0x22237501, 0x5c6b584b, 0x3b5f1a3e, 0x7d5f793a, 

	0x4d5e314c, 0x1c132e49, 0x7e426636, 0x54152c60, 0x4059083e, 
	0x73127912, 0x49362658, 0x7013577b, 0x16184a69, 0x40133c68, 

	0x4823335d, 0x3c5c6057, 0x946a2f0f, 0x5d365442, 0x2c467579, 
	0x286c2f3c, 0x267d223a, 0x6119485e, 0x7140326b, 0x36497f03, 

	0x506b1844, 0xcf6b1954, 0x78285f0e, 0x5d112b24, 0x5f4c5a6a, 
	0x97181220, 0x2e1a6d67, 0x3a0e0146, 0x69455760, 0x25591937,
};

//-------------------------------------------------------------------------------------

CEtFileSystemLog g_DummyLog;

CEtFileHandle::CEtFileHandle()
{
	memset( &m_FileInfo, 0, sizeof( SPackingFileInfo ) );
	m_pData = NULL;
	m_dwCurPos = 0;
}

CEtFileHandle::~CEtFileHandle()
{
	SAFE_DELETEA( m_pData );
}

DWORD CEtFileHandle::Read(void* pBuff, DWORD nSize)
{
	if(!m_pData)
		return 0;

	if(m_FileInfo.dwOriginalSize <= m_dwCurPos)
		return 0;

	if(m_FileInfo.dwOriginalSize < m_dwCurPos+nSize) 
		nSize = m_FileInfo.dwOriginalSize-m_dwCurPos;

	memcpy(pBuff,m_pData+m_dwCurPos,nSize);
	m_dwCurPos += nSize;

	return nSize;
}

int CEtFileHandle::Seek(int lOffset, int iOrigin)
{
	if(!m_pData)
		return -1;

	DWORD lPos;
	switch(iOrigin)
	{
	case SEEK_SET: lPos = lOffset;			 break;
	case SEEK_CUR: lPos = lOffset+m_dwCurPos; break;
	case SEEK_END: lPos = m_FileInfo.dwOriginalSize+lOffset;	 break;
	default:								 return -1;
	}

	if(lPos > m_FileInfo.dwOriginalSize)
		return -1;

	m_dwCurPos = lPos;

	return m_dwCurPos;
}

#include <ImageHlp.h>
bool CEtFileHandle::ExportFile()
{
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	char szName[ _MAX_PATH ];

	_GetFullFileName( szName, _countof(szName), m_FileInfo.szFileName );

    std::string full(".");
    full += m_FileInfo.szFileName;
    std::string path(".");
    path += m_FileInfo.szFileName;
    auto index = path.find_last_of("\\");
    if (index != std::string::npos)
    {
        path.erase(index);
        path += "\\";
    }

    MakeSureDirectoryPathExists(path.c_str());



	DWORD dwAttr = ::GetFileAttributes( _T(full.c_str()) );
	if( dwAttr & FILE_ATTRIBUTE_READONLY )
	{
		dwAttr &= ~FILE_ATTRIBUTE_READONLY;
		::SetFileAttributes( _T(full.c_str()), dwAttr );
	}

	hFile = CreateFile(full.c_str(), GENERIC_READ|GENERIC_WRITE,  0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	DWORD nWriteSize = 0;

	if( WriteFile(hFile, m_pData, m_FileInfo.dwOriginalSize, &nWriteSize, NULL) == false )
	{
		CloseHandle(hFile);
		return false;
	}

	CloseHandle(hFile);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//	CEtPackingFile
//////////////////////////////////////////////////////////////////////////////////////////////////////
CEtPackingFile::CEtPackingFile()
{

	m_hFile = INVALID_HANDLE_VALUE;
	ZeroMemory(m_strCurDir, sizeof(m_strCurDir));
	memset( &m_PackingFileHeader, 0, sizeof( SPackingFileHeader ) );
	m_PackingFileHeader.dwFileInfoOffset = PACKING_HEADER_RESERVED + sizeof( SPackingFileHeader );
	m_bIsReadOnly = true;
	m_pLog = &g_DummyLog;

	CloseFileSystem();
}

CEtPackingFile::~CEtPackingFile()
{
	CloseFileSystem();
}


LONGLONG CEtPackingFile::_Seek(HANDLE hFile, LONGLONG nDistance, DWORD SeekType)
{
	if( hFile == INVALID_HANDLE_VALUE )
		return -1;

	LARGE_INTEGER li;

	li.QuadPart = nDistance;
	li.LowPart = SetFilePointer (hFile, li.LowPart, &li.HighPart, SeekType);

	if(li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		li.QuadPart = -1;
	}

	return li.QuadPart;
}


bool CEtPackingFile::_Read(HANDLE hFile, IN void* pBuffer, DWORD ToReadSize)
{
	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	DWORD nReadSize = 0;
	if( ReadFile(hFile, pBuffer, ToReadSize, &nReadSize, NULL ) == false )
		return false;

	if( ToReadSize > 0 && nReadSize < 1 )
		return false;

	return true;
}


bool CEtPackingFile::_Write(HANDLE hFile, IN void* pBuffer, DWORD ToWriteSize)
{
	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	DWORD nWriteSize = 0;
	if( WriteFile(hFile, pBuffer, ToWriteSize, &nWriteSize, NULL ) == false )
		return false;

	return true;
}

void CEtPackingFile::SetCurDir(const char* pTok)
{
	if( strcmp( pTok, "." ) == 0 ) 
		return;

	if( strcmp( pTok, ".." ) == 0 )
	{
		if(strlen(m_strCurDir) == 1 && m_strCurDir[0] == '\\')
			return;

		char strCurDir[_MAX_PATH] = { 0, };
		strcpy_s(strCurDir,m_strCurDir);

		int iLen = (int)strlen(strCurDir);

		if(strCurDir[iLen-1] == '\\' ) strCurDir[iLen-1] = '\0';

		for(int i=0;i<_MAX_PATH;i++)
		{
			char* cp = strCurDir+(_MAX_PATH-1-i);

			if(*cp == '\\')
				break;

			*cp = '\0';
		}

		strcpy(m_strCurDir,strCurDir);
		return;
	}

	strcat(m_strCurDir,pTok);
	strcat(m_strCurDir,"\\");
}

void CEtPackingFile::GetSafeName(char* strDst,const char* strName)
{
	ZeroMemory(strDst, _MAX_FNAME);

	int iLen = (int)strlen(strName);

	if(iLen >= _MAX_FNAME) 
		iLen = _MAX_FNAME-1;

	strncpy(strDst,strName,iLen);
}

DWORD CEtPackingFile::CalcChecksum( int nFileIndex )
{
	char *pBuffer;
	int i, nSize;
	DWORD dwChecksum = 0, *pdwBuffer;

	// nSize甫 4狼 硅荐肺 官槽促..
	nSize = m_vecPackingFileInfo[ nFileIndex ].dwCompressSize / 4;
	if( m_vecPackingFileInfo[ nFileIndex ].dwCompressSize % 4 )
	{
		nSize++;
	}
	nSize *= 4;
	pBuffer = new char[ nSize ];
	memset( pBuffer, 0, nSize );
	pdwBuffer = ( DWORD * )pBuffer;
	for( i = 0; i < nSize / 4; i++ )
	{
		dwChecksum = pdwBuffer[ i ] ^ dwChecksum;
	}

	delete [] pBuffer;
	return dwChecksum;
}

void CEtPackingFile::GenerateChecksum()
{
	int i;
	int nSize = ( int )m_vecPackingFileInfo.size();

	for( i = 0; i < nSize; i++ )
	{
		if( m_vecPackingFileInfo[ i ].dwCompressSize == 0 )
		{
			continue;
		}
		m_vecPackingFileInfo[ i ].dwChecksum = CalcChecksum( i );
	}
}

void CEtPackingFile::CloseFileSystem()
{
	m_strCurDir[ 0 ] = '\0';

	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		if( ( !m_bIsReadOnly ) && ( m_PackingFileHeader.nFileCount != m_vecPackingFileInfo.size() ) )
		{
			if( m_PackingFileHeader.nVersion <= PACKING_FILE_NOCHECKSUM_VERSION )
			{
				GenerateChecksum();
				m_PackingFileHeader.bRequireHeaderWrite = true;
				m_PackingFileHeader.nVersion = PACKING_FILE_VERSION;
			}
			m_PackingFileHeader.nFileCount = ( int )m_vecPackingFileInfo.size();
			_Seek( m_hFile, 0, SEEK_SET );
			_Write( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) );
		}
		if( ( !m_bIsReadOnly ) && ( m_PackingFileHeader.bRequireHeaderWrite ) )
		{
			_Seek( m_hFile, m_PackingFileHeader.dwFileInfoOffset, SEEK_SET );
			_Write( m_hFile, &m_vecPackingFileInfo[ 0 ], ( DWORD )m_vecPackingFileInfo.size() * sizeof( SPackingFileInfo ) );

			m_PackingFileHeader.bRequireHeaderWrite = false;
			_Seek( m_hFile, 0, SEEK_SET );
			_Write( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) );
		}
	}

	m_vecPackingFileInfo.clear();
	m_mapPackingFileIndex.clear();

	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}


void
CEtPackingFile::_Encode(IN BYTE* pBuffer, int nBufSize, DWORD nCode)
{
	return;
	for ( int i = 0 ; i < nBufSize ; i+=8 )
	{
		pBuffer[i] ^= nCode; 
	}
}

void 
CEtPackingFile::_Decode(IN BYTE* pBuffer, int nBufSize, DWORD nCode)
{
	return;
	for ( int i = 0 ; i < nBufSize ; i+=8 )
	{
		pBuffer[i] ^= nCode; 
	}
}

bool CEtPackingFile::OpenFileSystem(const char* strPath, bool bReadOnly /* = false */ )
{
	CloseFileSystem();

	m_bIsReadOnly = bReadOnly;
	if( bReadOnly )
	{
		m_hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,		// 吝汗 Read 啊瓷窍霸 荐沥.
			FILE_ATTRIBUTE_NORMAL,NULL);
	}
	else
	{
		m_hFile = CreateFile(strPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
	}


	DWORD nError;
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		nError = GetLastError();
		
		return false;
	}

	// 颇老 矫胶袍 庆歹甫 肺靛茄促.
	_Read( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) );
	if( ( m_PackingFileHeader.nVersion < PACKING_FILE_NOCHECKSUM_VERSION ) || ( m_PackingFileHeader.nVersion > PACKING_FILE_VERSION ) )
	{

		return false;
	}

	// 备滚傈 颇老矫胶袍俊绰 颇老牢器 可悸捞 救甸绢 乐绢辑 颇老场俊辑 Seek栏肺 茫酒具 茄促.
	if( m_PackingFileHeader.nVersion > PACKING_FILE_NOCHECKSUM_VERSION )
	{
		// bRequireHeaderWrite 蔼捞 悸泼 蹬 乐栏搁 菩摹锭 力措肺 辆丰 救灯巴烙.. 肋给等 颇老 矫胶袍栏肺 牢沥..
		if( m_PackingFileHeader.bRequireHeaderWrite == true )
		{
			//rlkt_pak
			//MessageBoxA(NULL, FormatA(" Open File System nVersion >PACKING_FILE_NOCHECKSUM_VERSION").c_str(), "AddPackingFile", MB_OK);

			return false;
		}
		_Seek( m_hFile, m_PackingFileHeader.dwFileInfoOffset, SEEK_SET );
	}
	else
	{
		m_PackingFileHeader.dwFileInfoOffset = ( DWORD )_Seek( m_hFile, -( LONGLONG )( m_PackingFileHeader.nFileCount * sizeof( SPackingFileInfo ) ), SEEK_END );
	}
	if( m_PackingFileHeader.nFileCount )
	{
		m_vecPackingFileInfo.resize( m_PackingFileHeader.nFileCount );
		_Read( m_hFile, &m_vecPackingFileInfo[ 0 ], sizeof( SPackingFileInfo ) * m_PackingFileHeader.nFileCount );

		int i;
		for( i = 0; i < ( int )m_vecPackingFileInfo.size(); i++ )
		{
			if( m_vecPackingFileInfo[ i ].dwCompressSize > m_vecPackingFileInfo[ i ].dwAllocSize )
			{
				//rlkt_pak TEST ONLY!!
				//m_vecPackingFileInfo[i].dwAllocSize = m_vecPackingFileInfo[i].dwCompressSize;
				//MessageBoxA(NULL, FormatA("File ID: %d\nFile offset : %X read size: %X Size of Array : %d\n file count: %d\n CompressSize > DWAllocSize (%d - %d)\n\nFile:%s\nOrig size: %d\noffset : %X",i, m_PackingFileHeader.dwFileInfoOffset, sizeof(SPackingFileInfo)* m_PackingFileHeader.nFileCount, m_vecPackingFileInfo.size() , m_PackingFileHeader.nFileCount, m_vecPackingFileInfo[i].dwCompressSize, m_vecPackingFileInfo[i].dwAllocSize, m_vecPackingFileInfo[i].szFileName, m_vecPackingFileInfo[i].dwOriginalSize, m_vecPackingFileInfo[i].dwOffset).c_str(), "AddPackingFile", MB_OK);

				return false;
			}
		}
	}

	GeneratePackingMap();

	// 泅犁 叼泛配府甫 弥惑困 叼泛配府肺 官槽促.
	ChangeDir("\\");

	m_strPackingFileName = strPath;

	return true;
}

bool CEtPackingFile::NewFileSystem(const char* strPath)
{
	CloseFileSystem();

	m_hFile = CreateFile(strPath, GENERIC_READ|GENERIC_WRITE,  0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,NULL);

	DWORD nError;
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		nError = GetLastError();
		return false;
	}

	// 庆歹 沥焊 父甸扁
	memset( &m_PackingFileHeader, 0, sizeof( SPackingFileHeader ) );
	strcpy_s( m_PackingFileHeader.szHeaderString, 256, PACKING_FILE_STRING );
	m_PackingFileHeader.nVersion = PACKING_FILE_VERSION;
	m_PackingFileHeader.nFileCount = 0;
	m_PackingFileHeader.dwFileInfoOffset = PACKING_HEADER_RESERVED + sizeof( SPackingFileHeader );

	// 庆歹 扁废
	if( _Write( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) ) == false )
	{
		return false;
	}
	char cReserved[ PACKING_HEADER_RESERVED ];
	memset( cReserved, 0, PACKING_HEADER_RESERVED );
	_Write( m_hFile, cReserved, PACKING_HEADER_RESERVED );

	m_bIsReadOnly = false;

	return true;
}

bool CEtPackingFile::OptimizeFileSystem(const char* strPath)
{
	if( m_hFile == INVALID_HANDLE_VALUE) 
		return false;

	CEtPackingFile FileSystem;

	if( !FileSystem.NewFileSystem(strPath) )
		return false;

	int i;
	int nFileCount = GetFileCount();
	for( i = 0; i < nFileCount; i++ )
	{
		SPackingFileInfo *pPackingFileInfo = GetFileInfo( i );
		if( pPackingFileInfo == NULL )
		{
			continue;
		}
		CEtFileHandle *pFileHandle = OpenFile( pPackingFileInfo->szFileName );
		if( pFileHandle == NULL )
		{
			continue;
		}
		AddFile( pPackingFileInfo->szFileName, pFileHandle->m_pData, pFileHandle->m_FileInfo.dwCompressSize );
		CloseFile( pFileHandle );
	}

	return true;
}

DWORD CEtPackingFile::GetFileSystemSize()
{
	LARGE_INTEGER liSeekCur, liSeekEnd;
	liSeekCur.QuadPart = _Seek( m_hFile, 0, SEEK_CUR );
	liSeekEnd.QuadPart = _Seek( m_hFile, 0, SEEK_END );
	_Seek( m_hFile, liSeekCur.QuadPart, SEEK_SET );
	return liSeekEnd.LowPart;
}

void CEtPackingFile::SetRootPath( const char *szPath ) 
{
	m_szRootPath = szPath; 
	ToLowerA( m_szRootPath );
}


CEtFileHandle *CEtPackingFile::OpenFile( const char *strPath )
{
	if( m_hFile == INVALID_HANDLE_VALUE)
		return NULL;

	int nFindIndex = FindFile( strPath );
	if( nFindIndex == -1 )
	{
		return NULL;
	}
	SPackingFileInfo *pFileInfo = GetFileInfo( nFindIndex );
	if( pFileInfo == NULL )
	{
		return NULL;
	}
	
	_Seek( m_hFile, pFileInfo->dwOffset, SEEK_SET );

	char *pCompressBuffer = new char[ pFileInfo->dwCompressSize ];
	_Read( m_hFile, pCompressBuffer, pFileInfo->dwCompressSize );

	DWORD dwSeed = ET_ENCRYPT_SEED[ pFileInfo->dwSeedValue ];
	_Decode( ( BYTE * )pCompressBuffer, pFileInfo->dwCompressSize, dwSeed );

	DWORD dwOriginalSize = pFileInfo->dwOriginalSize;
	char *pUncompressBuffer = new char[ dwOriginalSize ];

	if( g_bUseCompress )
	{	
		
		//rlkt_CRYPT
		if (pFileInfo->dwCrypted)
		{
			Crypt::GetInstance().Decrypt((BYTE*)pCompressBuffer, pFileInfo->dwCompressSize);
		}

		//rlkt_pak 2016 added custom !!!
		DWORD dwOrigin = dwOriginalSize;
		/*int result = LZ4_decompress_fast(pCompressBuffer, pUncompressBuffer, dwOriginalSize);
		//MessageBoxA(NULL, FormatA("Decompress %s\ndwOriginalSize %d\ndwCompressSize %d\nResult %d",pFileInfo->szFileName,dwOriginalSize,pFileInfo->dwCompressSize,result).c_str(), "OK!", MB_OK);

		if (result < 0)
		{
			delete[] pCompressBuffer;
			delete[] pUncompressBuffer;
			return NULL;
		}*/
		if( uncompress( ( BYTE * )pUncompressBuffer, ( uLongf * )&dwOriginalSize, ( BYTE * )pCompressBuffer, pFileInfo->dwCompressSize ) != Z_OK )
		{
			delete [] pCompressBuffer;
			delete [] pUncompressBuffer;
			return NULL;
		}
		ASSERT( dwOrigin == dwOriginalSize );
	}
	else
	{
		CopyMemory( pUncompressBuffer, pCompressBuffer, pFileInfo->dwCompressSize );
	}


	delete [] pCompressBuffer;

	CEtFileHandle *pFileHandle = new CEtFileHandle;

	pFileHandle->m_FileInfo = *pFileInfo;
	pFileHandle->m_pData = pUncompressBuffer;
	pFileHandle->m_dwCurPos = 0;

	return pFileHandle;
}

CEtFileHandle *CEtPackingFile::OpenOnly( const char *strPath )
{
	if( m_hFile == INVALID_HANDLE_VALUE)
		return NULL;

	int nFindIndex = FindFile( strPath );
	if( nFindIndex == -1 )
	{
		return NULL;
	}
	SPackingFileInfo *pFileInfo = GetFileInfo( nFindIndex );
	if( pFileInfo == NULL )
	{
		return NULL;
	}

	_Seek( m_hFile, pFileInfo->dwOffset, SEEK_SET );

	char *pCompressBuffer = new char[ pFileInfo->dwCompressSize ];
	_Read( m_hFile, pCompressBuffer, pFileInfo->dwCompressSize );

	CEtFileHandle *pFileHandle = new CEtFileHandle;

	pFileHandle->m_FileInfo = *pFileInfo;
	pFileHandle->m_pData = pCompressBuffer;
	pFileHandle->m_dwCurPos = 0;

	return pFileHandle;
}

void CEtPackingFile::_Tokenize( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters /* = " " */ )
{
	// 盖 霉 臂磊啊 备盒磊牢 版快 公矫
	std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 备盒磊啊 酒囱 霉 臂磊甫 茫绰促
	std::string::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::string::npos != pos || std::string::npos != lastPos )
	{
		// token阑 茫疽栏聪 vector俊 眠啊茄促
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 备盒磊甫 顿绢逞绰促.  "not_of"俊 林狼窍扼
		lastPos = str.find_first_not_of( delimiters, pos );
		// 促澜 备盒磊啊 酒囱 臂磊甫 茫绰促
		pos = str.find_first_of( delimiters, lastPos );
	}
}


// str 巩磊凯 吝俊辑 szOld 啊 乐栏搁 傈何 szNew 肺 函版茄促.
void CEtPackingFile::_AllReplace( std::string& str, std::string& szOld, std::string& szNew )
{
	while (true)
	{
		std::string::size_type npos = str.find(szOld);
		if( npos < str.size() )
		{
			str.replace(npos, szOld.size(), szNew);
		}
		else
			break;
	}
}

int CEtPackingFile::FindFile( const char *pFileName )
{
	char szName[ _MAX_PATH ];
	_GetFullFileName( szName, _countof(szName), pFileName );
	_strlwr( szName );
	TrimString( szName );

	INT64 ulHashCode = GetHashCode64( szName );
	std::map< INT64, int >::iterator it = m_mapPackingFileIndex.find( ulHashCode );
	while( it != m_mapPackingFileIndex.end() )
	{
		char szFindName[ _MAX_PATH ];
		DWORD dwIndex =  it->second;
		if( m_vecPackingFileInfo[ dwIndex ].dwCompressSize != 0 )
		{
			_GetFullFileName( szFindName, _countof(szFindName), m_vecPackingFileInfo[ dwIndex ].szFileName );
			if( _stricmp( szName, szFindName ) == 0 )
			{
				char szPath[ _MAX_PATH ];
				_GetPath( szPath, _countof(szPath), pFileName );
				_strlwr( szPath );
				TrimString( szPath );
				if( szPath[ 0 ] == 0 )
				{
					return dwIndex;
				}
				else
				{
					char szFindPath[ _MAX_PATH ];
					_GetPath( szFindPath, _countof(szFindPath), m_vecPackingFileInfo[ dwIndex ].szFileName );
					if( _stricmp( szPath, szFindPath ) == 0 )
					{
						return dwIndex;
					}
				}
			}
		}
		ulHashCode++;
		it = m_mapPackingFileIndex.find( ulHashCode );
	}

	return -1;
}

void CEtPackingFile::AddPackingMap( int nFileInfoIndex )
{
	if( ( nFileInfoIndex < 0 ) || ( nFileInfoIndex >= ( int )m_vecPackingFileInfo.size() ) )
	{
		return;
	}

	char szName[ _MAX_PATH ], szPath[ _MAX_PATH ];
	_GetFullFileName( szName, _countof(szName), m_vecPackingFileInfo[ nFileInfoIndex ].szFileName );
	_strlwr( szName );
	TrimString( szName );
	_GetPath( szPath, _countof(szPath), m_vecPackingFileInfo[ nFileInfoIndex ].szFileName );

	INT64 ulHashCode = GetHashCode64( szName );
	std::map< INT64, int >::iterator it = m_mapPackingFileIndex.find( ulHashCode );
	while( it != m_mapPackingFileIndex.end() )
	{
		ulHashCode++;
		it = m_mapPackingFileIndex.find( ulHashCode );
	}
	m_mapPackingFileIndex.insert(  std::make_pair( ulHashCode, nFileInfoIndex ) );
}

void CEtPackingFile::RemovePackingMap( int nFileInfoIndex )
{
	if( ( nFileInfoIndex < 0 ) || ( nFileInfoIndex >= ( int )m_vecPackingFileInfo.size() ) )
	{
		return;
	}

	char szName[ _MAX_PATH ];
	_GetFullFileName( szName, _countof(szName), m_vecPackingFileInfo[ nFileInfoIndex ].szFileName );
	_strlwr( szName );
	TrimString( szName );

	INT64 ulHashCode = GetHashCode64( szName );
	std::map< INT64, int >::iterator it = m_mapPackingFileIndex.find( ulHashCode );
	while( it != m_mapPackingFileIndex.end() )
	{
		if( it->second == nFileInfoIndex )
		{
			m_mapPackingFileIndex.erase( it );
			break;
		}
		ulHashCode++;
		it = m_mapPackingFileIndex.find( ulHashCode );
	}
}

void CEtPackingFile::GeneratePackingMap()
{
	int i;

	m_mapPackingFileIndex.clear();

	for( i = 0; i < ( int )m_vecPackingFileInfo.size(); i++ )
	{
		AddPackingMap( i );
	}
}

int	CEtPackingFile::FindSuitableEmptySpace( DWORD dwSize )
{
	int i, nMinAllocIndex = -1;
	DWORD dwMinAllocSize = ULONG_MAX;

	for( i = 0; i < ( int )m_vecPackingFileInfo.size(); i++ )
	{
		if( m_vecPackingFileInfo[ i ].dwCompressSize != 0 )
		{
			continue;
		}
		if( m_vecPackingFileInfo[ i ].dwAllocSize < dwSize )
		{
			continue;
		}
		if( m_vecPackingFileInfo[ i ].dwAllocSize == dwSize )
		{
			return i;
		}
		if( m_vecPackingFileInfo[ i ].dwAllocSize * 0.8f > dwSize )
		{
			continue;
		}
		if( m_vecPackingFileInfo[ i ].dwAllocSize < dwMinAllocSize )
		{
			dwMinAllocSize = m_vecPackingFileInfo[ i ].dwAllocSize;
			nMinAllocIndex = i;
		}
	}

	return nMinAllocIndex;
}

bool CEtPackingFile::AddFile(const char* strPath)
{
	if( m_hFile == INVALID_HANDLE_VALUE) return false;

	// 眠啊且 角力 颇老 肺靛
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile( strPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	// 颇老 农扁甫 舅酒郴辑.
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if( dwFileSize == INVALID_FILE_SIZE )
	{
		CloseHandle(hFile);
		return true;
	}

	_Seek(hFile, 0, FILE_BEGIN);
	char *pData = new char[dwFileSize];
	_Read(hFile, pData, dwFileSize);

	CloseHandle(hFile);

	char strName[_MAX_FNAME] = { 0, };
	char szFullName[ _MAX_PATH ];

	GetNameOnly(strName,strPath);
	strcpy_s( szFullName, _MAX_PATH, m_strCurDir );
	strcat_s( szFullName, _MAX_PATH, strName );
	if(!AddFile(szFullName,pData,dwFileSize))
	{
		delete[] pData;
		return false;
	}

	delete[] pData;

	return true;
}

bool CEtPackingFile::AddFile(const char* strName, char *pData, DWORD nSize)
{
	if( m_hFile == INVALID_HANDLE_VALUE) 
		return false;

	if(nSize <= 0)
		return true;

	DWORD lDstSize = nSize + 12 + max( 1, ( nSize + 12 ) / 1000 );
	char *pDstData = new char[lDstSize];

	if( g_bUseCompress )
	{
		// 拘绵茄促.
		if(compress2( ( BYTE * )pDstData, (uLongf*)&lDstSize, ( BYTE * )pData, nSize, Z_BEST_SPEED) != Z_OK)
		{
			delete[] pDstData;
			return false;
		}
	}
	else
	{
		CopyMemory(pDstData, pData, nSize);
		lDstSize = nSize;
	}

	DWORD nSeedKey = 0;

	CopyMemory(&nSeedKey, pDstData, sizeof(DWORD));

	// 利寸茄 矫靛 牢郸胶甫 窍唱 榜扼辑..
	DWORD nSeedCnt = nSeedKey%ET_SEED_CNT;
	DWORD nSeed = ET_ENCRYPT_SEED[nSeedCnt];

	// 利寸洒.. XOR 肺 函版窍磊 -_-
	_Encode( ( BYTE * )pDstData, lDstSize, nSeed );

	char szName[ _MAX_PATH ];
	strcpy_s( szName, _MAX_PATH, strName );
	_strlwr( szName );
	TrimString( szName );
	int nFindFileIndex = FindFile( szName );
	if( nFindFileIndex != -1 )
	{
		Remove( szName );
	}

	int nFindEmptyIndex = FindSuitableEmptySpace( lDstSize );
	SPackingFileInfo PackingFileInfo;
	if( nFindEmptyIndex == -1 )
	{
		memset( &PackingFileInfo, 0, sizeof( SPackingFileInfo ) );
		strcpy_s( PackingFileInfo.szFileName, _MAX_FNAME, szName );
		PackingFileInfo.dwCompressSize = lDstSize;
		PackingFileInfo.dwOriginalSize = nSize;
		PackingFileInfo.dwAllocSize = lDstSize;
		PackingFileInfo.dwOffset = m_PackingFileHeader.dwFileInfoOffset;
		PackingFileInfo.dwSeedValue = nSeedCnt;
		m_vecPackingFileInfo.push_back( PackingFileInfo );
		nFindEmptyIndex = ( int )m_vecPackingFileInfo.size() - 1;

		_Seek( m_hFile, m_vecPackingFileInfo[ nFindEmptyIndex ].dwOffset, SEEK_SET );
		_Write( m_hFile, pDstData,lDstSize );
		// 颇老 庆歹 静绰单 矫埃 腹捞 吧妨辑 钎矫父 秦滴绊 CloseFileSystem()俊辑 庆歹 茄波锅俊 Write茄促
		m_PackingFileHeader.bRequireHeaderWrite = true;
		m_PackingFileHeader.dwFileInfoOffset = ( DWORD )_Seek( m_hFile, 0, SEEK_CUR );
	}
	else
	{
		PackingFileInfo = m_vecPackingFileInfo[ nFindEmptyIndex ];
		strcpy_s( PackingFileInfo.szFileName, _MAX_FNAME, szName );
		PackingFileInfo.dwCompressSize = lDstSize;
		PackingFileInfo.dwOriginalSize = nSize;
		PackingFileInfo.dwSeedValue = nSeedCnt;
		m_vecPackingFileInfo[ nFindEmptyIndex ] = PackingFileInfo;

		_Seek( m_hFile, m_vecPackingFileInfo[ nFindEmptyIndex ].dwOffset, SEEK_SET );
		_Write( m_hFile, pDstData,lDstSize );

		LONGLONG nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFindEmptyIndex );
		_Seek( m_hFile, nFindFileOffset, SEEK_SET );
		_Write( m_hFile, &m_vecPackingFileInfo[ nFindEmptyIndex ], sizeof( SPackingFileInfo ) );
	}

	m_PackingFileHeader.nFileCount = ( int )m_vecPackingFileInfo.size();
	_Seek( m_hFile, 0, SEEK_SET );
	_Write( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) );

	delete [] pDstData;

	AddPackingMap( nFindEmptyIndex );
	CalcChecksum( nFindEmptyIndex );

	return true;
}

void CEtPackingFile::WriteReserveFileInfo()
{
	if( ( !m_bIsReadOnly ) && ( m_PackingFileHeader.bRequireHeaderWrite ) )
	{
		_Seek( m_hFile, m_PackingFileHeader.dwFileInfoOffset, SEEK_SET );
		_Write( m_hFile, &m_vecPackingFileInfo[ 0 ], ( DWORD )m_vecPackingFileInfo.size() * sizeof( SPackingFileInfo ) );

		m_PackingFileHeader.bRequireHeaderWrite = false;
		_Seek( m_hFile, 0, SEEK_SET );
		_Write( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) );

		m_PackingFileHeader.bRequireHeaderWrite = false;
	}
}

void CEtPackingFile::RemoveFileInfo( int nFileIndex )
{
	if( nFileIndex < 0 || nFileIndex >= static_cast<int>(m_vecPackingFileInfo.size()) )
		return;

	m_vecPackingFileInfo[ nFileIndex ].dwCompressSize = 0;
	m_vecPackingFileInfo[ nFileIndex ].dwOriginalSize = 0;

	LONGLONG nFindFileOffset;
	nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFileIndex );
	_Seek( m_hFile, nFindFileOffset, SEEK_SET );
	_Write( m_hFile, &m_vecPackingFileInfo[ nFileIndex ], sizeof( SPackingFileInfo ) );
}

int CEtPackingFile::ReservePatchFile( SPackingFileInfo *pFileInfo )
{
	if( m_hFile == INVALID_HANDLE_VALUE) 
		return -1;

	if( pFileInfo->dwOriginalSize <= 0 )
		return -1;

	SPackingFileInfo PackingFileInfo;
	PackingFileInfo = *pFileInfo;
	PackingFileInfo.dwAllocSize = pFileInfo->dwCompressSize;
	PackingFileInfo.dwOffset = m_PackingFileHeader.dwFileInfoOffset;
	m_vecPackingFileInfo.push_back( PackingFileInfo );

	// 抗距父 吧绊 庆歹 Write绰 抗距捞 促 场抄 饶 菩摹 傈 角青
	m_PackingFileHeader.bRequireHeaderWrite = true;
	m_PackingFileHeader.dwFileInfoOffset += PackingFileInfo.dwCompressSize;

	return static_cast<int>( m_vecPackingFileInfo.size() - 1 );
}

bool CEtPackingFile::PatchFileIndex( CEtFileHandle *pFileHandle, int nFileIndex, BOOL bSaveFileInfo )
{
	if( m_hFile == INVALID_HANDLE_VALUE || nFileIndex < 0 || nFileIndex >= static_cast<int>( m_vecPackingFileInfo.size() ) ) 
		return false;

	if( pFileHandle->m_FileInfo.dwOriginalSize <= 0 )
		return true;

	if( pFileHandle->GetFileContext()->dwCompressSize > m_vecPackingFileInfo[ nFileIndex ].dwAllocSize )
		return false;

	if( bSaveFileInfo )
	{
		char szName[ _MAX_PATH ];
		strcpy_s( szName, _MAX_PATH, pFileHandle->m_FileInfo.szFileName );
		_strlwr( szName );
		TrimString( szName );

		SPackingFileInfo PackingFileInfo;
		PackingFileInfo = m_vecPackingFileInfo[ nFileIndex ];
		strcpy_s( PackingFileInfo.szFileName, _MAX_FNAME, szName );
		PackingFileInfo.dwCompressSize = pFileHandle->m_FileInfo.dwCompressSize;
		PackingFileInfo.dwOriginalSize = pFileHandle->m_FileInfo.dwOriginalSize;
		PackingFileInfo.dwSeedValue = pFileHandle->m_FileInfo.dwSeedValue;
		m_vecPackingFileInfo[ nFileIndex ] = PackingFileInfo;
	}

	// File Write
	_Seek( m_hFile, m_vecPackingFileInfo[ nFileIndex ].dwOffset, SEEK_SET );
	_Write( m_hFile, pFileHandle->m_pData, pFileHandle->m_FileInfo.dwCompressSize );

	// FileInfo Write
	LONGLONG nFindFileOffset;
	nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFileIndex );
	_Seek( m_hFile, nFindFileOffset, SEEK_SET );
	_Write( m_hFile, &m_vecPackingFileInfo[ nFileIndex ], sizeof( SPackingFileInfo ) );

	return true;
}

bool CEtPackingFile::PatchFile( CEtFileHandle *pFileHandle )
{
	if( m_hFile == INVALID_HANDLE_VALUE) 
		return false;

	if( pFileHandle->m_FileInfo.dwOriginalSize <= 0)
		return true;

	char szName[ _MAX_PATH ];
	strcpy_s( szName, _MAX_PATH, pFileHandle->m_FileInfo.szFileName );
	_strlwr( szName );
	TrimString( szName );
	int nFindFileIndex = FindFile( szName );
	if( nFindFileIndex != -1 )
	{
		m_vecPackingFileInfo[ nFindFileIndex ].dwCompressSize = 0;
		m_vecPackingFileInfo[ nFindFileIndex ].dwOriginalSize = 0;
	}

	int nFindEmptyIndex = FindSuitableEmptySpace( pFileHandle->m_FileInfo.dwCompressSize );
	SPackingFileInfo PackingFileInfo;
	if( nFindEmptyIndex == -1 )
	{
		PackingFileInfo = pFileHandle->m_FileInfo;
		PackingFileInfo.dwAllocSize = pFileHandle->m_FileInfo.dwCompressSize;
		PackingFileInfo.dwOffset = m_PackingFileHeader.dwFileInfoOffset;
		m_vecPackingFileInfo.push_back( PackingFileInfo );
		nFindEmptyIndex = ( int )m_vecPackingFileInfo.size() - 1;

		_Seek( m_hFile, m_vecPackingFileInfo[ nFindEmptyIndex ].dwOffset, SEEK_SET );
		_Write( m_hFile, pFileHandle->m_pData, pFileHandle->m_FileInfo.dwCompressSize );
		// 颇老 庆歹 静绰单 矫埃 腹捞 吧妨辑 钎矫父 秦滴绊 CloseFileSystem()俊辑 庆歹 茄波锅俊 Write茄促
		m_PackingFileHeader.bRequireHeaderWrite = true;
		m_PackingFileHeader.dwFileInfoOffset = ( DWORD )_Seek( m_hFile, 0, SEEK_CUR );
	}
	else
	{
		PackingFileInfo = m_vecPackingFileInfo[ nFindEmptyIndex ];
		strcpy_s( PackingFileInfo.szFileName, _MAX_FNAME, szName );
		PackingFileInfo.dwCompressSize = pFileHandle->m_FileInfo.dwCompressSize;
		PackingFileInfo.dwOriginalSize = pFileHandle->m_FileInfo.dwOriginalSize;
		PackingFileInfo.dwSeedValue = pFileHandle->m_FileInfo.dwSeedValue;
		m_vecPackingFileInfo[ nFindEmptyIndex ] = PackingFileInfo;

		_Seek( m_hFile, m_vecPackingFileInfo[ nFindEmptyIndex ].dwOffset, SEEK_SET );
		_Write( m_hFile, pFileHandle->m_pData, pFileHandle->m_FileInfo.dwCompressSize );

		LONGLONG nFindFileOffset;
		if( ( nFindFileIndex != -1 ) && ( nFindFileIndex != nFindEmptyIndex ) )
		{
			nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFindFileIndex );
			_Seek( m_hFile, nFindFileOffset, SEEK_SET );
			_Write( m_hFile, &m_vecPackingFileInfo[ nFindFileIndex ], sizeof( SPackingFileInfo ) );
		}
		nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFindEmptyIndex );
		_Seek( m_hFile, nFindFileOffset, SEEK_SET );
		_Write( m_hFile, &m_vecPackingFileInfo[ nFindEmptyIndex ], sizeof( SPackingFileInfo ) );
	}

	return true;
}

bool CEtPackingFile::Remove(const char* strName)
{
	if( m_hFile == INVALID_HANDLE_VALUE) 
		return false;

	char szFileName[ _MAX_PATH ];
	if( strName[ 0 ] == '\\' )
	{
		strcpy_s( szFileName, _MAX_PATH, strName );
	}
	else
	{
		strcpy_s( szFileName, _MAX_PATH, m_strCurDir );
		strcat_s( szFileName, _MAX_PATH, strName );
	}

	int nFindFileIndex = FindFile( szFileName );
	if( nFindFileIndex != -1 )
	{
		m_vecPackingFileInfo[ nFindFileIndex ].dwCompressSize = 0;
		m_vecPackingFileInfo[ nFindFileIndex ].dwOriginalSize = 0;
		LONGLONG nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFindFileIndex );
		_Seek( m_hFile, nFindFileOffset, SEEK_SET );
		_Write( m_hFile, &m_vecPackingFileInfo[ nFindFileIndex ], sizeof( SPackingFileInfo ) );
		_Seek( m_hFile, 0, SEEK_SET );

		RemovePackingMap( nFindFileIndex );
		AddPackingMap( nFindFileIndex );

		return true;
	}

	return false;
}

bool CEtPackingFile::Rename(const char* strName,const char* strRename)
{
	if( m_hFile == INVALID_HANDLE_VALUE)
		return false;

	int nFindFileIndex = FindFile( strName );
	if( nFindFileIndex != -1 )
	{
		strcpy_s( m_vecPackingFileInfo[ nFindFileIndex ].szFileName, _MAX_FNAME, strRename );
		LONGLONG nFindFileOffset = m_PackingFileHeader.dwFileInfoOffset + ( sizeof( SPackingFileInfo ) * nFindFileIndex );
		_Seek( m_hFile, nFindFileOffset, SEEK_SET );
		_Write( m_hFile, &m_vecPackingFileInfo[ nFindFileIndex ], sizeof( SPackingFileInfo ) );
		_Seek( m_hFile, 0, SEEK_SET );

		RemovePackingMap( nFindFileIndex );
		AddPackingMap( nFindFileIndex );

		return true;
	}

	return false;
}

bool CEtPackingFile::ChangeDir(const char* strPath)
{
	if( m_hFile == INVALID_HANDLE_VALUE ) 
		return false;

	if( strPath[ 0 ] == '\\' )
	{
		strcpy_s( m_strCurDir, _MAX_PATH, strPath );
		return true;
	}
	if( _stricmp( strPath, "." ) == 0 )
	{
		return true;
	}
	if( _stricmp( strPath, ".." ) == 0 )
	{
		int nStrLength = ( int )strlen( m_strCurDir );
		if( nStrLength == 1 )
		{
			return false;
		}
		m_strCurDir[ nStrLength - 1 ] = 0;
		char *pFindPtr = strrchr( m_strCurDir, '\\' );
		if( pFindPtr )
		{
			*( pFindPtr + 1 ) = 0;
		}
		return true;
	}

	strcat_s( m_strCurDir, _MAX_PATH, strPath );
	if( strPath[ strlen( strPath ) - 1 ] != '\\' )
	{
		strcat_s( m_strCurDir, _MAX_PATH, "\\" );
	}

	return true;
}

void CEtPackingFile::GetNameOnly(char* strDst,const char* strPath)
{
	char strTemp[_MAX_PATH] = { 0, };
	strcpy_s(strTemp,strPath);
	_strrev(strTemp);

	char* cp = strtok(strTemp, "\\/\x0\n\r\t");
	strcpy_s(strTemp,cp);
	_strrev(strTemp);
	GetSafeName(strDst,strTemp);
}

void CEtPackingFile::GetPathOnly(char* strDst, const char* strPath)
{
	char strName[_MAX_FNAME] = { 0, };
	GetNameOnly(strName,strPath);

	size_t iLen = strlen(strPath)-strlen(strName);

	if(!iLen)
	{
		strDst[0] = '\0';
	}
	else
	{
		//_tcsncpy_s(strDst, sizeof(strDst), strPath,iLen);
		strncpy(strDst, strPath,iLen);
		strDst[iLen] = '\0';
	}
}

void CEtPackingFile::GetFileInfoListCurDir( std::vector< SPackingFileInfo * > &vecFileInfo )
{
	int i;

	for( i = 0; i < ( int )m_vecPackingFileInfo.size(); i++ )
	{
		if( m_vecPackingFileInfo[ i ].dwCompressSize == 0 )
		{
			continue;
		}
		char szPath[ _MAX_PATH ];
		_GetPath( szPath, _countof(szPath), m_vecPackingFileInfo[ i ].szFileName );
		if( _stricmp( szPath, m_strCurDir ) == 0 )
		{
			vecFileInfo.push_back( &m_vecPackingFileInfo[ i ] );
		}
	}
}

void CEtPackingFile::GetDirListCurDir( std::vector< std::string > &vecDirList )
{
	int i, nStrLength;

	vecDirList.push_back( "." );
	if( _stricmp( m_strCurDir, "\\" ) != 0 )
	{
		vecDirList.push_back( ".." );
	}
	nStrLength = ( int )strlen( m_strCurDir );
	for( i = 0; i < ( int )m_vecPackingFileInfo.size(); i++ )
	{
		char szPath[ _MAX_PATH ], szFindPath[ _MAX_PATH];
		_GetPath( szPath, _countof(szPath), m_vecPackingFileInfo[ i ].szFileName );
		if( strstr( szPath, m_strCurDir ) != szPath )
		{
			continue;
		}
		char *pFindPtr = strchr( szPath + nStrLength, '\\' );
		if( pFindPtr == NULL )
		{
			continue;
		}
		*pFindPtr = 0;
		strcpy_s( szFindPath, _MAX_PATH, szPath + nStrLength );
		std::vector< std::string >::iterator it;
		it = std::find( vecDirList.begin(), vecDirList.end(), szFindPath );
		if( it == vecDirList.end() )
		{
			vecDirList.push_back( szFindPath );
		}
	}
}

bool CEtPackingFile::Patch( const char* strPatchPath, CPatchCallback* pCallpack, std::vector< CFileNameString > *pvecSkipList )
{
	CEtPackingFile srcPack;
	bool bResult = srcPack.OpenFileSystem(strPatchPath, true);

	m_pLog->Log( "srcPack.OpenFileSystem : %s " , strPatchPath );

	if( bResult == false )
	{
		if( pCallpack )
		{
			pCallpack->OnError( CPatchCallback::SrcPackOpenFailed, CFileNameString("SrcPackOpenFailed") );

		}
		m_pLog->Log( "OnError.SrcPackOpenFailed ");
		return false;
	}

	int nSrcFileCount = srcPack.GetFileCount();
	m_pLog->Log( "Get srcFileList : %u " , nSrcFileCount );


	if( nSrcFileCount <= 0 )
	{
		if( pCallpack )
		{
			pCallpack->OnError( CPatchCallback::SrcPackFileSizeZero, CFileNameString("SrcPackFileSizeZero") );
		}
		m_pLog->Log( "OnError SrcPackFileSizeZero ");

		return false;
	}

	int i;
	for( i = 0; i < nSrcFileCount; i++ )
	{
		SPackingFileInfo *pPackingFileInfo = srcPack.GetFileInfo( i );
		if( pPackingFileInfo == NULL )
		{
			continue;
		}

		bool bSkip = false;
		if( pvecSkipList )
		{
			int j;
			for( j = 0; j < ( int )pvecSkipList->size(); j++ )
			{
				// 颇老捞抚 菊俊.. \ 嘿绢 乐绢辑 弊芭 力寇茄 捞抚栏肺 厚背秦具 力措肺 等促.
				if( _stricmp( pPackingFileInfo->szFileName + 1, ( *pvecSkipList )[ j ].c_str() ) == 0 )
				{
					bSkip = true;
					break;
				}
			}
		}
		if( bSkip )
		{
			continue;
		}

		CEtFileHandle *pFileHandle = srcPack.OpenOnly( pPackingFileInfo->szFileName );
		if( pFileHandle == NULL )
		{
			continue;
		}
		bool bResultInner = PatchFile( pFileHandle );
//		bool bResult = AddFile( pPackingFileInfo->szFileName, pFileHandle->m_pData, pFileHandle->m_FileInfo.dwOriginalSize );
		CloseFile( pFileHandle );
		if( ( !bResultInner ) && ( pCallpack ) )
		{
			pCallpack->OnError( CPatchCallback::DstPackAddFailed, pPackingFileInfo->szFileName );
		}

		if( pCallpack )
		{
			pCallpack->OnPatch( i, nSrcFileCount, pPackingFileInfo->szFileName );
		}
	}

	m_PackingFileHeader.nFileCount = ( int )m_vecPackingFileInfo.size();
	_Seek( m_hFile, 0, SEEK_SET );
	_Write( m_hFile, &m_PackingFileHeader, sizeof( SPackingFileHeader ) );

	return true;
}

//-----------------------------------------------------------------------------------------------------------------

CEtFileSystem::CEtFileSystem( bool bDynamicPath )
{
	m_bDynamicPath = bDynamicPath;
}

CEtFileSystem::~CEtFileSystem()
{
	RemoveAll();
}

void CEtFileSystem::AddPackingFolder( const char* szPackingFileName, const char* szExt )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CFileNameString szFindFile;
	szFindFile = szPackingFileName;
	szFindFile += "\\*.";
	szFindFile += szExt;

	hFind = FindFirstFile( szFindFile.c_str() , &FindFileData );

	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			CFileNameString szFullPath;
			szFullPath = szPackingFileName;
			szFullPath += "\\";
			szFullPath += FindFileData.cFileName;
			CEtFileSystem::AddPackingFile(szFullPath.c_str(), szPackingFileName);
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}

	FindClose( hFind );
}

void CEtFileSystem::AddPackingFolder( const WCHAR* szPackingFileName, const char* szExt )
{
	//rlkt_pak
	//MessageBoxA(NULL, FormatA(" AddPackingFolder szPackingFileName: %s ext: %s", szPackingFileName, szExt).c_str(), "AddPackingFolder", MB_OK);
	WIN32_FIND_DATAW FindFileData;
	HANDLE hFind;
	std::wstring szFindFile;
	szFindFile = szPackingFileName;
	szFindFile += L"\\*.";

	std::string szTempExt = szExt;
	std::wstring wszTempExt;
	ToWideString(szTempExt, wszTempExt);
	szFindFile += wszTempExt;

	hFind = FindFirstFileW( szFindFile.c_str() , &FindFileData );

	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			std::wstring szFullPath;
			szFullPath = szPackingFileName;
			szFullPath += L"\\";
			szFullPath += FindFileData.cFileName;

			std::string szTempBufferPath;
			std::string szTempBufferName;
			ToMultiString( szFullPath, szTempBufferPath );
			ToMultiString( std::wstring(szPackingFileName), szTempBufferName );
			CEtFileSystem::AddPackingFile(szTempBufferPath.c_str(), szTempBufferName.c_str());
		}
		if( !FindNextFileW( hFind, &FindFileData ) ) break;
	}

	FindClose( hFind );
}

bool CEtFileSystem::AddPackingFile(const char* szPackingFileName, const char *szRootPath)
{
	//rlkt_pak
	//MessageBoxA(NULL, FormatA(" AddPackingFile szPackingFileName: %s szRootPath: %s", szPackingFileName, szRootPath).c_str(), "AddPackingFile", MB_OK);

	std::map<CFileNameString, CEtPackingFile*>::iterator it = m_PackingFileMap.find( CFileNameString(szPackingFileName) );

	if( it != m_PackingFileMap.end() )
		return false;

	CEtPackingFile* pPackingFile = new CEtPackingFile();
	pPackingFile->SetRootPath( szRootPath );
	bool bResult = pPackingFile->OpenFileSystem(szPackingFileName, true);
	if( !bResult )
	{
		//rlkt_pak
		//MessageBoxA(NULL, FormatA(" OpenFileSystem read only false szPackingFileName: %s szRootPath: %s", szPackingFileName, szRootPath).c_str(), "AddPackingFile", MB_OK);

		delete pPackingFile;
		return false;
	}

    //TODO(Cussrro):导出资源文件
    //auto file_count = pPackingFile->GetFileCount();
    //for (int i = 0; i < file_count; i++) {
    //    auto info = pPackingFile->GetFileInfo(i);
    //    auto handle = pPackingFile->OpenFile(info->szFileName);
    //    if (!info || !handle) {
    //        OutputDebugStringA("未找到:");
    //        OutputDebugStringA(info->szFileName);
    //        OutputDebugStringA("\n");
    //        pPackingFile->CloseFile(handle);
    //        continue;
    //    }

    //    handle->ExportFile();
    //    pPackingFile->CloseFile(handle);
    //}


	m_PackingFileMap.insert(make_pair(CFileNameString(szPackingFileName), pPackingFile));

	return true;
}

void CEtFileSystem::RemoveAll()
{
	std::map<CFileNameString, CEtPackingFile*>::iterator it = m_PackingFileMap.begin();

	for ( ; it != m_PackingFileMap.end() ; it++ )
	{
		CEtPackingFile* pPackingFile = it->second;
		delete pPackingFile;
	}

	m_PackingFileMap.clear();
}

CEtFileHandle *CEtFileSystem::OpenFile( const char* strPath, bool bOpenOnly )
{
	int nFindIndex = -1;
	CEtPackingFile *pFindPacking = FindFile( strPath, nFindIndex );
	if( pFindPacking )
	{
		if( bOpenOnly )
		{
			return pFindPacking->OpenOnly( strPath );
		}
		else
		{
			return pFindPacking->OpenFile( strPath );
		}
	}

	return NULL;
}

void CEtFileSystem::CloseFile( CEtFileHandle* pFileHandle )
{
	SAFE_DELETE( pFileHandle );
}

CEtPackingFile *CEtFileSystem::FindFile( const char *pFileName, int &nFindIndex )
{
	std::map< CFileNameString, CEtPackingFile * >::iterator it = m_PackingFileMap.begin();
	for ( ; it != m_PackingFileMap.end() ; it++ )
	{
		CEtPackingFile *pPackingFile = it->second;
		if( pPackingFile == NULL )
		{
			continue;
		}
		nFindIndex = pPackingFile->FindFile( pFileName );
		if( nFindIndex != -1 )
		{
			return pPackingFile;
		}
	}

	return NULL;
}

void CEtFileSystem::AddResourcePath( const char *szPath, bool bIncludeSubDir )
{
}

void CEtFileSystem::RemoveResourcePath( const char *szPath, bool bIncludeSubDir )
{
}

void CEtFileSystem::CacheFileList( int nIndex, bool bAdd )
{
}

CFileNameString CEtFileSystem::GetFullName( const char *szFileName, bool *bFind )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bDynamicPath);
	if( bFind ) *bFind = false;
	char szName[_MAX_PATH] = { 0, };

	if( ( szFileName[ 0 ] == '\\' ) || ( IsFullName( szFileName ) ) )
	{
		strcpy_s( szName, szFileName );
	}
	else
	{
		_GetFullFileName( szName, _countof(szName), szFileName );
	}

	int nFindIndex = -1;
	CEtPackingFile *pFindPacking = FindFile( szName, nFindIndex );
	if( pFindPacking )
	{
		if( bFind ) *bFind = true;
		return pFindPacking->GetFileInfo( nFindIndex )->szFileName;
	}

	return szFileName;
}

CFileNameString CEtFileSystem::GetFullPath( const char *szPath )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bDynamicPath);
	CFileNameString szTemp = szPath;
	ToLowerA( szTemp );
	std::map< CFileNameString, CEtPackingFile * >::iterator it;
	for ( it = m_PackingFileMap.begin(); it != m_PackingFileMap.end(); it++ )
	{
		CEtPackingFile *pPackingFile = it->second;
		if( pPackingFile == NULL )
		{
			continue;
		}
		int i;
		int nFileCount = pPackingFile->GetFileCount();
		for( i = 0; i < nFileCount; i++ )
		{
			SPackingFileInfo *pPackingFileInfo = pPackingFile->GetFileInfo( i );
			if( pPackingFileInfo == NULL )
			{
				continue;
			}
			char *pFindPtr = strstr( pPackingFileInfo->szFileName, szTemp.c_str() );
			if( pFindPtr == NULL )
			{
				continue;
			}
			pFindPtr = strchr( pFindPtr, '\\' );
			if( pFindPtr )
			{
				char szReturnPath[ _MAX_PATH ];
				*pFindPtr = 0;
				strcpy_s( szReturnPath, _MAX_PATH, pPackingFileInfo->szFileName );
				*pFindPtr = '\\';
				return szReturnPath;
			}
		}
	}

	return szPath;
}

void CEtFileSystem::FindFullPathList( const char *szPath, std::vector<CFileNameString> &szVecResult )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bDynamicPath);
	CFileNameString szTemp = "\\";
	szTemp += szPath;
	ToLowerA( szTemp );
	std::map< CFileNameString, CEtPackingFile * >::iterator it;
	for ( it = m_PackingFileMap.begin(); it != m_PackingFileMap.end(); it++ )
	{
		CEtPackingFile *pPackingFile = it->second;
		if( pPackingFile == NULL )
		{
			continue;
		}
		int i;
		int nFileCount = pPackingFile->GetFileCount();
		for( i = 0; i < nFileCount; i++ )
		{
			SPackingFileInfo *pPackingFileInfo = pPackingFile->GetFileInfo( i );
			if( pPackingFileInfo == NULL )
			{
				continue;
			}
			char *pFindPtr = strstr( pPackingFileInfo->szFileName, szTemp.c_str() );
			if( pFindPtr == NULL )
			{
				continue;
			}
			pFindPtr = strchr( pPackingFileInfo->szFileName, '\\' );
			if( pFindPtr )
			{
				char szReturnPath[ _MAX_PATH ];
				/*
				*pFindPtr = 0;
				strcpy_s( szReturnPath, _MAX_PATH, pPackingFileInfo->szFileName );
				*pFindPtr = '\\';
				*/
				_GetPath( szReturnPath, _countof(szReturnPath), pPackingFileInfo->szFileName );
				szReturnPath[ strlen(szReturnPath)-1] = 0;
				bool bExist = false;
				for( DWORD j=0; j<szVecResult.size(); j++ ) {
					if( strcmp( szVecResult[j].c_str(), szReturnPath ) == NULL ) {
						bExist = true;
						break;
					}
				}
				if( !bExist ) szVecResult.push_back( szReturnPath );
			}
		}
	}
}

void CEtFileSystem::FindFileListInFolder( const char *szFolderName, const char *szExt, std::vector<CFileNameString> &szVecResult)
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bDynamicPath);
	std::string strFolder = szFolderName;
	strFolder += "\\";

	/*
	UINT nCodePage = CEtResourceMng::GetInstance().GetCodePage();
	CFileNameString sztemp;
	const char *szCodePage = CEtResourceMng::GetInstance().FindCodePageString(nCodePage);
	strFolder += szCodePage;
	*/

	std::vector<std::string> ResultArray;
	std::map< std::string, std::vector<std::string> >::iterator iterFind = m_szMapCacheFileListInFolder.find( strFolder );
	if( iterFind != m_szMapCacheFileListInFolder.end() ) {
		ResultArray = iterFind->second;
	}
	else {
		std::map< CFileNameString, CEtPackingFile * >::iterator it = m_PackingFileMap.begin();
		for ( ; it != m_PackingFileMap.end() ; it++ )
		{
			int i;
			CEtPackingFile *pPackingFile = it->second;
			if( pPackingFile == NULL )
			{
				continue;
			}
			int nFileCount = pPackingFile->GetFileCount();
			for( i = 0; i < nFileCount; i++ )
			{
				SPackingFileInfo *pPackingFileInfo = pPackingFile->GetFileInfo( i );
				if( pPackingFileInfo == NULL || pPackingFileInfo->dwCompressSize == 0 || pPackingFileInfo->dwOriginalSize == 0 )
				{
					continue;
				}
				std::string szFolderKey = pPackingFileInfo->szFileName;
				if( szFolderKey.find(strFolder.c_str()) < szFolderKey.size() )
				{
					RemoveStringA(szFolderKey, strFolder);
					ResultArray.push_back(szFolderKey);
				}
			}
		}

		// 菩欧颇老 救俊 乐绰 颇老吝俊 \resource\ext肺 矫累窍绰 颇老阑 掘绢坷绰 风凭牢单,
		// 葛电 颇老阑 促 第瘤扁锭巩俊, 概锅 150ms狼 掉饭捞啊 积变促.
		// 捞霸 痢痢 菩欧颇老救俊 颇老捞 疵绢唱绊, ext狼 俺荐啊 腹酒龙荐废 歹 腹篮 荐青矫埃捞 鞘夸窍骨肺,
		// 捞犯霸 某矫甫 抵辑 掘绢坷绰 屈怕肺 窍摆促.
		// 捞犯霸 窍聪 蒲救静绰 巴苞 厚搅茄 肺爹加档甫 焊咯霖促.(TableDB肺靛 何盒)
		m_szMapCacheFileListInFolder.insert( make_pair( strFolder, ResultArray ) );
	}

	/*
	std::string strExt = szExt;
	RemoveStringA(strExt, std::string("*"));

	for ( size_t i = 0 ; i < ResultArray.size() ; i++ )
	{
		std::string str = ResultArray[i];
		if( str.find( strExt ) < str.size() )
		{
			szVecResult.push_back(str);
		}
	}
	*/
	std::vector<std::string> szVecNameStr;
	std::vector<std::string> szVecExtStr;

	char szTemp[256];
	sprintf_s( szTemp, szExt );
	_strlwr_s( szTemp );

	char szNameTemp[256];
	char szExtTemp[64];
	_GetFileName( szNameTemp, _countof(szNameTemp), szTemp );
	_GetExt( szExtTemp, _countof(szExtTemp), szTemp );

	for( int i=0; ; i++ ) {
		const char *szStr = _GetSubStrByCount( i, szNameTemp, '*' );
		if( szStr == NULL || strlen(szStr) < 1 ) break;
		szVecNameStr.push_back( szStr );
	}

	for( int i=0; ; i++ ) {
		const char *szStr = _GetSubStrByCount( i, szExtTemp, '*' );
		if( szStr == NULL || strlen(szStr) < 1 ) break;
		szVecExtStr.push_back( szStr );
	}

	for ( size_t i = 0 ; i < ResultArray.size() ; i++ )
	{
		std::string str = ResultArray[i];
		bool bResult = true;
		char *pPtr;
		for( DWORD j=0; j<szVecNameStr.size(); j++ ) {
			pPtr = (char*)strstr( str.c_str(), szVecNameStr[j].c_str() );
			if( pPtr == NULL ) {
//			if( str.find( szVecNameStr[j] ) >= str.size() ) {
				bResult = false;
				break;
			}
			if( j == 0 && pPtr != str.c_str() ) {
				bResult = false;
				break;
			}
		}
		if( !bResult ) continue;

		bResult = true;
		for( DWORD j=0; j<szVecExtStr.size(); j++ ) {
			pPtr = (char*)strstr( str.c_str(), szVecExtStr[j].c_str() );
//			if( str.find( szVecExtStr[j] ) >= str.size() ) {
			if( pPtr == NULL ) {
				bResult = false;
				break;
			}
		}
		if( !bResult ) continue;

		szVecResult.push_back(str);
	}
}