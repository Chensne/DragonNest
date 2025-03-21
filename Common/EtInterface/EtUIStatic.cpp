#include "StdAfx.h"
#include "EtUIStatic.h"
#include "EtUIDialog.h"
#include "EtUIXML.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUIStatic::CEtUIStatic( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
	, m_bBlink(false)
	, m_fBlinkTime(0.0f)
	, m_nBlinkAlpha(0)
{
	m_fBlendRate					= 0.8f;
	m_bManualControlColor			= false;
	m_fWidthScale = m_fHeightScale	= 1.f;
	m_BgTextColor.dwCurrentColor = 0;
	for( int i = 0; i < UI_STATE_COUNT; i++) {
		m_BgTextColor.dwColor[ i ] = 0;
	}
	m_fRotate = 0.f;
#ifdef FIX_STATICCONTROL_FOCUS_PROCESS
#else
	m_bCanHaveFocus = false;
#endif
	m_bButton = false;
	m_bForceBlend = false;

	// #14364 스태틱 컨트롤 왼쪽으로 누르고 화면 밖으로 커서 옮기고 떼면, SetCapture된 상태에서 ReleaseCapture없이 끝나게 되서 툴팁이 안뜬다.
	// (일반적인 버튼이라면, 화면 밖에서 마우스버튼 뗄때 다이얼로그 MsgProc의 focus::HandleMouse가 호출되면서 캡쳐상태가 풀려야한다.)
	// Static컨트롤은 SetCapture를 하지 않는게 원칙이어서 포커스컨트롤로 설정이 안되었었는데,
	// 어느분이 Static컨트롤을 버튼처럼 SetCapture하도록 바꿔놓아서, 어쩔 수 없이 포커스를 받게 바꾼다.
	//
	// 그렇다고 그냥 포커스를 받도록 처리하면, 포커스를 가지지 않아야할 상황에서도 자꾸 포커스가 가버려서 안된다.
	// 결국 선택한 방법은,
	// 버튼처럼 동작은 해야 기존 코드가 돌아가니 마우스 업,다운 처리는 그대로 하되,(Pressed는 처리)
	// 포커스를 가진다거나 하는 쪽은 다 빼기로 하겠다.(Capture도 포함)
}

CEtUIStatic::~CEtUIStatic(void)
{
}

void CEtUIStatic::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );

	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	if( m_Property.StaticProperty.nStringIndex > 0 )
	{
		SetText( GetPropertyString() );
	}
	else if( EtInterface::g_bEtUIInitTool )
	{
		if( (pElement->UVCoord.fWidth == 0.0f) && (pElement->UVCoord.fHeight == 0.0f) )
		{
			SetText( L"Static" );
		}
	}

	DWORD dwNormalColor = pElement->FontColor.dwColor[UI_STATE_NORMAL];
	for( int i = 0; i < UI_STATE_COUNT; i++) {	// 노멀 컬러와 같은건 다바꿔준다.
		if( pElement->FontColor.dwColor[ i ] == dwNormalColor ) {
			pElement->FontColor.dwColor[ i ] = m_Property.StaticProperty.dwFontColor;
		}
	}

	pElement->FontColor.dwCurrentColor = m_Property.StaticProperty.dwFontColor;
	pElement->bShadowFont = m_Property.StaticProperty.bShadowFont?true:false;

	// 원래는 
	// pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL] = m_Property.StaticProperty.dwShadowFontColor;
	// 이거 한줄만 있는게 정상인데,
	// 이 쉐도우폰트컬러가 프로젝트 중간에 추가된 인자다.
	//
	// 문제는 중간에 추가되었기때문에, 이전 버전 데이터들을 로딩할때 0으로 설정되어,
	// 로딩 후 일일이 바꿔줘야하는 일이 생긴다.(그냥 새로 추가하는건 당연히 문제없다.)
	//
	// 몇몇 다른 곳에 사용되는 인자들은 이렇게 되도 개수가 적어서 문제가 없었지만,
	// 이 쉐도우폰트의 경우엔 개수가 너무 많아서 이렇게 할 수가 없다.
	//
	// 그래서 0이 아닐때만 설정하도록 해야한다.(0이면 기본 템플릿의 쉐도우 폰트색상을 사용한다는 의미다.)
	//
	// 그 아래 반대의 대입이 있는건, (이거-> m_Property.StaticProperty.dwShadowFontColor = pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL];)
	// 이게 없을 경우
	// 로딩을 해도 쉐도우폰트가 계속 0으로 설정되어있고, 다른이름으로 새로 저장해도 별도의 변경이 없었으니 계속 0으로 저장되게된다.
	// 그 후 다음번 로딩때도 계속 0이라 기본 템플릿의 쉐도우 폰트색상을 사용하게된다.
	// 그래서 이런 경우엔 직접 바꿔주도록 했다.
	//
	// 이 방식의 문제점은 쉐도우폰트색상이 실제로 0x00000000으로 설정될때 실제 0으로 적용안되고, 기본 템플릿의 쉐도우폰트값으로 설정된다는 것이다.
	// 이럴 일은 없을테니, 한동안 이렇게 사용하도록 하겠다.

	if( m_Property.StaticProperty.dwShadowFontColor != 0 )
		pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL] = m_Property.StaticProperty.dwShadowFontColor;
	else
		m_Property.StaticProperty.dwShadowFontColor = pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL];

	if( m_Property.StaticProperty.dwTextureColor != 0xFFFFFFFF )
		SetTextureColor( m_Property.StaticProperty.dwTextureColor, 0 );

	if( m_Property.StaticProperty.bAutoTooltip == false )
		m_strRenderText = m_szText;
}

