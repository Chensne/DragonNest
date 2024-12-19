#include "StdAfx.h"
#include "DnNpcAnswerHtmlTextBox.h"
#include "DnTableDB.h"
#ifdef NPC_ANSWER_KEYBOARD_SELECT
#include "InputWrapper.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnNpcAnswerHtmlTextBox::CDnNpcAnswerHtmlTextBox(CEtUIDialog *pParent)
: CEtUIHtmlTextBox(pParent)
, m_nAnswerIndex(0)
, m_nSelectionSoundIndex(-1)
#ifdef NPC_ANSWER_ARROW
, m_nArrowLineIndex(0)
, m_fOffsetTime(0.0f)
#endif
#ifdef NPC_ANSWER_KEYBOARD_SELECT
, m_nKeyboardSelectLineIndex(0)
#endif
{
	// 이렇게 가변 타입주면 npc다이얼로그가 선택지 개수에 따라 커지거나 작아지지만, 예외처리 등이 하나도 안되있어서 이상해진다.
	// 하려면 툴팁과는 달리 별도처리해야할 듯.
	//m_Property.TextBoxProperty.VariableType = UI_TEXTBOX_HEIGHT;
}

CDnNpcAnswerHtmlTextBox::~CDnNpcAnswerHtmlTextBox(void)
{
	if( m_nSelectionSoundIndex == -1 ) 
	{
		CEtSoundEngine::GetInstance().RemoveSound( m_nSelectionSoundIndex );
	}
}

void CDnNpcAnswerHtmlTextBox::Initialize( SUIControlProperty *pProperty )
{
	CEtUIHtmlTextBox::Initialize( pProperty );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10002 );
	if( strlen( szFileName ) > 0 )
	{
		m_nSelectionSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}
}

bool CDnNpcAnswerHtmlTextBox::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	if( ( WM_LBUTTONDOWN == uMsg ) && ( !m_bFocus ) )
	{
		m_pParent->RequestFocus( this );
	}

	int nOldPosition = m_ScrollBar.GetTrackPos();

	if( m_ScrollBar.HandleMouse( uMsg, fX, fY, wParam, lParam ) )
	{
		int nCurPosition = m_ScrollBar.GetTrackPos();
		Scroll( nCurPosition-nOldPosition );
		return true;
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
		}
		break;
	case WM_LBUTTONUP:
		{
			if( m_bPressed )
			{
				m_bPressed = false;
				ReleaseCapture();

				if( IsInside( fX, fY ) )
				{
					UINT uMsgInner(WM_LBUTTONUP);
					if( wParam & MK_CONTROL )
					{
						uMsgInner |= 0x0100;
					}

					if( m_Property.TextBoxProperty.bRollOver )
					{
						if( SelectLine( fX, fY ) )
						{
							m_pParent->ProcessCommand( EVENT_TEXTBOX_SELECTION, true, this, uMsgInner );

							if( m_nSelectionSoundIndex != -1 )
							{
								CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSelectionSoundIndex );
							}
						}
					}
				}

				return true;
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( IsInside( fX, fY ) )
			{
				if( m_Property.TextBoxProperty.bRollOver )
				{
					SetMouseInLine( fX, fY );
				}
			}
		}
		break;
	case WM_MOUSEWHEEL:
		{
			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
			int nScrollPos = Scroll( -nScrollAmount );
			m_ScrollBar.ShowItem( nScrollPos );
			return true;
		}
		break;
	}

	return false;
}

