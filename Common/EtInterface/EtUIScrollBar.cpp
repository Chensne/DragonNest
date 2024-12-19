#include "StdAfx.h"
#include "EtUIScrollBar.h"
#include "EtUIDialog.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIScrollBar::CEtUIScrollBar( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
{
	m_Property.UIType = UI_CONTROL_SCROLLBAR;

	m_bShowThumb = true;
	m_bDrag = false;

	m_nPosition = 0;
	m_nPageSize = 1;
	m_nStart = 0;
	m_nEnd = 1;
	m_Arrow = CLEAR;

	SecureZeroMemory( m_ElementCoord, sizeof(m_ElementCoord) );

	QueryPerformanceCounter( &m_liArrowTime );

	m_ElementCoord[typeUpButton] = &m_UpButtonCoord;
	m_ElementCoord[typeDownButton] = &m_DownButtonCoord;
	m_ElementCoord[typeThumb] = &m_ThumbCoord;
	m_ElementCoord[typeUpButtonLight] = &m_UpButtonLightCoord;
	m_ElementCoord[typeDownButtonLight] = &m_DownButtonLightCoord;
	m_ElementCoord[typeThumbLight] = &m_ThumbLightCoord;

	m_BaseCoord[typeBaseUp] = &m_BaseCoordUp;
	m_BaseCoord[typeBaseMid] = &m_BaseCoordMid;
	m_BaseCoord[typeBaseDown] = &m_BaseCoordDown;

	m_pParentControl = NULL;
	m_fLastMouseX = 0.0f;
	m_fLastMouseY = 0.0f;
}

CEtUIScrollBar::~CEtUIScrollBar(void)
{
}

void CEtUIScrollBar::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );

	UpdateRects();
	m_nPosition = 0;
	m_nPageSize = 10;
	SetTrackRange( 0, 30 );
}

void CEtUIScrollBar::SetTemplate( int nTemplateIndex )
{
	CEtUIControl::SetTemplate( nTemplateIndex );

	m_bExistTemplateTexture = ( m_Template.m_hTemplateTexture ) ? true : false;
}

void CEtUIScrollBar::SetTrackRange( int nStart, int nEnd )
{
	m_nStart = nStart;
	m_nEnd = nEnd;
	Cap();
	UpdateThumbRect();
}

void CEtUIScrollBar::GetTrackRange( int &nStart, int &nEnd )
{
	nStart = m_nStart;
	nEnd = m_nEnd;
}

