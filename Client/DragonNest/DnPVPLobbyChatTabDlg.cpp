#include "StdAfx.h"
#include "DnPVPLobbyChatTabDlg.h"
#include "DnChatTabDlg.h"
#include "DnChatDlg.h"
#include "DnChatOptDlg.h"
#include "VillageSendPacket.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "PartySendPacket.h"
#include "EtUIXML.h"
#include "DebugCmdProc.h"
#include "DnChatHelpDlg.h"
#include "DnInterfaceString.h"
#include "DnLocalPlayerActor.h"
#include "MAChatBalloon.h"
#include "DnInvenTabDlg.h"
#include "DnPartyTask.h"
#include "DnTradeTask.h"
#include "DnMouseCursor.h"
#include "DnPVPChatDlg.h"
#include "DnPVPGameRoomDlg.h"
#include "TaskManager.h"
#include "DnPVPLobbyVillageTask.h"

#include "DnPVPObserverItemDlg.h"
#include "DnPVPGameRoomDlg.h"
#include "DnItemTask.h"

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
#include "DnLocalDataMgr.h"
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLobbyChatTabDlg::CDnPVPLobbyChatTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnChatTabDlg( dialogType, pParentDialog, nID, pCallback )
, m_pObserverListBox(NULL)
, m_pButtonCharInfo(NULL)
, m_pButtonObserver(NULL)
, m_pButtonConfig(NULL)
, m_pPVPOptionDlg(NULL)
, m_pPVPObserverDlg(NULL)
, m_pPVPInfoDlg(NULL)
, m_bInitPvpInfo(false)
#ifdef PRE_MOD_PVPOBSERVER
,m_bObserverMode(false)
#endif // PRE_MOD_PVPOBSERVER
#ifdef PRE_ADD_DWC
, m_bIsDWCUser(false)
#endif
{
	m_sObserverSlot.clear();
	m_SelectList = false;
}

CDnPVPLobbyChatTabDlg::~CDnPVPLobbyChatTabDlg(void)
{
	SAFE_DELETE( m_pChatHelpDlg );	
	SAFE_DELETE( m_pPVPObserverDlg );
	SAFE_DELETE( m_pPVPInfoDlg );
	SAFE_DELETE( m_pPVPOptionDlg );
}

void CDnPVPLobbyChatTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpChat.ui" ).c_str(), bShow );
	SetFadeMode( CEtUIDialog::AllowRender );
}

void CDnPVPLobbyChatTabDlg::InitialUpdate()
{
	m_pButtonCharInfo = GetControl<CEtUIRadioButton>("ID_RBT_INFO");
	m_pButtonCharInfo->Show(true);
	m_pButtonCharInfo->SetChecked(true,false);

	m_pButtonObserver = GetControl<CEtUIRadioButton>("ID_RBT_LIST");
	m_pButtonObserver->Show(false);
#ifdef PRE_MOD_PVPOBSERVER				
	SetTextObserver();
#endif // PRE_MOD_PVPOBSERVER

	m_pButtonConfig = GetControl<CEtUIRadioButton>("ID_RBT_OPTION");
	m_pButtonConfig->Show(true);

	m_pPVPObserverDlg = new CDnPVPObserverDlg(UI_TYPE_CHILD,this);
	m_pPVPInfoDlg = new CDnPVPInfoDlg( UI_TYPE_CHILD, this );

	m_pPVPObserverDlg->Initialize( true );
	m_pPVPInfoDlg->Initialize( true );

	m_pPVPOptionDlg = new CDnPVPOptionDlg( UI_TYPE_CHILD, this);
	m_pPVPOptionDlg->Initialize(false);

	if(m_pPVPObserverDlg && m_pPVPObserverDlg->GetObserverListBox())
 		m_pObserverListBox = m_pPVPObserverDlg->GetObserverListBox(); // 리스트자체가 하위로 분리가 되었기때문에 하위 클래스의 포인터를 넘겨주는 형식으로 바뀝니다.

	// Note : 일반 탭과 창
	m_pNormalTab = GetControl<CEtUIRadioButton>("ID_TAB_ALL");
	m_pChatNormalDlg = new CDnPVPChatDlg( UI_TYPE_CHILD, this );
	m_pChatNormalDlg->Initialize( false );
	AddTabDialog( m_pNormalTab, m_pChatNormalDlg );
	m_vecChatDlg.push_back( m_pChatNormalDlg );

	// Note : 귓속말 탭과 창.
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pWhisperTab = GetControl<CEtUIRadioButton>("ID_TAB_SECRET");
	m_pChatWhisperDlg = new CDnPVPChatDlg( UI_TYPE_CHILD, this ); 
	m_pChatWhisperDlg->Initialize( false );
	AddTabDialog( m_pWhisperTab, m_pChatWhisperDlg );
	m_vecChatDlg.push_back( m_pChatWhisperDlg );
#endif

	// Note : 파티 탭과 창
	m_pPartyTab = GetControl<CEtUIRadioButton>("ID_TAB_PARTY");
	m_pChatPartyDlg = new CDnPVPChatDlg( UI_TYPE_CHILD, this );
	m_pChatPartyDlg->Initialize( false );
	AddTabDialog( m_pPartyTab, m_pChatPartyDlg );
	m_vecChatDlg.push_back( m_pChatPartyDlg );
#ifndef PRE_ADD_CHAT_RENEWAL
	m_pPartyTab->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 632 ) );
