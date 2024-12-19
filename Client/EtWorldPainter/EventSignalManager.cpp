#include "StdAfx.h"
#include "EventSignalManager.h"
#include <winsock2.h>
#include <process.h>
#include "EventSignalItem.h"
#include "FileServer.h"

int CEventSignalManager::s_nUniqueIndex = 0;

CEventSignalManager::CEventSignalManager()
{
	m_szWorkingFolder = CFileServer::GetInstance().GetWorkingFolder();
	m_szWorkingFolder += "\\Resource\\EventInfo";

	m_szFileName = "EventAreaInfo.dat";

	CString szIP, szHost;

	GetLocalIP( szIP );
	GetHostName( szHost );

	m_szMyName = szHost;
	m_szMyName += ";";
	m_szMyName += szIP;

	char szBuf[256];
	sprintf_s( szBuf, m_szMyName );
	for( int i=0; i<(int)strlen(szBuf); i++ ) {
		if( szBuf[i] == '.' ) szBuf[i] = ';';
	}
	m_szMyName = szBuf;
}

CEventSignalManager::~CEventSignalManager()
{
	SAFE_DELETE_PVEC( m_pVecList );
}

void CEventSignalManager::GetHostName( CString &szHost )
{
	char szStr[512] = { 0, };

	WSADATA winsockInfo;
	WSAStartup( MAKEWORD( 2, 2 ), &winsockInfo );

	gethostname( szStr, 512 );

	szHost = szStr;

	WSACleanup();
}

void CEventSignalManager::GetLocalIP( CString &szIP )
{
	char szStr[512] = { 0, };

	WSADATA winsockInfo;
	WSAStartup( MAKEWORD( 2, 2 ), &winsockInfo );

	gethostname( szStr, 512 );
	HOSTENT *p = NULL;
	p = gethostbyname( szStr );

	struct in_addr *ptr;
	char **listptr = p->h_addr_list;

	if( p->h_addrtype == AF_INET ) {
		while( ( ptr = reinterpret_cast<struct in_addr*>(*listptr++) ) != NULL ) {
			szIP = inet_ntoa( *ptr );
			break;
		}
	}

	WSACleanup();
}


bool CEventSignalManager::AddSignalItem( CEventSignalItem *pSignalItem )
{
	if( pSignalItem->GetUniqueIndex() == -1 ) {
		pSignalItem->SetUniqueIndex( s_nUniqueIndex );
		s_nUniqueIndex++;
	}
	m_pVecList.push_back( pSignalItem );
	return true;
}

bool CEventSignalManager::RemoveSignalItem( CString &szName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetName() == szName ) {
			SAFE_DELETE( m_pVecList[i] );
			m_pVecList.erase( m_pVecList.begin() + i );
		}
	}
	return true;
}

DWORD CEventSignalManager::GetSignalItemCount()
{
	return (DWORD)m_pVecList.size();
}

CEventSignalItem *CEventSignalManager::GetSignalItem( DWORD dwIndex )
{
	if( dwIndex < 0 || dwIndex >= m_pVecList.size() ) return NULL;
	return m_pVecList[dwIndex];
}

CEventSignalItem *CEventSignalManager::GetSignalItemFromName( CString &szSignalName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetName() == szSignalName ) return m_pVecList[i];
	}
	return NULL;
}

CEventSignalItem *CEventSignalManager::GetSignalItemFromUniqueIndex( int nValue )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetUniqueIndex() == nValue ) return m_pVecList[i];
	}
	return NULL;
}

bool CEventSignalManager::IsExistSignalItem( CString &szName, CString &szOriginalName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetName() == szName ) {
			if( !szOriginalName.IsEmpty() && m_pVecList[i]->GetName() == szOriginalName ) continue;
			return true;
		}
	}
	return false;
}

void CEventSignalManager::BackupFiles( CString szFileName )
{
	char szFullName[2048];
	char szBackupName[2048];

	__time64_t ltime;
	char szBuf[2048];
	errno_t err;

	time( &ltime );


	err = _ctime64_s( szBuf, 2048, &ltime );
	if( err != 0 ) return;

	for( int i=0; i<(int)strlen(szBuf); i++ ) {
		if( szBuf[i] == ' ' ) szBuf[i] = '_';
		if( szBuf[i] == ':' ) szBuf[i] = ';';
	}
	szBuf[strlen(szBuf) - 1] = 0;


	char szExt[256];
	char szName[256];
	_GetExt( szExt, _countof(szExt), szFileName );
	_GetFileName( szName, _countof(szName), szFileName );

	sprintf_s( szFullName, "%s\\%s", m_szWorkingFolder, szFileName );
	sprintf_s( szBackupName, "%s\\%s_%s_%s.%s", m_szWorkingFolder, szName, szBuf, m_szMyName, szExt );

	CopyFile( szFullName, szBackupName, FALSE );
}

