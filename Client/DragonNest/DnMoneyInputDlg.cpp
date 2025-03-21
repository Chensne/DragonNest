#include "StdAfx.h"
#include "DnMoneyInputDlg.h"
#include "DnItemTask.h"
#include "DnCashShopTask.h"
#ifdef PRE_MOD_PETAL_WRITE
#include "DnInterfaceString.h"
#endif // PRE_MOD_PETAL_WRITE



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMoneyControl::CDnMoneyControl( CEtUIDialog *pDialog )  
{
	m_eMoneyType = GOLD_TYPE;

	m_bEnablePetal = false;

	m_pDialog = pDialog;
	m_pCoinG = NULL;
	m_pCoinS = NULL;
	m_pCoinC = NULL;
	m_pCoinP = NULL;
	m_pCoinGOutline = NULL;
	m_pCoinSOutline = NULL;
	m_pCoinCOutline = NULL;
	m_pCoinPOutline = NULL;
	m_pEditBoxMoneyG = NULL;
	m_pEditBoxMoneyS = NULL;
	m_pEditBoxMoneyC = NULL;	
	m_pEditBoxMoneyP = NULL;	
	m_pButtonC = NULL;
	m_pButtonG = NULL;
	m_pButtonP = NULL;
	m_pButtonS = NULL;
	m_pImageC = NULL;
	m_pImageG = NULL;
	m_pImageP = NULL;
	m_pImageS = NULL;
}

void CDnMoneyControl::InitialUpdate( bool bEnablePetal )
{
	m_pCoinG = m_pDialog->GetControl<CEtUIStatic>("ID_COIN_G");
	m_pCoinS = m_pDialog->GetControl<CEtUIStatic>("ID_COIN_S");
	m_pCoinC = m_pDialog->GetControl<CEtUIStatic>("ID_COIN_C");

	m_pEditBoxMoneyG = m_pDialog->GetControl<CEtUIEditBox>("ID_EDITBOX_MONEY_G");
	m_pEditBoxMoneyS = m_pDialog->GetControl<CEtUIEditBox>("ID_EDITBOX_MONEY_S");
	m_pEditBoxMoneyC = m_pDialog->GetControl<CEtUIEditBox>("ID_EDITBOX_MONEY_C");

	m_pCoinGOutline = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC_GOLD");
	m_pCoinSOutline = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC_SILVER");
	m_pCoinCOutline = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC_COPPER");
	
	m_pCoinGOutline->Enable( false );
	m_pCoinSOutline->Enable( false );
	m_pCoinCOutline->Enable( false );

	m_nMaxMoney = -1;

	if( bEnablePetal )
	{
		m_bEnablePetal = bEnablePetal;
		m_nMaxPetal = -1;

		m_pCoinP = m_pDialog->GetControl<CEtUIStatic>("ID_COIN_P");
		m_pEditBoxMoneyP = m_pDialog->GetControl<CEtUIEditBox>("ID_EDITBOX_MONEY_P");
		m_pCoinPOutline = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC0");

		m_pCoinPOutline->Enable( false );

		m_pImageG = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC3");
		m_pImageS = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC4");
		m_pImageC = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC5");
		m_pImageP = m_pDialog->GetControl<CEtUIStatic>("ID_STATIC_PETAL");

		m_pButtonG = m_pDialog->GetControl<CEtUIButton>("ID_BUTTON_DUMMY_G");
		m_pButtonS = m_pDialog->GetControl<CEtUIButton>("ID_BUTTON_DUMMY_S");
		m_pButtonC = m_pDialog->GetControl<CEtUIButton>("ID_BUTTON_DUMMY_C");
		m_pButtonP = m_pDialog->GetControl<CEtUIButton>("ID_BUTTON_DUMMY_P");

		SetMoneyType( m_eMoneyType );
	}
}