#endif // PRE_ADD_CHAT_RENEWAL
	m_pPartyTab->Enable(false);	

	// Note : 길드 탭과 창
	m_pGuildTab = GetControl<CEtUIRadioButton>("ID_TAB_GUILD");
	m_pChatGuildDlg = new CDnPVPChatDlg( UI_TYPE_CHILD, this );
	m_pChatGuildDlg->Initialize( false );
	AddTabDialog( m_pGuildTab, m_pChatGuildDlg );
	m_vecChatDlg.push_back( m_pChatGuildDlg );

	// Note : 시스템 탭과 창
	m_pSystemTab = GetControl<CEtUIRadioButton>("ID_TAB_SYSTEM");
	m_pChatSystemDlg = new CDnPVPChatDlg( UI_TYPE_CHILD, this );
	m_pChatSystemDlg->Initialize( false );
	AddTabDialog( m_pSystemTab, m_pChatSystemDlg );
	m_vecChatDlg.push_back( m_pChatSystemDlg );

#ifdef PRE_PRIVATECHAT_CHANNEL
	// Note : 사설 채널 탭과 창
	m_pPrivateChannelTab = GetControl<CEtUIRadioButton>( "ID_TAB_CHANNEL" );
	m_pPrivateChannelDlg = new CDnPVPChatDlg( UI_TYPE_CHILD, this );
	m_pPrivateChannelDlg->Initialize( false );
	AddTabDialog( m_pPrivateChannelTab, m_pPrivateChannelDlg );
	m_vecChatDlg.push_back( m_pPrivateChannelDlg );
#endif // PRE_PRIVATECHAT_CHANNEL

#ifndef PRE_ADD_CHAT_RENEWAL
	WCHAR wszTemp[64];
	swprintf_s(wszTemp, 64, L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 601 ));
	m_pNormalTab->SetText( wszTemp );
