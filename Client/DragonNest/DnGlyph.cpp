#include "StdAfx.h"
#include "DnGlyph.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_DN_SMART_PTR_STATIC( CDnGlyph, 2000 )

CDnGlyph::CDnGlyph()
: CDnUnknownRenderObject( false )
{
	m_GlyphType = GlyphTypeEnum::None;
}

CDnGlyph::~CDnGlyph()
{
}

DnGlyphHandle CDnGlyph::CreateGlyph( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	if( nTableID < 1 ) return CDnGlyph::Identity();

	DnGlyphHandle hGlyph = (new CDnGlyph)->GetMySmartPtr();
	if( !hGlyph ) return CDnGlyph::Identity();

	hGlyph->Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );
	return hGlyph;
}

bool CDnGlyph::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) return false;
	if( !pSox->IsExistItem( nTableID ) ) return false;

	m_GlyphType = (GlyphTypeEnum)pSox->GetFieldFromLablePtr( nTableID, "_GlyphType" )->GetInteger();

	CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );

	return true;
}
