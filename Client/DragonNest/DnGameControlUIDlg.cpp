#include "StdAfx.h"
#include "DnGameControlUIDlg.h"
#include "DnGameControlComboDlg.h"
#include "DnLocalPlayerActor.h"
#include "InputWrapper.h"
#include "DnGameOptionTask.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnGameControlUIDlg::CDnGameControlUIDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: BaseClass( dialogType, pParentDialog, nID, pCallback )
, m_pStaticIcon(NULL)
, m_pStaticText(NULL)
, m_bTextHide(false)
, m_fTextHideTime(0.0f)
, m_bInputMode(false)
, m_nInputModeItemIndex(0)
, m_bResetDefaultAllKeySetting(false)
, m_pGameControlComboDlg(NULL)
, m_iPrevSelect(CDnGameControlComboDlg::KEYBOARD_MOUSE)
, m_bPressKey(false)
, m_cKey( 0 )
, m_PressTime( 0 )
{
}

CDnGameControlUIDlg::~CDnGameControlUIDlg()
{
}

void CDnGameControlUIDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameControlUIDlg.ui" ).c_str(), bShow );
}

void CDnGameControlUIDlg::InitialUpdate()
{
	char szControlName[32];
	for( int i = 0; i < NUM_INPUT; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_BOX%d", i);
		m_InputItem[i].m_pStaticBox = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_BUTTON_BOX%d", i);
		m_InputItem[i].m_pButtonBox = GetControl<CEtUIButton>(szControlName);
	}

	m_pStaticIcon = GetControl<CEtUIStatic>("ID_STATIC_ICON");
	m_pStaticText = GetControl<CEtUIStatic>("ID_STATIC_TEXT");

	m_pStaticIcon->Show( false );
	m_pStaticText->SetText( L"" );

	// 툴에서 하이드 시키고 직접 해제하는게 안전해서 이렇게 한다.
	// 차후 디파인 없앨때 툴에있는 기본속성 하이드로 되어있는거 풀어야한다.
	if( GetControl("ID_STATIC_BAR8") ) GetControl("ID_STATIC_BAR8")->Show( true );
	if( GetControl("ID_STATIC8") ) GetControl("ID_STATIC8")->Show( true );
	if( GetControl("ID_STATIC_BOX8") ) GetControl("ID_STATIC_BOX8")->Show( true );
	if( GetControl("ID_BUTTON_BOX8") ) GetControl("ID_BUTTON_BOX8")->Show( true );

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	if( GetControl("ID_STATIC_BAR9") ) GetControl("ID_STATIC_BAR9")->Show( true );
	if( GetControl("ID_STATIC9") ) GetControl("ID_STATIC9")->Show( true );
	if( GetControl("ID_STATIC_BOX9") ) GetControl("ID_STATIC_BOX9")->Show( true );
	if( GetControl("ID_BUTTON_BOX9") ) GetControl("ID_BUTTON_BOX9")->Show( true );
#endif
}

void CDnGameControlUIDlg::GetComboDialog(CDnGameControlComboDlg * pDialog)
{
	if( !pDialog )	return;

	m_pGameControlComboDlg = pDialog;

	AddChildDialog( pDialog );
}

void CDnGameControlUIDlg::ProcessCombo(const int index)
{
	m_pStaticIcon->Show( false );
	m_pStaticText->SetText( L"" );
	SetInputMode( false );

	ImportSetting();
}