#endif // PRE_ADD_CHAT_RENEWAL

	m_pChatNormalDlg->InitOption( CHAT_NORMAL );
	m_pChatPartyDlg->InitOption( CHAT_PARTY );
	m_pChatGuildDlg->InitOption( CHAT_GUILD );
	m_pChatSystemDlg->InitOption( CHAT_SYSTEM );
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pChatWhisperDlg->InitOption( CHAT_PRIVATE );
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pPrivateChannelDlg->InitOption( CHAT_PRIVATE_CHANNEL );
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	CDnLocalDataMgr::GetInstance().SetChatOption( m_pChatNormalDlg->GetChatOptDlg().GetChatTabType(), m_pChatNormalDlg->GetChatOptDlg().GetOption() );
	CDnLocalDataMgr::GetInstance().SetChatOption( m_pChatPartyDlg->GetChatOptDlg().GetChatTabType(), m_pChatPartyDlg->GetChatOptDlg().GetOption() );
	CDnLocalDataMgr::GetInstance().SetChatOption( m_pChatGuildDlg->GetChatOptDlg().GetChatTabType(), m_pChatGuildDlg->GetChatOptDlg().GetOption() );
	CDnLocalDataMgr::GetInstance().SetChatOption( m_pChatSystemDlg->GetChatOptDlg().GetChatTabType(), m_pChatSystemDlg->GetChatOptDlg().GetOption() );
	CDnLocalDataMgr::GetInstance().SetChatOption( m_pChatWhisperDlg->GetChatOptDlg().GetChatTabType(), m_pChatWhisperDlg->GetChatOptDlg().GetOption() );
	CDnLocalDataMgr::GetInstance().SetChatOption( m_pPrivateChannelDlg->GetChatOptDlg().GetChatTabType(), m_pPrivateChannelDlg->GetChatOptDlg().GetOption() );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL

	SetCheckedTab( m_pNormalTab->GetTabID() );

	m_mapCommandMode.insert( make_pair(std::wstring(L"/s "), CHAT_NORMAL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/S "), CHAT_NORMAL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/1 "), CHAT_NORMAL) );

	m_mapCommandMode.insert( make_pair(std::wstring(L"/p "), CHAT_PARTY) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/P "), CHAT_PARTY) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/2 "), CHAT_PARTY) );

	m_mapCommandMode.insert( make_pair(std::wstring(L"/g "), CHAT_GUILD) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/G "), CHAT_GUILD) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/3 "), CHAT_GUILD) );

	m_mapCommandMode.insert( make_pair(std::wstring(L"/w "), CHAT_PRIVATE) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/W "), CHAT_PRIVATE) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/4 "), CHAT_PRIVATE) );

#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/c "), CHAT_PRIVATE_CHANNEL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/C "), CHAT_PRIVATE_CHANNEL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/5 "), CHAT_PRIVATE_CHANNEL) );
#else // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/c "), CHAT_CHANNEL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/C "), CHAT_CHANNEL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/5 "), CHAT_CHANNEL) );

	m_mapCommandMode.insert( make_pair(std::wstring(L"/t "), CHAT_WORLD) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/T "), CHAT_WORLD) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/6 "), CHAT_WORLD) );
#endif // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/r "), CHAT_PRIVATE_REPLY) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/R "), CHAT_PRIVATE_REPLY) );

#if defined(_KRAZ) || defined(_RDEBUG)
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㄴ "), CHAT_NORMAL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅔ "), CHAT_PARTY) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅎ "), CHAT_GUILD) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅈ "), CHAT_PRIVATE) );
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅊ "), CHAT_PRIVATE_CHANNEL) );
#else // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅊ "), CHAT_CHANNEL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅆ "), CHAT_WORLD) );
#endif // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㄱ "), CHAT_PRIVATE_REPLY) );
#endif

	m_mapCommandMessage.insert( make_pair(std::wstring(L"/?"),			CMD_MSG_HELP) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/invite"),		CMD_MSG_INVITE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/makeparty"),	CMD_MSG_MAKEPARTY) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/leaveparty"),	CMD_MSG_LEAVEPARTY) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/ban"),		CMD_MSG_BAN) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/trade"),		CMD_MSG_TRADE) );
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chcreate"), CMD_MSG_CHANNEL_CREATE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chjoin"), CMD_MSG_CHANNEL_JOIN) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chpassword"), CMD_MSG_CHANNEL_PASSWORD) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chinvite"), CMD_MSG_CHANNEL_INVITE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chleave"), CMD_MSG_CHANNEL_LEAVE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chban"), CMD_MSG_CHANNEL_BAN) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chmaster"), CMD_MSG_CHANNEL_MASTER) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/chhelp"), CMD_MSG_CHANNEL_HELP) );
#endif // PRE_PRIVATECHAT_CHANNEL