void CEtUIStatic::Process( float fElapsedTime )
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
	else if( m_bPressed && m_Property.StaticProperty.bProcessInput )
	{
		m_CurrentState = UI_STATE_PRESSED;
	}
	else if( m_bFocus && m_Property.StaticProperty.bProcessInput )
	{
		m_CurrentState = UI_STATE_FOCUS;
	}
	else if( m_bMouseEnter && m_Property.StaticProperty.bProcessInput )
	{
		m_CurrentState = UI_STATE_MOUSEENTER;
	}

	if( m_bForceBlend )
	{
		fElapsedTime = m_fBlendRate;
	}
	else
	{
		UpdateBlendRate();
	}

	if (m_bManualControlColor == false)
	{
		SUIElement *pElement = GetElement(0);
		if( !pElement ) return;		

		pElement->TextureColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->FontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		pElement->ShadowFontColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
		m_BgTextColor.Blend( m_CurrentState, fElapsedTime, m_fBlendRate );
	}
}

void CEtUIStatic::Show( bool bShow )
{
	if (bShow == false && m_bManualControlColor)
		m_bManualControlColor = false;

	CEtUIControl::Show(bShow);
}

void CEtUIStatic::Render( float fElapsedTime )
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	m_nBlinkAlpha = 255;

	SUICoord coord;
	coord = m_Property.UICoord;
	if (m_fWidthScale != 1.f)
		coord.fWidth *= m_fWidthScale;
	if (m_fHeightScale != 1.f)
		coord.fHeight *= m_fHeightScale;

	if( m_bBlink )
	{
		//LOCAL_TIME time = CGlobalInfo::GetInstance().m_LocalTime;
		m_fBlinkTime += fElapsedTime * 1000;

		int BLEND_TIME = 500;
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

	D3DXCOLOR textureColor( pElement->TextureColor.dwCurrentColor );
	textureColor.a = m_nBlinkAlpha/255.0f;

	if( (m_Property.StaticProperty.nFrameLeft > 0) || (m_Property.StaticProperty.nFrameTop > 0) 
		|| (m_Property.StaticProperty.nFrameRight > 0) || (m_Property.StaticProperty.nFrameBottom > 0) )
	{
		if( m_bExistTemplateTexture )
		{
			for( int i=0; i<9; i++ )
			{
				if( m_bBlink )
				{
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_BaseUVCoord[i], textureColor, m_BaseCoord[i], m_fRotate, m_fZValue );
				}
				else
				{
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, m_BaseUVCoord[i], pElement->TextureColor.dwCurrentColor, m_BaseCoord[i], m_fRotate, m_fZValue );
					//m_pParent->DrawRect( m_BaseCoord[i], EtInterface::debug::BLUE );
				}
			}
		}
		else
		{
			for( int i=0; i<9; i++ )
			{
				if( m_bBlink )
				{
					m_pParent->DrawSprite( m_BaseUVCoord[i], textureColor, m_BaseCoord[i], m_fRotate, m_fZValue );
				}
				else
				{
					m_pParent->DrawSprite( m_BaseUVCoord[i], pElement->TextureColor.dwCurrentColor, m_BaseCoord[i], m_fRotate, m_fZValue );
					//m_pParent->DrawRect( m_BaseCoord[i], EtInterface::debug::BLUE );
				}
			}
		}
	}
	else
	{
		if( m_bExistTemplateTexture )
		{
			if( m_bBlink )
			{
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, textureColor, coord, m_fRotate, m_fZValue );
			}
			else
			{
				m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, coord, m_fRotate, m_fZValue );
				//m_pParent->DrawRect( coord, EtInterface::debug::BLUE );
			}
		}
		else
		{
			if( m_bBlink )
			{
				m_pParent->DrawSprite( pElement->UVCoord, textureColor, coord, m_fRotate, m_fZValue );
			}
			else
			{
				m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, coord, m_fRotate, m_fZValue);
				//m_pParent->DrawRect( coord, EtInterface::debug::BLUE );
			}
		}
	}	