bool CDnGameControlUIDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( m_bInputMode )
	{
		if( uMsg == WM_KEYDOWN )
		{
			if( wParam == VK_ESCAPE )
			{
				int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;
				if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
				{
					m_InputItem[m_nInputModeItemIndex].m_cKey = CInputJoyPad::NULL_VALUE;
					m_InputItem[m_nInputModeItemIndex].m_cKeySecond = CInputJoyPad::NULL_VALUE;
					m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText( L"" );
				}

				m_pStaticIcon->Show( false );
				m_pStaticText->SetText( L"" );
				SetInputMode( false );
				return true;
			}

			// 모달창에서 처리하는거라 OnInputReceive로 들어가지 않는다.
			// 어차피 UI쪽이랑 일반 입력이랑 나눠서 쓰기때문에,
			// 어느 곳에서 처리하던지 결국은 두가지 형태로 나눠서 해야한다.
			// 그러니 그냥 메세지 프로시저에서 하는 걸로 처리하겠다.
			// (구현하고보니 DIK로 처리하는편이 넘락등의 문제때문에 훨씬 편할 거 같다.)
			//
			// 그리고 전체를 저장할때는 결국 DIK던지 VK던지 하나를 골라서 해야 일괄적이므로,
			// 기본 인풋인 DIK로 저장하도록 하겠다.

			// NumLock키가 눌려져있지 않을 경우엔 넘버패드쪽 키들을 변환시켜줘야한다.
			//if( !(GetKeyState(VK_NUMLOCK)&0x01) && !(lParam & 0x01000000) )
			//	wParam = _NoNumLockVKToNumLockVK( (BYTE)wParam );
			//BYTE cDik = _ToDIK( (BYTE)wParam );

			// 영문입력이 아닐때 wParam값으로는 키를 판단할 수 없어서 아래 함수를 만들어쓰게되었다.
			bool bTransNumLock = ( !(GetKeyState(VK_NUMLOCK)&0x01) && !(lParam & 0x01000000) ) ? true : false;

			int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;
			if( CDnGameControlComboDlg::KEYBOARD_MOUSE != nSelect 	)
				return true;

			BYTE cDik = _GetCurrentPushKey( bTransNumLock );
			if( !cDik )
			{
				// 변경할 수 없는 키
				m_pStaticIcon->Show( true );
				m_pStaticText->SetTextColor( textcolor::RED );
				m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3156 ) );
				m_bTextHide = true;
				m_fTextHideTime = 3.0f;
			}
			else
			{
				// 변경할 수 있는 키

				// 다른 곳에 쓰이고 있는가.
				if( IsUsedKey( cDik ) )
				{
					m_pStaticIcon->Show( true );
					m_pStaticText->SetTextColor( textcolor::RED );
					m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3157 ) );
					m_bTextHide = true;
					m_fTextHideTime = 3.0f;
				}
				else
				{
					// 설정
					m_InputItem[m_nInputModeItemIndex].m_cKey = cDik;
					m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText( g_szKeyString[cDik] );

					m_pStaticIcon->Show( false );
					m_pStaticText->SetTextColor( textcolor::GREENYELLOW );
					m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3158 ) );
					m_bTextHide = true;
					m_fTextHideTime = 3.0f;
					SetInputMode( false );
				}
			}

			return true;
		}
	}

	return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnGameControlUIDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;
	if( CDnGameControlComboDlg::XBOX360_PAD == nSelect 
#if defined(_JP)
		|| CDnGameControlComboDlg::HANGAME_PAD == nSelect 
#endif
#if defined(_KR)
		|| CDnGameControlComboDlg::GAMMAC_PAD == nSelect 
#endif
		)
		return;

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BUTTON_BOX" ) )
		{
			SetInputMode( false );
			for( int i = 0; i < NUM_INPUT; ++i ) {
				if( m_InputItem[i].m_pButtonBox == pControl ) {
					m_nInputModeItemIndex = i;
					break;
				}
			}
			SetInputMode( true );
			m_pStaticIcon->Show( false );
			m_pStaticText->SetTextColor( textcolor::YELLOW );
			m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3155 ) );
		}
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameControlUIDlg::Process( float fElapsedTime )
{
	BaseClass::Process( fElapsedTime );

	if( !IsShow() )
		return;

	if( !m_pStaticIcon || !m_pStaticText )
		return;

	if( m_bTextHide )
	{
		if( m_fTextHideTime <= 0.0f )
		{
			m_pStaticIcon->Show( false );
			if( m_bInputMode ) {
				m_pStaticText->SetTextColor( textcolor::YELLOW );
				m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3155 ) );
			}
			else {
				m_pStaticText->SetText( L"" );
			}
			m_bTextHide = false;
		}
		else
		{
			m_fTextHideTime -= fElapsedTime;
		}
	}

	//패드 값 설정
	if( m_bInputMode )
	{
		int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

		if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
		{
			if( !CDnActor::s_hLocalActor ) return;
			CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

			if( !m_bPressKey )	//입력값이 없다면
			{
				if( localActor->GetEventJoypadCount() > 0 )	
				{
					BYTE cKey = localActor->GetEventJoypad(0);

					if( IsUsedButton( cKey, cKey ) )
					{
						m_pStaticIcon->Show( true );
						m_pStaticText->SetTextColor( textcolor::RED );
						m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3157 ) );
						m_bTextHide = true;
						m_fTextHideTime = 3.0f;
					}
					else
					{
						m_cKey = cKey;
						m_bPressKey = true;
						m_PressTime = timeGetTime();
					}
				}
			}
			else
			{
				std::pair<BYTE, BYTE> ckeys = std::make_pair(m_cKey, CInputJoyPad::NULL_VALUE);
				BYTE cKeySecond = CInputJoyPad::NULL_VALUE;
				for( DWORD itr = 0; itr < localActor->GetEventJoypadCount(); ++itr ) {
					if( m_cKey != localActor->GetEventJoypad( itr ) )
						cKeySecond = localActor->GetEventJoypad( itr );
				}

				if( localActor->IsPushJoypadButton( ckeys ) )	// 계속 누르고 있을 경우
				{
					WCHAR wszTitle[32];
					if( timeGetTime() - m_PressTime > 2000 )	//첫번째 키 누르고 있은지 2초가 지났다.
					{
						if( cKeySecond == CInputJoyPad::NULL_VALUE )
						{
							wsprintf( wszTitle, L"%s +", IW_PAD_STRING(m_cKey) );
							m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText(wszTitle);
						}
						else
						{
							if( IsUsedButton( m_cKey, cKeySecond ) )
							{
								m_pStaticIcon->Show( true );
								m_pStaticText->SetTextColor( textcolor::RED );
								m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3157 ) );
								m_bTextHide = true;
								m_fTextHideTime = 3.0f;
							}
							else
							{
								m_InputItem[m_nInputModeItemIndex].m_cKey = m_cKey;
								m_InputItem[m_nInputModeItemIndex].m_cKeySecond = cKeySecond;

								wsprintf( wszTitle, L"%s + %s", IW_PAD_STRING(m_InputItem[m_nInputModeItemIndex].m_cKey), IW_PAD_STRING(m_InputItem[m_nInputModeItemIndex].m_cKeySecond) );
								m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText( wszTitle );

								m_pStaticIcon->Show( false );
								m_pStaticText->SetTextColor( textcolor::GREENYELLOW );
								m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3158 ) );
								m_bTextHide = true;
								m_fTextHideTime = 3.0f;
								SetInputMode( false );
							}
						}
					}
					else
					{
						wsprintf( wszTitle, L"%s", IW_PAD_STRING(m_cKey) );
						m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText(wszTitle);
					}
				}
				else	//바로 등록 하면 된다.
				{
					if( IsUsedButton( m_cKey, CInputJoyPad::NULL_VALUE ) )
					{
						WCHAR wszTitle[32] = {0, };
						m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText(wszTitle);

						m_pStaticIcon->Show( false );
						m_pStaticText->SetTextColor( textcolor::RED );
						m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3157 ) );
						m_bTextHide = true;
						m_fTextHideTime = 3.0f;
						SetInputMode( false );
					}
					else
					{
						m_InputItem[m_nInputModeItemIndex].m_cKey = m_cKey;
						m_InputItem[m_nInputModeItemIndex].m_cKeySecond = CInputJoyPad::NULL_VALUE;
						m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetText( IW_PAD_STRING(m_cKey) );

						m_pStaticIcon->Show( false );
						m_pStaticText->SetTextColor( textcolor::GREENYELLOW );
						m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3158 ) );
						m_bTextHide = true;
						m_fTextHideTime = 3.0f;
						SetInputMode( false );
					}
				}
			}
		}
	}
}

void CDnGameControlUIDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( m_pGameControlComboDlg )
		ShowChildDialog(m_pGameControlComboDlg, bShow);

	if( bShow )
	{
		ImportSetting();
	}
	else
	{
		m_pStaticIcon->Show( false );
		m_pStaticText->SetText( L"" );
		if( IsInputMode() ) SetInputMode( false );
		m_bTextHide = false;
		m_bResetDefaultAllKeySetting = false;
	}
	BaseClass::Show( bShow );
}

void CDnGameControlUIDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

			if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect )
			{
				m_InputItem[0].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_CHAR];
				m_InputItem[1].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_COMMUNITY];
				m_InputItem[2].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_INVEN];
				m_InputItem[3].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUEST];
				m_InputItem[4].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_SKILL];
				m_InputItem[5].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_MISSION];
				m_InputItem[6].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_MAP];
				m_InputItem[7].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_HELP];
				m_InputItem[8].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_CHAT];
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
				m_InputItem[9].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_REPUTE];	// 채팅룸이 먼저 릴리즈될테니,
#endif

				for( int i = 0; i < NUM_INPUT; ++i )
					m_InputItem[i].m_pStaticBox->SetText( g_szKeyString[m_InputItem[i].m_cKey] );
			}
			else if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
			{
				m_InputItem[0].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[0].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[1].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[1].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[2].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[2].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[3].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[3].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[4].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[4].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[5].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[5].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[6].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[6].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[7].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[7].m_cKeySecond = CInputJoyPad::NULL_VALUE;
				m_InputItem[8].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[8].m_cKeySecond = CInputJoyPad::NULL_VALUE;
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
				m_InputItem[9].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[9].m_cKeySecond = CInputJoyPad::NULL_VALUE;
#endif

				WCHAR wszTitle[32] = {0, };
				for( int i = 0; i < NUM_INPUT; ++i )
					m_InputItem[i].m_pStaticBox->SetText( wszTitle );
			}

			m_bResetDefaultAllKeySetting = true;
		}
	}
}

