#include "StdAfx.h"
#include "EtUICheckBox.h"
#include "EtUIDialog.h"
#include "EtUITemplate.h"
#include "EtUIXML.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtUICheckBox::CEtUICheckBox( CEtUIDialog *pParent )
	: CEtUIButton( pParent )
{
	m_Property.UIType = UI_CONTROL_CHECKBOX;
	m_bChecked = false;
}

CEtUICheckBox::~CEtUICheckBox(void)
{
}

void CEtUICheckBox::SetCheckedInternal( bool bChecked, bool bFromInput, bool bProcessCommand ) 
{ 
	m_bChecked = bChecked; 
	ASSERT( m_pParent != NULL );
	if( bProcessCommand && m_pParent ) m_pParent->ProcessCommand( EVENT_CHECKBOX_CHANGED, bFromInput, this ); 
}

void CEtUICheckBox::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );

	if( ( pProperty ) && ( pProperty->CheckBoxProperty.nStringIndex > 0 ) )
	{
		SetText( m_pParent->GetUIString( CEtUIXML::idCategory1, pProperty->CheckBoxProperty.nStringIndex ) );
	}

	UpdateRects();
}

bool CEtUICheckBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{	
		return false;
	}

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_SPACE:
				m_bPressed = true;
				return true;
			}
		}

	case WM_KEYUP:
		{
			switch( wParam )
			{
			case VK_SPACE:
				if( m_bPressed )
				{
					m_bPressed = false;
					SetCheckedInternal( !m_bChecked, true );
				}
				return true;
			}
		}
	}

	return false;
}

bool CEtUICheckBox::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( IsInside( fX, fY ) )
			{
				m_bPressed = true;
				SetCapture( m_pParent->GetHWnd() );
				if( !m_bFocus )
				{
					m_pParent->RequestFocus( this );
				}
				return true;
			}
			break;
		}

	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
				ReleaseCapture();
				if( IsInside( fX, fY ) )
				{
					SetCheckedInternal( !m_bChecked, true );
				}
				return true;
			}
			break;
		}
	}

	return false;
}

void CEtUICheckBox::OnHotkey() 
{ 
	m_pParent->RequestFocus( this ); 
	SetCheckedInternal( !m_bChecked, true ); 
}

void CEtUICheckBox::SetButtonRects( int nAllign )
{
	m_ButtonCoord = m_Property.UICoord;

	switch( nAllign )
	{
		case 0:		// Left
			m_ButtonCoord.fWidth = m_Property.UICoord.fHeight * 0.75f;	// DEFAULT_UI_SCREEN_WIDTH / DEFAULT_UI_SCREEN_HEIGHT

			m_TextCoord = m_Property.UICoord;
			m_TextCoord.fX += ( m_ButtonCoord.fWidth + m_Property.CheckBoxProperty.fSpace );
			m_TextCoord.fWidth -= ( m_ButtonCoord.fWidth + m_Property.CheckBoxProperty.fSpace );
			break;
		case 1:		// Center
			m_TextCoord = m_Property.UICoord;
			break;
		case 2:		// Right
			m_ButtonCoord.fWidth = m_Property.UICoord.fHeight * 0.75f;
			m_ButtonCoord.fX = m_Property.UICoord.Right() - m_ButtonCoord.fWidth;

			m_TextCoord = m_Property.UICoord;
			m_TextCoord.fWidth -= ( m_ButtonCoord.fWidth + m_Property.CheckBoxProperty.fSpace );
			break;
	}
}

void CEtUICheckBox::UpdateRects()
{
	SetButtonRects( m_Property.CheckBoxProperty.nAllign );
}

void CEtUICheckBox::Process( float fElapsedTime )
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
}

void CEtUICheckBox::Render( float fElapsedTime )
{
	// 여기서 한번 호출하면 첫 프레임에서 모든 Blend에 대해 BlendRate이 0으로 작동되서,
	// 블렌드때문에 반짝이는 현상을 없앨 수 있다.
	UpdateBlendRate();

	SUIElement *pElement = GetElement(0);

	if( ( m_CurrentState == UI_STATE_MOUSEENTER ) && ( m_Template.m_vecElement.size() >= 3 ) )
	{
		pElement = GetElement(2);
	}

	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_ButtonCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_ButtonCoord );

	UI_CONTROL_STATE SelectState(m_CurrentState);

	if( !m_bChecked )
	{
		SelectState = UI_STATE_HIDDEN;
	}

	pElement = GetElement(1);
	pElement->TextureColor.Blend( SelectState, fElapsedTime, m_fBlendRate );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_ButtonCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_ButtonCoord );

	if( m_bChecked )
	{
		pElement = GetElement(1);
	}
	else
	{
		pElement = GetElement(0);
	}

	if( pElement )
	{
		pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

		m_pParent->DrawDlgText( m_szText.c_str(), pElement, pElement->FontColor.dwCurrentColor, m_TextCoord );
	}
}