#ifdef PRE_UISTATIC_PARTTEXT_COLOR
	if (m_PartTextColorInfoList.size() > 0)
	{
		SUICoord uiTextCoord(m_FontCoord);
		uiTextCoord.fX += pElement->fFontHoriOffset;
		uiTextCoord.fY += pElement->fFontVertOffset;

		std::list<SPartTextInfo>::const_iterator iter = m_PartTextColorInfoList.begin();
		for (; iter != m_PartTextColorInfoList.end(); ++iter)
		{
			const SPartTextInfo& info = (*iter);
			SUICoord partTextStartCoord(uiTextCoord);
			partTextStartCoord.fX += info.fPartWidthOffset;
			partTextStartCoord.fWidth = info.fWidth;

			if (m_bBlink)
			{
				D3DXCOLOR fontColor(info.dwTextColor);
				fontColor.a = m_nBlinkAlpha/255.0f;

				//	todo : TextDlg도 texture scaling 시에 함께 변경되어야 하지만, 프레임 정렬 등과 맞물려있고, 폰트 확축 관련한 문제도 있어 우선 적용 않는다. 추후 수정 필요. by kalliste
				m_pParent->DrawDlgText(info.text.c_str(), pElement, fontColor, partTextStartCoord, -1, info.dwFontFormat, false, m_fZValue , info.dwBGColor);
			}
			else
			{
				m_pParent->DrawDlgText(info.text.c_str(), pElement, info.dwTextColor, partTextStartCoord, -1, info.dwFontFormat, false, m_fZValue , info.dwBGColor);
			}
		}
	}
	else
	{
		if( !m_strRenderText.empty() ) {
			SUICoord uiTextCoord(m_FontCoord);
			uiTextCoord.fX += pElement->fFontHoriOffset;
			uiTextCoord.fY += pElement->fFontVertOffset;
			if( m_bBlink )
			{
				D3DXCOLOR fontColor( pElement->FontColor.dwCurrentColor );
				fontColor.a = m_nBlinkAlpha/255.0f;

				//	todo : TextDlg도 texture scaling 시에 함께 변경되어야 하지만, 프레임 정렬 등과 맞물려있고, 폰트 확축 관련한 문제도 있어 우선 적용 않는다. 추후 수정 필요. by kalliste
				m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, fontColor, uiTextCoord, -1, m_Property.StaticProperty.dwFontFormat , false, m_fZValue , m_BgTextColor.dwCurrentColor );
			}
			else
			{
				m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, pElement->FontColor.dwCurrentColor, uiTextCoord, -1, m_Property.StaticProperty.dwFontFormat, false, m_fZValue , m_BgTextColor.dwCurrentColor );
			}
		}
	}
#else
	if( !m_strRenderText.empty() ) {
		SUICoord uiTextCoord(m_FontCoord);
		uiTextCoord.fX += pElement->fFontHoriOffset;
		uiTextCoord.fY += pElement->fFontVertOffset;
		if( m_bBlink )
		{
			D3DXCOLOR fontColor( pElement->FontColor.dwCurrentColor );
			fontColor.a = m_nBlinkAlpha/255.0f;

			//	todo : TextDlg도 texture scaling 시에 함께 변경되어야 하지만, 프레임 정렬 등과 맞물려있고, 폰트 확축 관련한 문제도 있어 우선 적용 않는다. 추후 수정 필요. by kalliste
			m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, fontColor, uiTextCoord, -1, m_Property.StaticProperty.dwFontFormat , false, m_fZValue , m_BgTextColor.dwCurrentColor );
		}
		else
		{
			m_pParent->DrawDlgText( m_strRenderText.c_str(), pElement, pElement->FontColor.dwCurrentColor, uiTextCoord, -1, m_Property.StaticProperty.dwFontFormat, false, m_fZValue , m_BgTextColor.dwCurrentColor );
		}
	}
#endif

