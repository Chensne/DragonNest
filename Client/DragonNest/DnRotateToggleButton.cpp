#include "StdAfx.h"
#include "DnRotateToggleButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define ROTATE_BUTTON_INIT_STATE_INDEX 0
#define ROTATE_BUTTON_BASIC_ELEMENT_COUNT 2

CDnRotateToggleButton::CDnRotateToggleButton(CEtUIDialog *pParent)
	: CEtUIButton(pParent)
{
	m_AllElementCount = 0;
	m_ToggleElementCount = 0;
	m_bCheckRotate = false;
	m_CurrentRotateStateIndex = ROTATE_BUTTON_INIT_STATE_INDEX ;
}

CDnRotateToggleButton::~CDnRotateToggleButton(void)
{
}

void CDnRotateToggleButton::Initialize(SUIControlProperty *pProperty)
{
	CEtUIButton::Initialize(pProperty);

	m_AllElementCount = GetElementCount();
}

bool CDnRotateToggleButton::RegisterRotate(int rotateStateIndex, int elementIndex, const WCHAR* pBtnText)
{
	if (elementIndex < 0)
		return false;

	std::map<int, SRotateToggleData>::iterator iter = m_ElementMap.find(rotateStateIndex);
	if (iter != m_ElementMap.end())
		return false;

	SRotateToggleData data;
	data.elementIndex = elementIndex;
	data.btnText = (pBtnText == NULL) ? GetText() : pBtnText;

	m_ElementMap.insert(std::make_pair(rotateStateIndex, data));

	return true;
}

bool CDnRotateToggleButton::HandleMouse(UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
				ReleaseCapture();
				m_MouseCoord.fX = 0;
				m_MouseCoord.fY = 0;
				if (IsInside( fX, fY ))
				{
					m_bCheckRotate = true;
					m_pParent->ProcessCommand(EVENT_BUTTON_ONCLICK, true, this, m_CurrentRotateStateIndex);
					if (m_bCheckRotate == false)
						return false;

					if (Advance(true) == false)
						return false;

					PlayButtonSound();

					return true;
				}
			}
		}
		break;
	}

	return CEtUIButton::HandleMouse(uMsg, fX, fY, wParam, lParam);
}

bool CDnRotateToggleButton::Advance(bool bTriggerByUser)
{
	SetState(m_CurrentRotateStateIndex + 1, true, true, bTriggerByUser);
	return true;
}

void CDnRotateToggleButton::SetState(int rotateStateIndex, bool bRotate, bool bProcessCommand, bool bTriggerByUser)
{
	if (m_ElementMap.size() <= 0)
	{
		_ASSERT(0);
		return;
	}

	int tempIdx = rotateStateIndex;
	std::map<int, SRotateToggleData>::iterator iter = m_ElementMap.find(rotateStateIndex);
	if (iter == m_ElementMap.end())
	{
		if (bRotate)
		{
			iter = m_ElementMap.begin();
			tempIdx = (*iter).first;
		}
		else
		{
			ResetState();
			return;
		}
	}

	m_CurrentRotateStateIndex = tempIdx;
	m_CurrentRotateStateData = (*iter).second;

	SetText(m_CurrentRotateStateData.btnText.c_str());

	if (bProcessCommand)
		m_pParent->ProcessCommand(EVENT_BUTTON_CLICKED, bTriggerByUser, this, m_CurrentRotateStateIndex);
}

void CDnRotateToggleButton::ResetState()
{
	m_CurrentRotateStateIndex = 0;
	m_CurrentRotateStateData.Reset();
}

void CDnRotateToggleButton::Render(float fElapsedTime)
{
	if (m_AllElementCount == ROTATE_BUTTON_BASIC_ELEMENT_COUNT)
	{
		CEtUIButton::Render(fElapsedTime);
	}
	else
	{
		m_CurrentState = UI_STATE_NORMAL;

		m_MoveButtonCoord = m_Property.UICoord;
		m_MoveBaseCoordLeft = m_BaseCoordLeft;
		m_MoveBaseCoordMid = m_BaseCoordMid;
		m_MoveBaseCoordRight = m_BaseCoordRight;

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

			m_MoveButtonCoord.fX += m_Property.ButtonProperty.fMoveOffsetHori;
			m_MoveButtonCoord.fY += m_Property.ButtonProperty.fMoveOffsetVert;

			m_MoveBaseCoordLeft.fX += m_Property.ButtonProperty.fMoveOffsetHori;
			m_MoveBaseCoordLeft.fY += m_Property.ButtonProperty.fMoveOffsetVert;

			m_MoveBaseCoordMid.fX += m_Property.ButtonProperty.fMoveOffsetHori;
			m_MoveBaseCoordMid.fY += m_Property.ButtonProperty.fMoveOffsetVert;

			m_MoveBaseCoordRight.fX += m_Property.ButtonProperty.fMoveOffsetHori;
			m_MoveBaseCoordRight.fY += m_Property.ButtonProperty.fMoveOffsetVert;
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

		if( m_Property.ButtonProperty.ButtonType == UI_BUTTON_CLICK )
		{
			pElement = GetElement(0);

			if( ( m_CurrentState == UI_STATE_MOUSEENTER ) && ( m_Template.m_vecElement.size() >= 2 ) )
			{
				pElement = GetElement(1);
			}
		}

		if( !pElement )
			return;

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

		pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

		RenderButtonSprite(pElement);

		if (m_CurrentRotateStateData.elementIndex >= 0)
		{
			SUIElement* pRotateElement = GetElement(m_CurrentRotateStateData.elementIndex);

			RenderButtonSprite(pRotateElement);
		}

		m_MoveButtonCoord.fX += pElement->fFontHoriOffset;
		m_MoveButtonCoord.fY += pElement->fFontVertOffset;

		m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, pElement->FontColor.dwCurrentColor, m_MoveButtonCoord );
	}
}


