#include "StdAfx.h"
#include "EtUILineEditBox.h"
#include "EtUIIME.h"
#include "strsafe.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUILineEditBox::CEtUILineEditBox( CEtUIDialog *pParent )
	: CEtUIControl( pParent )
{
	m_Property.UIType = UI_CONTROL_LINE_EDITBOX;

	m_bCaretOn = true;
	m_fCaretBlinkTime = GetCaretBlinkTime() * 0.001f;
	QueryPerformanceCounter( &m_liLastBlinkTime );
	m_bInsertMode = true;
	m_bMouseDrag = false;
	m_nCurLine = -1;

	m_nSelStartPos = 0;
	m_nSelEndPos = 0;
	m_nSelStartLine = 0;
	m_nOriginSelStartPos = 0;
	m_nOriginSelEndPos = 0;
	m_nSelEndLine = 0;
	m_fCompositionWidth = 0.0f;

	m_nMaxTextLength = MAX_COMPSTRING_SIZE;
}

CEtUILineEditBox::~CEtUILineEditBox(void)
{
	DeleteBufferList();
}

void CEtUILineEditBox::DeleteBufferList()
{
	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		SAFE_DELETE( (*iter) );
	}
	m_listBuffer.clear();
}

void CEtUILineEditBox::Initialize( SUIControlProperty *pProperty )
{
	CEtUIControl::Initialize( pProperty );
	UpdateRects();
}

void CEtUILineEditBox::SetTemplate( int nTemplateIndex )
{
	CEtUIControl::SetTemplate( nTemplateIndex );

	SUIElement *pElement = GetElement(0);
	if( pElement )
	{
		//m_pFont = m_pParent->GetFont( pElement->nFontIndex, pElement->nFontHeight );
		if( AddLine( NULL ) )
			DownCurrentLine();
	}
}

void CEtUILineEditBox::Focus( bool bFocus )
{
	CEtUIControl::Focus( bFocus );
	CEtUIControl::SetFocusEditBox( IsFocus() );
	ResetCaretBlink();

	if (IsFocus() == false && IsTextSelected())
	{
		SBufferInfo* pLine = GetLine(m_nSelEndLine);
		if (pLine)
		{
			m_nSelStartPos = m_nSelEndPos = pLine->nCaretPos;
			m_nOriginSelStartPos = m_nOriginSelEndPos = pLine->nOriginCaretPos;
			m_nSelStartLine = m_nSelEndLine;
		}
	}	
}

//void CEtUILineEditBox::SetText( LPCWSTR wszText, bool bSelected )
//{
//	ASSERT( wszText != NULL );
//
//	m_Buffer.SetText( wszText );
//	m_nFirstVisible = 0;
//
//	PlaceCaret( m_Buffer.GetTextSize() );
//	if( bSelected )
//	{
//		m_nSelStart = 0;
//	}
//	else
//	{
//		m_nSelStart = m_nCaret;
//	}
//}

HRESULT CEtUILineEditBox::GetTextCopy( LPWSTR strDest, UINT bufferCount )
{
	ASSERT( strDest );
	std::wstring strTemp;

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		int nBufferSize = (*iter)->pUniBuffer->GetBufferSize();
		if( nBufferSize )
		{
			strTemp += (*iter)->pUniBuffer->GetBuffer();

			if( (*iter)->pNextLine == NULL )
			{
				strTemp += L"\n";
			}
		}
		else
		{
			strTemp += L"\n";
		}
	}

	if( bufferCount != 0 )
	{
		//wcscpy_s( strDest, bufferCount, strTemp.c_str() );
		StringCchCopyW(strDest, bufferCount, strTemp.c_str());
	}

	return S_OK;
}

HRESULT CEtUILineEditBox::GetOriginTextCopy( LPWSTR strDest, UINT bufferCount )
{
	ASSERT( strDest );
	std::wstring strTemp;

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		int nBufferSize = (*iter)->pOriginUniBuffer->GetTextSize();
		if( nBufferSize )
		{
			strTemp += (*iter)->pOriginUniBuffer->GetBuffer();

			if( (*iter)->pNextLine == NULL )
			{
				strTemp += L"\n";
			}
		}
		else
		{
			strTemp += L"\n";
		}
	}

	if( bufferCount != 0 )
	{
		StringCchCopyW(strDest, bufferCount, strTemp.c_str());
	}

	return S_OK;
}

void CEtUILineEditBox::ClearText()
{
	m_nSelStartPos = 0;
	m_nSelEndPos = 0;
	m_nOriginSelStartPos = 0;
	m_nOriginSelEndPos = 0;
	m_nSelStartLine = 0;
	m_nSelEndLine = 0;

	DeleteBufferList();
	if( AddLine( NULL ) )
		DownCurrentLine();
}

bool CEtUILineEditBox::HandleKeyboard( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	bool bHandled(false);

	SBufferInfo *pBufferInfo(NULL);
	pBufferInfo = GetCurrentLine();
	if( !pBufferInfo ) return false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_TAB:
				break;
			case VK_HOME:
				{
					PlaceCaretBegin( pBufferInfo );
					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						SetSelectStartPos();
					}
					else
					{
						SetSelectEndPos();
					}
					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_END:
				{
					PlaceCaretEnd( pBufferInfo );
					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						SetSelectStartPos();
					}
					else
					{
						SetSelectEndPos();
					}
					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_INSERT:
				{
					m_bInsertMode = !m_bInsertMode;
				}
				break;
			case VK_DELETE:
				{
					if( IsTextSelected() )
					{
						DeleteSelectionText();
					}
					else
					{
						if( pBufferInfo->IsEndCaret() )
						{
							SBufferInfo *pNextLine = GetLine( m_nCurLine+1 );
							if( pNextLine )
							{
								PasteTrailFromLine( pBufferInfo, pNextLine );
							}
						}
						else
						{
							if( !CheckTagTextRemove( pBufferInfo, pBufferInfo->nCaretPos ) )
							{
								RemoveOriginCharLine( pBufferInfo, pBufferInfo->nOriginCaretPos );
								RemoveCharLine( pBufferInfo, pBufferInfo->nCaretPos );
							}
						}
					}

					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_LEFT:
				{
					if( pBufferInfo->nCaretPos > 0 )
					{
						PlaceCaret( pBufferInfo, pBufferInfo->nCaretPos - 1 );
					}
					else
					{
						SBufferInfo *pBufferInfoInner = GetLine( m_nCurLine-1 );
						if( pBufferInfoInner )
						{
							UpCurrentLine();
							PlaceCaretEnd( pBufferInfoInner );
						}
					}

					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						SetSelectStartPos();
					}
					else
					{
						SetSelectEndPos();
					}

					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_RIGHT:
				{
					if( pBufferInfo->nCaretPos < pBufferInfo->GetTextSize() )
					{
						PlaceCaret( pBufferInfo, pBufferInfo->nCaretPos + 1 );
					}
					else
					{
						SBufferInfo *pBufferInfoInner = GetLine( m_nCurLine+1 );
						if( pBufferInfoInner )
						{
							DownCurrentLine();
							PlaceCaretBegin( pBufferInfoInner );
						}
					}

					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						SetSelectStartPos();
					}
					else
					{
						SetSelectEndPos();
					}

					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_UP:
				{
					UpCurrentLine();
					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						SetSelectStartPos();
					}
					else
					{
						SetSelectEndPos();
					}
					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_DOWN:
				{
					DownCurrentLine();
					if( GetKeyState( VK_SHIFT ) >= 0 )
					{
						SetSelectStartPos();
					}
					else
					{
						SetSelectEndPos();
					}
					ResetCaretBlink();
					bHandled = true;
				}
				break;
			case VK_ESCAPE:
				// 원래 LineEditBox, LineIMEEditBox 는 EVENT_EDITBOX 계열을 사용하지 않았으나,
				// 메일쓰기창에서 포커스가 가있는 상태에서 Esc로 창을 닫을 수 있어야해서,
				// 이렇게 EVENT_EDITBOX_ESCAPE를 사용하게 되었다.
				// EVENT_EDITBOX 계열의 다른 메세지도 전부 필요하게 된다면, 그때가서 추가하도록 하겠다.
				m_pParent->ProcessCommand( EVENT_EDITBOX_ESCAPE, true, this );
				bHandled = true;
				break;
			default:
				bHandled = wParam != VK_ESCAPE;
			}
		}
		break;
	}

	return bHandled;
}

