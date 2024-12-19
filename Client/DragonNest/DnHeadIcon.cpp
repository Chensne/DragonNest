#include "StdAfx.h"
#include "DnHeadIcon.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnHeadIcon::CDnHeadIcon()
{
	m_dwHeadIconStartTime = 0;
	m_dwRenderTime = HEADICON_RENDERTIME;
	m_bReleaseTexture = false;
}

CDnHeadIcon::~CDnHeadIcon(void)
{
	if( m_bReleaseTexture )
		SAFE_RELEASE_SPTR( m_hTex );
}

DWORD CDnHeadIcon::GetHeadIconAlpha( DWORD dwTime )
{
	static BYTE ALPHA = 192;
	DWORD dwAlpha = ALPHA;
	float fDuration = (float)dwTime / HEADICON_RENDERTIME;

	if ( fDuration >= 1.0f ) {
		dwAlpha = 0;
	}
	else if( fDuration > 0.9f )
	{
		dwAlpha = (DWORD)(ALPHA * (1.0f - (fDuration - 0.9f)*10.0f));
	}
	
	return dwAlpha;
}

DWORD CDnHeadIcon::GetHeadIconCustomAlpha( DWORD dwTime )
{
	static BYTE ALPHA = 192;
	DWORD dwAlpha = ALPHA;
	float fDuration = (float)dwTime / m_dwRenderTime;

	if ( fDuration >= 1.0f ) {
		dwAlpha = 0;
	}
	else if( fDuration > 0.9f )
	{
		dwAlpha = (DWORD)(ALPHA * (1.0f - (fDuration - 0.9f)*10.0f));
	}

	return dwAlpha;
}

void CDnHeadIcon::RenderHeadIcon( EtVector3 vHeadIconPos, float fElapsedTime )
{	
	DWORD dwAlpha = GetHeadIconCustomAlpha( GetTickCount() - m_dwHeadIconStartTime ) ;
	if( dwAlpha != 0 && m_hTex ) {
		EtColor Color = D3DCOLOR_ARGB(dwAlpha, 255, 255, 255);

		EtVector2 vSize;
		vSize.x = m_hTex->Width() * 0.5f;
		vSize.y = m_hTex->Height() * 0.5f;
		CEtSprite::GetInstance().Begin(0);
		CEtSprite::GetInstance().DrawSprite3D( (EtTexture*)m_hTex->GetTexturePtr(), m_hTex->Width(), m_hTex->Height(), Color, vSize, vHeadIconPos, 0.0f);
		CEtSprite::GetInstance().End();
	}
}

void CDnHeadIcon::SetHeadIcon( EtTextureHandle hTexture, DWORD dwStartTime, DWORD dwRenderTime, bool bReleaseTexture )
{
	if( m_bReleaseTexture )
		SAFE_RELEASE_SPTR( m_hTex );

	m_hTex = hTexture;
	m_dwHeadIconStartTime = dwStartTime;

	m_dwRenderTime = dwRenderTime;
	m_bReleaseTexture = bReleaseTexture;
}

bool CDnHeadIcon::IsRenderHeadIcon()
{
	if( GetTickCount() < m_dwRenderTime + m_dwHeadIconStartTime )
		return true;
	return false;
}

void CDnHeadIcon::StopRenderHeadIcon()
{
	m_dwHeadIconStartTime = 0;
	if( m_bReleaseTexture )
	{
		SAFE_RELEASE_SPTR( m_hTex );
		m_bReleaseTexture = false;
	}
}

