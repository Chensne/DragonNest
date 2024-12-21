#include "StdAfx.h"
#include "DnMessageBox.h"
#include "DnNpcDlg.h"
#include "DnAuthTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMessageBox::CDnMessageBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pButtonOK(NULL)
	, m_pButtonCancel(NULL)
	, m_pButtonRetry(NULL)
	, m_pButtonYes(NULL)
	, m_pButtonNo(NULL)
	, m_pButtonAuthPw(NULL)
	, m_pButtonWaterBottle(NULL)
	, m_pButtonWater(NULL)
	, m_pStaticTitle(NULL)
	, m_BtnType(MB_OK)
	, m_bTopMost(false)
	, m_bSmartMoveNoButton(false)
	, m_bNoOverwrite(false)
	, m_bIgnoreEnterHotkey(false)
	, m_pStaticMessage(NULL)
{
}

CDnMessageBox::~CDnMessageBox(void)
{
}

void CDnMessageBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MessageBox.ui" ).c_str(), bShow );
}

void CDnMessageBox::InitialUpdate()
{
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");
	m_pButtonRetry = GetControl<CEtUIButton>("ID_RETRY");
	m_pButtonYes = GetControl<CEtUIButton>("ID_YES");
	m_pButtonNo = GetControl<CEtUIButton>("ID_NO");
	m_pButtonAuthPw = GetControl<CEtUIButton>("ID_BTN_AUTH_PW");
	m_pButtonWaterBottle = GetControl<CEtUIButton>("ID_BT_BUCKET");
	m_pButtonWater = GetControl<CEtUIButton>("ID_BT_USEPOT");
	m_pStaticMessage = GetControl<CEtUIStatic>("ID_MESSAGE");
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TITLE");

	m_pButtonOK->GetUICoord( m_ButtonCoord[0] );
	m_pButtonCancel->GetUICoord( m_ButtonCoord[1] );
	m_pButtonYes->GetUICoord( m_ButtonCoord[2] );

	m_strTitle = m_pStaticTitle->GetText();
	m_dwTitleColor = m_pStaticTitle->GetTextColor();
}

void CDnMessageBox::SetMessageBox( CDnInterface::InterfaceTypeEnum emType, LPCWSTR pwszMessage, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	if( IsShow() && m_bNoOverwrite )
		return;

	m_bIgnoreEnterHotkey = m_bSmartMoveNoButton = bSmartMoveNoButton;
	m_bNoOverwrite = bNoOverwrite;
	m_bTopMost = bTopMost;
	m_emInterfaceType = emType;
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_pStaticMessage->SetText( pwszMessage );

	ShowControl( false );

	m_vecButtonControl.clear();

	if (bNoBtn)
	{
		m_BtnType = uType;
		return;
	}

	switch( uType )
	{
	case MB_OK:
		m_vecButtonControl.push_back( m_pButtonOK );
		break;
	case MB_OKCANCEL:
		m_vecButtonControl.push_back( m_pButtonOK );
		m_vecButtonControl.push_back( m_pButtonCancel );
		break;
	case MB_RETRYCANCEL:
		m_vecButtonControl.push_back( m_pButtonRetry );
		m_vecButtonControl.push_back( m_pButtonCancel );
		break;
	case MB_YESNO:
		m_vecButtonControl.push_back( m_pButtonYes );
		m_vecButtonControl.push_back( m_pButtonNo );
		break;
	case MB_YESNOCANCEL:
		m_vecButtonControl.push_back( m_pButtonYes );
		m_vecButtonControl.push_back( m_pButtonNo );
		m_vecButtonControl.push_back( m_pButtonCancel );
		break;
	case MB_CANCELTRYCONTINUE: 
		m_vecButtonControl.push_back( m_pButtonAuthPw);
		m_vecButtonControl.push_back( m_pButtonYes );
		m_vecButtonControl.push_back( m_pButtonNo );
		break; 
	case MB_ABORTRETRYIGNORE:	//물주기용
		m_vecButtonControl.push_back( m_pButtonWaterBottle);
		m_vecButtonControl.push_back( m_pButtonWater );
		m_vecButtonControl.push_back( m_pButtonCancel );
		break;



	default:
		ASSERT( 0&&"CDnMessageBox::SetMessageBox" );
		break;
	}

	switch( uType )
	{
	case MB_OK:
		m_vecButtonControl[0]->SetUICoord( m_ButtonCoord[2] );
		break;
	case MB_OKCANCEL:
	case MB_RETRYCANCEL:
	case MB_YESNO:
		m_vecButtonControl[0]->SetUICoord( m_ButtonCoord[1] );
		m_vecButtonControl[1]->SetUICoord( m_ButtonCoord[2] );
		break;
	case MB_YESNOCANCEL:
	case MB_CANCELTRYCONTINUE:
	case MB_ABORTRETRYIGNORE:
		m_vecButtonControl[0]->SetUICoord( m_ButtonCoord[0] );
		m_vecButtonControl[1]->SetUICoord( m_ButtonCoord[1] );
		m_vecButtonControl[2]->SetUICoord( m_ButtonCoord[2] );
		break;
	default:
		ASSERT( 0&&"CDnMessageBox::SetMessageBox" );
		break;
	}

	m_BtnType = uType;

	for( int i = 0; i < ( int )m_vecButtonControl.size(); i++ )
	{
		m_vecButtonControl[ i ]->Show( true );
	}
}

