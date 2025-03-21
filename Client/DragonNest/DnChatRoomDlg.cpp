#include "StdAfx.h"
#include "DnChatRoomDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "InputWrapper.h"
#include "ChatRoomSendPacket.h"
#include "DnLocalPlayerActor.h"
#include "DnRestraintTask.h"
#include "DnChatOption.h"
#include "DnInterfaceString.h"
#include "VillageSendPacket.h"
#include "DnNameLinkMng.h"
#include "DnChatRoomMemberListDlg.h"
#include "DnChatRoomConfigDlg.h"
#include "DnChatRoomPopupDlg.h"
#include "DnChatRoomTask.h"
#include "DnTradeTask.h"
#include "DnTradePrivateMarket.h"
#include "DnPrivateMarketDlg.h"
#include "DnPartyTask.h"
#ifdef PRE_ADD_MULTILANGUAGE
#include "DnChatDlg.h"
#endif // PRE_ADD_MULTILANGUAGE

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatRoomDlg::CDnChatRoomDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticTitle(NULL)
, m_nChatMode(CHAT_NORMAL)
, m_pButtonModeUp(NULL)
, m_pButtonModeDown(NULL)
, m_pStaticMode(NULL)
, m_pIMEEditBox(NULL)
, m_pButtonReport(NULL)
, m_pChatTextBox(NULL)
, m_pButtonDummy(NULL)
, m_pSelectMaster(NULL)
, m_pStaticMasterLevel(NULL)
, m_pStaticMasterJob(NULL)
, m_pStaticMasterName(NULL)
, m_pListBoxExPlayer(NULL)
, m_pChatRoomConfigDlg(NULL)
, m_pChatRoomPopupDlg(NULL)
, m_fPaperingRemainTime(0.0f)
, m_bLinked(false)
, m_bMasterIsMe( false )
, m_bPrivateModeChecker( false )
, m_nMasterSessionID( 0 )
, m_nRoomID( 0 )
{
	memset( m_pRenderPassChildDlg, 0, sizeof( m_pRenderPassChildDlg ) );
	memset( m_pTempChildDialog, 0, sizeof( m_pTempChildDialog ) );
}

CDnChatRoomDlg::~CDnChatRoomDlg(void)
{
	SAFE_DELETE( m_pChatRoomConfigDlg );
	SAFE_DELETE( m_pChatRoomPopupDlg );
	m_pListBoxExPlayer->RemoveAllItems();
}

void CDnChatRoomDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatRoomDlg.ui" ).c_str(), bShow );
}

void CDnChatRoomDlg::InitialUpdate()
{
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_STATIC_TITLE");

	m_pButtonModeUp = GetControl<CEtUIButton>("ID_BUTTON_MODE_UP");
	m_pButtonModeDown = GetControl<CEtUIButton>("ID_BUTTON_MODE_DOWN");
	m_pStaticMode = GetControl<CEtUIStatic>("ID_STATIC_MODE");
	m_pIMEEditBox = GetControl<CEtUIIMEEditBox>("ID_EDITBOX_CHAT");
	m_pButtonReport = GetControl<CEtUIButton>("ID_BUTTON_REPORT");

	m_pChatTextBox = GetControl<CEtUITextBox>("ID_TEXTBOX_CHAT");
	m_pButtonDummy = GetControl<CEtUIButton>("ID_BUTTON_DUMMY");

	m_pSelectMaster = GetControl<CEtUIStatic>("ID_STATIC_SELECT");
	m_pStaticMasterLevel = GetControl<CEtUIStatic>("ID_TEXT_LV");
	m_pStaticMasterJob = GetControl<CEtUIStatic>("ID_TEXT_JOB");
	m_pStaticMasterName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pListBoxExPlayer = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_USERLIST");

	m_pButtonReport->Show( false );
	m_pSelectMaster->Show( false );

	m_pButtonDummy->Show( false );	// 필요없는 듯.

	m_pChatRoomConfigDlg = new CDnChatRoomConfigDlg( UI_TYPE_CHILD_MODAL, this );	// ChatRoomCreateDlg가 메인메뉴 안에 있어서 어쩔 수 없이 따로 가진다.
	m_pChatRoomConfigDlg->Initialize( false );

	m_pChatRoomPopupDlg = new CDnChatRoomPopupDlg( UI_TYPE_CHILD, this );
	m_pChatRoomPopupDlg->Initialize( false );

	m_bLinked = false;


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

#if defined(_KR) || defined(_RDEBUG)
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㄴ "), CHAT_NORMAL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅔ "), CHAT_PARTY) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅎ "), CHAT_GUILD) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅈ "), CHAT_PRIVATE) );
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅊ "), CHAT_PRIVATE_CHANNEL) );
#else // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅊ "), CHAT_CHANNEL) );
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㅅ "), CHAT_WORLD) );
#endif // PRE_PRIVATECHAT_CHANNEL
	m_mapCommandMode.insert( make_pair(std::wstring(L"/ㄱ "), CHAT_PRIVATE_REPLY) );
#endif
}

void CDnChatRoomDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	GetTradeTask().GetTradePrivateMarket().ClearTradeUserInfoList();
#ifdef PRE_MOD_INTEG_SYSTEM_STATE
	GetPartyTask().ClearInviteInfoList(true);
#else
	GetPartyTask().ClearInviteInfoList();