bool CEtUILineEditBox::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
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
			if( !m_bFocus )
			{
				m_pParent->RequestFocus( this );
			}

			if( !IsInside( fX, fY ) )
			{
				return false;
			}

			m_bMouseDrag = true;
			SetCapture( m_pParent->GetHWnd() );

			int nLine = GetLineMouseIn( fX, fY );
			if( nLine != -1 )
			{
				SetCurrentLine( nLine );
				SBufferInfo *pBufferInfo = GetCurrentLine();
				if( pBufferInfo )
				{
					int nCP, nTrail, nX1st;
					CEtFontMng& FontMng = CEtFontMng::GetInstance();
					SUIElement* pElement = GetElement( 0 );
					nX1st = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 0 );
					//pBufferInfo->CPtoX( 0, FALSE, &nX1st );
					nCP = FontMng.GetCaretFromCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 
														( int )( m_pParent->GetScreenWidth() * ( fX - pBufferInfo->TextCoord.fX ) + nX1st ), nTrail );

					//if( SUCCEEDED( pBufferInfo->XtoCP( ( int )( m_pParent->GetScreenWidth() * ( fX - pBufferInfo->TextCoord.fX ) + nX1st ), &nCP, &nTrail ) ) )
					if( 0 <= nCP )
					{
						if( ( nTrail ) && ( nCP < pBufferInfo->GetTextSize() ) )
						{
							PlaceCaret( pBufferInfo, nCP+1 );
						}
						else
						{
							PlaceCaret( pBufferInfo, nCP );
						}

						SetSelectStartPos();
						ResetCaretBlink();
					}
				}
			}

			return true;
		}
	case WM_LBUTTONUP:
		ReleaseCapture();
		m_bMouseDrag = false;
		break;
	case WM_MOUSEMOVE:
		if( m_bMouseDrag )
		{
			int nLine = GetLineMouseIn( fX, fY );
			if( nLine != -1 )
			{
				SetCurrentLine( nLine );
				SBufferInfo *pBufferInfo = GetCurrentLine();
				if( pBufferInfo )
				{
					int nCP, nTrail, nX1st;
					CEtFontMng& FontMng = CEtFontMng::GetInstance();
					SUIElement* pElement = GetElement( 0 );
					nX1st = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 0 );
					//pBufferInfo->CPtoX( 0, FALSE, &nX1st );
					nCP = FontMng.GetCaretFromCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 
													    ( int )( m_pParent->GetScreenWidth() * ( fX - pBufferInfo->TextCoord.fX ) + nX1st ), nTrail );

					//if( SUCCEEDED( pBufferInfo->XtoCP( ( int )( m_pParent->GetScreenWidth() * ( fX - pBufferInfo->TextCoord.fX ) + nX1st ), &nCP, &nTrail ) ) )
					if( 0 <= nCP )
					{
						if( ( nTrail ) && ( nCP < pBufferInfo->GetTextSize() ) )
						{
							PlaceCaret( pBufferInfo, nCP+1 );
						}
						else
						{
							PlaceCaret( pBufferInfo, nCP );
						}

						SetSelectEndPos();
					}
				}
			}
		}
		break;
	}

	return false;
}

bool CEtUILineEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() )
	{
		return false;
	}

	SBufferInfo *pBufferInfo(NULL);
	pBufferInfo = GetCurrentLine();
	if( !pBufferInfo ) return false;

	switch( uMsg )
	{
	case WM_KEYUP:
	case WM_KEYDOWN:
		break;
		//return true;
	case WM_CHAR:
		switch( ( WCHAR )wParam )
		{
		case VK_BACK:
			{
				if( IsTextSelected() )
				{
					DeleteSelectionText();
				}
				else
				{
					if( pBufferInfo->nCaretPos > 0 )
					{
						PlaceCaret( pBufferInfo, pBufferInfo->nCaretPos - 1 );
						if( !CheckTagTextRemove( pBufferInfo, pBufferInfo->nCaretPos ) )
						{
							RemoveOriginCharLine( pBufferInfo, pBufferInfo->nOriginCaretPos );
							RemoveCharLine( pBufferInfo, pBufferInfo->nCaretPos );
						}
					}
					else if( IsBeginLine(pBufferInfo) )
					{
					}
					else
					{
						if( pBufferInfo->GetTextSize() > 0 )
						{
							UpCurrentLine();
							PlaceCaretEnd( GetCurrentLine() );

							pBufferInfo = GetLine( m_nCurLine );
							if( pBufferInfo && pBufferInfo->IsEndCaret() )
							{
								SBufferInfo *pNextLine = GetLine( m_nCurLine+1 );
								if( pNextLine )
								{
									PasteTrailFromLine( pBufferInfo, pNextLine );
								}
							}
						}
						else
						{
							if( !IsBeginLine(pBufferInfo) )
							{
								DeleteLine( pBufferInfo );
								UpCurrentLine();
								PlaceCaretEnd( GetCurrentLine() );
							}
						}
					}
				}

				SetSelectStartPos();
				ResetCaretBlink();
			}
			break;
		case 24:        // Ctrl-X Cut
		case VK_CANCEL: // Ctrl-C Copy
			CopyToClipboard();
			if( ( WCHAR )wParam == 24 )
			{
				DeleteSelectionText();
			}
			break;
		case 22:		// Ctrl-V Paste
			PasteFromClipboard();
			break;
		case 1:			// Ctrl-A Select All
			if( !IsTextSelected() )
			{
				m_nSelStartLine = 0;
				m_nSelStartPos = 0;
				m_nOriginSelStartPos = 0;

				int nSize = (int)m_listBuffer.size()-1;
				SetCurrentLine( nSize );
				SBufferInfo *pLine = GetCurrentLine();
				PlaceCaretEnd( GetCurrentLine() );

				m_nSelEndLine = nSize;
				m_nSelEndPos = pLine->GetTextSize();
				m_nOriginSelEndPos = pLine->GetOriginTextSize();
			}
			break;
		case VK_RETURN:
			{
				if( pBufferInfo->IsEndCaret() )
				{
					if( AddLine( pBufferInfo ) )
						DownCurrentLine();
				}
				else
				{
					SplitLineToAdd( pBufferInfo );
				}

				SetSelectStartPos();
			}
			break;
			// Junk characters we don't want in the string
		case 26:  // Ctrl Z
		case 2:   // Ctrl B
		case 14:  // Ctrl N
		case 19:  // Ctrl S
		case 4:   // Ctrl D
		case 6:   // Ctrl F
		case 7:   // Ctrl G
		case 10:  // Ctrl J
		case 11:  // Ctrl K
		case 12:  // Ctrl L
		case 17:  // Ctrl Q
		case 23:  // Ctrl W
		case 5:   // Ctrl E
		case 18:  // Ctrl R
		case 20:  // Ctrl T
		case 25:  // Ctrl Y
		case 21:  // Ctrl U
		case 9:   // Ctrl I
		case 15:  // Ctrl O
		case 16:  // Ctrl P
		case 27:  // Ctrl [
		case 29:  // Ctrl ]
		case 28:  // Ctrl \ 
			break;

		default:
			if( IsTextSelected() )
			{
				DeleteSelectionText();
			}

			if( ( !m_bInsertMode ) && ( pBufferInfo->nCaretPos < pBufferInfo->GetTextSize() ) )
			{
				(*pBufferInfo->pUniBuffer)[pBufferInfo->nCaretPos] = (WCHAR)wParam;
				PlaceCaret( pBufferInfo, pBufferInfo->nCaretPos + 1 );
			} 
			else
			{
				InsertCharLine( pBufferInfo, pBufferInfo->nCaretPos, (WCHAR)wParam );
			}

			SetSelectStartPos();
			ResetCaretBlink();
		}
		return true;
	}

	return false;
}