//#define _KAL_TEST
#ifdef _KAL_TEST
	SFontDrawEffectInfo Info;
	Info.dwFontColor = 0xFFFFFFFF;
	wchar_t wszBuff[256]={0};
	MultiByteToWideChar(CP_UTF8, 0, m_Property.szUIName, -1, wszBuff, (int)strlen(m_Property.szUIName) );

	SUICoord screenCoord;
	m_pParent->GetPosition(screenCoord.fX, screenCoord.fY);
	screenCoord.SetCoord(m_Property.UICoord.fX + screenCoord.fX, m_Property.UICoord.fY + screenCoord.fY, m_Property.UICoord.fWidth, m_Property.UICoord.fHeight);

	CEtFontMng::GetInstance().DrawTextW( pElement->nFontIndex, 16, wszBuff, 0xffffffff, screenCoord, -1, Info );
#endif
}

void CEtUIStatic::SetShadowColor( DWORD dwColor, bool bSkipAlpha )
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL] = dwColor;
	// Hidden 상태 일수도 있으니 알파는 건드리지 말자. 
	if( bSkipAlpha ) {
		pElement->ShadowFontColor.dwCurrentColor = 
			EtColor( EtColor(dwColor).r, EtColor(dwColor).g, EtColor(dwColor).b, EtColor(pElement->ShadowFontColor.dwCurrentColor).a );
	}
	else {
		pElement->ShadowFontColor.dwCurrentColor = dwColor;
	}
}

void CEtUIStatic::SetTextColor( DWORD dwColor, bool bSkipAlpha )
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	pElement->FontColor.dwColor[UI_STATE_NORMAL] = dwColor;
	// Hidden 상태 일수도 있으니 알파는 건드리지 말자. 
	if( bSkipAlpha ) {
		pElement->FontColor.dwCurrentColor = 
			EtColor( EtColor(dwColor).r, EtColor(dwColor).g, EtColor(dwColor).b, EtColor(pElement->FontColor.dwCurrentColor).a );
	}
	else {
		pElement->FontColor.dwCurrentColor = dwColor;
	}
}

void CEtUIStatic::SetBgTextColor( DWORD dwColor , UI_CONTROL_STATE State /*= UI_STATE_NORMAL */)
{
	m_BgTextColor.dwCurrentColor = dwColor;
	m_BgTextColor.dwColor[ State ] = dwColor;
}

void CEtUIStatic::SetTextureColor( DWORD dwColor )
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	pElement->TextureColor.dwCurrentColor = dwColor;
}

DWORD CEtUIStatic::GetTextColor()
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return 0;

	return pElement->FontColor.dwColor[UI_STATE_NORMAL];
}

DWORD CEtUIStatic::GetShadowColor()
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return 0;

	return pElement->ShadowFontColor.dwColor[UI_STATE_NORMAL];
}

DWORD CEtUIStatic::GetCurTextureColor()
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return 0;

	return pElement->TextureColor.dwCurrentColor;
}

DWORD CEtUIStatic::GetTextureColor()
{
	SUIElement *pElement = GetElement(0);
	if( !pElement ) return 0;

	return pElement->TextureColor.dwColor[UI_STATE_NORMAL];
}

void CEtUIStatic::SetTextureColor( DWORD dwColor, int nElementIndex, UI_CONTROL_STATE emState )
{
	SUIElement *pElement = GetElement(nElementIndex);
	if( !pElement ) return;

	pElement->TextureColor.dwColor[emState] = dwColor;
}

void CEtUIStatic::SetBlink( bool bBlink )
{
	m_bBlink = bBlink;

	if( !m_bBlink )
		m_fBlinkTime = 0.0f;
}

bool CEtUIStatic::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDOWN:	
	case WM_RBUTTONDOWN:	
		{
			if( IsInside( fX, fY ) )
			{
				m_bPressed = true;
#ifdef FIX_STATICCONTROL_FOCUS_PROCESS
#else
				SetCapture( m_pParent->GetHWnd() );
				if( !m_bFocus )
				{
					m_pParent->RequestFocus( this );	
				}
				if( uMsg == WM_LBUTTONDBLCLK )
				{
					// 아래 ReleaseCapture를 호출하지 않으면,
					// Static컨트롤을 더블클릭한 후 EtDialog의 MsgProc로 메세지를 보내게 될때,
					// 이후 GetCapture부분에서 해당 핸들을 리턴하게 된다.
					//
					// 문제가 되는 대표적인 부분은 파티리스트, 아이템조합리스트, 문장보옥 리스트 등
					// 직접 더블클릭처리를 하고 있는 가짜 리스트 형태의 다이얼로그에서,
					// 더블클릭 처리 후 return true대신에 break를 호출해 return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
					// 가 호출될때였다.
					//
					// 파티리스트에서 특정 파티를 더블클릭해서 파티가입하고나서 알트탭이나, 아무 UI를 누르기 전까진,
					// 모든 UI에 마우스오버효과(UI컨트롤에 대면 나오는 것들)가 나오지 않은 것.
					// 어쨌든 이런 식의 강제 ReleaseCapture함수 호출로 위 문제는 해결되었다.
					//
					// 두번째 해결책은 break대신에 return true를 호출하는 것이다.
					ReleaseCapture();
				}
#endif
				return true;
			}
		}
		break;

	case WM_LBUTTONUP:
		if( m_bButton && m_bPressed && IsInside( fX, fY )) {
			if( uMsg == WM_LBUTTONUP ) {
				if( wParam & MK_SHIFT ) {
					uMsg |= 0x0010;
				}
			}
			m_pParent->ProcessCommand( EVENT_BUTTON_CLICKED, true, this, uMsg );
		}
	case WM_RBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