void CDnMessageBox::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( bShow )
	{
		if (m_vecButtonControl.size() > 0)
		{
			if( m_bSmartMoveNoButton && m_BtnType != MB_OK && m_vecButtonControl.size() > 1 && m_vecButtonControl[1] ) 
			{
				if( m_BtnType == MB_CANCELTRYCONTINUE )	//2차 생성버튼 전용 포커스를 처리요청으로
					m_SmartMove.SetControl( m_vecButtonControl[1] );
				else 
					m_SmartMove.SetControl( m_vecButtonControl[1] );

				m_bSmartMoveNoButton = false;	// 1회성 설정으로 끝나게 처리한다.
			}
			else 
			{
				if( m_BtnType == MB_CANCELTRYCONTINUE )
				{
					m_SmartMove.SetControl( m_vecButtonControl[1] );
				}
				else 
				{
					m_SmartMove.SetControl( m_vecButtonControl[0] );
				}

			}
			m_SmartMove.MoveCursor();

			// 모달창의 focus 처리 하다가 focus쪽에 대한 이해가 부족해 우선 중지합니다.
			//std::vector<CEtUIControl*> pControlVec = focus::GetPrevControl();
			//int nSize = pControlVec.size();
			//
			//focus::PushControl(this);
			//focus::SetFocus(m_vecButtonControl[0]);
		}
	}
	else
	{
		//focus::PopControl(this);

		// 아이템 파괴 확인창이라면, Show(false)될때 No콜백을 직접 보내준다.
		if( m_nDialogID == MESSAGEBOX_23 )
			CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonNo, 0 );

		if (m_vecButtonControl.size() > 0)
			m_SmartMove.ReturnCursor();

		SetDialogID(-1);
		SetCallback(NULL);
		SetTitle( m_strTitle.c_str(), m_dwTitleColor );
		m_bNoOverwrite = false;	// 창 닫힐때 초기화
	}

	CEtUIDialog::Show( bShow );
}

void CDnMessageBox::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		Show(false);
	}
}