void CEtUILineEditBox::UpdateRects()
{
	SUICoord sSymbolCoord;
	m_pParent->CalcTextRect( L"X", GetElement(0), sSymbolCoord );

	float fLineSpace(0.0f);

	if( m_Property.UIType == UI_CONTROL_LINE_IMEEDITBOX )
	{
		fLineSpace = m_Property.LineIMEEditBoxProperty.fLineSpace;
	}
	else if( m_Property.UIType == UI_CONTROL_LINE_EDITBOX )
	{
		fLineSpace = m_Property.LineEditBoxProperty.fLineSpace;
	}

	float fY(m_Property.UICoord.fY);

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		(*iter)->TextCoord = m_Property.UICoord;
		(*iter)->TextCoord.fY = fY;
		(*iter)->TextCoord.fHeight = sSymbolCoord.fHeight;
		fY += sSymbolCoord.fHeight + fLineSpace;
	}
}

void CEtUILineEditBox::Render( float fElapsedTime )
{
	//HRESULT hr;
	int nSelStartX(0), nCaretX(0), nXFirst(0);
	SUIElement *pElement(NULL);

	UpdateBlendRate();

	//int nVecSize = (int)m_Template.m_vecElement.size();
	//for( int i = 0; i < nVecSize; i++ )
	//{
	//	pElement = GetElement(i);
	//	pElement->TextureColor.Blend( UI_STATE_NORMAL, fElapsedTime, m_fBlendRate );
	//	m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_RenderCoord[ i ] );
	//}

	//m_pParent->DrawRect( m_Property.UICoord, EtInterface::debug::BLUE );

	SBufferInfo *pBufferInfo(NULL);
	pBufferInfo = GetCurrentLine();
	if( !pBufferInfo ) return;

	SUICoord SelectionCoord;
	
	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	pElement = GetElement( 0 );

	if( m_nSelStartLine == m_nSelEndLine )
	{
		//pBufferInfo->CPtoX( 0, FALSE, &nXFirst );
		nXFirst = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 0 );
		//hr = pBufferInfo->CPtoX( pBufferInfo->nCaretPos, FALSE, &nCaretX );
		nCaretX = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, pBufferInfo->nCaretPos );

		if( IsTextSelected() )
		{
			//hr = pBufferInfo->CPtoX( m_nSelStartPos, FALSE, &nSelStartX );
			nSelStartX = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nSelStartPos );
		}
		else
		{
			nSelStartX = nCaretX;
		}

		if( pBufferInfo->nCaretPos != m_nSelStartPos )
		{
			int nSelLeftX = nCaretX;
			int nSelRightX = nSelStartX;

			if( nSelLeftX > nSelRightX )
			{
				std::swap( nSelLeftX, nSelRightX );
			}

			SelectionCoord.SetCoord( ( ( float )nSelLeftX ) / m_pParent->GetScreenWidth(), pBufferInfo->TextCoord.fY, 
									( ( float )( nSelRightX - nSelLeftX ) ) / m_pParent->GetScreenWidth(), pBufferInfo->TextCoord.fHeight );

			SelectionCoord.fX += pBufferInfo->TextCoord.fX - ( ( float )nXFirst ) / m_pParent->GetScreenWidth();
			float fMinRight = min( pBufferInfo->TextCoord.Right(), SelectionCoord.Right() );
			float fMinBottom = min( pBufferInfo->TextCoord.Bottom(), SelectionCoord.Bottom() );
			SelectionCoord.fWidth = fMinRight - SelectionCoord.fX;
			SelectionCoord.fHeight = fMinBottom - SelectionCoord.fY;
			SelectionCoord.fX = max( pBufferInfo->TextCoord.fX, SelectionCoord.fX );
			SelectionCoord.fY = max( pBufferInfo->TextCoord.fY, SelectionCoord.fY );

			bool bEnableZ = GetEtDevice()->EnableZ( false );
			m_pParent->DrawRect( SelectionCoord, m_Property.LineEditBoxProperty.dwSelBackColor );
			GetEtDevice()->EnableZ( bEnableZ );
		}
	}
	else
	{
		bool bSwap(false);

		if( m_nSelStartLine > m_nSelEndLine )
		{
			std::swap( m_nSelStartLine, m_nSelEndLine );
			std::swap( m_nSelStartPos, m_nSelEndPos );

			bSwap = true;
		}

		int nSelStartLine = m_nSelStartLine;
		int nSelEndLine = m_nSelEndLine;
		int nSelStartPos = m_nSelStartPos;
		int nSelEndPos = m_nSelEndPos;

		for( int i=nSelStartLine; i<=nSelEndLine; ++i )
		{
			SBufferInfo *pBufferInfoInner = GetLine(i);
			if( !pBufferInfoInner ) continue;

			if( i > nSelStartLine )
			{
				nSelStartPos = 0;
			}

			if( i < nSelEndLine )
			{
				nSelEndPos = pBufferInfoInner->GetTextSize();
			}
			else
			{
				nSelEndPos = m_nSelEndPos;
			}

			int nSelLeftX(0);
			int nSelRightX(0);

			nSelLeftX = FontMng.GetCaretPos( pBufferInfoInner->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nSelStartPos );
			//pBufferInfoInner->CPtoX( nSelStartPos, FALSE, &nSelLeftX );
			nSelRightX = FontMng.GetCaretPos( pBufferInfoInner->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nSelEndPos );
			//pBufferInfoInner->CPtoX( nSelEndPos, FALSE, &nSelRightX );
			
			SelectionCoord.SetCoord( ( ( float )nSelLeftX ) / m_pParent->GetScreenWidth(), pBufferInfoInner->TextCoord.fY, 
									( ( float )( nSelRightX - nSelLeftX ) ) / m_pParent->GetScreenWidth(), pBufferInfoInner->TextCoord.fHeight );

			SelectionCoord.fX += pBufferInfoInner->TextCoord.fX - ( ( float )nXFirst ) / m_pParent->GetScreenWidth();
			float fMinRight = min( pBufferInfoInner->TextCoord.Right(), SelectionCoord.Right() );
			float fMinBottom = min( pBufferInfoInner->TextCoord.Bottom(), SelectionCoord.Bottom() );
			SelectionCoord.fWidth = fMinRight - SelectionCoord.fX;
			SelectionCoord.fHeight = fMinBottom - SelectionCoord.fY;
			SelectionCoord.fX = max( pBufferInfoInner->TextCoord.fX, SelectionCoord.fX );
			SelectionCoord.fY = max( pBufferInfoInner->TextCoord.fY, SelectionCoord.fY );

			bool bEnableZ = GetEtDevice()->EnableZ( false );
			m_pParent->DrawRect( SelectionCoord, m_Property.LineEditBoxProperty.dwSelBackColor );
			GetEtDevice()->EnableZ( bEnableZ );
		}

		if( bSwap )
		{
			std::swap( m_nSelStartLine, m_nSelEndLine );
			std::swap( m_nSelStartPos, m_nSelEndPos );
		}

		nCaretX = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nSelEndPos );
		//pBufferInfo->CPtoX( m_nSelEndPos, FALSE, &nCaretX );
	}

	pElement = GetElement(0);

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		m_pParent->DrawDlgText( (*iter)->pUniBuffer->GetBuffer(), pElement,	m_Property.LineEditBoxProperty.dwTextColor, (*iter)->TextCoord, -1, pElement->dwFontFormat, true );

		//m_pParent->DrawRect( (*iter)->TextCoord, EtInterface::debug::YELLOW );

		//if( (*iter)->nCaretPos != m_nSelStartPos )
		//{
		//	int nFirstToRender, nNumChatToRender;

		//	nFirstToRender = max( 0, min( m_nSelStartPos, (*iter)->nCaretPos ) );
		//	nNumChatToRender = max( m_nSelStartPos, (*iter)->nCaretPos ) - nFirstToRender;

		//	m_pParent->DrawDlgText( (*iter)->pUniBuffer->GetBuffer() + nFirstToRender, pElement,
		//		m_Property.LineEditBoxProperty.dwSelTextColor, SelectionCoord,
		//		-1, pElement->dwFontFormat, true );
		//}
	}

	LARGE_INTEGER liCurTime, liFrequency;
	float fElapsedBlinkTime;

	QueryPerformanceCounter( &liCurTime );
	QueryPerformanceFrequency( &liFrequency );
	fElapsedBlinkTime = ( ( float )( liCurTime.QuadPart - m_liLastBlinkTime.QuadPart ) ) / liFrequency.QuadPart;

	if( fElapsedBlinkTime >= m_fCaretBlinkTime )
	{
		m_bCaretOn = !m_bCaretOn;
		m_liLastBlinkTime = liCurTime;
	}

	SUICoord CaretCoord;
	CaretCoord = pBufferInfo->TextCoord;
	CaretCoord.fX += ( ( float )( -nXFirst + nCaretX - 1 ) ) / m_pParent->GetScreenWidth();
	CaretCoord.fWidth = 2.0f / m_pParent->GetScreenWidth();

	if (IsFocus())
	{
		CEtUIEditBox::s_CurrentCaretPos = CaretCoord;
		if( m_pParent )
		{
			CEtUIEditBox::s_CurrentCaretPos.fX += m_pParent->GetXCoord();
			CEtUIEditBox::s_CurrentCaretPos.fY += m_pParent->GetYCoord();
		}
	}

	if( ( m_bFocus ) && ( m_bCaretOn ) && ( !CEtUIIME::s_bHideCaret ) )
	{
		if( !m_bInsertMode )
		{
			int nRightEdgeX;
			//pBufferInfo->CPtoX( pBufferInfo->nCaretPos, TRUE, &nRightEdgeX );
			nRightEdgeX = FontMng.GetCaretPos( pBufferInfo->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, pBufferInfo->nCaretPos );
			CaretCoord.fWidth = pBufferInfo->TextCoord.fX + ( ( float )( nRightEdgeX - nXFirst ) ) / m_pParent->GetScreenWidth() - CaretCoord.fX;
		}
		bool bEnableZ = GetEtDevice()->EnableZ( false );
		m_pParent->DrawRect( CaretCoord, m_Property.LineEditBoxProperty.dwCaretColor );
		GetEtDevice()->EnableZ( bEnableZ );
	}
}