#ifdef FIX_STATICCONTROL_FOCUS_PROCESS
#else
				ReleaseCapture();
#endif
				return true;
			}
		}
		break;
	}

	return false;
}

void CEtUIStatic::UpdateRects()
{
	if( GetElementCount() == 0 )
		return;

	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	if( (m_Property.StaticProperty.nFrameLeft > 0) || (m_Property.StaticProperty.nFrameTop > 0) 
		|| (m_Property.StaticProperty.nFrameRight > 0) || (m_Property.StaticProperty.nFrameBottom > 0) )
	{
		float fFrameLeft(0.0f), fFrameTop(0.0f), fFrameRight(0.0f), fFrameBottom(0.0f);
		float fModWidth(0.0f), fModHeight(0.0f);

		EtTextureHandle hTexture = m_pParent->GetUITexture();
		if( m_bExistTemplateTexture )
			hTexture = m_Template.m_hTemplateTexture;
		if( hTexture )
		{
			int nTextureWidth = hTexture->Width();
			int nTextureHeight = hTexture->Height();

			fFrameLeft = m_Property.StaticProperty.nFrameLeft / ( float )nTextureWidth;
			fFrameTop = m_Property.StaticProperty.nFrameTop / ( float )nTextureHeight;
			fFrameRight = m_Property.StaticProperty.nFrameRight / ( float )nTextureWidth;
			fFrameBottom = m_Property.StaticProperty.nFrameBottom / ( float )nTextureHeight;

			if( pElement->fTextureWidth > 0.0f && pElement->fTextureHeight > 0.0f )
			{
				fModWidth = ((pElement->fTextureWidth * DEFAULT_UI_SCREEN_WIDTH) / nTextureWidth) - (fFrameLeft+fFrameRight);
				fModHeight = ((pElement->fTextureHeight * DEFAULT_UI_SCREEN_HEIGHT) / nTextureHeight) - (fFrameTop+fFrameBottom);
			}
			else
			{
				fModWidth = (m_Property.UICoord.fWidth / nTextureWidth) - (fFrameLeft+fFrameRight);
				fModHeight = (m_Property.UICoord.fHeight / nTextureHeight) - (fFrameTop+fFrameBottom);
			}

			SUICoord UVCoord = pElement->UVCoord;
			if( m_bExistTemplateTexture )
				UVCoord = pElement->TemplateUVCoord;

			m_BaseUVCoord[0].SetCoord( UVCoord.fX,						UVCoord.fY,							fFrameLeft,		fFrameTop );
			m_BaseUVCoord[1].SetCoord( UVCoord.fX+fFrameLeft,			UVCoord.fY,							fModWidth,		fFrameTop );
			m_BaseUVCoord[2].SetCoord( UVCoord.fX+fFrameLeft+fModWidth,	UVCoord.fY,							fFrameRight,	fFrameTop );
			m_BaseUVCoord[3].SetCoord( UVCoord.fX,						UVCoord.fY+fFrameTop,				fFrameLeft,		fModHeight );
			m_BaseUVCoord[4].SetCoord( UVCoord.fX+fFrameLeft,			UVCoord.fY+fFrameTop,				fModWidth,		fModHeight );
			m_BaseUVCoord[5].SetCoord( UVCoord.fX+fFrameLeft+fModWidth,	UVCoord.fY+fFrameTop,				fFrameRight,	fModHeight );
			m_BaseUVCoord[6].SetCoord( UVCoord.fX,						UVCoord.fY+fFrameTop+fModHeight,	fFrameLeft,		fFrameBottom );
			m_BaseUVCoord[7].SetCoord( UVCoord.fX+fFrameLeft,			UVCoord.fY+fFrameTop+fModHeight,	fModWidth,		fFrameBottom );
			m_BaseUVCoord[8].SetCoord( UVCoord.fX+fFrameLeft+fModWidth,	UVCoord.fY+fFrameTop+fModHeight,	fFrameRight,	fFrameBottom );
		}

		fFrameLeft = m_Property.StaticProperty.nFrameLeft / (float)DEFAULT_UI_SCREEN_WIDTH;
		fFrameTop = m_Property.StaticProperty.nFrameTop / (float)DEFAULT_UI_SCREEN_HEIGHT;
		fFrameRight = m_Property.StaticProperty.nFrameRight / (float)DEFAULT_UI_SCREEN_WIDTH;
		fFrameBottom = m_Property.StaticProperty.nFrameBottom / (float)DEFAULT_UI_SCREEN_HEIGHT;

		fModWidth = m_Property.UICoord.fWidth - (fFrameLeft+fFrameRight);
		fModHeight = m_Property.UICoord.fHeight - (fFrameTop+fFrameBottom);

		m_BaseCoord[0].SetCoord( m_Property.UICoord.fX,							m_Property.UICoord.fY,						fFrameLeft,		fFrameTop );
		m_BaseCoord[1].SetCoord( m_Property.UICoord.fX+fFrameLeft,				m_Property.UICoord.fY,						fModWidth,		fFrameTop );
		m_BaseCoord[2].SetCoord( m_Property.UICoord.fX+fFrameLeft+fModWidth,	m_Property.UICoord.fY,						fFrameRight,	fFrameTop );
		m_BaseCoord[3].SetCoord( m_Property.UICoord.fX,							m_Property.UICoord.fY+fFrameTop,			fFrameLeft,		fModHeight );
		m_BaseCoord[4].SetCoord( m_Property.UICoord.fX+fFrameLeft,				m_Property.UICoord.fY+fFrameTop,			fModWidth,		fModHeight );
		m_BaseCoord[5].SetCoord( m_Property.UICoord.fX+fFrameLeft+fModWidth,	m_Property.UICoord.fY+fFrameTop,			fFrameRight,	fModHeight );
		m_BaseCoord[6].SetCoord( m_Property.UICoord.fX,							m_Property.UICoord.fY+fFrameTop+fModHeight,	fFrameLeft,		fFrameBottom );
		m_BaseCoord[7].SetCoord( m_Property.UICoord.fX+fFrameLeft,				m_Property.UICoord.fY+fFrameTop+fModHeight,	fModWidth,		fFrameBottom );
		m_BaseCoord[8].SetCoord( m_Property.UICoord.fX+fFrameLeft+fModWidth,	m_Property.UICoord.fY+fFrameTop+fModHeight,	fFrameRight,	fFrameBottom );
	}

	m_FontCoord = m_Property.UICoord;
}

