#pragma once

#include "DnItem.h"
#include "EtUINameLinkInterface.h"

class CDnNameLinkMng : public CEtUINameLinkInterface
{
public:
	struct NameLinkItemInfo
	{
		std::wstring	name;
		DWORD			color;
	};

	CDnNameLinkMng();
	virtual ~CDnNameLinkMng() {}

	void MakeNameLinkString_UserName(std::wstring& res, const std::wstring& userName);

	void MakeNameLinkString_Skill(std::wstring& res, const CDnSkill& skill);
	void MakeSkillNameLinkInfo(std::wstring& res, const CDnSkill& itemInfo);

	void MakeNameLinkString_Item(std::wstring& res, const CDnItem& item);
	void MakeNameLinkString_Item(std::wstring& res, const TItem& itemInfo);
#ifdef PRE_ADD_DMIX_DESIGNNAME
	#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		void MakeItemNameLinkInfo(std::wstring& res, ITEMCLSID classId, int enchantLevel, int seed, int durability, 
			int potentialIndex, int optionIndex, int sealCount, bool bSoulBound, bool bEternity, ITEMCLSID lookItemId, int potentialMoveCount);
	#else
		void MakeItemNameLinkInfo(std::wstring& res, ITEMCLSID classId, int enchantLevel, int seed, int durability, 
			int potentialIndex, int optionIndex, int sealCount, bool bSoulBound, bool bEternity, ITEMCLSID lookItemId);
	#endif
#else
	#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		void MakeItemNameLinkInfo(std::wstring& res, ITEMCLSID classId, int enchantLevel, int seed, int durability, 
			int potentialIndex, int optionIndex, int sealCount, bool bSoulBound, bool bEternity, int potentialMoveCount);
	#else
		void MakeItemNameLinkInfo(std::wstring& res, ITEMCLSID classId, int enchantLevel, int seed, int durability, 
			int potentialIndex, int optionIndex, int sealCount, bool bSoulBound, bool bEternity);
	#endif
#endif
	CDnItem* MakeItem(const std::wstring& tagString);

	WCHAR GetNameLinkSeperator() const;

protected:
	virtual bool OnParseTextItemInfo(const std::wstring& argString);

private:
	TItemInfo	m_MakeLinkItemInfo;
	CDnItem*	m_pLinkItem;
};