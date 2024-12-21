#include "StdAfx.h"
#include "SignalManager.h"
#include <winsock2.h>
#include <process.h>
#include "SignalItem.h"
#include "SignalCustomRender.h"

int CSignalManager::s_nUniqueIndex = 0;
CSignalManager g_SignalManager;

CSignalManager::CSignalManager()
{
	char szStr[2048] = { 0, };
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "WorkingFolder", szStr, 2048 );
	if( strlen(szStr) == 0 )
		m_szWorkingFolder = "\\\\eye-ad\\ToolData\\SignalData";
	else m_szWorkingFolder = szStr;

	m_szFileName = "ActionInfo.dat";

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

	InitEnableSaveAndPasteSignal();
	Load();
	LoadDefineList();
}

CSignalManager::~CSignalManager()
{

	SAFE_DELETE_PVEC( m_pVecList );
	for( DWORD i=0; i<m_VecDefineList.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecDefineList[i].szVecList );
	}
	SAFE_DELETE_VEC( m_VecDefineList );
}

void CSignalManager::GetHostName( CString &szHost )
{
	char szStr[512] = { 0, };

	WSADATA winsockInfo;
	WSAStartup( MAKEWORD( 2, 2 ), &winsockInfo );

	gethostname( szStr, 512 );

	szHost = szStr;

	WSACleanup();
}

void CSignalManager::GetLocalIP( CString &szIP )
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


bool CSignalManager::AddSignalItem( CSignalItem *pSignalItem )
{
	if( pSignalItem->GetUniqueIndex() == -1 ) {
		pSignalItem->SetUniqueIndex( s_nUniqueIndex );
		s_nUniqueIndex++;
	}
	SetEnableSaveAndPasteSignal( pSignalItem );
	m_pVecList.push_back( pSignalItem );
	return true;
}

bool CSignalManager::RemoveSignalItem( CString &szName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetName() == szName ) {
			SAFE_DELETE( m_pVecList[i] );
			m_pVecList.erase( m_pVecList.begin() + i );
		}
	}
	return true;
}

DWORD CSignalManager::GetSignalItemCount()
{
	return (DWORD)m_pVecList.size();
}

CSignalItem *CSignalManager::GetSignalItem( DWORD dwIndex )
{
	if( dwIndex < 0 || dwIndex >= m_pVecList.size() ) return NULL;
	return m_pVecList[dwIndex];
}

CSignalItem *CSignalManager::GetSignalItemFromName( CString &szSignalName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetName() == szSignalName ) return m_pVecList[i];
	}
	return NULL;
}

CSignalItem *CSignalManager::GetSignalItemFromUniqueIndex( int nValue )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetUniqueIndex() == nValue ) return m_pVecList[i];
	}
	return NULL;
}

bool CSignalManager::IsExistSignalItem( CString &szName, CString &szOriginalName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( m_pVecList[i]->GetName() == szName ) {
			if( !szOriginalName.IsEmpty() && m_pVecList[i]->GetName() == szOriginalName ) continue;
			return true;
		}
	}
	return false;
}

void CSignalManager::BackupFiles( CString szFileName )
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

bool CSignalManager::Save()
{
#ifndef NO_BACKUP
	BackupFiles( m_szFileName );
#endif //NO_BACKUP

	FILE *fp;
	char szFullName[2048];
	sprintf_s( szFullName, "%s\\%s", m_szWorkingFolder, m_szFileName );
	fopen_s( &fp, szFullName, "wb" );
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

bool CSignalManager::Load()
{
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
		CSignalItem *pItem = new CSignalItem( -1 );
		if( pItem->Load( fp ) == false ) {
			SAFE_DELETE( pItem );
			SAFE_DELETE_PVEC( m_pVecList );
			bResult = false;
			break;
		}
		SetEnableSaveAndPasteSignal( pItem );
		m_pVecList.push_back( pItem );
	}
	fclose(fp);

	return bResult;
}

void CSignalManager::ExportEnum( FILE *fp )
{
	fprintf_s( fp, "#pragma once\n\n" );
	fprintf_s( fp, "#pragma pack(push, 4)\n" );
	fprintf_s( fp, "// 시그널 타입 열거\n" );
	fprintf_s( fp, "enum SignalTypeEnum {\n" );
	CString szStr;
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		szStr.Format( "	STE_%s = %d,\n", m_pVecList[i]->GetName(), m_pVecList[i]->GetUniqueIndex() );
		fprintf_s( fp, szStr );
	}
	fprintf_s( fp, "};\n\n" );
	fprintf_s( fp, "// 시그널 별 스트럭쳐\n" );
}

bool CSignalManager::ExportHeader( CString szFileName )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "wt" );
	if( fp == NULL ) return false;

	std::map<int, int> VecDataUsingCount;
	std::map<int, int> VecDataUsingCount64;

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
		//fprintf_s( fp, "const int %sUsingCount = %d;\n\n", m_pVecList[i]->GetName(), nUsingBuffCount );
	}

	fprintf_s(fp, "static int GetSignalDataUsingCount(int signalIdx) {\n\tswitch(signalIdx) {\n");
	std::map<int, int>::const_iterator iter = VecDataUsingCount.begin();
	for (; iter != VecDataUsingCount.end(); ++iter)
	{
		fprintf_s(fp, "\tcase %d: return %d;\n", (*iter).first, (*iter).second);
	}
	fprintf_s(fp, "\t}\n\treturn 64;\n};\n");

	fprintf_s(fp, "\nstatic int GetSignalDataUsingCount64(int signalIdx) {\n\tswitch(signalIdx) {\n");
	iter = VecDataUsingCount64.begin();
	for (; iter != VecDataUsingCount64.end(); ++iter)
	{
		fprintf_s(fp, "\tcase %d: return %d;\n", (*iter).first, (*iter).second);
	}
	fprintf_s(fp, "\t}\n\treturn 64;\n};\n");


	fprintf_s( fp, "\n#pragma pack(pop)\n\n" );
	fclose(fp);

	return bResult;
}