#endif
	GetInterface().CloseGuildInviteReqDlg( true );

	if( bShow )
	{
		// 비전투모드로.
		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		if( pActor->IsBattleMode() )
		{
			if( !pActor->IsDie() && ( pActor->IsStay() || pActor->IsMove() ) )
			{
				if( pActor->IsMove() ) pActor->CmdStop( "Stand" );
				pActor->CmdToggleBattle( false );
			}
		}

		CDnLocalPlayerActor::LockInput( true );

		LinkChildDialog( true );

		GetControl<CEtUIButton>("ID_BUTTON_INVENTORY")->SetHotKey( _ToVK( g_UIWrappingKeyData[IW_UI_INVEN] ) );
		m_nChatMode = CHAT_NORMAL;
		SetChatModeText();
		m_pIMEEditBox->ClearText();
		RequestFocus( m_pIMEEditBox );
	}
	else
	{
		CDnLocalPlayerActor::LockInput( false );

		LinkChildDialog( false );

		m_pChatTextBox->ClearText();
		m_pListBoxExPlayer->RemoveAllItems();
		m_pSelectMaster->Show( false );
		m_bMasterIsMe = false;
		m_vecSessionID.clear();

		if( m_pChatRoomPopupDlg && m_pChatRoomPopupDlg->IsShow() )
			ShowChildDialog( m_pChatRoomPopupDlg, false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnChatRoomDlg::LinkChildDialog( bool bLink )
{
	if( m_bLinked == bLink )
		return;

	if( bLink )
	{
		// 강제로 Render를 호출하는 거로는 MsgProc처리가 복잡해져서 이렇게 임시로 Child 등록하는 방법을 택했다.
		// 보여질때 AllignType이랑 DlgCoord만 바꾸고 차일드로 연결했다가 다시 풀어주기만 하면 끝이다.
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
		{
			AddChildDialog( pInvenDlg );
			pInvenDlg->ShowRadioButton( false );
		}
		m_pTempChildDialog[0] = (CEtUIDialog *)pInvenDlg;
		m_pTempChildDialog[1] = (CEtUIDialog *)GetInterface().GetNameLinkToolTipDlg();
		m_pTempChildDialog[2] = GetInterface().GetAcceptRequestDialog();
		m_pTempChildDialog[3] = GetInterface().GetGuildInviteReqDlg();
		for( int i = 1; i < NUM_TEMP_CHILD; ++i )
			AddChildDialog( m_pTempChildDialog[i] );

		m_pRenderPassChildDlg[0] = m_pTempChildDialog[2];
		m_pRenderPassChildDlg[1] = m_pTempChildDialog[3];

		// 자식을 임시로 연결시켜 메세지처리는 되게하고, 렌더링에서 제외하는 걸로 하려 했더니
		// ChatRoomDlg가 Modal이라서 그냥 처리해서는 안된다.(두번 그려지는 거 방지하기 위해... 다음부터는 이런식으로 기획 안나오게 해야할 듯.)
		// 그래서,
		// InvenTabDlg은 메인메뉴 하이드 시키고, 자식으로 직접 그리고,
		// AcceptRequestDialog와 GuildInviteReqDlg는 그릴때 제외해서 그린다.
		GetInterface().GetMainMenuDialog()->Show( false );

		// 네임링크 툴팁의 경우엔
		// 자식으로 한번 렌더링 되고, 원래대로 UI_TYPE_BOTTOM_MSG 쪽에서 한번 렌더링, 그래서 두번 렌더링 된다.
		// 툴팁의 렌더쪽에서, 네임링크용 툴팁이며 채팅룸이 Show상태일때 패스할 수도 있지만,
		// 두번 겹쳐 렌더링된다해서 크게 문제될 상황이 아니라 그냥 넘어가기로 한다.

		// 마지막으로 
		SetRenderPriority( m_pChatRoomConfigDlg, true );
		SetRenderPriority( m_pChatRoomPopupDlg, true );
	}
	else
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
		{
			pInvenDlg->ShowRadioButton( true );
			if( pInvenDlg->IsShow() )
				ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BUTTON_INVENTORY"), 0 );
			DelChildDialog( pInvenDlg );
			m_pTempChildDialog[0] = NULL;
		}
		for( int i = 1; i < NUM_TEMP_CHILD; ++i ) {
			DelChildDialog( m_pTempChildDialog[i] );
			m_pTempChildDialog[i] = NULL;
		}

		GetInterface().GetMainMenuDialog()->Show( true );
	}
	m_bLinked = bLink;

	// 참고로 강제로 이렇게 자식 링크 걸어버리면, Process가 두번 호출되서 좀 더 빨리 Process되는데,
	// 거래신청은 시간계산을 태스크가 하기때문에 괜찮지만
	// 길드초대의 경우엔 다이얼로그에서 하기때문에 시간이 반밖에 안된다.
	// 그래서 시간값 넣는 곳에서 강제로 2배해서 처리해둔다.
}

void CDnChatRoomDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

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

		if( IsCmdControl("ID_BUTTON_OPTION") )
		{
			if( !GetTradeTask().GetTradePrivateMarket().GetPrivateMarketDlg()->IsShow() )
			{
				m_pChatRoomConfigDlg->SetInfo( GetChatRoomTask().GetChatRoomView(), GetChatRoomTask().GetChatRoomAllow(), GetChatRoomTask().GetPassword() );
				ShowChildDialog( m_pChatRoomConfigDlg, true );
			}
		}
		else if( IsCmdControl("ID_BUTTON_INVENTORY") )
		{
			if( !GetTradeTask().GetTradePrivateMarket().GetPrivateMarketDlg()->IsShow() )
			{
				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
				if( pInvenDlg )
					GetInterface().GetMainMenuDialog()->ToggleShowDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			}
		}
		if( IsCmdControl("ID_BUTTON_CLOSE") )
		{
			// 채팅룸 방장이라면 메세지가 바뀐다.
			if( m_bMasterIsMe && m_pListBoxExPlayer->GetSize() > 0 )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8137 ), MB_YESNO, MESSAGEBOX_EXITROOM, this, true, true );
			else
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8134 ), MB_YESNO, MESSAGEBOX_EXITROOM, this );
		}
	}
	else if( nCommand == EVENT_TEXTBOX_SELECTION )
	{
		if( IsCmdControl("ID_TEXTBOX_CHAT" ) )
		{
			CWord selectedWord = m_pChatTextBox->GetSelectedWordData();
			if (selectedWord.m_strWordWithTag.empty() == false)
				EtInterface::GetNameLinkMgr().TranslateText(std::wstring(), selectedWord.m_strWordWithTag.c_str(), this);
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		if( AddEditBoxString() )
		{
			focus::ReleaseControl();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnChatRoomDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	std::list<float>::iterator iter = m_listRecentChatTime.begin();
	while( iter != m_listRecentChatTime.end() )
	{
		*iter -= fElapsedTime;
		if( *iter <= 0.0f )
		{
			iter = m_listRecentChatTime.erase( iter );
			continue;
		}
		++iter;
	}

	if( m_fPaperingRemainTime > 0.0f )
		m_fPaperingRemainTime -= fElapsedTime;

	if( !IsShow() )
		return;

	if( m_pChatRoomPopupDlg->IsShow() || m_pChatRoomConfigDlg->IsShow() )
	{
		bool bTradeDlg = false;
		if( GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() )
			bTradeDlg = true;
		if( GetTradeTask().GetTradePrivateMarket().GetPrivateMarketDlg()->IsShow() )
			bTradeDlg = true;

		if( bTradeDlg )
		{
			if( m_pChatRoomConfigDlg->IsShow() )
				ShowChildDialog( m_pChatRoomConfigDlg, false );
			if( m_pChatRoomPopupDlg->IsShow() )
				ShowChildDialog( m_pChatRoomPopupDlg, false );
		}
	}

	// Show상태였을때 락인풋 풀릴 경우 있을거다.(거래 종료 같은 상황) 강제로 다시 락 시킨다.
	if( !CDnLocalPlayerActor::IsLockInput() )
		CDnLocalPlayerActor::LockInput( true );
}

void CDnChatRoomDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( nID == MESSAGEBOX_EXITROOM )
		{
			if( IsCmdControl( "ID_YES" ) )
			{
				SendLeaveChatRoom();
			}
		}
	}
}

