#include "StdAfx.h"
#include "EtUIMovieControl.h"
#include "EtUIDialog.h"
#include "EtUIXML.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtUIMovieControl::CEtUIMovieControl( CEtUIDialog *pParent )
: CEtUIControl( pParent )
{
	//m_fBlendRate = 0.8f;
}

CEtUIMovieControl::~CEtUIMovieControl(void)
{
	//DeleteTexture();
}

void CEtUIMovieControl::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );

	if( ( pProperty ) && ( pProperty->StaticProperty.nStringIndex > 0 ) )
	{
		SetText( m_pParent->GetUIString( CEtUIXML::idCategory1, pProperty->StaticProperty.nStringIndex ) );
	}
}

void CEtUIMovieControl::Process( float fElapsedTime )
{
	m_CurrentState = UI_STATE_NORMAL;

	if( !IsShow() )
	{
		m_CurrentState = UI_STATE_HIDDEN;
	}
	else if( !IsEnable() )
	{
		m_CurrentState = UI_STATE_DISABLED;
	}

	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
}

void CEtUIMovieControl::Render( float fElapsedTime )
{
	//if( !m_hTexture )
	//	return;

	//SUIElement *pElement = GetElement(0);
	//if( !pElement ) return;

	//m_pParent->DrawSprite( m_hTexture, m_TextureUV, pElement->TextureColor.dwCurrentColor, m_ControlCoord );
	//m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_ControlCoord );
	//m_pParent->DrawDlgText( m_szText.c_str(), pElement, pElement->FontColor.dwCurrentColor, m_Property.UICoord, -1, m_Property.TextureControlProperty.dwFontFormat );
}

void CEtUIMovieControl::UpdateRects()
{
	//m_ControlCoord.SetCoord( m_Property.UICoord.fX, m_Property.UICoord.fY, m_TextureCoord.fWidth / m_pParent->GetScreenWidth(), m_Property.UICoord.fHeight );
	m_ControlCoord.SetCoord( m_Property.UICoord.fX, m_Property.UICoord.fY, m_Property.UICoord.fWidth, m_Property.UICoord.fHeight );
}