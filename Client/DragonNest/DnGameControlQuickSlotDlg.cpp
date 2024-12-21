#include "StdAfx.h"
#include "DnGameControlQuickSlotDlg.h"
#include "DnGameControlComboDlg.h"
#include "DnLocalPlayerActor.h"
#include "InputWrapper.h"
#include "DnGameOptionTask.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnGameControlQuickSlotDlg::CDnGameControlQuickSlotDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
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

CDnGameControlQuickSlotDlg::~CDnGameControlQuickSlotDlg()
{
}

void CDnGameControlQuickSlotDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameControlQuickSlotDlg.ui" ).c_str(), bShow );
}

void CDnGameControlQuickSlotDlg::InitialUpdate()
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
}

void CDnGameControlQuickSlotDlg::GetComboDialog(CDnGameControlComboDlg * pDialog)
{
	if( !pDialog )	return;

	m_pGameControlComboDlg = pDialog;

	AddChildDialog( pDialog );
}

void CDnGameControlQuickSlotDlg::ProcessCombo(const int index)
{
	m_pStaticIcon->Show( false );
	m_pStaticText->SetText( L"" );
	SetInputMode( false );

	ImportSetting();
}

bool CDnGameControlQuickSlotDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

			// ���â���� ó���ϴ°Ŷ� OnInputReceive�� ���� �ʴ´�.
			// ������ UI���̶� �Ϲ� �Է��̶� ������ ���⶧����,
			// ��� ������ ó���ϴ��� �ᱹ�� �ΰ��� ���·� ������ �ؾ��Ѵ�.
			// �׷��� �׳� �޼��� ���ν������� �ϴ� �ɷ� ó���ϰڴ�.
			// (�����ϰ��� DIK�� ó���ϴ����� �Ѷ����� ���������� �ξ� ���� �� ����.)
			//
			// �׸��� ��ü�� �����Ҷ��� �ᱹ DIK���� VK���� �ϳ��� ��� �ؾ� �ϰ����̹Ƿ�,
			// �⺻ ��ǲ�� DIK�� �����ϵ��� �ϰڴ�.

			// NumLockŰ�� ���������� ���� ��쿣 �ѹ��е��� Ű���� ��ȯ��������Ѵ�.
			//if( !(GetKeyState(VK_NUMLOCK)&0x01) && !(lParam & 0x01000000) )
			//	wParam = _NoNumLockVKToNumLockVK( (BYTE)wParam );
			//BYTE cDik = _ToDIK( (BYTE)wParam );

			// �����Է��� �ƴҶ� wParam�����δ� Ű�� �Ǵ��� �� ��� �Ʒ� �Լ��� �����ԵǾ���.
			bool bTransNumLock = ( !(GetKeyState(VK_NUMLOCK)&0x01) && !(lParam & 0x01000000) ) ? true : false;

			int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;
			if( CDnGameControlComboDlg::KEYBOARD_MOUSE != nSelect 	)
				return true;

			BYTE cDik = _GetCurrentPushKey( bTransNumLock );
			if( !cDik )
			{
				// ������ �� ���� Ű
				m_pStaticIcon->Show( true );
				m_pStaticText->SetTextColor( textcolor::RED );
				m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3156 ) );
				m_bTextHide = true;
				m_fTextHideTime = 3.0f;
			}
			else
			{
				// ������ �� �ִ� Ű

				// �ٸ� ���� ���̰� �ִ°�.
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
					// ����
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

void CDnGameControlQuickSlotDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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

void CDnGameControlQuickSlotDlg::Process( float fElapsedTime )
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

	//�е� �� ����
	if( m_bInputMode )
	{
		int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

		if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
		{
			if( !CDnActor::s_hLocalActor ) return;
			CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

			if( !m_bPressKey )	//�Է°��� ���ٸ�
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

				if( localActor->IsPushJoypadButton( ckeys ) )	// ��� ������ ���� ���
				{
					WCHAR wszTitle[32];
					if( timeGetTime() - m_PressTime > 2000 )	//ù��° Ű ������ ������ 2�ʰ� ������.
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
				else	//�ٷ� ��� �ϸ� �ȴ�.
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

void CDnGameControlQuickSlotDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( m_pGameControlComboDlg )
		ShowChildDialog(m_pGameControlComboDlg, bShow);

	if( bShow )
	{
		// ������ ����� �������̽��� ���� �ε�ǰ� ���� ����Ű ������ �´�.
		// �̶� �̹� Show(false)�Ǿ��ִ� ���¶� ������ Import�Լ��� ȣ���ϴ��� �ؾ� �ɼ�â�� ����� ���̰� �ȴ�.
		// �׷��� ���� Show�ɶ����� Import�ϴ� ����� ���ϰڴ�.(���콺 ���� ������ ����� ������.)
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

void CDnGameControlQuickSlotDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

			if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect )
			{
				m_InputItem[0].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT1];
				m_InputItem[1].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT2];
				m_InputItem[2].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT3];
				m_InputItem[3].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT4];
				m_InputItem[4].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT5];
				m_InputItem[5].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT6];
				m_InputItem[6].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT7];
				m_InputItem[7].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT8];
				m_InputItem[8].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT9];
				m_InputItem[9].m_cKey = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT10];

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
				m_InputItem[9].m_cKey = CInputJoyPad::NULL_VALUE;	m_InputItem[9].m_cKeySecond = CInputJoyPad::NULL_VALUE;

				WCHAR wszTitle[32] = {0, };
				for( int i = 0; i < NUM_INPUT; ++i )
					m_InputItem[i].m_pStaticBox->SetText( wszTitle );
			}
			m_bResetDefaultAllKeySetting = true;
		}
	}
}

