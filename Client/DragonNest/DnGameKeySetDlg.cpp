#include "stdafx.h"
#include "DnGameKeySetDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnGameKeySetListDlg.h"
#include "GameOption.h"
#include "DnLocalPlayerActor.h"
#ifdef _US
#include "DnPlayerCamera.h"
#endif
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif

#define MESSAGEBOX_DEFAULT_VALUE 100

CDnGameKeySetDlg::CDnGameKeySetDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_nControlTabID( CDnGameControlTask::eControlType_Keyboard )
, m_nControlTabIndex( 0 )
, m_bApplyQuestion( false )
, m_pButton_Default( NULL )
, m_pButton_Apply( NULL )
, m_pButton_Cancel( NULL )
, m_pStatic_Notice( NULL )
, m_pSelectListItem( NULL )
, m_pCheckSmartMove( NULL )
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
, m_pCheckSmartMoveEx( NULL )
#endif
, m_pButtonDown( NULL )
, m_pButtonUp( NULL )
, m_pSliderMouse( NULL )
, m_pStaticMouse( NULL )
, m_pButtonDownCamera( NULL )
, m_pButtonUpCamera( NULL )
, m_pSliderCamera( NULL )
, m_pStaticCamera( NULL )
, m_pButtonDownCursor( NULL )
, m_pButtonUpCursor( NULL )
, m_pSliderCursor( NULL )
, m_pStaticCursor( NULL )
#if defined(_US)
, m_pStaticMouseInvert( NULL )
, m_pStaticMouseInvertBar( NULL )
, m_pCheckMouseInvert( NULL )
#endif
{
	memset( m_pButton_ControlType, NULL, sizeof(CEtUIRadioButton *)*eControlType_Count );
	memset( m_pList_CategorySection, NULL, sizeof(CEtUIRadioButton *)*eCategorySection_Count );
}

CDnGameKeySetDlg::~CDnGameKeySetDlg()
{

}

void CDnGameKeySetDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameKeySetDlg.ui" ).c_str(), bShow );
}

void CDnGameKeySetDlg::InitialUpdate()
{
	char szControlName[256] = {0,};
	for( int itr = 0; itr < eControlType_Count; ++itr )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_RBT%d", itr );
		m_pButton_ControlType[itr] = GetControl<CEtUIRadioButton>( szControlName );
		m_pButton_ControlType[itr]->Show( false );
	}

	for( int itr = 0; itr < eCategorySection_Count; ++itr )
	{
		sprintf_s( szControlName, _countof(szControlName), "ID_LISTBOXEX_LIST%d", itr );
		m_pList_CategorySection[itr] = GetControl<CEtUIListBoxEx>( szControlName );
	}

	m_pButton_Default = GetControl<CEtUIButton>( "ID_BT_RESET" );
	m_pButton_Apply = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pButton_Cancel = GetControl<CEtUIButton>( "ID_BT_CANCEL" );

	m_pStatic_Notice = GetControl<CEtUIStatic>( "ID_TEXT_NOTICE" );
	m_pStatic_Notice->Show( false );

	InitializeControlTypeTab();

	_InitialUpdate_MouseControl();
}

void CDnGameKeySetDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( true == bShow )
	{
		m_pButton_ControlType[m_nControlTabIndex]->SetChecked( true );
		m_nControlTabID = m_pButton_ControlType[m_nControlTabIndex]->GetButtonID();
		m_bApplyQuestion = true;
		ChangeControlTypeTab( m_nControlTabID );

		_Import_MouseControl();

		CDnLocalPlayerActor::LockInput( true );
	}
	else
	{
		if( NULL != m_pSelectListItem )
			m_pSelectListItem->DisableKeyListData( true );
		else
			SetNotice( eNotice_Disable );

		CDnLocalPlayerActor::LockInput( false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnGameKeySetDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg  )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_OK" ) )
		{
			GetGameControlTask().ChangeValue( m_nControlTabID );
			_Export_MouseControl();

			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060) );	// UISTRING: 적용 되었습니다.
			return;
		}
		else if( IsCmdControl( "ID_BT_CANCEL" ) )
		{
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if( !pMainMenuDlg )
				return;

			CDnLocalPlayerActor::LockInput( false );

			pMainMenuDlg->ShowSystemDialog( true );
			return;
		}
		else if( IsCmdControl( "ID_BT_RESET" ) )
		{
			GetInterface().MessageBox( 119, MB_YESNO, MESSAGEBOX_DEFAULT_VALUE, this );	// UISTRING : 컨트롤 세팅을 초기화 시키시겠습니까?
			return;
		}
		else if( IsCmdControl( "ID_BT_SLIDER_UP0" ) )
		{
			int nValue = m_pSliderMouse->GetValue();
			if( (++nValue) > 100 ) nValue = 100;

			m_pSliderMouse->SetValue( nValue );
			m_pStaticMouse->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BT_SLIDER_DOWN0" ) )
		{
			int nValue = m_pSliderMouse->GetValue();
			if( (--nValue) < 0 ) nValue = 0;

			m_pSliderMouse->SetValue( nValue );
			m_pStaticMouse->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BT_SLIDER_UP1" ) )
		{
			int nValue = m_pSliderCursor->GetValue();
			if( (++nValue) > 100 ) nValue = 100;

			m_pSliderCursor->SetValue( nValue );
			m_pStaticCursor->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BT_SLIDER_DOWN1" ) )
		{
			int nValue = m_pSliderCursor->GetValue();
			if( (--nValue) < 0 ) nValue = 0;

			m_pSliderCursor->SetValue( nValue );
			m_pStaticCursor->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BT_SLIDER_UP2" ) )
		{
			int nValue = m_pSliderCamera->GetValue();
			if( (++nValue) > 100 ) nValue = 100;

			m_pSliderCamera->SetValue( nValue );
			m_pStaticCamera->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_BT_SLIDER_DOWN2" ) )
		{
			int nValue = m_pSliderCamera->GetValue();
			if( (--nValue) < 0 ) nValue = 0;

			m_pSliderCamera->SetValue( nValue );
			m_pStaticCamera->SetIntToText( nValue );
			return;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_CONTROL_KEY);
		}
#endif
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED && bTriggeredByUser )
	{
		if( strstr( pControl->GetControlName(), "ID_RBT" ) )
		{
			CEtUIRadioButton * pButton = dynamic_cast<CEtUIRadioButton *>( pControl );

			if( NULL == pButton )
				return;

			if( false == ChangeControlTypeTab( pButton->GetButtonID() ) )
			{
				for( int itr = 0; itr < eControlType_Count; ++itr )
				{
					if( m_pButton_ControlType[itr]->GetButtonID() == m_nControlTabID )
					{
						m_pButton_ControlType[itr]->SetChecked( true );
						return;
					}
				}
			}

			for( int itr = 0; itr < eControlType_Count; ++itr )
			{
				if( m_pButton_ControlType[itr]->GetButtonID() == pButton->GetButtonID() )
				{
					m_nControlTabIndex = itr;
					break;
				}
			}
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		CEtUIListBoxEx * pListBox = dynamic_cast<CEtUIListBoxEx *>( pControl );

		if( NULL == pListBox )
			return;

		SelectListCommand( pListBox );

		pListBox->SecDragMode( false );
		
	}
	else if( nCommand == EVENT_SLIDER_VALUE_CHANGED )
	{
		if( IsCmdControl( "ID_SLIDER0" ) )
		{
			int nValue = m_pSliderMouse->GetValue();
			m_pStaticMouse->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_SLIDER1" ) )
		{
			int nValue = m_pSliderCursor->GetValue();
			m_pStaticCursor->SetIntToText( nValue );
			return;
		}
		else if( IsCmdControl( "ID_SLIDER2" ) )
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


	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameKeySetDlg::Process( float fElapsedTime )
{
	if( false == m_bShow )
		return;

	ProcessNotice( fElapsedTime );

	CEtUIDialog::Process( fElapsedTime );
}

void CDnGameKeySetDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg  )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_DEFAULT_VALUE:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_YES") )
				{
					GetGameControlTask().DefaultValue( m_nControlTabID );
					_Reset_MouseControl();
					RefreshListData();
					ResetSelectListItem();
				}
			}
		}
		break;
	}
}

void CDnGameKeySetDlg::InitializeControlTypeTab()
{
	std::map<int, int> mControlTypeData;

	GetGameControlTask().GetControlTypeData( mControlTypeData );

	int nCount = 0;
	std::map<int, int>::iterator Itor = mControlTypeData.begin();
	for( ; Itor != mControlTypeData.end(); ++Itor, ++nCount )
	{
		m_pButton_ControlType[nCount]->SetButtonID( Itor->first );
		m_pButton_ControlType[nCount]->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Itor->second ) );
		m_pButton_ControlType[nCount]->Show( true );
	}
}