#if defined(_KRAZ) || defined(_RDEBUG)
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/파티초대"),	CMD_MSG_INVITE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/파티생성"),	CMD_MSG_MAKEPARTY) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/파티탈퇴"),	CMD_MSG_LEAVEPARTY) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/파티추방"),	CMD_MSG_BAN) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/거래"),		CMD_MSG_TRADE) );
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/채널개설"), CMD_MSG_CHANNEL_CREATE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/채널참여"), CMD_MSG_CHANNEL_JOIN) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/채널비밀번호"), CMD_MSG_CHANNEL_PASSWORD) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/채널초대"), CMD_MSG_CHANNEL_INVITE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/나가기"), CMD_MSG_CHANNEL_LEAVE) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/채널추방"), CMD_MSG_CHANNEL_BAN) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/방장"), CMD_MSG_CHANNEL_MASTER) );
	m_mapCommandMessage.insert( make_pair(std::wstring(L"/채널도움말"), CMD_MSG_CHANNEL_HELP) );
#endif // PRE_PRIVATECHAT_CHANNEL
#endif

	m_pStaticMode = GetControl<CEtUIStatic>("ID_STATIC_MODE");
	m_pIMEEditBox = GetControl<CEtUIIMEEditBox>("ID_EDITBOX_CHAT");

	m_pButtonModeUp = GetControl<CEtUIButton>("ID_BUTTON_MODE_UP");
	m_pButtonModeDown = GetControl<CEtUIButton>("ID_BUTTON_MODE_DOWN");

	//m_pButtonSize = GetControl<CEtUIButton>("ID_BUTTON_SIZE");
	m_pButtonOption = GetControl<CEtUIButton>("ID_BUTTON_OPTION");
	//m_pButtonClose = GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
	m_pButtonHelp = GetControl<CEtUIButton>("ID_BUTTON_HELP");
	m_pButtonReport = GetControl<CEtUIButton>("ID_BUTTON_REPORT");
	m_pButtonReport->Show( false );

	m_pButtonModeUp->Enable(false);
	m_pButtonModeDown->Enable(false);
	m_pButtonOption->Enable(false);
	m_pButtonHelp->Enable(false);

	SetChatModeText();

	m_pChatHelpDlg = new CDnChatHelpDlg( UI_TYPE_CHILD, this, CHAT_HELP_DIALOG, this );
	m_pChatHelpDlg->Initialize( false );

	m_pChatHelpDlg->AddChatHelpText( L"/s", L"/0", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 501 ), CHAT_NORMAL, chatcolor::NORMAL );
	m_pChatHelpDlg->AddChatHelpText( L"/p", L"/1", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 632 ), CHAT_PARTY, chatcolor::PARTY );
#ifdef PRE_ADD_DWC
	int nMid = 5007; // Mid: 길드
	if(GetDWCTask().IsDWCChar()) nMid = 121018; // Mid: 팀
	m_pChatHelpDlg->AddChatHelpText( L"/g", L"/2", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMid ), CHAT_GUILD, chatcolor::GUILD );
#else
	m_pChatHelpDlg->AddChatHelpText( L"/g", L"/2", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5007 ), CHAT_GUILD, chatcolor::GUILD );
#endif
	
	m_pChatHelpDlg->AddChatHelpText( L"/w", L"/3", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 648 ), CHAT_PRIVATE, chatcolor::PRIVATE );
	
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pChatHelpDlg->AddChatHelpText( L"/c", L"/4", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 605 ), CHAT_PRIVATE_CHANNEL, chatcolor::PRIVATECHANNEL );
#else // PRE_PRIVATECHAT_CHANNEL
	m_pChatHelpDlg->AddChatHelpText( L"/c", L"/4", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 605 ), CHAT_CHANNEL, chatcolor::CHANNEL );
	m_pChatHelpDlg->AddChatHelpText( L"/t", L"/5", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 606 ), CHAT_WORLD, chatcolor::WORLD );
#endif // PRE_PRIVATECHAT_CHANNEL
	m_bPVPLobbyChat = true;

#ifdef PRE_ADD_DWC
	m_bIsDWCUser = GetDWCTask().IsDWCChar();
	if(m_bIsDWCUser)
		m_pButtonConfig->Show(false);
#endif // PRE_ADD_DWC
}