void CDnGameControlUIDlg::ExportSetting()
{
	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

	bool bReqSend = false;
	if( m_bResetDefaultAllKeySetting )
	{
		if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)	//기본값 세팅
		{
			g_UIWrappingKeyData[IW_UI_CHAR]			= g_DefaultUIWrappingKeyData[IW_UI_CHAR];
			g_UIWrappingKeyData[IW_UI_COMMUNITY]	= g_DefaultUIWrappingKeyData[IW_UI_COMMUNITY];
			g_UIWrappingKeyData[IW_UI_INVEN]		= g_DefaultUIWrappingKeyData[IW_UI_INVEN];
			g_UIWrappingKeyData[IW_UI_QUEST]		= g_DefaultUIWrappingKeyData[IW_UI_QUEST];
			g_UIWrappingKeyData[IW_UI_SKILL]		= g_DefaultUIWrappingKeyData[IW_UI_SKILL];
			g_UIWrappingKeyData[IW_UI_MISSION]		= g_DefaultUIWrappingKeyData[IW_UI_MISSION];
			g_UIWrappingKeyData[IW_UI_MAP]			= g_DefaultUIWrappingKeyData[IW_UI_MAP];
			g_UIWrappingKeyData[IW_UI_HELP]			= g_DefaultUIWrappingKeyData[IW_UI_HELP];
			g_UIWrappingKeyData[IW_UI_CHAT]			= g_DefaultUIWrappingKeyData[IW_UI_CHAT];
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
			g_UIWrappingKeyData[IW_UI_REPUTE]		= g_DefaultUIWrappingKeyData[IW_UI_REPUTE];
#endif
			GetInterface().ApplyUIHotKey();
		}
		else if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
		{
			g_UIWrappingJoypadData[IW_UI_CHAR] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_CHAR] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_COMMUNITY] = CInputJoyPad::NULL_VALUE;		g_UIWrappingJoypadAssistData[IW_UI_COMMUNITY] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_INVEN] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_INVEN] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUEST] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_QUEST] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_SKILL] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_SKILL] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_MISSION] = CInputJoyPad::NULL_VALUE;		g_UIWrappingJoypadAssistData[IW_UI_MISSION] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_MAP] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_MAP] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_HELP] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_HELP] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_CHAT] = CInputJoyPad::NULL_VALUE;			g_UIWrappingJoypadAssistData[IW_UI_CHAT] = CInputJoyPad::NULL_VALUE;
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
			g_UIWrappingJoypadData[IW_UI_REPUTE] = CInputJoyPad::NULL_VALUE;		g_UIWrappingJoypadAssistData[IW_UI_REPUTE] = CInputJoyPad::NULL_VALUE;