void CEtUILineEditBox::PlaceCaret( SBufferInfo *pBufferInfo, int nCP )
{
	if( !pBufferInfo ) return;
	if( nCP < 0 ) return;
	if( nCP > pBufferInfo->GetTextSize() ) return;

	//ASSERT( pBufferInfo );
	//ASSERT( ( nCP >= 0 ) && ( nCP <= pBufferInfo->GetTextSize() ) );
	pBufferInfo->nCaretPos = nCP;
	pBufferInfo->nOriginCaretPos = GetOriginTextCaretPos( pBufferInfo, nCP, true );
}

int CEtUILineEditBox::GetOriginTextCaretPos( SBufferInfo* pBufferInfo, int nCaretPos, bool bRetWithCalcTagLast )
{
	if( nCaretPos == 0 || pBufferInfo == NULL || pBufferInfo->pOriginUniBuffer == NULL )
		return 0;

	std::wstring text = pBufferInfo->pOriginUniBuffer->GetBuffer();
	int caretPosFound = 0;
	int OriginTextCaretPos = 0;
	bool bInsideTag = false;

	WORD sep = EtInterface::GetNameLinkMgr().GetNameLinkSeperator();
	while( OriginTextCaretPos < int(text.size()) )
	{
		if( nCaretPos == caretPosFound )
		{
			if( bRetWithCalcTagLast )
			{
				if( bInsideTag )
				{
					std::wstring parsee = text.substr( OriginTextCaretPos, std::wstring::npos );
					std::wstring targetTag;
					targetTag = FormatW( L"%ce", sep );
					std::wstring::size_type loc = parsee.find( targetTag.c_str() );
					OriginTextCaretPos += (loc != std::wstring::npos) ? int(loc + 2) : int(parsee.length());
				}
			}
			else
			{
				CEtUINameLinkMgr::eNameLinkTagType type = EtInterface::GetNameLinkMgr().IsTagType( text, OriginTextCaretPos );
				if( type != CEtUINameLinkMgr::eNLTAG_NONE )
				{
					std::wstring parsee = text.substr( OriginTextCaretPos, std::wstring::npos );
					std::wstring targetTag;
					targetTag = FormatW( L"%ce", sep );
					std::wstring::size_type loc = parsee.find( targetTag.c_str() );
					OriginTextCaretPos += (loc != std::wstring::npos) ? int(loc + 2) : int(parsee.length());
				}
			}
			break;
		}

		CEtUINameLinkMgr::eNameLinkTagType type = EtInterface::GetNameLinkMgr().IsTagType( text, OriginTextCaretPos );
		if( type != CEtUINameLinkMgr::eNLTAG_NONE )
		{
			if( type == CEtUINameLinkMgr::eNLTAG_START )
				bInsideTag = true;
			else if( type == CEtUINameLinkMgr::eNLTAG_END )
				bInsideTag = false;

			//	todo by kalliste : parsee 의 태그 텍스트 길이가 일정 이하일 때 에러처리. 할 필요가 없으면 태그 타입에 따라 고정값 넘기는 함수 사용.
			int offset = 0;
			std::wstring parsee = text.substr( ++OriginTextCaretPos, std::wstring::npos );
			EtInterface::GetNameLinkMgr().GetTagOffsetByParseText( parsee, offset );
			OriginTextCaretPos += offset;
		}
		else
		{
			++caretPosFound;
			++OriginTextCaretPos;
		}
	}

	return OriginTextCaretPos;
}

