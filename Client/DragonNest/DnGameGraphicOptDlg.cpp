#include "StdAfx.h"
#include "DnGameGraphicOptDlg.h"
#include "DnMainFrame.h"
#include "DnWorld.h"
#include "GameOption.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnGameGraphicConfirmDlg.h"
#include "DnPlayerActor.h"
#include "DnPlayerCamera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameGraphicOptDlg::CDnGameGraphicOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnOptionDlg( dialogType, pParentDialog, nID, pCallback )
	, m_pButtonBrightDown(NULL)
	, m_pButtonBrightUp(NULL)
	, m_pStaticBright(NULL)
	, m_pComboBoxQuality(NULL)
	, m_pComboBoxResolution(NULL)
	, m_pSliderBright(NULL)
	, m_nResolutionBack(0)
	, m_nQualityBack(0)
	, m_pWindowMode(NULL) 
	, m_pComboBoxUISize(NULL)
	, m_nUISizeBack(0)
	, m_nCurSelectedUISize(0)
	, m_pGameGraphicConfirmDlg(NULL)
#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	, m_pComboBoxTextureQuality(NULL)
	, m_nTextureQualityBack(0)
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	, m_pComboBoxEffectQuality(NULL)
	, m_nEffectQualityBack(0)
#endif // PRE_MOD_OPTION_EFFECT_QUALITY

#ifdef PRE_ADD_37745
	, m_pHideMainWeapon(NULL)
#endif
	, m_pComboBoxScreenShotFormat(NULL)
	, m_nScreenShotBack(0)
	, m_pCheckVSync(NULL)
#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	, m_pCheckExclusivePlayerEffect(NULL)
#endif
	, m_bConfirmMessageBox( false )
	, m_bPreVSync( false )
	, m_bPreWindow( false )
	, m_nPreResolutionIndex( 0 )
	, m_pExtendView(NULL)
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	//, m_pComboBoxMemoryOptimize(NULL)
#endif
{
}

CDnGameGraphicOptDlg::~CDnGameGraphicOptDlg(void)
{
	SAFE_DELETE( m_pGameGraphicConfirmDlg );
}

void CDnGameGraphicOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameGraphicOptDlg.ui" ).c_str(), bShow );
}

void CDnGameGraphicOptDlg::InitialUpdate()
{

	//hide invalid options:
	GetControl<CEtUIStatic>("ID_STATIC25");
	GetControl<CEtUIStatic>("ID_STATIC26");
	GetControl<CEtUIComboBox>("ID_COMBOBOX_EFFECT")->Show(false);
	GetControl<CEtUIComboBox>("ID_COMBOBOX_VIEW")->Show(false);
	//

#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	m_pComboBoxTextureQuality = GetControl<CEtUIComboBox>("ID_COMBOBOX_TEXTUREQUALITY");
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	m_pComboBoxEffectQuality = GetControl<CEtUIComboBox>("ID_COMBOBOX_EFFECTQUALITY");
#endif

#ifdef PRE_ADD_37745
	m_pHideMainWeapon =  static_cast< CEtUICheckBox *>( GetControl( "ID_CHECKBOX_WEAPON" ));
#endif

	m_pExtendView = static_cast< CEtUICheckBox *>(GetControl("ID_CHECKBOX_VIEW"));

	m_pComboBoxQuality = GetControl<CEtUIComboBox>("ID_COMBOBOX_QUALITY");
	m_pComboBoxResolution = GetControl<CEtUIComboBox>("ID_COMBOBOX_RESOLUTION");
	m_pComboBoxUISize = GetControl<CEtUIComboBox>("ID_COMBOBOX_UISIZE");
	m_pComboBoxScreenShotFormat = GetControl<CEtUIComboBox>("ID_COMBOBOX_SCREEN");

	m_pButtonBrightDown = GetControl<CEtUIButton>("ID_BUTTON_BRIGHT_DOWN");
	m_pButtonBrightUp = GetControl<CEtUIButton>("ID_BUTTON_BRIGHT_UP");
	m_pSliderBright = GetControl<CEtUISlider>("ID_SLIDER_BRIGHT");
	m_pStaticBright = GetControl<CEtUIStatic>("ID_STATIC_BRIGHT");
	m_pWindowMode = static_cast< CEtUICheckBox * >( GetControl( "ID_CHECKBOX_WINDOW" ) );

	m_pCheckVSync = GetControl<CEtUICheckBox>("ID_CHECKBOX_VSYNC");

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	m_pCheckExclusivePlayerEffect = GetControl<CEtUICheckBox>("ID_CHECKBOX_EFFECT");
#endif

	m_pGameGraphicConfirmDlg = new CDnGameGraphicConfirmDlg( UI_TYPE_MODAL, NULL, 0, this );
	m_pGameGraphicConfirmDlg->Initialize( false );

	m_pComboBoxUISize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3055 ), NULL, 1 );
	m_pComboBoxUISize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3056 ), NULL, 2 );
	m_pComboBoxUISize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3057 ), NULL, 3 );
	m_pComboBoxUISize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3058 ), NULL, 4 );

	m_pComboBoxScreenShotFormat->AddItem( L"JPG", NULL, D3DXIFF_JPG );
	m_pComboBoxScreenShotFormat->AddItem( L"PNG", NULL, D3DXIFF_PNG );
	m_pComboBoxScreenShotFormat->AddItem( L"TGA", NULL, D3DXIFF_TGA );

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// 	m_pComboBoxMemoryOptimize = GetControl<CEtUIComboBox>("ID_COMBOBOX_MEMORY");
// 	m_pComboBoxMemoryOptimize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3055 ), NULL, 1 );
// 	m_pComboBoxMemoryOptimize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3056 ), NULL, 2 );
// 	m_pComboBoxMemoryOptimize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3057 ), NULL, 3 );
// 	m_pComboBoxMemoryOptimize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3058 ), NULL, 4 );
// 	m_pComboBoxMemoryOptimize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3482 ), NULL, 5 );
// 	m_pComboBoxMemoryOptimize->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3483 ), NULL, 6 );
#endif