void CEtUIScrollBar::UpdateRects()
{
	if( GetElementCount() == 0 )
		return;

	SUIElement *pElement(NULL);

	pElement = GetElement(typeUpButton);
	if( pElement )
	{
		m_UpButtonCoord.SetCoord( m_Property.UICoord.fX, m_Property.UICoord.fY, pElement->fTextureWidth, pElement->fTextureHeight );
		m_UpButtonLightCoord = m_UpButtonCoord;
	}

	pElement = GetElement(typeDownButton);
	if( pElement )
	{
		m_DownButtonCoord.SetCoord( m_Property.UICoord.fX, m_Property.UICoord.Bottom() - pElement->fTextureHeight, pElement->fTextureWidth, pElement->fTextureHeight );
		m_DownButtonLightCoord = m_DownButtonCoord;
	}

	pElement = GetElement(typeThumb);
	if( pElement )
	{
		m_ThumbCoord.fX = m_Property.UICoord.fX;
		m_ThumbCoord.fWidth = pElement->fTextureWidth;
	}
	
	pElement = GetElement(typeBase);
	if( pElement )
	{
		float fBaseUpDownHeight = 6.0f / DEFAULT_UI_SCREEN_HEIGHT;
		m_BaseCoordUp.SetCoord( m_Property.UICoord.fX, m_UpButtonCoord.Bottom(), pElement->fTextureWidth, fBaseUpDownHeight );
		m_BaseCoordMid.SetCoord( m_Property.UICoord.fX, m_BaseCoordUp.Bottom(), pElement->fTextureWidth, m_Property.UICoord.fHeight - ((m_UpButtonCoord.fHeight+fBaseUpDownHeight)*2.0f) );
		m_BaseCoordDown.SetCoord( m_Property.UICoord.fX, m_BaseCoordMid.Bottom(), pElement->fTextureWidth, fBaseUpDownHeight );

		EtTextureHandle hTexture = m_pParent->GetUITexture();
		if( m_bExistTemplateTexture )
			hTexture = m_Template.m_hTemplateTexture;
		if( hTexture )
		{
			int nTextureHeight = hTexture->Height();
			float fBaseUpDownHeightInner = 6.0f / nTextureHeight;

			SUICoord UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;

			m_BaseUVCoord[typeBaseUp] = UVCoord;
			m_BaseUVCoord[typeBaseUp].fHeight = fBaseUpDownHeightInner;

			m_BaseUVCoord[typeBaseDown] = UVCoord;
			m_BaseUVCoord[typeBaseDown].fY = UVCoord.Bottom() - fBaseUpDownHeightInner;
			m_BaseUVCoord[typeBaseDown].fHeight = fBaseUpDownHeightInner;

			m_BaseUVCoord[typeBaseMid] = UVCoord;
			m_BaseUVCoord[typeBaseMid].fY = UVCoord.fY + fBaseUpDownHeightInner;
			m_BaseUVCoord[typeBaseMid].fHeight = ((pElement->fTextureHeight * DEFAULT_UI_SCREEN_HEIGHT) - 12.0f) / nTextureHeight;
		}

		m_TrackCoord.SetCoord( m_Property.UICoord.fX, m_UpButtonCoord.Bottom(), pElement->fTextureWidth, m_Property.UICoord.fHeight - (m_UpButtonCoord.fHeight*2.0f) );
	}
	
	UpdateThumbRect();
}

void CEtUIScrollBar::UpdateThumbRect()
{
	if( m_nEnd - m_nStart > m_nPageSize )
	{
		float fThumbHeight(0.0f);

		if( m_Template.m_vecElement.size() )
		{
			SUIElement *pElement = GetElement(typeThumb);
			if( pElement )
			{
				fThumbHeight = max( m_TrackCoord.fHeight * m_nPageSize / ( m_nEnd - m_nStart ), pElement->fTextureHeight );
			}
		}
		
		int nMaxPosition = m_nEnd - m_nStart - m_nPageSize;
		m_ThumbCoord.fY = m_TrackCoord.fY + ( m_nPosition - m_nStart ) * ( m_TrackCoord.fHeight - fThumbHeight ) / nMaxPosition;
		m_ThumbCoord.fHeight = fThumbHeight;
		m_bShowThumb = true;

	} 
	else
	{
		m_ThumbCoord.fHeight = 0.0f;
		m_bShowThumb = false;
	}
}

void CEtUIScrollBar::Cap()
{
	if( ( m_nPosition < m_nStart ) || ( m_nEnd - m_nStart <= m_nPageSize ) )
	{
		m_nPosition = m_nStart;
	}
	else if( ( m_nPosition + m_nPageSize ) > m_nEnd )
	{
		m_nPosition = m_nEnd - m_nPageSize;
	}
}

void CEtUIScrollBar::Scroll( int nDelta )
{
	if( !IsEnable() )
		return;

	m_nPosition += nDelta;
	Cap();
	UpdateThumbRect();
}

void CEtUIScrollBar::ShowItem( int nIndex )
{
	if( nIndex < 0 )
	{
		nIndex = 0;
	}
	else if( nIndex >= m_nEnd )
	{
		nIndex = m_nEnd - 1;
	}

	if( m_nPosition > nIndex )
	{
		m_nPosition = nIndex;
	}
	else
	{
		if( m_nPosition + m_nPageSize <= nIndex )
		{
			m_nPosition = nIndex - m_nPageSize + 1;
		}
	}

	UpdateThumbRect();
}