void CDnGameControlQuickSlotDlg::ExportSetting()
{
	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

	bool bReqSend = false;
	if( m_bResetDefaultAllKeySetting )	//�⺻�� ����
	{
		if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
		{
			g_UIWrappingKeyData[IW_UI_QUICKSLOT1] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT1];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT2] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT2];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT3] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT3];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT4] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT4];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT5] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT5];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT6] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT6];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT7] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT7];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT8] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT8];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT9] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT9];
			g_UIWrappingKeyData[IW_UI_QUICKSLOT10] = g_DefaultUIWrappingKeyData[IW_UI_QUICKSLOT10];
			GetInterface().ApplyUIHotKey();
		}
		else if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
		{
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT1] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT1] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT2] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT2] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT3] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT3] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT4] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT4] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT5] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT5] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT6] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT6] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT7] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT7] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT8] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT8] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT9] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT9] = CInputJoyPad::NULL_VALUE;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT10] = CInputJoyPad::NULL_VALUE;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT10] = CInputJoyPad::NULL_VALUE;
		}

		bReqSend = true;
		m_bResetDefaultAllKeySetting = false;
	}
	else if( IsChanged() )
	{
		if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
		{
			g_UIWrappingKeyData[IW_UI_QUICKSLOT1] = m_InputItem[0].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT2] = m_InputItem[1].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT3] = m_InputItem[2].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT4] = m_InputItem[3].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT5] = m_InputItem[4].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT6] = m_InputItem[5].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT7] = m_InputItem[6].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT8] = m_InputItem[7].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT9] = m_InputItem[8].m_cKey;
			g_UIWrappingKeyData[IW_UI_QUICKSLOT10] = m_InputItem[9].m_cKey;

			// UI ��Ű�� ��� �������̽��ܿ� ������ �˷��༭ ��Ű�� �ٲ���Ѵ�.
			for( int i = 0; i < NUM_INPUT; ++i )
				GetInterface().SetMainUIHotKey( IW_UI_QUICKSLOT1+i, _ToVK( m_InputItem[i].m_cKey ) );
		}
		else if( CDnGameControlComboDlg::CUSTOM1_PAD == nSelect )
		{
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT1] = m_InputItem[0].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT1] = m_InputItem[0].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT2] = m_InputItem[1].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT2] = m_InputItem[1].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT3] = m_InputItem[2].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT3] = m_InputItem[2].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT4] = m_InputItem[3].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT4] = m_InputItem[3].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT5] = m_InputItem[4].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT5] = m_InputItem[4].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT6] = m_InputItem[5].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT6] = m_InputItem[5].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT7] = m_InputItem[6].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT7] = m_InputItem[6].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT8] = m_InputItem[7].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT8] = m_InputItem[7].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT9] = m_InputItem[8].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT9] = m_InputItem[8].m_cKeySecond;
			g_UIWrappingJoypadData[IW_UI_QUICKSLOT10] = m_InputItem[9].m_cKey;	g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT10] = m_InputItem[9].m_cKeySecond;
		}

		bReqSend = true;
	}

	if( bReqSend )
	{
		GetGameOptionTask().ReqSendKeySetting();
		GetGameOptionTask().ReqSendJoypadSetting();
	}
}

void CDnGameControlQuickSlotDlg::ImportSetting()
{
	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

	if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
	{
		m_InputItem[0].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT1];
		m_InputItem[1].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT2];
		m_InputItem[2].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT3];
		m_InputItem[3].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT4];
		m_InputItem[4].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT5];
		m_InputItem[5].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT6];
		m_InputItem[6].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT7];
		m_InputItem[7].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT8];
		m_InputItem[8].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT9];
		m_InputItem[9].m_cKey = g_UIWrappingKeyData[IW_UI_QUICKSLOT10];

		for( int i = 0; i < NUM_INPUT; ++i )
			m_InputItem[i].m_pStaticBox->SetText( g_szKeyString[m_InputItem[i].m_cKey] );
	}
	else
	{
		m_InputItem[0].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT1];	m_InputItem[0].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT1];
		m_InputItem[1].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT2];	m_InputItem[1].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT2];
		m_InputItem[2].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT3];	m_InputItem[2].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT3];
		m_InputItem[3].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT4];	m_InputItem[3].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT4];
		m_InputItem[4].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT5];	m_InputItem[4].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT5];
		m_InputItem[5].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT6];	m_InputItem[5].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT6];
		m_InputItem[6].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT7];	m_InputItem[6].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT7];
		m_InputItem[7].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT8];	m_InputItem[7].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT8];
		m_InputItem[8].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT9];	m_InputItem[8].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT9];
		m_InputItem[9].m_cKey = g_UIWrappingJoypadData[IW_UI_QUICKSLOT10];	m_InputItem[9].m_cKeySecond = g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT10];

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