int CEtUILineEditBox::GetTextCaretPos( SBufferInfo* pBufferInfo, int nOriginTextCaretPos )
{
	if( nOriginTextCaretPos <= 0 || pBufferInfo == NULL || pBufferInfo->pOriginUniBuffer == NULL )
		return 0;

	std::wstring text = pBufferInfo->pOriginUniBuffer->GetBuffer();
	std::wstring::size_type linkStartPos = 0;
	int caretPosFound = 0;
	int curFullCaretPos = 0;
	bool bInsideTag = false;

	while( curFullCaretPos < int(text.size()) && (curFullCaretPos < nOriginTextCaretPos) )
	{
		CEtUINameLinkMgr::eNameLinkTagType type = EtInterface::GetNameLinkMgr().IsTagType( text, curFullCaretPos );
		if( type != CEtUINameLinkMgr::eNLTAG_NONE )
		{
			int offset = 0;
			std::wstring parsee = text.substr( curFullCaretPos + 1, std::wstring::npos );
			EtInterface::GetNameLinkMgr().GetTagOffsetByParseText( parsee, offset );
			curFullCaretPos += (offset + 1);
		}
		else
		{
			++caretPosFound;
			++curFullCaretPos;
		}
	}

	return caretPosFound;
}

void CEtUILineEditBox::PlaceCaretBegin( SBufferInfo *pBufferInfo )
{
	ASSERT( pBufferInfo );
	PlaceCaret( pBufferInfo, 0 );
}

void CEtUILineEditBox::PlaceCaretEnd( SBufferInfo *pBufferInfo )
{
	ASSERT( pBufferInfo );
	PlaceCaret( pBufferInfo, pBufferInfo->GetTextSize() );
}

void CEtUILineEditBox::DeleteSelectionText()
{
	if (m_nSelStartLine == m_nSelEndLine && m_nSelStartPos == m_nSelEndPos)
		return;

	if( m_nSelStartLine > m_nSelEndLine )
	{
		std::swap( m_nSelStartLine, m_nSelEndLine );
		std::swap( m_nSelStartPos, m_nSelEndPos );
		std::swap( m_nOriginSelStartPos, m_nOriginSelEndPos );
	}
	else if( m_nSelStartLine == m_nSelEndLine )
	{
		if( m_nSelStartPos > m_nSelEndPos )
		{
			std::swap( m_nSelStartPos, m_nSelEndPos );
			std::swap( m_nOriginSelStartPos, m_nOriginSelEndPos );
		}
	}

	SetCurrentLine( m_nSelStartLine );

	// Note : 삭제해야 할 모든 라인을 연결해 준다.
	for( int i=m_nSelStartLine; i<=m_nSelEndLine; i++ )
	{
		if( i+1 <= m_nSelEndLine )
		{
			SBufferInfo *pCurLine = GetLine(i);
			pCurLine->pNextLine = GetLine(i+1);
		}
	}

	// Note : 삭제해야할 문자의 개수를 카운트 한다.
	int nCount(0);

	for( int i=m_nSelStartLine; i<m_nSelEndLine; i++ )
	{
		SBufferInfo *pLine = GetLine(i);
		if( pLine->GetTextSize() == 0 )
		{
			// Note : 하나의 라인을 한문자로 간주한다.
			nCount += 1;
		}
		else
		{
			nCount += pLine->GetTextSize();
		}
	}

	nCount -= m_nSelStartPos;
	nCount += m_nSelEndPos;

	// Note : 카운트 만큼 문자를 지운다.
	int nSelLine = m_nSelStartLine;
	int nSelPos = m_nSelStartPos;
	int nOriginSelPos = m_nOriginSelStartPos;
	for( int i=0; i<nCount; )
	{
		SBufferInfo* pBufferInfo = GetLine( nSelLine );
		if( pBufferInfo ) 
		{
			bool bRet = false;

			if( !CheckTagTextRemove( pBufferInfo, nSelPos ) )
			{
				bRet = RemoveOriginCharLine( pBufferInfo, nOriginSelPos );
				bRet = RemoveCharLine( pBufferInfo, nSelPos );
			}

			if( bRet == false )
			{
				if( pBufferInfo && nSelPos >= pBufferInfo->GetTextSize() )
				{
					nSelLine++;
					nSelPos = 0;
					nOriginSelPos = 0;
					pBufferInfo->pNextLine = NULL;
					continue;
				}
			}
		}

		i++;
	}

	PlaceCaret( GetLine(m_nSelStartLine), m_nSelStartPos );
	SetSelectStartPos();
}

void CEtUILineEditBox::ResetCaretBlink()
{
	m_bCaretOn = true;
	QueryPerformanceCounter( &m_liLastBlinkTime );
}

void CEtUILineEditBox::CopyStringToClipboard( const wchar_t *wszText )
{
	if( !::OpenClipboard( NULL ) )
		return;
	HGLOBAL hBlock = NULL;
	int nLength = (int)wcslen( wszText );
	if( nLength > 0 )
	{
		::EmptyClipboard();
		hBlock = ::GlobalAlloc( GMEM_MOVEABLE, sizeof( wchar_t ) * ( nLength + 1 ) );
		if( hBlock )
		{
			WCHAR *pwszText = ( WCHAR * )::GlobalLock( hBlock );
			if( pwszText )
			{
				memcpy( pwszText, wszText, nLength*sizeof(wchar_t) );
				pwszText[nLength] = L'\0';
				::GlobalUnlock( hBlock );
			}
			::SetClipboardData( CF_UNICODETEXT, hBlock );
		}
	}
	::CloseClipboard();
	if( hBlock )
	{
		::GlobalFree( hBlock );
	}
}

void CEtUILineEditBox::CopyToClipboard()
{
	if( IsTextSelected() )
	{
		if( !OpenClipboard( NULL ) )
			return;

		int nSelStartLine = m_nSelStartLine;
		int nSelEndLine = m_nSelEndLine;
		int nSelStartPos = m_nSelStartPos;
		int nSelEndPos = m_nSelEndPos;

		if( nSelStartLine > nSelEndLine )
		{
			std::swap( nSelStartLine, nSelEndLine );
			std::swap( nSelStartPos, nSelEndPos );
		}
		else if( nSelStartLine == nSelEndLine )
		{
			if( nSelStartPos > nSelEndPos )
			{
				std::swap( nSelStartPos, nSelEndPos );
			}
		}

		wchar_t wszTemp[2048]={0};
		int nFirst, nLast;

		for( int i=nSelStartLine; i<=nSelEndLine; ++i )
		{
			SBufferInfo *pBufferInfo = GetLine(i);
			if( !pBufferInfo ) continue;

			if( i > nSelStartLine )
			{
				nFirst = 0;

				if( pBufferInfo->pPrevLine == NULL )
				{
					wcscat_s( wszTemp, _countof(wszTemp), L"\r\n" );
				}
			}
			else
			{
				nFirst = nSelStartPos;
			}

			if( i < nSelEndLine )
			{
				nLast = pBufferInfo->GetTextSize();
			}
			else
			{
				nLast = nSelEndPos;
			}

			wcsncat_s( wszTemp, _countof(wszTemp), pBufferInfo->pUniBuffer->GetBuffer()+nFirst, (nLast-nFirst) );
		}

		// Note : 라인에디터에서 복사한 텍스트를 클립보드에 카피한다.
		//
		HGLOBAL hBlock = NULL;
		int nLength = (int)wcslen(wszTemp);

		if( nLength > 0 )
		{
			EmptyClipboard();
			hBlock = GlobalAlloc( GMEM_MOVEABLE, sizeof( wchar_t ) * ( nLength + 1 ) );
			if( hBlock )
			{
				WCHAR *pwszText = ( WCHAR * )GlobalLock( hBlock );
				if( pwszText )
				{
					memcpy( pwszText, wszTemp, nLength*sizeof(wchar_t) );
					pwszText[nLength] = L'\0';
					GlobalUnlock( hBlock );
				}

				SetClipboardData( CF_UNICODETEXT, hBlock );
			}
		}

		CloseClipboard();
		if( hBlock )
		{
			GlobalFree( hBlock );
		}
	}
}

