#pragma once
#include "Singleton.h"
#include "StringUtil.h"
#include <map>

class TiXmlElement;
class CStream;

class CEtUIXML : public CSingleton<CEtUIXML>
{
public:
	enum emCategoryId
	{
		idCategory1 = 1,	// 클라이언트 메세지
		idCategory2 = 2,	// 서버 메세지
	};

public:
	CEtUIXML(void);
	virtual ~CEtUIXML(void);

public:
	typedef std::map< int, std::wstring >	XML_STRING_MAP;
	typedef XML_STRING_MAP::iterator		XML_STRING_MAP_ITER;

	struct TNationString
	{
		XML_STRING_MAP mapCategory1;
		XML_STRING_MAP mapCategory2;
		TNationString()
		{
			mapCategory1.clear();
			mapCategory2.clear();
		}
	};
	typedef std::map< int, TNationString > XML_NATIONSTRING_MAP;
	typedef XML_NATIONSTRING_MAP::iterator XML_NATIONSTRING_MAP_IT;

protected:
	XML_STRING_MAP m_mapCategory1;
	XML_STRING_MAP m_mapCategory2;
	XML_NATIONSTRING_MAP m_mapNationString;

	std::map<std::wstring, int> m_mapStringCategory1;

	int m_nCurCategoryId;
	int m_nCurMessageId;
	int m_nCurNationId;

protected:
	/*void ParsingCategory( TiXmlElement *pElement );*/
	void ParsingMessage( TiXmlElement *pElement );
	void ParsingLang( TiXmlElement *pElement );

	const char* GetLangId( UINT nCodePage );

public:
	const XML_STRING_MAP* GetCategoryList( emCategoryId categoryId );
	const wchar_t* GetUIString( emCategoryId categoryId, int nMessageId, int nNationId = -1 );
	bool CheckKoreanWord( const wchar_t *szString );

	int GetUIMessageID(const WCHAR * pwszString);

	// 국가별 디파인체크를 EtStringManager에서 할 수 없으니 외부에서 설정하도록 한다.
	// 기본값(국내빌드)은 false이며, 그 외 버전에서는 true로 한글나오면 M : ID로 바꿔버린다.
	static bool s_bCheckKoreanWord;

public:
	bool LoadXML( const char *szFileName, emCategoryId categoryId );
	bool Initialize( CStream *pStream, emCategoryId categoryId );
	bool Initialize( CStream *pStream, emCategoryId categoryId, int nNationId, bool bAttachData = false );

////////////////////////////////////////////////////////////////////////////////
protected:
	std::vector<std::wstring> m_vecAccountFilter;
	std::vector<std::wstring> m_vecChatFilter;
	std::vector<std::wstring> m_vecWhiteList;
	std::vector<std::wstring> m_vecReplaceFilter;

protected:
	void ParsingAccountWord( TiXmlElement *pElement );
	void ParsingChatWord( TiXmlElement *pElement );
	void ParsingWhiteListWord( TiXmlElement *pElement );
	void ParsingReplaceWord( TiXmlElement *pElement );

public:
	bool LoadFilter( const char *szFileName );
	bool LoadWhiteList( const char *szFileName );

	std::vector<std::wstring> &GetAccountFilter()	{ return m_vecAccountFilter; }
	std::vector<std::wstring> &GetChatFilter()		{ return m_vecChatFilter; }
	std::vector<std::wstring> &GetWhiteList()		{ return m_vecWhiteList; }
	std::vector<std::wstring> &GetReplaceFilter()	{ return m_vecReplaceFilter; }
private:
	//nation
	XML_STRING_MAP * _GetCategoryNationString(int nCategoryId, int nNationId = -1);
};

#define GetEtUIXML()		CEtUIXML::GetInstance()


// 스트링 조합 번역 예외 처리에 사용 할 xml
class CEtExceptionalUIXML : public CSingleton<CEtExceptionalUIXML>
{
public:
	CEtExceptionalUIXML();
	virtual ~CEtExceptionalUIXML();

public:
	bool LoadXML( const char* szFileName );
	bool Initialize( CStream* pStream );
	void ParsingMessage( TiXmlElement *pElement );

	std::wstring GetReplacementUIString( char* szParam );

protected:
	std::map< std::wstring, std::wstring >	m_mapReplacementString;
};

#define GetEtExeptionalUIXML()		CEtExceptionalUIXML::GetInstance()