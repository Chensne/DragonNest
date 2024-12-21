#include "StdAfx.h"
#include "shlobj.h"
#include "DnLocalDataMgr.h"
#include "DnMainFrame.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define MAX_CHAR_PER_LINE_IN_LOCALDATA 1024

CDnLocalDataMgr::CDnLocalDataMgr()
	: m_FileName(L"LocalData.dtx")
{
#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	m_bForceGraphicOptionLow = true;
#endif
}

CDnLocalDataMgr::~CDnLocalDataMgr()
{
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	m_vecChatOption.clear();
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
}

bool CDnLocalDataMgr::Load()
{
	FILE *stream = NULL;

	WCHAR wszFilename[MAX_PATH];

	TCHAR wszPath[MAX_PATH]={0};
	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), wszPath, CSIDL_PERSONAL, 0);

#ifdef _FINAL_BUILD
	wsprintf(wszFilename, L"%s\\DragonNest", wszPath);
	_wmkdir(wszFilename);
	wsprintf(wszFilename, L"%s\\DragonNest\\Config", wszPath);
	_wmkdir(wszFilename);
	wsprintf(wszFilename, L"%s\\DragonNest\\Config\\%s", wszPath, m_FileName.c_str());
#else
	wsprintf(wszFilename, L".\\%s", m_FileName.c_str());
#endif

	if (_wfopen_s(&stream, wszFilename, L"rb") != 0)
		return false;

	fseek( stream, 0L, SEEK_SET );

	WORD mark;
	fread(&mark, sizeof(WORD), 1, stream);

	WCHAR wszString[MAX_CHAR_PER_LINE_IN_LOCALDATA] = {0, };
	while(true)
	{
		int numRead = fwscanf_s(stream, L"%s", wszString, _countof(wszString));
		if (numRead != 1)
			break;
		SetValue(stream , wszString);
	}

	fclose(stream);

	return true;
}

bool CDnLocalDataMgr::SetValue(FILE *stream , WCHAR* wszString)
{
	if (wszString == NULL)
		return false;

	int numRead = 0;

#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	if (wcsstr(wszString, L"SetGraphicOptionAtOnce"))
	{
		int setGraphicOption = 0;
		numRead = fwscanf_s(stream, L"%d", &setGraphicOption);
		if (numRead == 1)
			m_bForceGraphicOptionLow = ((setGraphicOption == 1) ? true : false);
		return true;
	}
#endif // PRE_MOD_IDN_GRAPHICOPTION_LOW
#ifdef PRE_PARTY_DB
	if (wcsstr(wszString, L"PartySearchHistoryWords"))
	{
		WCHAR historyWords[Party::Max::SearchWordLen * Party::Max::SearchWordHistory] = {0,};
		numRead = fwscanf_s(stream, L"%[^\n]", historyWords, _countof(historyWords));
		if (numRead == 1)
			TokenizeW(std::wstring(historyWords), m_PartySearchHistoryList, L",");
		return true;
	}
#endif
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	if( wcsstr( wszString, L"ChatOption" ) )
	{
		WCHAR ChatOption[MAX_CHAT_TAB_TYPE * MAX_CHAT_TAB_CHARACTER] = {0,};
		numRead = fwscanf_s( stream, L"%[^\n]", ChatOption, _countof(ChatOption) );
		if( numRead == 1 )
		{
			std::vector<std::wstring> vecChatOption;
			TokenizeW( std::wstring(ChatOption), vecChatOption, L"," );

			for( int i=0; i<static_cast<int>( vecChatOption.size() ); i++ )
			{
				int nChatTypeValue = _wtoi( vecChatOption[i].c_str() );
				m_vecChatOption.push_back( nChatTypeValue );
			}
		}
		return true;
	}
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	return false;
}

bool CDnLocalDataMgr::Save()
{
	FILE *stream = NULL;
	WCHAR wszFilename[MAX_PATH];

#ifdef _FINAL_BUILD
	TCHAR wszPath[MAX_PATH]={0};
	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), wszPath, CSIDL_PERSONAL, 0);

	wsprintfW(wszFilename, L"%s\\DragonNest", wszPath);
	_wmkdir(wszFilename);
	wsprintfW(wszFilename, L"%s\\DragonNest\\Config", wszPath);
	_wmkdir(wszFilename);
	wsprintfW(wszFilename, L"%s\\DragonNest\\Config\\%s", wszPath, m_FileName.c_str());