void CEtUILineEditBox::SetTextBuffer( const WCHAR* pwszText )
{
	if( pwszText == NULL )
		return;

	int nLength = (int)wcslen(pwszText);
	for( int i=0; i<nLength; i++ )
	{
		SBufferInfo *pBufferInfo = GetCurrentLine();
		if( !pBufferInfo ) continue;

		if( pwszText[i] == L'\r' || pwszText[i] == L'\n' )
		{
			if( pBufferInfo->IsEndCaret() )
			{
				if( AddLine( pBufferInfo ) )
					DownCurrentLine();
			}
			else
			{
				SplitLineToAdd( pBufferInfo );
			}

			SetSelectStartPos();
		}
		else
		{
			InsertCharLine( pBufferInfo, pBufferInfo->nCaretPos, pwszText[i], true );
			SetSelectStartPos();
		}
	}
}

void CEtUILineEditBox::AddText( LPCWSTR wszText, bool bFocus )
{
	ASSERT( wszText != NULL );

	std::wstring translated;
	int pureTextLength = EtInterface::GetNameLinkMgr().TranslateText( translated, wszText );
	if( IsTextFull( pureTextLength ) )
		return;

	SBufferInfo* pBufferInfo = GetCurrentLine();
	if( pBufferInfo )
	{
		int nTagFieldStart, nTagFieldEnd;
		if( IsInTagField( pBufferInfo, pBufferInfo->nCaretPos, nTagFieldStart, nTagFieldEnd ) )
		{
			pBufferInfo->nCaretPos = GetTextCaretPos( pBufferInfo, nTagFieldEnd );
		}

		std::wstring strBuffer = pBufferInfo->pOriginUniBuffer->GetBuffer();
		int nBufferLength = static_cast<int>( strBuffer.length() );
		if( pBufferInfo->nOriginCaretPos == nBufferLength )
		{
			strBuffer += wszText;
		}
		else
		{
			std::wstring strTemp = strBuffer;
			if( nBufferLength - pBufferInfo->nOriginCaretPos > 0 )
				strBuffer.erase( pBufferInfo->nOriginCaretPos, nBufferLength - pBufferInfo->nOriginCaretPos );
			if( pBufferInfo->nOriginCaretPos > 0 )
				strTemp.erase( 0, pBufferInfo->nOriginCaretPos );

			strBuffer += wszText;
			strBuffer += strTemp;
		}
		pBufferInfo->pOriginUniBuffer->SetText( strBuffer.c_str() );
		pBufferInfo->nOriginCaretPos += static_cast<int>( strBuffer.length() );
	}

	SetTextBuffer( translated.c_str() );
}

bool CEtUILineEditBox::CheckTagTextInsertPos( SBufferInfo* pBufferInfo, int nIndex )
{
	if( pBufferInfo == NULL )
		return false;

	bool bInTagField = false;

	int nTagFieldStart, nTagFieldEnd;
	if( IsInTagField( pBufferInfo, nIndex, nTagFieldStart, nTagFieldEnd ) )
	{
		pBufferInfo->nCaretPos = GetTextCaretPos( pBufferInfo, nTagFieldEnd );
		pBufferInfo->nOriginCaretPos = GetOriginTextCaretPos( pBufferInfo, pBufferInfo->nCaretPos, false );
		bInTagField = true;
	}

	return bInTagField;
}

bool CEtUILineEditBox::CheckTagTextRemove( SBufferInfo* pBufferInfo, int nCaretPos )
{
	if( pBufferInfo == NULL )
		return false;

	bool bInTagField = false;

	int nTagFieldStart, nTagFieldEnd;
	if( IsInTagField( pBufferInfo, nCaretPos, nTagFieldStart, nTagFieldEnd ) )
	{
		int nTextStart = GetTextCaretPos( pBufferInfo, nTagFieldStart );
		int nTextEnd = GetTextCaretPos( pBufferInfo, nTagFieldEnd );
		for( int i=nTextStart; i<nTextEnd; i++ )
		{
			RemoveCharLine( pBufferInfo, nTextStart );
		}

		for( int i=nTagFieldStart; i<=nTagFieldEnd+1; i++ )
		{
			RemoveOriginCharLine( pBufferInfo, nTagFieldStart );
		}

		pBufferInfo->nCaretPos = nTextStart;
		pBufferInfo->nOriginCaretPos = nTagFieldStart;
		bInTagField = true;
	}

	return bInTagField;
}

bool CEtUILineEditBox::IsInTagField( SBufferInfo* pBufferInfo, int nCaretPos, int& nTagFieldStart, int& nTagFieldEnd )
{
	if( pBufferInfo == NULL || pBufferInfo->pOriginUniBuffer == NULL )
		return false;

	if( pBufferInfo->GetTextSize() == pBufferInfo->GetOriginTextSize() )
		return false;

	bool bInTagField = false;
	std::wstring fullText = pBufferInfo->pOriginUniBuffer->GetBuffer();
	int nOriginCaretPos = GetOriginTextCaretPos( pBufferInfo, nCaretPos, false );

	std::wstring targetTag;
	std::wstring::size_type tagStart, tagEnd;
	tagStart = tagEnd = std::wstring::npos;
	EtInterface::GetNameLinkMgr().MakeTag( targetTag, CEtUINameLinkMgr::eNLTAG_START );
	tagStart = fullText.rfind( targetTag, nOriginCaretPos );

	if( tagStart != std::wstring::npos )
	{
		EtInterface::GetNameLinkMgr().MakeTag( targetTag, CEtUINameLinkMgr::eNLTAG_END );
		tagEnd = fullText.find( targetTag, tagStart );

		if( int(tagStart) <= nOriginCaretPos && nOriginCaretPos < int(tagEnd) )
		{
			nTagFieldStart = int(tagStart);
			nTagFieldEnd = int(tagEnd);
			bInTagField = true;
		}
	}

	return bInTagField;
}

void CEtUILineEditBox::PasteFromClipboard()
{
	DeleteSelectionText();

	if( !OpenClipboard( NULL ) )
		return;

	HANDLE handle = GetClipboardData( CF_UNICODETEXT );
	if( handle )
	{
		WCHAR *pwszText = ( WCHAR * )GlobalLock( handle );
		if( pwszText )
		{
			AddText( pwszText, true );
			GlobalUnlock( handle );
		}
	}

	CloseClipboard();
}

bool CEtUILineEditBox::IsMaxLine()
{
	float fHeight(0.0f);

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	if( iter != m_listBuffer.end() )
	{
		if( m_Property.UIType == UI_CONTROL_LINE_IMEEDITBOX )
		{
			fHeight = m_Property.LineIMEEditBoxProperty.fLineSpace;
		}
		else if( m_Property.UIType == UI_CONTROL_LINE_EDITBOX )
		{
			fHeight = m_Property.LineEditBoxProperty.fLineSpace;
		}

		fHeight += (*iter)->TextCoord.fHeight;
		fHeight *= ((int)m_listBuffer.size()+1);
	}

	if( m_Property.UICoord.fHeight < fHeight )
		return false;
	else
		return true;
}

int CEtUILineEditBox::GetLineMouseIn( float fX, float fY )
{
	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( int i=0; iter != m_listBuffer.end(); ++iter, i++ )
	{
		if( (*iter)->IsInside( fX, fY ) == true )
			return i;
	}

	return -1;
}

