#include "StdAfx.h"
#include "DnFriendDlg.h"
#include "DnFriendInfoDlg.h"
#include "DnFriendAddDlg.h"
#include "DnInterfaceDlgID.h"
#include "DnPartyTask.h"
#include "DnInterface.h"
#include "DnFriendGroupDlg.h"
#include "DnMainMenuDlg.h"
#include "GameOption.h"
#include "TaskManager.h"
#include "DnVillageTask.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnFriendDlg::CDnFriendDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pFriendGroupDlg(NULL)
	, m_pFriendInfoDlg(NULL)
	, m_pFriendAddDlg(NULL)
	, m_pButtonInvite(NULL)
	, m_pButtonInfo(NULL)
	, m_pButtonSendChat(NULL)
	, m_pButtonSendMail(NULL)
	, m_pButtonParty(NULL)
	, m_pButtonGuild(NULL)
	, m_pButtonAdd(NULL)
	, m_pButtonDel(NULL)
	, m_pCheckBoxHide(NULL)
	, m_pStaticCount(NULL)
	, m_pTreeFriend(NULL)
	, m_pButtonGroup(NULL)
	, m_ListUpdateTime(0.f)
{
}

CDnFriendDlg::~CDnFriendDlg(void)
{
}

void CDnFriendDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "FriendDlg.ui" ).c_str(), bShow );

	if( CDnFriendTask::IsActive() )
	{
		GetFriendTask().SetFriendDialog( this );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnFriendDlg::Initialize, 친구 다이얼로그가 만들어 지기 전에 친구 테스크가 생성되어야 합니다." );
	}
}

void CDnFriendDlg::InitialUpdate()
{
	m_pButtonGroup = GetControl<CEtUIButton>("ID_BUTTON_GROUP");
	m_pButtonInfo = GetControl<CEtUIButton>("ID_BUTTON_INFO");
	m_pButtonSendChat = GetControl<CEtUIButton>("ID_BUTTON_SEND_CHAT");
	m_pButtonInvite = GetControl<CEtUIButton>("ID_BUTTON_INVITE");        // Rotha - 친구 초대 버튼 설정

	//m_pButtonSendMail = GetControl<CEtUIButton>("ID_BUTTON_SEND_MAIL");
	//m_pButtonParty = GetControl<CEtUIButton>("ID_BUTTON_PARTY");
	//m_pButtonGuild = GetControl<CEtUIButton>("ID_BUTTON_GUILD");
	m_pButtonAdd = GetControl<CEtUIButton>("ID_BUTTON_ADD");
	m_pButtonDel = GetControl<CEtUIButton>("ID_BUTTON_DEL");
	EnableButton( false );

	m_pCheckBoxHide = GetControl<CEtUICheckBox>("ID_CHECKBOX_HIDE");
	m_pStaticCount = GetControl<CEtUIStatic>("ID_STATIC_COUNT");
	m_pTreeFriend = GetControl<CEtUITreeCtl>("ID_TREE_FRIEND");

	m_pFriendInfoDlg = new CDnFriendInfoDlg( UI_TYPE_CHILD, this, FRIEND_INFO_DIALOG, this );
	m_pFriendInfoDlg->Initialize( false );
	m_FriendDlgGroup.AddDialog( FRIEND_INFO_DIALOG, m_pFriendInfoDlg );

	m_pFriendAddDlg = new CDnFriendAddDlg( UI_TYPE_CHILD, this, FRIEND_ADD_DIALOG, this );
	m_pFriendAddDlg->Initialize( false );
	m_FriendDlgGroup.AddDialog( FRIEND_ADD_DIALOG, m_pFriendAddDlg );

	m_pFriendGroupDlg = new CDnFriendGroupDlg( UI_TYPE_CHILD, this, FRIEND_GROUP_DIALOG, this );
	m_pFriendGroupDlg->Initialize( false );
	m_FriendDlgGroup.AddDialog( FRIEND_GROUP_DIALOG, m_pFriendGroupDlg );

	if (m_pCheckBoxHide)
	{
		m_pCheckBoxHide->SetChecked(CGameOption::GetInstance().m_bCommFriendHideOffline);
	}
	else
	{
		_ASSERT(0);
	}
}

void CDnFriendDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	m_pTreeFriend->ResetSelectedItem();

	if( bShow )
	{
		EnableButton( false );
		RefreshFriendList();
		//m_pTreeFriend->ExpandAll();
	}
	else
	{
		//m_pTreeFriend->ResetSelectedItem();
		m_FriendDlgGroup.CloseAllDialog();
	}
}

void CDnFriendDlg::Process( float fElapsedTime )
{
	if (m_bShow)
	{
		if (m_ListUpdateTime >= _FRIEND_LIST_REFRESH_TIME)
		{
			GetFriendTask().RequestFriendLocationList();
			m_ListUpdateTime = 0.f;
		}
		else
		{
			m_ListUpdateTime += fElapsedTime;
		}
	}
	
	CEtUIDialog::Process(fElapsedTime);
}

void CDnFriendDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_GROUP") )
		{
			m_FriendDlgGroup.ShowDialog(FRIEND_GROUP_DIALOG, true);
			return;
		}

		if( IsCmdControl("ID_BUTTON_ADD") )
		{
			m_FriendDlgGroup.ShowDialog(FRIEND_ADD_DIALOG, true);
			return;
		}

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_FRIEND);
		}
#endif

		CTreeItem *pItem = m_pTreeFriend->GetSelectedItem();
		if( !pItem ) return;

		if( IsCmdControl("ID_BUTTON_INFO" ) )
		{
			INT64 nFriendID = pItem->GetItemValueInt64();
			GetFriendTask().RequestFriendDetailInfo( nFriendID );
			m_pFriendInfoDlg->SetFriendID( nFriendID );
			return;
		}
		if( IsCmdControl("ID_BUTTON_INVITE"))
		{
			GetPartyTask().ReqInviteParty( pItem->GetText() ); 
			return;
		}

		if( IsCmdControl("ID_BUTTON_SEND_CHAT" ) )
		{
			GetInterface().OpenPrivateChatDialog( pItem->GetText() );
			return;
		}

		if( IsCmdControl("ID_BUTTON_SEND_MAIL") )
		{
			INT64 nFriendID = pItem->GetItemValueInt64();
			CDnFriendTask::SFriendInfo *pFriendInfo = GetFriendTask().GetFriendInfo( nFriendID );
			if( pFriendInfo )
			{
				CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
				if( pMainMenuDlg ) pMainMenuDlg->OpenMailDialog( true, pFriendInfo->wszFriendName.c_str(), _T("") );
			}
			else
			{
				// Note : 친구를 상태를 선택하세요.
				//
			}
			return;
		}
		
		if( IsCmdControl("ID_BUTTON_PARTY") )
		{
			GetPartyTask().ReqInviteParty( pItem->GetText() );
			return;
		}

		if( IsCmdControl("ID_BUTTON_GUILD") )
		{
			// Note : 길드에 가입되어 있다면 길드 초청 함수를 호출한다.
			//
			return;
		}

		if( IsCmdControl("ID_BUTTON_DEL") )
		{
			// Note : 친구삭제 함수를 호출한다.
			//		삭제하기 전에 확인을 하는 메세지 박스를 띄운다.
			//		삭제는 클라이언트 지우고 서버에 그냥 지우라고 패킷만 보낸다.
			//
			GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1961 ), MB_YESNO, MESSAGEBOX_FRIEND_DELETE, this);	// UISTRING : 정말로 삭제하시겠습니까?
			return;
		}
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION )
	{
		if( IsCmdControl("ID_TREE_FRIEND" ) )
		{
			CTreeItem *pItem = m_pTreeFriend->GetSelectedItem();
			if( pItem && pItem->GetItemValueInt64() > 0 )
			{
				EnableButton( true );

				// Note : 만약 상세 정보창이 열려 있다면 선택된 아이디로 정보 갱신을 요구한다.
				//
				if( m_FriendDlgGroup.IsShowDialog(FRIEND_INFO_DIALOG) )
				{
					INT64 nFriendID = pItem->GetItemValueInt64();
					GetFriendTask().RequestFriendDetailInfo( nFriendID );
				}
			}
			else
			{
				EnableButton( false );
				m_FriendDlgGroup.ShowDialog( FRIEND_INFO_DIALOG, false );
			}

			return;
		}
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION_END )
	{
		if( IsCmdControl("ID_TREE_FRIEND" ) )
		{
			CTreeItem *pItem = m_pTreeFriend->GetSelectedItem();
			if( !pItem || (pItem->GetItemValueInt64() == 0) )
			{
				EnableButton( false );
				m_FriendDlgGroup.ShowDialog(FRIEND_INFO_DIALOG, false);
			}

			return;
		}
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_HIDE" ) )
		{
			CGameOption::GetInstance().m_bCommFriendHideOffline = m_pCheckBoxHide->IsChecked();
			RefreshFriendList();
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnFriendDlg::EnableButton( bool bEnable )
{
	if ( m_pButtonInfo )		m_pButtonInfo->Enable( bEnable );

	bool bPvp = false;
	CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
	if( (pVillageTask && pVillageTask->GetVillageType() == CDnVillageTask::PvPVillage) ||
		GetInterface().IsPVP() )
		bPvp = true;
	if (m_pButtonInvite)
	{
		if (bPvp)
			m_pButtonInvite->Enable(false);
		else
			m_pButtonInvite->Enable(bEnable);   // Rotha - 파티UI 친구 초대기능 자동 Hide 기능 추가
	}

	if ( m_pButtonSendChat )	m_pButtonSendChat->Enable( bEnable );
	if ( m_pButtonSendMail )	m_pButtonSendMail->Enable( bEnable );
	if ( m_pButtonParty )		m_pButtonParty->Enable( bEnable );
	if ( m_pButtonGuild )		m_pButtonGuild->Enable( bEnable );
	if ( m_pButtonDel )			m_pButtonDel->Enable( bEnable );
}

void CDnFriendDlg::SetCount( int nCur, int nMax )
{
	wchar_t szTemp[256]={0};
	swprintf_s( szTemp, 256, L"%d/%d", nCur, nMax );
	m_pStaticCount->SetText( szTemp );
}

void CDnFriendDlg::RefreshGroupDialog()
{
	m_pFriendGroupDlg->RefreshFriendGroup();
}

void CDnFriendDlg::RefreshFriendGroup()
{
	m_pTreeFriend->DeleteAllItems();

	AddFriendGroup( 0, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ) );

	const CDnFriendTask::FRIENDGROUPINFO_VEC &friendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
	for( int i=0; i<(int)friendGroupInfo.size(); ++i )
	{
		AddFriendGroup( friendGroupInfo[i].nGroupDBID, friendGroupInfo[i].wszGroupName.c_str() );
	}
}

void CDnFriendDlg::RefreshFriendInfo( INT64 biCharDBID )
{
	m_pFriendInfoDlg->RefreshFriendInfo( biCharDBID );
	m_FriendDlgGroup.ShowDialog(FRIEND_INFO_DIALOG, true);
}

void CDnFriendDlg::MakeGroupTitle(wchar_t* title, int groupId, const wchar_t* wszGroupName)
{
	CDnFriendTask::SFriendGroupCountInfo info = GetFriendTask().GetGroupCountInfo(groupId);
	swprintf_s(title, _MAX_FRIEND_TITLE, _T("%s (%d/%d)"), wszGroupName, info.logonFriends, info.wholeFriends);
}

#ifdef PRE_FIX_FRIEND_SORT
bool CDnFriendDlg::SortByName(const std::pair<INT64, CDnFriendTask::SFriendInfo>& comp1, const std::pair<INT64, CDnFriendTask::SFriendInfo>& comp2)
{
	const CDnFriendTask::SFriendInfo& info1 = comp1.second;
	const CDnFriendTask::SFriendInfo& info2 = comp2.second;

	return (0 > wcscmp(info1.wszFriendName.c_str(), info2.wszFriendName.c_str()));
}

void CDnFriendDlg::SortList(std::vector<CDnFriendTask::SFriendInfo>& result, const CDnFriendTask::FRIENDINFO_MAP& friendInfoList)
{
	std::vector<std::pair<INT64, CDnFriendTask::SFriendInfo> > convertList(friendInfoList.begin(), friendInfoList.end());
	std::sort(convertList.begin(), convertList.end(), SortByName);

	result.clear();
	std::vector<std::pair<INT64,CDnFriendTask::SFriendInfo> >::iterator convIter = convertList.begin();
	for (; convIter != convertList.end(); ++convIter)
	{
		std::pair<INT64,CDnFriendTask::SFriendInfo>& data = (*convIter);
		result.push_back(data.second);
	}
}
#endif

void CDnFriendDlg::RefreshFriendList()
{
	m_pTreeFriend->DeleteAllItems();

	CDnFriendTask::FRIENDINFO_MAP *pFriendInfoList(NULL);

	CTreeItem *pItemGroup(NULL);
	CTreeItem *pItemFriend(NULL);

	pFriendInfoList = GetFriendTask().GetFriendInfoList( 0 );

	if( pFriendInfoList )
	{
#ifdef PRE_FIX_FRIEND_SORT
		std::vector<CDnFriendTask::SFriendInfo> infoList;
		SortList(infoList, *pFriendInfoList);

		wchar_t title[_MAX_FRIEND_TITLE];
		memset(title, 0, sizeof(wchar_t) * _MAX_FRIEND_TITLE);
		MakeGroupTitle(title, 0, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ));
		pItemGroup = m_pTreeFriend->AddItem( CTreeItem::typeOpen, title, textcolor::FONT_GREEN );

		std::vector<CDnFriendTask::SFriendInfo>::const_iterator iter = infoList.begin();
		for( ; iter != infoList.end(); ++iter )
		{
			const CDnFriendTask::SFriendInfo& info = (*iter);
			if (GetHideOffline() && IsFriendInGame(info.Location) == false)
				continue;

			pItemFriend = m_pTreeFriend->AddChildItem( pItemGroup, CTreeItem::typeOpen, info.wszFriendName.c_str(), textcolor::DARKGRAY );
			pItemFriend->SetItemValueInt64(info.biFriendCharDBID);
			SetStateIcon(pItemFriend, info.Location.cServerLocation);
		}
#else
		wchar_t title[_MAX_FRIEND_TITLE];
		memset(title, 0, sizeof(wchar_t) * _MAX_FRIEND_TITLE);
		MakeGroupTitle(title, 0, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ));
		pItemGroup = m_pTreeFriend->AddItem( CTreeItem::typeOpen, title, textcolor::FONT_GREEN );

		CDnFriendTask::FRIENDINFO_MAP_ITER iter = pFriendInfoList->begin();
		for( ; iter != pFriendInfoList->end(); ++iter )
		{
			const CDnFriendTask::SFriendInfo& info = iter->second;
			if (GetHideOffline() && IsFriendInGame(info.Location) == false)
				continue;

			pItemFriend = m_pTreeFriend->AddChildItem( pItemGroup, CTreeItem::typeOpen, iter->second.wszFriendName.c_str(), textcolor::DARKGRAY );
			pItemFriend->SetItemValueInt64( iter->first );
			SetStateIcon(pItemFriend, info.Location.cServerLocation);
		}
#endif
	}
	
	const CDnFriendTask::FRIENDGROUPINFO_VEC &friendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
	for( int i=0; i<(int)friendGroupInfo.size(); ++i )
	{
		wchar_t title[_MAX_FRIEND_TITLE];
		memset(title, 0, sizeof(wchar_t) * _MAX_FRIEND_TITLE);
		MakeGroupTitle(title, friendGroupInfo[i].nGroupDBID, friendGroupInfo[i].wszGroupName.c_str());
		pItemGroup = m_pTreeFriend->AddItem( CTreeItem::typeOpen, title, textcolor::FONT_GREEN );
		pItemGroup->SetItemValueFloat( (float)friendGroupInfo[i].nGroupDBID );

		pFriendInfoList = GetFriendTask().GetFriendInfoList( friendGroupInfo[i].nGroupDBID );
		if( pFriendInfoList )
		{
#ifdef PRE_FIX_FRIEND_SORT
			std::vector<CDnFriendTask::SFriendInfo> infoList;
			SortList(infoList, *pFriendInfoList);

			std::vector<CDnFriendTask::SFriendInfo>::const_iterator iter = infoList.begin();
			for (; iter != infoList.end(); ++iter)
			{
				const CDnFriendTask::SFriendInfo& info = (*iter);
				if (GetHideOffline() && IsFriendInGame(info.Location) == false)
					continue;

				pItemFriend = m_pTreeFriend->AddChildItem(pItemGroup, CTreeItem::typeOpen, info.wszFriendName.c_str(), textcolor::DARKGRAY);
				pItemFriend->SetItemValueInt64(info.biFriendCharDBID);
				SetStateIcon(pItemFriend, info.Location.cServerLocation);
			}
#else
			CDnFriendTask::FRIENDINFO_MAP_ITER iter = pFriendInfoList->begin();
			for( ; iter != pFriendInfoList->end(); ++iter )
			{
				CDnFriendTask::SFriendInfo& info = iter->second;
				if (GetHideOffline() && IsFriendInGame(info.Location) == false)
					continue;

				pItemFriend = m_pTreeFriend->AddChildItem( pItemGroup, CTreeItem::typeOpen, info.wszFriendName.c_str(), textcolor::DARKGRAY );
				pItemFriend->SetItemValueInt64( iter->first );
				SetStateIcon(pItemFriend, info.Location.cServerLocation);
			}
#endif
		}
	}

	SetCount( GetFriendTask().GetFriendCount(), FRIEND_MAXCOUNT );

	m_pTreeFriend->ExpandAll();
}