#ifdef PRE_ADD_VSYNC_OFF
	m_pCheckVSync->Enable( false );
#endif // #ifdef PRE_ADD_VSYNC_OFF
}

void CDnGameGraphicOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{

		if( IsCmdControl("ID_BUTTON_BRIGHT_UP" ) )
		{
			int nSliderValue = m_pSliderBright->GetValue();
			if( (++nSliderValue) > 100 ) nSliderValue = 100;

			m_pSliderBright->SetValue( nSliderValue );
			m_pStaticBright->SetIntToText( nSliderValue );
			GetEtDevice()->SetGammaRamp( 1.0f, ( nSliderValue - 50 ) * 2 );
			return;
		}

		if( IsCmdControl("ID_BUTTON_BRIGHT_DOWN" ) )
		{
			int nSliderValue = m_pSliderBright->GetValue();
			if( (--nSliderValue) < 0 ) nSliderValue = 0;

			m_pSliderBright->SetValue( nSliderValue );
			m_pStaticBright->SetIntToText( nSliderValue );
			GetEtDevice()->SetGammaRamp( 1.0f, ( nSliderValue - 50 ) * 2 );
			return;
		}

		if( IsCmdControl("ID_BUTTON_APPLY") )
		{
			// 게임옵션의 적용버튼만 별도 처리한다.(풀스크린 변경시 확인메세지박스때문에)
			ExportSetting();
			SetUISize();	// 적용 누른 후 바로 UISize컨트롤 활성-비활성 결정되게 강제로 호출.
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// 			CDnActionBase::SetMemoryOptimize( CGameOption::GetInstance().m_nMemoryOptimize );	// 재시작안해도 적용되게 설정해둔다.
#endif
			if( m_bConfirmMessageBox ) {
				m_pGameGraphicConfirmDlg->Show( true );
			}
			else {
				CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
			}
			return;
		}
	}
	else if( nCommand == EVENT_SLIDER_VALUE_CHANGED )
	{
		if( IsCmdControl( "ID_SLIDER_BRIGHT" ) )
		{
			int nSliderValue = m_pSliderBright->GetValue();
			m_pStaticBright->SetIntToText( nSliderValue );

			GetEtDevice()->SetGammaRamp( 1.0f, ( nSliderValue - 50 ) * 2 );
			return;
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser ) 
	{
		if( IsCmdControl("ID_COMBOBOX_UISIZE") )
		{
			int nStep = 0;
			if( m_pComboBoxUISize->GetSelectedValue(nStep) )
			{
				if( m_nCurSelectedUISize != nStep )
				{
					BaseClass::SetDialogSize( nStep );
					m_nCurSelectedUISize = nStep;
				}
			}
			return;
		}
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameGraphicOptDlg::SetResolution()
{
	m_pComboBoxResolution->RemoveAllItems();

	CEtDevice *pEtDevice = GetEtDevice();
	if( !pEtDevice ) return;

	int nWidth = pEtDevice->Width();
	int nHeight = pEtDevice->Height();
	
	D3DDISPLAYMODE *pDisplayMode(NULL);
	wchar_t szTemp[256]={0};

	UINT nPrevWidth(0);
	UINT nPrevHeight(0);

	int nCount = pEtDevice->GetDisplayModeCount();

	for( int i=0, j=0; i<nCount; i++ )
	{
		pDisplayMode = pEtDevice->GetDisplayMode( i );
		if( !pDisplayMode ) continue;

		// Note : 기본 해상도 보다 작은 모드는 지원하지 않는다.
		//
		if( (pDisplayMode->Width >= 1024 ) && (pDisplayMode->Height >= 720 ) )
		{
			if( nPrevWidth != pDisplayMode->Width || nPrevHeight != pDisplayMode->Height )
			{
				if( ( pDisplayMode->Height / ( float )pDisplayMode->Width ) < 0.7f )
				{
					swprintf_s( szTemp, 256, L"%dx%d (Wide)", pDisplayMode->Width, pDisplayMode->Height );
				}
				else
				{
					swprintf_s( szTemp, 256, L"%dx%d", pDisplayMode->Width, pDisplayMode->Height );
				}
				m_pComboBoxResolution->AddItem( szTemp, pDisplayMode, 0 );

				nPrevWidth = pDisplayMode->Width;
				nPrevHeight = pDisplayMode->Height;

				if( (nWidth == nPrevWidth) && (nHeight == nPrevHeight) )
				{
					m_nResolutionBack = j;
				}

				j++;
			}
		}
	}

	m_pComboBoxResolution->SetSelectedByIndex( m_nResolutionBack );
}

void CDnGameGraphicOptDlg::SetBright()
{
	m_pSliderBright->SetValue( CGameOption::GetInstance().m_nBrightness + 50 );
	int nSliderValue = m_pSliderBright->GetValue();
	m_pStaticBright->SetIntToText( nSliderValue );

	if( CGameOption::GetInstance().m_bWindow )
	{
		m_pSliderBright->Enable( false );
		m_pButtonBrightDown->Enable( false );
		m_pButtonBrightUp->Enable( false );
	}
	else
	{
		m_pSliderBright->Enable( true );
		m_pButtonBrightDown->Enable( true );
		m_pButtonBrightUp->Enable( true );
	}
}

void CDnGameGraphicOptDlg::SetQuality()
{
	m_pComboBoxQuality->RemoveAllItems();

	m_pComboBoxQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3028 ), NULL, CGameOption::Graphic_High );
	m_pComboBoxQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3029 ), NULL, CGameOption::Graphic_Middle );
	m_pComboBoxQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3030 ), NULL, CGameOption::Graphic_Low );

	int nVSVersion = ( GetEtDevice()->GetVSVersion() & 0xffff ) >> 8;
	int nPSVersion = ( GetEtDevice()->GetPSVersion() & 0xffff ) >> 8;
	if( ( nVSVersion < 3 ) || ( nPSVersion < 3 ) )
	{
		CGameOption::GetInstance().m_nGraphicQuality = CGameOption::Graphic_Low;
		m_pComboBoxQuality->Enable( false );
	}

	m_pComboBoxQuality->SetSelectedByIndex( CGameOption::GetInstance().m_nGraphicQuality );
	m_nQualityBack = CGameOption::GetInstance().m_nGraphicQuality;
}

