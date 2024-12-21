#include "stdafx.h"
#include <d3dx9.h>
#include "Fade.h"
#include "EternityEngine.h"
#include "EtSprite.h"


CFade::CFade( LPDIRECT3DDEVICE9 pd3dDevice, LPD3DXSPRITE pSprite ) : m_pd3dDevice( pd3dDevice ),
																	 m_pBlackTexture( NULL ),
																	 m_pSprite( pSprite ),
																	 m_iFadeState( NONE ),
																	 m_fNowFadeAmount( 0.0f ),
																	 m_fFadeSpeed( 1.0f ),
																	 m_dwColor( 0xff000000 )
{
	// 검은 텍스쳐 생성
	//pd3dDevice->CreateTexture( 16, 16, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pBlackTexture, NULL );
	//D3DXCreateTexture( m_pd3dDevice, 16, 16, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pBlackTexture );
	//m_pBlackTexture->

	//HRESULT hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, L"./bitmaps/fade.dds", 16, 16, 1, 0,
	//										 D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, 
	//										 D3DX_FILTER_NONE, 0, NULL, NULL, &m_pBlackTexture );

	//_ASSERT( S_OK == hr );
}


CFade::~CFade(void)
{
	//if( m_pBlackTexture )
	//{
	//	m_pBlackTexture->Release();
	//	m_pBlackTexture = NULL;
	//}
}


void CFade::Reset( void )
{
	m_iFadeState = NONE;
	m_fNowFadeAmount = 0.0f;
	m_fFadeSpeed = 1.0f;
}


void CFade::FadeIn( float fSpeed, DWORD dwColor )
{
	m_iFadeState = FADE_IN;
	m_fNowFadeAmount = 1.0f;
	m_fFadeSpeed = fSpeed;
	m_dwColor = dwColor;
}


void CFade::FadeOut( float fSpeed, DWORD dwColor )
{
	m_iFadeState = FADE_OUT;
	m_fNowFadeAmount = 0.0f;
	m_fFadeSpeed = fSpeed;
	m_dwColor = dwColor;
}


void CFade::Render( float fDelta )
{
	if( NULL == m_pSprite )
		return;

	//if( NONE == m_iFadeState )
	//	return;

	if( FADE_IN == m_iFadeState )
		m_fNowFadeAmount -= (fDelta * m_fFadeSpeed);
	else
	if( FADE_OUT == m_iFadeState )
		m_fNowFadeAmount += (fDelta * m_fFadeSpeed);

	if( m_fNowFadeAmount < 0.0f )
	{
		m_fNowFadeAmount = 0.0f;
		m_iFadeState = NONE;
	}
	else
	if( m_fNowFadeAmount > 1.0f )
	{
		m_fNowFadeAmount = 1.0f;
		m_iFadeState = NONE;
	}

	//D3DXMATRIX matTrans, matScale;
	//D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 0.0f );

	//D3DXMatrixScaling( &matScale, 100.0f, 100.0f, 1.0f );

	//matTrans = matTrans * matScale;
	//m_pSprite->SetTransform( &matTrans );

	//m_pSprite->Begin( D3DXSPRITE_ALPHABLEND );

	//m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

	//DWORD dwAlpha = 0;
	//D3DXCOLOR Alpha( 0.0f, 0.0f, 0.0f, m_fNowFadeAmount );
	//dwAlpha = (DWORD)Alpha;
	//m_dwColor &= 0x00ffffff;
	//m_dwColor |= dwAlpha;
	//m_pSprite->Draw( m_pBlackTexture, NULL, NULL, NULL, m_dwColor );

	//m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

	//m_pSprite->End();

	SUICoord Coord( 0.0f, 0.0f, 1.0f, 1.0f );
	DWORD dwAlpha = 0;
	D3DXCOLOR Alpha( 0.0f, 0.0f, 0.0f, m_fNowFadeAmount );
	dwAlpha = (DWORD)Alpha;
	m_dwColor &= 0x00ffffff;
	m_dwColor |= dwAlpha;

	CEtSprite::GetInstance().Begin( D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_ALPHABLEND );
	CEtSprite::GetInstance().Flush();
	CEtSprite::GetInstance().DrawRect( Coord, m_dwColor );
	CEtSprite::GetInstance().End();
}