bool CDnFriendDlg::IsFriendInGame(const TCommunityLocation& location) const
{
	return (location.cServerLocation != CDnFriendTask::_LOCATION_NONE) ? true : false;
}

void CDnFriendDlg::SetStateIcon(CTreeItem* friendItem, BYTE location)
{
	if (friendItem != NULL)
	{
		CTreeItem::eClassIconType curState = (location == CDnFriendTask::_LOCATION_NONE) ? CTreeItem::eYELLOW : CTreeItem::eGREEN;
		friendItem->SetClassIconType( curState );
	}
	else
	{
		_ASSERT(0);
	}
}

// void CDnFriendDlg::RefreshFriendGroupTitle()
// {
// 	CTreeItem *pItemGroup(NULL);
// 
// 	const CDnFriendTask::FRIENDGROUPINFO_VEC &friendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
// 	for( int i=0; i<(int)friendGroupInfo.size(); ++i )
// 	{
// 		wchar_t title[_MAX_FRIEND_TITLE];
// 		memset(title, 0, sizeof(wchar_t) * _MAX_FRIEND_TITLE);
// 		MakeGroupTitle(title, friendGroupInfo[i].nGroupDBID, friendGroupInfo[i].wszGroupName.c_str());
// 		pItemGroup = m_pTreeFriend->AddItem( CTreeItem::typeOpen, title, textcolor::FONT_GREEN );
// 	}
// }

