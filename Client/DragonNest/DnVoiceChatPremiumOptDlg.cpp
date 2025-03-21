#include "StdAfx.h"
#include "DnVoiceChatPremiumOptDlg.h"
#ifdef _USE_VOICECHAT
#include "VoiceChatClient.h"
#include "DNVoiceChatTask.h"
#include "GameOption.h"
#include "DnInterface.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef _USE_VOICECHAT
CDnVoiceChatPremiumOptDlg::CDnVoiceChatPremiumOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pCheckSurround(NULL)
, m_pComboVoiceFont(NULL)
, m_pButtonTest(NULL)
, m_pStaticTest(NULL)
, m_fTestTime(0.0f)
, m_nTestMode(0)
, m_pButtonDown(NULL)
, m_pButtonUp(NULL)
, m_pSliderPitch(NULL)
, m_pStaticPitch(NULL)
, m_pButtonDown2(NULL)
, m_pButtonUp2(NULL)
, m_pSliderTimbre(NULL)
, m_pStaticTimbre(NULL)
{
}

CDnVoiceChatPremiumOptDlg::~CDnVoiceChatPremiumOptDlg()
{
}

void CDnVoiceChatPremiumOptDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameVoiceChat_Premium.ui" ).c_str(), bShow );
}

void CDnVoiceChatPremiumOptDlg::InitialUpdate()
{
	m_pCheckSurround = GetControl<CEtUICheckBox>("ID_CHECKBOX_SURROUND");
	m_pComboVoiceFont = GetControl<CEtUIComboBox>("ID_COMBOBOX_VOICEFONT");

	m_pButtonTest = GetControl<CEtUIButton>("ID_BUTTON_TEST");
	m_pStaticTest = GetControl<CEtUIStatic>("ID_STATIC_MICTEST");
	m_pStaticTest->SetText( L"" );

	m_pButtonDown = GetControl<CEtUIButton>("ID_BUTTON_DOWN");
	m_pButtonUp = GetControl<CEtUIButton>("ID_BUTTON_UP");
	m_pSliderPitch = GetControl<CEtUISlider>("ID_SLIDER_PITCH");
	m_pStaticPitch = GetControl<CEtUIStatic>("ID_STATIC_PITCH");

	m_pButtonDown2 = GetControl<CEtUIButton>("ID_BUTTON_DOWN2");
	m_pButtonUp2 = GetControl<CEtUIButton>("ID_BUTTON_UP2");
	m_pSliderTimbre = GetControl<CEtUISlider>("ID_SLIDER_TIMBRE");
	m_pStaticTimbre = GetControl<CEtUIStatic>("ID_STATIC_TIMBRE");


#if defined(_RDEBUG) || defined(_DEBUG) || defined(_KR)
	m_pCheckSurround->SetChecked( true );
	m_pCheckSurround->Enable( false );
#endif

	m_pComboVoiceFont->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3220 ), NULL, CVoiceChatClient::VoiceFontNone );
	m_pComboVoiceFont->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3221 ), NULL, CVoiceChatClient::VoiceFontW2M );
	m_pComboVoiceFont->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3222 ), NULL, CVoiceChatClient::VoiceFontM2W );
	m_pComboVoiceFont->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3223 ), NULL, CVoiceChatClient::VoiceFontELF );
	m_pComboVoiceFont->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3224 ), NULL, CVoiceChatClient::VoiceFontCustom );
}

void CDnVoiceChatPremiumOptDlg::Show( bool bShow ) 
{
	if( bShow == m_bShow )
		return;

	CEtUIDialog::Show( bShow );

	// 창을 닫거나 열때 변경된 것이 있을테니 다시 보이스폰트 체크를 한다. - 아직 Surround관련해서 설정하는건 없다.
	bool bPremium = false;
	if( CDnVoiceChatTask::IsActive() )
		bPremium = GetVoiceChatTask().CheckVoiceFont();

	if( bShow )
	{
		// 처음 열릴때 현재상태 확인해봐야한다.
		CheckEnableControl();

		// 저장된 내용대로 UI다시 Import
		if( bPremium )
		{
			m_pComboVoiceFont->SetSelectedByValue( CGameOption::GetInstance().m_nVoiceFont );
			m_pSliderPitch->SetValue( ToSliderValue( CGameOption::GetInstance().m_fVoicePitch ) );
			m_pSliderTimbre->SetValue( ToSliderValue( CGameOption::GetInstance().m_fVoiceTimbre ) );
		}
		else
		{
			// 프리미엄이 아닐때는 강제로 기본값으로 돌려두자.(재접하면 다 제대로 되는데.. 그래도 해주자.)
			m_pComboVoiceFont->SetSelectedByValue( CVoiceChatClient::VoiceFontNone );
			m_pSliderPitch->SetValue( ToSliderValue( 1.0f ) );
			m_pSliderTimbre->SetValue( ToSliderValue( 1.0f ) );
		}
	}
	else
	{
		GetControl<CEtUIButton>("ID_BUTTON_APPLY")->Enable( true );
		if( CVoiceChatClient::IsActive() && m_nTestMode != 0 ) CVoiceChatClient::GetInstance().StopMicTest();
	}
}

void CDnVoiceChatPremiumOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_APPLY") )
		{
			// 저장.
			int nVoiceFont = 0;
			m_pComboVoiceFont->GetSelectedValue( nVoiceFont );
			CGameOption::GetInstance().m_nVoiceFont = nVoiceFont;
			CGameOption::GetInstance().m_fVoicePitch = ToCustomValue( m_pSliderPitch->GetValue() );
			CGameOption::GetInstance().m_fVoiceTimbre = ToCustomValue( m_pSliderTimbre->GetValue() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060) );
			return;
		}
		else if( IsCmdControl("ID_BUTTON_CLOSE") || IsCmdControl("ID_BUTTON_CANCEL") )
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_BUTTON_TEST") )
		{
			m_fTestTime = 5.0f;
			m_nTestMode = 1;
			m_pButtonTest->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3183 ) );
			m_pButtonTest->Enable( false );

			m_pComboVoiceFont->Enable( false );
			m_pButtonDown->Enable( false );
			m_pButtonUp->Enable( false );
			m_pSliderPitch->Enable( false );
			m_pStaticPitch->Enable( false );
			m_pButtonDown2->Enable( false );
			m_pButtonUp2->Enable( false );
			m_pSliderTimbre->Enable( false );
			m_pStaticTimbre->Enable( false );

			if( CVoiceChatClient::IsActive() ) CVoiceChatClient::GetInstance().StartMicTestPhase1();
		}
		else if( IsCmdControl("ID_BUTTON_UP") )
		{
			float fPitch = ToCustomValue( m_pSliderPitch->GetValue() );
			fPitch += 0.1f;
			if( fPitch > 2.0f ) fPitch = 2.0f;
			m_pSliderPitch->SetValue( ToSliderValue( fPitch ) );	// 슬라이더 바뀌면 아래서 알아서 보이스클라이언트에 적용시킨다.
			return;
		}
		else if( IsCmdControl("ID_BUTTON_DOWN") )
		{
			float fPitch = ToCustomValue( m_pSliderPitch->GetValue() );
			fPitch -= 0.1f;
			if( fPitch < 0.0f ) fPitch = 0.0f;
			m_pSliderPitch->SetValue( ToSliderValue( fPitch ) );
			return;
		}
		else if( IsCmdControl("ID_BUTTON_UP2") )
		{
			float fTimbre = ToCustomValue( m_pSliderTimbre->GetValue() );
			fTimbre += 0.1f;
			if( fTimbre > 2.0f ) fTimbre = 2.0f;
			m_pSliderTimbre->SetValue( ToSliderValue( fTimbre ) );
			return;
		}
		else if( IsCmdControl("ID_BUTTON_DOWN2") )
		{
			float fTimbre = ToCustomValue( m_pSliderTimbre->GetValue() );
			fTimbre -= 0.1f;
			if( fTimbre < 0.0f ) fTimbre = 0.0f;
			m_pSliderTimbre->SetValue( ToSliderValue( fTimbre ) );
			return;
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser )
	{
		if( IsCmdControl( "ID_COMBOBOX_VOICEFONT" ) )
		{
			CEtUIComboBox *pComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_VOICEFONT");
			int nValue = 0;
			pComboBox->GetSelectedValue( nValue );
			

			bool bSlider = (nValue == CVoiceChatClient::VoiceFontCustom);
			m_pButtonDown->Enable( bSlider );
			m_pButtonUp->Enable( bSlider );
			m_pSliderPitch->Enable( bSlider );
			m_pStaticPitch->Enable( bSlider );
			m_pButtonDown2->Enable( bSlider );
			m_pButtonUp2->Enable( bSlider );
			m_pSliderTimbre->Enable( bSlider );
			m_pStaticTimbre->Enable( bSlider );

			float fPitch = 1.0f;
			float fTimbre = 1.0f;
			if( bSlider )
			{
				fPitch = ToCustomValue( m_pSliderPitch->GetValue() );
				fTimbre = ToCustomValue( m_pSliderTimbre->GetValue() );
			}
			if( CVoiceChatClient::IsActive() ) CVoiceChatClient::GetInstance().SetVoiceFont( (CVoiceChatClient::eVoiceFontType)nValue, fPitch, fTimbre );
		}
	}
	else if( nCommand == EVENT_SLIDER_VALUE_CHANGED )
	{
		if( IsCmdControl( "ID_SLIDER_PITCH" ) )
		{
			float fPitch = ToCustomValue( m_pSliderPitch->GetValue() );
			float fTimbre = ToCustomValue( m_pSliderTimbre->GetValue() );

			WCHAR wszTemp[256] = {0,};
			swprintf_s( wszTemp, _countof(wszTemp), L"%.2f", fPitch );
			m_pStaticPitch->SetText( wszTemp );

			if( CVoiceChatClient::IsActive() ) CVoiceChatClient::GetInstance().SetVoiceFont( CVoiceChatClient::VoiceFontCustom, fPitch, fTimbre );
		}
		else if( IsCmdControl( "ID_SLIDER_TIMBRE" ) )
		{
			float fPitch = ToCustomValue( m_pSliderPitch->GetValue() );
			float fTimbre = ToCustomValue( m_pSliderTimbre->GetValue() );

			WCHAR wszTemp[256] = {0,};
			swprintf_s( wszTemp, _countof(wszTemp), L"%.2f", fTimbre );
			m_pStaticTimbre->SetText( wszTemp );

			if( CVoiceChatClient::IsActive() ) CVoiceChatClient::GetInstance().SetVoiceFont( CVoiceChatClient::VoiceFontCustom, fPitch, fTimbre );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnVoiceChatPremiumOptDlg::CheckEnableControl()
{
	bool bMic = ( CDnVoiceChatTask::IsActive() && GetVoiceChatTask().IsInitialized() &&
		CVoiceChatClient::IsActive() && CVoiceChatClient::GetInstance().IsExistMic() );

	m_pComboVoiceFont->Enable( bMic );
	m_pButtonTest->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3181 ) );
	m_pButtonTest->Enable( bMic );
	if( bMic ) m_pStaticTest->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3225 ) );
	else m_pStaticTest->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3226 ) );

	bool bSlider = false;
	int nVoiceFont = 0;
	m_pComboVoiceFont->GetSelectedValue( nVoiceFont );
	if( bMic && nVoiceFont == CVoiceChatClient::VoiceFontCustom )
		bSlider = true;
	m_pButtonDown->Enable( bSlider );
	m_pButtonUp->Enable( bSlider );
	m_pSliderPitch->Enable( bSlider );
	m_pStaticPitch->Enable( bSlider );

	m_pButtonDown2->Enable( bSlider );
	m_pButtonUp2->Enable( bSlider );
	m_pSliderTimbre->Enable( bSlider );
	m_pStaticTimbre->Enable( bSlider );

	// 테스트중이었다면 초기화
	m_fTestTime = 0.0f;
	m_nTestMode = 0;
}