bool CDnChatRoomDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			if( wParam == VK_TAB )
			{
				if( GetChatMode() == CHAT_PRIVATE )
				{
					ChangePrivateName();
				}
				RequestFocus(m_pIMEEditBox);
				return true;
			}
		}
		break;
	}

	if( !drag::IsValid() )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		fMouseX = MousePoint.x / GetScreenWidth();
		fMouseY = MousePoint.y / GetScreenHeight();

		switch( uMsg )
		{
		case WM_RBUTTONDOWN:
			{
				std::wstring wszName;

				bool bIsInsideMasterSelect = false;
				SUICoord uiCoordsBase;
				float fMouseXinDlg, fMouseYinDlg;
				m_pSelectMaster->GetUICoord(uiCoordsBase);
				PointToFloat( MousePoint, fMouseXinDlg, fMouseYinDlg );
				if( uiCoordsBase.IsInside( fMouseXinDlg, fMouseYinDlg ) ) {
					bIsInsideMasterSelect = true;
					wszName = m_pStaticMasterName->GetText();
				}
				if( bIsInsideMasterSelect ) {
					m_pSelectMaster->Show( true );
					SListBoxItem *pSelectedItem = m_pListBoxExPlayer->GetSelectedItem();
					if( pSelectedItem ) {
						pSelectedItem->bSelected = false;
						m_pListBoxExPlayer->DeselectItem();
					}
				}
				else {
					m_pSelectMaster->Show( false );
				}

				bool bIsInsideListBoxItem = false;
				for( int i = m_pListBoxExPlayer->GetScrollBar()->GetTrackPos(); i < m_pListBoxExPlayer->GetSize(); ++i ) {
					CDnChatRoomMemberListDlg *pDlg = m_pListBoxExPlayer->GetItem<CDnChatRoomMemberListDlg>(i);
					if( !pDlg ) continue;
					SUICoord uiCoord;
					pDlg->GetDlgCoord( uiCoord );
					if( uiCoord.IsInside( fMouseX, fMouseY ) ) {
						bIsInsideListBoxItem = true;
						wszName = pDlg->GetName();
						break;
					}
				}

				if( GetTradeTask().GetTradePrivateMarket().IsRequesting() || GetTradeTask().GetTradePrivateMarket().IsAccepting() )
					break;
				if( GetTradeTask().GetTradePrivateMarket().GetPrivateMarketDlg()->IsShow() )
					break;
				if( m_pChatRoomConfigDlg->IsShow() )
					break;

				if( bIsInsideMasterSelect || bIsInsideListBoxItem ) {
					ShowChildDialog( m_pChatRoomPopupDlg, false );
					m_pChatRoomPopupDlg->SetPosition( fMouseX, fMouseY );
					static float fXRGap(4.0f/DEFAULT_UI_SCREEN_WIDTH), fYBGap(4.0f/DEFAULT_UI_SCREEN_HEIGHT);
					SUICoord sDlgCoord;
					m_pChatRoomPopupDlg->GetDlgCoord( sDlgCoord );
					if( (sDlgCoord.Right()+fXRGap) > GetScreenWidthRatio() )
						sDlgCoord.fX -= (sDlgCoord.Right()+fXRGap - GetScreenWidthRatio());
					m_pChatRoomPopupDlg->SetDlgCoord( sDlgCoord );
					m_pChatRoomPopupDlg->SetInfo( m_bMasterIsMe, wszName.c_str() );
					ShowChildDialog( m_pChatRoomPopupDlg, true );
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				if( IsMouseInDlg() ) {
					if( m_pChatRoomPopupDlg->IsShow() ) {
						SUICoord uiCoord;
						m_pChatRoomPopupDlg->GetDlgCoord( uiCoord );
						if( !uiCoord.IsInside( fMouseX, fMouseY ) ) {
							ShowChildDialog( m_pChatRoomPopupDlg, false );
							return true;
						}
					}
				}

				SUICoord uiCoordsBase;
				float fMouseXinDlg, fMouseYinDlg;
				m_pSelectMaster->GetUICoord(uiCoordsBase);
				PointToFloat( MousePoint, fMouseXinDlg, fMouseYinDlg );
				if( uiCoordsBase.IsInside( fMouseXinDlg, fMouseYinDlg ) ) {
					m_pSelectMaster->Show( true );
					SListBoxItem *pSelectedItem = m_pListBoxExPlayer->GetSelectedItem();
					if( pSelectedItem ) {
						pSelectedItem->bSelected = false;
						m_pListBoxExPlayer->DeselectItem();
					}
				}
				else {
					m_pSelectMaster->Show( false );
				}
			}
			break;
		}
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			if( wParam == VK_RETURN )
			{
				if( !m_pIMEEditBox->IsFocus() )
					RequestFocus( m_pIMEEditBox );
			}
			else if( wParam == VK_TAB )
			{
				if( GetChatMode() == CHAT_PRIVATE )
				{
					ChangePrivateName();
				}

				RequestFocus(m_pIMEEditBox);
				return true;
			}
		}
		break;
	}

	return bRet;
}