void CSignalManager::ChangeWorkingFolder( CString szPath )
{
	m_szWorkingFolder = szPath;

	Refresh();
}

bool CSignalManager::IsCanCheckOut()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	CString szFullName;
	szFullName.Format( "%s\\checkout.ini", m_szWorkingFolder );

	FILE *fp;
	fopen_s( &fp, szFullName, "r" );
	if( fp == NULL ) return true;
	fclose(fp);
	return false;
}

bool CSignalManager::CheckOut()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	if( IsCanCheckOut() == false ) return false;
	CString szFullName;
	szFullName.Format( "%s\\checkout.ini", m_szWorkingFolder );

	FILE *fp;
	fopen_s( &fp, szFullName, "wt" );
	fprintf_s( fp, m_szMyName );
	fclose(fp);

	return true;
}

bool CSignalManager::CheckIn()
{
#ifdef NO_LOCK
	return true;
#endif //NO_LOCK
	CString szFullName;
	szFullName.Format( "%s\\checkout.ini", m_szWorkingFolder );

	DeleteFile( szFullName );
	return true;
}

void CSignalManager::LoadDefineList()
{
	for( DWORD i=0; i<m_VecDefineList.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecDefineList[i].szVecList );
	}
	SAFE_DELETE_VEC( m_VecDefineList );

	std::vector< std::string > szVecList;
	FindFileListInDirectory( m_szWorkingFolder, "*.txt", szVecList, false, false );

	char szName[256] = { 0, };
	CString szFullPathName;
	DefineStringStruct Struct;
	for( DWORD i=0; i<szVecList.size(); i++ ) {
		Struct.szVecList.clear();
		_GetFileName( szName, _countof(szName), szVecList[i].c_str() );
		szFullPathName.Format( "%s\\%s", m_szWorkingFolder, szVecList[i].c_str() );
		Struct.szFileName = szName;

		if( ParseDefineString( szFullPathName, Struct.szVecList ) == false ) continue;
		m_VecDefineList.push_back( Struct );
	}
}

bool CSignalManager::ParseDefineString( const char *szFileName, std::vector<CString> &szVecList )
{
	FILE *fp;
	fopen_s( &fp, szFileName, "rt" );
	if( fp == NULL ) return false;

	char szBuf[1024] = { 0, };
	while( !feof(fp) ) {
		fscanf_s( fp, "%s", szBuf, 1024 );

		bool bExist = false;
		for( DWORD i=0; i<szVecList.size(); i++ ) {
			if( strcmp( szBuf, szVecList[i] ) == NULL ) {
				bExist = true;
				break;
			}
		}

		if( !bExist ) szVecList.push_back( szBuf );
	}
	fclose(fp);

	return true;
}

int CSignalManager::GetDefineCategoryIndex( CString szStr )
{
	for( DWORD i=0; i<m_VecDefineList.size(); i++ ) {
		if( m_VecDefineList[i].szFileName == szStr ) return i;
	}
	return -1;
}

void CSignalManager::Refresh()
{
	SAFE_DELETE_PVEC( m_pVecList );
	for( DWORD i=0; i<m_VecDefineList.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecDefineList[i].szVecList );
	}
	SAFE_DELETE_VEC( m_VecDefineList );

	Load();
	LoadDefineList();
}

void CSignalManager::InitEnableSaveAndPasteSignal()
{
	m_vEnableSaveAndPasteSignal.push_back( 11 );	// STE_Sound
	m_vEnableSaveAndPasteSignal.push_back( 12 );	// STE_Particle
	m_vEnableSaveAndPasteSignal.push_back( 79 );	// STE_Particle_LoopEnd
	m_vEnableSaveAndPasteSignal.push_back( 51 );	// STE_Decal
	m_vEnableSaveAndPasteSignal.push_back( 33 );	// STE_FX
	m_vEnableSaveAndPasteSignal.push_back( 80 );	// STE_FX_LoopEnd
	m_vEnableSaveAndPasteSignal.push_back( 37 );	// STE_ActionObject
	m_vEnableSaveAndPasteSignal.push_back( 31 );	// STE_CameraEffect_Shake
	m_vEnableSaveAndPasteSignal.push_back( 16 );	// STE_EnvironmentEffect
	m_vEnableSaveAndPasteSignal.push_back( 92 );	// STE_OtherSelfEffect
	m_vEnableSaveAndPasteSignal.push_back( 43 );	// STE_AttachSwordTrail
	m_vEnableSaveAndPasteSignal.push_back( 21 );	// STE_AttachTrail
	m_vEnableSaveAndPasteSignal.push_back( 19 );	// STE_AlphaBlending
}

void CSignalManager::SetEnableSaveAndPasteSignal( CSignalItem *pSignalItem )
{
	bool bEnable = IsEnableSaveAndPasteSignal( pSignalItem->GetUniqueIndex() );

	pSignalItem->SetEnableCopyAndPaste( bEnable );
}

bool CSignalManager::IsEnableSaveAndPasteSignal( const int nUniqueIndex )
{
	std::vector<int>::iterator first_Itor = m_vEnableSaveAndPasteSignal.begin();
	std::vector<int>::iterator end_Itor = m_vEnableSaveAndPasteSignal.end();
	std::vector<int>::iterator Itor = std::find( first_Itor, end_Itor, nUniqueIndex );

	//if( end_Itor == Itor )
	//	return false;

	return true;
}