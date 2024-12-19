#include "StdAfx.h"
#include "DnHelpKeyboardDlg.h"
#include "InputWrapper.h"
#include "DnHelpKeyboardTabDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnHelpKeyboardDlg::CDnHelpKeyboardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_bJoypadPush(false)
{
	m_pNormalTab = NULL;
	m_pBattleTab = NULL;
	m_pNormalTabDlg = NULL;
	m_pBattleTabDlg = NULL;
}

CDnHelpKeyboardDlg::~CDnHelpKeyboardDlg()
{
}

void CDnHelpKeyboardDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "HelpKeyboardDlg.ui" ).c_str(), bShow );
}

void CDnHelpKeyboardDlg::InitialUpdate()
{
	m_pNormalTab = GetControl<CEtUIRadioButton>("ID_RBT_NORMAL");
	m_pNormalTabDlg = new CDnHelpKeyboardTabDlg( UI_TYPE_CHILD, this );
	m_pNormalTabDlg->SetUIFileName( "HelpNormalKeyboardDlg.ui" );
	m_pNormalTabDlg->Initialize( false );
	AddTabDialog( m_pNormalTab, m_pNormalTabDlg );

	m_pBattleTab = GetControl<CEtUIRadioButton>("ID_RBT_ATTACK");
	m_pBattleTabDlg = new CDnHelpKeyboardTabDlg( UI_TYPE_CHILD, this );
	m_pBattleTabDlg->SetUIFileName( "HelpAttackKeyboardDlg.ui" );
	m_pBattleTabDlg->Initialize( false );
	AddTabDialog( m_pBattleTab, m_pBattleTabDlg );

	SetCheckedTab( m_pNormalTab->GetTabID() );
}

bool CDnHelpKeyboardDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			return true;
		}

		if( GetKeyState(_ToVK(g_UIWrappingKeyData[IW_UI_HELP]))&0x80 )	// 이렇게 검사해야 다국어로 바꿔도 제대로 작동한다.
		{
			if( m_bJoypadPush ) {
				m_bJoypadPush = false;
			}
			else
				Show(false);
			return true;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}