#ifdef NPC_ANSWER_KEYBOARD_SELECT
bool CDnNpcAnswerHtmlTextBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{	
		return false;
	}

	if( !m_Property.TextBoxProperty.bVerticalScrollBar )
	{
		return false;
	}

	if( uMsg == WM_KEYDOWN )
	{
		if( m_nCurLine < 0 )
			return false;

		int nVecSize = (int)m_vecLine.size();
		SLineData LineData;

		bool bSelectMoved = false;
		if( GetKeyState(_ToVK(IW(IW_MOVEFRONT))) & 0x80 || GetKeyState(VK_UP) & 0x80 )
		{
			if( nVecSize == 0 )
				return false;

			// 바로 위 선택지로 키보드셀렉트 이동
			int nPrevLineIndex = m_vecLine[0]->GetLineData().m_nData;
			for( int i=0; i<nVecSize; i++ )
			{
				LineData = m_vecLine[i]->GetLineData();
				if( LineData.m_nData == m_nKeyboardSelectLineIndex )
					break;

				nPrevLineIndex = LineData.m_nData;
			}
			if( m_nKeyboardSelectLineIndex != nPrevLineIndex )
			{
				m_nKeyboardSelectLineIndex = nPrevLineIndex;
				bSelectMoved = true;
			}
		}
		else if( GetKeyState(_ToVK(IW(IW_MOVEBACK))) & 0x80 || GetKeyState(VK_DOWN) & 0x80 )
		{
			// 바로 아래 선택지로 키보드셀렉트 이동
			bool bSelectNextLineIndex = false;
			for( int i=0; i<nVecSize; i++ )
			{
				LineData = m_vecLine[i]->GetLineData();

				if( bSelectNextLineIndex && LineData.m_nData != m_nKeyboardSelectLineIndex )
				{
					m_nKeyboardSelectLineIndex = LineData.m_nData;
					bSelectMoved = true;
					break;
				}

				if( LineData.m_nData == m_nKeyboardSelectLineIndex )
					bSelectNextLineIndex = true;
			}
		}
		else if( GetKeyState(_ToVK(IW(IW_JUMP))) & 0x80 || GetKeyState(VK_SPACE) & 0x80)
		{
			int nCount(0);
			for( int i=m_nCurLine; i<nVecSize; i++, nCount++ )
			{
				if( nCount >= m_nVisibleCount )
					break;

				LineData = m_vecLine[i]->GetLineData();
				if( LineData.m_nData == m_nKeyboardSelectLineIndex )
				{
					if( (int)m_vecLine[i]->m_vecWord.size() > 0 )
					{
						m_vecLine[i]->Select(true);
						m_vecLine[i]->SetMouseInLine(false);

						UINT uMsgInner( WM_KEYDOWN );
						m_pParent->ProcessCommand( EVENT_TEXTBOX_SELECTION, true, this, uMsgInner );

						if( m_nSelectionSoundIndex != -1 )
						{
							CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSelectionSoundIndex );
						}
						break;
					}
				}
			}
		}

		if( bSelectMoved )
		{
			// 여러 라인일수도 있으니 ArrowLineIndex 새로 구한다.
			for( int i=0; i<nVecSize; i++ )
			{
				LineData = m_vecLine[i]->GetLineData();
				if( LineData.m_nData == m_nKeyboardSelectLineIndex )
				{
					m_nArrowLineIndex = i;
					break;
				}
			}

			// 스크롤이 필요할때다.
			if( m_nArrowLineIndex < m_nCurLine )
			{
				int nScroll = (m_nCurLine - m_nArrowLineIndex + 1) / 2;	// +1 하는건 좀 더 자연스럽게 하기 위한 매직값
				for( int i = 0; i < nScroll; ++i )
					ScrollLineUp();
			}
			else if( m_nCurLine + m_nVisibleCount - 1 < m_nArrowLineIndex + 1 )	// 마지막 줄에서 걸리지 않고, 그 윗줄까지 보이게.
			{
				int nScroll = (m_nArrowLineIndex + 1 - (m_nCurLine + m_nVisibleCount - 1) + 1) / 2;
				for( int i = 0; i < nScroll; ++i )
					ScrollLineDown();
			}
		}
	}

	// 상위 클래스의 CEtUITextBox::HandleKeyboard 호출하지 않는다. 여기서 완전 별도로 처리.
	return false;
}
#endif

void CDnNpcAnswerHtmlTextBox::BeginParse(DWORD dwAppData, bool &bAbort)
{
	CEtUIHtmlTextBox::BeginParse(dwAppData, bAbort);
	m_nAnswerIndex = 0;

	// 미리 해두는 이유는 제일 처음에 혹시 BR(개행)이 나와서 라인데이터를 설정해야할지도 모르기때문.
	SLineData LineData;
	LineData.m_nData = m_nAnswerIndex;
	SetLineData(LineData);

#ifdef NPC_ANSWER_ARROW
	// 처음 파싱할때 Arrow도 초기화
	m_nArrowLineIndex = 0;
#endif
#ifdef NPC_ANSWER_KEYBOARD_SELECT
	m_nKeyboardSelectLineIndex = 0;
#endif
}

