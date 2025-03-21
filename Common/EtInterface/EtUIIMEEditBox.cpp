#include "StdAfx.h"
#include "EtUIIMEEditBox.h"
#include "EtUIDialog.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtUIIMEEditBox::CEtUIIMEEditBox( CEtUIDialog *pParent )
	: CEtUIEditBox( pParent ), m_bFirstVisibleOver( false )
{
	m_Property.UIType = UI_CONTROL_IMEEDITBOX;

	m_Property.IMEEditBoxProperty.fIndicatorSize = 0.0f;
	m_dwCompTargetColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
	m_dwCompTargetBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );
	m_dwCompTargetNonColor = D3DCOLOR_ARGB( 255, 255, 255, 0 );
	m_dwCompTargetNonBkColor = D3DCOLOR_ARGB( 255, 150, 150, 150 );

	m_bForceBottomPosition = false;
	m_nTempCompSize = 0;
}

CEtUIIMEEditBox::~CEtUIIMEEditBox(void)
{
}

void CEtUIIMEEditBox::Initialize( SUIControlProperty *pProperty )
{
	CEtUIEditBox::Initialize( pProperty );
	UpdateRects();
}

void CEtUIIMEEditBox::SetTemplate( int nTemplateIndex )
{
	CEtUIEditBox::SetTemplate( nTemplateIndex );

	if( m_Property.IMEEditBoxProperty.dwMaxChars == 0 ) 
		m_Property.IMEEditBoxProperty.dwMaxChars = 256;

	SetMaxEditLength( m_Property.IMEEditBoxProperty.dwMaxChars );
}

bool CEtUIIMEEditBox::MsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() ||  !IsKeyLock() )
	{
		return false;
	}

#if defined(DEBUG) || defined(_DEBUG)
	assert( m_bIMEStaticMsgProcCalled && L"To fix, call CEtUIDialogResourceManager::MsgProc() first" );