bool CEtUIStatic::SetTextWithEllipsis( const wchar_t *wszText, const wchar_t *wszSymbol )
{
	ASSERT( wszText&&wszSymbol);

	m_szText = wszText;
	m_strRenderText = m_szText;

	SUIElement *pElement = GetElement(0);
	if( !pElement ) return false;

	SUICoord sSymbolCoord;
	m_pParent->CalcTextRect( wszSymbol, pElement, sSymbolCoord );
	
	SUICoord sTextCoord;
	m_pParent->CalcTextRect( m_szText.c_str(), pElement, sTextCoord );

	if( sTextCoord.fWidth > m_Property.UICoord.fWidth )
	{
		float fRemainWidth = m_Property.UICoord.fWidth - sSymbolCoord.fWidth;
		int nStrLen = (int)m_szText.length();
		float fTextCoordHeight = sTextCoord.fHeight;
		int i(1), nStartPos(0);

		for( ; (nStartPos+i) <= nStrLen; i++)
		{
			SUICoord sSubTextCoord;
			std::wstring subStr = m_szText.substr(nStartPos,i);
			m_pParent->CalcTextRect( subStr.c_str(), pElement, sSubTextCoord );

			if( sTextCoord.fHeight < (m_Property.UICoord.fHeight - fTextCoordHeight) )
			{
				// 텍스트가 다음줄에 계속 찍힌다면...
				if( sSubTextCoord.fWidth > m_Property.UICoord.fWidth )
				{
					nStartPos += i-1;
					i = 0;
					fTextCoordHeight += sSubTextCoord.fHeight;
				}
			}
			else
			{
				// 마지막 라인을 검사한다.
				if( sSubTextCoord.fWidth > fRemainWidth )
				{
					i--;

					m_strRenderText = m_szText.substr(0,nStartPos+i);
					m_strRenderText += wszSymbol;
					return true;
				}
			}
		}
	}

	return false;
}

