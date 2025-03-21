#pragma once

#include "DnItem.h"
#include "DnUnknownRenderObject.h"

class CDnGlyph : public CDnUnknownRenderObject< CDnGlyph, MAX_SESSION_COUNT >, public CDnItem {
public:
	CDnGlyph( CMultiRoom *pRoom );
	virtual ~CDnGlyph();

	enum GlyphTypeEnum {
		None,
		Enchant,
		PassiveSkill,
		ActiveSkill,
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
		DragonFellowship = 5,
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	};

	enum GlyphSlotEnum {
		Enchant1,
		Enchant2,
		Enchant3,
		Enchant4,
		Enchant5,
		Enchant6,
		Enchant7,
		Enchant8,
		Skill1,
		Skill2,
		Skill3,
		Skill4,
		SpecialSkill1,
		Cash1,
		Cash2,
		Cash3,
#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
		Dragon1,
		Dragon2,
		Dragon3,
		Dragon4,
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)

		GlyphSlotEnum_Amount,
	};

protected:
	GlyphTypeEnum m_GlyphType;

public:
	static DnGlyphHandle CreateGlyph( CMultiRoom *pRoom, int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false );

public:
	virtual bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false);

	GlyphTypeEnum GetGlyphType() { return m_GlyphType; }

};