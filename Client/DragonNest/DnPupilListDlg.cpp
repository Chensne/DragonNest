#include "Stdafx.h"
#include "DnPupilListDlg.h"
#include "DnMasterIntroduceDlg.h"
#include "DnPartyTask.h"
#include "DnInterface.h"
#include "DnPupilInListDlg.h"
#include "DnMasterTask.h"
#include "SyncTimer.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif

CDnPupilListDlg::CDnPupilListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pMasterIntroduceDlg( NULL )
, m_pListBoxEx( NULL )
, m_pTitleButton( NULL )
, m_pChatButton( NULL )
, m_pJoinButton( NULL )
, m_pQuitButton( NULL )
, m_bTitle( false )
{
}

CDnPupilListDlg::~CDnPupilListDlg()
{
	SAFE_DELETE( m_pMasterIntroduceDlg );
}

void CDnPupilListDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PupilListDlg.ui" ).c_str(), bShow );
}

void CDnPupilListDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");

	m_pTitleButton = GetControl<CEtUIButton>( "ID_BT_TITLE" );
	m_pChatButton = GetControl<CEtUIButton>( "ID_BT_CHAT" );
	m_pJoinButton = GetControl<CEtUIButton>( "ID_BT_JOIN" );
	m_pQuitButton = GetControl<CEtUIButton>( "ID_BT_QUIT" );
	m_pTitleButton->Enable( false );
	m_pChatButton->Enable( false );
	m_pJoinButton->Enable( false );
	m_pQuitButton->Enable( false );

	m_pMasterIntroduceDlg = new CDnMasterIntroduceDlg( UI_TYPE_CHILD_MODAL, this );
	m_pMasterIntroduceDlg->Initialize( false );
}

void CDnPupilListDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		GetMasterTask().RequestMasterCharacterInfo();
	}

	CEtUIDialog::Show( bShow );
}

void CDnPupilListDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnPupilListDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_TITLE") )
		{
			if( m_bTitle )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7235 ), MB_YESNO, INTRODUCTION_DIALOG, this );
			else
				ShowChildDialog( m_pMasterIntroduceDlg, true );
		}
		else if( IsCmdControl("ID_BT_CHAT") )
		{
			int nIndex = m_pListBoxEx->GetSelectedIndex();

			if( -1 == nIndex )
				return;

			const SCPupilList * pPupilList = GetMasterTask().GetPupilList();
			GetInterface().OpenPrivateChatDialog( pPupilList->PupilInfoList[nIndex].wszCharName );
		}
		else if( IsCmdControl("ID_BT_JOIN") )
		{
			int nIndex = m_pListBoxEx->GetSelectedIndex();

			if( -1 == nIndex )
				return;

			const SCPupilList * pPupilList = GetMasterTask().GetPupilList();
			GetPartyTask().ReqInviteParty( pPupilList->PupilInfoList[nIndex].wszCharName );
		}
		else if( IsCmdControl("ID_BT_QUIT") )	//제자 추방
		{
			int nIndex = m_pListBoxEx->GetSelectedIndex();

			if( -1 == nIndex )
				return;

			wchar_t szString[256];
			const SCPupilList * pPupilList = GetMasterTask().GetPupilList();
			swprintf_s( szString, _countof(szString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7236 ), pPupilList->PupilInfoList[nIndex].wszCharName );
			GetInterface().MessageBox( szString, MB_YESNO, PUPIL_DELETE_DIALOG, this );
		}

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_PUPILLIST);
		}