bool CDnMoneyControl::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if( nCommand == EVENT_EDITBOX_CHANGE )
	{
		if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_G" ) )
		{
			int nLen = m_pEditBoxMoneyG->GetTextLength();

			// 입력 중 일곱자리가 넘어갈때(입력이든, 붙여넣기든)
			if( nLen > 6 )
			{
				// 길이제한 체크
				m_pEditBoxMoneyG->SetText( GetStrMoneyG() );
				return false;
			}

			// 값을 얻어와,
			INT64 nMoney = m_pEditBoxMoneyG->GetTextToInt64();

			// 전에 있던 10000 이상 값을 버리고,
			INT64 nBelow10000 = m_nMoney % 10000;

			// 새로운 100~10000 사이 값을 등록
			m_nMoney = nMoney * 10000 + nBelow10000;

			// UI컨트롤로부터 입력이 들어왔을때만 최종적으로 UpdateMoney를 호출하고,
			// ProcessCommand 함수를 직접 코드안에서 부르는 경우엔, 중간 단계일 가능성이 높으니, UPdateMoney를 호출하지 않는다.
			// 즉, 최종 보정은 마지막에 한번만 한다.
			if( bTriggeredByUser && UpdateMoney() )
			{
				m_pEditBoxMoneyG->SetText( GetStrMoneyG() );
				m_pCoinS->SetText( GetStrMoneyS() );
				m_pCoinC->SetText( GetStrMoneyC() );
			}
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_S" ) )
		{
			int nLen = m_pEditBoxMoneyS->GetTextLength();

			// 입력 중 세자리가 넘어갈때(입력이든, 붙여넣기든)
			if( nLen > 2 )
			{
				// 길이제한 체크
				std::wstring strGold = m_pCoinG->GetText();
				int nLenStrGold = (int)strGold.size();
				if( m_nMoney < 10000 ) nLenStrGold = 0;	// 예외처리
				if( nLenStrGold + nLen > 8 )
				{
					m_pEditBoxMoneyS->SetText( GetStrMoneyS() );
					return false;
				}

				// 길이제한에 괜찮다면, 세자리 넘어가는 숫자들을 모두 실버쪽으로 넘긴다.
				std::wstring strSilver = m_pEditBoxMoneyS->GetText();
				std::wstring strToGold = strSilver.substr(0, nLen-2);
				std::wstring strNewSilver = strSilver.substr(nLen-2);
				std::wstring strNewGold = m_pCoinG->GetText();
				if( m_nMoney < 10000 ) strNewGold = L"";
				strNewGold = strNewGold + strToGold;

				m_pEditBoxMoneyS->SetText( strNewSilver.c_str() );
				m_pEditBoxMoneyG->SetText( strNewGold.c_str() );

				m_pDialog->ProcessCommand( EVENT_EDITBOX_CHANGE, false, m_pEditBoxMoneyG, 0 );
				m_pDialog->ProcessCommand( EVENT_EDITBOX_RELEASEFOCUS, false, m_pEditBoxMoneyG, 0 );
			}

			// 값을 얻어와,
			INT64 nMoney = m_pEditBoxMoneyS->GetTextToInt64();
			if( nMoney < 0 ) nMoney = 0;

			// 전에 있던 100~10000 사이 값을 버리고,
			INT64 nBelow100 = m_nMoney % 100;
			m_nMoney -= nBelow100;
			INT64 nBelow10000 = m_nMoney % 10000;
			m_nMoney -= nBelow10000;

			// 새로운 100~10000 사이 값을 등록
			m_nMoney += nMoney * 100;

			// 100 미만 재등록
			m_nMoney += nBelow100;

			if( bTriggeredByUser && UpdateMoney() )
			{
				m_pEditBoxMoneyS->SetText( GetStrMoneyS() );
				m_pCoinG->SetText( GetStrMoneyG() );
				m_pCoinC->SetText( GetStrMoneyC() );
			}
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_C" ) )
		{
			int nLen = m_pEditBoxMoneyC->GetTextLength();

			// 입력 중 세자리가 넘어갈때(입력이든, 붙여넣기든)
			if( nLen > 2 )
			{
				// 길이제한 체크 - 0일때 제대로 안되있다.
				std::wstring strGold = m_pCoinG->GetText();
				int nLenStrGold = (int)strGold.size();
				if( m_nMoney < 10000 ) nLenStrGold = 0;	// 예외처리

				std::wstring strSilver = m_pCoinS->GetText();
				int nLenStrSilver = (int)strSilver.size();
				if( m_nMoney < 100 ) nLenStrSilver = 0;

				if( nLenStrGold + nLenStrSilver + nLen > 10 )
				{
					m_pEditBoxMoneyC->SetText( GetStrMoneyC() );
					return false;
				}

				// 길이제한에 괜찮다면, 세자리 넘어가는 숫자들을 모두 실버쪽으로 넘긴다.
				std::wstring strCopper = m_pEditBoxMoneyC->GetText();
				std::wstring strToSilver = strCopper.substr(0, nLen-2);
				std::wstring strNewCopper = strCopper.substr(nLen-2);
				std::wstring strNewSilver = m_pCoinS->GetText();
				if( m_nMoney < 100 ) strNewSilver = L"";
				strNewSilver = strNewSilver + strToSilver;

				m_pEditBoxMoneyC->SetText( strNewCopper.c_str() );
				m_pEditBoxMoneyS->SetText( strNewSilver.c_str() );

				m_pDialog->ProcessCommand( EVENT_EDITBOX_CHANGE, false, m_pEditBoxMoneyS, 0 );
				m_pDialog->ProcessCommand( EVENT_EDITBOX_RELEASEFOCUS, false, m_pEditBoxMoneyS, 0 );
			}
			else if( nLen < 2 )
			{
				// 길이가 전보다 짧아졌을때(백스페이스로 삭제하거나, 블럭설정 후 삭제했을때)
				// 에디트박스에 아무것도 없는상태에서 1눌렀을때는 실버쪽에 금액이 있어도 당기지 않는다.
				//
				// 근데 이거 꼭 필요한가? 기획자와 상의해서 우선 제외한다.
				if( nLen < m_nPrevLenMoneyC )
				{

				}
			}

			// 길이를 기억해둔다.
			m_nPrevLenMoneyC = nLen;

			// 값을 얻어와,
			INT64 nMoney = m_pEditBoxMoneyC->GetTextToInt64();
			if( nMoney < 0 ) nMoney = 0;

			// 전에 있던 100 미만의 값을 버리고,
			INT64 nBelow100 = m_nMoney % 100;
			m_nMoney -= nBelow100;

			// 새로운 100미만의 값을 등록
			m_nMoney += nMoney;

			if( bTriggeredByUser && UpdateMoney() )
			{
				m_pEditBoxMoneyC->SetText( GetStrMoneyC() );
				m_pCoinS->SetText( GetStrMoneyS() );
				m_pCoinG->SetText( GetStrMoneyG() );
			}
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_P" ) )
		{
			m_nPetal = m_pEditBoxMoneyP->GetTextToInt64();

			if( bTriggeredByUser && UpdatePetal() )
#ifdef PRE_MOD_PETAL_WRITE
				m_pEditBoxMoneyP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str() );
#else // PRE_MOD_PETAL_WRITE
				m_pEditBoxMoneyP->SetText( GetStrMoneyP() );
#endif // PRE_MOD_PETAL_WRITE
		}
	}
	else if( nCommand == EVENT_EDITBOX_FOCUS )
	{
		if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_G" ) )
		{
			m_pCoinG->Show( false );
			m_pCoinGOutline->Enable( true );
			m_pEditBoxMoneyG->SetText( GetStrMoneyG(), true );
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_S" ) )
		{
			m_pCoinS->Show( false );
			m_pCoinSOutline->Enable( true );
			m_pEditBoxMoneyS->SetText( GetStrMoneyS(), true );
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_C" ) )
		{
			m_pCoinC->Show( false );
			m_pCoinCOutline->Enable( true );
			m_pEditBoxMoneyC->SetText( GetStrMoneyC(), true );
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_P" ) )
		{
			m_pCoinP->Show( false );
			m_pCoinPOutline->Enable( true );
#ifdef PRE_MOD_PETAL_WRITE
			m_pEditBoxMoneyP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str(), true );
#else // PRE_MOD_PETAL_WRITE
			m_pEditBoxMoneyP->SetText( GetStrMoneyP(), true );
#endif // PRE_MOD_PETAL_WRITE
		}
	}
	else if( nCommand == EVENT_EDITBOX_RELEASEFOCUS )
	{
		if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_G" ) )
		{
			m_pEditBoxMoneyG->ClearText();
			m_pCoinGOutline->Enable( false );
			m_pCoinG->SetText( GetStrMoneyG() );
			m_pCoinG->Show( true );
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_S" ) )
		{
			m_pEditBoxMoneyS->ClearText();
			m_pCoinSOutline->Enable( false );
			m_pCoinS->SetText( GetStrMoneyS() );
			m_pCoinS->Show( true );
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_C" ) )
		{
			m_pEditBoxMoneyC->ClearText();
			m_pCoinCOutline->Enable( false );
			m_pCoinC->SetText( GetStrMoneyC() );
			m_pCoinC->Show( true );
		}
		else if( m_pDialog->IsCmdControl("ID_EDITBOX_MONEY_P" ) )
		{
			m_pEditBoxMoneyP->ClearText();
			m_pCoinPOutline->Enable( false );
#ifdef PRE_MOD_PETAL_WRITE
			m_pCoinP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str() );
#else // PRE_MOD_PETAL_WRITE
			m_pCoinP->SetText( GetStrMoneyP() );
#endif // PRE_MOD_PETAL_WRITE
			m_pCoinP->Show( true );
		}
	}
	else if ( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( m_pDialog->IsCmdControl("ID_BUTTON_DUMMY_G" ) )
		{
			m_pDialog->RequestFocus( m_pEditBoxMoneyG );
			return false;
		}
		else if( m_pDialog->IsCmdControl("ID_BUTTON_DUMMY_S" ) )
		{
			m_pDialog->RequestFocus( m_pEditBoxMoneyS );
			return false;
		}
		else if( m_pDialog->IsCmdControl("ID_BUTTON_DUMMY_C" ) )
		{
			m_pDialog->RequestFocus( m_pEditBoxMoneyC );
			return false;
		}
		else if( m_pDialog->IsCmdControl("ID_BUTTON_DUMMY_P" ) )
		{
			m_pDialog->RequestFocus( m_pEditBoxMoneyP );
			return false;
		}
	}

	return true;
}

void CDnMoneyControl::SetMax()
{
	if( GOLD_TYPE == m_eMoneyType )
	{
		INT64 nMaxMoney;

		if( m_nMaxMoney == -1 )
			nMaxMoney = GetItemTask().GetCoin();
		else
			nMaxMoney = m_nMaxMoney;

		m_nMoney = nMaxMoney;
		m_pCoinG->SetText( GetStrMoneyG() );
		m_pCoinS->SetText( GetStrMoneyS() );
		m_pCoinC->SetText( GetStrMoneyC() );
	}
	else if( PETAL_TYPE == m_eMoneyType )
	{
		INT64 nMaxPetal;

		if( m_nMaxPetal == -1 )
			nMaxPetal = GetCashShopTask().GetUserReserve();
		else
			nMaxPetal = m_nMaxPetal;

		m_nPetal = nMaxPetal;
#ifdef PRE_MOD_PETAL_WRITE
		m_pCoinP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		m_pCoinP->SetText( GetStrMoneyP() );
#endif // PRE_MOD_PETAL_WRITE
	}
}

void CDnMoneyControl::Init()
{
	m_nMoney = 0;
	
	m_pCoinG->SetIntToText( 0 );
	m_pCoinS->SetIntToText( 0 );
	m_pCoinC->SetIntToText( 0 );

	m_nPrevLenMoneyG = 0;
	m_nPrevLenMoneyS = 0;
	m_nPrevLenMoneyC = 0;

	if( m_bEnablePetal )
	{
		m_nPetal = 0;
		m_pCoinP->SetIntToText( 0 );
		m_nPrevLenMoneyP = 0;
	}
}

bool CDnMoneyControl::UpdateMoney()
{
	// 최소보정은 항상 0
	if( m_nMoney < 0 )
	{
		m_nMoney = 0;
		return true;
	}

	// 최대 보정은 max값 설정된 걸로 한다. 설정되지 않았다면 인벤토리 금액으로 한다.
	INT64 nMaxMoney;
	if( m_nMaxMoney == -1 )
		nMaxMoney = GetItemTask().GetCoin();
	else
		nMaxMoney = m_nMaxMoney;

	if( m_nMoney > nMaxMoney )
	{
		m_nMoney = nMaxMoney;
		return true;
	}

	return false;
}

bool CDnMoneyControl::UpdatePetal()
{
	if( m_nPetal < 0 )
	{
		m_nPetal = 0;
		return true;
	}

	INT64 nMaxPetal;
	if( m_nMaxPetal == -1 )
		nMaxPetal = GetCashShopTask().GetUserReserve();
	else
		nMaxPetal = m_nMaxPetal;

	if( m_nPetal > nMaxPetal )
	{
		m_nPetal = nMaxPetal;
		return true;
	}

	return false;
}

void CDnMoneyControl::Show( bool bShow )
{
	if( bShow && m_bEnablePetal )
	{
		SetMoneyType( m_eMoneyType );
	}
	else
	{
		Init();
	}
}

bool CDnMoneyControl::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_TAB )
		{
			if( m_pEditBoxMoneyC->IsFocus() )
			{
				m_pDialog->RequestFocus( m_pEditBoxMoneyG );
				return true;
			}
		}
	}

	if( uMsg == WM_MOUSEWHEEL )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		m_pDialog->PointToFloat( MousePoint, fMouseX, fMouseY );

		if( m_pDialog->IsMouseInDlg() )
		{
			UINT uLines;
			SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
			if( nScrollAmount > 0 )
			{
				if( m_pEditBoxMoneyG->IsFocus() )
				{
					// 휠로 할때는 각 단위별로 올라야한다.
					m_nMoney += 10000;
					UpdateMoney();
					m_pEditBoxMoneyG->SetText( GetStrMoneyG() );
					m_pCoinS->SetText( GetStrMoneyS() );
					m_pCoinC->SetText( GetStrMoneyC() );
				}
				else if( m_pEditBoxMoneyS->IsFocus() )
				{
					m_nMoney += 100;
					UpdateMoney();
					m_pEditBoxMoneyS->SetText( GetStrMoneyS() );
					m_pCoinG->SetText( GetStrMoneyG() );
					m_pCoinC->SetText( GetStrMoneyC() );
				}
				else if( m_pEditBoxMoneyC->IsFocus() )
				{
					m_nMoney += 1;
					UpdateMoney();
					m_pEditBoxMoneyC->SetText( GetStrMoneyC() );
					m_pCoinS->SetText( GetStrMoneyS() );
					m_pCoinG->SetText( GetStrMoneyG() );
				}
				else if( m_pEditBoxMoneyP && m_pEditBoxMoneyP->IsFocus() )
				{
					m_nMoney += 100;
					UpdatePetal();
#ifdef PRE_MOD_PETAL_WRITE
					m_pEditBoxMoneyP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str() );
#else // PRE_MOD_PETAL_WRITE
					m_pEditBoxMoneyP->SetText( GetStrMoneyP() );
#endif // PRE_MOD_PETAL_WRITE
				}
				return true;
			}
			else if( nScrollAmount < 0 )
			{
				if( m_pEditBoxMoneyG->IsFocus() )
				{
					// 휠로 할때는 각 단위별로 올라야한다.
					m_nMoney -= 10000;
					UpdateMoney();
					m_pEditBoxMoneyG->SetText( GetStrMoneyG() );
					m_pCoinS->SetText( GetStrMoneyS() );
					m_pCoinC->SetText( GetStrMoneyC() );
				}
				else if( m_pEditBoxMoneyS->IsFocus() )
				{
					m_nMoney -= 100;
					UpdateMoney();
					m_pEditBoxMoneyS->SetText( GetStrMoneyS() );
					m_pCoinG->SetText( GetStrMoneyG() );
					m_pCoinC->SetText( GetStrMoneyC() );
				}
				else if( m_pEditBoxMoneyC->IsFocus() )
				{
					m_nMoney -= 1;
					UpdateMoney();
					m_pEditBoxMoneyC->SetText( GetStrMoneyC() );
					m_pCoinS->SetText( GetStrMoneyS() );
					m_pCoinG->SetText( GetStrMoneyG() );
				}
				else if( m_pEditBoxMoneyP && m_pEditBoxMoneyP->IsFocus() )
				{
					m_nMoney -= 100;
					UpdatePetal();
#ifdef PRE_MOD_PETAL_WRITE
					m_pEditBoxMoneyP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str() );
#else // PRE_MOD_PETAL_WRIT
					m_pEditBoxMoneyP->SetText( GetStrMoneyP() );
#endif // PRE_MOD_PETAL_WRIT
				}
				return true;
			}
		}
	}
	return false;
}