bool CDnGameKeySetDlg::ChangeControlTypeTab( int nControlTabID )
{
	if( false == m_bApplyQuestion && true == GetGameControlTask().IsChangeValue( m_nControlTabID ) )
	{
		m_bApplyQuestion = true;
		GetInterface().MessageBox( 3170 );	// UISTRING : 적용 버튼을 클릭하지 않으면, 이전 상태로 돌아갑니다.
		return false;
	}

	ResetSelectListItem();
	m_nControlTabID = nControlTabID;
	m_bApplyQuestion = false;

	GetGameControlTask().SetWrappingDataByControlType( nControlTabID );

	CGameOption::GetInstance().SetInputDevice( nControlTabID );
	CGameOption::GetInstance().Save();
	CGameOption::GetInstance().ApplyInputDevice();

	std::vector<CDnGameControlTask::SKeyData *> vKeyData[eCategorySection_Count];

	for( int itr = 0; itr < eCategorySection_Count; ++itr )
	{
		GetGameControlTask().GetDataByType( nControlTabID, itr, vKeyData[itr] );

		m_pList_CategorySection[itr]->RemoveAllItems();
		SetCategoryTypeList( itr, vKeyData[itr] );
	}

	return true;
}

void CDnGameKeySetDlg::SetCategoryTypeList( const int eCategoryIndex, std::vector<CDnGameControlTask::SKeyData *> & vKeyData )
{
	for( UINT itr = 0; itr < vKeyData.size(); ++itr )
	{
		CDnGameControlTask::SKeyData * pData = vKeyData[itr];
		if( false == pData->bShow )
			continue;

		CDnGameKeySetListDlg * pItemDlg = m_pList_CategorySection[eCategoryIndex]->AddItem<CDnGameKeySetListDlg>();
		pItemDlg->SetData( vKeyData[itr], this );
	}
}

void CDnGameKeySetDlg::SelectListCommand( CEtUIListBoxEx * pListBox )
{
	if( NULL == pListBox )
		return;

	SListBoxItem * pListItem = pListBox->GetSelectedItem();
	if( NULL == pListItem )
		return;

	CDnGameKeySetListDlg * pSelectListItem = (CDnGameKeySetListDlg *)pListItem->pData;
	if( NULL == pSelectListItem )
	{
		ResetSelectListItem();
	}
	else
	{
		if( NULL != m_pSelectListItem && m_pSelectListItem == pSelectListItem )
		{
			ResetSelectListItem();
		}
		else
		{
			if( NULL != m_pSelectListItem )
				m_pSelectListItem->DisableKeyListData( true );

			m_pSelectListItem = NULL;

			if( true == pSelectListItem->EnableKeyListData() )
				m_pSelectListItem = pSelectListItem;
		}
	}

	for( int itr = 0; itr < eCategorySection_Count; ++itr )
		m_pList_CategorySection[itr]->DeselectItem();
}

void CDnGameKeySetDlg::SetNotice( const eNotice_Type eType )
{
	switch( eType )
	{
	case eNotice_Enable:
		{
			m_sNoticeData.bShow = true;
			m_sNoticeData.fShowTime = -1.f;
			m_sNoticeData.dwTextColor = textcolor::YELLOW;
			m_sNoticeData.wszText = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3155 );	// UISTRING : 변경할 키를 입력하세요 (ESC: 취소)
		}
		break;

	case eNotice_Disable:
		{
			m_sNoticeData.bShow = false;
			m_sNoticeData.fShowTime = -1.f;
		}
		break;

	case eNotice_Success:
		{
			m_sNoticeData.bShow = true;
			m_sNoticeData.fShowTime = 3.0f;
			m_sNoticeData.dwTextColor = textcolor::RED;
			m_sNoticeData.wszText = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3158 );	// UISTRING : 입력한 키로 설정되었습니다.
		}
		break;

	case eNotice_Fail:
		{
			m_sNoticeData.bShow = true;
			m_sNoticeData.fShowTime = 3.0f;
			m_sNoticeData.dwTextColor = textcolor::RED;
			m_sNoticeData.wszText = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3156 );	// UISTRING : 입력한 키로는 설정할 수 없습니다.
		}
		break;
	}

	m_pStatic_Notice->Show( m_sNoticeData.bShow );
	m_pStatic_Notice->SetText( m_sNoticeData.wszText );
	m_pStatic_Notice->SetTextColor( m_sNoticeData.dwTextColor );
}