void CDnNpcAnswerHtmlTextBox::StartTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	if( pTag )
	{
		static CStringW strStartTag;
		strStartTag = pTag->getTagName();

		if( strStartTag.CompareNoCase(L"font") == 0 )
		{
			CLiteHTMLAttributes *pAtt = const_cast<CLiteHTMLAttributes*>(pTag->getAttributes());
			if( pAtt )
			{
				for( int i=0; i<pAtt->getCount(); i++ )
				{
					CLiteHTMLElemAttr &pElemAttr = pAtt->getAttribute(i);
					if( pElemAttr.isColorValue() )
					{
						// 원인은 모르겠지만 html 컬러와 r 과 b 가 반대인거 같다..
						COLORREF clr = pElemAttr.getColorValue();
						BYTE b = GetRValue(clr);
						BYTE g = GetGValue(clr);
						BYTE r = GetBValue(clr);

						m_dwTextColor = D3DCOLOR_XRGB(r,g,b);
						break;
					}
				}
			}
		}
		else if( strStartTag.CompareNoCase(L"br") == 0 )
		{
			++m_nAnswerIndex;
			SLineData LineData;
			LineData.m_nData = m_nAnswerIndex;
			SetLineData(LineData);

			AddLine();
		}
		else if( strStartTag.CompareNoCase(L"p") == 0 )
		{
			// p는 한번도 본적이 없는데.. 어쨌든 개행이니 처리해두겠다.
			++m_nAnswerIndex;
			SLineData LineData;
			LineData.m_nData = m_nAnswerIndex;
			SetLineData(LineData);

			AddLine();
		}
		else if (strStartTag.CompareNoCase(L"div") == 0)
		{
			CLiteHTMLAttributes *pAtt = const_cast<CLiteHTMLAttributes*>(pTag->getAttributes());
			if( pAtt )
			{
				for (int i = 0; i < pAtt->getCount(); ++i)
				{
					//	todo : vertical center align
					CLiteHTMLElemAttr &pElemAttr = pAtt->getAttribute(i);
					if (pElemAttr.getValue().CompareNoCase(L"center") == 0)
						m_Property.TextBoxProperty.AllignHori = AT_HORI_CENTER;
				}
			}
		}
	}

	bAbort = false;
}

void CDnNpcAnswerHtmlTextBox::EndTag(CLiteHTMLTag *pTag, DWORD dwAppData, bool &bAbort)
{
	if( pTag )
	{
		static CStringW strEndTag;
		strEndTag = pTag->getTagName();

		if( strEndTag.CompareNoCase(L"font") == 0 )
		{
			//m_dwTextColor = textcolor::NPC_SPEECH;
			m_dwTextColor = m_dwDefaultTextColor;
		}
		else if( strEndTag.CompareNoCase(L"p") == 0 )
		{
			// p는 한번도 본적이 없는데.. 어쨌든 개행이니 처리해두겠다.
			++m_nAnswerIndex;
			SLineData LineData;
			LineData.m_nData = m_nAnswerIndex;
			SetLineData(LineData);

			AddLine();
		}
		else if (strEndTag.CompareNoCase(L"div") == 0)
		{
			m_Property.TextBoxProperty.AllignHori = AT_HORI_NONE;
		}
	}

	bAbort = false;
}

void CDnNpcAnswerHtmlTextBox::Characters(const CStringW &rText, DWORD dwAppData, bool &bAbort)
{
	// 라인 중간에 Font같은 구문이 있을 경우 뒤에 있는 나머지 글자들과 별도로 AppendText되기때문에 라인데이터가 풀려있을 수 있다.
	// 예를 들어 아래와 같이 되있을때
	// <HTML>[이벤트] <FONT color=#086171>비스킷과 함께 똑똑해져요!</FONT><BR>[이벤트] <FONT color=#086171>아이린의 예절 교육</FONT>[진행중]<BR>(처음으로)</HTML>
	// [이벤트] 
	// 비스킷과 함께 똑똑해져요!(폰트색 바뀌어서)
	// 이렇게 두번의 AppendText가 호출되고, 첫번째 AppendText될때 ClearLineData가 호출되니 다시 라인데이터를 셋해주는 것이다.
	SLineData LineData;
	LineData.m_nData = m_nAnswerIndex;
	SetLineData(LineData);

	// Npc대사는 여기로 들어온다.
	// 보통 이런 형태로 들어오기때문에([퀘스트]<BR>[상점거래])
	// 들어올때마다 인덱스를 증가시켜 라인데이터로 사용하면 된다.
	//
	// 그런데 이런 데이터도 있다.
	// ([Lv2] <Font 태크...> 하운드의 영양식 <BR> [Lv3] 생략..)
	// 즉, Lv2와 하운드의 영양식이 따로 올 수도 있기때문에, 꼭 BR(개행)에서만 라인데이터 증가시켜 구분해야한다.
	CEtUIHtmlTextBox::Characters(rText, dwAppData, bAbort);
}