void CDnChatRoomDlg::Render( float fElapsedTime )
{
	if( !IsAllowRender() ) return;

	// #41573 인벤 다이얼로그를 닫은 직후 채팅방 개설해 인벤토리 열면,
	// 인벤의 부모인 메인메뉴 다이얼로그의 텍스처 최적화루틴이 동작하면서 인벤 다이얼로그의 텍스처까지 지우게 된다.
	// 그래서 강제로 이렇게 다시 복구해서 렌더링하게 한다.
	if( m_pTempChildDialog[0] && m_pTempChildDialog[0]->IsShow() )
		m_pTempChildDialog[0]->LoadDialogTexture();

	CheckCommandMode();

	std::list<CEtUIDialog*>::iterator iterTemp[NUM_TEMP_CHILD_RENDER_PASS];
	int nIndexTemp[NUM_TEMP_CHILD_RENDER_PASS];
	
	for(int n=0; n<NUM_TEMP_CHILD_RENDER_PASS; n++)
		nIndexTemp[n] = 0;

	if( m_bLinked )
	{
		// 강제로 인벤토리를 자식윈도우로 연결해뒀기때문에, MainMenu쪽에서 기본적으로 찍히고, 여기서 또 렌더링되니,
		// 이걸 막기 위해서, 임시로 링크걸어둔 자식 다이얼로그는 임시로 NULL 처리한다.
		// 항상 고정된 순서라면 그냥 특정 인덱스에 넣을텐데,
		// 원래 자식 다이얼로그는 Show될때마다 리스트 순서가 바뀌는게 기본이라 아래처럼 처리했다.
		std::list<CEtUIDialog*>::iterator iter = m_listChildDialog.begin();
		int nIndex = 0;
		if( iter != m_listChildDialog.end() )
		{
			for( ; iter != m_listChildDialog.end(); ++iter )
			{
				for( int j = 0; j < NUM_TEMP_CHILD_RENDER_PASS; ++j )
				{
					if( *iter == m_pRenderPassChildDlg[j] )
					{
						iterTemp[nIndex] = iter;
						nIndexTemp[nIndex] = j;
						++nIndex;
						break;
					}
				}
			}
		}

		for( int i = 0; i < NUM_TEMP_CHILD_RENDER_PASS; ++i )
		{
			*(iterTemp[i]) = NULL;
		}
	}

	CEtUIDialog::Render( fElapsedTime );

	if( m_bLinked )
	{
		for( int i = 0; i < NUM_TEMP_CHILD_RENDER_PASS; ++i )
		{
			*(iterTemp[i]) = m_pRenderPassChildDlg[nIndexTemp[i]];
		}
	}
}

void CDnChatRoomDlg::SetRoomInfo( int nRoomID, UINT nSessionID, LPCWSTR pwszRoomName, bool bMasterIsMe )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;

	CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( !pPlayer ) return;

	m_nRoomID = nRoomID;
	m_nMasterSessionID = nSessionID;

	m_pStaticTitle->SetText( pwszRoomName );
	m_pStaticMasterLevel->SetIntToText( hActor->GetLevel() );
	m_pStaticMasterJob->SetText( pPlayer->GetJobName() );
	m_pStaticMasterName->SetText( hActor->GetName() );

	GetControl<CEtUIButton>("ID_BUTTON_OPTION")->Show( bMasterIsMe );

	m_bMasterIsMe = bMasterIsMe;
}

int CDnChatRoomDlg::GetRoomID()
{
	if( !IsShow() ) return 0;
	return m_nRoomID;
}

bool CDnChatRoomDlg::IsMyChatRoomMember( UINT nSessionID )
{
	if( m_nMasterSessionID == nSessionID ) return true;
	if( m_vecSessionID.empty() ) return false;
	for( int i = 0; i < (int)m_vecSessionID.size(); ++i )
	{
		if( m_vecSessionID[i] == nSessionID )
			return true;
	}
	return false;
}

void CDnChatRoomDlg::AddUser( UINT nSessionID, bool bNotify )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
	if( !hActor ) return;

	AddUser( hActor, bNotify );
}

void CDnChatRoomDlg::AddUser( DnActorHandle hActor, bool bNotify )
{
	if( !hActor ) return;

	CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( !pPlayer ) return;

	int nSessionID = hActor->GetUniqueID();
	int nIndex = -1;
	for( int i = 0; i < (int)m_vecSessionID.size(); ++i )
	{
		if( m_vecSessionID[i] == nSessionID )
		{
			nIndex = i;
			break;
		}
	}
	if( nIndex != -1 )
	{
		// 왜 추가한 유저를 또 추가하려고 하는거지? 뭐가 꼬인건가.
		return;
	}

	CDnChatRoomMemberListDlg *pItemDlg = m_pListBoxExPlayer->AddItem<CDnChatRoomMemberListDlg>();
	pItemDlg->SetInfo( pPlayer->GetName(), pPlayer->GetJobName(), pPlayer->GetLevel() );

	if( bNotify )
	{
		WCHAR wszTemp[256];
		swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8131 ), pPlayer->GetName() );
		AddChat( L"", wszTemp, CHAT_SYSTEM );
	}

	m_vecSessionID.push_back( nSessionID );

	if( m_pChatRoomPopupDlg && m_pChatRoomPopupDlg->IsShow() )
		ShowChildDialog( m_pChatRoomPopupDlg, false );
}