void CDnGameKeySetDlg::ProcessNotice( float fElapsedTime )
{
	if( true == m_sNoticeData.bShow && 0.f < m_sNoticeData.fShowTime )
	{
		m_sNoticeData.fShowTime -= fElapsedTime;
		if( 0.f > m_sNoticeData.fShowTime )
			SetNotice( eNotice_Disable );
	}
}

void CDnGameKeySetDlg::RefreshListData()
{
	for( int itr = 0; itr < eCategorySection_Count; ++itr )
	{
		for( int list_itr = 0; list_itr < m_pList_CategorySection[itr]->GetSize(); ++list_itr )
		{
			CDnGameKeySetListDlg * pItem = m_pList_CategorySection[itr]->GetItem<CDnGameKeySetListDlg>( list_itr );

			if( NULL == pItem )
				continue;

			pItem->SetKeyString();
		}
	}
}

void CDnGameKeySetDlg::ResetSelectListItem( const bool bNoticeDisable /*= true*/ )
{
	if( NULL == m_pSelectListItem )
		return;
	
	if( bNoticeDisable )
		m_pSelectListItem->DisableKeyListData( true );
	m_pSelectListItem = NULL;
}

bool CDnGameKeySetDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( NULL != m_pSelectListItem )
	{
		if( uMsg == WM_KEYDOWN )
		{
			if( wParam == VK_ESCAPE )
			{
				ResetSelectListItem();
				return true;
			}

			m_pSelectListItem->MsgProc( hWnd, uMsg, wParam, lParam );
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnGameKeySetDlg::_InitialUpdate_MouseControl()
{
	m_pCheckSmartMove = GetControl<CEtUICheckBox>("ID_CHECKBOX0");
#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	m_pCheckSmartMoveEx = GetControl<CEtUICheckBox>("ID_CHECKBOX1");
#endif

	m_pButtonDown = GetControl<CEtUIButton>("ID_BT_SLIDER_DOWN0");
	m_pButtonUp = GetControl<CEtUIButton>("ID_BT_SLIDER_UP0");
	m_pSliderMouse = GetControl<CEtUISlider>("ID_SLIDER0");
	m_pStaticMouse = GetControl<CEtUIStatic>("ID_TEXT_COUNT0");

	m_pButtonDownCursor = GetControl<CEtUIButton>("ID_BT_SLIDER_DOWN1");
	m_pButtonUpCursor = GetControl<CEtUIButton>("ID_BT_SLIDER_UP1");
	m_pSliderCursor = GetControl<CEtUISlider>("ID_SLIDER1");
	m_pStaticCursor = GetControl<CEtUIStatic>("ID_TEXT_COUNT1");

	m_pButtonDownCamera = GetControl<CEtUIButton>("ID_BT_SLIDER_DOWN2");
	m_pButtonUpCamera = GetControl<CEtUIButton>("ID_BT_SLIDER_UP2");
	m_pSliderCamera = GetControl<CEtUISlider>("ID_SLIDER2");
	m_pStaticCamera = GetControl<CEtUIStatic>("ID_TEXT_COUNT2");

#if defined(_US)
	m_pStaticMouseInvert = GetControl<CEtUIStatic>("ID_TEXT5");
	m_pStaticMouseInvertBar = GetControl<CEtUIStatic>("ID_STATIC_BOX6");
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

void CDnGameKeySetDlg::_Export_MouseControl()
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

void CDnGameKeySetDlg::_Import_MouseControl()
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

void CDnGameKeySetDlg::_Reset_MouseControl()
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