void CDnFriendDlg::AddFriendGroup( int nGroupID, const wchar_t *wszGroupName )
{
	wchar_t title[_MAX_FRIEND_TITLE];
	memset(title, 0, sizeof(wchar_t) * _MAX_FRIEND_TITLE);
	MakeGroupTitle(title, nGroupID, wszGroupName);

	CTreeItem *pItemGroup = m_pTreeFriend->AddItem( CTreeItem::typeOpen, title, textcolor::FONT_GREEN );
	pItemGroup->SetItemValueFloat( (float)nGroupID );
}

void CDnFriendDlg::OnAddFriendGroup()
{
	RefreshFriendList();
	m_pFriendGroupDlg->OnAddFriendGroup();
	m_pTreeFriend->ResetSelectedItem();
	//m_pTreeFriend->ExpandAll();
}

void CDnFriendDlg::RenameFriendGroup( int nGroupID, const wchar_t *wszGroupName )
{
	wchar_t title[_MAX_FRIEND_TITLE];
	memset(title, 0, sizeof(wchar_t) * _MAX_FRIEND_TITLE);
	MakeGroupTitle(title, nGroupID, wszGroupName);

	CTreeItem *pItemGroup = m_pTreeFriend->FindItemFloat( (float)nGroupID );
	pItemGroup->SetText( title, textcolor::FONT_GREEN );
}

