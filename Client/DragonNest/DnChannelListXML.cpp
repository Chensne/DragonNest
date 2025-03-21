#include "StdAfx.h"
#include "DnChannelListXML.h"
#include "DebugSet.h"
#include "tinyxml.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChannelListXML::CDnChannelListXML(void)
{
}

CDnChannelListXML::~CDnChannelListXML(void)
{
	m_mapChannelList.clear();
}

bool CDnChannelListXML::Initialize( CStream *pStream )
{
	if( !pStream )
	{
		CDebugSet::ToLogFile( "CDnChannelListXML::Initialize, pStream is NULL!" );
		return false;
	}

	int nLen = pStream->Size();
	std::auto_ptr<char> pBuffer(new char[nLen+1]);
	SecureZeroMemory(pBuffer.get(), nLen+1);
	pStream->Read(pBuffer.get(), nLen);

	TiXmlDocument doc;
	doc.Parse(pBuffer.get(), 0, TIXML_ENCODING_UTF8 );
	if ( doc.Error() )
	{
		CDebugSet::ToLogFile( "CDnChannelListXML::Initialize, 'ChannelList.xml' parsing failed!" );
		return false;
	}

	TiXmlElement* pElem = doc.RootElement();
	if ( !pElem )
	{
		CDebugSet::ToLogFile( "CEtChannelListXML::Initialize, Invalid ChannelList Element!" );
		return false;
	}

	TiXmlNode *child(NULL);
	while( child = pElem->IterateChildren( child ) )
	{
		ParsingChannel( child->ToElement() );
	}

	return true;
}

void CDnChannelListXML::ParsingChannel( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtChannelListXML::ParsingChannel, Invalid pElement!" );
		return;
	}

	int nChannelIndex;
	if ( TIXML_SUCCESS != pElement->QueryIntAttribute( "index", &nChannelIndex ) )
	{
		CDebugSet::ToLogFile( "CEtChannelListXML::ParsingChannel, QueryIntAttribute(index) failed!" );
		return;
	}

	std::string strChannelName;
	strChannelName = pElement->Attribute( "name" );
	if( strChannelName.empty() )
	{
		CDebugSet::ToLogFile( "CEtChannelListXML::ParsingChannel, Channel Name is 0! Index:%d", nChannelIndex );
		return;
	}

	const char *pText = pElement->GetText();

	if( strlen( pText ) == 0 )
	{
		CDebugSet::ToLogFile( "CEtChannelListXML::ParsingChannel, pText length is 0! Index:%d", nChannelIndex );
		return;
	}

	if( strlen( pText ) >= 2048 )
	{
		CDebugSet::ToLogFile( "CEtChannelListXML::ParsingChannel, pText length is Big! Index:%d", nChannelIndex );
		return;
	}

	SChannelName sChannelName;
	wchar_t wszBuff[2048]={0};
	MultiByteToWideChar(CP_UTF8, 0, strChannelName.c_str(), -1, wszBuff, 2048 );
	sChannelName.strName = wszBuff;
	MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 2048 );
	sChannelName.strComment = wszBuff;
	m_mapChannelList.insert( make_pair( nChannelIndex, sChannelName ) );
}

int CDnChannelListXML::GetChannelListCount()
{
	return (int)m_mapChannelList.size();
}

bool CDnChannelListXML::GetChannelInfo( int index, std::wstring &strName, std::wstring &strComment )
{
	INDEX_CHANNEL_MAP_ITER iter = m_mapChannelList.find( index );
	if( iter != m_mapChannelList.end() )
	{
		strName = iter->second.strName;
		strComment = iter->second.strComment;
		return true;
	}

	CDebugSet::ToLogFile( "CEtChannelListXML::GetChannelInfo, Channel Index not found! Index:%d", index );
	return false;
}