#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
void CDnGameGraphicOptDlg::SetTextureQuality()
{
	if( m_pComboBoxTextureQuality )
	{
		m_pComboBoxTextureQuality->RemoveAllItems();

		m_pComboBoxTextureQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3028 ), NULL, CGameOption::Texture_High );
		m_pComboBoxTextureQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3030 ), NULL, CGameOption::Texture_Low );

		m_pComboBoxTextureQuality->SetSelectedByIndex( CGameOption::GetInstance().m_nTextureQuality );
	}
	m_nTextureQualityBack = CGameOption::GetInstance().m_nTextureQuality;
}
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
void CDnGameGraphicOptDlg::SetEffectQuality()
{
	if( m_pComboBoxEffectQuality )
	{
		m_pComboBoxEffectQuality->RemoveAllItems();

		m_pComboBoxEffectQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3028 ), NULL, CGameOption::Effect_High );
		m_pComboBoxEffectQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3029 ), NULL, CGameOption::Effect_Middle);
		m_pComboBoxEffectQuality->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3030 ), NULL, CGameOption::Effect_Low );

		m_pComboBoxEffectQuality->SetSelectedByIndex( CGameOption::GetInstance().m_nEffectQuality );
	}

	m_nEffectQualityBack = CGameOption::GetInstance().m_nEffectQuality;
}
#endif

