#pragma once

struct irange {
	int nMin;
	int nMax;
	irange() { nMin = nMax = 0; }
	irange( int Min, int Max ) { nMin = Min; nMax = Max; }
};

struct frange {
	float fMin;
	float fMax;
	frange() { fMin = fMax = 0.f; }
	frange( float Min, float Max ) { fMin = Min; fMax = Max; }
};

#define UM_PROPERTYGRID_ONCUSTOM_DIALOG WM_USER + 5002