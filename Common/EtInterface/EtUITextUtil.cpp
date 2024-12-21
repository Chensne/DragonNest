#include "StdAfx.h"
#include "EtUITextUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

namespace CEtInterface
{
	namespace debug
	{
		void OutputDebugUICoord( const wchar_t *pString, const SUICoord& coord, float fWidth, float fHeight )
		{
			int nX = (int)(coord.fX * fWidth);
			int nY = (int)(coord.fY * fHeight);
			int nW = (int)(coord.fWidth * fWidth);
			int nH = (int)(coord.fHeight * fHeight);

			wchar_t strTemp[256]={0};
			swprintf( strTemp, 256, L"%s, UICoord=[X:%d][Y:%d][W:%d][H:%d]\n", pString, nX, nY, nW, nH );
			OutputDebugStringW( strTemp );
		}
	}
}