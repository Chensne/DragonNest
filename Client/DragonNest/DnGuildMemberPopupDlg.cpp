#include "StdAfx.h"
#include "DnGuildMemberPopupDlg.h"
#include "DnInterface.h"
#include "DnFriendTask.h"
#include "TaskManager.h"
#include "DnPlayerActor.h"
#include "DnGuildTask.h"
#include "DnChatTabDlg.h"
#include "DnGuildRoleChangeDlg.h"
#include "DnWorld.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMemberPopupDlg::CDnGuildMemberPopupDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pButtonPrivate(NULL)
, m_pButtonFriend(NULL)
, m_pButtonCopyName(NULL)
, m_pButtonChangeRole(NULL)
, m_pButtonExile(NULL)
, m_pButtonMaster(NULL)
, m_pButtonClose(NULL)
, m_pGuildRoleChangeDlg(NULL)
, m_nCharacterDBID(0)
{
}

CDnGuildMemberPopupDlg::~CDnGuildMemberPopupDlg(void)
{
	SAFE_DELETE(m_pGuildRoleChangeDlg);
}

void CDnGuildMemberPopupDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildMemberPopupDlg.ui" ).c_str(), bShow );
}

void CDnGuildMemberPopupDlg::InitialUpdate()
{
	m_pButtonPrivate = GetControl<CEtUIButton>("ID_BUTTON_WHISPER");
	m_pButtonFriend = GetControl<CEtUIButton>("ID_BUTTON_FRIEND");
	m_pButtonCopyName = GetControl<CEtUIButton>("ID_BUTTON_COPYNAME");
	m_pButtonChangeRole = GetControl<CEtUIButton>("ID_BUTTON_GRADE");
	m_pButtonExile = GetControl<CEtUIButton>("ID_BUTTON_GUILDOUT");
	m_pButtonMaster = GetControl<CEtUIButton>("ID_BUTTON_BOSS");
	m_pButtonClose = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

	m_pGuildRoleChangeDlg = new CDnGuildRoleChangeDlg( UI_TYPE_MODAL );
	m_pGuildRoleChangeDlg->Initialize( false );
}

void CDnGuildMemberPopupDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		InitControl();
	}
	else
	{
		m_pButtonPrivate->Enable(false);
		m_pButtonFriend->Enable(false);
		m_pButtonCopyName->Enable(false);
		m_pButtonChangeRole->Enable(false);
		m_pButtonExile->Enable(false);
		m_pButtonMaster->Enable(false);
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildMemberPopupDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	// DBID로 검사한다.
	TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( m_nCharacterDBID );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_WHISPER")) 
		{
			GetInterface().GetChatDialog()->ShowEx(true);
			GetInterface().GetChatDialog()->SetPrivateName( pCurMember->wszCharacterName );
		}
		else if( IsCmdControl("ID_BUTTON_FRIEND") )
		{
			if( !pCurMember ) return;
			wchar_t wszQuery[255];
			swprintf_s(wszQuery, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1418 ), pCurMember->wszCharacterName );	// UISTRING : %s님을 친구목록에 추가하시겠습니까?
			GetInterface().MessageBox( wszQuery, MB_YESNO, MESSAGEBOX_FRIEND_QUERY, this);
		}
		else if( IsCmdControl("ID_BUTTON_COPYNAME") )
		{
			if( !pCurMember ) return;
			CEtUILineEditBox::CopyStringToClipboard( const_cast<wchar_t*>(pCurMember->wszCharacterName) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1422 ), false );
		}
		else if( IsCmdControl("ID_BUTTON_GRADE") )
		{
			if( CheckValidCommand() )
			{
				m_pGuildRoleChangeDlg->SetGuildMemberDBID( m_nCharacterDBID );
				m_pGuildRoleChangeDlg->Show( true );
			}
		}
		else if( IsCmdControl("ID_BUTTON_GUILDOUT") )
		{
			if( !pCurMember ) return;

			if( CheckValidCommand() )
			{
				wchar_t wszQuery[255];
				swprintf_s(wszQuery, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3787 ), pCurMember->wszCharacterName );
				GetInterface().MessageBox( wszQuery, MB_YESNO, GUILD_EXILE_DIALOG, this );
			}
		}
		else if( IsCmdControl("ID_BUTTON_BOSS") )
		{
			if( !pCurMember ) return;

			if( CheckValidCommand() )
			{
				wchar_t wszQuery[255];
				swprintf_s(wszQuery, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3788 ), pCurMember->wszCharacterName );
				GetInterface().MessageBox( wszQuery, MB_YESNO, GUILD_MASTER_DIALOG, this );
			}
		}

		Show(false);
		return;
	}
}