#endif
	}
	if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		if( IsCmdControl("ID_LISTBOXEX_LIST") )
		{
			const int nSelection = m_pListBoxEx->GetSelectedIndex();

			if( -1 != nSelection )
			{
#ifdef PRE_FIX_62281
				const SCPupilList * pPupilList = GetMasterTask().GetPupilList();

				if( NULL == pPupilList )
					return;

				if( 0 > nSelection || MasterSystem::Max::PupilCount <= nSelection )
					return;
#endif

				CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
				if( pGameTask )
				{
					m_pChatButton->Enable( true );

#ifdef PRE_FIX_62281
					m_pJoinButton->Enable( pPupilList->PupilInfoList[nSelection].Location.cServerLocation );
#else
					m_pJoinButton->Enable( false );
#endif
					m_pQuitButton->Enable( false );
					return;
				}

#ifdef PRE_FIX_62281
#else
				const SCPupilList * pPupilList = GetMasterTask().GetPupilList();

				if( NULL == pPupilList )
					return;

				if( 0 > nSelection || MasterSystem::Max::PupilCount <= nSelection )
					return;
#endif

				if( pPupilList->PupilInfoList[nSelection].Location.cServerLocation )
				{
					m_pChatButton->Enable( true );
					m_pJoinButton->Enable( true );
				}
				else
				{
					m_pChatButton->Enable( false );
					m_pJoinButton->Enable( false );
				}

				m_pQuitButton->Enable( true );
			}
			else
			{
				m_pChatButton->Enable( false );
				m_pJoinButton->Enable( false );
				m_pQuitButton->Enable( false );
			}
		}
	}
}

void CDnPupilListDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			if( PUPIL_DELETE_DIALOG == nID )
			{
				const int nIndex = m_pListBoxEx->GetSelectedIndex();

				if( 0 > nIndex || MasterSystem::Max::PupilCount <= nIndex )
					return;

				const SCPupilList * pPupilList = GetMasterTask().GetPupilList();

				GetMasterTask().RequestLeave( pPupilList->PupilInfoList[nIndex].biCharacterDBID, true );

			}
			else if( INTRODUCTION_DIALOG == nID )
				GetMasterTask().RequestMasterIntroduction( false, GetMasterTask().GetMasterCharacterInfo().wszSelfIntroduction );
		}
	}
}

void CDnPupilListDlg::RefreshIntroduction()
{
	m_bTitle = GetMasterTask().GetIntroductionRegister();

	m_pMasterIntroduceDlg->OnRecvIntroduction( m_bTitle );

	if( m_bTitle )
	{
		m_pTitleButton->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7232 ) );
		CONTROL( Static, ID_TEXT_TITLE )->SetText( GetMasterTask().GetMasterCharacterInfo().wszSelfIntroduction );
	}
	else
	{
		m_pTitleButton->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7228 ) );
		CONTROL( Static, ID_TEXT_TITLE )->SetText( L"" );
	}

	CONTROL( Static, ID_TEXT_PUPILCOUNT )->SetIntToText( GetMasterTask().GetMasterCharacterInfo().iGraduateCount );
	CONTROL( Static, ID_TEXT_RESPECT )->SetIntToText( GetMasterTask().GetMasterCharacterInfo().iRespectPoint );
}

void CDnPupilListDlg::RefreshPupilList()
{
	m_pListBoxEx->RemoveAllItems();
	m_pChatButton->Enable( false );
	m_pJoinButton->Enable( false );
	m_pQuitButton->Enable( false );

	const SCPupilList * pPupilList = GetMasterTask().GetPupilList();

	if( NULL == pPupilList )
		return;

	CDnPupilInListDlg * pItem = NULL; 

	for( int itr = 0; itr < pPupilList->cCount; ++itr )
	{
		pItem = m_pListBoxEx->InsertItem<CDnPupilInListDlg>(itr);

		pItem->SetInfo( pPupilList->PupilInfoList[itr] );
	}

	IsIntroduction();
}

void CDnPupilListDlg::RefreshLeave()
{
	const int iPenaltyRespectPoint = GetMasterTask().GetPenaltyRespectPoint();

	CONTROL( Static, ID_TEXT_RESPECT )->SetIntToText( CONTROL( Static, ID_TEXT_RESPECT )->GetTextToInt() - iPenaltyRespectPoint );

	m_pListBoxEx->RemoveItem( m_pListBoxEx->GetSelectedIndex() );

	IsIntroduction();
}

void CDnPupilListDlg::RefreshRespectPoint()
{
	
}

void CDnPupilListDlg::IsIntroduction()
{
	if ( GetMasterTask().GetSimpleInfo().iPupilCount >= MasterSystem::Max::PupilCount )
		m_pTitleButton->Enable( false );
	else
		m_pTitleButton->Enable( true );

	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));

	if( pGameTask )
		m_pTitleButton->Enable( false );
	else
		m_pTitleButton->Enable( true );
}