bool CDnMessageBox::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	// 핫키설정 대신 사용하던 ESC키처리 코드로 예상된다.
	// 이 코드때문에 ESC누르면 Show가 풀리는데,
	// 이로 인해 show되지 않은 컨트롤에 대해 부모의 ProcessCommand가 호출되지 않고,
	// 결과적으로 CDnInvenTabDlg::OnUICallbackProc함수의 m_pTrashItemButton = NULL;이 호출되지 않으면서
	// 아이템 파괴 메세지박스를 한번 ESC하고 나선 계속 나오지 않는 버그로 이어진 것이다.
	//
	// 옵저버 등을 이용해 고칠 수도 있지만,
	// 우선 코드 파악이 덜 된 상태에서 복잡도를 증가시키는건 또 다른 문제를 야기시킬 수 있기때문에,
	// 아래 코드를 주석처리하는 것으로 처리하겠다.

//	if( uMsg == WM_KEYDOWN )
//	{
//		if( wParam == VK_ESCAPE )
//		{
////			CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl("ID_BUTTON_CLOSE"), 0 );
//			Show(false);
//			return true;
//		}
//	}

// 	if (uMsg == WM_KEYDOWN)
// 	{
// 		if (wParam == VK_RETURN)
// 		{
// 			if (m_BtnType == MB_OK)
// 			{
// 				Show(false);
// 				return true;
// 			}
// 		}
// 	}

	if( m_bIgnoreEnterHotkey )
	{
		if( uMsg == WM_KEYDOWN )
		{
			if( m_BtnType == MB_YESNO )
			{
				if( m_pButtonYes->GetHotKey() == wParam )
				{
					ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonNo, 0 );
					return true;
				}
			}
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMessageBox::ShowControl( bool bShow )
{
	m_pButtonOK->Show(bShow);
	m_pButtonCancel->Show(bShow);
	m_pButtonRetry->Show(bShow);
	m_pButtonYes->Show(bShow);
	m_pButtonNo->Show(bShow);
	m_pButtonAuthPw->Show(bShow);
	m_pButtonWater->Show(bShow);
	m_pButtonWaterBottle->Show(bShow);
}

void CDnMessageBox::SetTitle( LPCWSTR wszTitle, DWORD dwColor )
{
	m_pStaticTitle->SetText( wszTitle );
	m_pStaticTitle->SetTextColor( dwColor );
}


//////////////////////////////////////////////////////////////////////////

CDnMessageBoxTextBox::CDnMessageBoxTextBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnMessageBox( dialogType, pParentDialog, nID, pCallback )
, m_pTextMessage( NULL )
{
}

CDnMessageBoxTextBox::~CDnMessageBoxTextBox(void)
{
}

void CDnMessageBoxTextBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MessageBox_TextBox.ui" ).c_str(), bShow );
}

void CDnMessageBoxTextBox::InitialUpdate()
{
	CDnMessageBox::InitialUpdate();
	m_pTextMessage = GetControl<CEtUITextBox>( "ID_TEXTBOX_MESSAGE" );
}