void CDnRotateToggleButton::RenderButtonSprite(SUIElement* pElement)
{
	if (pElement == NULL)
		return;

	D3DXCOLOR colorMouseEnter( pElement->TextureColor.dwColor[UI_STATE_MOUSEENTER] );
	colorMouseEnter.a = m_nBlinkAlpha / 255.0f;

	if( (pElement->fTextureWidth < m_Property.UICoord.fWidth) && m_Property.ButtonProperty.bVariableWidth )
	{
		// Note : 텍스쳐 사이즈보다 커지면 3부분으로 나누어서 출력한다.
		//		버튼의 경우는 가로 사이즈만 가변으로 한다. 세로도 가변으로 할경우 추가 작업이 필요하다.
		//
		m_MoveBaseCoordLeft.SetCoord( m_Property.UICoord.fX, m_Property.UICoord.fY, m_Property.ButtonProperty.fVariableWidthLength, m_Property.UICoord.fHeight );
		m_MoveBaseCoordMid.SetCoord( m_MoveBaseCoordLeft.Right(), m_Property.UICoord.fY, m_Property.UICoord.fWidth-(m_Property.ButtonProperty.fVariableWidthLength*2.0f), m_Property.UICoord.fHeight );
		m_MoveBaseCoordRight.SetCoord( m_MoveBaseCoordMid.Right(), m_Property.UICoord.fY, m_Property.ButtonProperty.fVariableWidthLength, m_Property.UICoord.fHeight );

		EtTextureHandle hTexture = m_pParent->GetUITexture();
		if( m_bExistTemplateTexture )
			hTexture = m_Template.m_hTemplateTexture;
		if( hTexture )
		{
			int nTextureWidth = hTexture->Width();
			float fBaseWidth = m_Property.ButtonProperty.fVariableWidthLength * DEFAULT_UI_SCREEN_WIDTH / nTextureWidth;

			SUICoord UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;

			m_uvCoordLeft = UVCoord;
			m_uvCoordLeft.fWidth = fBaseWidth;

			m_uvCoordRight = UVCoord;
			m_uvCoordRight.fX = UVCoord.Right() - fBaseWidth;
			m_uvCoordRight.fWidth = fBaseWidth;

			m_uvCoordMid = UVCoord;
			m_uvCoordMid.fX = UVCoord.fX + fBaseWidth;
			m_uvCoordMid.fWidth = m_uvCoordMid.fWidth - fBaseWidth*2.0f;
		}

		m_pParent->DrawSprite( hTexture, m_uvCoordLeft, pElement->TextureColor.dwCurrentColor, m_MoveBaseCoordLeft );
		m_pParent->DrawSprite( hTexture, m_uvCoordMid, pElement->TextureColor.dwCurrentColor, m_MoveBaseCoordMid );
		m_pParent->DrawSprite( hTexture, m_uvCoordRight, pElement->TextureColor.dwCurrentColor, m_MoveBaseCoordRight );

		if( m_bBlink && m_CurrentState == UI_STATE_NORMAL )
		{
			SUIElement *pElementOver = GetElement(1);
			SUICoord uvCoordLeft;
			SUICoord uvCoordMid;
			SUICoord uvCoordRight;
			if( hTexture )
			{
				int nTextureWidth = hTexture->Width();
				float fBaseWidth = m_Property.ButtonProperty.fVariableWidthLength * DEFAULT_UI_SCREEN_WIDTH / nTextureWidth;

				SUICoord UVCoord = pElementOver->UVCoord;
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
			m_pParent->DrawSprite( hTexture, uvCoordLeft, colorMouseEnter, m_MoveBaseCoordLeft );
			m_pParent->DrawSprite( hTexture, uvCoordMid, colorMouseEnter, m_MoveBaseCoordMid );
			m_pParent->DrawSprite( hTexture, uvCoordRight, colorMouseEnter, m_MoveBaseCoordRight );
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
			SUIElement *pElementOver = GetElement(1);
			if( pElementOver )
			{
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElementOver->TemplateUVCoord, colorMouseEnter, m_MoveButtonCoord );
				else
					m_pParent->DrawSprite( pElementOver->UVCoord, colorMouseEnter, m_MoveButtonCoord );
			}
		}
	}
}