#endif

	static LPARAM lAlt = 0x80000000, lCtrl = 0x80000000, lShift = 0x80000000;
	bool bpbTrappedData, *pbTrapped;
	HIMC hImc;

	pbTrapped = &bpbTrappedData;
	*pbTrapped = false;
	if( !s_bEnableImeSystem )
	{
		return CEtUIEditBox::MsgProc( uMsg, wParam, lParam );
	}

	// 참고자료.
	// IME 조합중에 오는 WM_KEYDOWN 의 wParam을 보면 항상 VK_PROCESSKEY다.
	// 원래의 vk값이 필요하다면, ImmGetVirtualKey함수를 통해 얻으면 되는데,
	// TranslateMessage가 호출되지 않은 상태여야만 한다.
	// 그런데 아래와 같은 코드는 백스페이스 제외하고 나머지는 디폴트 프로시저로 안보내서 IME처리가 안되게 하려는 코드처럼 보이겠지만,
	// 이렇게 해봤자 IME는 내부적으로 다 메세지를 처리하고 있다.(즉, 조합되고 있다.)
	// 이런 식으로 메세지를 무시하는 방법으로는 조합을 막을 수 없는 거 같다.
	//if( m_bFirstVisibleOver )
	//{
	//	if( uMsg == WM_KEYDOWN && wParam == VK_PROCESSKEY )
	//	{
	//		UINT nKey = ImmGetVirtualKey( m_pParent->GetHWnd() );
	//		if( nKey == VK_BACK )
	//			m_bFirstVisibleOver = false;
	//		else
	//			return true;
	//	}
	//}

	switch( uMsg )
	{
	case WM_IME_COMPOSITION:
		{
			// Note : m_Buffer가 static 변수가 아니라서 이 부분에서 체크를 한다. 
			//		최대 길이라면 더 이상 문자 조합을 하지 않는다.
			//
			if( m_Buffer.IsMaxEditBoxLength() )
			{
				PostMessage( GetFocus(), WM_IME_ENDCOMPOSITION, 0, 0 );
				break;
			}

			// Note: 조합중인 문자열이 너무 길어서 완성된 문자열을 밀어내서 하나도 보이지 않게 되는 경우를 막는다.
			// 안그러면 m_nFirstVisible 값이 문자열 길이를 넘어가게 되어버림.
			// 백 스페이스를 제외한 글자가 추가되는 경우에만 체크한다. 안그러면 꽉찬 상황에서 지워지지도 않아서 먹통됨.
			if( m_bFirstVisibleOver )
			{
				HIMC hImc;
				hImc = _ImmGetContext( m_pParent->GetHWnd() );
				_ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
				_ImmReleaseContext( m_pParent->GetHWnd(), hImc );
				m_bFirstVisibleOver = false;
				break;

				// #14215 이슈(m_bFirstVisibleOver 상태가 되었을때 위에처럼 리셋하지 말고 유지되게 하기)를 해결하기 위해 여러가지로 시도를 했었다.
				// 1. 우선 이렇게 리셋되는 이유가 m_bFirstVisibleOver 상태로 바꾸는 거 자체가 별로라 생각되서,
				// m_bFirstVisibleOver를 true로 만드는 PlaceCaret 검사부분에서 ImmSetCompositionString을 호출해 조합중인 글자의 길이를 조절하려 했으나
				// 아무리 설정해도 함수가 FALSE를 리턴한다.
				// 구글에서 찾아봐도 안된다고 하는 사람은 많은데, 어떻게 고쳐야된다고 적어둔건 하나도 못찾았다.
				// 2. ImmProcessKey나 ImmLockIMC를 이용해 사이즈 제한을 두려 했으나, 함수콜이 실패해서 안됨.
				// 3. 여러 함수들이 제대로 작동하지 않아서 메세지 자체를 막기로 했다.
				// 그래서 WM_IME_COMPOSITION 을 디폴트 프로시저로 안보내 무시하려고 했으나,
				// 디폴트 프로시저로 가지 않더라도 IME가 내부적으로 메세지를 따로 받기때문에 조합Result를 얻어보면 계속 조합되고 있는 상태의 스트링이 전달되었다.
				// 즉, 메세지를 안보내는 방법으로도 조합 맥스길이 조절 실패.
				// (디폴트 프로시저로 IME메세지를 안보내도 내부적으로 처리되고 있다는 걸 확인하고 싶다면,
				//  m_bFirstVisibleOver 일때 return true 해놓고, 쭉 입력해보면, 조합윈도우에 더이상 문자가 추가되지 않으나,
				//  알트탭 한번 했다오면, wm_paint로 갱신되면서 눌렀던 글자들이 모두 조합되있는것을 확인할 수 있다.)
				// 4. 그래서 계속 찾아보니 키보드 후킹을 통해서만, IME로 가는 메세지를 차단할 수 있었다고 한다.
				// 5. 대부분의 경우엔 조합중인 글자에 대해서는 길이제한을 두지 않고, 조합 Result로 올때 길이제한을 건다고 한다.
				//
				// 단지 길이제한을 위해서, 키보드후킹까지 쓰는건 좀 별로인거 같아서
				// 좋은 개선안이 생기기 전까지 우선 현재 방법대로 유지하도록 하겠다. 
				// 아니면 그냥 현재까지 조합된 글자를 강제로 Result로 내보내던지..
			}

			LONG lRet;  
			WCHAR wszCompStr[ MAX_COMPSTRING_SIZE ]={0};

			*pbTrapped = true;
			hImc = _ImmGetContext( m_pParent->GetHWnd() );
			if( hImc == NULL )
			{
				break;
			}

			if( lParam & GCS_CURSORPOS )
			{
				s_nCompCaret = _ImmGetCompositionStringW( hImc, GCS_CURSORPOS, NULL, 0 );
				if( s_nCompCaret < 0 )
				{
					s_nCompCaret = 0;
				}
			}

			if( lParam & GCS_RESULTSTR )
			{
				lRet = _ImmGetCompositionStringW( hImc, GCS_RESULTSTR, wszCompStr, sizeof( wszCompStr ) );
				if( lRet > 0 )
				{
					lRet /= sizeof( WCHAR );
					wszCompStr[ lRet ] = 0;
					TruncateCompString( false, ( int )wcslen( wszCompStr ) );
					s_CompString.SetText( wszCompStr );

					if( CEtUIIME::s_bRESULTSTR_NotSendComp == false )
					{
						SendCompString();
					}
					else
					{
						GetEtDevice()->SetDialogBoxMode(false);
						CEtUIIME::s_bRESULTSTR_NotSendComp = true;
					}

					ResetCompositionString();
				}
			}

			if( lParam & GCS_COMPSTR )
			{
				lRet = _ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
				if( lRet > 0 )
				{
					int i;

					lRet /= sizeof( WCHAR );
					wszCompStr[ lRet ] = 0;
					TruncateCompString( false, ( int )wcslen( wszCompStr ) );
					s_CompString.SetText( wszCompStr );
					m_pParent->ProcessCommand(EVENT_EDITBOX_IME_CHANGE, true, this, WM_IME_COMPOSITION);
					
					if( ( GetLanguage() == LANG_CHT ) && ( !GetImeId() ) )
					{
						if( lstrlenW( s_CompString.GetBuffer() ) )
						{
							s_CandList.dwCount = 4;             
							s_CandList.dwSelection = ( DWORD ) - 1;
							for( i = 3; i >= 0; i-- )
							{
								if( i > lstrlenW( s_CompString.GetBuffer() ) - 1 )
								{
									s_CandList.awszCandidate[ i ][ 0 ] = 0;
								}
								else
								{
									s_CandList.awszCandidate[ i ][ 0 ] = s_CompString[ i ];
									s_CandList.awszCandidate[ i ][ 1 ] = 0;
								}
							}
							s_CandList.dwPageSize = MAX_CANDLIST;
							memset( ( void * )s_CompString.GetBuffer(), 0, sizeof( WCHAR ) * 4  );
							s_bShowReadingWindow = true;
							GetReadingWindowOrientation( 0 );
							if( s_bHorizontalReading )
							{
								s_CandList.nReadingError = -1;
								s_wszReadingString[ 0 ] = 0;
								for( i = 0; i < ( int )s_CandList.dwCount; i++ )
								{
									if( s_CandList.dwSelection == i )
									{
										s_CandList.nReadingError = lstrlenW( s_wszReadingString );
									}
									wcscat_s( s_wszReadingString, 32, s_CandList.awszCandidate[ i ] );
								}
							}
						}
						else
						{
							s_CandList.dwCount = 0;
							s_bShowReadingWindow = false;
						}
					}

					if( s_bInsertOnType )
					{
						int nCount;

						SendCompString();
						nCount = lstrlenW( s_CompString.GetBuffer() + s_nCompCaret );
						for( i = 0; i < nCount; i++ )
						{
							SendMessage( m_pParent->GetHWnd(), WM_KEYDOWN, VK_LEFT, 0 );
						}
						SendMessage( m_pParent->GetHWnd(), WM_KEYUP, VK_LEFT, 0 );
					}
				}

				ResetCaretBlink();

				// Note : 한글조합중에 문자열이 컨트롤의 크기를 벗어나는 것을 조정한다.
				//		현재는 한글만 테스트 했고 이 후에 중국, 일본의 경우도 체크해야 한다.
				//
				//		위치는 여기가 되야한다.
				PlaceCaret( (int)lRet );
			}

			if( lParam & GCS_COMPATTR )
			{
				lRet = _ImmGetCompositionStringW( hImc, GCS_COMPATTR, s_abyCompStringAttr, sizeof( s_abyCompStringAttr ) );
				if( lRet > 0 )
				{
					s_abyCompStringAttr[lRet] = 0;
				}
			}

			if( CEtUIIME::s_bProcess_GCS_COMPCLAUSE )
			{
				if( lParam & GCS_COMPCLAUSE )
				{
					lRet = _ImmGetCompositionStringW( hImc, GCS_COMPCLAUSE, s_adwCompStringClause, sizeof( s_adwCompStringClause ) );
					s_adwCompStringClause[ lRet / sizeof( DWORD ) ] = 0;
				}
			}

			if( CEtUIIME::s_bRESULTSTR_NotSendComp == true )
			{
				// 컴포지션 윈도우 위치 조정.
				float fX1 = CEtUIIMEEditBox::s_CurrentCaretPos.fX * m_pParent->GetScreenWidth();
				float fX2 = CEtUIIMEEditBox::s_CurrentCaretPos.fY * m_pParent->GetScreenHeight();

				RECT rcWindow;
				GetWindowRect( m_pParent->GetHWnd(), &rcWindow );

				POINT pt;
				pt.x = (long)fX1 - rcWindow.left;
				pt.y = (long)fX2 - rcWindow.top;

				// 풀스크린때는 윈도우 캡션을 고려하지 않고 그 자리에 렌더링해 원래 입력창이 가려진다.
				// 그래서 강제로 20픽셀 내리도록 하겠다.
				if( !(GetWindowLong( m_pParent->GetHWnd(), GWL_STYLE ) & WS_CAPTION) )
					pt.y += 20;

				ClientToScreen(m_pParent->GetHWnd(), &pt);

				COMPOSITIONFORM cf;
				cf.dwStyle = CFS_POINT;
				cf.ptCurrentPos.x = pt.x;
				cf.ptCurrentPos.y = pt.y;
				ImmSetCompositionWindow(hImc, &cf);
			}

			_ImmReleaseContext( m_pParent->GetHWnd(), hImc );

			if( CEtUIIME::s_bRESULTSTR_NotSendComp == true )
			{
				return CEtUIEditBox::MsgProc( uMsg, wParam, lParam );
			}
		}
		break;

	case WM_IME_ENDCOMPOSITION:
		TruncateCompString();
		ResetCompositionString();
		s_bHideCaret = false;
		s_bShowReadingWindow = false;
		m_pParent->ProcessCommand(EVENT_EDITBOX_IME_CHANGE, true, this, WM_IME_ENDCOMPOSITION);
		break;

	case WM_IME_NOTIFY:
		switch( wParam )
		{
		case IMN_SETCONVERSIONMODE:
		case IMN_SETOPENSTATUS:
			CheckToggleState();
			break;

		case IMN_OPENCANDIDATE:
		case IMN_CHANGECANDIDATE:
			{
				int i, j;
				LPCANDIDATELIST lpCandList;
				DWORD dwLenRequired;

				s_CandList.bShowWindow = true;
				*pbTrapped = true;
				hImc = _ImmGetContext( m_pParent->GetHWnd() );
				if( hImc == NULL )
				{
					break;
				}
				lpCandList = NULL;
				s_bShowReadingWindow = false;
				dwLenRequired = _ImmGetCandidateListW( hImc, 0, NULL, 0 );
				if( dwLenRequired )
				{
					lpCandList = ( LPCANDIDATELIST )HeapAlloc( GetProcessHeap(), 0, dwLenRequired );
					dwLenRequired = _ImmGetCandidateListW( hImc, 0, lpCandList, dwLenRequired );
				}

				if( lpCandList )
				{
					int nPageTopIndex;

					s_CandList.dwSelection = lpCandList->dwSelection;
					s_CandList.dwCount = lpCandList->dwCount;
					nPageTopIndex = 0;
					s_CandList.dwPageSize = min( lpCandList->dwPageSize, MAX_CANDLIST );
					if( GetPrimaryLanguage() == LANG_JAPANESE )
					{
						nPageTopIndex = ( s_CandList.dwSelection / s_CandList.dwPageSize ) * s_CandList.dwPageSize;
					}
					else
					{
						nPageTopIndex = lpCandList->dwPageStart;
					}

					if( GetLanguage() == LANG_CHS && !GetImeId() )
					{
						s_CandList.dwSelection = ( DWORD ) - 1;
					}
					else
					{
						s_CandList.dwSelection = s_CandList.dwSelection - nPageTopIndex;
					}

					memset( s_CandList.awszCandidate, 0, sizeof(s_CandList.awszCandidate) );
					for( i = nPageTopIndex, j = 0; i < ( int )lpCandList->dwCount && j < ( int )s_CandList.dwPageSize; i++, j++ )
					{
						LPWSTR pwsz;
						WCHAR *pwszNewCand;

						pwsz = s_CandList.awszCandidate[ j ];
						*pwsz = ( WCHAR )( L'0' + ( ( j + 1 ) % 10 ) );
						pwsz++;
						if( s_bVerticalCand )
						{
							*pwsz = L' ';
							pwsz++;
						}
						pwszNewCand = ( LPWSTR )( ( BYTE * )lpCandList + lpCandList->dwOffset[ i ] );
						while( *pwszNewCand )
						{
							*pwsz = *pwszNewCand;
							pwsz++;
							pwszNewCand++;
						}
						if( !s_bVerticalCand )
						{
							*pwsz = L' ';
							pwsz++;
						}
						*pwsz = 0;
					}

					s_CandList.dwCount = lpCandList->dwCount - lpCandList->dwPageStart;
					if( s_CandList.dwCount > lpCandList->dwPageSize )
					{
						s_CandList.dwCount = lpCandList->dwPageSize;
					}

					HeapFree( GetProcessHeap(), 0, lpCandList );
					_ImmReleaseContext( m_pParent->GetHWnd(), hImc );

					if( GetPrimaryLanguage() == LANG_KOREAN || GetLanguage() == LANG_CHT && !GetImeId() )
					{
						s_CandList.dwSelection = ( DWORD ) - 1;
					}

					if( !s_bVerticalCand )
					{
						WCHAR wszCand[ 256 ] = L"";

						s_CandList.nFirstSelected = 0;
						s_CandList.nHoriSelectedLen = 0;
						for( i = 0; i < MAX_CANDLIST; i++ )
						{
							WCHAR wszEntry[ 32 ];

							if( s_CandList.awszCandidate[ i ][ 0 ] == L'\0' )
							{
								break;
							}
							swprintf_s( wszEntry, 32, L"%s ", s_CandList.awszCandidate[ i ] );
							if( i == s_CandList.dwSelection )
							{
								s_CandList.nFirstSelected = lstrlenW( wszCand );
								s_CandList.nHoriSelectedLen = lstrlenW( wszEntry ) - 1;
							}
							wcscat_s( wszCand, 256, wszEntry );
						}
						wszCand[ lstrlenW( wszCand ) - 1 ] = L'\0';
						s_CandList.HoriCand.SetText( wszCand );
					}
				}
				break;
			}

		case IMN_CLOSECANDIDATE:
			{
				s_CandList.bShowWindow = false;
				if( !s_bShowReadingWindow )
				{
					s_CandList.dwCount = 0;
					memset( s_CandList.awszCandidate, 0, sizeof( s_CandList.awszCandidate ) );
				}
				*pbTrapped = true;
				break;
			}

		case IMN_PRIVATE:
			{
				DWORD dwId;

				if( !s_CandList.bShowWindow )
				{
					GetPrivateReadingString();
				}

				dwId = GetImeId();
				switch( dwId )
				{
					case IMEID_CHT_VER42:
					case IMEID_CHT_VER43:
					case IMEID_CHT_VER44:
					case IMEID_CHS_VER41:
					case IMEID_CHS_VER42:
						if( ( lParam == 1 ) || ( lParam == 2 ) )
						{
							*pbTrapped = true;
						}
						break;

					case IMEID_CHT_VER50:
					case IMEID_CHT_VER51:
					case IMEID_CHT_VER52:
					case IMEID_CHT_VER60:
					case IMEID_CHS_VER53:
						if( (lParam == 16) || (lParam == 17) || (lParam == 26) || (lParam == 27) || (lParam == 28) )
						{
							*pbTrapped = true;
						}
						break;
				}
			}
			break;

		default:
			*pbTrapped = true;
			break;
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		if( !( lAlt & 0x80000000 ) && wParam != VK_MENU && ( GetAsyncKeyState( VK_MENU ) & 0x8000 ) == 0 )
		{
			PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_MENU, ( lAlt & 0x01ff0000 ) | 0xC0000001 );
		}   
		else if( !( lCtrl & 0x80000000 ) && wParam != VK_CONTROL && ( GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) == 0 )
		{
			PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_CONTROL, ( lCtrl & 0x01ff0000 ) | 0xC0000001 );
		}
		else if( !( lShift & 0x80000000 ) && wParam != VK_SHIFT && ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) == 0 )
		{
			PostMessageW( GetFocus(), WM_KEYUP, (WPARAM)VK_SHIFT, ( lShift & 0x01ff0000 ) | 0xC0000001 );
		}
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		switch ( wParam )
		{
			case VK_MENU:
				lAlt = lParam;
				break;
			case VK_SHIFT:
				lShift = lParam;
				break;
			case VK_CONTROL:
				lCtrl = lParam;
				break;
		}

	default:
		return CEtUIEditBox::MsgProc( uMsg, wParam, lParam );

	}  

	return *pbTrapped;
}