void CDnPVPLobbyChatTabDlg::Show( bool bShow )
{
	CDnChatTabDlg::Show( bShow );

	if( bShow == false )
	{
		if(m_pPVPOptionDlg )
		{
			m_pPVPOptionDlg->Show( false );
		}		
	}

	else
	{
		// #73276 pvp 대기실에서 설정탭을 선택후 다시 들어가면, 설정칸에 아무것도 없음.
		if( m_pButtonCharInfo )
			m_pButtonCharInfo->SetChecked( true );	
	}
}

void CDnPVPLobbyChatTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	CDnChatDlg *pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
	
	if( nCommand == EVENT_RADIOBUTTON_CHANGED ) 
	{
		if( pChatDlg )
		{
			pChatDlg->ShowOptDlg( false );
		}
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	pChatDlg = static_cast<CDnChatDlg*>(m_groupTabDialog.GetShowDialog());
	if( !pChatDlg )
		return;

	if( nCommand == EVENT_EDITBOX_KEYUP )
	{
		SetNextChatHistory();
		return;
	}

	if( nCommand == EVENT_EDITBOX_KEYDOWN )
	{
		SetPrevChatHistory();
		return;
	}

	if( nCommand == EVENT_EDITBOX_SHIFT_KEYUP )
	{
		SetChatNextMode();
		return;
	}

	if( nCommand == EVENT_EDITBOX_SHIFT_KEYDOWN )
	{
		SetChatPrevMode();
		return;
	}

	if( nCommand == EVENT_EDITBOX_LCTRL_KEYUP )
	{
		// 탭 교체
		int nNextTab = GetCurrentTabID() + 1;
		if( nNextTab > 4) nNextTab = 1;
		SetCheckedTab( nNextTab );
		return;
	}

	if( nCommand == EVENT_EDITBOX_LCTRL_KEYDOWN )
	{
		// 탭 교체
		int nNextTab = GetCurrentTabID() - 1;
		if( nNextTab < 1) nNextTab = 4;
		SetCheckedTab( nNextTab );
		return;
	}

	if( nCommand == EVENT_EDITBOX_STRING )
	{
		if( !m_bSetFocusEditBox )
		{
			AddEditBoxString();
		}
		else
		{
			m_bSetFocusEditBox = false;
		}

		return;
	}

	if( nCommand == EVENT_EDITBOX_ESCAPE )
	{
		focus::ReleaseControl();
		return;
	}

	if( nCommand == EVENT_EDITBOX_CHANGE )
	{
		if( GetInterface().GetGameRoomDlg() )
			GetInterface().GetGameRoomDlg()->BaseProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
	}

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_MODE_UP" ) )
		{
			SetChatNextMode();
			return;
		}

		if( IsCmdControl("ID_BUTTON_MODE_DOWN" ) )
		{
			SetChatPrevMode();
			return;
		}

		if( IsCmdControl("ID_BUTTON_OPTION" ) )
		{
			bool bShow = pChatDlg->IsShowOptDlg();
			pChatDlg->ShowOptDlg( !bShow );
			m_bShowOptDlg = !bShow;
			return;
		}

		if( IsCmdControl("ID_BUTTON_HELP") )
		{
			bool bShow = m_pChatHelpDlg->IsShow();
			ShowChildDialog( m_pChatHelpDlg, !bShow );
			return;
		}

		if( IsCmdControl("ID_BUTTON_REPORT") )
		{
			return;
		}
	}

	if( nCommand == EVENT_RADIOBUTTON_CHANGED ) 
	{
		if( IsCmdControl("ID_RBT_INFO") )
		{
			m_pPVPInfoDlg->Show(true);
			m_pPVPObserverDlg->Show(false);
			m_pPVPOptionDlg->Show(false);
		}
		if( IsCmdControl("ID_RBT_LIST") )
		{
			m_pPVPInfoDlg->Show(false);
			m_pPVPObserverDlg->Show(true);
			m_pPVPOptionDlg->Show(false);
		}
		if( IsCmdControl("ID_RBT_OPTION") )
		{
			m_pPVPInfoDlg->Show(false);
			m_pPVPObserverDlg->Show(false);
			m_pPVPOptionDlg->Show(true);
		}
		if( pControl == m_pButtonCharInfo
			|| pControl == m_pButtonObserver
			|| pControl == m_pButtonConfig
			)
		{
			CEtUIControl *pTabControl = (CEtUIControl *)GetCurrentTabControl();
			ProcessCommand( EVENT_RADIOBUTTON_CHANGED, false, pTabControl, 0 );
		}
		else if( pChatDlg->GetChatType() != CHAT_SYSTEM ) // 왜 버튼별로 안하고 컨트롤 들어올때마다 검사해서 넣어주는지 모르겟다.. 나중에 수정해야할듯.
		{
			SetChatMode( pChatDlg->GetChatType() );
		}

		if( m_bShowOptDlg )
		{
			pChatDlg->ShowOptDlg( true );
		}

		RequestFocus(m_pIMEEditBox);

		return;
	}

	if(pChatDlg && IsShow())
	{
		if(!pChatDlg->IsShowScrollBar())
			pChatDlg->ShowScrollBar(true); // 무조건 그려줍니다
	}
}