void CDnFriendDlg::DeleteFriendGroup( int nGroupID )
{
	CTreeItem *pItemGroup = m_pTreeFriend->FindItemFloat( (float)nGroupID );
	m_pTreeFriend->DeleteItem( pItemGroup );
}

void CDnFriendDlg::AddFriend( int nGroupID, INT64 biCharDBID, CDnFriendTask::eLocationState state, const wchar_t *wszFriendName )
{
	//enum eLocationState
	//{
	//	_LOCATION_NONE = 0,		//옵후 라인 입니다
	//	_LOCATION_MOVE,			//서버 이동중인 상태도 체킹해야 할까? 해서 일단 맹글어 봅뉘다
	//	_LOCATION_LOGIN,
	//	_LOCATION_VILLAGE,
	//	_LOCATION_GAME,
	//};

	// Note : 친구 추가할 때 로케이션을 얻어서 온라인, 오프라인 상태를 설정한다.
	//		

	RefreshFriendList();
	EnableButton(false);
	m_pTreeFriend->ResetSelectedItem();
	//m_pTreeFriend->ExpandAll();

// 	CTreeItem *pItemGroup = m_pTreeFriend->FindItemFloat( (float)nGroupID );
// 	CTreeItem *pItemFriend = m_pTreeFriend->AddChildItem( pItemGroup, CTreeItem::typeOpen, wszFriendName, textcolor::DARKGRAY );
// 	pItemFriend->SetItemValueInt( nFriendID );
// 	m_pTreeFriend->ExpandAllChildren( pItemGroup );
// 
// 	CTreeItem::eClassIconType curState = (state == CDnFriendTask::_LOCATION_NONE) ? CTreeItem::eYELLOW : CTreeItem::eGREEN;
// 	pItemFriend->SetClassIconType(curState);
// 
// 	SetCount( GetFriendTask().GetFriendCount(), FRIEND_MAXCOUNT );
}

