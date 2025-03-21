#pragma once
#include "EtType.h"

class CEtSystemFont
{
public:
	CEtSystemFont(void);
	virtual ~CEtSystemFont(void);

private:
	int			m_nCharWidth[256];
	int			m_nCharHeight[256];

	EtTextureHandle m_hTexture;

	std::vector< unsigned short > m_Indices;
	std::vector< STextureDiffuseVertex2D > m_Vertices;

public:
	void DrawText( LPCSTR pString, LPRECT pRect, D3DCOLOR Color);
	void Initialize( int nHeight, const char *pFonrName );
	static CEtSystemFont* Create( int nHeight, const char *pFontName );
};
