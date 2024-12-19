#include "StdAfx.h"
#include "EtUIRadioButton.h"
#include "EtUIDialog.h"
#include "EtUIXML.h"
#include "EtSoundEngine.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtUIRadioButton::CEtUIRadioButton( CEtUIDialog *pParent )
	: CEtUICheckBox( pParent )
	, m_bEnableDragMode( true )
	, m_bIgnoreAdjustAlpha( false )
{
	m_Property.UIType = UI_CONTROL_RADIOBUTTON;
}

CEtUIRadioButton::~CEtUIRadioButton(void)
{
}

void CEtUIRadioButton::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );

	if( ( pProperty ) && ( pProperty->RadioButtonProperty.nStringIndex > 0 ) )
	{
		SetText( m_pParent->GetUIString( CEtUIXML::idCategory1, pProperty->RadioButtonProperty.nStringIndex ) );
	}

	UpdateRects();

	if( m_Property.RadioButtonProperty.szSoundFileName[0] != '\0' )
	{
		m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( m_Property.RadioButtonProperty.szSoundFileName, false, false );
	}
}

bool CEtUIRadioButton::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
		return false;

	if( drag::IsValid() && !m_bEnableDragMode )
		return true;

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
					m_pParent->ClearRadioButtonGroup( m_Property.RadioButtonProperty.nButtonGroup );
					m_bChecked = !m_bChecked;
					m_pParent->ProcessCommand( EVENT_RADIOBUTTON_CHANGED, true, this );
					if( m_nSoundIndex != -1 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndex );
				}
				return true;
			}
		}
	}

	return false;
}

bool CEtUIRadioButton::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
		return false;

	if( drag::IsValid() && !m_bEnableDragMode )
		return true;

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
					m_pParent->ClearRadioButtonGroup( m_Property.RadioButtonProperty.nButtonGroup );
					m_bChecked = !m_bChecked;
					m_pParent->ProcessCommand( EVENT_RADIOBUTTON_CHANGED, true, this );
					//rlkt_fix
					if( m_nSoundIndex != -1 && m_nSoundIndex > 0 ) CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndex );
				}
				return true;
			}
			break;
		}
	}

	return false;
}

void CEtUIRadioButton::OnHotkey()
{
	m_pParent->RequestFocus( this ); 
	SetCheckedInternal( true, true, true, true );
}

void CEtUIRadioButton::UpdateRects()
{
	SetButtonRects( m_Property.RadioButtonProperty.nAllign );
}

void CEtUIRadioButton::SetCheckedInternal( bool bChecked, bool bClearGroup, bool bFromInput, bool bSound )
{
	if( ( bChecked ) && ( bClearGroup ) )
	{
		m_pParent->ClearRadioButtonGroup( m_Property.RadioButtonProperty.nButtonGroup );
	}

	m_bChecked = bChecked;
	if( m_bChecked )
	{
		m_pParent->ProcessCommand( EVENT_RADIOBUTTON_CHANGED, bFromInput, this );
		if( m_nSoundIndex != -1 && true == bSound )
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndex );
	}
}

void CEtUIRadioButton::SetButtonRects( int nAllign )
{
	if( m_Property.RadioButtonProperty.nIconIndex > 0 )
	{
		m_ButtonCoord = m_Property.UICoord;

		// Note : 아이콘과 텍스트 위치를 설정한다.
		//
		SUIElement *pElement(NULL);
		pElement = GetElement(m_Property.RadioButtonProperty.nIconIndex);

		if( pElement )
		{
			m_IconCoord.SetCoord( m_Property.UICoord.fX + ((m_Property.UICoord.fWidth-pElement->fTextureWidth)/2.0f), m_Property.UICoord.Bottom() - pElement->fTextureHeight - 0.008f, pElement->fTextureWidth, pElement->fTextureHeight );
			m_TextCoord.SetCoord( m_IconCoord.Right(), m_IconCoord.fY, m_Property.UICoord.fWidth - m_IconCoord.fWidth - 0.01f, m_Property.UICoord.fHeight );
		}
	}
	else
	{
		CEtUICheckBox::SetButtonRects( nAllign );
	}
}