bool CDnMoneyControl::IsEditBoxFocus()
{
	return (m_pEditBoxMoneyG->IsFocus() || m_pEditBoxMoneyS->IsFocus() || m_pEditBoxMoneyC->IsFocus() || ( m_pEditBoxMoneyP && m_pEditBoxMoneyP->IsFocus() ) );
}

void CDnMoneyControl::SetMoney( INT64 nMoney )
{
	m_nMoney = nMoney;
	UpdateMoney();

	m_pCoinG->SetText( GetStrMoneyG() );
	m_pCoinS->SetText( GetStrMoneyS() );
	m_pCoinC->SetText( GetStrMoneyC() );
	
	m_pEditBoxMoneyG->SetText( L"" );
	m_pEditBoxMoneyS->SetText( L"" );
	m_pEditBoxMoneyC->SetText( L"" );

	if( m_bEnablePetal )
	{
#ifdef PRE_MOD_PETAL_WRITE
		m_pCoinP->SetText( DN_INTERFACE::UTIL::GetAddCommaString( m_nPetal ).c_str() );
#else // PRE_MOD_PETAL_WRIT
		m_pCoinP->SetText( GetStrMoneyP() );
#endif // PRE_MOD_PETAL_WRITE
		m_pEditBoxMoneyP->SetText( L"" );
	}
}