bool CDnGameControlQuickSlotDlg::IsChanged()
{
	int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;

	if( CDnGameControlComboDlg::KEYBOARD_MOUSE == nSelect 	)
	{
		if( m_InputItem[0].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT1] ||
			m_InputItem[1].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT2] ||
			m_InputItem[2].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT3] ||
			m_InputItem[3].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT4] ||
			m_InputItem[4].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT5] ||
			m_InputItem[5].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT6] ||
			m_InputItem[6].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT7] ||
			m_InputItem[7].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT8] ||
			m_InputItem[8].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT9] ||
			m_InputItem[9].m_cKey != g_UIWrappingKeyData[IW_UI_QUICKSLOT10] )
			return true;
	}
	else
	{
		if( ( m_InputItem[0].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT1] || m_InputItem[0].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT1] ) || 
			( m_InputItem[1].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT2] || m_InputItem[1].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT2] ) || 
			( m_InputItem[2].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT3] || m_InputItem[2].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT3] ) || 
			( m_InputItem[3].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT4] || m_InputItem[3].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT4] ) || 
			( m_InputItem[4].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT5] || m_InputItem[4].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT5] ) || 
			( m_InputItem[5].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT6] || m_InputItem[5].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT6] ) || 
			( m_InputItem[6].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT7] || m_InputItem[6].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT7] ) || 
			( m_InputItem[7].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT8] || m_InputItem[7].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT8] ) || 
			( m_InputItem[8].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT9] || m_InputItem[8].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT9] ) || 
			( m_InputItem[9].m_cKey != g_UIWrappingJoypadData[IW_UI_QUICKSLOT10] || m_InputItem[9].m_cKeySecond != g_UIWrappingJoypadAssistData[IW_UI_QUICKSLOT10] ) )
			return true;
	}
	return false;
}

void CDnGameControlQuickSlotDlg::ResetDefault()
{
	if( IsInputMode() ) SetInputMode( false );

	GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3105 ), MB_YESNO, 0, this );
}

void CDnGameControlQuickSlotDlg::SetInputMode( bool bMode, bool bProcessBlink )
{
	CDnLocalPlayerActor::LockInput( bMode );
	m_bInputMode = bMode;
	m_bPressKey = false;

	if( bProcessBlink && 0 <= m_nInputModeItemIndex && m_nInputModeItemIndex < NUM_INPUT && m_InputItem[m_nInputModeItemIndex].m_pStaticBox ) {
		m_InputItem[m_nInputModeItemIndex].m_pStaticBox->SetBlink( bMode );
	}
}

bool CDnGameControlQuickSlotDlg::IsUsedKey( BYTE cDIK )
{
	for( int itr = 0; itr < IW_NORMALATTK; ++itr ) {
		if( IW(itr) == cDIK ) return true;
	}

	for( int itr = IW_UI_CHAR; itr < UIWrappingKeyIndex_Amount; ++itr ) {
		if( g_UIWrappingKeyData[itr] == cDIK ) return true;
	}

	for( int itr = 0; itr < NUM_INPUT; ++itr ) {
		if( itr == m_nInputModeItemIndex ) continue;
		if( m_InputItem[itr].m_cKey == cDIK ) return true;;
	}

	return false;
}

bool CDnGameControlQuickSlotDlg::IsUsedButton(BYTE cFirstKey, BYTE cSecondKey)
{
	bool bFirstKey, bSecondKey;
	std::pair<BYTE, BYTE> data;
	for( int itr = 0; itr < WrappingKeyIndex_Amount; ++itr ) {
		data = IW_PAD(itr);
		bFirstKey = bSecondKey = false;

		if( cFirstKey == data.first )	bFirstKey = true;
		if( cSecondKey == data.second )	bSecondKey = true;

		if( bFirstKey && bSecondKey ) return true;
		if( bFirstKey && data.second != CInputJoyPad::NULL_VALUE && cSecondKey == CInputJoyPad::NULL_VALUE ) return true;
	}

	for( int itr = IW_UI_CHAR; itr < UIWrappingKeyIndex_Amount; ++itr ) {
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