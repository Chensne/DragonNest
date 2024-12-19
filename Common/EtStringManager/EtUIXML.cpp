#include "StdAfx.h"
#include "EtUIXML.h"
#include "DebugSet.h"
#include "tinyxml.h"
#include "Stream.h"
#include "SundriesFunc.h"
#include "../../Common/EtResManager/EtResourceMng.h"
#include "SundriesFunc.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool CEtUIXML::s_bCheckKoreanWord = false;

CEtUIXML::CEtUIXML(void)
	: m_nCurCategoryId(-1)
	, m_nCurMessageId(-1)
	, m_nCurNationId(-1)
{
}

CEtUIXML::~CEtUIXML(void)
{
	m_mapCategory1.clear();
	m_mapCategory2.clear();
	m_mapNationString.clear();
	m_mapStringCategory1.clear();
	m_nCurCategoryId = -1;
	m_nCurMessageId = -1;
	m_nCurNationId = -1;
}

bool CEtUIXML::LoadXML( const char *szFileName, emCategoryId categoryId )
{
	CResMngStream Stream( szFileName );
	if( !Stream.IsValid() )
	{
		CDebugSet::ToLogFile( "Failed:CEtUIXML::LoadXML, File(%s), Category(%d)", szFileName, categoryId );
		return false;
	}

	if( !Initialize( &Stream, categoryId ) )
	{
		CDebugSet::ToLogFile( "Failed:CEtUIXML::LoadXML, File(%s), Category(%d)", szFileName, categoryId );
		return false;
	}

	return true;
}

bool CEtUIXML::Initialize( CStream *pStream, emCategoryId categoryId )
{
	if( !pStream )
	{
		CDebugSet::ToLogFile( "CEtUIXML::Initialize, pStream is NULL!" );
		return false;
	}
	
	if( categoryId != idCategory1 && categoryId != idCategory2 ) {
		return false;
	}

	m_nCurCategoryId = categoryId;	

	int nLen = pStream->Size();
	char *pBuffer = new char[nLen+1];
	SecureZeroMemory(pBuffer, nLen+1);
	pStream->Read(pBuffer, nLen);

	TiXmlDocument doc;
	doc.Parse(pBuffer, 0, TIXML_ENCODING_UTF8 );
	if ( doc.Error() )
	{
		CDebugSet::ToLogFile( "CEtUIXML::Initialize, parsing failed!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlElement* pElem = doc.RootElement();
	if ( !pElem )
	{
		CDebugSet::ToLogFile( "CEtUIXML::Initialize, Invalid Messages Element!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlNode *child(NULL);
	while( child = pElem->IterateChildren( child ) )
	{
		ParsingMessage( child->ToElement() );
	}
	delete [] pBuffer;

	return true;
}

bool CEtUIXML::Initialize( CStream *pStream, emCategoryId categoryId, int nNationId, bool bAttachData/* = false*/ )
{
	XML_NATIONSTRING_MAP_IT itor = m_mapNationString.find(nNationId);
	if (itor == m_mapNationString.end() || bAttachData )
	{
		m_nCurNationId = nNationId;
		return Initialize(pStream, categoryId);
	}
	_ASSERT_EXPR(0, L"NationId Already Exists!");
	return false;
}

//void CEtUIXML::ParsingCategory( TiXmlElement *pElement )
//{
//	if( !pElement )
//	{
//		CDebugSet::ToLogFile( "CEtUIXML::ParsingCategory, Invalid pElement!" );
//		return;
//	}
//
//	if ( TIXML_SUCCESS != pElement->QueryIntAttribute( "mid", &m_nCurCategoryId ) )
//	{
//		CDebugSet::ToLogFile( "CEtUIXML::ParsingCategory, QueryIntAttribute() failed!" );
//		return;
//	}
//
//	//switch( m_nCurCategoryId )
//	//{
//	//case idCategory1:	m_pCurMapCategory = &m_mapCategory1;	break;
//	//case idCategory2:	m_pCurMapCategory = &m_mapCategory2;	break;
//	//default:
//	//	CDebugSet::ToLogFile( "CEtUIXML::ParsingCategory, Invalid Category Id!" );
//	//	return;
//	//}
//
//	TiXmlNode *child(NULL);
//	while( child = pElement->IterateChildren( child ) )
//	{
//		ParsingMessage( child->ToElement() );
//	}
//}

void CEtUIXML::ParsingMessage( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingMessage, Invalid pElement!" );
		return;
	}

	if ( TIXML_SUCCESS != pElement->QueryIntAttribute( "mid", &m_nCurMessageId ) )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingMessage, QueryIntAttribute() failed! CategoryId:%d", m_nCurCategoryId );
		return;
	}

	const char *pText = pElement->GetText();
	if( !pText || ( strlen( pText ) == 0 ) )
	{
		OutputDebug( "CEtUIXML::ParsingMessage, pText length is 0! Category:%d, Message:%d\n", m_nCurCategoryId, m_nCurMessageId );
		return;
	}

	if( strlen( pText ) >= 4056 )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingMessage, pText length is over 4056 Category:%d, Message:%d", m_nCurCategoryId, m_nCurMessageId );
		return;
	}

	wchar_t wszBuff[4056]={0};
	MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 4056 );

	XML_STRING_MAP *pCurMapCategory = NULL;
	if (m_nCurNationId == -1)
	{
		switch( m_nCurCategoryId )
		{
		case idCategory1:	pCurMapCategory = &m_mapCategory1;	break;
		case idCategory2:	pCurMapCategory = &m_mapCategory2;	break;
		default:
			CDebugSet::ToLogFile( "CEtUIXML::Initialize, Invalid Category Id!" );
			return;
		}
	}
	else
	{
		pCurMapCategory = _GetCategoryNationString(m_nCurCategoryId, m_nCurNationId);
		if (pCurMapCategory == NULL)
		{
			TNationString NationString;
			m_mapNationString.insert(std::make_pair(m_nCurNationId, NationString));

			pCurMapCategory = _GetCategoryNationString(m_nCurCategoryId, m_nCurNationId);
			if (pCurMapCategory == NULL)
			{
				CDebugSet::ToLogFile( "CEtUIXML::Initialize, NationString Insert Failed!" );
				return;
			}
		}
	}
	
	pCurMapCategory->insert( make_pair( m_nCurMessageId, wszBuff ));

	if (m_nCurCategoryId == idCategory1)
	{
		std::wstring wstrName = wszBuff;
		std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);
		m_mapStringCategory1.insert(std::make_pair(wstrName.c_str(), m_nCurMessageId));
	}


	//TiXmlNode *child(NULL);
	//while( child = pElement->IterateChildren( child ) )
	//{
	//	ParsingLang( child->ToElement() );
	//}
}

