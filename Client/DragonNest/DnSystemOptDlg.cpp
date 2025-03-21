#include "StdAfx.h"
#include "DnSystemOptDlg.h"
#include "DnGameGraphicOptDlg.h"
#include "DnGameSoundOptDlg.h"
#include "DnGameControlOptDlg.h"
#include "DnGameVoiceChatOptDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnOptionDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnSystemOptDlg::CDnSystemOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnOptionTabDlg( dialogType, pParentDialog, nID, pCallback, true )
	, m_pGameGraphicOptDlg(NULL)
	, m_pTabButtonGraphic(NULL)
	, m_pTabButtonSound(NULL)
	, m_pGameSoundOptDlg(NULL)
	, m_pGameControlOptDlg(NULL)
	, m_pGameVoiceChatOptDlg(NULL)
	, m_pTabButtonControl(NULL)
	, m_pTabButtonVoiceChat(NULL)
{
}

CDnSystemOptDlg::~CDnSystemOptDlg(void)
{
}

void CDnSystemOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "SystemOptDlg.ui" ).c_str(), bShow );
}

void CDnSystemOptDlg::InitialUpdate()
{
	m_pTabButtonGraphic = GetControl<CEtUIRadioButton>("ID_TAB_GRAPHIC");
	m_pGameGraphicOptDlg = new CDnGameGraphicOptDlg( UI_TYPE_CHILD, this, SYSTEM_GRAPHIC_DIALOG, this );
	m_pGameGraphicOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonGraphic, m_pGameGraphicOptDlg );

	m_pTabButtonSound = GetControl<CEtUIRadioButton>("ID_TAB_SOUND");
	m_pGameSoundOptDlg = new CDnGameSoundOptDlg( UI_TYPE_CHILD, this, SYSTEM_SOUND_DIALOG, this  );
	m_pGameSoundOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonSound, m_pGameSoundOptDlg );

	m_pTabButtonControl = GetControl<CEtUIRadioButton>("ID_TAB_CONTROL");
	m_pGameControlOptDlg = new CDnGameControlOptDlg( UI_TYPE_CHILD, this, SYSTEM_CONTROL_DIALOG, this  );
	m_pGameControlOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonControl, m_pGameControlOptDlg );

#ifdef _USE_VOICECHAT
	m_pTabButtonVoiceChat = GetControl<CEtUIRadioButton>("ID_TAB_VOICECHAT");
	m_pGameVoiceChatOptDlg = new CDnGameVoiceChatOptDlg( UI_TYPE_CHILD, this, SYSTEM_VOICECHAT_DIALOG, this );
	m_pGameVoiceChatOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonVoiceChat, m_pGameVoiceChatOptDlg );
#else
	m_pTabButtonVoiceChat = GetControl<CEtUIRadioButton>("ID_TAB_VOICECHAT");
	m_pTabButtonVoiceChat->Show( false );
#endif

#ifdef PRE_REMOVE_EU_CBTUI_1206
	if(m_pTabButtonVoiceChat)
		m_pTabButtonVoiceChat->Show(false);
#endif // PRE_REMOVE_EU_CBTUI_1206

//	SetCheckedTab( m_pTabButtonGraphic->GetTabID() );
}

//void CDnSystemOptDlg::Show( bool bShow ) 
//{
//	if( m_bShow == bShow )
//		return;
//
//	//if( bShow )
//	//{
//	//	SetCheckedTab( m_pTabButtonGraphic->GetTabID() );
//	//	//m_nPrevTabID = m_pTabButtonGraphic->GetTabID();
//	//}
//	//else
//	//{
//	//	m_pGameGraphicOptDlg->Show( false );
//	//	m_pGameSoundOptDlg->Show( false );
//	//	m_pGameControlOptDlg->Show( false );
//	//}
//
//	BaseClass::Show( bShow );
////	CDnMouseCursor::GetInstance().ShowCursor( bShow );
//}

void CDnSystemOptDlg::ImportSetting()
{
	m_pGameGraphicOptDlg->ImportSetting();
	m_pGameSoundOptDlg->ImportSetting();
#ifdef _USE_VOICECHAT
	m_pGameVoiceChatOptDlg->ImportSetting();
#endif
}

//
//void CDnSystemOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
//{
//	//SetCmdControlName( pControl->GetControlName() );
//
//	//if( nCommand == EVENT_RADIOBUTTON_CHANGED && bTriggeredByUser )
//	//{
//	//	// 실제로 현재 활성화되어있는 탭버튼을 그냥 클릭해도 라디오버튼체인지가 오게되어있다.
//	//	// 그래서 다른지 확인한다.
//	//	if( m_nPrevTabID != GetCurrentTabID() )
//	//	{
//	//		if( m_bShowChangeMsg )
//	//		{
//	//			// 현재 다이얼로그 구해서,
//	//			CEtUIDialog *pDialog = m_groupTabDialog.GetShowDialog();
//	//			if( pDialog ) {
//
//	//				CDnOptionDlg*pOptDlg = dynamic_cast<CDnOptionDlg*>(pDialog);
//
//	//				// 변했다면,
//	//				if( pOptDlg && pOptDlg->IsChanged() ) {
//
//	//					// 1회만 보여주는거니,
//	//					m_bShowChangeMsg = false;
//
//	//					// 탭버튼 선택된걸 원래대로 돌리고,
//	//					SetCheckedTab( m_nPrevTabID );
//
//	//					// 메세지박스 3170. 1회 보여주기.
//	//					GetInterface().MessageBox( 3170 );
//
//	//					return;
//	//				}
//	//			}
//	//		}
//	//		else
//	//		{
//	//			m_bShowChangeMsg = true;
//	//		}
//	//	}
//	//}
//
//	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
//
//	//// 라디오버튼 체인지마다 현재 값을 기억
//	//if( nCommand == EVENT_RADIOBUTTON_CHANGED )
//	//	m_nPrevTabID = GetCurrentTabID();
//}

void CDnSystemOptDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( !pMainMenuDlg ) return;

		if( IsCmdControl( "ID_BUTTON_APPLY" ) )
		{
			//pMainMenuDlg->ShowSystemDialog( true );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060) );
			return;
		}

		if( IsCmdControl( "ID_BUTTON_CANCEL" ) )
		{
			pMainMenuDlg->ShowSystemDialog( true );
			return;
		}
	}
}