void CDnGameGraphicOptDlg::SetWindowMode()
{
	if( !m_pWindowMode )
	{
		return;
	}
	m_pWindowMode->SetChecked( CGameOption::GetInstance().m_bWindow );
}

void CDnGameGraphicOptDlg::Set_ETC_Effect()
{
#ifdef PRE_ADD_37745
	if( m_pHideMainWeapon )
		m_pHideMainWeapon->SetChecked( CGameOption::GetInstance().m_bHideWeaponByBattleMode );
#endif
	
} 

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL

void CDnGameGraphicOptDlg::SetEnableSkillEffect()
{
	if( m_pCheckExclusivePlayerEffect )
		m_pCheckExclusivePlayerEffect->SetChecked( CGameOption::GetInstance().m_bEnablePlayerEffect );
}

#endif 

void CDnGameGraphicOptDlg::SetVSync()
{
	if( !m_pCheckVSync )
	{
		return;
	}
	m_pCheckVSync->SetChecked( CGameOption::GetInstance().m_bVSync );
}

void CDnGameGraphicOptDlg::SetUISize()
{
	m_pComboBoxUISize->SetSelectedByValue( CGameOption::GetInstance().m_nUISize );

	CEtDevice *pEtDevice = GetEtDevice();
	if( !pEtDevice ) return;

	int nWidth = pEtDevice->Width();
	int nHeight = pEtDevice->Height();

	// 1024x768이면 UISize 콤보박스를 비활성 시킨다.
	if( nWidth <= DEFAULT_UI_SCREEN_WIDTH || nHeight <= DEFAULT_UI_SCREEN_HEIGHT )
		m_pComboBoxUISize->Enable( false );
	else
		m_pComboBoxUISize->Enable( true );

	m_nUISizeBack = CGameOption::GetInstance().m_nUISize;
	m_nCurSelectedUISize = m_nUISizeBack;
}

void CDnGameGraphicOptDlg::SetScreenShotFormat()
{
	if( CGameOption::GetInstance().m_nScreenShotFormat != D3DXIFF_JPG &&
		CGameOption::GetInstance().m_nScreenShotFormat != D3DXIFF_PNG &&
		CGameOption::GetInstance().m_nScreenShotFormat != D3DXIFF_TGA )
		CGameOption::GetInstance().m_nScreenShotFormat = D3DXIFF_JPG;
	m_pComboBoxScreenShotFormat->SetSelectedByValue( CGameOption::GetInstance().m_nScreenShotFormat );
	m_nScreenShotBack = CGameOption::GetInstance().m_nScreenShotFormat;
}

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// void CDnGameGraphicOptDlg::SetMemoryOptimize()
// {
// 	m_pComboBoxMemoryOptimize->SetSelectedByValue( CGameOption::GetInstance().m_nMemoryOptimize );
//  	m_nMemoryOptimizeBack = CGameOption::GetInstance().m_nMemoryOptimize;
//}
#endif