void CDnNpcAnswerHtmlTextBox::EndParse(DWORD dwAppData, bool bIsAborted)
{
	CEtUIHtmlTextBox::EndParse(dwAppData, bIsAborted);
	m_nAnswerIndex = 0;
	ClearLineData();
}

void CDnNpcAnswerHtmlTextBox::RenderText( float fElapsedTime )
{
	if( m_nCurLine == -1 )
		return;

	SUICoord sLineCoord;
	int nCount(0);
	int nVecSize = (int)m_vecLine.size();

	// 롤오버된 라인의 라인데이터를 미리 구해놓고,
	int nSelectedLineData = -1;

	SLineData LineData;
	for( int i=m_nCurLine; i<nVecSize; i++, nCount++ )
	{
		if( nCount >= m_nVisibleCount )
			break;

		if( m_Property.TextBoxProperty.bRollOver )
		{
			if( IsMouseEnter() && m_vecLine[i]->IsMouseInLine() )
			{
				LineData = m_vecLine[i]->GetLineData();
				nSelectedLineData = LineData.m_nData;
#ifdef NPC_ANSWER_KEYBOARD_SELECT
				m_nKeyboardSelectLineIndex = nSelectedLineData;
#endif
			}
		}
	}

	nCount = 0;
	// 동일한 라인데이터를 가진 라인들은 셀렉트 형태로 그린다.
	for( int i=m_nCurLine; i<nVecSize; i++, nCount++ )
	{
		if( nCount >= m_nVisibleCount )
			break;

		if( m_Property.TextBoxProperty.bRollOver )
		{
			LineData = m_vecLine[i]->GetLineData();
#ifdef NPC_ANSWER_KEYBOARD_SELECT
			if( LineData.m_nData == m_nKeyboardSelectLineIndex )
#else
			if( LineData.m_nData == nSelectedLineData )
#endif
			{
				RenderLine( fElapsedTime, m_vecLine[i], true );
				continue;
			}
		}
		RenderLine( fElapsedTime, m_vecLine[i] );
		//m_pParent->DrawRect( m_vecLine[i].m_uiCoord, EtInterface::debug::BLUE );
	}

#ifdef NPC_ANSWER_ARROW
	// 화살표가 위치할 라인을 구해둔다.
	if( nSelectedLineData != -1 )
	{
		// 위와 달리 m_CurLine부터 시작하지 않고 0으로 시작한다.
		// 이렇게 해야 두 줄 이상 답변의 경우 맨 윗줄이 스크롤되서 안보일때 화살표가 보이지 않게 된다.
		for( int i=0; i<nVecSize; i++ )
		{
			LineData = m_vecLine[i]->GetLineData();
			if( LineData.m_nData == nSelectedLineData )
			{
				m_nArrowLineIndex = i;
				break;
			}
		}
	}
#endif
}

#ifdef NPC_ANSWER_ARROW
void CDnNpcAnswerHtmlTextBox::Render( float fElapsedTime )
{
	CEtUIHtmlTextBox::Render( fElapsedTime );

	// 화살표 렌더링
	SUIElement *pArrowElement = GetElement(3);
	if( m_nCurLine != -1 && pArrowElement ) {
		SUICoord sCoord;
		int nCount(0);
		int nVecSize = (int)m_vecLine.size();
		for( int i=m_nCurLine; i<nVecSize; i++, nCount++ )
		{
			if( nCount >= m_nVisibleCount ) {
				// 안그려질땐
				m_fOffsetTime = 0.0f;
				break;
			}

			if( i == m_nArrowLineIndex )
			{
				// 그려질땐
				m_fOffsetTime += fElapsedTime * 1000;

				int nOffsetX = 0;
				float OFFSET_VALUE = 4.0f;	// 얼마나 이동하는지.
				int MOVE_TIME = 300;		// 한번 좌에서 우로 이동하는시간.
				float BLEND_RANGE = 1.0f;
				int nTemp = (int)m_fOffsetTime%(MOVE_TIME+MOVE_TIME);
				if( nTemp < MOVE_TIME )
					nOffsetX = int(( BLEND_RANGE / (float)MOVE_TIME ) * nTemp * 255.0f);
				else
					nOffsetX = int(( BLEND_RANGE / (float)MOVE_TIME ) * ( (MOVE_TIME+MOVE_TIME) - nTemp ) * 255.0f);
				nOffsetX = min(nOffsetX, 255);

				// 0~255의 값을 -OFFSET_VALUE ~ +OFFSET_VALUE 범위로 변경.
				float fOffsetResult = ((nOffsetX / 255.0f) - 0.5f) * OFFSET_VALUE / DEFAULT_UI_SCREEN_WIDTH;
				sCoord = m_vecLine[i]->m_uiCoord;
				sCoord.fX = sCoord.fX - 11.0f / DEFAULT_UI_SCREEN_WIDTH + fOffsetResult;
				sCoord.fY = sCoord.fY + (sCoord.fHeight-pArrowElement->fTextureHeight)/2.0f + 4.0f / DEFAULT_UI_SCREEN_HEIGHT;
				sCoord.fWidth = pArrowElement->fTextureWidth;
				sCoord.fHeight = pArrowElement->fTextureHeight;
				if( m_bExistTemplateTexture )
					m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pArrowElement->TemplateUVCoord, 0xffffffff, sCoord );
				else
					m_pParent->DrawSprite( pArrowElement->UVCoord, 0xffffffff, sCoord );
				break;
			}
		}
	}
}