void CDnChatRoomDlg::DelUser( UINT nSessionID, BYTE cLeaveReason )
{
	int nIndex = -1;
	for( int i = 0; i < (int)m_vecSessionID.size(); ++i )
	{
		if( m_vecSessionID[i] == nSessionID )
		{
			nIndex = i;
			break;
		}
	}

	if( nIndex != -1 )
	{
		m_pListBoxExPlayer->RemoveItem( nIndex );
		m_vecSessionID.erase( m_vecSessionID.begin() + nIndex );

		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( nSessionID );
		if( hActor )
		{
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
			if( pPlayer )
			{
				int nMsgIndex = 0;
				switch( cLeaveReason )
				{
				case CHATROOMLEAVE_LEAVE:			nMsgIndex = 8132; break;
				case CHATROOMLEAVE_KICKED:			nMsgIndex = 121061; break;
				case CHATROOMLEAVE_FOREVERKICKED:	nMsgIndex = 121062; break;
				}
				if( nMsgIndex )
				{
					WCHAR wszTemp[256];
					swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex ), pPlayer->GetName() );
					AddChat( L"", wszTemp, CHAT_SYSTEM );
				}
			}
		}

		if( m_pChatRoomPopupDlg && m_pChatRoomPopupDlg->IsShow() )
			ShowChildDialog( m_pChatRoomPopupDlg, false );
	}
}

void CDnChatRoomDlg::ResizeIMECCtl( LPCWSTR wszHeader )
{
	DWORD dwTextColor(chatcolor::NORMAL);

	switch( GetChatMode() )
	{
	case CHAT_NORMAL:	dwTextColor = chatcolor::NORMAL;	break;
	case CHAT_PRIVATE:	dwTextColor = chatcolor::PRIVATE;	break;
	case CHAT_PARTY:	dwTextColor = chatcolor::PARTY;		break;
	case CHAT_GUILD:	dwTextColor = chatcolor::GUILD;		break;
	case CHAT_CHANNEL:	dwTextColor = chatcolor::CHANNEL;	break;
	case CHAT_WORLD:	dwTextColor = chatcolor::WORLD;		break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:	dwTextColor = chatcolor::PRIVATECHANNEL;	break;
#endif // PRE_PRIVATECHAT_CHANNEL
	default:
		ASSERT( 0&&"CDnChatRoomDlg::ResizeIMECCtl" );
		break;
	}

	m_pIMEEditBox->SetTextColor( dwTextColor );
	m_pIMEEditBox->SetCompTextColor( dwTextColor );
	m_pStaticMode->SetTextColor( dwTextColor );
	m_pStaticMode->SetText( wszHeader );

	SUICoord sTextCoord;
	CalcTextRect( wszHeader, m_pStaticMode->GetElement(0), sTextCoord );
	sTextCoord.fWidth += 0.001f;

	SUICoord sCtlCoord;
	m_pStaticMode->GetUICoord( sCtlCoord );
	float fTemp = sTextCoord.fWidth - sCtlCoord.fWidth;
	sCtlCoord.fWidth = sTextCoord.fWidth;
	m_pStaticMode->SetUICoord( sCtlCoord );

	m_pIMEEditBox->GetUICoord( sCtlCoord );
	sCtlCoord.fX += fTemp;
	sCtlCoord.fWidth -= fTemp;
	m_pIMEEditBox->SetUICoord( sCtlCoord );
}

void CDnChatRoomDlg::SetChatPrevMode()
{
	int nCurChatMode = GetChatMode();
	while(true)
	{
		nCurChatMode >>= 1;

		if( nCurChatMode == 0 )
			nCurChatMode = CHAT_WORLD;

		if( SetChatMode( nCurChatMode ) )
		{
			RequestFocus(m_pIMEEditBox);
			break;
		}
	}
}

void CDnChatRoomDlg::SetChatNextMode()
{
	int nCurChatMode = GetChatMode();
	while(true)
	{
		nCurChatMode <<= 1;

		if( nCurChatMode >= CHAT_MODE_MAX )
			nCurChatMode = CHAT_NORMAL;

		if( SetChatMode( nCurChatMode ) )
		{
			RequestFocus(m_pIMEEditBox);
			break;
		}
	}
}

void CDnChatRoomDlg::SetChatModeText()
{
	if( !m_pStaticMode )
		return;

	wchar_t wszMsg[256]={0};

	switch( m_nChatMode )
	{
	case CHAT_NORMAL:
		swprintf_s( wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 501 ) );
		break;
	case CHAT_PARTY:
		swprintf_s( wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 632 ) );
		break;
	case CHAT_GUILD:
		swprintf_s( wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5007 ) );
		break;
	case CHAT_PRIVATE:
		{
			swprintf_s( wszMsg, 256, L"[%s][%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 648), m_strPrivateUserName.c_str() );

			if( !m_strPrivateUserName.empty() )
			{
				AddPrivateName( m_strPrivateUserName );
			}
		}
		break;
	case CHAT_CHANNEL:
		swprintf_s( wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 605 ) );
		break;
	case CHAT_WORLD:
		swprintf_s( wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 606 ));
		break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHAT_PRIVATE_CHANNEL:
		swprintf_s( wszMsg, 256, L"[%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1202 ));
		break;
#endif // PRE_PRIVATECHAT_CHANNEL
	}
	ResizeIMECCtl(wszMsg);
}