void CDnGameGraphicOptDlg::ExportSetting()
{
	// 익스포트 하기 직전에 현재 옵션에 저장된 값을 기억해둔다.
	m_nPreResolutionIndex = m_nResolutionBack;
	m_bPreWindow = CGameOption::GetInstance().m_bWindow;
	m_bPreVSync = CGameOption::GetInstance().m_bVSync;

	bool bOptionChanged = false;
	bool bChangeDisplayMode = false;
	// Note : 퀄리티
	{
		m_pComboBoxQuality->GetSelectedValue( m_nQualityBack );
		if( CGameOption::GetInstance().m_nGraphicQuality != m_nQualityBack )
		{
			CGameOption::GetInstance().m_nGraphicQuality = m_nQualityBack;
			//GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3049 ), MB_OK );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3049 ), false );
			bOptionChanged = true;
		}
	}
#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	// Note : 텍스쳐퀄리티
	{
		if( m_pComboBoxTextureQuality )
			m_pComboBoxTextureQuality->GetSelectedValue( m_nTextureQualityBack );

		if( CGameOption::GetInstance().m_nTextureQuality != m_nTextureQualityBack )
		{
			CGameOption::GetInstance().m_nTextureQuality = m_nTextureQualityBack;
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3049 ), false );
			bOptionChanged = true;
		}
	}
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	// Note : 이펙트퀼리티
	{
		if( m_pComboBoxEffectQuality )
			m_pComboBoxEffectQuality->GetSelectedValue( m_nEffectQualityBack );

		if( CGameOption::GetInstance().m_nEffectQuality != m_nEffectQualityBack )
		{
			CGameOption::GetInstance().m_nEffectQuality = m_nEffectQualityBack;
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3049 ), false );
			bOptionChanged = true;
		}
	}
#endif

	// Note : 밝기
	if( CGameOption::GetInstance().m_nBrightness != m_pSliderBright->GetValue() - 50 )
	{
		CGameOption::GetInstance().m_nBrightness = m_pSliderBright->GetValue() - 50;
		bOptionChanged = true;
	}
	
	// Note : 해상도
	{
		D3DDISPLAYMODE *pDisplayMode = (D3DDISPLAYMODE*)m_pComboBoxResolution->GetSelectedData();
		if( pDisplayMode )
		{
			if( ( CGameOption::GetInstance().m_nWidth != pDisplayMode->Width ) || ( CGameOption::GetInstance().m_nHeight != pDisplayMode->Height ) )
			{
				CGameOption::GetInstance().m_nWidth = pDisplayMode->Width;
				CGameOption::GetInstance().m_nHeight = pDisplayMode->Height;
				bChangeDisplayMode = true;
				bOptionChanged = true;
			}
		}

		m_nResolutionBack = m_pComboBoxResolution->GetSelectedIndex();
	}

	// Note : 창모드
	if( CGameOption::GetInstance().m_bWindow != m_pWindowMode->IsChecked() )
	{
		CGameOption::GetInstance().m_bWindow = m_pWindowMode->IsChecked();
		bChangeDisplayMode = true;
		bOptionChanged = true;
	}

	// Note : VSync
	if( CGameOption::GetInstance().m_bVSync != m_pCheckVSync->IsChecked() )
	{
		CGameOption::GetInstance().m_bVSync = m_pCheckVSync->IsChecked();
		bChangeDisplayMode = true;
		bOptionChanged = true;
	}

	// 해상도 변경 없이 창모드->풀스크린이거나,
	// 풀스크린 상태에서 해상도 변경 하거나,
	m_bConfirmMessageBox = false;
	if( CGameOption::GetInstance().m_bWindow == false && bChangeDisplayMode )
		m_bConfirmMessageBox = true;

	// UISize
	int nUISize = 1;
	if( m_pComboBoxUISize->GetSelectedValue( nUISize ) )
	{
		if( CGameOption::GetInstance().m_nUISize != nUISize )
		{
			CGameOption::GetInstance().m_nUISize = nUISize;
			m_nUISizeBack = nUISize;
			m_nCurSelectedUISize = m_nUISizeBack;
			bOptionChanged = true;
		}
	}

	// Note : 스크린샷 포맷
	{
		m_pComboBoxScreenShotFormat->GetSelectedValue( m_nScreenShotBack );
		if( CGameOption::GetInstance().m_nScreenShotFormat != m_nScreenShotBack )
		{
			CGameOption::GetInstance().m_nScreenShotFormat = m_nScreenShotBack;
			bOptionChanged = true;
		}
	}

#ifdef PRE_ADD_37745
	if( CGameOption::GetInstance().m_bHideWeaponByBattleMode != m_pHideMainWeapon->IsChecked() )
	{
		CGameOption::GetInstance().m_bHideWeaponByBattleMode = m_pHideMainWeapon->IsChecked();
		RefreshActorView(eActorRefreshType::eHideMainWeapon);
		bOptionChanged = true;
	}
