#include "StdAfx.h"
#include "DnGameTable.h"
#include "RenderBase.h"
#include "EtResourceMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameTable::CDnGameTable(void)
{
	std::vector<CFileNameString> szVecList;
	FindExtFileList( "WeaponTable.dnt", szVecList );
	DNTableFileFormat* pWeaponTable = new DNTableFileFormat();
	for( int i = 0; i < (int)szVecList.size(); ++i )
	{
		pWeaponTable->Load( szVecList[i].c_str(), false );
	}
	m_pVecList.push_back( pWeaponTable );

	szVecList.clear();
	FindExtFileList( "FileTable.dnt", szVecList );
	DNTableFileFormat* pFileTable = new DNTableFileFormat();
	for( int i = 0; i < (int)szVecList.size(); ++i )
	{
		pFileTable->Load( szVecList[i].c_str(), false );
	}
	m_pVecList.push_back( pFileTable );
}

CDnGameTable::~CDnGameTable(void)
{
	SAFE_DELETE_PVEC( m_pVecList );
}

DNTableFileFormat* CDnGameTable::GetTable( int iTableEnum )
{
	DNTableFileFormat* pResult = NULL;

	if( iTableEnum < (int)m_pVecList.size() )
		pResult = m_pVecList.at( iTableEnum );

	return pResult;
}

void CDnGameTable::FindExtFileList( const char *szFileName, std::vector<CFileNameString> &szVecList )
{
	char szTemp[_MAX_PATH] = { 0, };
	char szName[256] = { 0, };
	char szExt[256] = { 0, };
	_GetFileName( szName, _countof(szName), szFileName );
	_GetExt( szExt, _countof(szExt), szFileName );
	sprintf_s( szTemp, "%s*.%s", szName, szExt );
	CEtResourceMng::GetInstance().FindFileListAll_IgnoreExistFile( "ext", szTemp, szVecList );
}