void CEtUIXML::ParsingLang( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingLang, Invalid pElement or pMapCategory!" );
		return;
	}

	//if( !strcmp( pElement->Attribute( "id" ), GetLangId( CEtResourceMng::GetInstance().GetCodePage() ) ) )
	//{
	//	const char *pText = pElement->GetText();

	//	if( strlen( pText ) == 0 )
	//	{
	//		CDebugSet::ToLogFile( "CDnInterfaceXML::ParsingLang, pText length is 0! Category:%d, Message:%d", m_nCurCategoryId, m_nCurMessageId );
	//		return;
	//	}

	//	if( strlen( pText ) >= 2048 )
	//	{
	//		CDebugSet::ToLogFile( "CDnInterfaceXML::ParsingLang, pText length is Big! Category:%d, Message:%d", m_nCurCategoryId, m_nCurMessageId );
	//		return;
	//	}

	//	wchar_t wszBuff[2048]={0};
	//	MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 2048 );
	//	m_pCurMapCategory->insert( make_pair( m_nCurMessageId, std::wstring(wszBuff) ) );
	//}
}

const char* CEtUIXML::GetLangId( UINT nCodePage )
{
	switch( nCodePage )
	{
	case 949: return "KO";
	default:
		CDebugSet::ToLogFile( "CEtUIXML::GetLangId, Invalid Code Page : %d", nCodePage );
		return NULL;
	}

	return NULL;
}

