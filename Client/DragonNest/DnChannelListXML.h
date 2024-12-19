#pragma once

class TiXmlElement;

class CDnChannelListXML
{
public:
	CDnChannelListXML(void);
	virtual ~CDnChannelListXML(void);

public:
	struct SChannelName
	{
		std::wstring strName;
		std::wstring strComment;
	};

	typedef std::map<int,SChannelName>		INDEX_CHANNEL_MAP;
	typedef INDEX_CHANNEL_MAP::iterator		INDEX_CHANNEL_MAP_ITER;

protected:
	INDEX_CHANNEL_MAP m_mapChannelList;

protected:
	void ParsingChannel( TiXmlElement *pElement );

public:
	bool GetChannelInfo( int index, std::wstring &strName, std::wstring &strComment );
	int GetChannelListCount();

public:
	bool Initialize( CStream *pStream );
};