#endif
		}
		bReqSend = true;
		m_bResetDefaultAllKeySetting = false;
	}
	else if( IsChanged() )
	{
		if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
		{
			g_UIWrappingKeyData[IW_UI_CHAR] = m_InputItem[0].m_cKey;
			g_UIWrappingKeyData[IW_UI_COMMUNITY] = m_InputItem[1].m_cKey;
			g_UIWrappingKeyData[IW_UI_INVEN] = m_InputItem[2].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUEST] = m_InputItem[3].m_cKey;
			g_UIWrappingKeyData[IW_UI_SKILL] = m_InputItem[4].m_cKey;
			g_UIWrappingKeyData[IW_UI_MISSION] = m_InputItem[5].m_cKey;
			g_UIWrappingKeyData[IW_UI_MAP] = m_InputItem[6].m_cKey;
			g_UIWrappingKeyData[IW_UI_HELP] = m_InputItem[7].m_cKey;
			g_UIWrappingKeyData[IW_UI_CHAT] = m_InputItem[8].m_cKey;
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
			g_UIWrappingKeyData[IW_UI_REPUTE] = m_InputItem[9].m_cKey;
#endif

			for( int i = 0; i < NUM_INPUT; ++i )
				GetInterface().SetMainUIHotKey( IW_UI_CHAR+i, _ToVK( m_InputItem[i].m_cKey ) );
		}
		else if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
		{
			g_UIWrappingJoypadData[IW_UI_CHAR] = m_InputItem[0].m_cKey;			g_UIWrappingJoypadAssistData[IW_UI_CHAR] = m_InputItem[0].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_COMMUNITY] = m_InputItem[1].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_COMMUNITY] = m_InputItem[1].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_INVEN] = m_InputItem[2].m_cKey;		g_UIWrappingJoypadAssistData[IW_UI_INVEN] = m_InputItem[2].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUEST] = m_InputItem[3].m_cKey;		g_UIWrappingJoypadAssistData[IW_UI_QUEST] = m_InputItem[3].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_SKILL] = m_InputItem[4].m_cKey;		g_UIWrappingJoypadAssistData[IW_UI_SKILL] = m_InputItem[4].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_MISSION] = m_InputItem[5].m_cKey;		g_UIWrappingJoypadAssistData[IW_UI_MISSION] = m_InputItem[5].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_MAP] = m_InputItem[6].m_cKey;			g_UIWrappingJoypadAssistData[IW_UI_MAP] = m_InputItem[6].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_HELP] = m_InputItem[7].m_cKey;			g_UIWrappingJoypadAssistData[IW_UI_HELP] = m_InputItem[7].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_CHAT] = m_InputItem[8].m_cKey;			g_UIWrappingJoypadAssistData[IW_UI_CHAT] = m_InputItem[8].m_cKeySecond;
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
			g_UIWrappingJoypadData[IW_UI_REPUTE] = m_InputItem[9].m_cKey;		g_UIWrappingJoypadAssistData[IW_UI_REPUTE] = m_InputItem[9].m_cKeySecond;
#endif
		}

		bReqSend = true;
	}

	if( bReqSend )
	{
		GetGameOptionTask().ReqSendKeySetting();
		GetGameOptionTask().ReqSendJoypadSetting();
	}
}

void CDnGameControlUIDlg::ImportSetting()
{
	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

	if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
	{
		m_InputItem[0].m_cKey = g_UIWrappingKeyData[IW_UI_CHAR];
		m_InputItem[1].m_cKey = g_UIWrappingKeyData[IW_UI_COMMUNITY];
		m_InputItem[2].m_cKey = g_UIWrappingKeyData[IW_UI_INVEN];
		m_InputItem[3].m_cKey = g_UIWrappingKeyData[IW_UI_QUEST];
		m_InputItem[4].m_cKey = g_UIWrappingKeyData[IW_UI_SKILL];
		m_InputItem[5].m_cKey = g_UIWrappingKeyData[IW_UI_MISSION];
		m_InputItem[6].m_cKey = g_UIWrappingKeyData[IW_UI_MAP];
		m_InputItem[7].m_cKey = g_UIWrappingKeyData[IW_UI_HELP];
		m_InputItem[8].m_cKey = g_UIWrappingKeyData[IW_UI_CHAT];
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
		m_InputItem[9].m_cKey = g_UIWrappingKeyData[IW_UI_REPUTE];
#endif

		for( int i = 0; i < NUM_INPUT; ++i )
			m_InputItem[i].m_pStaticBox->SetText( g_szKeyString[m_InputItem[i].m_cKey] );
	}
	else
	{
		m_InputItem[0].m_cKey = g_UIWrappingJoypadData[IW_UI_CHAR];			m_InputItem[0].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_CHAR];
		m_InputItem[1].m_cKey = g_UIWrappingJoypadData[IW_UI_COMMUNITY];	m_InputItem[1].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_COMMUNITY];
		m_InputItem[2].m_cKey = g_UIWrappingJoypadData[IW_UI_INVEN];		m_InputItem[2].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_INVEN];
		m_InputItem[3].m_cKey = g_UIWrappingJoypadData[IW_UI_QUEST];		m_InputItem[3].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUEST];
		m_InputItem[4].m_cKey = g_UIWrappingJoypadData[IW_UI_SKILL];		m_InputItem[4].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_SKILL];
		m_InputItem[5].m_cKey = g_UIWrappingJoypadData[IW_UI_MISSION];		m_InputItem[5].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_MISSION];
		m_InputItem[6].m_cKey = g_UIWrappingJoypadData[IW_UI_MAP];			m_InputItem[6].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_MAP];
		m_InputItem[7].m_cKey = g_UIWrappingJoypadData[IW_UI_HELP];			m_InputItem[7].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_HELP];
		m_InputItem[8].m_cKey = g_UIWrappingJoypadData[IW_UI_CHAT];			m_InputItem[8].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_CHAT];
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
		m_InputItem[9].m_cKey = g_UIWrappingJoypadData[IW_UI_REPUTE];		m_InputItem[9].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_REPUTE];