CEtUILineEditBox::SBufferInfo *CEtUILineEditBox::AddLine( SBufferInfo *pLine )
{
	if( !IsMaxLine() )
		return NULL;

	SBufferInfo *pBufferInfo = new SBufferInfo( /*m_pFont*/ );

	if( pLine == NULL )
	{
		m_listBuffer.push_front( pBufferInfo );
	}
	else
	{
		std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
		for( ; iter != m_listBuffer.end(); ++iter )
		{
			if( (*iter) == pLine )
			{
				iter = m_listBuffer.insert( ++iter, pBufferInfo );
				break;
			}
		}

		if (iter == m_listBuffer.end())
			SAFE_DELETE(pBufferInfo);
	}
	
	UpdateRects();
	return pBufferInfo;
}

bool CEtUILineEditBox::AppendLine( SBufferInfo *pLine )
{
	if( !IsMaxLine() )	return false;
	if( !pLine )		return false;

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		if( (*iter) == pLine )
		{
			SBufferInfo *pBufferInfo = new SBufferInfo( /*m_pFont*/ );

			SBufferInfo *pNextLine = pLine->pNextLine;
			if( pNextLine )
			{
				pBufferInfo->pNextLine = pNextLine;
				pNextLine->pPrevLine = pBufferInfo;
			}

			pLine->pNextLine = pBufferInfo;
			pBufferInfo->pPrevLine = pLine;

			m_listBuffer.insert( ++iter, pBufferInfo );
			UpdateRects();
			break;
		}
	}

	return true;
}

void CEtUILineEditBox::DeleteLine( SBufferInfo *pLine )
{
	if( !pLine )
		return;

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		SBufferInfo* pInfo = *iter;
		if( pInfo == pLine )
		{
			SBufferInfo *pPrevLine = pLine->pPrevLine;
			SBufferInfo *pNextLine = pLine->pNextLine;

			if( pPrevLine ) pPrevLine->pNextLine = pNextLine;
			if( pNextLine ) pNextLine->pPrevLine = pPrevLine;

			SAFE_DELETE(pInfo);
			m_listBuffer.erase( iter );
			UpdateRects();
			return;
		}
	}
}

void CEtUILineEditBox::UpCurrentLine()
{
	AdjustCaretPosition( GetLine( m_nCurLine ), GetLine( m_nCurLine-1) );

	m_nCurLine--;
	if( m_nCurLine < 0 )
	{
		m_nCurLine = 0;
	}
}

void CEtUILineEditBox::DownCurrentLine()
{
	AdjustCaretPosition( GetLine( m_nCurLine ), GetLine( m_nCurLine+1) );

	m_nCurLine++;
	if( m_nCurLine >= (int)m_listBuffer.size() )
	{
		m_nCurLine = (int)m_listBuffer.size()-1;
	}
}

void CEtUILineEditBox::AdjustCaretPosition( SBufferInfo *pCurLine, SBufferInfo *pNextLine )
{
	int nCurX(0), nNextX(0);
	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	SUIElement* pElement = GetElement( 0 );

	if( pCurLine )
	{
		nCurX = FontMng.GetCaretPos( pCurLine->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, pCurLine->nCaretPos );
		//pCurLine->CPtoX( pCurLine->nCaretPos, FALSE, &nCurX );
	}
	else
	{
		nCurX = INT_MAX;
	}

	if( pNextLine )
	{
		nNextX = FontMng.GetCaretPos( pNextLine->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, pNextLine->GetTextSize() );
		//pNextLine->CPtoX( pNextLine->GetTextSize(), FALSE, &nNextX );

		if( nCurX <= nNextX )
		{
			int nCaretPos, nTrail;
			nCaretPos = FontMng.GetCaretFromCaretPos( pNextLine->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nCurX, nTrail );
			//pNextLine->XtoCP( nCurX, &nCaretPos, &nTrail );
			PlaceCaret( pNextLine, nCaretPos );
		}
		else
		{
			PlaceCaretEnd( pNextLine );
		}
	}
}

bool CEtUILineEditBox::IsBeginLine( SBufferInfo *pLine )
{
	if( !pLine )
		return false;

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();

	if( pLine == (*iter) )
		return true;

	return false;
}

bool CEtUILineEditBox::IsEndLine( SBufferInfo *pLine )
{
	if( !pLine )
		return false;

	std::list<SBufferInfo*>::reverse_iterator iter = m_listBuffer.rbegin();

	if( pLine == (*iter) )
		return true;

	return false;
}

bool CEtUILineEditBox::IsEmpty()
{
	if( m_listBuffer.empty() )
		return true;

	std::wstring strTemp;
	bool bEmpty(true);

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( ; iter != m_listBuffer.end(); ++iter )
	{
		int nBufferSize = (*iter)->pUniBuffer->GetBufferSize();
		if( nBufferSize )
		{
			strTemp = (*iter)->pUniBuffer->GetBuffer();
			if( !strTemp.empty() )
			{
				bEmpty = false;
				break;
			}
		}
	}

	return bEmpty;
}

void CEtUILineEditBox::InsertCharLine( SBufferInfo *pLine, int nIndex, WCHAR wChar, bool bSetText, bool bLineFeed )
{
	if( !pLine )
		return;

	if (IsTextFull(1))
		return;

	if( !bSetText )
	{
		if( CheckTagTextInsertPos( pLine, nIndex ) )
			nIndex = pLine->nCaretPos;
	}

	if( pLine->InsertChar( nIndex, wChar ) )
	{
		if( !bSetText && !bLineFeed )
		{
			pLine->InsertOriginChar( GetOriginTextCaretPos( pLine, nIndex, false ), wChar );
			pLine->nOriginCaretPos++;
		}

		PlaceCaret( pLine, pLine->nCaretPos + 1 );
		m_nSelEndPos = m_nSelStartPos = pLine->nCaretPos;
		m_nOriginSelEndPos = m_nOriginSelStartPos = pLine->nOriginCaretPos;
	}

	int nX;
	int nLastCP = pLine->GetTextSize();
	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	SUIElement* pElement = GetElement( 0 );

	nX = FontMng.GetCaretPos( pLine->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nLastCP );
	int nTextWidth = ( int )( pLine->TextCoord.fWidth * m_pParent->GetScreenWidth() );

	if( nX > nTextWidth )
	{
		if( IsEndLine(pLine) || (!pLine->pNextLine) )
		{
			if( AppendLine( pLine ) == false )
			{
				// Note : 더 이상 추가할 수 없다면 바로 전에 추가한 문자를 삭제하고 캐럿의 위치를 조정해준다.
				pLine->RemoveChar( nLastCP-1 );
				pLine->RemoveOriginChar( GetOriginTextCaretPos( pLine, nLastCP - 1, false ) );
				PlaceCaret( pLine, pLine->nCaretPos - 1 );
				return;
			}
		}

		if( nLastCP == pLine->nCaretPos )
		{
			PlaceCaret( pLine, pLine->nCaretPos - 1 );

			if( GetCurrentLine() == pLine )
			{
				DownCurrentLine();
				GetLine( m_nCurLine )->nCaretPos = 0;
			}
		}

		int nTagFieldStart, nTagFieldEnd;
		if( IsInTagField( pLine, nLastCP - 1, nTagFieldStart, nTagFieldEnd ) )
		{
			nLastCP = GetTextCaretPos( pLine, nTagFieldStart - 1 ) + 1;
		}

		if( nLastCP - 1 >=0 && nLastCP - 1 < pLine->pUniBuffer->GetBufferSize() )
		{
			WCHAR wCharInner = (*(pLine->pUniBuffer))[nLastCP - 1];
			pLine->RemoveChar( nLastCP - 1 );
			pLine->RemoveOriginChar( GetOriginTextCaretPos( pLine, nLastCP - 1, false ) );

			InsertCharLine( pLine->pNextLine, 0, wCharInner, false, false );
		}
	}
}

