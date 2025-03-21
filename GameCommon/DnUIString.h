#pragma once

// 서버에서도 같이 사용해야해서 DnInterfaceString에서 빼게되었다.

#ifdef PRE_ADD_WORD_GENDER
#define ADJECTIVE_GENDER_TAGE L"@gender:"
#define NOUN_GENDER_TAGE L"#gender:"

void ChangeWordOrder( std::vector<std::string>& vecParam );
std::wstring ApplyWordGenderString( bool bIsAdjective, std::wstring& wszParamString, std::vector<std::string>& vecParam );
#endif // PRE_ADD_WORD_GENDER

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
void MakeUIStringUseVariableParamByBaseMsg( std::wstring &wszStr, std::wstring& wszBaseMessage, char *szParam, std::vector <std::pair<std::wstring, int>> * pvList = NULL );
void MakeUIStringUseVariableParam( std::wstring &wszStr, int nMessageId, char *szParam, std::vector <std::pair<std::wstring, int>> * pvList = NULL );
#else		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
void MakeUIStringUseVariableParamByBaseMsg( std::wstring &wszStr, std::wstring& wszBaseMessage, char *szParam );
void MakeUIStringUseVariableParam( std::wstring &wszStr, int nMessageId, char *szParam );
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#if defined(PRE_ADD_ITEM_GAINTABLE) && defined(_CLIENT)
void MakeUIStringItemGainDescription(std::wstring& result, const std::string& baseMessage);
#endif


// 문자열파싱 - 앞의 문자열의 포맷에 맞게 뒤의 문자열을 가져와서 결합.
//
// :  "{0} {1} 처치시달성" | "{가나다},{abc}" => 가나다 abc 처치시달성
// 
std::wstring ParseUIParamString( int destID, char * pStrParam );