#endif

	if (CGameOption::GetInstance().m_bExtendView != m_pExtendView->IsChecked())
	{
		CGameOption::GetInstance().m_bExtendView = m_pExtendView->IsChecked();
		RefreshActorView(eActorRefreshType::eCameraExtend);
		bOptionChanged = true;
	}

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	if( CGameOption::GetInstance().m_bEnablePlayerEffect != m_pCheckExclusivePlayerEffect->IsChecked() )
	{
		CGameOption::GetInstance().m_bEnablePlayerEffect = m_pCheckExclusivePlayerEffect->IsChecked();
		bOptionChanged = true;
	}
#endif 

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// 	int nMemoryOptimize = 1;
// 	if( m_pComboBoxMemoryOptimize->GetSelectedValue( nMemoryOptimize ) )
// 	{
// 		if( CGameOption::GetInstance().m_nMemoryOptimize != nMemoryOptimize )
// 		{
// 			CGameOption::GetInstance().m_nMemoryOptimize = nMemoryOptimize;
// 			m_nMemoryOptimizeBack = nMemoryOptimize;
// 			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3049 ), false );
// 			bOptionChanged = true;
// 		}
// 	}
#endif

	if( bOptionChanged )
	{
		CopyFile( _T( "Config.ini" ), _T( "Config.ini_" ), FALSE );
		CGameOption::GetInstance().Save();
	}
	if( bChangeDisplayMode )
	{
		CGameOption::GetInstance().ChangeDisplayMode();
	}

}

void CDnGameGraphicOptDlg::CancelOption()
{
	m_pComboBoxResolution->SetSelectedByIndex( m_nResolutionBack );
	m_pComboBoxQuality->SetSelectedByIndex( m_nQualityBack );
#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	if( m_pComboBoxTextureQuality )
		m_pComboBoxTextureQuality->SetSelectedByIndex( m_nTextureQualityBack );
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if( m_pComboBoxEffectQuality )
		m_pComboBoxEffectQuality->SetSelectedByIndex( m_nEffectQualityBack );
#endif // PRE_MOD_OPTION_EFFECT_QUALITY
	m_pComboBoxUISize->SetSelectedByValue( m_nUISizeBack );
	CDnOptionDlg::SetDialogSize( m_nUISizeBack );
	m_nCurSelectedUISize = m_nUISizeBack;
	m_pComboBoxScreenShotFormat->SetSelectedByValue( m_nScreenShotBack );
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// 	m_pComboBoxMemoryOptimize->SetSelectedByValue( m_nMemoryOptimizeBack );
#endif
	
	GetEtDevice()->SetGammaRamp( 1.0f, CGameOption::GetInstance().m_nBrightness * 2 );
}

void CDnGameGraphicOptDlg::ImportSetting()
{
	SetResolution();
	SetBright();
	SetQuality();
	SetWindowMode();
	SetUISize();
	Set_ETC_Effect();
#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	SetTextureQuality();
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	SetEffectQuality();
#endif

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	SetEnableSkillEffect();
#endif

	SetVSync();
	SetScreenShotFormat();

	//rlkt_camera
	if (m_pExtendView)
		m_pExtendView->SetChecked(CGameOption::GetInstance().m_bExtendView);


#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// 	SetMemoryOptimize();
#endif
}