bool CDnChatRoomDlg::SetChatMode( int nChatMode )
{
	if( nChatMode == CHAT_PARTY )
	{
		return false;
	}
	else if( nChatMode == CHAT_GUILD )
	{
		// 길드에 가입안되어있다면,
		if( CDnActor::s_hLocalActor ) {
			CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if( !pPlayer->IsJoinGuild() ) {
				return false;
			}
		}
	}
	else if( nChatMode == CHAT_PRIVATE )
	{
		// 귓속말 대상이 없으면
		if( GetPrivateName() == L"" )
			return false;
	}
	else if( nChatMode == CHAT_CHANNEL )
	{
		// 클베에서 우선 기능 비활성화.
		return false;
	}
	else if( nChatMode == CHAT_WORLD )
	{
		return false;
	}

	m_nChatMode = nChatMode;
	SetChatModeText();
	return true;
}

bool CDnChatRoomDlg::AddEditBoxString()
{
	std::wstring strChat = m_pIMEEditBox->GetText();
	strChat.reserve( CHATLENMAX );

	std::wstring chatTemp;
	chatTemp = boost::algorithm::trim_copy(strChat);
	if (chatTemp.empty())
		return true;

	if( CheckCommandMode( strChat.c_str() ) )
		return false;

	// 명령어 처리하기전에 기억해놔야 히스토리 이용해서 명령어 재입력을 할 수 있다.
	AddChatHistotry( strChat.c_str() );

	// 여기서 채팅 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_CHAT, true ) )
		return true;

	// 제스처에도 명령어 있다.
	// 제스처는 다른 명령어들과 달리 포커스 잃게 하지 않는다. 그냥 채팅중이므로.
	//GetGestureTask().UseGestureByChat( strChat.c_str() );

	// 채팅방 열고있는 중엔 치트 쓸 수 없다. 캐릭터 고정.
	if( strChat[0] == L'/' )
	{
		m_pIMEEditBox->ClearText();
		return false;
	}

	SendChat( m_nChatMode, strChat, m_strPrivateUserName );

	m_pIMEEditBox->ClearText();

	return false;
}

void CDnChatRoomDlg::ChangePrivateName()
{
	if( !m_listPrivateName.empty() )
	{
		m_strPrivateUserName = m_listPrivateName.front();
		m_listPrivateName.pop_front();
		m_listPrivateName.push_back( m_strPrivateUserName );

		wchar_t wszMsg[256]={0};
		swprintf_s( wszMsg, 256, L"[%s][%s]:", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 648), m_strPrivateUserName.c_str() );
		ResizeIMECCtl(wszMsg);
	}
}

void CDnChatRoomDlg::AddPrivateName( const std::wstring strPrivateName )
{
	PRIVATENAME_LIST_ITER iter = m_listPrivateName.begin();
	for( ; iter != m_listPrivateName.end(); )
	{
		if( (*iter) == strPrivateName )
		{
			iter = m_listPrivateName.erase(iter);
		}
		else ++iter;
	}

	m_listPrivateName.push_back( strPrivateName );
}

void CDnChatRoomDlg::SetPrivateName( const std::wstring &strPrivateName )
{
	m_strPrivateUserName = strPrivateName;

	SetChatMode( CHAT_PRIVATE );
	m_pIMEEditBox->ClearText();
	RequestFocus( m_pIMEEditBox );
}

bool CDnChatRoomDlg::GetLastPrivateName( std::wstring &strLastPrivateName )
{
	if( !m_strLastPrivateUserName.empty() )
	{
		strLastPrivateName = m_strLastPrivateUserName;
		return true;
	}

	return false;
}

void CDnChatRoomDlg::SetLastPrivateName( LPCWSTR wszLastPrivateName )
{
	ASSERT( wszLastPrivateName&&"CDnChatTabDlg::SetLastPrivateName" );
	m_strLastPrivateUserName = wszLastPrivateName;
}

void CDnChatRoomDlg::AddChatHistotry( LPCWSTR wszChat )
{
	if( m_listChatHistory.size() > 20 )
	{
		m_listChatHistory.pop_back();
		m_listChatHistory.push_front( wszChat );
	}
	else
	{
		m_listChatHistory.push_front( wszChat );
	}
}

void CDnChatRoomDlg::SetPrevChatHistory()
{
	if( !m_listChatHistory.empty() )
	{
		std::wstring strChat = m_listChatHistory.back();
		m_listChatHistory.pop_back();
		m_listChatHistory.push_front( strChat );

		m_pIMEEditBox->SetText( strChat.c_str() );
	}
}

void CDnChatRoomDlg::SetNextChatHistory()
{
	if( !m_listChatHistory.empty() )
	{
		std::wstring strChat = m_listChatHistory.front();
		m_listChatHistory.pop_front();
		m_listChatHistory.push_back( strChat );

		m_pIMEEditBox->SetText( strChat.c_str() );
	}

	return;
}

void CDnChatRoomDlg::CheckCommandMode()
{
	std::wstring strCmd;
	COMMANDMODE_MAP_ITER iter;
	m_bPrivateModeChecker = false;

	strCmd = m_pIMEEditBox->GetText();

	std::wstring::size_type idx = strCmd.find_first_of(L" ");
	if (idx != std::wstring::npos)
	{
		std::wstring checkStr = strCmd.substr(0, idx + 1);

		iter = m_mapCommandMode.find(checkStr);
		if( iter != m_mapCommandMode.end() )
		{
			int chatType = iter->second;
			if( chatType & CHAT_PRIVATE_REPLY )
			{
				std::wstring strLastName;
				if( GetLastPrivateName( strLastName ) )
				{
					SetPrivateName( strLastName );
				}
				return;
			}

			if( chatType == CHAT_PRIVATE )
			{
				m_bPrivateModeChecker = true;
			}
			else
			{
				SetChatMode( chatType );
				m_pIMEEditBox->ClearText();
				RequestFocus(m_pIMEEditBox);
			}
		}

		if (m_bPrivateModeChecker)
		{
			// Note : 상대이름이 입력될때 까지 모드를 바꾸지 않는다.
			//
			std::wstring::size_type begIdx, endIdx;

			begIdx = strCmd.find_first_of(L" ");
			begIdx = (begIdx != std::wstring::npos) ? ++begIdx : std::wstring::npos;
			endIdx = strCmd.find_first_of(L" ", begIdx);

			if( endIdx != std::wstring::npos )
			{
				std::wstring strPrivateUserName = strCmd.substr(begIdx, endIdx-begIdx);
				EtInterface::GetNameLinkMgr().TranslateText(m_strPrivateUserName, strPrivateUserName.c_str());
				if ((int)m_strPrivateUserName.size() > CGlobalInfo::GetInstance().m_nClientCharNameLenMax)
					m_strPrivateUserName.resize(CGlobalInfo::GetInstance().m_nClientCharNameLenMax);

				SetChatMode( CHAT_PRIVATE );
				m_pIMEEditBox->ClearText();
				RequestFocus(m_pIMEEditBox);

				m_bPrivateModeChecker = false;
			}
		}
	}
}

