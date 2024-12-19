#include "StdAfx.h"
#include "DnCutSceneTable.h"
#include "LuaDelegate.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnCutSceneTable::CDnCutSceneTable(void)
{
	const char* pResPath = LUA_DELEGATE.GetString( "resource_path" );
	string strExtPath(pResPath);

	strExtPath.append( "\\Ext\\" );
	strExtPath.append( "PropTable.dnt" );
	DNTableFileFormat*  pPropTable = new DNTableFileFormat;
	pPropTable->Load( strExtPath.c_str() );

	m_pvlTable.push_back( pPropTable );
}

CDnCutSceneTable::~CDnCutSceneTable(void)
{
	for_each( m_pvlTable.begin(), m_pvlTable.end(), DeleteData<DNTableFileFormat* >() );
}


DNTableFileFormat*  CDnCutSceneTable::GetTable( int iTableEnum )
{
	DNTableFileFormat*  pResult = NULL;

	if( iTableEnum < (int)m_pvlTable.size() )
		pResult = m_pvlTable.at( iTableEnum );

	return pResult;
}