void CDnMessageBoxTextBox::SetMessageBox( CDnInterface::InterfaceTypeEnum emType, LPCWSTR pwszMessage, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	if( IsShow() && m_bNoOverwrite )
		return;

	m_bIgnoreEnterHotkey = m_bSmartMoveNoButton = bSmartMoveNoButton;
	m_bNoOverwrite = bNoOverwrite;
	m_bTopMost = bTopMost;
	m_emInterfaceType = emType;
	m_nDialogID = nID;
	m_pCallback = pCall;
	m_pTextMessage->AddText( pwszMessage );

	ShowControl( false );

	m_vecButtonControl.clear();

	if (bNoBtn)
	{
		m_BtnType = uType;
		return;
	}

	switch( uType )
	{
		case MB_OK:
			m_vecButtonControl.push_back( m_pButtonOK );
			break;
		case MB_OKCANCEL:
			m_vecButtonControl.push_back( m_pButtonOK );
			m_vecButtonControl.push_back( m_pButtonCancel );
			break;
		case MB_RETRYCANCEL:
			m_vecButtonControl.push_back( m_pButtonRetry );
			m_vecButtonControl.push_back( m_pButtonCancel );
			break;
		case MB_YESNO:
			m_vecButtonControl.push_back( m_pButtonYes );
			m_vecButtonControl.push_back( m_pButtonNo );
			break;
		case MB_YESNOCANCEL:
			m_vecButtonControl.push_back( m_pButtonYes );
			m_vecButtonControl.push_back( m_pButtonNo );
			m_vecButtonControl.push_back( m_pButtonCancel );
			break;
		case MB_CANCELTRYCONTINUE: 
			m_vecButtonControl.push_back( m_pButtonAuthPw);
			m_vecButtonControl.push_back( m_pButtonYes );
			m_vecButtonControl.push_back( m_pButtonNo );
			break; 
		case MB_ABORTRETRYIGNORE:	//물주기용
			m_vecButtonControl.push_back( m_pButtonWaterBottle);
			m_vecButtonControl.push_back( m_pButtonWater );
			m_vecButtonControl.push_back( m_pButtonCancel );
			break;
		default:
			ASSERT( 0&&"CDnMessageBox::SetMessageBox" );
			break;
	}

	switch( uType )
	{
		case MB_OK:
			m_vecButtonControl[0]->SetUICoord( m_ButtonCoord[2] );
			break;
		case MB_OKCANCEL:
		case MB_RETRYCANCEL:
		case MB_YESNO:
			m_vecButtonControl[0]->SetUICoord( m_ButtonCoord[1] );
			m_vecButtonControl[1]->SetUICoord( m_ButtonCoord[2] );
			break;
		case MB_YESNOCANCEL:
		case MB_CANCELTRYCONTINUE:
		case MB_ABORTRETRYIGNORE:
			m_vecButtonControl[0]->SetUICoord( m_ButtonCoord[0] );
			m_vecButtonControl[1]->SetUICoord( m_ButtonCoord[1] );
			m_vecButtonControl[2]->SetUICoord( m_ButtonCoord[2] );
			break;
		default:
			ASSERT( 0&&"CDnMessageBox::SetMessageBox" );
			break;
	}

	m_BtnType = uType;

	for( int i = 0; i < ( int )m_vecButtonControl.size(); i++ )
	{
		m_vecButtonControl[ i ]->Show( true );
	}
}

void CDnMessageBoxTextBox::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CDnMessageBox::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_TEXTBOX_SELECTION )
	{
		if( IsShow() && IsCmdControl( "ID_TEXTBOX_MESSAGE" ) )
		{
			CWord selectedWord = m_pTextMessage->GetSelectedWordData();
			if( selectedWord.m_strWordWithTag.empty() == false )
				EtInterface::GetNameLinkMgr().TranslateText( std::wstring(), selectedWord.m_strWordWithTag.c_str(), this );
		}
	}
}

#include <exdisp.h>
CComQIPtr<IWebBrowser2> g_pWebBrowser;
HWND					g_hwndWebBrowser;

void CDnMessageBoxTextBox::OnOpenHyperLink( const std::wstring& argString )
{
	HRESULT hr;
	hr = CoCreateInstance( CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (LPVOID*)&g_pWebBrowser );

	if( FAILED(hr) )
		return;

	g_pWebBrowser->get_HWND( (long*)&g_hwndWebBrowser );

	CComVariant vUrl( argString.c_str() ), vEmpty;
	VARIANT vFlags;
	V_VT(&vFlags) = VT_I4;
	V_I4(&vFlags) = navNoReadFromCache;
	hr = g_pWebBrowser->Navigate2( &vUrl, &vFlags, &vEmpty, &vEmpty, &vEmpty );

	if( SUCCEEDED(hr) )
	{
		g_pWebBrowser->put_Visible( VARIANT_TRUE );
		ShowWindow( (HWND)g_hwndWebBrowser, SW_SHOW );
		SetForegroundWindow( (HWND)g_hwndWebBrowser );
		SetActiveWindow( (HWND)g_hwndWebBrowser );
		InvalidateRect( g_hwndWebBrowser, NULL, true );
	}
}