bool CEtUIScrollBar::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() )
		return false;

	static float fThumbOffsetY;

	m_fLastMouseX = fX;
	m_fLastMouseY = fY;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if( m_UpButtonCoord.IsInside( fX, fY ) )
		{
			SetCapture( m_pParent->GetHWnd() );
			Scroll( -1 );
			m_Arrow = CLICKED_UP;
			QueryPerformanceCounter( &m_liArrowTime );
			return true;
		}
		if( m_DownButtonCoord.IsInside( fX, fY ) )
		{
			SetCapture( m_pParent->GetHWnd() );
			Scroll( 1 );
			m_Arrow = CLICKED_DOWN;
			QueryPerformanceCounter( &m_liArrowTime );
			return true;
		}
		if( m_ThumbCoord.IsInside( fX, fY ) )
		{
			SetCapture( m_pParent->GetHWnd() );
			m_bDrag = true;
			fThumbOffsetY = fY - m_ThumbCoord.fY;
			return true;
		}
		if( ( fX >= m_ThumbCoord.fX ) && ( fX < m_ThumbCoord.fX + m_ThumbCoord.fWidth ) )
		{
			SetCapture( m_pParent->GetHWnd() );
			if( ( fY < m_ThumbCoord.fY ) && ( fY >= m_TrackCoord.fY ) )
			{
				Scroll( -( m_nPageSize - 1 ) );
				m_Arrow = CLICKED_UP;
				QueryPerformanceCounter( &m_liArrowTime );
				return true;
			} 
			else if( ( fY >= m_ThumbCoord.Bottom() ) && ( fY < m_TrackCoord.Bottom() ) )
			{
				Scroll( m_nPageSize - 1 );
				m_Arrow = CLICKED_DOWN;
				QueryPerformanceCounter( &m_liArrowTime );
				return true;
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			m_bDrag = false;
			ReleaseCapture();
			UpdateThumbRect();
			m_Arrow = CLEAR;
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( m_bDrag )
			{
				m_ThumbCoord.fY = fY - fThumbOffsetY;

				if( m_ThumbCoord.fY < m_TrackCoord.fY )
				{
					m_ThumbCoord.fY = m_TrackCoord.fY;
				}
				else if( m_ThumbCoord.Bottom() > m_TrackCoord.Bottom() )
				{
					m_ThumbCoord.fY = m_TrackCoord.Bottom() - m_ThumbCoord.fHeight;
				}

				int nMaxFirstItem = m_nEnd - m_nStart - m_nPageSize;
				float fMaxThumb = m_TrackCoord.fHeight - m_ThumbCoord.fHeight;

				m_nPosition = m_nStart + ( int )( ( m_ThumbCoord.fY - m_TrackCoord.fY + fMaxThumb / ( nMaxFirstItem * 2 ) ) * nMaxFirstItem  / fMaxThumb );
				return true;
			}
		}
		break;
	}

	return false;
}

bool CEtUIScrollBar::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( WM_CAPTURECHANGED == uMsg )
	{
		if( (HWND)lParam != m_pParent->GetHWnd() )
		{
			m_bDrag = false;
		}
	}

	return false;
}

