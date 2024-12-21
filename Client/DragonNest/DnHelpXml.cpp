#include "StdAfx.h"
#include "DnHelpXml.h"
#include "DebugSet.h"
#include "tinyxml.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnHelpXml::CDnHelpXml(void)
{
}

CDnHelpXml::~CDnHelpXml(void)
{
	m_mapCategoryList.clear();
}

bool CDnHelpXml::Initialize( CStream *pStream )
{
	if( !pStream )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::Initialize, pStream is NULL!" );
		return false;
	}

	int nLen = pStream->Size();
	char *pBuffer = new char[nLen+1];
	SecureZeroMemory(pBuffer, nLen+1);
	pStream->Read(pBuffer, nLen);

	TiXmlDocument doc;
	doc.Parse(pBuffer, 0, TIXML_ENCODING_UTF8 );
	if ( doc.Error() )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::Initialize, 'Help.xml' parsing failed!" );
		SAFE_DELETEA( pBuffer );
		return false;
	}

	TiXmlElement* pElem = doc.RootElement();
	if ( !pElem )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::Initialize, Invalid CategoryList Element!" );
		SAFE_DELETEA( pBuffer );
		return false;
	}

	TiXmlNode *child(NULL);
	while( child = pElem->IterateChildren( child ) )
	{
		ParsingCategory( child->ToElement() );
	}

	SAFE_DELETEA( pBuffer );
	return true;
}

void CDnHelpXml::ParsingCategory( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::ParsingCategory, Invalid pElement!" );
		return;
	}

	int nIndex;
	if ( TIXML_SUCCESS != pElement->QueryIntAttribute( "index", &nIndex ) )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::ParsingCategory, QueryIntAttribute(index) failed!" );
		return;
	}

	std::string strTitle;
	strTitle = pElement->Attribute( "Title" );
	if( strTitle.empty() )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::ParsingCategory, Html is 0! Index:%d", nIndex );
		return;
	}

	const char *szHtml = pElement->GetText();

	if( strlen( szHtml ) == 0 )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::ParsingCategory, szHtml length is 0! Index:%d", nIndex );
		return;
	}

	if( strlen( szHtml ) >= 4096 )
	{
		CDebugSet::ToLogFile( "CDnHelpXml::ParsingCategory, szHtml length is Big! Index:%d", nIndex );
		return;
	}

	SHelpCategory sHelpCategory;
	wchar_t wszBuff[4096]={0};

	MultiByteToWideChar(CP_UTF8, 0, strTitle.c_str(), -1, wszBuff, 4096 );
	sHelpCategory.strTitle = wszBuff;

	MultiByteToWideChar(CP_UTF8, 0, szHtml, -1, wszBuff, 4096 );
	sHelpCategory.strHtml = wszBuff;

	m_mapCategoryList.insert( make_pair( nIndex, sHelpCategory ) );
}

int CDnHelpXml::GetListCount()
{
	return (int)m_mapCategoryList.size();
}

const wchar_t* CDnHelpXml::GetTitle( int index )
{
	INDEX_CATEGORY_MAP_ITER iter = m_mapCategoryList.find( index );
	if( iter != m_mapCategoryList.end() )
	{
		return iter->second.strTitle.c_str();
	}

	CDebugSet::ToLogFile( "CDnHelpXml::GetTitle, Category Index not found! Index:%d", index );
	return NULL;
}

const wchar_t* CDnHelpXml::GetHtml( int index )
{
	INDEX_CATEGORY_MAP_ITER iter = m_mapCategoryList.find( index );
	if( iter != m_mapCategoryList.end() )
	{
		return iter->second.strHtml.c_str();
	}

	CDebugSet::ToLogFile( "CDnHelpXml::GetHtml, Category Index not found! Index:%d", index );
	return NULL;
}

void CDnHelpXml::SetCategoryIndex( std::vector<int> &vecIndex )
{
	vecIndex.clear();

	INDEX_CATEGORY_MAP_ITER iter = m_mapCategoryList.begin();
	for( ; iter != m_mapCategoryList.end(); ++iter )
	{
		vecIndex.push_back( iter->first );
	}
}