bool CEventSignalManager::Save()
{
#ifndef NO_BACKUP
	BackupFiles( m_szFileName );
#endif //NO_BACKUP

	FILE *fp;
	char szFullName[2048];
	sprintf_s( szFullName, "%s\\%s", m_szWorkingFolder, m_szFileName );
	fopen_s( &fp, szFullName, "wb" );
	//MessageBoxA(NULL, szFullName, "Savin SIgnals", MB_OK);
	if( fp == NULL ) return false;

	fwrite( &s_nUniqueIndex, sizeof(int), 1, fp );

	bool bResult = true;
	DWORD dwCount = (DWORD)m_pVecList.size();
	fwrite( &dwCount, sizeof(int), 1, fp );
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->Save( fp ) == false ) {
			bResult = false;
			break;
		}
	}
	fclose(fp);

	return bResult;
}

bool CEventSignalManager::Load()
{
	CGlobalValue::GetInstance().UpdateInitDesc( "Register Event Signal Info.." );

	FILE *fp;
	char szFullName[2048];
	sprintf_s( szFullName, "%s\\%s", m_szWorkingFolder, m_szFileName );
	fopen_s( &fp, szFullName, "rb" );
	if( fp == NULL ) return false;

	fread( &s_nUniqueIndex, sizeof(int), 1, fp );

	bool bResult = true;
	DWORD dwCount;
	fread( &dwCount, sizeof(int), 1, fp );
	for( DWORD i=0; i<dwCount; i++ ) {
		CEventSignalItem *pItem = new CEventSignalItem( -1 );
		if( pItem->Load( fp ) == false ) {
			SAFE_DELETE( pItem );
			SAFE_DELETE_PVEC( m_pVecList );
			bResult = false;
			break;
		}
		m_pVecList.push_back( pItem );
	}
	fclose(fp);

	return bResult;
}

void CEventSignalManager::ExportEnum( FILE *fp )
{
	fprintf_s( fp, "#pragma once\n\n" );
	fprintf_s( fp, "#pragma pack(push, 4)\n" );
	fprintf_s( fp, "// 이벤트 컨트롤 타입 열거\n" );
	fprintf_s( fp, "enum EventControlTypeEnum {\n" );
	CString szStr;
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		szStr.Format( "	ETE_%s = %d,\n", m_pVecList[i]->GetName(), m_pVecList[i]->GetUniqueIndex() );
		fprintf_s( fp, szStr );
	}
	fprintf_s( fp, "};\n\n" );
	fprintf_s( fp, "// 이벤트 컨트롤 별 스트럭쳐\n" );
}

bool CEventSignalManager::ExportHeader( CString szFileName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "wt" );
	if( fp == NULL ) return false;

	std::map<int,int> VecDataUsingCount;
	std::map<int,int> VecDataUsingCount64;

	ExportEnum( fp );
	bool bResult = true;
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		int nUsingBuffCount = 0;
		int nUsingBuffCount64 = 0;
		if( m_pVecList[i]->ExportHeader( fp, nUsingBuffCount, nUsingBuffCount64 ) == false ) {
			bResult = false;
			break;
		}

		VecDataUsingCount.insert(std::make_pair(m_pVecList[i]->GetUniqueIndex(), nUsingBuffCount));
		VecDataUsingCount64.insert(std::make_pair(m_pVecList[i]->GetUniqueIndex(), nUsingBuffCount64));
	}

	fprintf_s(fp, "static int GetEventSignalDataUsingCount(int signalIdx) {\n\tswitch(signalIdx) {\n");
	std::map<int, int>::const_iterator iter = VecDataUsingCount.begin();
	for (; iter != VecDataUsingCount.end(); ++iter)
	{
		fprintf_s(fp, "\tcase %d: return %d;\n", (*iter).first, (*iter).second);
	}
	fprintf_s(fp, "\t}\n\treturn 256;\n};\n");

	fprintf_s(fp, "static int GetEventSignalDataUsingCount64(int signalIdx) {\n\tswitch(signalIdx) {\n");
	iter = VecDataUsingCount64.begin();
	for (; iter != VecDataUsingCount64.end(); ++iter)
	{
		fprintf_s(fp, "\tcase %d: return %d;\n", (*iter).first, (*iter).second);
	}
	fprintf_s(fp, "\t}\n\treturn 256;\n};\n");

	fprintf_s( fp, "\n#pragma pack(pop)\n" );
	fclose(fp);

	return bResult;
}

void CEventSignalManager::ChangeWorkingFolder( CString szPath )
{
	m_szWorkingFolder = szPath;
	m_szWorkingFolder += "\\Resource\\EventInfo";
}

bool CEventSignalManager::IsCanCheckOut()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	CString szFullName;
	szFullName.Format( "%s\\checkout.ini", m_szWorkingFolder, m_szFileName );

	FILE *fp;
	fopen_s( &fp, szFullName, "r" );
	if( fp == NULL ) return true;
	fclose(fp);
	return false;
}

bool CEventSignalManager::CheckOut()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	if( IsCanCheckOut() == false ) return false;
	CString szFullName;
	szFullName.Format( "%s\\checkout.ini", m_szWorkingFolder, m_szFileName );

	FILE *fp;
	fopen_s( &fp, szFullName, "wt" );
	fprintf_s( fp, m_szMyName );
	fclose(fp);

	return true;
}

bool CEventSignalManager::CheckIn()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	CString szFullName;
	szFullName.Format( "%s\\checkout.ini", m_szWorkingFolder, m_szFileName );

	DeleteFile( szFullName );
	return true;
}