#include "StdAfx.h"
#include "DnGuildMemberInfoDlg.h"
#include "DnGuildTask.h"
#include "DnActor.h"
#include "DnInterfaceString.h"
#include "SyncTimer.h"
#include "DnGuildDlg.h"
#include "DnGuildIntroduceDlg.h"
#include "DnPartyTask.h"
#include "TaskManager.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildMemberInfoDlg::CDnGuildMemberInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pGuildDlg(NULL)
, m_pGuildIntroduceDlg(NULL)
, m_nCharacterDBID(0)
{
}

CDnGuildMemberInfoDlg::~CDnGuildMemberInfoDlg(void)
{
	SAFE_DELETE( m_pGuildIntroduceDlg );
}

void CDnGuildMemberInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildMemberInfoDlg.ui" ).c_str(), bShow );
}

void CDnGuildMemberInfoDlg::InitialUpdate()
{
	m_pGuildIntroduceDlg = new CDnGuildIntroduceDlg( UI_TYPE_MODAL, NULL );
	m_pGuildIntroduceDlg->Initialize( false );
}

void CDnGuildMemberInfoDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON0") )
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_BUTTON_INTRODUCE") )
		{
			if( m_pGuildDlg && m_pGuildDlg->CheckValidCommand() )
				m_pGuildIntroduceDlg->Show( true );
		}
		else if( IsCmdControl("ID_BUTTON_INVITE") )
		{
			GetPartyTask().ReqInviteParty( GetControl<CEtUIStatic>("ID_STATIC_NAME")->GetText() );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildMemberInfoDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildMemberInfoDlg::SetGuildMemberDBID( INT64 nDBID )
{
	m_nCharacterDBID = nDBID;
	InitControl();
}

void CDnGuildMemberInfoDlg::InitControl()
{
	if( !CDnActor::s_hLocalActor ) return;

	// DBID로 검사한다.
	TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( m_nCharacterDBID );
	if( !pCurMember ) return;

	GetControl<CEtUIStatic>("ID_STATIC_NAME")->SetText( pCurMember->wszCharacterName );
	GetControl<CEtUIStatic>("ID_STATIC_USERLEVEL")->SetIntToText( pCurMember->cLevel );
	GetControl<CEtUIStatic>("ID_STATIC_CLASS")->SetText( DN_INTERFACE::STRING::GetJobString( pCurMember->nJob ) );

	WCHAR wszRole[32];
#ifdef PRE_ADD_BEGINNERGUILD
	if (CDnGuildTask::IsActive() && GetGuildTask().IsBeginnerGuild())
	{
		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1385 ) ); // UISTRING : 초보자
	}
	else
	{
		switch( pCurMember->btGuildRole )
		{
		case GUILDROLE_TYPE_MASTER:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );	break;
		case GUILDROLE_TYPE_SUBMASTER:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );	break;
		case GUILDROLE_TYPE_SENIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ) );	break;
		case GUILDROLE_TYPE_REGULAR:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ) );	break;
		case GUILDROLE_TYPE_JUNIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ) );	break;
		}
	}
#else
	switch( pCurMember->btGuildRole )
	{
	case GUILDROLE_TYPE_MASTER:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3733 ) );	break;
	case GUILDROLE_TYPE_SUBMASTER:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ) );	break;
	case GUILDROLE_TYPE_SENIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ) );	break;
	case GUILDROLE_TYPE_REGULAR:	swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ) );	break;
	case GUILDROLE_TYPE_JUNIOR:		swprintf_s( wszRole, _countof(wszRole), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ) );	break;
	}
#endif
	GetControl<CEtUIStatic>("ID_STATIC_GRADE")->SetText( wszRole );

	std::wstring wszStr;
	DN_INTERFACE::STRING::GetLocationText( wszStr, pCurMember->Location );
	GetControl<CEtUIStatic>("ID_STATIC_MAP")->SetText( wszStr );

	std::wstring wszLogout;
	if( pCurMember->Location.cServerLocation != _LOCATION_NONE )
	{
		// 접속중이면 접속 중 표시.
		wszLogout = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3798 );
	}
	else
	{
		// 역할 이름과 최근 접속종료시간 구한 후
		// 만약 0이면 한번도 로그아웃 한게 없으니 그냥 안찍고,
		// -1이면 잘못보내준거니 안찍고,
		// 그 외 값이면 현재부터 시간값 차이 구해서, 
		if( pCurMember->LastLogoutDate != 0 && pCurMember->LastLogoutDate != -1 ) {
			if( m_pGuildDlg ) m_pGuildDlg->GetLogoutText( wszLogout, pCurMember->LastLogoutDate );
		}
	}
	GetControl<CEtUIStatic>("ID_STATIC_ONLINE")->SetText( wszLogout );
	GetControl<CEtUIStatic>("ID_STATIC_MEMO")->SetText( pCurMember->wszGuildMemberIntroduce );

	// 아래 기능 둘다 마을에서만 가능하게 한다.
	bool bVillage = true;
	if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
		bVillage = false;

	// 자신의 소개만 변경 가능.
	bool bMe = true;
	if( __wcsicmp_l( pCurMember->wszCharacterName, CDnActor::s_hLocalActor->GetName() ) != 0 )
		bMe = false;
	bool bIntroduce = false;
	if( bMe && bVillage ) bIntroduce = true;
	GetControl<CEtUIButton>("ID_BUTTON_INTRODUCE")->Enable( bIntroduce );

	// 자신이 아니면서 접속중인 파티원에게는 파티초대가 가능
	bool bParty = true;
	if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeWorldMap ) bParty = false;
	if( bMe ) bParty = false;
	if( pCurMember->Location.cServerLocation == _LOCATION_NONE ) bParty = false;
	if( !CDnPartyTask::IsActive() ) bParty = false;
	CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstance().GetTask( "PartyTask" ));
	if( pPartyTask && bParty )
	{
		if( pPartyTask->IsPartyMember( pCurMember->wszCharacterName ) ) bParty = false;
		if( pPartyTask->GetPartyRole() == CDnPartyTask::MEMBER ) bParty = false;
		if( pPartyTask->GetPartyRole() == CDnPartyTask::LEADER )
		{
			if( pPartyTask->IsPartyFull() ) bParty = false;
		}
		if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap )
		{
			if( pPartyTask->GetPartyRole() == CDnPartyTask::SINGLE ) bParty = false;
			if( pPartyTask->IsPartyMemberInviting() ) bParty = false;
		}
	}
	GetControl<CEtUIButton>("ID_BUTTON_INVITE")->Enable( bParty );
}