void CEtUIScrollBar::Render( float fElapsedTime )
{
	if( m_Template.m_vecElement.empty() )
	{
		return;
	}

	if( m_Arrow != CLEAR )
	{
		LARGE_INTEGER liCurTime, liFrequency;
		float fElapsedArrowTime;

		QueryPerformanceFrequency( &liFrequency );
		QueryPerformanceCounter( &liCurTime );
		fElapsedArrowTime = ( ( float )( liCurTime.QuadPart - m_liArrowTime.QuadPart ) ) / liFrequency.QuadPart;

		if( m_UpButtonCoord.IsInside( m_fLastMouseX, m_fLastMouseY ) )
		{
			switch( m_Arrow )
			{
			case CLICKED_UP:
				if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_DELAY )
				{
					Scroll( -1 );
					m_Arrow = HELD_UP;
					m_liArrowTime = liCurTime;
				}
				break;
			case HELD_UP:
				if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_REPEAT )
				{
					Scroll( -1 );
					m_liArrowTime = liCurTime;
				}
				break;
			}
		} 
		else if ( m_DownButtonCoord.IsInside( m_fLastMouseX, m_fLastMouseY ) )
		{
			switch( m_Arrow )
			{
			case CLICKED_DOWN:
				if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_DELAY )
				{
					Scroll( 1 );
					m_Arrow = HELD_DOWN;
					m_liArrowTime = liCurTime;
				}
				break;
			case HELD_DOWN:
				if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_REPEAT )
				{
					Scroll( 1 );
					m_liArrowTime = liCurTime;
				}
				break;
			}
		}
		else if( ( m_fLastMouseX >= m_ThumbCoord.fX ) && ( m_fLastMouseX < m_ThumbCoord.fX + m_ThumbCoord.fWidth ) )
		{
			if( ( m_fLastMouseY < m_ThumbCoord.fY ) && ( m_fLastMouseY >= m_TrackCoord.fY ) )
			{
				switch( m_Arrow )
				{
				case CLICKED_UP:
					if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_DELAY )
					{
						Scroll( -( m_nPageSize - 1 ) );
						m_Arrow = HELD_UP;
						m_liArrowTime = liCurTime;
					}
					break;
				case HELD_UP:
					if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_REPEAT )
					{
						Scroll( -( m_nPageSize - 1 ) );
						m_liArrowTime = liCurTime;
					}
					break;
				}
			} 
			else if( ( m_fLastMouseY >= m_ThumbCoord.Bottom() ) && ( m_fLastMouseY < m_TrackCoord.Bottom() ) )
			{
				switch( m_Arrow )
				{
				case CLICKED_DOWN:
					if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_DELAY )
					{
						Scroll( m_nPageSize - 1 );
						m_Arrow = HELD_DOWN;
						m_liArrowTime = liCurTime;
					}
					break;
				case HELD_DOWN:
					if( fElapsedArrowTime > SCROLLBAR_ARROWCLICK_REPEAT )
					{
						Scroll( m_nPageSize - 1 );
						m_liArrowTime = liCurTime;
					}
					break;
				}
			}
		}
	}

	m_CurrentState = UI_STATE_NORMAL;

	if( !IsShow() )
	{
		m_CurrentState = UI_STATE_HIDDEN;
	}
	else if( !IsEnable() || !m_bShowThumb )
	{
		m_CurrentState = UI_STATE_DISABLED;
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
	
	// Note : 스크롤바의 Base 부분
	{
		pElement = GetElement(typeBase);
		pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

		if( m_bExistTemplateTexture )
		{
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_BaseUVCoord[typeBaseUp], pElement->TextureColor.dwCurrentColor, (*m_BaseCoord[typeBaseUp]) );
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_BaseUVCoord[typeBaseDown], pElement->TextureColor.dwCurrentColor, (*m_BaseCoord[typeBaseDown]) );
			m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_BaseUVCoord[typeBaseMid], pElement->TextureColor.dwCurrentColor, (*m_BaseCoord[typeBaseMid]) );
		}
		else
		{
			m_pParent->DrawSprite( m_BaseUVCoord[typeBaseUp], pElement->TextureColor.dwCurrentColor, (*m_BaseCoord[typeBaseUp]) );
			m_pParent->DrawSprite( m_BaseUVCoord[typeBaseDown], pElement->TextureColor.dwCurrentColor, (*m_BaseCoord[typeBaseDown]) );
			m_pParent->DrawSprite( m_BaseUVCoord[typeBaseMid], pElement->TextureColor.dwCurrentColor, (*m_BaseCoord[typeBaseMid]) );
		}
	}

	pElement = GetElement(typeUpButton);
	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeUpButton]) );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeUpButton]) );

	pElement = GetElement(typeDownButton);
	pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeDownButton]) );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeDownButton]) );

	{ // Note : Thumb Render
		pElement = GetElement(typeThumb);
		pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

		// Note : 텍스쳐 사이즈보다 커지면 3부분으로 나누어서 출력한다.
		//
		if( pElement->fTextureHeight < m_ElementCoord[typeThumb]->fHeight )
		{
			SUICoord coordUp, coordMid, coordBottom;
			SUICoord uvCoordUp, uvCoordMid, uvCoordBottom;

			coordUp.SetCoord( m_ElementCoord[typeThumb]->fX, m_ElementCoord[typeThumb]->fY, m_ElementCoord[typeThumb]->fWidth, pElement->fFontVertOffset );
			coordMid.SetCoord( m_ElementCoord[typeThumb]->fX, coordUp.Bottom(), m_ElementCoord[typeThumb]->fWidth, m_ElementCoord[typeThumb]->fHeight - (pElement->fFontVertOffset*2.0f) );
			coordBottom.SetCoord( m_ElementCoord[typeThumb]->fX, coordMid.Bottom(), m_ElementCoord[typeThumb]->fWidth, pElement->fFontVertOffset );

			EtTextureHandle hTexture = m_pParent->GetUITexture();
			if( m_bExistTemplateTexture )
				hTexture = m_Template.m_hTemplateTexture;
			if( hTexture )
			{
				int nTextureHeight = hTexture->Height();
				float fBaseHeight = pElement->fFontVertOffset * DEFAULT_UI_SCREEN_HEIGHT / nTextureHeight;

				SUICoord UVCoord = pElement->UVCoord;
				if( m_bExistTemplateTexture )
					UVCoord = pElement->TemplateUVCoord;

				uvCoordUp = UVCoord;
				uvCoordUp.fHeight = fBaseHeight;

				uvCoordBottom = UVCoord;
				uvCoordBottom.fY = UVCoord.Bottom() - fBaseHeight;
				uvCoordBottom.fHeight = fBaseHeight;

				uvCoordMid = UVCoord;
				uvCoordMid.fY = UVCoord.fY + fBaseHeight;
				uvCoordMid.fHeight = ((pElement->fTextureHeight-(pElement->fFontVertOffset*2.0f))*DEFAULT_UI_SCREEN_HEIGHT) / nTextureHeight;
			}

			m_pParent->DrawSprite( hTexture, uvCoordUp, pElement->TextureColor.dwCurrentColor, coordUp );
			m_pParent->DrawSprite( hTexture, uvCoordMid, pElement->TextureColor.dwCurrentColor, coordMid );
			m_pParent->DrawSprite( hTexture, uvCoordBottom, pElement->TextureColor.dwCurrentColor, coordBottom );
		}
		else
		{
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeThumb]) );
			else
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeThumb]) );
		}
	}

	if( m_pParentControl && m_bShowThumb && (m_Template.m_vecElement.size() > 4) )
	{
		m_fLastMouseX = CEtUIDialog::s_fScreenMouseX - m_pParent->GetXCoord();
		m_fLastMouseY = CEtUIDialog::s_fScreenMouseY - m_pParent->GetYCoord();

		if( m_UpButtonCoord.IsInside( m_fLastMouseX, m_fLastMouseY ) )
		{
			pElement = GetElement(typeUpButtonLight);
			pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeUpButtonLight]) );
			else
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeUpButtonLight]) );
		}
		else if( m_DownButtonCoord.IsInside( m_fLastMouseX, m_fLastMouseY ) )
		{
			pElement = GetElement(typeDownButtonLight);
			pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
			if( m_bExistTemplateTexture )
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeDownButtonLight]) );
			else
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeDownButtonLight]) );
		}

		if( m_ThumbCoord.IsInside( m_fLastMouseX, m_fLastMouseY ) || m_bDrag )
		{
			pElement = GetElement(typeThumbLight);
			pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );

			// Note : 텍스쳐 사이즈보다 커지면 3부분으로 나누어서 출력한다.
			//
			if( pElement->fTextureHeight < m_ElementCoord[typeThumb]->fHeight )
			{
				SUICoord coordUp, coordMid, coordBottom;
				SUICoord uvCoordUp, uvCoordMid, uvCoordBottom;

				coordUp.SetCoord( m_ElementCoord[typeThumb]->fX, m_ElementCoord[typeThumb]->fY, m_ElementCoord[typeThumb]->fWidth, pElement->fFontVertOffset );
				coordMid.SetCoord( m_ElementCoord[typeThumb]->fX, coordUp.Bottom(), m_ElementCoord[typeThumb]->fWidth, m_ElementCoord[typeThumb]->fHeight - (pElement->fFontVertOffset*2.0f) );
				coordBottom.SetCoord( m_ElementCoord[typeThumb]->fX, coordMid.Bottom(), m_ElementCoord[typeThumb]->fWidth, pElement->fFontVertOffset );

				EtTextureHandle hTexture = m_pParent->GetUITexture();
				if( m_bExistTemplateTexture )
					hTexture = m_Template.m_hTemplateTexture;
				if( hTexture )
				{
					int nTextureHeight = hTexture->Height();
					float fBaseHeight = pElement->fFontVertOffset * DEFAULT_UI_SCREEN_HEIGHT / nTextureHeight;

					SUICoord UVCoord = pElement->UVCoord;
					if( m_bExistTemplateTexture )
						UVCoord = pElement->TemplateUVCoord;

					uvCoordUp = UVCoord;
					uvCoordUp.fHeight = fBaseHeight;

					uvCoordBottom = UVCoord;
					uvCoordBottom.fY = UVCoord.Bottom() - fBaseHeight;
					uvCoordBottom.fHeight = fBaseHeight;

					uvCoordMid = UVCoord;
					uvCoordMid.fY = UVCoord.fY + fBaseHeight;
					uvCoordMid.fHeight = ((pElement->fTextureHeight-(pElement->fFontVertOffset*2.0f))*DEFAULT_UI_SCREEN_HEIGHT) / nTextureHeight;
				}

				m_pParent->DrawSprite( hTexture, uvCoordUp, pElement->TextureColor.dwCurrentColor, coordUp );
				m_pParent->DrawSprite( hTexture, uvCoordMid, pElement->TextureColor.dwCurrentColor, coordMid );
				m_pParent->DrawSprite( hTexture, uvCoordBottom, pElement->TextureColor.dwCurrentColor, coordBottom );
			}
			else
			{
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeThumb]) );
				else
					m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, (*m_ElementCoord[typeThumb]) );
			}
		}
	}

	//m_pParent->DrawRect( m_Property.UICoord, EtInterface::debug::RED );
}

void CEtUIScrollBar::FindInputPos( std::vector<EtVector2> &vecPos )
{
	CEtUIDialog *pDialog = GetParent();
	if( !pDialog ) return;

	float fPosX = 0.0f;
	float fPosY = 0.0f;

	if( m_bShowThumb )
	{
		fPosX = pDialog->GetXCoord() + m_UpButtonCoord.fX + m_UpButtonCoord.fWidth / 2.0f;
		fPosY = pDialog->GetYCoord() + m_UpButtonCoord.fY + m_UpButtonCoord.fHeight / 2.0f;
		vecPos.push_back( EtVector2(fPosX, fPosY) );

		fPosX = pDialog->GetXCoord() + m_DownButtonCoord.fX + m_DownButtonCoord.fWidth / 2.0f;
		fPosY = pDialog->GetYCoord() + m_DownButtonCoord.fY + m_DownButtonCoord.fHeight / 2.0f;
		vecPos.push_back( EtVector2(fPosX, fPosY) );
	}
}