bool CDnChatRoomDlg::CheckCommandMode( LPCWSTR wszMsg )
{
	std::wstring strCmd(wszMsg);
	if( strCmd.size() < 2 ) return false;
	if( (strCmd.size() > 2) && (strCmd[2] != L' ') ) return false;

	strCmd = strCmd.substr(0,2);
	strCmd += L" ";
	COMMANDMODE_MAP_ITER iter = m_mapCommandMode.find( strCmd );

	if( iter != m_mapCommandMode.end() )
	{
		if( iter->second&CHAT_PRIVATE_REPLY )
		{
			std::wstring strLastName;
			if( GetLastPrivateName( strLastName ) )
			{
				SetPrivateName( strLastName );
			}
			return true;
		}

		if( iter->second == CHAT_PRIVATE )
		{
			strCmd = wszMsg;
			std::wstring::size_type begIdx = strCmd.find_first_not_of(L" ", 2);
			if( begIdx != std::wstring::npos )
			{
				std::wstring strPrivateUserName = strCmd.substr(begIdx);
				EtInterface::GetNameLinkMgr().TranslateText(m_strPrivateUserName, strPrivateUserName.c_str());
				if( (int)m_strPrivateUserName.size() > CGlobalInfo::GetInstance().m_nClientCharNameLenMax )
				{
					m_strPrivateUserName.resize( CGlobalInfo::GetInstance().m_nClientCharNameLenMax );
				}
			}
		}

		SetChatMode( iter->second );
		m_pIMEEditBox->ClearText();
		RequestFocus(m_pIMEEditBox);

		return true;
	}

	return false;
}

void CDnChatRoomDlg::SendChat( int chatMode, std::wstring& chatMsg, const std::wstring& userName )
{
	DN_INTERFACE::UTIL::CheckChat( chatMsg, '*' );

	switch( chatMode )
	{
	case CHAT_NORMAL:
		{
			SendChatRoomMsg( chatMsg.c_str() );
		}
		break;
	case CHAT_PRIVATE:
		{
			if( CheckPapering() )
			{
				wchar_t wszMsg[256]={0};
				int nTime = (int)m_fPaperingRemainTime;
				swprintf_s( wszMsg, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 672 ), nTime );
				m_pChatTextBox->AddText( wszMsg, textcolor::RED );
				return;
			}

			if( userName.empty() )
				break;

			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( pInvenDlg )
			{
				// wszFromCharName과 거래중인 상대방 이름이 같을 경우에만 내 말풍선에 등록.
				if( pInvenDlg->IsYourName( userName.c_str() ) )
					pInvenDlg->AddPrivateMarketChatMe( chatMsg.c_str() );
			}

			wchar_t wszMsg[1024]={0};
			swprintf_s( wszMsg, 1024, L"[%s]<<< : %s", userName.c_str(), chatMsg.c_str() );
			AddChat( userName.c_str(), wszMsg, CHAT_PRIVATE );

			SendChatPrivateMsg( userName.c_str(), chatMsg.c_str() );
		}
		break;
	case CHAT_GUILD:
		{
			SendChatMsg( CHATTYPE_GUILD, chatMsg.c_str() );
		}
		break;
	}
}

