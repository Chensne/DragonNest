#include "StdAfx.h"
#include "DnChatRoomPopupDlg.h"
#include "DnInterface.h"
#include "DnFriendTask.h"
#include "TaskManager.h"
#include "DnChatRoomTask.h"
#include "DnGuildTask.h"
#include "DnTradeTask.h"
#include "DnTradePrivateMarket.h"
#include "DnPartyKickDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatRoomPopupDlg::CDnChatRoomPopupDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pButtonFriend(NULL)
, m_pButtonPrivate(NULL)
, m_pButtonGuild(NULL)
, m_pButtonExile(NULL)
, m_pButtonNameCopy(NULL)
, m_pKickDlg(NULL)
, m_bMaster( false )
{
}

CDnChatRoomPopupDlg::~CDnChatRoomPopupDlg(void)
{
	SAFE_DELETE(m_pKickDlg);
}

void CDnChatRoomPopupDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatPopup.ui" ).c_str(), bShow );
}

void CDnChatRoomPopupDlg::InitialUpdate()
{
	m_pButtonFriend = GetControl<CEtUIButton>("ID_BUTTON_FRIEND");
	m_pButtonPrivate = GetControl<CEtUIButton>("ID_BUTTON_TRADE");
	m_pButtonGuild = GetControl<CEtUIButton>("ID_BUTTON_GUILD");
	m_pButtonExile = GetControl<CEtUIButton>("ID_BUTTON_EXILE");
	m_pButtonNameCopy = GetControl<CEtUIButton>("ID_BUTTON_SAVE");

	m_pKickDlg = new CDnPartyKickDlg( UI_TYPE_MODAL, NULL, KICK_DIALOG, this );
	m_pKickDlg->Initialize( false );

	// 상속받아 InitialUpdate재정의 해도 되지만, 이게 더 편하다.
	m_pKickDlg->GetControl( "ID_STATIC_TITLE" )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8135 ) );
	m_pKickDlg->GetControl( "ID_STATIC_TEXT" )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8136 ) );
}

void CDnChatRoomPopupDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		InitControl();
	}
	else
	{
		m_pButtonFriend->Enable( false );
		m_pButtonPrivate->Enable( false );
		m_pButtonGuild->Enable( false );
		m_pButtonExile->Enable( false );
		m_pButtonNameCopy->Enable( false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnChatRoomPopupDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_FRIEND") )
		{
			wchar_t wszQuery[255];
			swprintf_s(wszQuery, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1418 ), m_wszName.c_str() );	// UISTRING : %s님을 친구목록에 추가하시겠습니까?
			GetInterface().MessageBox( wszQuery, MB_YESNO, MESSAGEBOX_FRIEND_QUERY, this );
		}
		else if( IsCmdControl("ID_BUTTON_TRADE") )
		{
			DnActorHandle hActor = CDnActor::FindActorFromName( (TCHAR *)m_wszName.c_str() );
			if( hActor )
				GetTradeTask().GetTradePrivateMarket().RequestPrivateMarket( hActor->GetUniqueID() );
		}
		else if( IsCmdControl("ID_BUTTON_GUILD") )
		{
			GetGuildTask().RequestInviteGuildMember( m_wszName.c_str() );
		}
		else if( IsCmdControl("ID_BUTTON_EXILE") )
		{
			m_pKickDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_SAVE") )
		{
			CEtUILineEditBox::CopyStringToClipboard( m_wszName.c_str() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1422 ), false );	// UISTRING : 해당 캐릭터의 이름을 저장하였습니다. 이제 붙여넣기를 할 수 있습니다.
		}

		Show(false);
		return;
	}
}

void CDnChatRoomPopupDlg::SetInfo( bool bMaster, LPCWSTR pwszName )
{
	m_bMaster = bMaster;
	m_wszName = pwszName;
}

void CDnChatRoomPopupDlg::InitControl()
{
	if( !CDnActor::s_hLocalActor ) return;

	if( __wcsicmp_l( m_wszName.c_str(), CDnActor::s_hLocalActor->GetName() ) != 0 ) {
		m_pButtonPrivate->Enable( true );
		m_pButtonFriend->Enable( true );
		m_pButtonGuild->Enable( true );
		m_pButtonNameCopy->Enable( true );
	}

	// 다이렉트메뉴와 달리 여기선 옵션체크 검사를 하지 않는다.
	// 방에 들어온거 자체를 거래나 길드가입 하려고 들어온 것으로 가정하는 것.

	// 자신이 누군가에게 거래를 요청중이라면, 혹은 누군가 자신에게 거래를 요청중이라면,
	if( GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() )
		m_pButtonPrivate->Enable( false );

	// 친구추가는 한번 더 검사해야한다. 이미 친추되어있으면 false.
	if( !CheckFriendCondition(m_wszName.c_str()) )
		m_pButtonFriend->Enable( false );

	// 길드 초대 가능한지.
	if( m_pButtonGuild->IsEnable() ) {
		if( GetGuildTask().GetGuildInfo()->IsSet() &&
			GetGuildTask().GetAuth( static_cast<eGuildRoleType>(GetGuildTask().GetMyRole()), GUILDAUTH_TYPE_INVITE ) &&
			CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) {
				m_pButtonGuild->Enable( true );
		}
		else {
			m_pButtonGuild->Enable( false );
		}
	}

	// 추방은 방장만 가능.
	if( m_bMaster ) {
		if( __wcsicmp_l( m_wszName.c_str(), CDnActor::s_hLocalActor->GetName() ) != 0 )
			m_pButtonExile->Enable( m_bMaster );
	}
}

void CDnChatRoomPopupDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_FRIEND_QUERY:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if (IsCmdControl("ID_YES"))
				{
					GetFriendTask().RequestFriendAdd( 0, m_wszName.c_str() );
				}
			}
		}
		break;
	case KICK_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_NORMAL_KICK") )
				{
					GetChatRoomTask().RequestKickUser( m_wszName.c_str(), false );
				}
				else if( IsCmdControl("ID_PERMAN_KICK") )
				{
					GetChatRoomTask().RequestKickUser( m_wszName.c_str(), true );
				}
				m_pKickDlg->Show( false );
			}
		}
		break;
	}
}

bool CDnChatRoomPopupDlg::CheckFriendCondition(const WCHAR* name)
{
	CDnFriendTask* pFriendTask = static_cast<CDnFriendTask*>(CTaskManager::GetInstance().GetTask( "FriendTask" ));
	if( !pFriendTask )
		return false;

	if (pFriendTask->IsFriend(name))
		return false;
	return true;
}

bool CDnChatRoomPopupDlg::IsFriend( const WCHAR *name )
{
	CDnFriendTask* pFriendTask = static_cast<CDnFriendTask*>(CTaskManager::GetInstance().GetTask( "FriendTask" ));
	bool bFriend = ( pFriendTask != NULL ) && pFriendTask->IsFriend(name);
	return bFriend;
}