void CDnVoiceChatPremiumOptDlg::DisableApplyButton()
{
	GetControl<CEtUIButton>("ID_BUTTON_APPLY")->Enable( false );
}

int CDnVoiceChatPremiumOptDlg::ToSliderValue( float fCustomValue )
{
	const float fMin = 0.5f;
	const float fMax = 2.0f;
	return (int)( (fCustomValue - fMin) / (fMax - fMin) * 100.0f );
}

float CDnVoiceChatPremiumOptDlg::ToCustomValue( int nSliderValue )
{
	const float fMin = 0.5f;
	const float fMax = 2.0f;
	return ((nSliderValue / 100.0f) * (fMax - fMin) + fMin);
}

void CDnVoiceChatPremiumOptDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	if( !IsShow() ) return;

	bool bMic = ( CDnVoiceChatTask::IsActive() && GetVoiceChatTask().IsInitialized() &&
		CVoiceChatClient::IsActive() && CVoiceChatClient::GetInstance().IsExistMic() );
	if( !bMic ) return;

	// 테스트 검사
	if( m_fTestTime > 0.0f )
	{
		m_fTestTime -= fElapsedTime;

		WCHAR wszText[256] = {0,};
		if( m_nTestMode == 1 )
		{
			swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3184 ), (int)m_fTestTime );
			m_pStaticTest->SetText( wszText );

			// 테스트 종료
			if( m_fTestTime <= 0.0f )
			{
				m_fTestTime = 5.0f;
				m_nTestMode = 2;
				m_pButtonTest->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3183 ) );
				if( CVoiceChatClient::IsActive() ) CVoiceChatClient::GetInstance().StartMicTestPhase2( test_finished_callback );
			}
		}
		else if( m_nTestMode == 2 )
		{
			swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3227 ), (int)m_fTestTime );
			m_pStaticTest->SetText( wszText );

			// 테스트 종료
			if( m_fTestTime <= 0.0f )
			{
				m_nTestMode = 0;
				CheckEnableControl();
			}
		}
	}
}

void CDnVoiceChatPremiumOptDlg::test_finished_callback(void* rock)
{
}
#endif