const wchar_t* CEtUIXML::GetUIString( emCategoryId categoryId, int nMessageId, int nNationId )
{
	//rlkt
	static wchar_t *szEmpty = L"";
	static WCHAR wszMIDError[64];
	wsprintfW( wszMIDError, L"M:%d", nMessageId );

	//rlkt
	if( nMessageId == 0 )
		return szEmpty;

	XML_STRING_MAP *pCurMapCategory = NULL;
	if (m_mapCategory1.empty() && m_mapCategory2.empty() )
	{
		pCurMapCategory = _GetCategoryNationString(categoryId, nNationId == -1 ? 0 : nNationId);
		if (pCurMapCategory == NULL)
			CDebugSet::ToLogFile( "CEtUIXML::GetUIString, Invalid NationID : %d", nNationId );
	}
	else
	{
		switch( categoryId )
		{
		case idCategory1:	pCurMapCategory = &m_mapCategory1;	break;
		case idCategory2:	pCurMapCategory = &m_mapCategory2;	break;
		default: 
			CDebugSet::ToLogFile( "CEtUIXML::GetUIString, Invalid Category : %d", categoryId );
			break;
		}
	}

	if( !pCurMapCategory ) 
		return szEmpty;

	XML_STRING_MAP_ITER iter = pCurMapCategory->find( nMessageId );
	if( iter != pCurMapCategory->end() )
	{
		// UIString편집번역툴을 보니 공백으로만 이루어진 스트링이나 널('\0')만을 등록할 수 없게 되어있다.
		// 그리고, Assign 국가에 따라 국내에는 추가해도 중국쪽에만 추가를 안한다던지 할 수 있기때문에,
		// id에 따른 값자체가 존재하지 않을 수도 있다.
		const wchar_t *szUIString = iter->second.c_str();
		if( s_bCheckKoreanWord )
		{
			if( CheckKoreanWord(szUIString) )
				return wszMIDError;
		}
		return szUIString;
	}

	CDebugSet::ToLogFile( "CEtUIXML::GetUIString, Message not found! Category:%d, Message:%d", categoryId, nMessageId );
	return wszMIDError;
}

bool CEtUIXML::CheckKoreanWord( const wchar_t *szString )
{
	int i;
	int nLength = (int)wcslen( szString );
	for ( i = 0; i < nLength; ++i )
	{
		if( szString[i] > 44032 && szString[i] < 55203 )
			return true;
	}
	// 다른 코드에선 이렇게 검사하기도 한다.
	//szString[n] >= 0xAC00 && code_point <= 0xD743 

	return false;
}

int CEtUIXML::GetUIMessageID(const WCHAR * pwszString)
{
	if (pwszString == NULL) return -1;

	std::wstring wstrName = pwszString;
	std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);

	std::map<std::wstring, int>::iterator ii = m_mapStringCategory1.find(wstrName);
	if (m_mapStringCategory1.end() != ii)
		return (*ii).second;
	return -1;
}

const CEtUIXML::XML_STRING_MAP* CEtUIXML::GetCategoryList( emCategoryId categoryId )
{
	switch( categoryId )
	{
	case idCategory1:	return &m_mapCategory1;
	case idCategory2:	return &m_mapCategory2;
	default: 
		CDebugSet::ToLogFile( "CEtUIXML::GetCategoryList, Invalid Category : %d", categoryId );
		break;
	}

	return NULL;
}