void CDnChatRoomDlg::AddChatMessage( eChatType eType, LPCWSTR wszFromCharName, LPCWSTR wszChatMsg )
{
	wchar_t wszMsg[1024]={0};

	std::wstring fromCharNameString;
	if (wcslen( wszFromCharName ) != 0)
	{
		CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
		if (pMgr)
			pMgr->MakeNameLinkString_UserName(fromCharNameString, wszFromCharName);
	}

	int nType = CHAT_NORMAL;
	DWORD dwBgColor = 0;
	CDnInvenTabDlg *pInvenDlg = NULL;
	switch( eType )
	{
	case CHATTYPE_NORMAL:
	case CHATTYPE_GM:
		// 채팅방 들어가면 일반말은 안들린다.
		return;
	case CHATTYPE_PRIVATE:
	case CHATTYPE_PRIVATE_GM:
		nType = CHAT_PRIVATE;
		pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
		{
			// wszFromCharName과 거래중인 상대방 이름이 같을 경우에만 내 말풍선에 등록.
			if( pInvenDlg->IsYourName( wszFromCharName ) )
				pInvenDlg->AddPrivateMarketChatYou( wszChatMsg );
		}

		// IsPrivate옵션 여부와 상관없이 이건 저장해둔다.
		// 이렇게 해야 Shift+r로 응답이 가능해진다.
		SetLastPrivateName( wszFromCharName );

		swprintf_s( wszMsg, 1024, L"[%s]>>> : %s", fromCharNameString.c_str(), wszChatMsg );
		AddPrivateName( wszFromCharName );

		if( eType == CHATTYPE_PRIVATE_GM ) dwBgColor = D3DCOLOR_ARGB(0x80,0x22,0x8B,0x22);
		break;
	case CHATTYPE_CHATROOM:
		// 채팅방까지 들어왔는데 차단검사 해야할까..
		//if (CDnIsolateTask::IsActive() && GetIsolateTask().IsBlackList(fromCharNameString.c_str()))
		//	return;
		nType = CHAT_NORMAL;
		if( fromCharNameString.empty() )
			swprintf_s( wszMsg, 1024, L"%s", wszChatMsg );
		else 
			swprintf_s( wszMsg, 1024, L"[%s] : %s", fromCharNameString.c_str(), wszChatMsg );
		break;
	case CHATTYPE_GUILD:
		nType = CHAT_GUILD;		
		swprintf_s( wszMsg, 1024, L"[%s][%s] : %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5007), fromCharNameString.c_str(), wszChatMsg );
		break;
	case CHATTYPE_SYSTEM:
		nType = CHAT_SYSTEM;
		swprintf_s( wszMsg, 1024, L"%s", wszChatMsg );
		break;
	case CHATTYPE_RAIDNOTICE:
		{
			nType = CHAT_RAIDNOTICE;
			swprintf_s( wszMsg, 1024, L"[%s]: %s", fromCharNameString.c_str(), wszChatMsg );

			int soundIndex = CDnPartyTask::GetInstance().GetPartySoundIndex(CDnPartyTask::REQUESTREADY);
			CEtSoundEngine::GetInstance().PlaySound("2D", soundIndex);
		}
		break;
	}

	AddChat( wszFromCharName, wszMsg, nType, dwBgColor );
}

void CDnChatRoomDlg::AppendChatEditBox(LPCWSTR wszString, bool bFocus)
{
	if (m_pIMEEditBox->GetFullTextLength() + lstrlenW(wszString) >= CHATLENMAX)
		return;

	if (bFocus == false)
		focus::ReleaseControl();

	m_pIMEEditBox->AddText(wszString, bFocus);
}

bool CDnChatRoomDlg::OnParseTextItemInfo(const std::wstring& argString)
{
	CDnNameLinkMng* pMgr = GetInterface().GetNameLinkMng();
	if (pMgr)
	{
		CDnItem* pLinkItem = pMgr->MakeItem(argString);
		if (pLinkItem)
		{
			CDnTooltipDlg* pToolTipDlg = GetInterface().GetNameLinkToolTipDlg();
			if (pToolTipDlg)
			{
				ITEM_SLOT_TYPE slotType = ST_ITEM_NONE;
				if (pLinkItem->GetType() == MIInventoryItem::Item)
					slotType = ST_INVENTORY;
				else if (pLinkItem->GetType() == MIInventoryItem::Skill)
					slotType = ST_SKILL;
				pToolTipDlg->ShowTooltip(pLinkItem, pLinkItem->GetType(), slotType, GetScreenWidthRatio() * 0.5f, GetScreenHeightRatio() * 0.83f, true);
			}
		}
	}

	return true;
}

bool CDnChatRoomDlg::OnParseTextUserName(const std::wstring& name)
{
	SetPrivateName(name);
	return true;
}

void CDnChatRoomDlg::AddChat( LPCWSTR wszName, LPCWSTR szMessage, int nType, DWORD dwBackgroundColor )
{
	DWORD dwTextColor(textcolor::WHITE);

	switch( nType )
	{
	case CHAT_NORMAL:	dwTextColor = chatcolor::NORMAL;	break;
	case CHAT_PARTY:	dwTextColor = chatcolor::PARTY;		break;
	case CHAT_GUILD:	dwTextColor = chatcolor::GUILD;		break;
	case CHAT_PRIVATE:	dwTextColor = chatcolor::PRIVATE;	break;
	case CHAT_SYSTEM:	dwTextColor = textcolor::GOLD;		break;
	case CHAT_RAIDNOTICE: dwTextColor = chatcolor::RAIDNOTICE;		break;
	}

	if( wszName != NULL && wszName[0] != '\0' )
	{
		SLineData sLineData;
		sLineData.m_strData = wszName;
		m_pChatTextBox->SetLineData( sLineData );
	}

	bool bAutoScroll = false;

	if( m_pChatTextBox->IsLastPage() || m_pChatTextBox->IsEmpty() )
		bAutoScroll = true;

	// 스크롤이 올려져있더라도 자신이 말한 내용이라면 자동스크롤을 적용시킨다.
	if( !bAutoScroll && wszName ) {
		bool bMyChat = false;
		if( CDnActor::s_hLocalActor ) {
			if( __wcsicmp_l( wszName, CDnActor::s_hLocalActor->GetName() ) == 0 )
				bMyChat = true;
		}
		if( bMyChat )
			bAutoScroll = true;
	}
	m_pChatTextBox->SetAutoScroll( bAutoScroll );

#ifdef PRE_ADD_MULTILANGUAGE
	std::wstring wstrMessage;
	CDnChatDlg::ChangeItemNameLinkByLanguage( szMessage, wstrMessage );
	m_pChatTextBox->AddText( wstrMessage.c_str(), dwTextColor, UITEXT_NONE, dwBackgroundColor );
#else // PRE_ADD_MULTILANGUAGE
	m_pChatTextBox->AddText( szMessage, dwTextColor, UITEXT_NONE, dwBackgroundColor );
#endif // PRE_ADD_MULTILANGUAGE
}

bool CDnChatRoomDlg::CheckPapering()
{
	// 채팅도배로 금지시간 중이라면,
	if( m_fPaperingRemainTime > 0.0f )
		return true;

	// 추가했을때 제한 횟수에 걸리면,
	if( (int)m_listRecentChatTime.size() + 1 >= CHAT_PAPERING_CHECKCOUNT )
		m_fPaperingRemainTime = (float)CHAT_PAPERING_RESTRICTIONTIME;

	m_listRecentChatTime.push_back((float)CHAT_PAPERING_CHECKTIME);
	return false;
}

bool CDnChatRoomDlg::IsEmptyRoom() const
{
	return (m_pListBoxExPlayer->GetSize() <= 0);
}