void CDnNpcAnswerHtmlTextBox::RenderWord( float fElapsedTime, CWord& sWord, bool bRollOver, int nBorderFlag )
{
	std::wstring strRenderString;
	SUICoord uiRenderCoord;

	strRenderString = sWord.m_strWord;
	uiRenderCoord = sWord.m_sProperty.uiCoord;

	// 제대로 한다면 텍스트박스에 AddText할때부터 라인에 들어갈 텍스트영역에서 화살표 가로크기만큼 빼고 계산해야하지만,
	// 이러면 AddText부터 UpdateRect까지 전부다 새로 만들어야한다.
	// 이렇게 할바엔 어차피 npc대화 특성 상 우측끝이 명확하지 않게 보이기때문에,
	// 텍스트만 우측으로 밀어서 렌더링하도록 하겠다.
	SUIElement *pArrowElement = GetElement(3);
	if( pArrowElement )
	{
		uiRenderCoord.fX += pArrowElement->fTextureWidth * 0.66f;
		uiRenderCoord.fWidth += pArrowElement->fTextureWidth * 0.66f;
	}

	if( sWord.m_sProperty.bSymbol )
	{
		strRenderString += L"...";
		uiRenderCoord.fWidth += m_fSymbolWidth;
	}

	UI_CONTROL_STATE currentState = UI_STATE_NORMAL;

	if( !IsShow() )
	{
		currentState = UI_STATE_HIDDEN;
	}
	else if( !IsEnable() )
	{
		currentState = UI_STATE_DISABLED;
	}

	sWord.m_sProperty.uiColor.Blend( currentState, fElapsedTime, m_fBlendRate );

	SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	if( bRollOver )
	{
		//m_pParent->DrawDlgText( strRenderString.c_str(), pElement, textcolor::ORANGE, uiRenderCoord, -1, -1, true );
		m_pParent->DrawDlgText( strRenderString.c_str(), pElement, sWord.m_sProperty.uiColor.dwCurrentColor, uiRenderCoord, -1, -1, true, 0.f, sWord.m_sProperty.BgColor, nBorderFlag );
	}
	else
	{
		m_pParent->DrawDlgText( strRenderString.c_str(), pElement, sWord.m_sProperty.uiColor.dwCurrentColor, uiRenderCoord, -1, -1, true, 0.f, sWord.m_sProperty.BgColor, nBorderFlag );
	}

	//m_pParent->DrawRect( uiRenderCoord, EtInterface::debug::RED );
}
#endif

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
void CDnNpcAnswerHtmlTextBox::GetLinePos( std::vector<EtVector2> &vecSmartMovePos )
{
	if( m_nCurLine == -1 )
		return;

	SUICoord sLineCoord;
	int nCount(0);
	int nVecSize = (int)m_vecLine.size();

	float fX, fY;
	for( int i=m_nCurLine; i<nVecSize; i++, nCount++ )
	{
		if( nCount >= m_nVisibleCount )
			break;
		fX = m_pParent->GetXCoord() + m_vecLine[i]->m_uiCoord.fX + GetUICoord().fWidth / 2.0f;
		fY = m_pParent->GetYCoord() + m_vecLine[i]->m_uiCoord.fY + m_vecLine[i]->m_uiCoord.fHeight / 2.0f + 4.0f/DEFAULT_UI_SCREEN_HEIGHT;	// 렌더링에서 y를 밀기때문에 여기서도 같이 밀어준다.
		vecSmartMovePos.push_back( EtVector2(fX, fY) );
	}
}
#endif