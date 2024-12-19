#include "StdAfx.h"
#include "EtTextDraw.h"
#include "EtCamera.h"
#include "D3DDevice9/EtSystemFont.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtTextDraw::CEtTextDraw(void)
{
	m_pFont = NULL;
	m_pSystemFont = NULL;
	m_pSprite = NULL;
	m_bUseSystemFont = true;
}

CEtTextDraw::~CEtTextDraw(void)
{
	SAFE_RELEASE( m_pFont );
	SAFE_DELETE( m_pSystemFont );
	SAFE_RELEASE( m_pSprite );
}

void CEtTextDraw::Initialize( int nScreenWidth, int nScreenHeight )
{
	m_pFont = (ID3DXFont *)GetEtDevice()->CreateFont( "Tahoma" );
	m_pSystemFont = (CEtSystemFont *)GetEtDevice()->CreateSystemFont( "Tahoma" );
	m_pSprite = ( ID3DXSprite * )GetEtDevice()->CreateSprite();
	m_nScreenWidth = nScreenWidth;
	m_nScreenHeight = nScreenHeight;
}

void CEtTextDraw::OnLostDevice()
{
	if( m_pFont )
	{
		m_pFont->OnLostDevice(); 
	}
	if( m_pSprite )
	{
		m_pSprite->OnLostDevice();
	}
}

void CEtTextDraw::OnResetDevice()
{
	if( m_pFont )
	{
		m_pFont->OnResetDevice(); 
	}
	if( m_pSprite )
	{
		m_pSprite->OnResetDevice();
	}
	m_nScreenWidth = GetEtDevice()->Width();
	m_nScreenHeight = GetEtDevice()->Height();
}

void CEtTextDraw::DrawText2D( EtVector2 &Position, const char *pString, DWORD dwColor )
{
	SText2D PrintText;

	PrintText.szText = pString;
	PrintText.nX = ( int )( m_nScreenWidth * Position.x );
	PrintText.nY = ( int )( m_nScreenHeight * Position.y );
	PrintText.dwColor = dwColor;
	m_vecText2D.push_back( PrintText );

	//OutputDebug("%ws",PrintText);
}

void CEtTextDraw::DrawText3D( EtVector3 &Position, const char *pString, DWORD dwColor )
{
	SText3D PrintText;

	PrintText.szText = pString;
	PrintText.Position = Position;
	PrintText.dwColor = dwColor;
	m_vecText3D.push_back( PrintText );
}

void CEtTextDraw::Render()
{
	int i;
	RECT Rect;
	EtMatrix TextMat, Identity;

	EtMatrixRotationX( &TextMat, EtToRadian( 180.0f ) );
	EtMatrixIdentity( &Identity );

	for( i = 0; i < ( int )m_vecText3D.size(); i++ )
	{
		TextMat._41 = m_vecText3D[ i ].Position.x;
		TextMat._42 = m_vecText3D[ i ].Position.y;
		TextMat._43 = m_vecText3D[ i ].Position.z;

		m_pSprite->SetWorldViewLH( &Identity, CEtCamera::GetActiveCamera()->GetViewMat() );
		m_pSprite->Begin( D3DXSPRITE_OBJECTSPACE | D3DXSPRITE_BILLBOARD );
		m_pSprite->SetTransform( &TextMat );		
		m_pFont->DrawText( m_pSprite, m_vecText3D[ i ].szText.c_str(), -1, NULL, DT_NOCLIP, m_vecText3D[ i ].dwColor );
		m_pSprite->End();
	}

	m_vecText3D.clear();

	for( i = 0; i < ( int )m_vecText2D.size(); i++ )
	{
		SetRect( &Rect, m_vecText2D[ i ].nX, m_vecText2D[ i ].nY, 0, 0 );        
		if( m_bUseSystemFont ) {
			m_pSystemFont->DrawText( m_vecText2D[ i ].szText.c_str(), &Rect, m_vecText2D[ i ].dwColor );
		}
		else {
				m_pFont->DrawText( NULL, m_vecText2D[ i ].szText.c_str(), -1, &Rect, DT_NOCLIP , m_vecText2D[ i ].dwColor );

		}		
	}

	m_vecText2D.clear();
}

void CEtTextDraw::Clear()
{
	m_vecText3D.clear();
	m_vecText2D.clear();
}