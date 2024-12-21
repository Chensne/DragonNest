#pragma once
#include "EtUITextBoxData.h"

class CEtUINameLinkInterface;
class CEtUINameLinkMgr
{
public:
	enum eNameLinkTagType
	{
		eNLTAG_NONE,
		eNLTAG_START,
		eNLTAG_END,
		eNLTAG_COLOR,
		eNLTAG_RESTORECOLOR,
		eNLTAG_ITEMINFO,
		eNLTAG_USERNAME,
		eNLTAG_SKILL,
	};

	enum eNLItemInfoCategory
	{
		eNLITEM_CLASSID,
		eNLITEM_ENCHANTLEVEL,
		eNLITEM_SEED,
		eNLITEM_DURABILITY,
		eNLITEM_POTENTIALIDX,
		eNLITEM_OPTIONIDX,
		eNLITEM_SEALCOUNT,
		eNLITEM_MAX
	};

	CEtUINameLinkMgr();
	virtual ~CEtUINameLinkMgr(void) {}

	bool				ParseText(std::wstring& parsee, int& tagOffset, CEtUINameLinkInterface* pInterface = NULL) const;
	bool				ParseText(SWORD_PROPERTY& prop, std::wstring& parsee, int& tagOffset) const;

	bool				GetTagOffsetByParseText(const std::wstring& parsee, int& tagOffset) const;
	WCHAR				GetNameLinkSeperator() const;
	WCHAR				GetNameLinkTagCloser() const;
	int					GetCmdLength() const 		{ return 1; }

	int					TranslateText(std::wstring& translated, const WCHAR* originalStr, CEtUINameLinkInterface* pInterface = NULL) const;
	int					TranslateText(std::vector<CWord>& translatedWords, const SWORD_PROPERTY& basicProp, const WCHAR* originalStr) const;

	bool				MakeTag(std::wstring& result, eNameLinkTagType type) const;
	bool				AppendTag(std::wstring& result, eNameLinkTagType type) const;

	bool				IsTagType(const std::wstring& text, int pos, eNameLinkTagType type) const;
	eNameLinkTagType	IsTagType(const std::wstring& text, int pos) const;

private:
	void				PasteTextPiece(std::wstring& buffer, const std::wstring& piece) const;
};