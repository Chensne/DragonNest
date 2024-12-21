#pragma once

#include "DnGlyph.h"

class MAPlateUser
{
public:
	MAPlateUser();
	virtual ~MAPlateUser();

protected:
	DnGlyphHandle m_hGlyph[CDnGlyph::GlyphSlotEnum_Amount];
	bool m_bSelfDelete[CDnGlyph::GlyphSlotEnum_Amount];

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual bool AttachGlyph( DnGlyphHandle hGlyph, CDnGlyph::GlyphSlotEnum Index, bool bDelete = false );
	virtual bool DetachGlyph( CDnGlyph::GlyphSlotEnum Index );

	DnGlyphHandle GetGlyph( CDnGlyph::GlyphSlotEnum Index ) { return m_hGlyph[Index]; }
};