bool CEtUIXML::LoadFilter( const char *szFileName )
{
	CResMngStream Stream( szFileName );
	if( !Stream.IsValid() )
	{
		CDebugSet::ToLogFile( "Failed:CEtUIXML::LoadFilter, File(%s)", szFileName );
		return false;
	}

	int nLen = Stream.Size();
	char *pBuffer = new char[nLen+1];
	SecureZeroMemory(pBuffer, nLen+1);
	Stream.Read(pBuffer, nLen);

	TiXmlDocument doc;
	doc.Parse(pBuffer, 0, TIXML_ENCODING_UTF8 );
	if ( doc.Error() )
	{
		CDebugSet::ToLogFile( "CEtUIXML::LoadFilter, parsing failed!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlElement* pElem = doc.RootElement();
	if ( !pElem )
	{
		CDebugSet::ToLogFile( "CEtUIXML::LoadFilter, Invalid RootElement!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlNode *child(NULL);

	while( child = pElem->IterateChildren( "Account", child ) )
	{
		ParsingAccountWord( child->ToElement() );
	}

	while( child = pElem->IterateChildren( "Chat", child ) )
	{
		ParsingChatWord( child->ToElement() );
	}

	while( child = pElem->IterateChildren( "Replacement", child ) )
	{
		ParsingReplaceWord( child->ToElement() );
	}

	delete [] pBuffer;
	return true;
}

bool CEtUIXML::LoadWhiteList( const char *szFileName )
{
	CResMngStream Stream( szFileName );
	if( !Stream.IsValid() )
	{
		CDebugSet::ToLogFile( "Failed:CEtUIXML::LoadWhiteList, File(%s)", szFileName );
		return false;
	}

	int nLen = Stream.Size();
	char *pBuffer = new char[nLen+1];
	SecureZeroMemory(pBuffer, nLen+1);
	Stream.Read(pBuffer, nLen);

	TiXmlDocument doc;
	doc.Parse(pBuffer, 0, TIXML_ENCODING_UTF8 );
	if ( doc.Error() )
	{
		CDebugSet::ToLogFile( "CEtUIXML::LoadWhiteList, parsing failed!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlElement* pElem = doc.RootElement();
	if ( !pElem )
	{
		CDebugSet::ToLogFile( "CEtUIXML::LoadWhiteList, Invalid RootElement!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlNode *child(NULL);

	while( child = pElem->IterateChildren( "SafeWord", child ) )
	{
		ParsingWhiteListWord( child->ToElement() );
	}

	delete [] pBuffer;
	return true;
}

CEtUIXML::XML_STRING_MAP * CEtUIXML::_GetCategoryNationString(int nCategoryId, int nNationId/* = -1*/)
{
	XML_STRING_MAP *pCurMapCategory = NULL;
	XML_NATIONSTRING_MAP_IT NationItor = m_mapNationString.find(nNationId);
	if (NationItor != m_mapNationString.end())
	{
		switch( nCategoryId )
		{
		case idCategory1: pCurMapCategory = &(*NationItor).second.mapCategory1;	break;
		case idCategory2: pCurMapCategory = &(*NationItor).second.mapCategory2;	break;
		default:
			CDebugSet::ToLogFile( "CEtUIXML::Initialize, Invalid Category Id!" );
			return pCurMapCategory;
		}
	}
	return pCurMapCategory;
}

void CEtUIXML::ParsingAccountWord( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingAccountWord, Invalid pElement!" );
		return;
	}

	TiXmlNode *child(NULL);
	TiXmlElement *pElem(NULL);

	while( child = pElement->IterateChildren( child ) )
	{
		pElem = child->ToElement();
		if( !pElem ) continue;

		const char *pText = pElem->GetText();
		if( !pText || ( strlen( pText ) == 0 ) )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingAccountWord, pText length is 0!" );
			return;
		}

		if( strlen( pText ) >= 2048 )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingAccountWord, pText length is Big!" );
			return;
		}

		wchar_t wszBuff[2048]={0};
		MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 2048 );

		std::wstring strTemp(wszBuff);
		std::transform( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

		m_vecAccountFilter.push_back( strTemp );
	}
}

void CEtUIXML::ParsingChatWord( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingChatWord, Invalid pElement!" );
		return;
	}

	TiXmlNode *child(NULL);
	TiXmlElement *pElem(NULL);

	while( child = pElement->IterateChildren( child ) )
	{
		pElem = child->ToElement();
		if( !pElem ) continue;

		const char *pText = pElem->GetText();
		if( !pText || ( strlen( pText ) == 0 ) )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingChatWord, pText length is 0!" );
			return;
		}

		if( strlen( pText ) >= 2048 )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingChatWord, pText length is Big!" );
			return;
		}

		wchar_t wszBuff[2048]={0};
		MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 2048 );

		std::wstring strTemp(wszBuff);
		std::transform( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

		m_vecChatFilter.push_back( strTemp );
	}
}

void CEtUIXML::ParsingWhiteListWord( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingWhiteListWord, Invalid pElement!" );
		return;
	}

	TiXmlNode *child(NULL);
	TiXmlElement *pElem(NULL);

	while( child = pElement->IterateChildren( child ) )
	{
		pElem = child->ToElement();
		if( !pElem ) continue;

		const char *pText = pElem->GetText();
		if( !pText || ( strlen( pText ) == 0 ) )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingWhiteListWord, pText length is 0!" );
			return;
		}

		if( strlen( pText ) >= 2048 )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingWhiteListWord, pText length is Big!" );
			return;
		}

		wchar_t wszBuff[2048]={0};
		MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 2048 );

		std::wstring strTemp(wszBuff);
		std::transform( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

		m_vecWhiteList.push_back( strTemp );
	}
}

void CEtUIXML::ParsingReplaceWord( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtUIXML::ParsingReplaceWord, Invalid pElement!" );
		return;
	}

	TiXmlNode *child(NULL);
	TiXmlElement *pElem(NULL);

	while( child = pElement->IterateChildren( child ) )
	{
		pElem = child->ToElement();
		if( !pElem ) continue;

		const char *pText = pElem->GetText();
		if( !pText || ( strlen( pText ) == 0 ) )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingReplaceWord, pText length is 0!" );
			return;
		}

		if( strlen( pText ) >= 2048 )
		{
			CDebugSet::ToLogFile( "CEtUIXML::ParsingReplaceWord, pText length is Big!" );
			return;
		}

		wchar_t wszBuff[2048]={0};
		MultiByteToWideChar(CP_UTF8, 0, pText, -1, wszBuff, 2048 );

		std::wstring strTemp(wszBuff);
		std::transform( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

		m_vecReplaceFilter.push_back( strTemp );
	}
}