bool CEtUIIMEEditBox::HandleMouse( UINT uMsg, float fX, float fY, WPARAM wParam, LPARAM lParam )
{
	if( !IsEnable() || !IsShow() || !IsKeyLock() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			float fFontHeight = m_pParent->GetFontHeight( m_Template.m_vecElement[5].nFontIndex, m_Template.m_vecElement[5].nFontHeight );
			//ID3DXFont *pFont = m_pParent->GetFont( m_Template.m_vecElement[5].nFontIndex, m_Template.m_vecElement[5].nFontHeight );
			int nCompStrWidth = 0;
			CEtFontMng& FontMng = CEtFontMng::GetInstance();
			SUIElement* pElement = GetElement( 0 );
			nCompStrWidth = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_CompString.GetTextSize() );
			//s_CompString.CPtoX( s_CompString.GetTextSize(), FALSE, &nCompStrWidth );

			if( ( s_fCompStringX <= fX ) && ( s_fCompStringY <= fY ) &&
				( s_fCompStringX + nCompStrWidth / m_pParent->GetScreenWidth() > fX ) && ( s_fCompStringY + fFontHeight > fY ) )
			{
				HIMC hImc;
				int nCharBodyHit, nCharHit;
				int nTrail;

				nCharBodyHit = FontMng.GetCaretFromCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 
														   (int)((fX - s_fCompStringX) * m_pParent->GetScreenWidth()), nTrail );
				//s_CompString.XtoCP( ( int )( ( fX - s_fCompStringX ) * m_pParent->GetScreenWidth() ), &nCharBodyHit, &nTrail );
				if( ( nTrail ) && ( nCharBodyHit < s_CompString.GetTextSize() ) )
				{
					nCharHit = nCharBodyHit + 1;
				}
				else
				{
					nCharHit = nCharBodyHit;
				}

				hImc = _ImmGetContext( m_pParent->GetHWnd() );
				if( !hImc )
				{
					return true;
				}

				_ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 );
				_ImmReleaseContext( m_pParent->GetHWnd(), hImc );

				switch( GetPrimaryLanguage() )
				{
				case LANG_JAPANESE:
					if( s_nFirstTargetConv != -1 )
					{
						int nClauseClicked, nClauseSelected, nSendCount;
						BYTE byVirtKey;

						nClauseClicked = 0;
						while( ( int )s_adwCompStringClause[ nClauseClicked + 1 ] <= nCharBodyHit )
						{
							nClauseClicked++;
						}

						nClauseSelected = 0;
						while( ( int )s_adwCompStringClause[ nClauseSelected + 1 ] <= s_nFirstTargetConv )
						{
							nClauseSelected++;
						}

						if( nClauseClicked > nClauseSelected )
						{
							byVirtKey = VK_RIGHT;
						}
						else
						{
							byVirtKey = VK_LEFT;
						}
						nSendCount = abs( nClauseClicked - nClauseSelected );
						while( nSendCount-- > 0 )
						{
							SendKey( byVirtKey );
						}
						return true;
					}
				case LANG_CHINESE:
					{
						BYTE byVirtKey;
						int nSendCount;

						if( nCharHit > s_nCompCaret )
						{
							byVirtKey = VK_RIGHT;
						}
						else
						{
							byVirtKey = VK_LEFT;
						}
						nSendCount = abs( nCharHit - s_nCompCaret );
						while( nSendCount-- > 0 )
						{
							SendKey( byVirtKey );
						}
						break;
					}
				}
				return true;
			}

			if( ( s_CandList.bShowWindow ) && ( s_CandList.CandidateCoord.IsInside( fX, fY ) ) )
			{
				if( s_bVerticalCand )
				{
					int nRow;

					nRow = ( int )( ( fY - s_CandList.CandidateCoord.fY ) / fFontHeight );

					if( nRow < ( int )s_CandList.dwCount )
					{
						switch( GetPrimaryLanguage() )
						{
							case LANG_CHINESE:
							case LANG_KOREAN:
								SendKey( ( BYTE ) ( '0' + nRow + 1 ) );
								break;

							case LANG_JAPANESE:
							{
								BYTE byVirtKey;
								int nNumToHit, nStrike;

								if( nRow > ( int )s_CandList.dwSelection )
								{
									byVirtKey = VK_DOWN;
								}
								else
								{
									byVirtKey = VK_UP;
								}
								nNumToHit = abs( ( int) ( nRow - s_CandList.dwSelection ) );
								for( nStrike = 0; nStrike < nNumToHit; nStrike++ )
								{
									SendKey( byVirtKey );
								}
								SendKey( VK_RIGHT );
								SendKey( VK_LEFT );
								break;
							}
						}
					}
				} 
				else
				{
					int i, nCharHit, nTrail, nCandidate, nEntryStart;

					nCharHit = FontMng.GetCaretFromCaretPos( s_CandList.HoriCand.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 
														   (int)((fX - s_CandList.CandidateCoord.fX) * m_pParent->GetScreenWidth()), nTrail );
					//s_CandList.HoriCand.XtoCP( ( int )( ( fX - s_CandList.CandidateCoord.fX ) * m_pParent->GetScreenWidth() ), &nCharHit, &nTrail );

					nCandidate = s_CandList.dwCount - 1;
					nEntryStart = 0;
					for( i = 0; i < ( int )s_CandList.dwCount; i++ )
					{
						if( nCharHit >= nEntryStart )
						{
							nEntryStart += lstrlenW( s_CandList.awszCandidate[ i ] ) + 1;
						} 
						else
						{
							nCandidate = i - 1;
							break;
						}
					}

					switch( GetPrimaryLanguage() )
					{
						case LANG_CHINESE:
						case LANG_KOREAN:
							SendKey( ( BYTE ) ( '0' + nCandidate + 1 ) );
							break;
					}
				}
				return true;
			}
		}
	}

	return CEtUIEditBox::HandleMouse( uMsg, fX, fY, wParam, lParam );
}

