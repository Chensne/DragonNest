#include "StdAfx.h"
#include "EtWorldDecal.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtWorldDecal::CEtWorldDecal( CEtWorldSector *pSector )
: m_vPos( 0.f, 0.f )
, m_vColor( 255, 255, 255, 255 )
{
	m_pParentSector = pSector;
	m_fRadius = 1.f;
	m_fRotate = 0.f;
	m_fAlpha = 1.f;

	m_pCurrentNode = NULL;
}

CEtWorldDecal::~CEtWorldDecal()
{
}

bool CEtWorldDecal::Initialize( EtVector2 vPos, float fRadius, float fRotate, EtColor Color, float fAlpha, const char *szTextureName )
{
	m_vPos = vPos;
	m_fRadius = fRadius;
	m_fRotate = fRotate;
	m_vColor = Color;
	m_fAlpha = fAlpha;
	m_szTextureName = szTextureName;

	return true;
}


void CEtWorldDecal::GetBoundingCircle( SCircle &Circle )
{
	Circle.Center = m_vPos;
	Circle.fRadius = m_fRadius;
}

