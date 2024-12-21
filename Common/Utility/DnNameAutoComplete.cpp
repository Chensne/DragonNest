
#include "stdafx.h"
#include "DnNameAutoComplete.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnUIString.h"

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

CNameAutoComplete::CNameAutoComplete()
{
}

CNameAutoComplete::~CNameAutoComplete()
{
	m_mNameItemList.clear();
}

void CNameAutoComplete::LoadData(int nLimit)
{	
	if (m_mNameItemList.empty() == false)
		return;

	DNTableFileFormat *pItemSox = GetDNTable( CDnTableDB::TITEM );

	int _NameID = pItemSox->GetFieldNum( "_NameID" );
	int _NameIDParam = pItemSox->GetFieldNum( "_NameIDParam" );
	int _Type = pItemSox->GetFieldNum( "_Type" );
	int _Rank = pItemSox->GetFieldNum( "_Rank" );
	int _Reversion = pItemSox->GetFieldNum( "_Reversion" );
	int _LevelLimit = pItemSox->GetFieldNum( "_LevelLimit" );
	int _IsUse = pItemSox->GetFieldNum( "_isUse" );

	int nItemID = 0;
	int nNameID = 0;
	int nType = 0;
	int nRank = 0;
	int nReversion = 0;
	int nLevelLimit = 0;
	bool bIsUse = false;

	char *szNameParam = NULL;
	wstring wszName;
	std::vector <std::pair<std::wstring, int>> vPartialNames;
	std::vector <std::pair<std::wstring, int>>::iterator itorPartial;
	std::vector<std::wstring> tokens;
	for (int i = 0; i < pItemSox->GetItemCount(); i++)
	{
		wszName.clear();
		vPartialNames.clear();

		nItemID = pItemSox->GetItemID(i);
		int iIdx = pItemSox->GetIDXprimary( nItemID );

		nNameID = pItemSox->GetFieldFromLablePtr( iIdx, _NameID )->GetInteger();
		szNameParam = pItemSox->GetFieldFromLablePtr( iIdx, _NameIDParam )->GetString();
		nType = pItemSox->GetFieldFromLablePtr( iIdx, _Type )->GetInteger();
		nRank = pItemSox->GetFieldFromLablePtr( iIdx, _Rank )->GetInteger();
		nReversion = pItemSox->GetFieldFromLablePtr( iIdx, _Reversion )->GetInteger();
		nLevelLimit = pItemSox->GetFieldFromLablePtr( iIdx, _LevelLimit )->GetInteger();
		bIsUse = pItemSox->GetFieldFromLablePtr( iIdx, _IsUse )->GetBool();

		if (bIsUse == false)
			continue;

		if (nLevelLimit > nLimit)
			continue;		

		MakeUIStringUseVariableParam( wszName, nNameID, szNameParam, &vPartialNames );

		if (_CheckExceptCharacter(wszName))
			continue;

		if (_CheckOnlyNumber(wszName.c_str()))
			continue;

		if (nReversion != ITEMREVERSION_GUILD)
			_InsertItem(wszName, nNameID, nType, nRank, nItemID);
		
		tokens.clear();
		TokenizeW(wszName, tokens, L" ");
		for (int tokencnt = 0; tokencnt < (int)tokens.size(); tokencnt++)
		{
			int nMessageId = GetEtUIXML().GetUIMessageID(tokens[tokencnt].c_str());
			if (nMessageId != -1)
			{
				if (_CheckExceptCharacter(tokens[tokencnt]))
					continue;

				if (_CheckOnlyNumber(tokens[tokencnt].c_str()))
					continue;

				_InsertItem(tokens[tokencnt], nMessageId, -1, -1, 0);
			}
		}
	}
}

