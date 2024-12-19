#include "StdAfx.h"
#include "DnPVPLadderInviteUserListDlg.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnWorld.h"
#include "DnFriendTask.h"
#include "DnGuildTask.h"
#include "DnPVPLobbyVillageTask.h"
#include "PvPSendPacket.h"
#include "GuildSendPacket.h"
#if defined(PRE_ADD_DWC)
#include "DnDWCTask.h"
#include "DnPlayerActor.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPLadderInviteUserListDlg::CDnPVPLadderInviteUserListDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	nSelectedPage = eLadderInviteUserCommon::Page_WaitUser;
	vWaitUserSlot.reserve(PvPCommon::WaitUserList::Common::MaxPerPage);     // 슬롯의 최대 갯수는 12개입니다.
	m_nUserType = eWaitUser::TypeLobby;

	m_fRefreshTime = 0.f;
	m_nUserPageIndex = 0;
	m_nUserMaxPage = 0;
	m_nRefreshUserCount = 0;
	m_fRequestGuildTimeLimit = 0.f;
	m_fRequestFriendTimeLimit = 0.f;

	m_pStatic_Select = NULL;
	m_pStatic_Page = NULL;
	m_pButton_Refresh = NULL;
	m_pButton_Prev = NULL;
	m_pButton_Next = NULL;
	m_pButton_WaitUser = NULL;
	m_pButton_Friend = NULL;
	m_pButton_Guild = NULL;
	m_pButton_Invite = NULL;
	m_pEditBoxUserName = NULL;
}

CDnPVPLadderInviteUserListDlg::~CDnPVPLadderInviteUserListDlg(void)
{
}

void CDnPVPLadderInviteUserListDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpLadderInviteDlg.ui" ).c_str(), bShow );
}

void CDnPVPLadderInviteUserListDlg::InitialUpdate()
{
	InitializeCommonUI();
	InitializeUserListUI();
}

void CDnPVPLadderInviteUserListDlg::InitializeCommonUI()
{
	m_pStatic_Select = GetControl<CEtUIStatic>( "ID_SELECT" );
	m_pStatic_Page = GetControl<CEtUIStatic>( "ID_TEXT_PAGE" );
	m_pButton_Refresh = GetControl<CEtUIButton>( "ID_BT_REFRESH" );
	m_pButton_Prev = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pButton_Next = GetControl<CEtUIButton>( "ID_BT_NEXT" );
	m_pButton_WaitUser = GetControl<CEtUIRadioButton>( "ID_RBT_TAB0" );
	m_pButton_Friend = GetControl<CEtUIRadioButton>( "ID_RBT_TAB1" );
	m_pButton_Guild = GetControl<CEtUIRadioButton>( "ID_RBT_TAB2" );
	m_pButton_Invite = GetControl<CEtUIButton>( "ID_BT_INVITE" );
	m_pEditBoxUserName = GetControl<CEtUIIMEEditBox>( "ID_IMEEDITBOX_NAME" );

	m_pButton_Refresh->SetDisableTime(1.f);
	m_pButton_Guild->Enable(false); // 길드는 가입되어야 활성화 시킵니다.
	m_pButton_Prev->Enable(false);
	m_pButton_Next->Enable(false);
	m_pStatic_Select->Show(false);
	m_pButton_WaitUser->SetChecked(true);

#ifdef PRE_ADD_DWC
	// IME창에 DWC유저가 아니거나, 멤버가 아닌 유저를 초대하지 못하도록 비활성화 시킴
	m_pEditBoxUserName->Enable( !GetDWCTask().IsDWCChar() );
#endif
}

void CDnPVPLadderInviteUserListDlg::InitializeUserListUI()
{
	char szUIName[256] = "";

	for(int i=0;i<PvPCommon::WaitUserList::Common::MaxPerPage; i++)
	{
		sUI_LadderUserList sWAitUserSlotTemp;
		vWaitUserSlot.push_back(sWAitUserSlotTemp);

		sprintf(szUIName,"ID_TEXT_LEVEL%d",i);
		vWaitUserSlot[i].pStatic_Level = GetControl<CEtUIStatic>( szUIName );
		sprintf(szUIName,"ID_TEXT_NAME%d",i);
		vWaitUserSlot[i].pStatic_Name = GetControl<CEtUIStatic>( szUIName );
		sprintf(szUIName,"ID_TEXTUREL_CLASS%d",i);
		vWaitUserSlot[i].pStatic_JobIcon = GetControl<CDnJobIconStatic>( szUIName );
		sprintf(szUIName,"ID_STATIC_BAR%d",i);
		vWaitUserSlot[i].pStaticDummy = GetControl<CEtUIStatic>( szUIName );
	}
}

void CDnPVPLadderInviteUserListDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	m_fRefreshTime += fElapsedTime;

	if( m_fRefreshTime > PvPCommon::Common::RoomListRefreshGapTime/1000  ) //자동 갱신시간 5초
	{
		m_fRefreshTime =0.0f;
		SendRefreshWaitUserList(m_nUserPageIndex);
	}

	if(m_fRequestFriendTimeLimit  > -1) m_fRequestFriendTimeLimit  -= fElapsedTime;
	if(m_fRequestGuildTimeLimit  > -1) m_fRequestGuildTimeLimit  -= fElapsedTime;
}

void CDnPVPLadderInviteUserListDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if(bShow)
	{
		CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
		if(pPVPLobbyTask)
		{
			if(!pPVPLobbyTask->GetUserInfo().GuildSelfView.IsSet()) // 길드에 가입해있는가?
				m_pButton_Guild->Enable(false);
			else
				m_pButton_Guild->Enable(true);

			m_pEditBoxUserName->ClearText();
		}
		ClearWaitUserList();
		SendRefreshWaitUserList(m_nUserPageIndex);

#ifdef PRE_ADD_DWC
		SetDWCModeControls();
#endif
	}

	CEtUIDialog::Show( bShow );
}

void CDnPVPLadderInviteUserListDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CLOSE"))	
		{
			Show(false);
		}
		if(IsCmdControl("ID_BT_INVITE" ))
		{
			CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
			if(!pPVPLobbyTask)
				return;
			if( __wcsicmp_l( m_pEditBoxUserName->GetText(), pPVPLobbyTask->GetUserInfo().wszCharacterName ) == NULL ) // 나를 초대하는경우
			{
				GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3900 ) , textcolor::YELLOW ,4.0f );
				return;
			}
			else if( m_pEditBoxUserName->GetFullTextLength() == 0)
			{
				GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3539 ) , textcolor::YELLOW ,4.0f );
				return;
			}

			SendLadderInviteUser(m_pEditBoxUserName->GetText());
		}
		if(IsCmdControl("ID_BT_REFRESH" ))
		{
			SendRefreshWaitUserList(m_nUserPageIndex); // CurPage 값이들어가야 하겠습니다. // UserType 도 들어가야함
			m_pStatic_Select->Show(false);
			m_pEditBoxUserName->ClearText();
			return;
		}
		if(IsCmdControl("ID_BT_NEXT" ))
		{
			if( m_nUserPageIndex < Page::MaxPage )
			{
				m_nUserPageIndex++;
			}

			if(m_nUserPageIndex > m_nUserMaxPage)
				m_nUserPageIndex = m_nUserMaxPage;

			SendRefreshWaitUserList(m_nUserPageIndex);
			return;
		}
		if(IsCmdControl("ID_BT_PRIOR" ))
		{
			if( m_nUserPageIndex > 0 )
			{
				m_nUserPageIndex--;
			}
			SendRefreshWaitUserList(m_nUserPageIndex);
			return;
		}
	}
	else if(nCommand == EVENT_RADIOBUTTON_CHANGED)
	{

		if(IsCmdControl("ID_RBT_TAB0"))
		{
#if defined(PRE_ADD_DWC)
			if(GetDWCTask().IsDWCChar())
				m_nUserType = eWaitUser::TypeDWC;
			else
				m_nUserType = eWaitUser::TypeLobby;

			m_nUserPageIndex = 0;
			SendRefreshWaitUserList(m_nUserPageIndex);
			return;
#else
			m_nUserType = eWaitUser::TypeLobby;
			m_nUserPageIndex = 0;
			SendRefreshWaitUserList(m_nUserPageIndex);
			return;
#endif // PRE_ADD_DWC
		}
		if(IsCmdControl("ID_RBT_TAB1"))
		{
			m_nUserType = eWaitUser::TypeFriend;
			m_nUserPageIndex = 0;

			ClearWaitUserList(); // 리스트 지워준뒤
			SendRefreshWaitUserList(m_nUserPageIndex); 
			return;

		}
		if(IsCmdControl("ID_RBT_TAB2"))
		{
			m_nUserType = eWaitUser::TypeGuild;
			m_nUserPageIndex = 0;
			ClearWaitUserList(); // 리스트 지워준뒤
			SendRefreshWaitUserList(m_nUserPageIndex);
			return;
		}

	}
}

bool CDnPVPLadderInviteUserListDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	if( !IsShow() )
	{
		return false;
	}	

	switch( uMsg )
	{
	case WM_LBUTTONDBLCLK:
		break;

	case WM_LBUTTONDOWN:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			for( int i = 0 ; i < m_nRefreshUserCount ; ++i ) 
			{
				if( !vWaitUserSlot[i].pStaticDummy ) continue;
				SUICoord uiCoord;
				vWaitUserSlot[i].pStaticDummy->GetUICoord( uiCoord );

				if( uiCoord.IsInside( fMouseX, fMouseY ) ) 
				{
					m_pStatic_Select->SetUICoord(vWaitUserSlot[i].pStaticDummy->GetUICoord());
					m_pStatic_Select->Show(true);
					m_pEditBoxUserName->SetText(vWaitUserSlot[i].pStatic_Name->GetText());
					break;
				}
			}
		}
		break;

	case WM_RBUTTONDOWN:
			break;

	case WM_MOUSEWHEEL:
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPVPLadderInviteUserListDlg::ClearWaitUserList(bool bShow)
{
	if(vWaitUserSlot.empty())
		return;

	for(int i=0;i<(int)vWaitUserSlot.size();i++)
	{
		vWaitUserSlot[i].pStatic_Level->SetText(L"");
		vWaitUserSlot[i].pStatic_Name->SetText(L"");
		vWaitUserSlot[i].pStatic_JobIcon->Show(bShow);
	}
}

void CDnPVPLadderInviteUserListDlg::SetFriendList(int nPageNumber)
{
	int nFriendCount = 0;    // 페이지별 친구숫자
	int nAllFriendCount = 0; // 모든 친구숫자

	if(!CTaskManager::GetInstance().GetTask( "FriendTask" ))
		return;

	const CDnFriendTask::FRIEND_MAP& mFriendList = GetFriendTask().GetFriendList();    // 친구 리스트를 받습니다.

	if(mFriendList.empty())
		return;

	CDnFriendTask::FRIEND_MAP::const_iterator iter = mFriendList.begin();
	PvPCommon::WaitUserList::Repository sFriendUserList;

	for( ; iter != mFriendList.end(); ++iter ) // 친구 목록의 <그룹 맵 리스트>
	{
		const CDnFriendTask::FRIENDINFO_MAP *pFriendInfo = iter->second;
		CDnFriendTask::FRIENDINFO_MAP::const_iterator _iter;
		_iter = pFriendInfo->begin();

		for (; _iter != pFriendInfo->end(); ++_iter)
		{
			const CDnFriendTask::SFriendInfo *info = &(_iter->second);

			if(nFriendCount< PvPCommon::WaitUserList::Common::MaxPerPage)
			{

				if((nPageNumber *  PvPCommon::WaitUserList::Common::MaxPerPage) <= nAllFriendCount)
				{
					if(info->bHaveDetailInfo)
					{
						if(info->Location.cServerLocation == CDnFriendTask::_LOCATION_NONE ) // 오프라인중
							continue;

						sFriendUserList.WaitUserList[nFriendCount].cJob = info->nJob;
						sFriendUserList.WaitUserList[nFriendCount].cLevel = info->nLevel;
						sFriendUserList.WaitUserList[nFriendCount].cPvPLevel = 0;
						wsprintf(sFriendUserList.WaitUserList[nFriendCount].wszCharName,info->wszFriendName.c_str()); // 디테일 정보가 있을때만 표기해줍니다.
						nFriendCount++;
					}
					else if(!info->bHaveDetailInfo)
					{
						GetFriendTask().RequestFriendDetailInfo( info->biFriendCharDBID ); // 값이없으면 요청합니다.
						continue;
					}
					nAllFriendCount++;
				}
				else
				{
					nAllFriendCount++;
				}
			}
			else
			{
				break;
			}
		}

		if(nFriendCount >= PvPCommon::WaitUserList::Common::MaxPerPage)
			break;
	}

	if(m_nUserType == eWaitUser::TypeFriend)
	{
		m_nUserMaxPage = nAllFriendCount/PvPCommon::WaitUserList::Common::MaxPerPage;
		m_nRefreshUserCount = nFriendCount;
		SetWaitUserList(sFriendUserList,m_nUserMaxPage);
	}
}