void CEtUIStatic::SetText( const std::wstring &strText , bool bFit, float fScaleX, float fScaleY )
{
	if( m_Property.StaticProperty.bAutoTooltip && bFit == false ) {
		SetTooltipText( L"" );
		if( SetTextWithEllipsis( strText.c_str(), L"..." ) )
			SetTooltipText( strText.c_str() );
		return;
	}

	CEtUIControl::SetText(strText, bFit);
	if( bFit && GetElement(0) ) {
		SUICoord Coord;
		if( !strText.empty() ) {
			int nFontHeight = GetElement(0)->nFontHeight;
			int nFontIndex = GetElement(0)->nFontIndex;			
			DWORD dwFontFormat = GetElement(0)->dwFontFormat;
			CEtFontMng::GetInstance().CalcTextRect( nFontIndex, nFontHeight, strText.c_str(), dwFontFormat, Coord, -1 );
		}
		float fFrameWidth = (m_Property.StaticProperty.nFrameLeft + m_Property.StaticProperty.nFrameRight) / (float)DEFAULT_UI_SCREEN_WIDTH;
		float fFrameHeight = (m_Property.StaticProperty.nFrameTop + m_Property.StaticProperty.nFrameBottom) / (float)DEFAULT_UI_SCREEN_HEIGHT;
		
		SetSize( (Coord.fWidth + fFrameWidth ) * fScaleX,
					(Coord.fHeight + fFrameHeight ) * fScaleY );	

		SUICoord ParentCoord;
		m_pParent->GetDlgCoord(ParentCoord);
		SetPosition( (ParentCoord.fWidth - m_Property.UICoord.fWidth) * 0.5f , m_Property.UICoord.fY );

		m_FontCoord.fY -= 2.0f / DEFAULT_UI_SCREEN_HEIGHT;		
	}
}

const wchar_t* CEtUIStatic::GetPropertyString()
{
	const wchar_t *pString = L"";
	if( m_Property.StaticProperty.nStringIndex != -1 ) {
		pString = m_pParent->GetUIString( CEtUIXML::idCategory1, m_Property.StaticProperty.nStringIndex );
	}
	return pString;
}

#ifdef PRE_UISTATIC_PARTTEXT_COLOR
void CEtUIStatic::SetPartColorText(const std::wstring& colorText, DWORD dwTextColor, DWORD dwBGColor)
{

#ifdef PRE_MOD_UISTATIC_PARTTEXT_COLOR
	if( ! IsExistString( m_szText, colorText) )
		return;
#else
	size_t found, prevFound = 0;
	found = m_szText.find(colorText);
	if (found == std::wstring::npos)
		return;
#endif 

#ifdef PRE_MOD_UISTATIC_PARTTEXT_COLOR
	int nOffset = 0;
	std::string::size_type nMaxPos = m_strRenderText.find_first_of('.', nOffset );
	if( nMaxPos != std::wstring::npos )
		m_szText = m_strRenderText;
#endif


	SUIElement* pElement = GetElement(0);
	if (!pElement)
		return;

	float fAllignOffset = 0.f;
	DWORD dwStaticFormat = m_Property.StaticProperty.dwFontFormat;

	if (CEtFontMng::GetInstance().GetFontSet(pElement->nFontIndex) == NULL) 
		return;

	D3DXCOLOR textureColor( pElement->FontColor.dwCurrentColor );
	D3DXCOLOR shadowColor( pElement->ShadowFontColor.dwCurrentColor );

	if( textureColor.a != 1.0f )
		shadowColor.a = shadowColor.a * textureColor.a;

	SFontDrawEffectInfo Info;
	Info.nDrawType = pElement->nDrawType;
	Info.nWeight = pElement->nWeight;
	Info.fAlphaWeight = pElement->fAlphaWeight;
	Info.fGlobalBlurAlphaWeight = pElement->fGlobalBlurAlphaWeight;
	Info.dwFontColor = pElement->FontColor.dwCurrentColor;
	Info.dwEffectColor = shadowColor;

	SUICoord textCoord;
	m_pParent->CalcTextRect( m_szText.c_str() , pElement , textCoord );
	float fFullSizeX = m_Property.UICoord.fWidth - textCoord.fWidth;
	float fHalfSizeX = (fFullSizeX * 0.5f);


	if (dwStaticFormat & DT_CENTER)
	{
		fAllignOffset += fHalfSizeX;
		dwStaticFormat &= (~DT_CENTER);
	}
	else if (dwStaticFormat & DT_RIGHT)
	{
		fAllignOffset += fFullSizeX;
		dwStaticFormat &= (~DT_RIGHT);
	}

	if (m_PartTextColorInfoList.empty() == false)
	{
		std::list<SPartTextInfo>::iterator iter = m_PartTextColorInfoList.begin();
		for (; iter != m_PartTextColorInfoList.end();)
		{
			SPartTextInfo& curInfo = (*iter);

			size_t found, prevFound = 0;
#ifdef PRE_MOD_UISTATIC_PARTTEXT_COLOR
			std::wstring wszCopareStr = curInfo.text;
			ToLowerW(wszCopareStr);
			found = wszCopareStr.find(colorText);
#else
			found = curInfo.text.find(colorText);
#endif	
			if (found == std::wstring::npos || 
				(curInfo.dwBGColor == dwBGColor && curInfo.dwTextColor == dwTextColor))
			{
				++iter;
				continue;
			}

			SPartTextInfo partInfo;
			partInfo.dwTextColor = dwTextColor;
			partInfo.dwBGColor = dwBGColor;
			partInfo.dwFontFormat = dwStaticFormat;
			partInfo.fPartWidthOffset = curInfo.fPartWidthOffset;
			MakePartColorText(iter, curInfo.text, colorText, pElement, partInfo);

			iter = m_PartTextColorInfoList.erase(iter);
		}
	}
	else
	{
		SPartTextInfo partInfo;
		partInfo.dwTextColor = dwTextColor;
		partInfo.dwBGColor = dwBGColor;
		partInfo.dwFontFormat = dwStaticFormat;
		partInfo.fPartWidthOffset = fAllignOffset;
		MakePartColorText(m_PartTextColorInfoList.end(), m_szText, colorText, pElement,partInfo);
	}
}

