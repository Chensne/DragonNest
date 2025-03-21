#pragma once

#include "LostDeviceProcess.h"

class CEtSystemFont;
struct SText2D
{
	std::string szText;
	int nX;
	int nY;
	DWORD dwColor;
};
struct SText3D
{
	std::string szText;
	EtVector3 Position;
	DWORD dwColor;
};

class CEtTextDraw : public CSingleton< CEtTextDraw >, public CLostDeviceProcess
{
public:
	CEtTextDraw();
	virtual ~CEtTextDraw();

protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	ID3DXFont *m_pFont;
	CEtSystemFont *m_pSystemFont;
	ID3DXSprite *m_pSprite;

	std::vector< SText2D > m_vecText2D;
	std::vector< SText3D > m_vecText3D;

	bool		m_bUseSystemFont;

public:
	void Initialize( int nScreenWidth, int nScreenHeight );
	void OnLostDevice();
	void OnResetDevice();
	void DrawText2D( EtVector2 &Position, const char *pString, DWORD dwColor );
	void DrawText3D( EtVector3 &Position, const char *pString, DWORD dwColor );
	void Render();
	void Clear();
};