#endif

		WCHAR wszTitle[32];
		for( int i = 0; i < NUM_INPUT; ++i )
		{
			CInputJoyPad::NULL_VALUE == m_InputItem[i].m_cKeySecond ?
				wsprintf( wszTitle, L"%s", IW_PAD_STRING(m_InputItem[i].m_cKey) ) :
			wsprintf( wszTitle, L"%s + %s", IW_PAD_STRING(m_InputItem[i].m_cKey), IW_PAD_STRING(m_InputItem[i].m_cKeySecond) );

			m_InputItem[i].m_pStaticBox->SetText( wszTitle );
		}
	}
}

bool CDnGameControlUIDlg::IsChanged()
{
	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

	if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
	{
		if( m_InputItem[0].m_cKey != g_UIWrappingKeyData[IW_UI_CHAR] ||
			m_InputItem[1].m_cKey != g_UIWrappingKeyData[IW_UI_COMMUNITY] ||
			m_InputItem[2].m_cKey != g_UIWrappingKeyData[IW_UI_INVEN] ||
			m_InputItem[3].m_cKey != g_UIWrappingKeyData[IW_UI_QUEST] ||
			m_InputItem[4].m_cKey != g_UIWrappingKeyData[IW_UI_SKILL] ||
			m_InputItem[5].m_cKey != g_UIWrappingKeyData[IW_UI_MISSION] ||
			m_InputItem[6].m_cKey != g_UIWrappingKeyData[IW_UI_MAP] ||
			m_InputItem[7].m_cKey != g_UIWrappingKeyData[IW_UI_HELP] ||
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
			m_InputItem[8].m_cKey != g_UIWrappingKeyData[IW_UI_CHAT] ||
			m_InputItem[9].m_cKey != g_UIWrappingKeyData[IW_UI_REPUTE] )
#else
			m_InputItem[8].m_cKey != g_UIWrappingKeyData[IW_UI_CHAT] )
#endif
			return true;
	}
	else
	{
		if( ( m_InputItem[0].m_cKey != g_UIWrappingJoypadData[IW_UI_CHAR]		|| m_InputItem[0].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_CHAR] ) || 
			( m_InputItem[1].m_cKey != g_UIWrappingJoypadData[IW_UI_COMMUNITY]	|| m_InputItem[1].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_COMMUNITY] ) || 
			( m_InputItem[2].m_cKey != g_UIWrappingJoypadData[IW_UI_INVEN]		|| m_InputItem[2].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_INVEN] ) || 
			( m_InputItem[3].m_cKey != g_UIWrappingJoypadData[IW_UI_QUEST]		|| m_InputItem[3].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUEST] ) || 
			( m_InputItem[4].m_cKey != g_UIWrappingJoypadData[IW_UI_SKILL]		|| m_InputItem[4].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_SKILL] ) || 
			( m_InputItem[5].m_cKey != g_UIWrappingJoypadData[IW_UI_MISSION]	|| m_InputItem[5].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_MISSION] ) || 
			( m_InputItem[6].m_cKey != g_UIWrappingJoypadData[IW_UI_MAP]		|| m_InputItem[6].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_MAP] ) || 
			( m_InputItem[7].m_cKey != g_UIWrappingJoypadData[IW_UI_HELP]		|| m_InputItem[7].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_HELP] ) ||
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
			( m_InputItem[8].m_cKey != g_UIWrappingJoypadData[IW_UI_CHAT]		|| m_InputItem[8].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_CHAT] ) ||
			( m_InputItem[9].m_cKey != g_UIWrappingJoypadData[IW_UI_REPUTE]		|| m_InputItem[9].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_REPUTE] ) )