void CDnPVPLadderInviteUserListDlg::SetGuildList(int nPageNumber)
{
	if(!CTaskManager::GetInstance().GetTask( "GuildTask" ))
		return;

	int GuildList = GetGuildTask().GetGuildMemberCount();  // 길드 리스트를 받습니다.
	int nUserCount = 0;

	PvPCommon::WaitUserList::Repository sUserList;

	if(!GetGuildTask().IsHaveGuildInfo())
	{
		GetGuildTask().RequestGetGuildInfo(true);
		m_fRequestGuildTimeLimit = eWaitUser::RequestTimeLimit_Guild;
		return;
	}

	for(int i=0;i<GuildList;i++) // 길드 숫자 만큼 돌리는데
	{
		if(i>=(nPageNumber * PvPCommon::WaitUserList::Common::MaxPerPage))
		{
			if(nUserCount<PvPCommon::WaitUserList::Common::MaxPerPage)
			{
				TGuildMember *pMember = GetGuildTask().GetGuildMemberFromArrayIndex(i);
				if (pMember->Location.cServerLocation == CDnFriendTask::_LOCATION_NONE) // 오프라인중입니다.
				{
					continue;
				}
				sUserList.WaitUserList[nUserCount].cJob = pMember->nJob;
				sUserList.WaitUserList[nUserCount].cLevel = pMember->cLevel;
				wsprintf(sUserList.WaitUserList[nUserCount].wszCharName,pMember->wszCharacterName);
				nUserCount++;
			}
			else
			{
				continue;
			}
		}
	}

	if(m_nUserType == eWaitUser::TypeGuild)
	{
		m_nUserMaxPage = GuildList/PvPCommon::WaitUserList::Common::MaxPerPage;
		m_nRefreshUserCount = nUserCount;
		SetWaitUserList(sUserList,nPageNumber);
	}
}

#if defined(PRE_ADD_DWC)
void CDnPVPLadderInviteUserListDlg::SetDWCMemberList(int nPageNumber)
{
	int nUserCount = 0;
	PvPCommon::WaitUserList::Repository sUserList;

	if(GetDWCTask().HasDWCTeam() == false)
		return;

	std::vector<TDWCTeamMember> vMemberList = GetDWCTask().GetDwcTeamMemberList();	
	if(vMemberList.empty()) 
		return;
	
	int nMemberCnt = (int)vMemberList.size();
	for(int i = 0; i < nMemberCnt; i++)
	{
		if( i >= (nPageNumber * PvPCommon::WaitUserList::Common::MaxPerPage) )
		{
			if( nUserCount < PvPCommon::WaitUserList::Common::MaxPerPage )
			{
				TDWCTeamMember Member = vMemberList[i];

				if( Member.Location.cServerLocation == _LOCATION_NONE ) // 오프라인중입니다.
					continue;

				//if( pCommonTask->IsInPartyList(Member.biCharacterDBID) ) // 이미 파티에 들어와있으면 건너뜀
				//	continue;

				if( GetDWCTask().IsMyName(Member.wszCharacterName) )
					continue;

				sUserList.WaitUserList[nUserCount].cJob = Member.cJobCode;
				sUserList.WaitUserList[nUserCount].cLevel = DWC::eDWCCharLevel::DWC_MAX_LEVEL;
				wsprintf(sUserList.WaitUserList[nUserCount].wszCharName, Member.wszCharacterName);
				nUserCount++;
			}
			else
			{
				continue;
			}
		}
	}

	if(m_nUserType == eWaitUser::TypeDWC)
	{
		m_nUserMaxPage = nMemberCnt / PvPCommon::WaitUserList::Common::MaxPerPage;
		m_nRefreshUserCount = nUserCount;
		SetWaitUserList(sUserList,nPageNumber);
	}
}
#endif