void CDnPVPLobbyChatTabDlg::Render( float fElapsedTime )
{
	if( !IsShow() )
		return;

	CDnPVPLobbyVillageTask *pPVPVillageTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if( !pPVPVillageTask ) 
	{
		m_bInitPvpInfo = false;                   // 나갓따 들어온경우를 대비
		return;
	}

	if(pPVPVillageTask && !m_bInitPvpInfo && pPVPVillageTask->GetEnterd())
	{
		RefreshMyPVPInfo(); // 정보자체를 pvpvillagetask 에서 받기때문에 , 테스크를 체크한뒤 초기값을 한번 셋해주어야한다.
		m_bInitPvpInfo = true;
	}

	// pvp로비의 경우에 후보리스트가 조합중인 글자 위로 뜨게 된다.
	// 이때 채팅내용은 자식윈도우기때문에 해당 텍스트박스보다 먼저 렌더링 될 수밖에 없는 구조다.
	// 이걸 막기 위해서, Render를 상속받아 재정의할까 했는데 너무 길어서, 편법을 쓰기로 한다.
	// 먼저 컨트롤 중에 IME를 골라내서(한개만 있다고 가정) NULL포인터로 대체 후 일반적인 렌더링 한 다음 다시 넣어둔다.
	int nIndex;
	CEtUIControl *pIMEEditBox = NULL;
	int nVecCtlSize = (int)m_vecControl.size();
	for( nIndex = 0; nIndex < nVecCtlSize; nIndex++ )
	{
		if( !m_vecControl[nIndex] ) continue;
		if( m_vecControl[nIndex]->GetType() == UI_CONTROL_IMEEDITBOX )
		{
			pIMEEditBox = m_vecControl[nIndex];
			m_vecControl[nIndex] = NULL;
			break;
		}
	}

	CDnChatTabDlg::Render( fElapsedTime );

	if( pIMEEditBox )
	{
		pIMEEditBox->Render( fElapsedTime );
		m_vecControl[nIndex] = pIMEEditBox;
	}
}

bool CDnPVPLobbyChatTabDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	CTask *pPVPVillageTask = CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if( !pPVPVillageTask ) return false;

#ifdef PRE_FIX_57852
	//PvP 채팅창에서 오른쪽 하단에서의 마우스 이벤트 처리를 위해서
	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDOWN || uMsg == WM_MOUSEMOVE)
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if (GetControl("ID_BOARD_OBSERVER"))
			m_bPvPLobbyChatOtherAreaInside = GetControl("ID_BOARD_OBSERVER")->IsInside(fMouseX, fMouseY);
	}

	return __super::MsgProc(hWnd, uMsg, wParam, lParam);
#else
	return CDnChatTabDlg::MsgProc( hWnd, uMsg, wParam, lParam );
#endif	
}

