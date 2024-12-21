#include "StdAfx.h"
#include "EtSystemFont.h"
#include "EtTexture.h"
#include "EtType.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSystemFont::CEtSystemFont(void)
{
	memset(m_nCharHeight, 0, sizeof(m_nCharHeight));
	memset(m_nCharWidth, 0, sizeof(m_nCharWidth));	
}

CEtSystemFont::~CEtSystemFont(void)
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CEtSystemFont::Initialize( int nFontHeight, const char *pFonrName )
{
	HFONT hFont;
	HDC hDC;
	HBITMAP hBM;
	HFONT hOldFont;
	HBITMAP hOldBM;

	hDC = ::CreateCompatibleDC( NULL );
	hFont = CreateFont (nFontHeight , 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
		FF_DONTCARE | DEFAULT_PITCH, pFonrName);	
	hBM = ::CreateBitmap(16, 16, 1, 1, NULL);
	hOldFont = (HFONT)::SelectObject( hDC, hFont );

	int nCharMaxWidth = 0;
	int nCharMaxHeight = 0;
	for( int ch = 0; ch < 256; ch++) {
		char szText[2] = {ch, '\0' };
		SIZE size;
		::GetTextExtentPoint(hDC, szText, 1, (LPSIZE)&size);
		nCharMaxWidth = EtMax( size.cx , nCharMaxWidth );
		nCharMaxHeight = EtMax( size.cy , nCharMaxHeight );
		m_nCharWidth[ ch ] = size.cx;
		m_nCharHeight[ ch ] = size.cy;
	}
	hOldBM = (HBITMAP)::SelectObject( hDC,  hBM );	

	nCharMaxWidth = CEtTexture::CalcTextureSize( nCharMaxWidth );
	nCharMaxHeight = CEtTexture::CalcTextureSize( nCharMaxHeight );
		
	ASSERT( nCharMaxWidth == 16 && nCharMaxHeight == 16 );

	m_hTexture = CEtTexture::CreateNormalTexture( nCharMaxWidth * 16, nCharMaxHeight * 16, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_MANAGED );
	int nStride = 0;
	BYTE *pPtr = (BYTE*)m_hTexture->Lock( nStride, true);	
	
	for( int y = 0; y < nCharMaxHeight* 16; y++)	
	for( int x = 0; x < nCharMaxHeight* 16; x++)	
	{
		pPtr[ nStride * y  + x * 4 + 0 ] = 255;
		pPtr[ nStride * y  + x * 4 + 1 ] = 255;
		pPtr[ nStride * y  + x * 4 + 2 ] = 255;
		pPtr[ nStride * y  + x * 4 + 3 ] = 0;
	}
	
	::SetBkColor(hDC, RGB(0,0,0));
	::SetTextColor(hDC, RGB(255,255,255));
	::SetBkMode(hDC, OPAQUE);

	int nTotalBytes = 16 * 16 / 8;	
	BYTE *pBits = new BYTE[ nTotalBytes ];
	for( int ch= 0; ch < 256; ch++) {
		char szText[2] = {ch, '\0' };
		::TextOut( hDC, 0, 0, szText, 1 );			
		memset(pBits, 0, nTotalBytes );
		::GetBitmapBits(hBM, nTotalBytes, pBits );

		for( int y = 0; y < m_nCharHeight[ ch ]; y++)
		{
			for( int x = 0; x < 2; x++)
			{
				int addr = (16 * 16) * (y+(ch/16)*16) + ((x*8)+(ch%16)*16);

				for( int bit = 0; bit < 8; bit++) 
				{
					if( bit + x * 8 >= m_nCharWidth[ ch ] ) 
						break;
					if( (pBits[x + y * 2 ] & (1<<(7 - bit) )) != 0  ) {
						pPtr[ (addr+bit) * 4 + 3 ] = 0xFF;
					}					
				}
			}
		}
	}
	SAFE_DELETEA( pBits );
	m_hTexture->Unlock();

	::SelectObject( hDC, hOldFont );
	::SelectObject( hDC, hOldBM );
	::DeleteDC( hDC );	
	::DeleteObject( hBM );
	::DeleteObject( hFont );
}

