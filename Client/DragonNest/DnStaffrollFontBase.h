#pragma once

class CDnStaffrollFontBase {
public:
	CDnStaffrollFontBase() {}
	virtual ~CDnStaffrollFontBase() {}

public:
	virtual void SetText( const WCHAR *wszStr ) {}
	virtual void SetAlpha( float fAlpha ) {}
};