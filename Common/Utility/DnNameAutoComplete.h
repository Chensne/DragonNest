
#pragma once

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

#define UNICODE_TYPE_NUMBERS_START		0x0030
#define UNICODE_TYPE_NUMBERS_END		0x0039
#define UNICODE_TYPE_EXCEPTPERCENT		0x0025
#define UNICODE_TYPE_EXCEPT_CHAR1_START	0x0028
#define UNICODE_TYPE_EXCEPT_CHAR1_END	0x0029
#define UNICODE_TYPE_EXCEPT_CHAR2		0x0027
#define UNICODE_TYPE_EXCEPT_CHAR3		0x002D
#define UNICODE_TYPE_EXCEPT_CHAR4		0x002F
#define UNICODE_TYPE_KOREAN_START		0xAC00
#define UNICODE_TYPE_KOREAN_END			0xD79F

#define IS_IN_UNICODERANGE(nMin, nMax, nValue)		(nMin <= nValue && nMax >= nValue)

class CNameAutoComplete : public CSingleton<CNameAutoComplete>
{
public:
	CNameAutoComplete();
	virtual ~CNameAutoComplete();

	struct TNameAutoItem
	{
		std::wstring wstrName;
		int nNameID;
		int nItemType;		// -1 일경우 완전한 아이템 이름이 아님
		int nItemRank;		// -1 일경우 완전한 아이템 이름이 아님
		int nItemID;

		TNameAutoItem()
		{
			wstrName.clear();
			nNameID = 0;
			nItemRank = -1;
			nItemType = -1;
			nItemID = -1;
		}
	};

	void LoadData(int nLimit);	

	void GetMatchNameList(const WCHAR * pwszSrc, std::vector <std::pair<TNameAutoItem*, std::vector<TNameAutoItem*>>> &vList);
	const TNameAutoItem * GetAutoNameData(const WCHAR * pwszSrc, std::vector<TNameAutoItem*> &vList);

private:
	std::map <std::wstring, TNameAutoItem> m_mNameItemList;

	bool _InsertItem(std::wstring &wstrName, int nNameID, int nItemType = -1, int nItemRank = -1, int nItemID = -1);
	bool _InsertItem(const WCHAR * pwszName, int nNameID, int nItemType = -1, int nItemRank = -1, int nItemID = -1);
	bool _IsExistItem(const WCHAR * pwszSrc);
	bool _VerifyItem(std::wstring &wstrName);

	bool _CheckOnlyNumber(const WCHAR * pwszCheck);
	bool _CheckExceptCharacter(std::wstring &wstrCheck);

	class _OderPredicater
	{
	public:
		_OderPredicater(const WCHAR * pwszSrc)
		{
			m_pwszSrc = pwszSrc;
			m_SrcSize = wcslen(m_pwszSrc);
		}

		bool operator() (CNameAutoComplete::TNameAutoItem * lhs, CNameAutoComplete::TNameAutoItem * rhs)
		{
			std::wstring wstrlhs, wstrrhs;
			wstrlhs = lhs->wstrName.substr(0, m_SrcSize);
			wstrrhs = rhs->wstrName.substr(0, m_SrcSize);
			if (wcsstr(wstrlhs.c_str(), m_pwszSrc) && wcsstr(wstrrhs.c_str(), m_pwszSrc) == NULL)
				return true;
			else if (wcsstr(wstrlhs.c_str(), m_pwszSrc) == NULL && wcsstr(wstrrhs.c_str(), m_pwszSrc))
				return false;
			return true;
		}
	private:
		const WCHAR * m_pwszSrc;
		size_t m_SrcSize;
	};
};

#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE