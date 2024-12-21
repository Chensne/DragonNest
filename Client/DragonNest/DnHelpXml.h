#pragma once

class TiXmlElement;

class CDnHelpXml
{
public:
	CDnHelpXml(void);
	virtual ~CDnHelpXml(void);

public:
	struct SHelpCategory
	{
		std::wstring strTitle;
		std::wstring strHtml;
	};

	typedef std::map<int,SHelpCategory>		INDEX_CATEGORY_MAP;
	typedef INDEX_CATEGORY_MAP::iterator	INDEX_CATEGORY_MAP_ITER;

protected:
	INDEX_CATEGORY_MAP m_mapCategoryList;

protected:
	void ParsingCategory( TiXmlElement *pElement );

public:
	const wchar_t* GetTitle( int index );
	const wchar_t* GetHtml( int index );

	int GetListCount();
	void SetCategoryIndex( std::vector<int> &vecIndex );

public:
	bool Initialize( CStream *pStream );
};