//////////////////////////////////////////////////////////////////////////
// class CEtExceptionalUIXML

CEtExceptionalUIXML::CEtExceptionalUIXML()
{
}

CEtExceptionalUIXML::~CEtExceptionalUIXML()
{
	m_mapReplacementString.clear();
}

bool CEtExceptionalUIXML::LoadXML( const char *szFileName )
{
	CResMngStream Stream( szFileName );
	if( !Stream.IsValid() )
	{
		CDebugSet::ToLogFile( "Failed:CEtExceptionalUIXML::LoadXML, File(%s)", szFileName );
		return false;
	}

	if( !Initialize( &Stream ) )
	{
		CDebugSet::ToLogFile( "Failed:CEtExceptionalUIXML::LoadXML, File(%s)", szFileName );
		return false;
	}

	return true;
}

bool CEtExceptionalUIXML::Initialize( CStream *pStream )
{
	if( !pStream )
	{
		CDebugSet::ToLogFile( "CEtExceptionalUIXML::Initialize, pStream is NULL!" );
		return false;
	}

	int nLen = pStream->Size();
	char *pBuffer = new char[nLen+1];
	SecureZeroMemory( pBuffer, nLen+1 );
	pStream->Read( pBuffer, nLen );

	TiXmlDocument doc;
	doc.Parse( pBuffer, 0, TIXML_ENCODING_UTF8 );
	if( doc.Error() )
	{
		CDebugSet::ToLogFile( "CEtExceptionalUIXML::Initialize, parsing failed!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlElement* pElem = doc.RootElement();
	if( !pElem )
	{
		CDebugSet::ToLogFile( "CEtExceptionalUIXML::Initialize, Invalid Messages Element!" );
		delete [] pBuffer;
		return false;
	}

	TiXmlNode *child( NULL );
	while( child = pElem->IterateChildren( child ) )
	{
		ParsingMessage( child->ToElement() );
	}

	delete [] pBuffer;

	return true;
}

void CEtExceptionalUIXML::ParsingMessage( TiXmlElement *pElement )
{
	if( !pElement )
	{
		CDebugSet::ToLogFile( "CEtExceptionalUIXML::ParsingMessage, Invalid pElement!" );
		return;
	}

	int nIndex;
	if ( TIXML_SUCCESS != pElement->QueryIntAttribute( "index", &nIndex ) )
	{
		CDebugSet::ToLogFile( "CEtExceptionalUIXML::ParsingMessage, QueryIntAttribute() failed! Index:%d", nIndex );
		return;
	}

	std::string strCombineText;
	strCombineText = pElement->Attribute( "CombineText" );
	if( strCombineText.empty() )
	{
		CDebugSet::ToLogFile( "CEtExceptionalUIXML::CombineText is 0! Index:%d", nIndex );
		return;
	}

	const char* pText = pElement->GetText();
	if( !pText || ( strlen( pText ) == 0 ) )
	{
		OutputDebug( "CEtExceptionalUIXML::ParsingMessage, pText length is 0! Index:%d\n", nIndex );
		return;
	}

	wchar_t wszCombineText[2048]={0};
	wchar_t wszReplaceText[2048]={0};
	MultiByteToWideChar( CP_UTF8, 0, strCombineText.c_str(), -1, wszCombineText, 2048 );
	MultiByteToWideChar( CP_UTF8, 0, pText, -1, wszReplaceText, 2048 );

	std::map< std::wstring, std::wstring >::iterator iter = m_mapReplacementString.find( wszCombineText );
	if( iter == m_mapReplacementString.end() )
	{
		m_mapReplacementString.insert( std::make_pair( wszCombineText, wszReplaceText ) );
	}
}

std::wstring CEtExceptionalUIXML::GetReplacementUIString( char* szParam )
{
	std::wstring strReplacementText;

	if( static_cast<int>( m_mapReplacementString.size() ) == 0 )
		return strReplacementText;

	if( szParam == NULL || static_cast<int>( strlen( szParam ) ) == 0 )
		return strReplacementText;

	wchar_t wszBuff[2048]={0};
	MultiByteToWideChar( CP_UTF8, 0, szParam, -1, wszBuff, 2048 );

	std::map< std::wstring, std::wstring >::iterator iter = m_mapReplacementString.find( wszBuff );
	if( iter != m_mapReplacementString.end() )
	{
		strReplacementText = iter->second;
	}

	return  strReplacementText;
}