void CNameAutoComplete::GetMatchNameList(const WCHAR * pwszSrc, std::vector <std::pair<TNameAutoItem*, std::vector<TNameAutoItem*>>> &vList)
{
	if (pwszSrc == NULL) return;

	std::wstring wstrSrc(pwszSrc);
	std::transform(wstrSrc.begin(), wstrSrc.end(), wstrSrc.begin(), towlower);
	size_t pwszSrcLen = wcslen(pwszSrc);

	std::vector<std::wstring> tokens;
	TokenizeW(pwszSrc, tokens, L" ");

	bool bFirstMatched = false;
	std::vector<std::wstring>::iterator ipartial;
	std::map <std::wstring, TNameAutoItem>::iterator ii;
	std::vector<TNameAutoItem*> vPartialList;

	if (tokens.size() > 1)
	{
		for (int nPartial = 0; nPartial < static_cast<int>(tokens.size()); nPartial++)
		{
			if (nPartial != static_cast<int>(tokens.size()) - 1)
			{
				ii = m_mNameItemList.find(tokens[nPartial]);
				if (ii == m_mNameItemList.end()) break;

				if (!wcsicmp((*ii).first.c_str(), tokens[nPartial].c_str()))
					vPartialList.push_back(&(*ii).second);
			}
			else
			{
				for (ii = m_mNameItemList.begin(); ii != m_mNameItemList.end(); ii++)
				{
					if (tokens[nPartial].size() > (*ii).first.size()) continue;
													
					std::wstring::size_type pos = (*ii).first.find_first_of(L" ");
					if (pos != static_cast<size_t>(-1)) continue;
					
					if (!_wcsnicmp((*ii).first.c_str(), tokens[nPartial].c_str(), tokens[nPartial].size()))
					{
						vPartialList.push_back(&(*ii).second);
						vList.push_back(std::make_pair(reinterpret_cast<TNameAutoItem*>(NULL), vPartialList));
						break;
					}
				}

				if (ii == m_mNameItemList.end())
					vList.push_back(std::make_pair(reinterpret_cast<TNameAutoItem*>(NULL), vPartialList));
			}
		}
	}

	if (vList.empty() == false)
	{
		bool bErase = false;
		std::vector <std::pair<TNameAutoItem*, std::vector<TNameAutoItem*>>>::iterator iVerify;
		for (iVerify = vList.begin(); iVerify != vList.end(); )
		{
			if ((*iVerify).first == NULL)
			{
				std::wstring wstrVerify;
				for (int nCheck = 0; nCheck < static_cast<int>((*iVerify).second.size()); nCheck++)
				{
					wstrVerify += (*iVerify).second[nCheck]->wstrName;
					if (nCheck != static_cast<int>((*iVerify).second.size()) - 1)
						wstrVerify += L" ";
				}

				if (wstrVerify.empty() == false)
				{
					if (_VerifyItem(wstrVerify) == false || static_cast<int>(wstrVerify.size()) < pwszSrcLen)
					{
						iVerify = vList.erase(iVerify);
						bErase = true;
					}
				}
			}

			if (bErase == false)
				iVerify++;

			bErase = false;
		}
	}

	vPartialList.clear();
	for (ii = m_mNameItemList.begin(); ii != m_mNameItemList.end(); ii++)
	{
		if (pwszSrcLen > (*ii).first.size()) continue;
		if (wcsstr((*ii).first.c_str(), wstrSrc.c_str()))
			vList.push_back(std::make_pair(&(*ii).second, vPartialList));
	}
}

const CNameAutoComplete::TNameAutoItem * CNameAutoComplete::GetAutoNameData(const WCHAR * pwszSrc, std::vector<TNameAutoItem*> &vList)
{
	if (pwszSrc == NULL) return NULL;

	std::wstring wstrName = pwszSrc;
	std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);
	size_t pwszSrcLen = wcslen(pwszSrc);

	std::map <std::wstring, TNameAutoItem>::iterator ii = m_mNameItemList.find(wstrName);
	if (ii != m_mNameItemList.end())
		return &(*ii).second;

	std::vector<std::wstring> tokens;
	TokenizeW(pwszSrc, tokens, L" ");

	bool bFirstMatched = false;
	std::vector<std::wstring>::iterator ipartial;

	if (tokens.size() > 1)
	{
		for (int nPartial = 0; nPartial < static_cast<int>(tokens.size()); nPartial++)
		{
			if (nPartial != static_cast<int>(tokens.size()) - 1)
			{
				ii = m_mNameItemList.find(tokens[nPartial]);
				if (ii == m_mNameItemList.end()) break;

				if (!wcsicmp((*ii).first.c_str(), tokens[nPartial].c_str()))
					vList.push_back(&(*ii).second);
			}
			else
			{
				for (ii = m_mNameItemList.begin(); ii != m_mNameItemList.end(); ii++)
				{
					if (tokens[nPartial].size() > (*ii).first.size()) continue;
													
					std::wstring::size_type pos = (*ii).first.find_first_of(L" ");
					if (pos != static_cast<size_t>(-1)) continue;
					
					if (!_wcsnicmp((*ii).first.c_str(), tokens[nPartial].c_str(), tokens[nPartial].size()))
					{
						vList.push_back(&(*ii).second);
						break;
					}
				}
			}
		}
	}
	return NULL;
}

