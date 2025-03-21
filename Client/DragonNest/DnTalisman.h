#pragma once
#include "DnUnknownRenderObject.h"
#include "DnItem.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM)

class CDnTalisman : public CDnUnknownRenderObject< CDnTalisman >,
					public CDnItem
{
public:
	CDnTalisman();
	virtual ~CDnTalisman();

	enum eTalismanType {
		TYPE_STATE_UP = 0,
		TYPE_DROP_PERCENT_UP,
		TYPE_GOLD_PERCENT_UP
	};

protected:
	eTalismanType m_ItemType;   // 탈리스만 기본 타입 ( 0- 능력치 , 1- 매직찬스 , 2- 골드찬스 )
	int			  m_nPeriod;	// 탈리스만이 활성화 되는 기간.

public:
	virtual	bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );
	static	DnTalismanHandle CreateTalisman( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );

	int				GetTalismanPeriod()	{ return m_nPeriod;  }
	eTalismanType	GetTalismanType()	{ return m_ItemType; }
};

#endif // PRE_ADD_TALISMAN_SYSTEM