void CDnGuildMemberPopupDlg::InitControl()
{
	if( !CDnActor::s_hLocalActor ) return;

	// DBID로 검사한다.
	TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( m_nCharacterDBID );
	if( !pCurMember ) return;

	// 귓속말, 친구추가는 자신빼고 나머지 길드원에게 가능
	if( __wcsicmp_l( pCurMember->wszCharacterName, CDnActor::s_hLocalActor->GetName() ) != 0 ) {
		m_pButtonPrivate->Enable(true);
		m_pButtonFriend->Enable(true);
	}

	// 친구추가는 한번 더 검사해야한다. 이미 친추되어있으면 false.
	if( !CheckFriendCondition(pCurMember->wszCharacterName) )
		m_pButtonFriend->Enable(false);

	// 이름 복사은 언제나 true
	m_pButtonCopyName->Enable(true);

	// 길드장의 경우엔 자신을 제외한 모든 길드원의 직급변경, 추방, 위임이 가능
	CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pPlayer->IsGuildMaster() ) {
		if( pCurMember->btGuildRole != GUILDROLE_TYPE_MASTER ) {
			m_pButtonChangeRole->Enable(true);
			m_pButtonExile->Enable(true);
			m_pButtonMaster->Enable(true);
		}
	}

	// 길드부장도 길드장, 길드부장 제외한 나머지 길드원의 직급 변경, 추방이 가능해진다.
	if( pPlayer->GetGuildRole() == GUILDROLE_TYPE_SUBMASTER ) {
		if( pCurMember->btGuildRole != GUILDROLE_TYPE_MASTER && pCurMember->btGuildRole != GUILDROLE_TYPE_SUBMASTER ) {
			m_pButtonChangeRole->Enable(true);
			if( GetGuildTask().GetAuth( GUILDROLE_TYPE_SUBMASTER, GUILDAUTH_TYPE_EXILE ) )
				m_pButtonExile->Enable(true);
		}
	}

#ifdef PRE_DEBUG_GUILDAUTH
	m_pButtonChangeRole->Enable(true);
	m_pButtonExile->Enable(true);
	m_pButtonMaster->Enable(true);
#endif	// #ifdef PRE_DEBUG_GUILDAUTH
}

void CDnGuildMemberPopupDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( m_nCharacterDBID );
	if( !pCurMember ) return;

	switch( nID ) 
	{
	case MESSAGEBOX_FRIEND_QUERY:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if (IsCmdControl("ID_YES"))
				{
					GetFriendTask().RequestFriendAdd( 0, pCurMember->wszCharacterName );
				}
			}
		}
		break;
	case GUILD_EXILE_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if (IsCmdControl("ID_YES"))
				{
					GetGuildTask().RequestExileGuild( pCurMember->nAccountDBID, pCurMember->nCharacterDBID );
				}
			}
		}
		break;
	case GUILD_MASTER_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if (IsCmdControl("ID_YES"))
				{
					GetGuildTask().RequestChangeGuildMemberInfo( GUILDMEMBUPDATE_TYPE_GUILDMASTER, 0, 0, 0, 0, pCurMember->nAccountDBID, pCurMember->nCharacterDBID );
				}
			}
		}
		break;
	}
}

bool CDnGuildMemberPopupDlg::CheckFriendCondition(const WCHAR* name)
{
	CDnFriendTask* pFriendTask = static_cast<CDnFriendTask*>(CTaskManager::GetInstance().GetTask( "FriendTask" ));
	if( !pFriendTask )
		return false;

	if (pFriendTask->IsFriend(name))
		return false;
	return true;
}

bool CDnGuildMemberPopupDlg::IsFriend( const WCHAR *name )
{
	CDnFriendTask* pFriendTask = static_cast<CDnFriendTask*>(CTaskManager::GetInstance().GetTask( "FriendTask" ));
	bool bFriend = ( pFriendTask != NULL ) && pFriendTask->IsFriend(name);
	return bFriend;
}

bool CDnGuildMemberPopupDlg::CheckValidCommand()
{
	if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3797 ) );
		return false;
	}
	return true;
}
