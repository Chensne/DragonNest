#include "StdAfx.h"
#include "DnGameControlComboDlg.h"
#include "DnGameControlOptDlg.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameControlComboDlg::CDnGameControlComboDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pComboBox(NULL)
, m_pControlOptDlg(NULL)
, m_bInitialized( false )
{
}

CDnGameControlComboDlg::~CDnGameControlComboDlg()
{
	m_pParentDialog = NULL;
}

void CDnGameControlComboDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameControlComboDlg.ui" ).c_str(), bShow );
}

void CDnGameControlComboDlg::InitialUpdate()
{
	m_pComboBox = GetControl<CEtUIComboBox>( "ID_COMBOBOX_SELECT" );

	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2010014), NULL, KEYBOARD_MOUSE );
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 102002), NULL, XBOX360_PAD );
#if defined(_JP)
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3116), NULL,	HANGAME_PAD );
#endif
#if defined(_KR)
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 117), NULL,	GAMMAC_PAD );
#endif
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 102004), NULL, CUSTOM1_PAD );

	m_bInitialized = true;
	m_pComboBox->SetSelectedByValue( CGameOption::GetInstance().GetInputDevice() );
}

void CDnGameControlComboDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl( "ID_COMBOBOX_SELECT" ) )
		{
			int nStateIndex = m_pComboBox->GetSelectedIndex();

			if( m_bInitialized )
			{
				CGameOption::GetInstance().SetInputDevice( nStateIndex );
				CGameOption::GetInstance().Save();
			}

			if( m_pControlOptDlg )
				m_pControlOptDlg->ProcessCombo(nStateIndex);

			// 조이스틱 장치가 생성 되지 않았다면 조이스틱 정보를 다시한번 초기화한다.
			// 게임 시작시 초기화 하는데 조이스틱이 중간에 연결될 경우를 대비해서 이다.
			/*if( KEYBOARD_MOUSE != nStateIndex )
			{
				if( !CDnActor::s_hLocalActor ) return;
				CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

				if( NULL == localActor->IsJoypad() )
				{
					CInputDevice::GetInstance().CreateDevice( CInputDevice::JOYPAD );
				}
			}*/
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