void CEtUIIMEEditBox::Focus( bool fFocus )
{
	CEtUIEditBox::Focus( fFocus );

	// 중국 일부 IME의 경우 강제로 아래처럼 막아주지 않으면
	// 디폴트프로시저로 들어오는 영문자들 다 받아채서 캔디데이트를 띄워버린다.
	// 
	// 포커스가 없을때 한영키 전환이 안된다는 것때문에 국내빌드에선 사용하지 않는 것일지도 모르겠지만,
	// 해외빌드에선 제공되는 IME를 사용해야하고, 포커스가 없을때도 뜨는 캔디데이트를 방지하기 위해, 아래 코드를 사용해야만 한다.
	//
	// EtInterface에서는 국가별 디파인이 없으므로 static bool 값으로 외부에서 설정받아 사용한다.
	if( CEtUIIME::s_bDisableIMEonKillFocus )
	{
		if( fFocus )
		{
			HIMC hImc;

			if( s_bEnableImeSystem )
			{
				_ImmAssociateContext( m_pParent->GetHWnd(), s_hImcDef );
				CheckToggleState();
			} 
			else
			{
				_ImmAssociateContext( m_pParent->GetHWnd(), NULL );
			}
			hImc = _ImmGetContext( m_pParent->GetHWnd() );
			if( hImc != NULL ) 
			{
				if( !s_bEnableImeSystem )
				{
					s_ImeState = IMEUI_STATE_OFF;
				}
				_ImmReleaseContext( m_pParent->GetHWnd(), hImc );
				CheckToggleState();
			}
		}
		else
		{
			FinalizeString( false );
			_ImmAssociateContext( m_pParent->GetHWnd(), NULL );
		}
	}
}

void CEtUIIMEEditBox::PumpMessage()
{
	MSG msg;

	while( PeekMessageW( &msg, NULL, 0, 0, PM_NOREMOVE ) )
	{
		if( !GetMessageW( &msg, NULL, 0, 0 ) )
		{
			PostQuitMessage( (int)msg.wParam );
			return;
		}
		TranslateMessage( &msg );
		DispatchMessageA( &msg );
	}
}