bool CEtUILineEditBox::RemoveCharLine( SBufferInfo *pLine, int nIndex )
{
	if( !pLine )
		return false;

	bool bRet = pLine->RemoveChar( nIndex );

	if (bRet == false)
	{
		return false;
	}

	if( pLine->pNextLine )
	{
		PasteTrailFromLine( pLine, pLine->pNextLine );
	}
	else
	{
		if( (pLine->GetTextSize() == 0) && ( GetCurrentLine() != pLine ) )
		{
			DeleteLine( pLine );
		}
	}
	
	return true;
}

bool CEtUILineEditBox::RemoveOriginCharLine( SBufferInfo* pBufferInfo, int nIndex )
{
	if( !pBufferInfo ) 
		return false;

	bool bRet = pBufferInfo->RemoveOriginChar( nIndex );

	return bRet;
}

CEtUILineEditBox::SBufferInfo *CEtUILineEditBox::GetLine( int nLineIndex )
{
	if( nLineIndex < 0 )
		return NULL;

	std::list<SBufferInfo*>::iterator iter = m_listBuffer.begin();
	for( int i=0; iter != m_listBuffer.end(); ++iter, ++i )
	{
		if( i >= nLineIndex )
		{
			return (*iter);
		}
	}

	return NULL;
}

void CEtUILineEditBox::SetCurrentLine( int nLineIndex )
{
	if( nLineIndex < 0 || nLineIndex >= (int)m_listBuffer.size() )
	{
		_ASSERT(0);
		return;
	}
	m_nCurLine = nLineIndex;
}

CEtUILineEditBox::SBufferInfo *CEtUILineEditBox::GetCurrentLine()
{
	if( m_nCurLine < 0 )
		return NULL;

	return GetLine( m_nCurLine );
}

void CEtUILineEditBox::PasteTrailFromLine( SBufferInfo *pDestLine, SBufferInfo *pSrcLine )
{
	if (pDestLine == NULL || pSrcLine == NULL)
	{
		_ASSERT(0);
		return;
	}

	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	SUIElement* pElement = GetElement( 0 );

	pDestLine->pNextLine = pSrcLine;
	pSrcLine->pPrevLine = pDestLine;

	if( pDestLine->IsEndCaret() && (pSrcLine->GetTextSize() == 0) )
	{
		pDestLine->pNextLine = pSrcLine->pNextLine;
		DeleteLine( pSrcLine );
		return;
	}

	int i(0), nX(0), nTextWidth(0);

	// Note : 삭제된 공간에 들어갈 수 있을때까지 문자를 입력한다. 글자마다 차지하는 공간이 다르기 때문...
	while(1)
	{
		if( i >= pSrcLine->GetTextSize() )
		{
			break;
		}

		WCHAR wChar = (*(pSrcLine->pUniBuffer))[i++];
		int nTagFieldStart, nTagFieldEnd;
		bool bInTagField = IsInTagField( pDestLine, pDestLine->GetTextSize(), nTagFieldStart, nTagFieldEnd );
		pDestLine->InsertChar( pDestLine->GetTextSize(), wChar );
		if( !bInTagField )
			pDestLine->InsertOriginChar( pDestLine->GetOriginTextSize(), wChar );

		int nLastCP = pDestLine->GetTextSize();
		//pDestLine->CPtoX( nLastCP, FALSE, &nX );
		nX = FontMng.GetCaretPos( pDestLine->pUniBuffer->GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nLastCP );
		nTextWidth = ( int )( pDestLine->TextCoord.fWidth * m_pParent->GetScreenWidth() );

		if( nX > nTextWidth )
		{
			// Note : 마지막에 넘어선 문자는 제거한다.
			pDestLine->RemoveChar( pDestLine->GetTextSize()-1 );
			if( !bInTagField )
				pDestLine->RemoveOriginChar( pDestLine->GetOriginTextSize()-1 );
			i--;
			break;
		}
	}

	// Note : 앞 라인에 추가된 개수만큼 삭제를 시켜준다.
	for( int j=0; j<i; j++ )
	{
		RemoveCharLine( pSrcLine, 0 );
	}
}

void CEtUILineEditBox::SetSelectStartPos()
{
	SBufferInfo *pBufferInfo = GetCurrentLine();
	if( pBufferInfo )
	{
		m_nSelEndPos = m_nSelStartPos = pBufferInfo->nCaretPos;
		m_nOriginSelEndPos = m_nOriginSelStartPos = pBufferInfo->nOriginCaretPos;
		m_nSelEndLine = m_nSelStartLine = m_nCurLine;
	}
}

void CEtUILineEditBox::SetSelectEndPos()
{
	SBufferInfo *pBufferInfo = GetCurrentLine();
	if( pBufferInfo )
	{
		m_nSelEndPos = pBufferInfo->nCaretPos;
		m_nOriginSelEndPos = pBufferInfo->nOriginCaretPos;
		m_nSelEndLine = m_nCurLine;
	}
}

void CEtUILineEditBox::SplitLineToAdd( SBufferInfo *pLine )
{
	if( pLine == NULL )
		return;

	int nTagFieldStart, nTagFieldEnd;
	if( IsInTagField( pLine, pLine->nCaretPos, nTagFieldStart, nTagFieldEnd ) )
		return;

	std::wstring strTemp( pLine->pUniBuffer->GetBuffer() );
	std::wstring strOriginTemp( pLine->pOriginUniBuffer->GetBuffer() );

	pLine->pUniBuffer->Clear();
	pLine->pUniBuffer->SetText( strTemp.substr( 0, pLine->nCaretPos ).c_str() );
	pLine->pOriginUniBuffer->Clear();
	pLine->pOriginUniBuffer->SetText( strOriginTemp.substr( 0, pLine->nOriginCaretPos ).c_str() );

	SBufferInfo *pAddLine = AddLine( pLine );
	if( pAddLine )
	{
		pAddLine->pUniBuffer->SetText( strTemp.substr( pLine->nCaretPos ).c_str() );
		pAddLine->pOriginUniBuffer->SetText( strOriginTemp.substr( pLine->nOriginCaretPos ).c_str() );

		if( pLine->pNextLine )
		{
			PasteTrailFromLine( pAddLine, pLine->pNextLine );
			pLine->pNextLine = NULL;
		}

		DownCurrentLine();
		PlaceCaretBegin( pAddLine );
	}
}

void CEtUILineEditBox::SetMaxEditLength(DWORD dwMaxLength)
{
	ASSERT( dwMaxLength > 0 );
	m_nMaxTextLength = dwMaxLength;
}

int CEtUILineEditBox::GetTextLength() const
{
	int wholeTextLength = 0;
	std::list<SBufferInfo*>::const_iterator iter = m_listBuffer.begin();
	for(; iter != m_listBuffer.end(); ++iter)
		wholeTextLength += (*iter)->pUniBuffer->GetTextSize();

	return wholeTextLength;
}

bool CEtUILineEditBox::IsTextFull(int textAddedLength) const
{
	int wholeTextLength = GetTextLength();
	if (wholeTextLength + textAddedLength < m_nMaxTextLength)
		return false;

	return true;
}