void CDnPVPLobbyChatTabDlg::ShowObserverDlg(bool bTrue)
{
	m_pButtonObserver->Show(bTrue); 
	m_pPVPObserverDlg->Show(bTrue);
	m_pPVPInfoDlg->Show(!bTrue);
	m_pButtonCharInfo->SetChecked(!bTrue,false);
	m_pButtonObserver->SetChecked(bTrue,false);

	if( CDnItemTask::IsActive() )
	{
		int nTotalFatigue = 0;
		int nTemp1, nTemp2, nTemp3, nTemp4;
		GetItemTask().GetFatigue( nTemp1, nTemp2, nTemp3, nTemp4 );
		nTotalFatigue = nTemp1 + nTemp2 + nTemp3 + nTemp4;
#if defined(PRE_ADD_VIP)
		int nTemp5 = GetItemTask().GetFatigue( CDnItemTask::eVIPFTG );
		nTotalFatigue += nTemp5;
#endif
		if( bTrue && nTotalFatigue )
		{
#ifdef PRE_ADD_DWC
			if(m_bIsDWCUser)
			{
				m_pPVPOptionDlg->Show(false);
				m_pPVPInfoDlg->Show(true);				
				m_pButtonCharInfo->SetChecked(true, false);
				m_pButtonObserver->SetChecked(false, false);
				m_pButtonConfig->SetChecked(false, false);				
			}
			else
			{
				m_pPVPOptionDlg->Show(true);
				m_pButtonCharInfo->SetChecked(false, false);
				m_pButtonObserver->SetChecked(false, false);
				m_pButtonConfig->SetChecked(true, false);
			}
#else
			m_pPVPOptionDlg->Show(true);
			m_pButtonCharInfo->SetChecked(false, false);
			m_pButtonObserver->SetChecked(false, false);
			m_pButtonConfig->SetChecked(true, false);
#endif
			
		}
		else
		{
			m_pButtonConfig->SetChecked(false, false);
		}
	}
	RefreshMyPVPInfo(); // PVP정보도 재갱신

#ifdef PRE_MOD_PVPOBSERVER
	if( bTrue && m_bObserverMode )
	{
		SUIControlProperty * pControlProperty = m_pButtonObserver->GetProperty();
		this->ClearRadioButtonGroup( pControlProperty->RadioButtonProperty.nButtonGroup );
		m_pButtonObserver->SetChecked( true );			
		this->ProcessCommand( EVENT_RADIOBUTTON_CHANGED, true, m_pButtonObserver );
	}
	
	if( !bTrue)
	{
		m_bObserverMode = false;
		SetTextObserver();
	}
#endif // PRE_MOD_PVPOBSERVER
}

void CDnPVPLobbyChatTabDlg::InsertObserver(int team , UINT uiUserState , UINT    uiSessionID , int iJobID , BYTE  cLevel , BYTE  cPVPLevel , const WCHAR * szPlayerName )
{
	// team 은 일단 사용안하는데 혹여나 나중에 필요한 상황을 위해서 냅둡니다.

	for(int i=0 ;i<(int)m_sObserverSlot.size();i++)
	{
		if(m_sObserverSlot[i].uiSessionID == uiSessionID) // 똑같은 사람이 두번 들어갈수는 없습니다.
			return;
	}

#ifdef PRE_MOD_PVPOBSERVER	
	int nPlayerCnt = GetInterface().IsEventRoom() ? PvPCommon::Common::ExtendMaxObserverPlayer : PvPCommon::Common::MaxObserverPlayer; 	
	if( (int)m_sObserverSlot.size() < nPlayerCnt ) // 운영자가 생성한 방의경우 옵저버가 16명.
#else
	if( (int)m_sObserverSlot.size() <  PvPCommon::Common::MaxObserverPlayer) // 지금은 옵져버가 4명만 들어가집니다.
#endif
	{
		if(!m_pObserverListBox) return;

		// 길드전에 들어오는 옵저버는 운영자라서 리스트에 추가 시켜 주지 않는다.
		if( !CDnBridgeTask::GetInstance().IsGuildWarSystem() )
		{
			CDnPVPObserverItemDlg *pItem = m_pObserverListBox->InsertItem<CDnPVPObserverItemDlg>((int)m_sObserverSlot.size());

			pItem->SetCharInfo(cLevel,DN_INTERFACE::STRING::GetJobString(iJobID),szPlayerName);
		}
	
		sObserverSlot sTempSlot;

		sTempSlot.uiUserState = uiUserState;
		sTempSlot.cPVPLevel = cPVPLevel;
		sTempSlot.cLevel = cLevel;
		sTempSlot.uiSessionID = uiSessionID;
		sTempSlot.iJobID = iJobID;
		swprintf_s(sTempSlot.wszPlayerName ,szPlayerName);
		m_sObserverSlot.push_back(sTempSlot);
#ifdef PRE_MOD_PVPOBSERVER		
		SetTextObserver();		
#endif // PRE_MOD_PVPOBSERVER
	}


#ifdef PRE_MOD_PVPOBSERVER
	CDnPVPLobbyVillageTask * pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if( pPVPLobbyTask )
	{
		if( __wcsicmp_l( szPlayerName, pPVPLobbyTask->GetUserInfo().wszCharacterName ) == 0 )
		{		
			m_bObserverMode = true;		
		}
	}
#endif // PRE_MOD_PVPOBSERVER
}

