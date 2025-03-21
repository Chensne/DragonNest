#include "StdAfx.h"
#include "EtUISlider.h"
#include "EtUIDialog.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUISlider::CEtUISlider( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
{
	m_Property.UIType = UI_CONTROL_SLIDER;
	m_Property.SliderProperty.nMinValue = 0;
	m_Property.SliderProperty.nMaxValue = 100;
	m_nValue = 0;
	m_fButtonX = 0.0f;
	m_bPressed = false;
	m_fDragOffset = 0.0f;
	m_fDragX = 0.0f;
}

CEtUISlider::~CEtUISlider(void)
{
}

void CEtUISlider::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );

	if( m_Property.SliderProperty.nMinValue == m_Property.SliderProperty.nMaxValue )
	{
		m_Property.SliderProperty.nMinValue = 0;
		m_Property.SliderProperty.nMaxValue = 100;
	}
	SetValue( 0 );

	UpdateRects();
}

bool CEtUISlider::IsInside( float fX, float fY )
{
	if( CEtUIControl::IsInside( fX, fY ) )
	{
		return true;
	}
	if( m_ButtonCoord.IsInside( fX, fY ) )
	{
		return true;
	}

	return false;
}

bool CEtUISlider::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	if( uMsg == WM_KEYDOWN )
	{
		switch( wParam )
		{
		case VK_HOME:
			SetValueInternal( m_Property.SliderProperty.nMinValue, true );
			return true;

		case VK_END:
			SetValueInternal( m_Property.SliderProperty.nMaxValue, true );
			return true;

		case VK_LEFT:
		case VK_DOWN:
			SetValueInternal( m_nValue - 1, true );
			return true;

		case VK_RIGHT:
		case VK_UP:
			SetValueInternal( m_nValue + 1, true );
			return true;

		case VK_NEXT:
			SetValueInternal( m_nValue - ( 10 > ( m_Property.SliderProperty.nMaxValue - m_Property.SliderProperty.nMinValue ) / 10 ? 10 : ( m_Property.SliderProperty.nMaxValue - m_Property.SliderProperty.nMinValue ) / 10 ), true );
			return true;

		case VK_PRIOR:
			SetValueInternal( m_nValue + ( 10 > ( m_Property.SliderProperty.nMaxValue - m_Property.SliderProperty.nMinValue ) / 10 ? 10 : ( m_Property.SliderProperty.nMaxValue - m_Property.SliderProperty.nMinValue ) / 10 ), true );
			return true;
		}
	}

	return false;
}

bool CEtUISlider::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if( m_ButtonCoord.IsInside( fX, fY ) )
		{
			m_bPressed = true;
			SetCapture( m_pParent->GetHWnd() );

			m_fDragX = fX;
			m_fDragOffset = m_fButtonX - m_fDragX;
			if( !m_bFocus )
			{
				m_pParent->RequestFocus( this );
			}

			return true;
		}
		if( m_Property.UICoord.IsInside( fX, fY ) )
		{
			m_fDragX = fX;
			m_fDragOffset = 0;               
			m_bPressed = true;

			if( !m_bFocus )
			{
				m_pParent->RequestFocus( this );
			}

			if( fX > m_fButtonX + m_Property.UICoord.fX )
			{
				SetValueInternal( m_nValue + 1, true );
				return true;
			}

			if( fX < m_fButtonX + m_Property.UICoord.fX )
			{
				SetValueInternal( m_nValue - 1, true );
				return true;
			}
		}
		break;
	case WM_LBUTTONUP:
		if( m_bPressed )
		{
			m_bPressed = false;
			ReleaseCapture();
			m_pParent->ProcessCommand( EVENT_SLIDER_VALUE_CHANGED, true, this );

			return true;
		}
		break;
	case WM_MOUSEMOVE:
		if( m_bPressed )
		{
			SetValueInternal( ValueFromPos( m_Property.UICoord.fX + fX + m_fDragOffset ), true );
			return true;
		}
		break;
	case WM_MOUSEWHEEL:
		{
			int nScrollAmount;

			nScrollAmount = int((short)HIWORD(wParam)) / WHEEL_DELTA;
			SetValueInternal( m_nValue - nScrollAmount, true );
			return true;
		}
	}

	return false;
}

void CEtUISlider::UpdateRects()
{
	SUIElement *pElement = GetElement(1);
	if( !pElement ) return;

	m_ButtonCoord = m_Property.UICoord;
	m_ButtonCoord.SetSize( pElement->fTextureWidth, pElement->fTextureHeight );
	m_ButtonCoord.fX -= m_ButtonCoord.fWidth * 0.5f;
	m_ButtonCoord.fY -= (m_ButtonCoord.fHeight - m_Property.UICoord.fHeight)/2.0f;

	m_fButtonX = ( ( m_nValue - m_Property.SliderProperty.nMinValue ) * m_Property.UICoord.fWidth / ( m_Property.SliderProperty.nMaxValue - m_Property.SliderProperty.nMinValue ) );
	m_ButtonCoord.fX += m_fButtonX;
}

void CEtUISlider::SetValueInternal( int nValue, bool bFromInput )
{
	nValue = max( m_Property.SliderProperty.nMinValue, nValue );
	nValue = min( m_Property.SliderProperty.nMaxValue, nValue );

	if( nValue == m_nValue )
	{
		return;
	}
	m_nValue = nValue;	
	UpdateRects();
	m_pParent->ProcessCommand( EVENT_SLIDER_VALUE_CHANGED, bFromInput, this );
}

int CEtUISlider::ValueFromPos( float fX )
{
	float fValuePerPixel;

	fValuePerPixel = ( m_Property.SliderProperty.nMaxValue - m_Property.SliderProperty.nMinValue ) / m_Property.UICoord.fWidth;
	return ( int ) (0.5f + m_Property.SliderProperty.nMinValue + fValuePerPixel * ( fX - m_Property.UICoord.fX ) ); 
}

void CEtUISlider::Render( float fElapsedTime )
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
	else if( m_bPressed )
	{
		m_CurrentState = UI_STATE_PRESSED;
	}
	else if( m_bMouseEnter )
	{
		m_CurrentState = UI_STATE_MOUSEENTER;
	}
	else if( m_bFocus )
	{
		m_CurrentState = UI_STATE_FOCUS;
	}

	UpdateBlendRate();

	SUIElement *pElement(NULL);

	pElement = GetElement(0);
	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate ); 
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_Property.UICoord );

	pElement = GetElement(1);
	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate ); 
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_ButtonCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_ButtonCoord );
}