void CEtUIStatic::MakePartColorText(std::list<SPartTextInfo>::iterator& iterToInsert, const std::wstring& orgText, const std::wstring colorText, SUIElement* pElement, const SPartTextInfo& partTextInfo)
{
	size_t found, prevFound = 0;
#ifdef PRE_MOD_UISTATIC_PARTTEXT_COLOR
	if( ! IsExistString( orgText, colorText) )
		return;

	std::wstring wszfoundCheck = orgText;
	ToLowerW(wszfoundCheck);
	found = wszfoundCheck.find(colorText);
#else
	found = orgText.find(colorText);
	if (found == std::wstring::npos)
		return;
#endif

	SPartTextInfo info;
	SUICoord temp;
	float fPrevWidthSum = partTextInfo.fPartWidthOffset;
	while(found != std::wstring::npos)
	{
		std::wstring prevFoundText;
		if (found > 0)
		{
			prevFoundText = orgText.substr(prevFound, found - prevFound);

			info.text = prevFoundText;
			info.dwTextColor = pElement->FontColor.dwCurrentColor;
			info.dwBGColor = m_BgTextColor.dwCurrentColor;
			info.fPartWidthOffset = fPrevWidthSum;
			m_pParent->CalcTextRect(prevFoundText.c_str(), pElement, temp);
			info.fWidth = temp.fWidth;
			info.dwFontFormat = partTextInfo.dwFontFormat;
			fPrevWidthSum += info.fWidth;

			m_PartTextColorInfoList.insert(iterToInsert, info);
		}

		std::wstring foundText = orgText.substr(found, colorText.size());

		info.text = foundText;
		info.dwBGColor = partTextInfo.dwBGColor;
		info.dwTextColor = partTextInfo.dwTextColor;
		info.fPartWidthOffset = fPrevWidthSum;
		m_pParent->CalcTextRect(foundText.c_str(), pElement, temp);
		info.fWidth = temp.fWidth;
		info.dwFontFormat = partTextInfo.dwFontFormat;
		fPrevWidthSum += info.fWidth;

		m_PartTextColorInfoList.insert(iterToInsert, info);

		prevFound = found + colorText.size();
		found = orgText.find(colorText, found + 1);
	}

	if (orgText.size() > prevFound)
	{
		info.text = orgText.substr(prevFound, std::wstring::npos);
		info.dwTextColor = pElement->FontColor.dwCurrentColor;
		info.dwBGColor = m_BgTextColor.dwCurrentColor;
		info.fPartWidthOffset = fPrevWidthSum;
		m_pParent->CalcTextRect(info.text.c_str(), pElement, temp);
		info.fWidth = temp.fWidth;
		info.dwFontFormat = partTextInfo.dwFontFormat;

		m_PartTextColorInfoList.insert(iterToInsert, info);
	}
}

void CEtUIStatic::ClearPartColorText()
{
	m_PartTextColorInfoList.clear();
}

void CEtUIStatic::ClearText()
{
	m_szText.clear();
	m_strRenderText.clear();
	if (m_Property.StaticProperty.bAutoTooltip)
		ClearTooltipText();
	ClearPartColorText();
}
#endif // PRE_UISTATIC_PARTTEXT_COLOR
