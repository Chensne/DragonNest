#pragma once

class CUIToolTemplate;

EtTextureHandle GenerateTexture( std::vector< CUIToolTemplate * > &vecToolTemplate, bool bUseTemplateUVCoord = false );

struct SEmptySpaceTexGen
{
	int nX;
	int nY;
	int nWidth;
	int nHeight;

	void Set( int x, int y, int width, int height )
	{
		nX = x;
		nY = y;
		nWidth = width;
		nHeight = height;
	}
	int GetSpace() { return nWidth * nHeight; }
};
