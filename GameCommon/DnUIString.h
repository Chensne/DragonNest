#pragma once

// ���������� ���� ����ؾ��ؼ� DnInterfaceString���� ���ԵǾ���.

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


// ���ڿ��Ľ� - ���� ���ڿ��� ���˿� �°� ���� ���ڿ��� �����ͼ� ����.
//
// :  "{0} {1} óġ�ô޼�" | "{������},{abc}" => ������ abc óġ�ô޼�
// 
std::wstring ParseUIParamString( int destID, char * pStrParam );