#else
			( m_InputItem[8].m_cKey != g_UIWrappingJoypadData[IW_UI_CHAT]		|| m_InputItem[8].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_CHAT] ) )
#endif
			return true;
	}
	return false;
}

void CDnGameControlUIDlg::ResetDefault()
{
	if( IsInputMode() ) SetInputMode( false );

	GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3105 ), MB_YESNO, 0, this );
}

void CDnGameControlUIDlg::SetInputMode( bool bMode, bool bProcessBlink )
{
	CDnLocalPlayerActor::LockInput( bMode );
	m_bInputMode = bMode;
	m_bPressKey = false;

	if( bProcessBlink && 0 <= m_nInputModeItemIndex && m_nInputModeItemIndex < NUM_INPUT && m_InputItem[m_nInputModeItemIndex].m_pStaticBox ) {
		m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetBlink( bMode );
	}
}

bool CDnGameControlUIDlg::IsUsedKey( BYTE cDIK )
{
	for( int itr = 0; itr < IW_NORMALATTK; ++itr ) {
		if( IW(itr) == cDIK ) return true;
	}

	for( int itr = IW_UI_QUICKSLOT1; itr < IW_UI_CHAR; ++itr ) {
		if( g_UIWrappingKeyData[itr] == cDIK ) return true;
	}

	for( int itr = IW_UI_QUICKSLOTCHANGE; itr < UIWrappingKeyIndex_Amount; ++itr ) {
		if( g_UIWrappingKeyData[itr] == cDIK ) return true;
	}

	for( int itr = 0; itr < NUM_INPUT; ++itr ) {
		if( itr == m_nInputModeItemIndex ) continue;
		if( m_InputItem[itr].m_cKey == cDIK ) return true;;
	}

	return false;
}

bool CDnGameControlUIDlg::IsUsedButton(BYTE cFirstKey, BYTE cSecondKey)
{
	bool bFirstKey, bSecondKey;
	std::pair<BYTE, BYTE> data;
	for( int itr = 0; itr < WrappingKeyIndex_Amount; ++itr ) {
		data = IW_PAD(itr);
		bFirstKey = bSecondKey = false;

		if( cFirstKey == data.first )	bFirstKey = true;
		if( cSecondKey == data.second )	bSecondKey = true;

		if( bFirstKey && bSecondKey ) return true;
	}

	for( int itr = IW_UI_QUICKSLOT1; itr < IW_UI_CHAR; ++itr ) {
		data = IW_UI(itr);
		bFirstKey = bSecondKey = false;

		if( cFirstKey == data.first )		bFirstKey = true;
		if( cSecondKey == data.second )		bSecondKey = true;

		if( bFirstKey && bSecondKey ) return true;
		if( bFirstKey && data.second != CInputJoyPad::NULL_VALUE && cSecondKey == CInputJoyPad::NULL_VALUE ) return true;
	}	

	for( int itr = IW_UI_QUICKSLOTCHANGE; itr < UIWrappingKeyIndex_Amount; ++itr ) {
		data = IW_UI(itr);
		bFirstKey = bSecondKey = false;

		if( cFirstKey == data.first )		bFirstKey = true;
		if( cSecondKey == data.second )		bSecondKey = true;

		if( bFirstKey && bSecondKey ) return true;
		if( bFirstKey && data.second != CInputJoyPad::NULL_VALUE && cSecondKey == CInputJoyPad::NULL_VALUE ) return true;
	}	

	for( int itr = 0; itr < NUM_INPUT; ++itr ) {
		if( itr == m_nInputModeItemIndex ) continue;
		bFirstKey = bSecondKey = false;

		if( cFirstKey == m_InputItem[itr].m_cKey )			bFirstKey = true;
		if( cSecondKey == m_InputItem[itr].m_cKeySecond)	bSecondKey = true;

		if( bFirstKey && bSecondKey ) return true;
		if( bFirstKey && m_InputItem[itr].m_cKeySecond != CInputJoyPad::NULL_VALUE && cSecondKey == CInputJoyPad::NULL_VALUE ) return true;
	}

	return false;
}