void CEtUIIMEEditBox::UpdateRects()
{
	float fTempWidth;

	fTempWidth = m_Property.UICoord.fWidth;
	if( m_Property.IMEEditBoxProperty.fIndicatorSize > 0.0f )
	{
		m_Property.UICoord.fWidth -= m_Property.IMEEditBoxProperty.fIndicatorSize + m_Property.IMEEditBoxProperty.fBorder * 2;
	}

	CEtUIEditBox::UpdateRects();

	m_IndicatorCoord.SetCoord( m_Property.UICoord.Right(), m_Property.UICoord.fY, m_Property.IMEEditBoxProperty.fIndicatorSize + m_Property.IMEEditBoxProperty.fBorder * 2, m_Property.UICoord.fHeight );
	m_Property.UICoord.fWidth = fTempWidth;
}

void CEtUIIMEEditBox::Show( bool bShow )
{
	CEtUIControl::Show( bShow );
	s_bHideCaret = false;
}

void CEtUIIMEEditBox::Render( float fElapsedTime )
{
	if( !IsShow() )
	{
		return;
	}

/*	if( m_Property.IMEEditBoxProperty.fIndicatorSize <= 0.0f )
	{
		int i;

		for( i = 0; i < 5; i++ )
		{
			SUICoord Coord;

			Coord.SetCoord( 0.0f, 0.0f, 0.1f, 0.1f );
			m_pParent->CalcTextRect( s_aszIndicator[ i ], &m_Template.m_vecElement[ 5 ], Coord );
			m_Property.IMEEditBoxProperty.fIndicatorSize = max( m_Property.IMEEditBoxProperty.fIndicatorSize, Coord.fWidth );
		}
		UpdateRects();
	}*/
	SUIElement *pElement;
	pElement = GetElement(0);

	if( pElement )
	{
		m_fCompositionWidth = 0.0f;
		if( m_bFocus && s_bHideCaret )
		{
			SUICoord CompCoord;
			m_pParent->CalcTextRect( s_CompString.GetBuffer(), pElement, CompCoord );
			m_fCompositionWidth = CompCoord.fWidth;
		}

		CEtUIEditBox::Render( fElapsedTime );

		//ID3DXFont *pFont = m_pParent->GetFont( pElement->nFontIndex, pElement->nFontHeight );
		//s_CompString.SetFont( pFont );
		//s_CandList.HoriCand.SetFont( pFont );
	}

	if( CEtUIIME::s_bDisableIMEonIMEControl )
		return;

	if( m_bFocus )
	{
		RenderIndicator( fElapsedTime );

		if( s_bHideCaret )
			RenderComposition( fElapsedTime );

		if( s_bShowReadingWindow )
		{
			RenderCandidateReadingWindow( fElapsedTime, true );
		}
		else if( s_CandList.bShowWindow )
		{
			RenderCandidateReadingWindow( fElapsedTime, false );
		}
	}
}

void CEtUIIMEEditBox::RenderCandidateReadingWindow( float fElapsedTime, bool bReading )
{
	UINT i, nNumEntries;
	SUICoord Coord;
	DWORD dwTextColor, dwTextBkColor, dwSelTextColor, dwSelBkColor;
	int nX, nXFirst, nXComp;

	nNumEntries = MAX_CANDLIST;
	if( bReading )
	{
		nNumEntries = 4;
	}

	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	SUIElement* pElement = GetElement( 0 );
 	nX = FontMng.GetCaretPos( m_Buffer.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nCaret );
	nXFirst = FontMng.GetCaretPos( m_Buffer.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nFirstVisible );
	//m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
	//m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

	if( bReading )
	{
		dwTextColor = m_Property.IMEEditBoxProperty.dwReadingTextColor;
		dwTextBkColor = m_Property.IMEEditBoxProperty.dwReadingBackColor;
		dwSelTextColor = m_Property.IMEEditBoxProperty.dwReadingSelTextColor;
		dwSelBkColor = m_Property.IMEEditBoxProperty.dwReadingSelBackColor;
	} 
	else
	{
		dwTextColor = m_Property.IMEEditBoxProperty.dwCandiTextColor;
		dwTextBkColor = m_Property.IMEEditBoxProperty.dwCandiBackColor;
		dwSelTextColor = m_Property.IMEEditBoxProperty.dwCandiSelTextColor;
		dwSelBkColor = m_Property.IMEEditBoxProperty.dwCandiSelBackColor;
	}

	if( ( GetLanguage() == LANG_CHT ) && ( !GetImeId() ) )
	{
		nXComp = 0;
	}
	else
	{
		if( GetPrimaryLanguage() == LANG_JAPANESE )
		{
			nXComp = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_nFirstTargetConv );
			//s_CompString.CPtoX( s_nFirstTargetConv, FALSE, &nXComp );
		}
		else
		{
			nXComp = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_nCompCaret );
			//s_CompString.CPtoX( s_nCompCaret, FALSE, &nXComp );
		}
	}

	//SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	float fWidthRequired = 0.0f;
	float fHeightRequired = 0.0f;
	float fSingleLineHeight = 0.0f;

	if( ( s_bVerticalCand && !bReading ) || ( !s_bHorizontalReading && bReading ) )
	{
		for( i = 0; i < nNumEntries; i++ )
		{
			if( s_CandList.awszCandidate[ i ][ 0 ] == L'\0' )
			{
				break;
			}
			Coord.SetCoord( 0.0f, 0.0f, 0.0f, 0.0f );
			m_pParent->CalcTextRect( s_CandList.awszCandidate[i], pElement, Coord );
			fWidthRequired = max( fWidthRequired, Coord.fWidth );
			fSingleLineHeight = max( fSingleLineHeight, Coord.fHeight );
		}
		fHeightRequired = fSingleLineHeight * nNumEntries;
	} 
	else
	{
		Coord.SetCoord( 0.0f, 0.0f, 0.0f, 0.0f );
		if( bReading )
		{
			m_pParent->CalcTextRect( s_wszReadingString, pElement, Coord );
		}
		else
		{
			m_pParent->CalcTextRect( s_CandList.HoriCand.GetBuffer(), pElement, Coord );
		}
		// 너무 꽉차게 그리지 않고 여유를 좀 두기 위해서 공간을 준다
		Coord.fWidth += 0.01f;
		Coord.fHeight += 0.01f;
		fWidthRequired = Coord.fWidth;;
		fSingleLineHeight = Coord.fHeight;
		fHeightRequired = Coord.fHeight;
	}

	bool bHasPosition = false;

	Coord.SetCoord( s_fCompStringX + FloatXCoord( nXComp ), s_fCompStringY + m_TextCoord.fHeight, 
		fWidthRequired, fHeightRequired );
	if( Coord.Right() > m_pParent->Width() )
	{
		Coord.fX -= Coord.Right() - m_pParent->Width();
		Coord.fWidth = m_pParent->Width() - Coord.fX;
	}
	if( Coord.Bottom() <= m_pParent->Height() )
	{
		bHasPosition = true;
	}
	else if( m_bForceBottomPosition )
	{
		bHasPosition = true;
	}

	if( !bHasPosition )
	{
		Coord.SetCoord( s_fCompStringX + FloatXCoord( nXComp ), s_fCompStringY - fHeightRequired, fWidthRequired, fHeightRequired );
		if( Coord.Right() > m_pParent->Width() )
		{
			Coord.fX -= Coord.Right() - m_pParent->Width();
			Coord.fWidth = m_pParent->Width();
		}
		if( Coord.fY >= 0.0f )
		{
			bHasPosition = true;
		}
	}

	if( !bHasPosition )
	{
		int nXCompTrail;

		nXCompTrail = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_nCompCaret );
		//s_CompString.CPtoX( s_nCompCaret, TRUE, &nXCompTrail );
		Coord.SetCoord( s_fCompStringX + FloatXCoord( nXCompTrail ), 0.0f, fWidthRequired, fHeightRequired );
		if( Coord.Right() <= m_pParent->Width() )
		{
			bHasPosition = true;
		}
	}

	if( !bHasPosition )
	{
		Coord.SetCoord( s_fCompStringX + FloatXCoord( nXComp ) - fWidthRequired, 0.0f, fWidthRequired, fHeightRequired );
		if( Coord.Right() >= 0.0f )
		{
			bHasPosition = true;
		}
	}

	if( !bHasPosition )
	{
		Coord.fX = 0.0f;
		Coord.fWidth = fWidthRequired;
	}

	if( !bReading )
	{
		s_CandList.CandidateCoord = Coord;
	}

	m_pParent->DrawRect( Coord, dwTextBkColor );
	if( ( s_bVerticalCand && !bReading ) || ( !s_bHorizontalReading && bReading ) )
	{
		for( i = 0; i < nNumEntries; i++ )
		{
			Coord.fHeight = fSingleLineHeight;
			if( s_CandList.dwSelection == i )
			{
				m_pParent->DrawRect( Coord, dwSelBkColor );
				m_pParent->DrawDlgText( s_CandList.awszCandidate[ i ], pElement, dwSelTextColor, Coord );
			} 
			else
			{
				m_pParent->DrawDlgText( s_CandList.awszCandidate[ i ], pElement, dwTextColor, Coord );
			}
			Coord.fY += fSingleLineHeight;
		}
	} 
	else
	{
		if( bReading )
		{
			//m_pParent->DrawDlgText( s_wszReadingString, pElement, dwTextColor, Coord );
			m_pParent->DrawDlgText( s_wszReadingString, pElement, dwTextColor, Coord, -1, -1, true );
		}
		else
		{
			m_pParent->DrawDlgText( s_CandList.HoriCand.GetBuffer(), pElement, dwTextColor, Coord, -1, -1, true/*, DT_VCENTER | DT_CENTER*/ );
		}
		if( !bReading )
		{
			int nXLeft, nXRight;
			
			nXLeft = FontMng.GetCaretPos( s_CandList.HoriCand.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_CandList.nFirstSelected );
			//s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected, FALSE, &nXLeft );
			nXRight = FontMng.GetCaretPos( s_CandList.HoriCand.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, 
										   s_CandList.nFirstSelected + s_CandList.nHoriSelectedLen );
			if( nXLeft != nXRight )
			{
				//s_CandList.HoriCand.CPtoX( s_CandList.nFirstSelected + s_CandList.nHoriSelectedLen, FALSE, &nXRight );
				Coord.fWidth = FloatXCoord( nXRight - nXLeft );
				Coord.fX += FloatXCoord( nXLeft );
				m_pParent->DrawRect( Coord, dwSelBkColor );
				m_pParent->DrawDlgText( s_CandList.HoriCand.GetBuffer() + s_CandList.nFirstSelected, pElement, dwSelTextColor, Coord, -1, -1, true );
			}
		}
	}
}