void CDnPVPLadderInviteUserListDlg::SetWaitUserList(PvPCommon::WaitUserList::Repository sUserList , UINT uiMaxPage)
{
	for(int i=0;i<(int)vWaitUserSlot.size();i++)
	{
		if(i>=m_nRefreshUserCount)
			break;

		if( sUserList.WaitUserList[i].cJob > 0 )
		{
			vWaitUserSlot[i].pStatic_JobIcon->SetIconID( sUserList.WaitUserList[i].cJob , true );
			vWaitUserSlot[i].pStatic_JobIcon->Show(true);
		}

		vWaitUserSlot[i].pStatic_Level->Show(true);
		vWaitUserSlot[i].pStatic_Level->SetIntToText(sUserList.WaitUserList[i].cLevel);
		vWaitUserSlot[i].pStatic_Name->SetText(sUserList.WaitUserList[i].wszCharName);
		vWaitUserSlot[i].pStatic_Name->Show(true);
	}

	m_nUserMaxPage = uiMaxPage;

	if(m_nUserPageIndex>m_nUserMaxPage)
	{
		m_nUserPageIndex = m_nUserMaxPage;
	}

	WCHAR wsTemp[256];
	wsprintf(wsTemp,L"%d/%d",m_nUserPageIndex+1,m_nUserMaxPage+1); // 최소값이 0/0 이기때문에 +1 씩 해줍니다.

	if(m_nUserPageIndex == m_nUserMaxPage) // 둘이 같다면 다음페이지 버튼 비활성화 해야합니다.
	{
		if(m_nUserPageIndex == 0)
		{
			m_pButton_Prev->Enable(false); // 현재 페이지가 최소값이라면 이전페이지 비활성화
		}
		else
		{
			m_pButton_Prev->Enable(true); // 맥스값이면 활성화

		}
		m_pButton_Next->Enable(false);
	}
	else
	{
		if(m_nUserPageIndex > 0)
		{
			m_pButton_Prev->Enable(true); // 현재 페이지가 0보다 커야지 이전페이지 활성.
		}
		else if(m_nUserPageIndex == 0)
		{
			m_pButton_Prev->Enable(false); // 최소일때는 항상 비활성화
		}

		m_pButton_Next->Enable(true);
	}

	m_pStatic_Page->SetText(wsTemp);
}

void CDnPVPLadderInviteUserListDlg::SendRefreshWaitUserList(UINT Page)
{
	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );

	if( pPVPLobbyTask &&  pPVPLobbyTask->GetEnterd())
	{
		if(m_nUserType == eWaitUser::TypeLobby)
		{
			SendPvPWaitUserList(Page,PvPCommon::WaitUserList::SortType::eSortType::Null,true);
		}
		else if(m_nUserType == eWaitUser::TypeFriend)
		{
			ClearWaitUserList();
			SetFriendList(Page);

			if(m_fRequestFriendTimeLimit <= 0)
			{
				if(CTaskManager::GetInstance().GetTask( "FriendTask" ))
					GetFriendTask().RequestFriendLocationList();

				m_fRequestFriendTimeLimit = eWaitUser::RequestTimeLimit_Friend; // 1분설정 - 갱신눌른다고 항상 패킷을 보내지않음
			}

		}
		else if(m_nUserType == eWaitUser::TypeGuild)
		{
			ClearWaitUserList();
			SetGuildList(Page);

			if(m_fRequestGuildTimeLimit <= 0)
			{
				if(CTaskManager::GetInstance().GetTask( "GuildTask" ))
					SendGetGuildInfo(true);

				m_fRequestGuildTimeLimit = eWaitUser::RequestTimeLimit_Guild;  // 1분설정 - 갱신눌른다고 항상 패킷을 보내지않음
			}
		}
#if defined(PRE_ADD_DWC)
		else if(m_nUserType == eWaitUser::TypeDWC)
		{
			SetDWCMemberList(Page);
		}
#endif
	}
}

#ifdef PRE_ADD_DWC
void CDnPVPLadderInviteUserListDlg::SetDWCModeControls()
{	
	bool bIsDWCUser = GetDWCTask().IsDWCChar();
	CEtUIStatic* pTempStatic = NULL;

	pTempStatic = GetControl<CEtUIStatic>("ID_TEXT1");
	if(pTempStatic) pTempStatic->Show(!bIsDWCUser);

	std::wstring wszText = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, (bIsDWCUser)? 120265 : 904);
	m_pButton_WaitUser->SetText(wszText); // 팀원;120225 // 대기:904

	m_pButton_Friend->Show(!bIsDWCUser);
	m_pButton_Guild->Show(!bIsDWCUser);
}
#endif // PRE_ADD_DWC