bool CDnGameGraphicOptDlg::IsChanged()
{
	bool bOptionChanged = false;

	// Note : 퀄리티
	{
		int nQualityBack;
		m_pComboBoxQuality->GetSelectedValue( nQualityBack );
		if( CGameOption::GetInstance().m_nGraphicQuality != nQualityBack )
			bOptionChanged = true;
	}

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	// Note : 이펙트퀼리티
	{
		int nQualityBack;
		m_pComboBoxEffectQuality->GetSelectedValue( nQualityBack );
		if( CGameOption::GetInstance().m_nEffectQuality != nQualityBack )
			bOptionChanged = true;
	}
#endif

	// Note : 밝기
	if( CGameOption::GetInstance().m_nBrightness != m_pSliderBright->GetValue() - 50 )
		bOptionChanged = true;

	// Note : 해상도
	{
		D3DDISPLAYMODE *pDisplayMode = (D3DDISPLAYMODE*)m_pComboBoxResolution->GetSelectedData();
		if( pDisplayMode ) {
			if( ( CGameOption::GetInstance().m_nWidth != pDisplayMode->Width ) || ( CGameOption::GetInstance().m_nHeight != pDisplayMode->Height ) )
				bOptionChanged = true;
		}
	}

	// Note : 창모드
	if( CGameOption::GetInstance().m_bWindow != m_pWindowMode->IsChecked() )
		bOptionChanged = true;

	// Note : VSync
	if( CGameOption::GetInstance().m_bVSync != m_pCheckVSync->IsChecked() )
		bOptionChanged = true;

#ifdef PRE_ADD_37745
	if( CGameOption::GetInstance().m_bHideWeaponByBattleMode != m_pHideMainWeapon->IsChecked() )
		bOptionChanged = true;                                                   
#endif 

	if (CGameOption::GetInstance().m_bExtendView != m_pExtendView->IsChecked())
		bOptionChanged = true;

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	if( CGameOption::GetInstance().m_bEnablePlayerEffect != m_pCheckExclusivePlayerEffect->IsChecked() )
		bOptionChanged = true;     
#endif 

	// UISize
	int nUISize = 1;
	if( m_pComboBoxUISize->GetSelectedValue( nUISize ) ) {
		if( CGameOption::GetInstance().m_nUISize != nUISize )
			bOptionChanged = true;
	}

	// Note : 스크린샷 포맷
	int nScreenShotBack = 1;
	m_pComboBoxScreenShotFormat->GetSelectedValue( nScreenShotBack );
	if( CGameOption::GetInstance().m_nScreenShotFormat != nScreenShotBack )
		bOptionChanged = true;

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
// 	int nMemoryOptimize = 1;
// 	m_pComboBoxMemoryOptimize->GetSelectedValue( nMemoryOptimize );
// 	if( CGameOption::GetInstance().m_nMemoryOptimize != nMemoryOptimize )
// 		bOptionChanged = true;
#endif

	return bOptionChanged;
}

void CDnGameGraphicOptDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if( pMainMenuDlg )
				pMainMenuDlg->ShowSystemDialog( true );

			return;
		}
		else if( IsCmdControl( "ID_NO" ) )
		{
			m_pComboBoxResolution->SetSelectedByIndex( m_nPreResolutionIndex );
			m_pWindowMode->SetChecked( m_bPreWindow );
			m_pCheckVSync->SetChecked( m_bPreVSync );
			ExportSetting();
			return;
		}
	}
}

void CDnGameGraphicOptDlg::RefreshActorView(eActorRefreshType eRefreshType)
{
	if(! (eRefreshType > eActorRefreshType::eNone && eRefreshType < eActorRefreshType::eMax) )
		return;

	std::map<DWORD, DnActorHandle>::iterator it = CDnActor::s_dwMapActorSearch.begin();
	for( ; it != CDnActor::s_dwMapActorSearch.end(); ++it ) {
		if( !it->second ) continue;
		if( !it->second->IsProcess() ) continue;
		if( !it->second->IsShow() ) continue;

		if( it->second->IsPlayerActor() )
		{
			if(eRefreshType == eHideMainWeapon)
			{
				CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>( it->second.GetPointer() );

				bool bShowMainWeapon = true;
#ifdef PRE_ADD_37745
				if(CGameOption::GetInstance().m_bHideWeaponByBattleMode && !pPlayer->IsBattleMode())
					bShowMainWeapon = false;
#endif	// #ifdef PRE_ADD_37745

				pPlayer->ShowWeapon( 0, bShowMainWeapon );
				pPlayer->ShowCashWeapon( 0, bShowMainWeapon );
			}
			if (eRefreshType == eCameraExtend)
			{
				CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera(CDnCamera::PlayerCamera).GetPointer();
				// 컷신이나 , NPC 카메라에 붙어있는 경우가 있으니 , GetActiveCamera를 사용할때 오류가 발생한다. 주의

				if (pCamera && pCamera->GetCameraType() == CDnCamera::PlayerCamera)
				{
					if (CGameOption::GetInstance().m_bExtendView)
					{
						pCamera->SetZoomDistance(CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX+200.f); //aici static // rlkt_camera
					} else {
						pCamera->SetZoomDistance(CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX); //aici static // rlkt_camera
					}
				}
			}
		}
	}
}