#else
	wsprintfW(wszFilename, L".\\%s", m_FileName.c_str());
#endif

	if (GetFileAttributesW(m_FileName.c_str()) != INVALID_FILE_ATTRIBUTES)
		SetFileAttributesW(m_FileName.c_str(), FILE_ATTRIBUTE_NORMAL);

	if (_wfopen_s( &stream, wszFilename, L"wb" ) != 0)
		return false;

	fseek(stream, 0L, SEEK_SET);

	WORD mark = 0xFEFF;
	fwrite(&mark, sizeof(WORD), 1, stream);

#ifdef PRE_PARTY_DB
	std::wstring wordsSaved;
	std::deque<std::wstring>::const_iterator iter = m_PartySearchHistoryList.begin();
	for (; iter != m_PartySearchHistoryList.end(); ++iter)
	{
		const std::wstring& words = (*iter);
		if (iter == m_PartySearchHistoryList.begin())
			wordsSaved = FormatW(L"PartySearchHistoryWords %s", words.c_str());
		else
			wordsSaved += FormatW(L",%s", words.c_str());
	}

	#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	if (wordsSaved.empty() == false)
		fwprintf(stream, L"%s\n", wordsSaved.c_str());
	#else
	fwprintf(stream, L"%s\n", wordsSaved.c_str());
	#endif
#endif

#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	fwprintf(stream, L"SetGraphicOptionAtOnce %d\n", ((m_bForceGraphicOptionLow) ? 1 : 0));
#endif

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	std::wstring strChatOption;
	for( int i=0; i<static_cast<int>( m_vecChatOption.size() ); i++ )
	{
		if( i == 0 )
			strChatOption = FormatW( L"ChatOption %d", m_vecChatOption[i] );
		else
			strChatOption += FormatW( L",%d", m_vecChatOption[i] );
	}

	if( !strChatOption.empty() )
		fwprintf( stream, L"%s\n", strChatOption.c_str() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	fclose(stream);

	return true;
}

#ifdef PRE_PARTY_DB
void CDnLocalDataMgr::SetPartySearchHistoryWord(const std::wstring& word)
{
	if (m_PartySearchHistoryList.size() >= Party::Max::SearchWordHistory)
		m_PartySearchHistoryList.pop_back();

	if (m_PartySearchHistoryList.empty() == false)
	{
		const std::wstring& lastWord = m_PartySearchHistoryList.front();
		if (lastWord.compare(word) == 0)
			return;
	}

	m_PartySearchHistoryList.push_front(word);
}
#endif

#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
void CDnLocalDataMgr::SetForceGraphicOptionLow(bool bForceGraphicOptionLow)
{
	m_bForceGraphicOptionLow = bForceGraphicOptionLow;
}
#endif

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
bool CDnLocalDataMgr::IsExistChatOption( int nChatTabType )
{
	if( m_vecChatOption.empty() || static_cast<int>( m_vecChatOption.size() ) <= nChatTabType )
		return false;

	return true;
}

bool CDnLocalDataMgr::GetChatOptionValue( int nChatTabType, int nChatType )
{
	if( m_vecChatOption.empty() )
		return false;

	if( nChatTabType >= static_cast<int>( m_vecChatOption.size() ) )
		return false;

	return m_vecChatOption[nChatTabType] & nChatType;
}

void CDnLocalDataMgr::SetChatOptionValue( int nChatTabType, int nChatType, bool bChatTypeValue )
{
	if( m_vecChatOption.empty() || static_cast<int>( m_vecChatOption.size() ) <= nChatTabType )
		return;

	bChatTypeValue ? m_vecChatOption[nChatTabType] |= nChatType : m_vecChatOption[nChatTabType] &= ~nChatType;
}

void CDnLocalDataMgr::SetChatOption( int nChatTabType, int nChatTypeValue )
{
	if( m_vecChatOption.empty() )
	{
		for( int i=0; i<MAX_CHAT_TAB_TYPE; i++ )
			m_vecChatOption.push_back( 0 );
	}

	m_vecChatOption[nChatTabType] = nChatTypeValue;
}
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
