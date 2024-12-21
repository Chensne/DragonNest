#pragma once
#include "EtUIType.h"

struct SUIElement_01
{
	enum { VERSION = 0x101 };

	SUIColor TextureColor;
	SUIColor FontColor;
	SUICoord UVCoord;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	int nFontWeight;
	bool bShadowFont;
	bool bItalic;
};

struct SUIElement_02
{
	enum { VERSION = 0x102 };

	SUIColor TextureColor;
	SUIColor FontColor;
	SUICoord UVCoord;
	DWORD dwFontFormat;
	int nFontIndex;
	bool bShadowFont;
};

struct SUIElement_03
{
	enum { VERSION = 0x103 };

	SUIColor TextureColor;
	SUIColor FontColor;
	SUICoord UVCoord;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
};

struct SUIElement_04
{
	enum { VERSION = 0x104 };

	SUIColor TextureColor;
	SUIColor FontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
};

struct SUIElement_05
{
	enum { VERSION = 0x105 };

	SUIColor TextureColor;
	SUIColor FontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	float fDelayTime;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
};

struct SUIElement_06
{
	enum { VERSION = 0x106 };

	SUIColorEx TextureColor;
	SUIColorEx FontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	float fDelayTime;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
};

struct SUIElement_07
{
	enum { VERSION = 0x107 };

	SUIColorEx TextureColor;
	SUIColorEx FontColor;
	SUIColorEx ShadowFontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	float fDelayTime;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
};


struct SUIElement_08
{
	enum { VERSION = 0x108 };

	SUIColorEx TextureColor;
	SUIColorEx FontColor;
	SUIColorEx ShadowFontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	float fDelayTime;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
	float fFontHoriOffset;
	float fFontVertOffset;
};

struct SUIElement_09
{
	enum { VERSION = 0x109 };

	SUIColorEx TextureColor;
	SUIColorEx FontColor;
	SUIColorEx ShadowFontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	float fDelayTime;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
	float fFontHoriOffset;
	float fFontVertOffset;
	int nDrawType;
	int nWeight;
	float fAlphaWeight;
};

struct SUIElement_10
{
	enum { VERSION = 0x10a };

	SUIColorEx TextureColor;
	SUIColorEx FontColor;
	SUIColorEx ShadowFontColor;
	SUICoord UVCoord;
	float fTextureWidth;
	float fTextureHeight;
	float fDelayTime;
	DWORD dwFontFormat;
	int nFontIndex;
	int nFontHeight;
	bool bShadowFont;
	float fFontHoriOffset;
	float fFontVertOffset;
	int nDrawType;
	int nWeight;
	float fAlphaWeight;
	float fGlobalBlurAlphaWeight;
};