void CDnFriendDlg::OnRecvDeleteFriend()
{
	RefreshFriendList();
	EnableButton(false);

	CTreeItem *pItem = m_pTreeFriend->GetSelectedItem();
	if (pItem)
		m_pTreeFriend->ResetSelectedItem();
}

void CDnFriendDlg::DeleteFriend( INT64 biCharDBID )
{
	CTreeItem *pItemFriend = m_pTreeFriend->FindItemInt64( biCharDBID );
	if( !pItemFriend )
	{
		CDebugSet::ToLogFile( "CDnFriendDlg::DeleteFriend, Invalid Friend ID(%I64d)!", biCharDBID );
		return;
	}

	m_pTreeFriend->DeleteItem( pItemFriend );

	SetCount( GetFriendTask().GetFriendCount(), FRIEND_MAXCOUNT );
}

void CDnFriendDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case FRIEND_INFO_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				m_FriendDlgGroup.ShowDialog(FRIEND_INFO_DIALOG, false);
			}
		}
		break;
	case FRIEND_ADD_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				m_FriendDlgGroup.ShowDialog(FRIEND_ADD_DIALOG, false);
			}
		}
		break;
	case FRIEND_GROUP_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( IsCmdControl("ID_BUTTON_CLOSE") )
				{
					m_FriendDlgGroup.ShowDialog(FRIEND_GROUP_DIALOG, false);
				}
			}
		}
		break;

	case MESSAGEBOX_FRIEND_DELETE:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if (IsCmdControl("ID_YES"))
				{
					CTreeItem *pItem = m_pTreeFriend->GetSelectedItem();
					if (pItem)
						GetFriendTask().RequestFriendDelete(pItem->GetItemValueInt64());
					else
						_ASSERT(0);
				}
			}
		}
		break;
	}
}

bool CDnFriendDlg::GetHideOffline() const
{
	return CGameOption::GetInstance().m_bCommFriendHideOffline;
}