#include "DnUtil.h"
#include "stdafx.h"
#include "Tlhelp32.h"

#pragma warning( disable : 4995 )

CString _fnLoadString( UINT id )
{
	CString sMsg = L"";
	
	if( id > 0 )
		sMsg.LoadString( id );

	return sMsg;
}

BOOL KillProcess( LPCTSTR strProcessName )
{
	WCHAR ExeName[1024] = {0,};
	wsprintf( ExeName , L"%s" , strProcessName );
	_tcsupr_s( ExeName );

	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	DWORD dwProcessID = GetCurrentProcessId();

	if ( hSnapshot != INVALID_HANDLE_VALUE ) 
	{ 
		PROCESSENTRY32 pe32 ; 
		pe32.dwSize=sizeof(PROCESSENTRY32); 
		BOOL bContinue ; 

		TCHAR strProcessName[ 1024 ] = {0,}; 

		if ( Process32First ( hSnapshot, &pe32 ) ) 
		{ 
			do 
			{ 
				if( pe32.th32ProcessID == dwProcessID  ) 
				{
					bContinue = Process32Next( hSnapshot, &pe32 ); 
					continue;
				}

				wsprintf( strProcessName , L"%s" , pe32.szExeFile );
				_tcsupr_s( strProcessName );               

				if( ( _tcsicmp(strProcessName, ExeName) != NULL ) ) 
				{ 
					HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, pe32.th32ProcessID ); 
					if( hProcess ) 
					{ 
						DWORD       dwExitCode; 
						GetExitCodeProcess( hProcess, &dwExitCode); 
						TerminateProcess( hProcess, dwExitCode); 
						CloseHandle(hProcess); 
					} 

				} 
				bContinue = Process32Next( hSnapshot, &pe32 ); 
			}
			while ( bContinue );        
		} 
		CloseHandle( hSnapshot );
	}
	return TRUE;
}

BOOL FindProcessName( LPCTSTR strProcessName )
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};
	UINT i;

	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if( hProcessSnap == INVALID_HANDLE_VALUE )
		return FALSE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if( Process32First( hProcessSnap, &pe32 ) )
	{
		do
		{
			for( i=0; i<=_tcslen(pe32.szExeFile); i++ )
			{
				pe32.szExeFile[i] = tolower( pe32.szExeFile[i] );
			}

			if( _tcsicmp(pe32.szExeFile, strProcessName) == 0 )
			{ 
				CloseHandle( hProcessSnap );
				return TRUE;
			}
		}
		while( Process32Next( hProcessSnap, &pe32 ) );
	}

	CloseHandle( hProcessSnap ); 
	return FALSE;
}

HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color )
{
	// get image properties
	BITMAP bmp = { 0 };
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	// allocate memory for extended image information
	LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( bi, 0, sizeof(BITMAPINFO) + 8 );
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// create temporary dc
	HDC dc = CreateIC( _T("DISPLAY"),NULL,NULL,NULL );
	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bi->bmiHeader.biSizeImage + 4 ];
	// allocate memory for color table
	if ( bi->bmiHeader.biBitCount == 8 )
	{
		// actually color table should be appended to this header(BITMAPINFO),
		// so we have to reallocate and copy it
		LPBITMAPINFO old_bi = bi;
		// 255 - because there is one in BITMAPINFOHEADER
		bi = (LPBITMAPINFO)new char[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD) ];
		memcpy( bi, old_bi, sizeof(BITMAPINFO) );
		// release old header
		delete old_bi;
	}
	// get bitmap info header
	BITMAPINFOHEADER& bih = bi->bmiHeader;
	// get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
	LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;
	// fill bits buffer
	res = GetDIBits( dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS );
	DeleteDC( dc );

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );
	// shift bits and byte per pixel (for comparing colors)
	LPBYTE pClr = (LPBYTE)&color;
	// swap red and blue components
	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
	// convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
	if ( bih.biBitCount == 16 )
	{
		// for 16 bit
		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
			((DWORD)(pClr[1] & 0xfc) << 3) |
			((DWORD)(pClr[2] & 0xf8) << 8);
		// for 15 bit
//		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
//				((DWORD)(pClr[1] & 0xf8) << 2) |
//				((DWORD)(pClr[2] & 0xf8) << 7);
	}

	const DWORD RGNDATAHEADER_SIZE	= sizeof(RGNDATAHEADER);
	const DWORD ADD_RECTS_COUNT		= 40;			// number of rects to be appended
	// to region data buffer

	// BitPerPixel
	BYTE	Bpp = bih.biBitCount >> 3;				// bytes per pixel
	// bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
	// so, both of them not
	DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
	DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
	// (after processing of current)
	DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
	INT		i, j;									// current position in mask image
	INT		first = 0;								// left position of current scan line
	// where mask was found
	bool	wasfirst = false;						// set when mask has been found in current scan line
	bool	ismask;									// set when current color is mask color

	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData = 
		(RGNDATAHEADER*)new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
	// zero region data header memory (header  part only)
	memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RGNDATAHEADER_SIZE;
	pRgnData->iType		= RDH_RECTANGLES;

	for ( i = 0; i < bih.biHeight; i++ )
	{
		for ( j = 0; j < bih.biWidth; j++ )
		{
			// get color
			switch ( bih.biBitCount )
			{
			case 8:
				ismask = (clr_tbl[ *pColor ] != color);
				break;
			case 16:
				ismask = (*(LPWORD)pColor != (WORD)color);
				break;
			case 24:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) != color);
				break;
			case 32:
				ismask = (*(LPDWORD)pColor != color);
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if ( wasfirst )
			{
				if ( !ismask )
				{
					// save current RECT
					pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
					// if buffer full reallocate it with more room
					if ( pRgnData->nCount >= dwRectsCount )
					{
						dwRectsCount += ADD_RECTS_COUNT;
						// allocate new buffer
						LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
						// copy current region data to it
						memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
						// delte old region data buffer
						delete pRgnData;
						// set pointer to new regiondata buffer to current
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
						// correct pointer to RECT table
						pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
					}
					wasfirst = false;
				}
			}
			else if ( ismask )		// set wasfirst when mask is found
			{
				first = j;
				wasfirst = true;
			}
		}

		if ( wasfirst && ismask )
		{
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
			// if buffer full reallocate it with more room
			if ( pRgnData->nCount >= dwRectsCount )
			{
				dwRectsCount += ADD_RECTS_COUNT;
				// allocate new buffer
				LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
				// copy current region data to it
				memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
				// delte old region data buffer
				delete pRgnData;
				// set pointer to new regiondata buffer to current
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				// correct pointer to RECT table
				pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
			}
			wasfirst = false;
		}

		pColor -= dwLineBackLen;
	}
	// release image data
	delete pBits;
	delete bi;

	// create region
	HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// release region data
	delete pRgnData;

	return hRgn;
}