void CDnMoneyControl::SetMoneyType( int eMoneyType )
{
	m_eMoneyType = eMoneyType;

	if( GOLD_TYPE == m_eMoneyType )
	{
		m_pCoinG->Show( true );
		m_pCoinS->Show( true );
		m_pCoinC->Show( true );
		m_pCoinP->Show( false );

		m_pCoinGOutline->Show( true );
		m_pCoinSOutline->Show( true );
		m_pCoinCOutline->Show( true );
		m_pCoinPOutline->Show( false );

		m_pEditBoxMoneyG->Show( true );
		m_pEditBoxMoneyS->Show( true );
		m_pEditBoxMoneyC->Show( true );
		m_pEditBoxMoneyP->Show( false );

		m_pImageG->Show( true );
		m_pImageS->Show( true );
		m_pImageC->Show( true );
		m_pImageP->Show( false );

		m_pButtonG->Show( true );
		m_pButtonS->Show( true );
		m_pButtonC->Show( true );
		m_pButtonP->Show( false );

		EnableFee( true );

		m_pDialog->RequestFocus( m_pEditBoxMoneyC );
	}
	else if( PETAL_TYPE == m_eMoneyType )
	{
		m_pCoinG->Show( false );
		m_pCoinS->Show( false );
		m_pCoinC->Show( false );
		m_pCoinP->Show( true );

		m_pCoinGOutline->Show( false );
		m_pCoinSOutline->Show( false );
		m_pCoinCOutline->Show( false );
		m_pCoinPOutline->Show( true );

		m_pEditBoxMoneyG->Show( false );
		m_pEditBoxMoneyS->Show( false );
		m_pEditBoxMoneyC->Show( false );
		m_pEditBoxMoneyP->Show( true );

		m_pImageG->Show( false );
		m_pImageS->Show( false );
		m_pImageC->Show( false );
		m_pImageP->Show( true );

		m_pButtonG->Show( false );
		m_pButtonS->Show( false );
		m_pButtonC->Show( false );
		m_pButtonP->Show( true );

		EnableFee( false );

		m_pDialog->RequestFocus( m_pEditBoxMoneyP );
	}
}