void CEtSystemFont::DrawText( LPCSTR pString, LPRECT pRect, D3DCOLOR Color )
{
	int nLength = (int)strlen( pString );	
	if( nLength <= 0 ) return;
	if( !m_hTexture || !m_hTexture->GetTexturePtr() ) return;

	GetEtDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	GetEtDevice()->SetPixelShader( NULL );
	GetEtDevice()->SetVertexShader( NULL );

	DWORD dwWidth = GetEtDevice()->Width();
	DWORD dwHeight = GetEtDevice()->Height();

	SUICoord ScreenCoord;
	ScreenCoord.fX = (float)pRect->left - 0.5f ;
	ScreenCoord.fY = (float)pRect->top - 0.5f;	

	float fZValue = 0.f;

	if( (int)m_Indices.size() < nLength * 6 )
		m_Indices.resize( nLength * 6 );
	if( (int)m_Vertices.size() < nLength * 4)
		m_Vertices.resize( nLength * 4);
	
	for( int i = 0; i < nLength; i++) {
		BYTE ch = pString[ i ];

		ScreenCoord.fWidth = 16.0f;
		ScreenCoord.fHeight = 16.0f;

		m_Vertices[ i * 4 + 0 ].Position = EtVector4( ScreenCoord.fX , ScreenCoord.fY, fZValue, 1.0f );
		m_Vertices[ i * 4 + 1 ].Position = EtVector4( (ScreenCoord.fX+ScreenCoord.fWidth), ScreenCoord.fY , fZValue, 1.0f );
		m_Vertices[ i * 4 + 2 ].Position = EtVector4( (ScreenCoord.fX+ScreenCoord.fWidth) , (ScreenCoord.fY+ScreenCoord.fHeight), fZValue, 1.0f );
		m_Vertices[ i * 4 + 3 ].Position = EtVector4( ScreenCoord.fX , (ScreenCoord.fY+ScreenCoord.fHeight) , fZValue, 1.0f );
		
		SUICoord UVCoord;
		UVCoord.fX = (ch % 16) / 16.0f;
		UVCoord.fY = (ch / 16) / 16.0f;
		UVCoord.fWidth = 1 / 16.0f;
		UVCoord.fHeight = 1 / 16.0f;

		m_Vertices[ i * 4 + 0 ].TexCoordinate = EtVector2( UVCoord.fX, UVCoord.fY );
		m_Vertices[ i * 4 + 1 ].TexCoordinate = EtVector2( UVCoord.fX+UVCoord.fWidth, UVCoord.fY );
		m_Vertices[ i * 4 + 2 ].TexCoordinate = EtVector2( UVCoord.fX+UVCoord.fWidth, UVCoord.fY + UVCoord.fHeight );
		m_Vertices[ i * 4 + 3 ].TexCoordinate = EtVector2( UVCoord.fX, UVCoord.fY + UVCoord.fHeight );

		ScreenCoord.fX += (m_nCharWidth[ch]);

		m_Vertices[ i * 4 + 0 ].Color = Color;
		m_Vertices[ i * 4 + 1 ].Color = Color;
		m_Vertices[ i * 4 + 2 ].Color = Color;
		m_Vertices[ i * 4 + 3 ].Color = Color;

		m_Indices[ 6 * i + 0 ] = 0 + i * 4;
		m_Indices[ 6 * i + 1 ] = 1 + i * 4;
		m_Indices[ 6 * i + 2 ] = 3 + i * 4;
		m_Indices[ 6 * i + 3 ] = 1 + i * 4;
		m_Indices[ 6 * i + 4 ] = 3 + i * 4;
		m_Indices[ 6 * i + 5 ] = 2 + i * 4;
	}
	GetEtDevice()->SetTexture( 0, m_hTexture->GetTexturePtr() );	

	bool bAlphaBlend = GetEtDevice()->EnableAlphaBlend( true );

	GetEtDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	GetEtDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	GetEtDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	GetEtDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	GetEtDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	GetEtDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );

	GetEtDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

	GetEtDevice()->DrawIndexedPrimitiveUP( PT_TRIANGLELIST, 0, 4 * nLength, 2 * nLength, &m_Indices.front(),  FMT_INDEX16, &m_Vertices.front(), sizeof(STextureDiffuseVertex2D) );

	GetEtDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	GetEtDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	GetEtDevice()->EnableAlphaBlend( bAlphaBlend );
	
}
CEtSystemFont* CEtSystemFont::Create( int nHeight, const char *pFontName )
{
	CEtSystemFont *pResultFont = new CEtSystemFont;
	pResultFont->Initialize( nHeight, pFontName );


	return pResultFont;
}
