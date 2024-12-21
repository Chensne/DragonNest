#include "StdAfx.h"
#include "DnWorldDecal.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldDecal::CDnWorldDecal( CEtWorldSector *pSector )
: CEtWorldDecal( pSector )
{
}

CDnWorldDecal::~CDnWorldDecal()
{
	SAFE_RELEASE_SPTR( m_hTexture );
	SAFE_RELEASE_SPTR( m_hDecal );
}

bool CDnWorldDecal::Initialize( EtVector2 vPos, float fRadius, float fRotate, EtColor Color, float fAlpha, const char *szTextureName )
{
	bool bResult = CEtWorldDecal::Initialize( vPos, fRadius, fRotate, Color, fAlpha, szTextureName );
	if( !bResult ) return false;
	if( m_szTextureName.empty() ) return false;

	m_hTexture = EternityEngine::LoadTexture( m_szTextureName.c_str() );
	m_hDecal = (new CEtDecal)->GetMySmartPtr();
	EtColor ColorTemp = m_vColor;
	ColorTemp *= m_vColor.a;
	ColorTemp.a = m_fAlpha;
	m_hDecal->UseFixedUV( true );
	m_hDecal->Initialize( m_hTexture, m_vPos.x, m_vPos.y, m_fRadius, FLT_MAX, 0.f, m_fRotate, ColorTemp );

	return true;
}