void CDnPVPLobbyChatTabDlg::RemoveObserver(int iSessionID ,bool bAll ) // bAll 옵져버 슬롯 완전 리셋!
{
	vector<sObserverSlot>::iterator iter;

	if(bAll)
	{
		m_pObserverListBox->RemoveAllItems();
		m_sObserverSlot.clear();

#ifdef PRE_MOD_PVPOBSERVER		
		SetTextObserver();		
#endif // PRE_MOD_PVPOBSERVER

		return;
	}
	
	for(int i=0; i<(int)m_sObserverSlot.size() ; i++)
	{
		if(m_sObserverSlot[i].uiSessionID == iSessionID ) // 나간사람과 옵져버 슬롯의 아이디가 같아요 = 나는 옵져버에요
		{
			m_pObserverListBox->RemoveItem(i);
			iter = m_sObserverSlot.begin();
			iter+=i;
			m_sObserverSlot.erase(iter);// 나간사람은 지워줘요
		}
	}
#ifdef PRE_MOD_PVPOBSERVER		
	SetTextObserver();		
#endif // PRE_MOD_PVPOBSERVER
}

CDnPVPLobbyChatTabDlg::sObserverSlot CDnPVPLobbyChatTabDlg::GetObserverSlotFromSessionID(int nSessionID)
{
	for(int i=0 ; i< (int)m_sObserverSlot.size() ; i++)
	{
		if(m_sObserverSlot[i].uiSessionID == nSessionID)
		{
			return m_sObserverSlot[i];
		}
	}

	CDnPVPLobbyChatTabDlg::sObserverSlot Temp;
	return Temp;
}

void CDnPVPLobbyChatTabDlg::SetObserverSlotUserState( const int nSessionID, const UINT uiUserState )
{
	for(int i=0 ; i< (int)m_sObserverSlot.size() ; i++)
	{
		if(m_sObserverSlot[i].uiSessionID == nSessionID)
		{
			m_sObserverSlot[i].uiUserState = uiUserState;
			return;
		}
	}
}

void CDnPVPLobbyChatTabDlg::RefreshMyPVPInfo()
{
	// 내 PVP 정보를 Refresh해줍니다.  이건 PVP 게임끝났을때 쏴줍니다 <메달획득 / XP정산이후가 됩니다>
	if(m_pPVPInfoDlg)
	{
#ifdef PRE_ADD_DWC
		m_pPVPInfoDlg->SetPVPInfo();
		if(GetDWCTask().IsDWCChar())
			m_pPVPInfoDlg->SetDWCInfo();
#else
		m_pPVPInfoDlg->SetPVPInfo();
#endif // PRE_ADD_DWC
	}
}

#ifdef PRE_MOD_PVPOBSERVER
void CDnPVPLobbyChatTabDlg::SetTextObserver()
{
	if( m_pButtonObserver )
	{
		std::wstring strObserver = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7855 ), m_sObserverSlot.size() ); // "관전자(%d)"		
		m_pButtonObserver->SetText( strObserver );
	}
}
#endif // PRE_MOD_PVPOBSERVER