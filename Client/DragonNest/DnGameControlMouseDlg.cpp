#include "StdAfx.h"
#include "DnGameControlMouseDlg.h"
#include "DnGameControlComboDlg.h"
#include "DnLocalPlayerActor.h"
#include "GameOption.h"
#include "DnPlayerCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameControlMouseDlg::CDnGameControlMouseDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: BaseClass( dialogType, pParentDialog, nID, pCallback )
, m_pCheckSmartMove(NULL)
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
, m_pCheckSmartMoveEx(NULL)
#endif
, m_pButtonDown(NULL)
, m_pButtonUp(NULL)
, m_pSliderMouse(NULL)
, m_pStaticMouse(NULL)
, m_pButtonDownCamera(NULL)
, m_pButtonUpCamera(NULL)
, m_pSliderCamera(NULL)
, m_pStaticCamera(NULL)
, m_pButtonDownCursor(NULL)
, m_pButtonUpCursor(NULL)
, m_pSliderCursor(NULL)
, m_pStaticCursor(NULL)
, m_pGameControlComboDlg(NULL)
#if defined(_US)
, m_pStaticMouseInvert(NULL)
, m_pStaticMouseInvertBar(NULL)
, m_pCheckMouseInvert(NULL)
#endif
{
}

CDnGameControlMouseDlg::~CDnGameControlMouseDlg()
{
}

void CDnGameControlMouseDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameControlMouseDlg.ui" ).c_str(), bShow );
}

void CDnGameControlMouseDlg::InitialUpdate()
{
	m_pCheckSmartMove = GetControl<CEtUICheckBox>("ID_CHECKBOX_SMARTMOVE");
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	m_pCheckSmartMoveEx = GetControl<CEtUICheckBox>("ID_CHECKBOX0");
#endif

	m_pButtonDown = GetControl<CEtUIButton>("ID_BUTTON_DOWN");
	m_pButtonUp = GetControl<CEtUIButton>("ID_BUTTON_UP");
	m_pSliderMouse = GetControl<CEtUISlider>("ID_SLIDER_MOUSE");
	m_pStaticMouse = GetControl<CEtUIStatic>("ID_STATIC_MOUSE");

	m_pButtonDownCursor = GetControl<CEtUIButton>("ID_BUTTON_DOWN1");
	m_pButtonUpCursor = GetControl<CEtUIButton>("ID_BUTTON_UP1");
	m_pSliderCursor = GetControl<CEtUISlider>("ID_SLIDER_CURSOR");
	m_pStaticCursor = GetControl<CEtUIStatic>("ID_STATIC_CURSOR");

	m_pButtonDownCamera = GetControl<CEtUIButton>("ID_BUTTON_DOWN2");
	m_pButtonUpCamera = GetControl<CEtUIButton>("ID_BUTTON_UP2");
	m_pSliderCamera = GetControl<CEtUISlider>("ID_SLIDER_CAMERA");
	m_pStaticCamera = GetControl<CEtUIStatic>("ID_STATIC_CAMERA");

#if defined(_US)
	m_pStaticMouseInvert = GetControl<CEtUIStatic>("ID_STATIC5");
	m_pStaticMouseInvertBar = GetControl<CEtUIStatic>("ID_STATIC_BAR5");
	m_pCheckMouseInvert = GetControl<CEtUICheckBox>("ID_CHECKBOX_MINVERT");

	m_pStaticMouseInvert->Show( true );
	m_pStaticMouseInvertBar->Show( true );
	m_pCheckMouseInvert->Show( true );
#endif

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	localActor->SetCursorSpeed( CGameOption::GetInstance().GetCursorSensitivity() );
	localActor->SetCameraSpeed( CGameOption::GetInstance().GetCameraSensitivity() );
}

void CDnGameControlMouseDlg::GetComboDialog(CDnGameControlComboDlg * pDialog)
{
	if( !pDialog )	return;

	m_pGameControlComboDlg = pDialog;

	AddChildDialog( pDialog );
}

void CDnGameControlMouseDlg::ProcessCombo(const int index)
{
	return;
}

void CDnGameControlMouseDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	//int nSelect = m_pGameControlComboDlg ? m_pGameControlComboDlg->GetComboSelectIndex() : CDnGameControlComboDlg::KEYBOARD_MOUSE;
	//if( CDnGameControlComboDlg::KEYBOARD_MOUSE != nSelect )
	//	return;

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_UP" ) )
		{
			int nValue = m_pSliderMouse->GetValue();
			if( (++nValue) > 100 ) nValue = 100;

			m_pSliderMouse->SetValue( nValue );
			m_pStaticMouse->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_DOWN" ) )
		{
			int nValue = m_pSliderMouse->GetValue();
			if( (--nValue) < 0 ) nValue = 0;

			m_pSliderMouse->SetValue( nValue );
			m_pStaticMouse->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_UP1" ) )
		{
			int nValue = m_pSliderCursor->GetValue();
			if( (++nValue) > 100 ) nValue = 100;

			m_pSliderCursor->SetValue( nValue );
			m_pStaticCursor->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_DOWN1" ) )
		{
			int nValue = m_pSliderCursor->GetValue();
			if( (--nValue) < 0 ) nValue = 0;

			m_pSliderCursor->SetValue( nValue );
			m_pStaticCursor->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_UP2" ) )
		{
			int nValue = m_pSliderCamera->GetValue();
			if( (++nValue) > 100 ) nValue = 100;

			m_pSliderCamera->SetValue( nValue );
			m_pStaticCamera->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_DOWN2" ) )
		{
			int nValue = m_pSliderCamera->GetValue();
			if( (--nValue) < 0 ) nValue = 0;

			m_pSliderCamera->SetValue( nValue );
			m_pStaticCamera->SetIntToText( nValue );
			return;
		}
		
	}
	else if( nCommand == EVENT_SLIDER_VALUE_CHANGED )
	{
		if( IsCmdControl( "ID_SLIDER_MOUSE" ) )
		{
			int nValue = m_pSliderMouse->GetValue();
			m_pStaticMouse->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_SLIDER_CURSOR" ) )
		{
			int nValue = m_pSliderCursor->GetValue();
			m_pStaticCursor->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_SLIDER_CAMERA" ) )
		{
			int nValue = m_pSliderCamera->GetValue();
			m_pStaticCamera->SetIntToText( nValue );
			return;
		}
	}
#if defined(_US)
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_CHECKBOX_MINVERT" ) )
		{
			DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
			
			if( hCamera )
			{
				CDnPlayerCamera* pPlayerCamera = dynamic_cast<CDnPlayerCamera*>(hCamera.GetPointer());
				if (pPlayerCamera && m_pCheckMouseInvert)
					pPlayerCamera->SetMouseInvert(m_pCheckMouseInvert->IsChecked());
			}
		}
	}
#endif

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameControlMouseDlg::ExportSetting()
{
	CGameOption::GetInstance().m_bSmartMove = m_pCheckSmartMove->IsChecked();
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	CGameOption::GetInstance().m_bSmartMoveMainMenu = m_pCheckSmartMoveEx->IsChecked();
#endif

#if defined(_US)
	if (m_pCheckMouseInvert)
	CGameOption::GetInstance().SetMouseInvert( m_pCheckMouseInvert->IsChecked() );
#endif

	CGameOption::GetInstance().SetMouseSensitivity(m_pSliderMouse->GetValue()/100.0f);
	CGameOption::GetInstance().SetCursorSensitivity(m_pSliderCursor->GetValue()/100.0f);
	CGameOption::GetInstance().SetCameraSensitivity(m_pSliderCamera->GetValue()/100.0f);
	CGameOption::GetInstance().ApplyControl();
	CGameOption::GetInstance().Save();

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	localActor->SetCursorSpeed( m_pSliderCursor->GetValue()/100.0f );
	localActor->SetCameraSpeed( m_pSliderCamera->GetValue()/100.0f );
}

void CDnGameControlMouseDlg::ImportSetting()
{
	int nValue = (int)( CGameOption::GetInstance().GetMouseSendsitivity() * 10000.f ) + 10;
	if( m_pSliderMouse ) m_pSliderMouse->SetValue( (int)( nValue / 100.f ) );
	if( m_pStaticMouse ) m_pStaticMouse->SetIntToText( (int)( nValue / 100.f ) );

	if( m_pCheckSmartMove )
		m_pCheckSmartMove->SetChecked( CGameOption::GetInstance().m_bSmartMove );
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	if( m_pCheckSmartMoveEx )
		m_pCheckSmartMoveEx->SetChecked( CGameOption::GetInstance().m_bSmartMoveMainMenu );
#endif

	nValue = (int)(CGameOption::GetInstance().GetCursorSensitivity() *10000.0f) + 10;
	if( m_pSliderCursor ) m_pSliderCursor->SetValue( (int)( nValue / 100.f ) );
	if( m_pStaticCursor ) m_pStaticCursor->SetIntToText( (int)( nValue / 100.f ) );

	nValue = (int)(CGameOption::GetInstance().GetCameraSensitivity() *10000.0f) + 10;
	if( m_pSliderCamera ) m_pSliderCamera->SetValue( (int)( nValue / 100.f ) );
	if( m_pStaticCamera ) m_pStaticCamera->SetIntToText( (int)( nValue / 100.f ) );

#if defined(_US)
	if( m_pCheckMouseInvert )
		m_pCheckMouseInvert->SetChecked( CGameOption::GetInstance().GetMouseInvert() );
#endif
}

bool CDnGameControlMouseDlg::IsChanged()
{
	if( !CDnActor::s_hLocalActor ) return false;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	int nValue = (int)( CGameOption::GetInstance().GetMouseSendsitivity() * 10000.f ) + 10;
	int nValueCamera;
	int nValueCursor;

	if( localActor->IsJoypad() )
	{
		nValueCamera = (int)(localActor->GetCameraSpeed() *10000.0f) + 10;
		nValueCursor = (int)(localActor->GetCursorSpeed() *10000.0f) + 10;
	}
	else
	{
		nValueCamera = m_pSliderCamera->GetValue() * 100;
		nValueCursor = m_pSliderCursor->GetValue() * 100;
	}

	if( (int)( nValue / 100.f ) != m_pSliderMouse->GetValue() ||
		(int)( nValueCamera / 100.0f) != m_pSliderCamera->GetValue() ||
		(int)( nValueCursor / 100.0f) != m_pSliderCursor->GetValue() ||
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
		CGameOption::GetInstance().m_bSmartMove != m_pCheckSmartMove->IsChecked() ||
		CGameOption::GetInstance().m_bSmartMoveMainMenu != m_pCheckSmartMoveEx->IsChecked() )
#else
		CGameOption::GetInstance().m_bSmartMove != m_pCheckSmartMove->IsChecked() )
#endif
		return true;
	return false;
}

void CDnGameControlMouseDlg::Show( bool bShow )
{
	if( m_bShow == bShow ) return;

	if( m_pGameControlComboDlg )
		ShowChildDialog(m_pGameControlComboDlg, bShow);

	if( bShow )
	{
		// 단축키로 감도 조절할 수 있으니 창 열릴때 다시 로드한다.
		ImportSetting();
	}
	BaseClass::Show( bShow );
}

void CDnGameControlMouseDlg::ResetDefault()
{
	int nValue = (int)( CGameOption::GetInstance().m_fDefaultMouseSensitivity * 10000.f ) + 10;
	if( m_pSliderMouse ) m_pSliderMouse->SetValue( (int)( nValue / 100.f ) );
	if( m_pStaticMouse ) m_pStaticMouse->SetIntToText( (int)( nValue / 100.f ) );
	m_pCheckSmartMove->SetChecked( CGameOption::GetInstance().m_bDefaultSmartMove );
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	m_pCheckSmartMoveEx->SetChecked( CGameOption::GetInstance().m_bDefaultSmartMoveMainMenu );
#endif
	
	if( m_pSliderCursor ) m_pSliderCursor->SetValue( 10 );
	if( m_pStaticCursor ) m_pStaticCursor->SetIntToText( 10 );

	if( m_pSliderCamera ) m_pSliderCamera->SetValue( 27 );
	if( m_pStaticCamera ) m_pStaticCamera->SetIntToText( 27 );
}