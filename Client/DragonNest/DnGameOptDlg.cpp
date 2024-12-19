#include "StdAfx.h"
#include "DnGameOptDlg.h"
#include "DnGameViewOptDlg.h"
#include "DnGameQuickMsgOptDlg.h"
#include "DnGameCommOptDlg.h"
#include "DnGameMovieOptDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnOptionDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameOptDlg::CDnGameOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnOptionTabDlg( dialogType, pParentDialog, nID, pCallback, true )
	, m_pTabButtonView(NULL)
	, m_pTabButtonMacro(NULL)
	, m_pTabButtonComm(NULL)
	, m_pGameViewOptDlg(NULL)
	, m_pGameQuickMsgOptDlg(NULL)
	, m_pGameCommOptDlg(NULL)
	, m_pTabButtonMovie(NULL)
	, m_pGameMovieOptDlg(NULL)
	//, m_bShowChangeMsg(true)
	//, m_nPrevTabID(0)
{
}

CDnGameOptDlg::~CDnGameOptDlg(void)
{
}

void CDnGameOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameOptDlg.ui" ).c_str(), bShow );
}

void CDnGameOptDlg::InitialUpdate()
{
	m_pTabButtonView = GetControl<CEtUIRadioButton>("ID_TAB_VIEW");
	m_pGameViewOptDlg = new CDnGameViewOptDlg( UI_TYPE_CHILD, this, GAMEOPTION_VIEW_DIALOG, this );
	m_pGameViewOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonView, m_pGameViewOptDlg );	

	m_pTabButtonComm = GetControl<CEtUIRadioButton>("ID_TAB_COMM");
	m_pGameCommOptDlg = new CDnGameCommOptDlg( UI_TYPE_CHILD, this, GAMEOPTION_COMM_DIALOG, this );
	m_pGameCommOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonComm, m_pGameCommOptDlg );

	m_pTabButtonMacro = GetControl<CEtUIRadioButton>("ID_TAB_MACRO");
	m_pGameQuickMsgOptDlg = new CDnGameQuickMsgOptDlg( UI_TYPE_CHILD, this, GAMEOPTION_MACRO_DIALOG, this );
	m_pGameQuickMsgOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonMacro, m_pGameQuickMsgOptDlg );

	m_pTabButtonMovie = GetControl<CEtUIRadioButton>("ID_TAB_MOVIE");
	m_pGameMovieOptDlg = new CDnGameMovieOptDlg( UI_TYPE_CHILD, this, GAMEOPTION_MOVIE_DIALOG, this );
	m_pGameMovieOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonMovie, m_pGameMovieOptDlg );
}

//void CDnGameOptDlg::Show( bool bShow ) 
//{
//	if( m_bShow == bShow )
//		return;
//
//	/*if( bShow )
//	{
//		SetCheckedTab( m_pTabButtonView->GetTabID() );
//		m_nPrevTabID = m_pTabButtonView->GetTabID();
//	}
//	else {
//		m_pGameViewOptDlg->Show( false );
//		m_pGameMacroOptDlg->Show( false );
//		m_pGameCommOptDlg->Show( false );
//		m_pGameMovieOptDlg->Show( false );
//	}*/
//
//	BaseClass::Show( bShow );
////	CDnMouseCursor::GetInstance().ShowCursor( bShow );
//}

//void CDnGameOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
//{
//	SetCmdControlName( pControl->GetControlName() );
//
//	//if( nCommand == EVENT_RADIOBUTTON_CHANGED && bTriggeredByUser )
//	//{
//	//	// ������ ���� Ȱ��ȭ�Ǿ��ִ� �ǹ�ư�� �׳� Ŭ���ص� ������ưü������ ���ԵǾ��ִ�.
//	//	// �׷��� �ٸ��� Ȯ���Ѵ�.
//	//	if( m_nPrevTabID != GetCurrentTabID() )
//	//	{
//	//		if( m_bShowChangeMsg )
//	//		{
//	//			// ���� ���̾�α� ���ؼ�,
//	//			CEtUIDialog *pDialog = m_groupTabDialog.GetShowDialog();
//	//			if( pDialog ) {
//	//				CDnOptionDlg *pOptDlg = dynamic_cast<CDnOptionDlg*>(pDialog);
//	//				// ���ߴٸ�,
//	//				if( pOptDlg->IsChanged() ) {
//
//	//					// 1ȸ�� �����ִ°Ŵ�,
//	//					m_bShowChangeMsg = false;
//
//	//					// �ǹ�ư ���õȰ� ������� ������,
//	//					SetCheckedTab( m_nPrevTabID );
//
//	//					// �޼����ڽ� 3170. 1ȸ �����ֱ�.
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
//	// ������ư ü�������� ���� ���� ���
//	//if( nCommand == EVENT_RADIOBUTTON_CHANGED )
//	//	m_nPrevTabID = GetCurrentTabID();
//}

void CDnGameOptDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
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

