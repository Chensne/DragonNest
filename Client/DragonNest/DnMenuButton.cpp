#include "StdAfx.h"
#include "DnMenuButton.h"
#include "DnCustomControlCommon.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMenuButton::CDnMenuButton( CEtUIDialog *pParent )
	: CEtUIButton( pParent )
	, m_bOpenDlg(false)
	, m_bBlink(false)
{
}

CDnMenuButton::~CDnMenuButton(void)
{
	SAFE_RELEASE_SPTR( m_hShortCutIcon );
}

void CDnMenuButton::Initialize( SUIControlProperty *pProperty )
{
	CEtUIButton::Initialize( pProperty );
	m_hShortCutIcon = LoadResource( CEtResourceMng::GetInstance().GetFullName( "MainBarShortCut.dds" ).c_str(), RT_TEXTURE );
	m_ShortCutCoord = m_Property.UICoord;
	m_ShortCutCoord.fWidth = (float)QUICKSLOT_SHORCUT_ICON_XSIZE / DEFAULT_UI_SCREEN_WIDTH;
	m_ShortCutCoord.fHeight = (float)QUICKSLOT_SHORCUT_ICON_YSIZE / DEFAULT_UI_SCREEN_HEIGHT;
	m_ShortCutCoord.fY = m_Property.UICoord.Bottom() - m_ShortCutCoord.fHeight;
}

void CDnMenuButton::Render( float fElapsedTime )
{
	m_CurrentState = UI_STATE_NORMAL;

	m_MoveButtonCoord = m_Property.UICoord;
	m_MoveBaseCoordLeft = m_BaseCoordLeft;
	m_MoveBaseCoordMid = m_BaseCoordMid;
	m_MoveBaseCoordRight = m_BaseCoordRight;

	SUICoord uiShortCut = m_ShortCutCoord;

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

		uiShortCut.fX += m_Property.ButtonProperty.fMoveOffsetHori;
		uiShortCut.fY += m_Property.ButtonProperty.fMoveOffsetVert;
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

	if( m_CurrentState == UI_STATE_MOUSEENTER )
	{
		pElement = GetElement(1);
	}
	else
	{
		if( m_bOpenDlg )
		{
			pElement = GetElement(2);
		}
		else
		{
			pElement = GetElement(0);
		}
	}

	if( !pElement )
		return;

	if( m_bBlink )
	{
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
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), m_MoveButtonCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, D3DCOLOR_ARGB(nAlpha,255,255,255), m_MoveButtonCoord );

		// �����Կ� ��Ű�� �׻� ������, �޴��� ���� �� ������ �˻�.
		if( m_Property.nHotKey > 0 )
			m_pParent->DrawSprite( m_hShortCutIcon, m_ShortCutUV, D3DCOLOR_ARGB(nAlpha,255,255,255), uiShortCut );
	}
	else
	{
		pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		if( m_bExistTemplateTexture )
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );
		else
			m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_MoveButtonCoord );

		if( m_Property.nHotKey > 0 )
			m_pParent->DrawSprite( m_hShortCutIcon, m_ShortCutUV, pElement->TextureColor.dwCurrentColor, uiShortCut );
	}
}

void CDnMenuButton::SetOpen( bool bOpen )
{ 
	m_bOpenDlg = bOpen;

	if( bOpen && m_bBlink )
	{
		m_bBlink = false;
	}
}

void CDnMenuButton::SetHotKey( int nKey )
{
	CEtUIButton::SetHotKey( nKey );
	CalcHotKeyUV( nKey, m_hShortCutIcon, m_ShortCutUV, QUICKSLOT_SHORCUT_ICON_XSIZE, QUICKSLOT_SHORCUT_ICON_YSIZE );
}