void CDnMoneyControl::EnableFee( bool bEnable )
{	
	m_pDialog->CONTROL( Static, ID_GOLD )->Enable( bEnable );
	m_pDialog->CONTROL( Static, ID_SILVER )->Enable( bEnable );
	m_pDialog->CONTROL( Static, ID_BRONZE )->Enable( bEnable );

	m_pDialog->CONTROL( Static, ID_GOLD )->SetText( L"" );
	m_pDialog->CONTROL( Static, ID_SILVER )->SetText( L"" );
	m_pDialog->CONTROL( Static, ID_BRONZE )->SetText( L"" );
}

//////////////////////////////////////////////////////////////////////////

CDnMoneyInputDlg::CDnMoneyInputDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pStaticTitle(NULL)
	, m_pButtonOK(NULL)
	, m_pButtonCancel(NULL)
	, m_pButtonMax(NULL)
	, m_pButtonInit(NULL)
	, m_MoneyControl( this )
{
}

CDnMoneyInputDlg::~CDnMoneyInputDlg(void)
{
}

void CDnMoneyInputDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MoneyInputDlg.ui" ).c_str(), bShow );
}

void CDnMoneyInputDlg::InitialUpdate()
{
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_STATIC_TITLE");

	m_pButtonOK = GetControl<CEtUIButton>("ID_BUTTON_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	
	m_pButtonMax = GetControl<CEtUIButton>("ID_BUTTON_MAX");
	m_pButtonInit = GetControl<CEtUIButton>("ID_BUTTON_INIT");

	m_MoneyControl.InitialUpdate();
}

void CDnMoneyInputDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( !m_MoneyControl.ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg ) ) {
		return;
	}

	if ( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_MAX" ) )
		{
			m_MoneyControl.SetMax();
			return;
		}
		else if( IsCmdControl("ID_BUTTON_INIT" ) )
		{
			m_MoneyControl.Init();
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMoneyInputDlg::SetInfo( LPCWSTR wszTitle, LPCWSTR wszOK )
{
	m_pStaticTitle->SetText( wszTitle );
	m_pButtonOK->SetText( wszOK );
}

void CDnMoneyInputDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	m_MoneyControl.Show( bShow );

	CEtUIDialog::Show( bShow );	
}

bool CDnMoneyInputDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			return true;
		}
		else if( wParam == VK_RETURN )
		{
			if( m_MoneyControl.IsEditBoxFocus() )
			{
				RequestFocus( m_pButtonOK );
				return true;
			}
		}
	}

	if( m_MoneyControl.MsgProc( hWnd, uMsg, wParam, lParam ) ) {
		return true;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}