void CEtUIRadioButton::Render( float fElapsedTime )
{
	m_CurrentState = UI_STATE_NORMAL;

	m_MoveButtonCoord = m_ButtonCoord;
	SUICoord textCoord = m_TextCoord;

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

		m_MoveButtonCoord.fX += m_Property.RadioButtonProperty.fMoveOffsetHori;
		m_MoveButtonCoord.fY += m_Property.RadioButtonProperty.fMoveOffsetVert;
		textCoord.fX += m_Property.RadioButtonProperty.fMoveOffsetHori;
		textCoord.fY += m_Property.RadioButtonProperty.fMoveOffsetVert;
	}
	else if( m_bMouseEnter )
	{
		m_CurrentState = UI_STATE_MOUSEENTER;
	}
	else if( m_bFocus && m_bChecked )
	{
		m_CurrentState = UI_STATE_FOCUS;
	}

	UpdateBlendRate();

	UI_CONTROL_STATE SelectState1, SelectState2;
	SUICoord iconCoord;
	iconCoord = m_IconCoord;

	if( m_bChecked )
	{
		SelectState1 = UI_STATE_HIDDEN;
		SelectState2 = m_CurrentState;
	}
	else
	{
		SelectState1 = m_CurrentState;
		SelectState2 = UI_STATE_HIDDEN;

		float fWidth = iconCoord.fWidth * 0.8f;
		float fHeight = iconCoord.fHeight * 0.8f;
		textCoord.fX -= (iconCoord.fWidth - fWidth);
		iconCoord.fX += (iconCoord.fWidth - fWidth)/2.0f;
		iconCoord.fY += (iconCoord.fHeight - fHeight)/2.0f;
		iconCoord.fWidth = fWidth;
		iconCoord.fHeight = fHeight;
	}

	m_nBlinkAlpha = 255;

	if( m_bBlink )
	{
		m_fBlinkTime += fElapsedTime * 1000;

		int BLEND_TIME = 1000;
		float BLEND_RANGE  = 1.0f;
		int nTemp = (int)m_fBlinkTime%(BLEND_TIME+BLEND_TIME);
		if( nTemp < BLEND_TIME ) 
		{
			m_nBlinkAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * nTemp * 255.0f);
		}
		else 
		{
			m_nBlinkAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * ( (BLEND_TIME+BLEND_TIME) - nTemp ) * 255.0f);
		}

		m_nBlinkAlpha = min(m_nBlinkAlpha, 255);
	}
	else
	{
		m_fBlinkTime = 0.0f;
	}

	SUIElement *pElement(NULL);

	pElement = GetElement(0);
	if( !m_bChecked && m_CurrentState == UI_STATE_MOUSEENTER )
	{
		if( m_Property.RadioButtonProperty.nIconIndex == 0 && m_Template.m_vecElement.size() > 2 )
		{
			pElement = GetElement(2);
		}
	}

	if( pElement )
	{
		D3DXCOLOR colorMouseEnter( pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER] );
		colorMouseEnter.a = m_nBlinkAlpha / 255.0f;

		pElement->TextureColor.Blend( SelectState1, fElapsedTime, m_fBlendRate );
		pElement->FontColor.Blend( SelectState1, fElapsedTime, m_fBlendRate );
		pElement->ShadowFontColor.Blend( SelectState1, fElapsedTime, m_fBlendRate );

		SUICoord uiTextCoord(textCoord);
		uiTextCoord.fX += pElement->fFontHoriOffset;
		uiTextCoord.fY += pElement->fFontVertOffset;

		if( m_bIgnoreAdjustAlpha == false ) {
			// 체크상태에서 체크풀리면서 일반상태로 바뀔때 한쪽은 히든에서 노말로, 한쪽은 노말에서 히든으로 변경되면서
			// 둘다 알파값이 낮아져 버튼이 깜박이는 것처럼 보일때가 있다.
			// 그래서 알파값을 아예 안낮추도록 강제로 조정한다.
			if( SelectState1 != UI_STATE_HIDDEN ) {
				if( (pElement->TextureColor.dwCurrentColor >> 24) < 256 )
					pElement->TextureColor.dwCurrentColor |= 0xFF000000;
			}
		}

		if( m_Property.RadioButtonProperty.fVariableWidthLength > 0.0f )
		{
			float fBaseWidth = 0.0f;
			EtTextureHandle hTexture = m_pParent->GetUITexture();
			if( m_bExistTemplateTexture )
				hTexture = m_Template.m_hTemplateTexture;
			if( hTexture )
				fBaseWidth = m_Property.RadioButtonProperty.fVariableWidthLength * DEFAULT_UI_SCREEN_WIDTH / hTexture->Width();

			SUICoord uvLeft, uvCenter, uvRight;
			SUICoord uiLeft, uiCenter, uiRight;
			SUICoord UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			uiLeft.SetCoord( m_MoveButtonCoord.fX, m_MoveButtonCoord.fY, m_Property.RadioButtonProperty.fVariableWidthLength, m_MoveButtonCoord.fHeight );
			uiCenter.SetCoord( uiLeft.Right(), m_MoveButtonCoord.fY, m_MoveButtonCoord.fWidth-(m_Property.RadioButtonProperty.fVariableWidthLength*2.0f), m_MoveButtonCoord.fHeight );
			uiRight.SetCoord( uiCenter.Right(), m_MoveButtonCoord.fY, m_Property.RadioButtonProperty.fVariableWidthLength, m_MoveButtonCoord.fHeight );
			uvLeft = uvCenter = uvRight = UVCoord;
			uvLeft.fWidth = fBaseWidth;
			uvCenter.fX = UVCoord.fX + fBaseWidth;
			uvCenter.fWidth = uvCenter.fWidth-fBaseWidth*2.0f;
			uvRight.fX = UVCoord.Right() - fBaseWidth;
			uvRight.fWidth = fBaseWidth;
			m_pParent->DrawSprite( hTexture, uvLeft, pElement->TextureColor.dwCurrentColor, uiLeft );
			m_pParent->DrawSprite( hTexture, uvCenter, pElement->TextureColor.dwCurrentColor, uiCenter );
			m_pParent->DrawSprite( hTexture, uvRight, pElement->TextureColor.dwCurrentColor, uiRight );

			if( m_bBlink && m_CurrentState == UI_STATE_NORMAL )
			{
				SUIElement *pElementOver = GetElement(2);
				if( pElementOver )
				{
					SUICoord uvCoordLeft;
					SUICoord uvCoordMid;
					SUICoord uvCoordRight;
					if( hTexture )
					{
						int nTextureWidth = hTexture->Width();
						float fBaseWidth = m_Property.RadioButtonProperty.fVariableWidthLength * DEFAULT_UI_SCREEN_WIDTH / nTextureWidth;

						UVCoord = pElementOver->UVCoord;
						if( m_bExistTemplateTexture )
							UVCoord = pElementOver->TemplateUVCoord;

						uvCoordLeft = UVCoord;
						uvCoordLeft.fWidth = fBaseWidth;

						uvCoordRight = UVCoord;
						uvCoordRight.fX = UVCoord.Right() - fBaseWidth;
						uvCoordRight.fWidth = fBaseWidth;

						uvCoordMid = UVCoord;
						uvCoordMid.fX = UVCoord.fX + fBaseWidth;
						uvCoordMid.fWidth = uvCoordMid.fWidth - fBaseWidth*2.0f;
					}
					m_pParent->DrawSprite( hTexture, uvCoordLeft, colorMouseEnter, uiLeft );
					m_pParent->DrawSprite( hTexture, uvCoordMid, colorMouseEnter, uiCenter );
					m_pParent->DrawSprite( hTexture, uvCoordRight, colorMouseEnter, uiRight );
				}
			}
		}
		else
		{
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
			else
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );

			if( m_bBlink && m_CurrentState == UI_STATE_NORMAL )
			{
				SUIElement *pElementOver = GetElement(2);
				if( pElementOver )
				{
					if( m_bExistTemplateTexture )
						m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElementOver->TemplateUVCoord, colorMouseEnter, m_MoveButtonCoord );
					else
						m_pParent->DrawSprite( pElementOver->UVCoord, colorMouseEnter, m_MoveButtonCoord );
				}
			}
		}
		//m_pParent->DrawDlgText( m_szText.c_str(), pElement, pElement->FontColor.dwCurrentColor, uiTextCoord );
		m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, pElement->FontColor.dwCurrentColor, uiTextCoord );
	}

	pElement = GetElement(1);
	if( pElement )
	{
		pElement->TextureColor.Blend( SelectState2, fElapsedTime, m_fBlendRate );
		pElement->FontColor.Blend( SelectState2, fElapsedTime, m_fBlendRate );
		pElement->ShadowFontColor.Blend( SelectState2, fElapsedTime, m_fBlendRate );

		SUICoord uiTextCoord(textCoord);
		uiTextCoord.fX += pElement->fFontHoriOffset;
		uiTextCoord.fY += pElement->fFontVertOffset;

		if( m_Property.RadioButtonProperty.fVariableWidthLength > 0.0f )
		{
			float fBaseWidth = 0.0f;
			EtTextureHandle hTexture = m_pParent->GetUITexture();
			if( m_bExistTemplateTexture )
				hTexture = m_Template.m_hTemplateTexture;
			if( hTexture )
				fBaseWidth = m_Property.RadioButtonProperty.fVariableWidthLength * DEFAULT_UI_SCREEN_WIDTH / hTexture->Width();

			SUICoord uvLeft, uvCenter, uvRight;
			SUICoord uiLeft, uiCenter, uiRight;
			SUICoord UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;
			uiLeft.SetCoord( m_MoveButtonCoord.fX, m_MoveButtonCoord.fY, m_Property.RadioButtonProperty.fVariableWidthLength, m_MoveButtonCoord.fHeight );
			uiCenter.SetCoord( uiLeft.Right(), m_MoveButtonCoord.fY, m_MoveButtonCoord.fWidth-(m_Property.RadioButtonProperty.fVariableWidthLength*2.0f), m_MoveButtonCoord.fHeight );
			uiRight.SetCoord( uiCenter.Right(), m_MoveButtonCoord.fY, m_Property.RadioButtonProperty.fVariableWidthLength, m_MoveButtonCoord.fHeight );
			uvLeft = uvCenter = uvRight = UVCoord;
			uvLeft.fWidth = fBaseWidth;
			uvCenter.fX = UVCoord.fX + fBaseWidth;
			uvCenter.fWidth = uvCenter.fWidth-fBaseWidth*2.0f;
			uvRight.fX = UVCoord.Right() - fBaseWidth;
			uvRight.fWidth = fBaseWidth;
			m_pParent->DrawSprite( hTexture, uvLeft, pElement->TextureColor.dwCurrentColor, uiLeft );
			m_pParent->DrawSprite( hTexture, uvCenter, pElement->TextureColor.dwCurrentColor, uiCenter );
			m_pParent->DrawSprite( hTexture, uvRight, pElement->TextureColor.dwCurrentColor, uiRight );
		}
		else
		{
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
			else
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
		}
		m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, pElement->FontColor.dwCurrentColor, uiTextCoord );
		//m_pParent->DrawDlgText( m_szText.c_str(), pElement, pElement->FontColor.dwCurrentColor, uiTextCoord );
	}

	if( m_Property.RadioButtonProperty.nIconIndex > 0 && IsShow() )
	{
		pElement = GetElement(m_Property.RadioButtonProperty.nIconIndex);
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, iconCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, iconCoord );
	}
}

void CEtUIRadioButton::SetText( const std::wstring &strText, bool bFit, float fScaleX, float fScaleY )
{
	if( m_Property.RadioButtonProperty.bAutoTooltip ) {
		SetTooltipText( L"" );
		if( SetTextWithEllipsis( strText.c_str(), L"..." ) )
			SetTooltipText( strText.c_str() );
		return;
	}
	CEtUIControl::SetText( strText );
}