void CEtUIIMEEditBox::RenderComposition( float fElapsedTime )
{
	int nX, nXFirst;

	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	SUIElement* pElement = GetElement( 0 );
	nX = FontMng.GetCaretPos( m_Buffer.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nCaret );
	//m_Buffer.CPtoX( m_nCaret, FALSE, &nX );
	nXFirst = FontMng.GetCaretPos( m_Buffer.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nFirstVisible );
	//m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nXFirst );

	SUICoord CompCoord, FirstCoord, CaretCoord;

	//SUIElement *pElement = GetElement(0);
	if( !pElement ) return;

	CompCoord.SetCoord( m_TextCoord.fX + ( nX - nXFirst ) / m_pParent->GetScreenWidth(), m_TextCoord.fY, 0.1f, m_TextCoord.fHeight );
	m_pParent->CalcTextRect( s_CompString.GetBuffer(), pElement, CompCoord );
	//if( CompCoord.Right() > m_TextCoord.Right() )
	//{
	//	CompCoord.fX += m_TextCoord.fX - CompCoord.fX;
	//	CompCoord.fY += CompCoord.fHeight;
	//}

	FirstCoord = CompCoord;
	s_fCompStringX = CompCoord.fX;
	s_fCompStringY = CompCoord.fY;

	int nCharLeft;
	const WCHAR *pwszComp;
	DWORD dwTextColor;

	dwTextColor = m_Property.IMEEditBoxProperty.dwCompTextColor;
	pElement->FontColor.dwCurrentColor = dwTextColor;
	pwszComp = s_CompString.GetBuffer();
	nCharLeft = s_CompString.GetTextSize();

	if( nCharLeft == 0 ) return;

	while( 1 )
	{
		int nLastInLine, nTrail, nNumCharToDraw;
		nLastInLine = FontMng.GetCaretFromCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight,
												  (int)((CompCoord.fX + CompCoord.fWidth - m_TextCoord.fX) * m_pParent->GetScreenWidth() + 0.0001f), nTrail );	// 23.9999때문에 23으로 int화 되서 그냥 0.0001 더해 올림처리 한다.
		if( 0 == nLastInLine )
			nLastInLine = 1;

		// Note: Trail 값은 무조건 1임.. 폰트매니져에서 새로 캐싱된 문자열은 Trail 값이 딸려 나오므로 더해줘야함..
		if( 0 != nTrail )
			nLastInLine += 1;

		//s_CompString.XtoCP( ( int )( ( m_TextCoord.Right() - CompCoord.fX ) * m_pParent->GetScreenWidth() ), &nLastInLine, &bTrail );
		
		nNumCharToDraw = min( nCharLeft, nLastInLine );
		m_pParent->CalcTextRect( pwszComp, pElement, CompCoord, nNumCharToDraw );
		CompCoord.fHeight = m_TextCoord.fHeight;

		if( GetPrimaryLanguage() == LANG_KOREAN )
		{
			if( m_bCaretOn )
			{
				m_pParent->DrawRect( CompCoord, m_Property.IMEEditBoxProperty.dwCompBackColor );
			}
			else
			{
				dwTextColor = pElement->FontColor.dwColor[ UI_STATE_NORMAL ];
			}
		} 
		else
		{
			m_pParent->DrawRect( CompCoord, m_Property.IMEEditBoxProperty.dwCompBackColor );
		}

		pElement->FontColor.dwCurrentColor = dwTextColor;
		//CEtFontMng::GetInstance().EnableFreeType( false );
		m_pParent->DrawDlgText( pwszComp, pElement, dwTextColor, CompCoord, nNumCharToDraw );
		//CEtFontMng::GetInstance().EnableFreeType( true );

		nCharLeft -= nNumCharToDraw;
		pwszComp += nNumCharToDraw;
		if( nCharLeft <= 0 )
		{
			break;
		}
		CompCoord.fX += m_TextCoord.fX - CompCoord.fX;
		CompCoord.fY += CompCoord.fHeight;
	}

	//CompCoord = FirstCoord;
	CompCoord.fX = FirstCoord.fX;
	CompCoord.fY = FirstCoord.fY;
	CompCoord.fWidth = FirstCoord.fWidth;

	int nCharFirst;
	BYTE *pAttr;
	const WCHAR *pcComp;

	nCharFirst = 0;
	nXFirst = 0;
	s_nFirstTargetConv = -1;
	for( pcComp = s_CompString.GetBuffer(), pAttr = s_abyCompStringAttr; *pcComp != L'\0'; pcComp++, pAttr++ )
	{
		DWORD dwBkColor;
		int nXLeft, nXRight;

		nXLeft = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, int(pcComp - s_CompString.GetBuffer()) );
		//s_CompString.CPtoX( ( int )( pcComp - s_CompString.GetBuffer() ), FALSE, &nXLeft );
		nXRight = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_CompString.GetTextSize() );
		//s_CompString.CPtoX( ( int )( pcComp - s_CompString.GetBuffer() ), TRUE, &nXRight );

		if( m_bFirstVisibleOver == false && m_nTempCompSize == 0 )
		{
			if( nXRight - nXFirst > ( int )( ( m_TextCoord.Right() - CompCoord.fX ) * m_pParent->GetScreenWidth() ) )
			{
				CompCoord.fX += m_TextCoord.fX - CompCoord.fX;
				CompCoord.fY += CompCoord.fHeight;
				nCharFirst = ( int )( pcComp - s_CompString.GetBuffer() );
				nXFirst = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nCharFirst );
				//s_CompString.CPtoX( nCharFirst, FALSE, &nXFirst );
			}
		}

		if( s_nCompCaret == ( int )( pcComp - s_CompString.GetBuffer() ) )
		{
			CaretCoord = CompCoord;
			CaretCoord.fX += ( nXLeft - nXFirst - 1 ) / m_pParent->GetScreenWidth();
			CaretCoord.fWidth = 2.0f / m_pParent->GetScreenWidth();
		}

		if( *pAttr == ATTR_TARGET_CONVERTED )
		{
			pElement->FontColor.dwCurrentColor = m_dwCompTargetColor;
			dwBkColor = m_dwCompTargetBkColor;
		}
		else
		{
			if( *pAttr == ATTR_TARGET_NOTCONVERTED )
			{
				pElement->FontColor.dwCurrentColor = m_dwCompTargetNonColor;
				dwBkColor = m_dwCompTargetNonBkColor;
			}
			else
			{
				continue;
			}
		}

		SUICoord TargetCoord;

		TargetCoord.SetCoord( CompCoord.fX + ( nXLeft - nXFirst ) / m_pParent->GetScreenWidth(), CompCoord.fY, ( nXRight - nXLeft ) / m_pParent->GetScreenWidth(), CompCoord.fHeight );
		m_pParent->DrawRect( TargetCoord, dwBkColor );
		m_pParent->DrawDlgText( pcComp, pElement, pElement->FontColor.dwCurrentColor, TargetCoord, 1 );
		if( s_nFirstTargetConv == -1 )
		{
			s_nFirstTargetConv = ( int )( pAttr - s_abyCompStringAttr );
		}
	}

	if( m_bCaretOn )
	{
		if( s_nCompCaret == s_CompString.GetTextSize() )
		{
			nX = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, s_nCompCaret );
			//s_CompString.CPtoX( s_nCompCaret, FALSE, &nX );
			CaretCoord = CompCoord;
			CaretCoord.fX += ( nX - nXFirst - 1 ) / m_pParent->GetScreenWidth();
			CaretCoord.fWidth = 2.0f / m_pParent->GetScreenWidth();
		}
		m_pParent->DrawRect( CaretCoord, m_Property.IMEEditBoxProperty.dwCompCaretColor );
	}
}

void CEtUIIMEEditBox::RenderIndicator( float fElapsedTime )
{
	if( m_Property.IMEEditBoxProperty.fIndicatorSize <= 0.0f )
	{
		return;
	}

	UpdateBlendRate();

	SUIElement *pElement;
	SUICoord TempCoord;//, CalcCoord;
	WCHAR *pwszIndicator;

	pElement = GetElement(5);
	pElement->TextureColor.Blend( UI_STATE_NORMAL, fElapsedTime, m_fBlendRate );
	if( m_bExistTemplateTexture )
		m_pParent->DrawSprite( m_Template.m_hTemplateTexture, pElement->TemplateUVCoord, pElement->TextureColor.dwCurrentColor, m_IndicatorCoord );
	else
		m_pParent->DrawSprite( pElement->UVCoord, pElement->TextureColor.dwCurrentColor, m_IndicatorCoord );

/*	TempCoord = m_IndicatorCoord;
	TempCoord.fX += m_Property.IMEEditBoxProperty.fSpace;
	TempCoord.fY += m_Property.IMEEditBoxProperty.fSpace;
	TempCoord.fWidth -= m_Property.IMEEditBoxProperty.fSpace * 2;
	TempCoord.fHeight -= m_Property.IMEEditBoxProperty.fSpace * 2;
*/
	pElement->FontColor.dwCurrentColor = m_Property.IMEEditBoxProperty.dwIndiTextColor;
	if( ( s_ImeState == IMEUI_STATE_ON ) && ( s_bEnableImeSystem ) )
	{
		pwszIndicator = s_pwszCurrIndicator;
	}
	else
	{
		pwszIndicator = s_aszIndicator[ 0 ];
	}
/*	if( ( s_ImeState == IMEUI_STATE_ON ) && ( s_bEnableImeSystem ) )
	{
		pElement->FontColor.dwCurrentColor = m_dwIndicatorImeColor;
	}
	else
	{
		pElement->FontColor.dwCurrentColor = m_dwIndicatorEngColor;
	}
	if( s_bEnableImeSystem )
	{
		pwszIndicator = s_pwszCurrIndicator;
	}
	else
	{
		pwszIndicator = s_aszIndicator[ 0 ];
	}
*/
	TempCoord = m_IndicatorCoord;
	m_pParent->CalcTextRect( pwszIndicator, pElement, TempCoord );
	float fMidHeight = ( m_IndicatorCoord.fHeight - TempCoord.fHeight ) / 2.0f;
	TempCoord.fX -= m_Property.IMEEditBoxProperty.fSpace;
	TempCoord.fY = m_IndicatorCoord.fY + fMidHeight;

	m_pParent->DrawDlgText( pwszIndicator, pElement, pElement->FontColor.dwCurrentColor, TempCoord );
}

void CEtUIIMEEditBox::TruncateCompString( bool bUseBackSpace, int iNewStrLen )
{
	if( !s_bInsertOnType )
	{
		return;
	}

	int i, cc;

	cc = ( int )wcslen( s_CompString.GetBuffer() );
	assert( iNewStrLen == 0 || iNewStrLen >= cc );

	for ( i = 0; i < cc - s_nCompCaret; i++ )
	{
		SendMessage( m_pParent->GetHWnd(), WM_KEYDOWN, VK_RIGHT, 0 );
	}
	SendMessage( m_pParent->GetHWnd(), WM_KEYUP, VK_RIGHT, 0 );

	if( ( bUseBackSpace ) || ( m_bInsertMode ) )
	{
		iNewStrLen = 0;
	}
	if( iNewStrLen < cc )
	{
		for( i = 0; i < cc - iNewStrLen; i++ )
		{
			SendMessage( m_pParent->GetHWnd(), WM_KEYDOWN, VK_BACK, 0 );
			SendMessageW( m_pParent->GetHWnd(), WM_CHAR, VK_BACK, 0 );
		}
		SendMessage( m_pParent->GetHWnd(), WM_KEYUP, VK_BACK, 0 );
	}
	else
	{
		iNewStrLen = cc;
	}

	for ( i = 0; i < iNewStrLen; i++ )
	{
		SendMessage( m_pParent->GetHWnd(), WM_KEYDOWN, VK_LEFT, 0 );
	}
	SendMessage( m_pParent->GetHWnd(), WM_KEYUP, VK_LEFT, 0 );
}

void CEtUIIMEEditBox::FinalizeString( bool bSend )
{
	HIMC hImc;
	static bool bProcessing = false;

	hImc = _ImmGetContext( m_pParent->GetHWnd() );
	if( hImc == NULL )
	{
		return;
	}

	if( bProcessing )
	{
		_ImmReleaseContext( m_pParent->GetHWnd(), hImc );
		return;
	}
	bProcessing = true;

	if( ( !s_bInsertOnType ) && ( bSend ) )
	{
		LONG lLength;

		lLength = lstrlenW( s_CompString.GetBuffer() );
		if( ( GetLanguage() == LANG_CHT ) && ( s_CompString[ lLength - 1 ] == 0x3000 ) )
		{
			s_CompString[ lLength - 1 ] = 0;
		}
		SendCompString();
	}

	TruncateCompString();
	s_bHideCaret = false;
	s_bShowReadingWindow = false;

	ResetCompositionString();
	_ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
	_ImmNotifyIME( hImc, NI_CLOSECANDIDATE, 0, 0 ); 
	_ImmReleaseContext( m_pParent->GetHWnd(), hImc );
	bProcessing = false;
}

void CEtUIIMEEditBox::SendCompString()
{
	for( int i = 0; i < lstrlenW( s_CompString.GetBuffer() ); i++ )
	{
		MsgProc( WM_CHAR, ( WPARAM )s_CompString[ i ], 0 );
	}
}

void CEtUIIMEEditBox::PlaceCaret( int nCP )
{
	int nX1, nX2, nCompSize(0);
	m_bFirstVisibleOver = false;
	m_nTempCompSize = 0;

	CEtFontMng& FontMng = CEtFontMng::GetInstance();
	SUIElement* pElement = GetElement( 0 );
	nX1 = FontMng.GetCaretPos( m_Buffer.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nFirstVisible );
	//m_Buffer.CPtoX( m_nFirstVisible, FALSE, &nX1 );
	nX2 = FontMng.GetCaretPos( m_Buffer.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, m_nCaret );
	//m_Buffer.CPtoX( m_nCaret, FALSE, &nX2 );
	nCompSize = FontMng.GetCaretPos( s_CompString.GetBuffer(), pElement->nFontIndex, pElement->nFontHeight, nCP );
	//s_CompString.CPtoX( nCP, FALSE, &nCompSize );

	int nTextWidth = (int)( m_TextCoord.fWidth * m_pParent->GetScreenWidth() );
	if( (nX2-nX1+nCompSize) > nTextWidth )
	{
		int nTemp = m_Buffer.GetMaxEditBoxLength() - m_Buffer.GetTextSize();
		int nCPNew1st(0);

		if( nTemp < 3 )	
			++nCPNew1st;
		else			
			nCPNew1st += 3;

		// Note: 조합중인 문자열이 너무 길어서 완성된 문자열을 밀어내서 하나도 보이지 않게 되는 경우를 막는다.
		// 안그러면 m_nFirstVisible 값이 문자열 길이를 넘어가게 되어버림.
		if( m_nFirstVisible + nCPNew1st < (int)wcslen( m_Buffer.GetBuffer() ) )
			m_nFirstVisible += nCPNew1st;
		else
			m_bFirstVisibleOver = true;

		// 아무래도 CustomUI에는 없는 m_bFirstVisibleOver 변수를 추가해 처리한거 자체가 별로인거 같아서,
		// 이 상태가 되었을때 ImmSetCompositionStringW함수를 호출해 조합중인 글자를 일정길이로 제한두려 했으나,
		// 아무리 설정해도 함수가 FALSE를 리턴한다.
		// 구글에서 찾아봐도 안된다고 하는 사람은 많은데, 어떻게 고쳐야된다고 적어둔건 하나도 못찾았다.

		// #30251 귓속말[아이디]가 길 경우 실제로 문자를 입력할 수 있는 공간은 얼마 되지 않는다.
		// 글자공간이 짧은데다가 자꾸 리셋된다고 별로라 해서, 이쪽 루틴을 다시 검토해봤는데,
		// CustomUI에는 없는 m_bFirstVisibleOver게 있는 이유는.. Custom UI와 달리, 조합중인 글자도 영역을 차지해서
		// 우측에 있는 글자를 밀어내는 기능이 드네엔 들어있기 때문이다.(그리고 이게 더 자연스럽다.)
		// 이건 거의 모든 편집기가 다 이렇게 되어있는데, 첨에는 모든 조합중인 문자가 다 이런줄 알았다.
		// 근데 해보니까, 한글만 이렇고,
		// 일본어, 중국어처럼 조합스트링이 2자 이상인 것들은 조합중인 윈도우가 따로 뜨고,
		// 문자열 사이에서 조합하더라도 우측 글자를 밀지 않다가 확정이 되어야만 밀어낸다.
		// 근데, 현재 드네는 그 구분이 되어있지 않다.
		// 그래서 아래와 같은 s_CompString 해결책을 사용한다.
		if( m_bFirstVisibleOver && (int)wcslen( s_CompString.GetBuffer() ) < 10 )
		{
			m_bFirstVisibleOver = false;
			m_nTempCompSize = nCompSize;
		}
	}
}

void CEtUIIMEEditBox::ClearText()
{
	CEtUIEditBox::ClearText();
	ResetCompositionString();
}

void CEtUIIMEEditBox::CheckIMEToggleState()
{
	CheckToggleState();
}

void CEtUIIMEEditBox::CancelIMEComposition()
{
	HIMC hImc;
	hImc = _ImmGetContext( m_pParent->GetHWnd() );
	if( hImc == NULL )
		return;

	_ImmNotifyIME( hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
}

void CEtUIIMEEditBox::GetIMECompositionString(std::wstring& str)
{
	HIMC hImc;
	hImc = _ImmGetContext( m_pParent->GetHWnd() );
	if( hImc == NULL )
		return;

	WCHAR wszCompStr[MAX_COMPSTRING_SIZE]={0};
	LONG lRet = _ImmGetCompositionStringW( hImc, GCS_COMPSTR, wszCompStr, sizeof( wszCompStr ) );
	if (lRet > 0)
	{
		lRet /= sizeof( WCHAR );
		wszCompStr[lRet] = 0;
		str = wszCompStr;
	}	
}

int CEtUIIMEEditBox::GetMaxChar() const
{
	return m_Property.IMEEditBoxProperty.dwMaxChars;
}

void CEtUIIMEEditBox::SetMaxChar( DWORD dwChar )
{
	m_Property.IMEEditBoxProperty.dwMaxChars = dwChar;
	SetMaxEditLength( dwChar );
}