bool CNameAutoComplete::_InsertItem(std::wstring &wstrName, int nNameID, int nItemType, int nItemRank, int nItemID)
{
	return _InsertItem(wstrName.c_str(), nNameID, nItemType, nItemRank, nItemID);
}

bool CNameAutoComplete::_InsertItem(const WCHAR * pwszName, int nNameID, int nItemType, int nItemRank, int nItemID)
{
	if (pwszName == NULL || wcslen(pwszName) <= 0)
		return false;
	if (_IsExistItem(pwszName))
		return false;

	std::wstring wstrName = pwszName;
	std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);
	TNameAutoItem NameInfo;
	NameInfo.wstrName = wstrName;
	NameInfo.nNameID = nNameID;
	NameInfo.nItemType = nItemType;
	NameInfo.nItemRank = nItemRank;
	NameInfo.nItemID = nItemID;

	m_mNameItemList.insert(std::make_pair(wstrName, NameInfo));
	return true;
}

bool CNameAutoComplete::_IsExistItem(const WCHAR * pwszSrc)
{
	if (pwszSrc == NULL) return NULL;

	std::wstring wstrName = pwszSrc;
	std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);

	std::map <std::wstring, TNameAutoItem>::iterator ii = m_mNameItemList.find(wstrName);
	if (ii != m_mNameItemList.end())
		return true;
	return false;
}

bool CNameAutoComplete::_VerifyItem(std::wstring &wstrName)
{
	std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);
	std::map <std::wstring, TNameAutoItem>::iterator ii;
	for (ii = m_mNameItemList.begin(); ii != m_mNameItemList.end(); ii++)
	{
		if (wcsstr((*ii).second.wstrName.c_str(), wstrName.c_str()))
			return true;
	}
	return false;
}

bool CNameAutoComplete::_CheckOnlyNumber(const WCHAR * pwszCheck)
{
	if (pwszCheck == NULL) return false;
	wchar_t wszChar;
	for (int i = 0; i < (int)wcslen(pwszCheck); i++)
	{
		wszChar = pwszCheck[i];
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_NUMBERS_START, UNICODE_TYPE_NUMBERS_END, wszChar) == false)
			return false;
	}
	return true;
}

bool CNameAutoComplete::_CheckExceptCharacter(std::wstring &wstrCheck)
{
	if (wstrCheck.size() <= 1) return true;

	std::wstring wstrName = wstrCheck;
	std::transform(wstrName.begin(), wstrName.end(), wstrName.begin(), towlower);

	if (wstrName.size() <= 0 || wstrName[0] == ' ' || wstrName[0] == '+' || (wstrName[0] == '/' && wstrName[1] == '/'))
		return true;

	if (wcsstr(wstrName.c_str(), L"m:"))
		return true;

	wchar_t wszChar;
	for (int i = 0; i < (int)wstrName.size(); i++)
	{
		wszChar = wstrName[i];
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_EXCEPTPERCENT, UNICODE_TYPE_EXCEPTPERCENT, wszChar)) return true;
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_EXCEPT_CHAR1_START, UNICODE_TYPE_EXCEPT_CHAR1_END, wszChar)) return true;
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_EXCEPT_CHAR2, UNICODE_TYPE_EXCEPT_CHAR2, wszChar)) return true;
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_EXCEPT_CHAR3, UNICODE_TYPE_EXCEPT_CHAR3, wszChar)) return true;
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_EXCEPT_CHAR4, UNICODE_TYPE_EXCEPT_CHAR4, wszChar)) return true;
		if (IS_IN_UNICODERANGE(UNICODE_TYPE_KOREAN_START, UNICODE_TYPE_KOREAN_END, wszChar)) return true;
	}
	return false;
}

#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE