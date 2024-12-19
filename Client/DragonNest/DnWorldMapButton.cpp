#include "StdAfx.h"
#include "DnWorldMapButton.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldMapButton::CDnWorldMapButton( CEtUIDialog *pParent )
	: CEtUIButton( pParent )
	, m_bOpened( false )
	, m_bBlink( false )
	, m_emMapButtonQuestType(typeNoneQuestButton)
	, m_fElapsedTime(0.0f)
{
}

CDnWorldMapButton::~CDnWorldMapButton(void)
{
}

void CDnWorldMapButton::Render( float fElapsedTime )
{
	/*if( !IsMouseEnter() && m_bPressed )
	{
		m_bPressed = false;
		ReleaseCapture();
	}*/

	m_CurrentState = UI_STATE_NORMAL;
	m_MoveButtonCoord = m_Property.UICoord;

	if( !IsShow() )
	{
		m_CurrentState = UI_STATE_HIDDEN;
	}
	else if( !IsEnable() )
	{
		m_CurrentState = UI_STATE_DISABLED;
	}
	else if( m_bPressed && m_bOpened )
	{
		m_CurrentState = UI_STATE_PRESSED;

		m_MoveButtonCoord.fX += m_Property.ButtonProperty.fMoveOffsetHori;
		m_MoveButtonCoord.fY += m_Property.ButtonProperty.fMoveOffsetVert;
	}
	else if( m_bMouseEnter && m_bOpened )
	{
		m_CurrentState = UI_STATE_MOUSEENTER;
	}
	else if( m_bFocus && m_bOpened )
	{
		m_CurrentState = UI_STATE_FOCUS;
	}

	UpdateBlendRate();

	SUIElement *pElement(NULL);

	if( m_bOpened == false )
	{
		pElement = GetElement(2);
	}
	else
	{
		if( m_Property.ButtonProperty.ButtonType == UI_BUTTON_CLICK )
		{
			pElement = GetElement(0);

			if( ( m_CurrentState == UI_STATE_MOUSEENTER ) && ( m_Template.m_vecElement.size() >= 2 ) )
			{
				pElement = GetElement(1);
			}
		}
	}

	if( !pElement ) 
		return;

	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
	m_pParent->DrawDlgText( m_szText.c_str(), pElement, pElement->FontColor.dwCurrentColor, m_MoveButtonCoord );
	
	if( m_bBlink ) {

		LOCAL_TIME time = CGlobalInfo::GetInstance().m_LocalTime;

		int nAlpha = 255;
		int BLEND_TIME = 500;
		float BLEND_RANGE  = 1.0f;
		int nTemp = (int)time%(BLEND_TIME+BLEND_TIME);
		if( nTemp < BLEND_TIME ) 
		{
			nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * nTemp * 255.0f);
		}
		else 
		{
			nAlpha = int(( BLEND_RANGE / (float)BLEND_TIME ) * ( (BLEND_TIME+BLEND_TIME) - nTemp ) *  255.0f);
		}
		nAlpha = min(nAlpha, 255);
		pElement = GetElement(1);
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), m_MoveButtonCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), m_MoveButtonCoord );
	}


	emMapButtonQuestType questType = GetButtonQuestType();
	if( questType > typeNoneQuestButton )
	{
		pElement = GetElement(questType);

		SUICoord questCoord;
		questCoord.SetSize( pElement->fTextureWidth, pElement->fTextureHeight );
		questCoord.fX = m_MoveButtonCoord.fX + (m_MoveButtonCoord.fWidth/2.0f);
		questCoord.fY = m_MoveButtonCoord.fY - (questCoord.fHeight/2.0f);

		//if( m_fElapsedTime >= 0.0f )
		//{
		//	if( m_fElapsedTime <= 0.2f )
		//	{
		//		m_CurrentState = UI_STATE_HIDDEN;
		//	}
		//	else if( m_fElapsedTime <= 1.0f )
		//	{
		//		m_CurrentState = UI_STATE_NORMAL;
		//	}

		//	m_fElapsedTime -= fElapsedTime;
		//}
		//else
		//{
		//	m_fElapsedTime = 1.0f;
		//}

		//pElement->TextureColor.Blend( UI_STATE_NORMAL, fElapsedTime, 0.8f );
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, questCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, questCoord );
	}
}