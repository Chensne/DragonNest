#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDWCTask.h"
#include "DnPlayerActor.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "DnDwcDlg.h"
#include "DnCommunityDlg.h"
#include "DnMainMenuDlg.h"
#include "DnChatRoomDlg.h"
#include "VillageSendPacket.h"
#include "DnInterfaceString.h"
#include "PvPSendPacket.h"
#include "DnPVPDWCTabDlg.h"
#include "DnPVPLobbyChatTabDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnDWCTask::CDnDWCTask() : CTaskListener(true)
, m_bIHaveDWCTeam(false)
, m_bDwcLeader(false)
, m_bIsDWCChar(false)
{
	m_wszMyName.clear();
	m_vDwcTeamInfoList.clear();	
	m_vDwcTeamMemberList.clear();
	m_listDwcInOutMemeberList.clear();
	memset(&m_DWCTeamNameInfo, 0, sizeof(m_DWCTeamNameInfo));
	memset(&m_DWCChannelInfo,  0, sizeof(m_DWCChannelInfo));
}

CDnDWCTask::~CDnDWCTask()
{
}

void CDnDWCTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 	
	{
	case CALLBACK_MEMBER_INVITE_MSG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				bool bIsAcceptInvite = false;
				if(strcmp( pControl->GetControlName(), "ID_OK" ) == 0) 
					bIsAcceptInvite = true;
				else if((strcmp( pControl->GetControlName(), "ID_CANCEL" ) == 0))
					bIsAcceptInvite = false;

				SendDwcMemberInviteRequest(bIsAcceptInvite);
				GetInterface().CloseDWCInviteReqDlg(false);
			}
			break;
		}
	}
}

void CDnDWCTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	bool bShow = IsDWCRankSession();
	if(m_bIsDWCChar)
		GetInterface().RefreshDWCMark(bShow, true);
}

void CDnDWCTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	switch(nMainCmd)
	{
		case SC_DWC: OnRecvDWCMessage(nSubCmd, pData, nSize);		break;
		case SC_NPC: OnRecvDWCNpcMessage(nSubCmd, pData, nSize);	break;
	}
}

void CDnDWCTask::OnRecvDWCNpcMessage( int nSubCmd, char *pData, int nSize )
{
	switch(nSubCmd)
	{
		case eNpc::eSCNpc::SC_OPEN_DWCTEAMCREATE: OnRecvShowDwcTeamCreateDlg(); break;
	}
}

void CDnDWCTask::OnRecvDWCMessage( int nSubCmd, char *pData, int nSize )
{
	switch(nSubCmd)
	{
	case eDWC::eSCDWC::SC_CREATE_DWCTEAM:				OnRecvCreatedDwcTeam((SCCreateDWCTeam*)pData);		break; // 팀 생성 응답

	case eDWC::eSCDWC::SC_DISMISS_DWCTEAM:				OnRecvDisMissDwcTeam((SCDismissDWCTeam*)pData);		break; // 팀 해체 응답
	case eDWC::eSCDWC::SC_LEAVE_DWCTEAM_MEMB:			OnRecvLeaveDwcMember((SCLeaveDWCTeamMember*)pData);	break; // 길드원 탈퇴 응답

	case eDWC::eSCDWC::SC_INVITE_DWCTEAM_MEMBREQ:		OnRecvInviteDwcMember((SCInviteDWCTeamMemberReq*)pData);	break; // 길드원 초대 요청 // ProcessBarDlg를 열어준다..
	case eDWC::eSCDWC::SC_INVITE_DWCTEAM_MEMBACK:		OnRecvInviteDwcMemberReq((SCInviteDWCTeamMemberAck*)pData);	break; // 길드원 초대 응답 // PorcessBarDlg에서 Yes / No여부를 알려준다.

	case eDWC::eSCDWC::SC_GET_DWCTEAM_INFO:				OnRecvDwcTeamInfo((SCGetDWCTeamInfo*)pData);		break; // 팀   정보
	case eDWC::eSCDWC::SC_GET_DWCTEAM_MEMBER:			OnRecvDwcTeamMemberInfo((SCGetDWCTeamMember*)pData);break; // 팀원 정보

	case eDWC::eSCDWC::SC_GET_DWC_RANKPAGE:				OnRecvDWCRankList((SCGetDWCRankPage*)pData);		break; // 랭킹 게시판
	case eDWC::eSCDWC::SC_GET_DWC_FINDRANK:				OnRecvDWCFindRank((SCGetDWCFindRank*)pData);		break; // 랭킹 게시판 검색

	case eDWC::eSCDWC::SC_INVITE_DWCTEAM_MEMBACK_RESULT:OnRecvDwcInviteDwcMemberAckResult((SCInviteDWCTeamMemberAckResult*)pData); break;
	case eDWC::eSCDWC::SC_CHANGE_DWCTEAM_MEMBERSTATE:	OnRecvDwcRefreshMemberState((SCChangeDWCTeamMemberState*)pData); break; // 멤버 로케이션 정보 새로고침
	case eDWC::eSCDWC::SC_DWC_TEAMNAME_INFO:			OnRecvDWCTeamNameInfo((SCDWCTeamNameInfo*)pData); break; // 팀 이름( 상대팀 포함 ) Recv
	case eDWC::eSCDWC::SC_DWCCHANNELINFO:				OnRecvDWCChannelInfo((SCDWCChannelInfo*)pData); break;

	case eDWC::eSCDWC::SC_CHANGE_DWCTEAM:				OnRecvChangeDWCTeam((SCChangeDWCTeam*)pData); break;
	}
}

void CDnDWCTask::OnRecvCreatedDwcTeam(SCCreateDWCTeam* pPacket) // 팀생성 Recv
{
	ASSERT(pPacket);
	
	if(pPacket->nRet == ERROR_NONE)
	{	
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120237)); // mid: 팀이 생성되었습니다  마음에 드는 팀원을 초대		
		
		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if(pCommunityDlg)
		{
			CDnDwcDlg*  pDWCDlg = pCommunityDlg->GetDWCDialog();
			if(pDWCDlg) pDWCDlg->Reset();
		}
	}
	else
	{
		GetInterface().ServerMessageBox(pPacket->nRet);
	}
	
	SendDWCTeamInfomation(true);
	GetInterface().CloseDwcTeamCreateDlg();
}

void CDnDWCTask::OnRecvLeaveDwcMember(SCLeaveDWCTeamMember* pPacket)
{
	ASSERT(pPacket);

	// 팀 탈퇴
	if(pPacket->nRet == ERROR_NONE)
	{
		// UISTRING : 님이 파티에서 탈퇴하였습니다.
		std::wstring temp = FormatW(_T("%s%s"), pPacket->wszCharacterName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3529 ));
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", temp.c_str());	

		// 나간 사람이 본인인지 체크.
		if(CDnActor::s_hLocalActor)
		{
			CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if(pPlayer)
			{
				if(wcscmp(pPacket->wszCharacterName, pPlayer->GetName()) == 0)
				{
					m_bDwcLeader = m_bIHaveDWCTeam = false;
					
					// 팀멤버창 CLOSE
					CDnCommunityDlg*  pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
					if(pCommunityDlg) pCommunityDlg->Show(false);

					m_vDwcTeamMemberList.clear();
					m_vDwcTeamInfoList.clear();
				}
			}
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);

	SendDWCTeamInfomation(true);
}

void CDnDWCTask::OnRecvDisMissDwcTeam(SCDismissDWCTeam* pPacket)
{
	ASSERT(pPacket);

	// 팀해체
	if(pPacket->nRet == ERROR_NONE)
	{
		// mid: 팀이 해체 되었습니다.
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120235));
		m_bDwcLeader = m_bIHaveDWCTeam = false;
		m_vDwcTeamMemberList.clear();
		m_vDwcTeamInfoList.clear();
		
		// 1. 커뮤니티창 OFF
		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if(pCommunityDlg && pCommunityDlg->IsShow()) 
			pCommunityDlg->Show(false);

		// 2. PVP Info 새로고침
		CDnPVPLobbyChatTabDlg* pPvPLobbyChatTabDlg = GetInterface().GetPVPLobbyChatTabDlg();
		if(pPvPLobbyChatTabDlg && pPvPLobbyChatTabDlg->IsShow())
			pPvPLobbyChatTabDlg->RefreshMyPVPInfo();

		// 3. 탈퇴했으면, 래더채널에서 나오도록 한다.
		CDnPVPDWCTabDlg* pPVPDwCLadderTabDlg = GetInterface().GetPVPDWCTablDlg();
		if(pPVPDwCLadderTabDlg)
		{
			if( pPVPDwCLadderTabDlg->GetSelectedLadderChannel() == LadderSystem::MatchType::eCode::_3vs3_DWC ||
				pPVPDwCLadderTabDlg->GetSelectedLadderChannel() == LadderSystem::MatchType::eCode::_3vs3_DWC_PRACTICE )
			{
				if( pPVPDwCLadderTabDlg->IsShow() ) {
					pPVPDwCLadderTabDlg->PushUIRadioButton("ID_RBT_COLO");
					GetInterface().MessageBox(120235); // Mid: 팀이 해체되었습니다.
				}
			}
		}
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
	
	SendDWCTeamInfomation(true);
}

void CDnDWCTask::OnRecvInviteDwcMember(SCInviteDWCTeamMemberReq* pPacket)
{	
	ASSERT(pPacket);

	if( GetInterface().IsOpenBlind() )
	{
		SendDwcMemberInviteRequest(false);
		return;
	}

	if( CDnLocalPlayerActor::IsLockInput() )
	{
		SendDwcMemberInviteRequest(false);
		return;
	}

	if( GetInterface().IsShowChangeJobDialog() )
	{
		SendDwcMemberInviteRequest(false);
		return;
	}

	if( GetInterface().IsShowMapMoveCashItemDlg() )
	{
		SendDwcMemberInviteRequest(false);
		return;
	}

	float fTime = _REQUEST_ACCEPT_TOTALTIME;
	if( GetInterface().GetChatRoomDlg()->IsShow() ) // 강제로 자식 링크 거는거기때문에, Process가 2회 호출된다. 그래서 시간 강제로 2배 해준다.
		fTime *= 2.0f;

	m_InvitedDwcTeamMemberInfo.nTeamID		 = pPacket->nTeamID;
	m_InvitedDwcTeamMemberInfo.nFromAcountID = pPacket->nFromAccountDBID;
	GetInterface().OpenDWCInviteReqDlg( pPacket->wszTeamName, pPacket->wszFromCharacterName, fTime, CALLBACK_MEMBER_INVITE_MSG, this);	
}

void CDnDWCTask::OnRecvInviteDwcMemberReq(SCInviteDWCTeamMemberAck* pPacket)
{
	ASSERT(pPacket);

	// 초대를 받았는지 여부 들어옴
	if(pPacket->nRet == ERROR_NONE)
	{
		// 채팅창에 %s님이 팀에 가입하였습니다.
		WCHAR str[128] = {0,};
		wsprintf(str, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120231), pPacket->wszToCharacterName);
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", str );
		
		// 바뀐 팀 멤버 정보를 받기 위해 서버에 요청
		SendDWCTeamInfomation(true); 
	}
	else if( pPacket->nRet == ERROR_DWC_REFUESED_TEAMINVITATION )
	{
		// %s님이 팀원 초대를 거절하셨습니다.
		WCHAR str[128] = {0,};
		wsprintf(str, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120233), pPacket->wszToCharacterName);
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", str);
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}

void CDnDWCTask::OnRecvDwcInviteDwcMemberAckResult(SCInviteDWCTeamMemberAckResult* pPacket)
{
	ASSERT(pPacket);

	if(pPacket->nRet == ERROR_NONE)
	{
	}
	else
		GetInterface().ServerMessageBox(pPacket->nRet);
}

void CDnDWCTask::OnRecvDwcTeamInfo(SCGetDWCTeamInfo* pPacket) // 팀 정보
{
	ASSERT(pPacket);

	if(pPacket->nRet == ERROR_NONE)
	{
		m_bIHaveDWCTeam = true;
		m_vDwcTeamInfoList.clear();
		m_vDwcTeamInfoList.push_back(pPacket->Info);

		// PVP Info 새로고침
		CDnPVPLobbyChatTabDlg* pPvPLobbyChatTabDlg = GetInterface().GetPVPLobbyChatTabDlg();
		if(pPvPLobbyChatTabDlg)
			pPvPLobbyChatTabDlg->RefreshMyPVPInfo();
	}	
	else if(pPacket->nRet == ERROR_DWC_HAVE_NOT_TEAM)
	{
		// mid: 팀이 해체 되었습니다. ( 재접속하는 도중에 팀이 폭파된 경우 마을에 접속시 알려줘야한다 )
		//if(m_bIHaveDWCTeam) // 팀이 있었다면 메세지를 보여준다..
		//	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120235));
		m_bDwcLeader = m_bIHaveDWCTeam = false;
		m_vDwcTeamMemberList.clear();
		m_vDwcTeamInfoList.clear();
	}
}

void CDnDWCTask::OnRecvDwcTeamMemberInfo(SCGetDWCTeamMember* pPacket) // 팀원 정보
{
	ASSERT(pPacket);

	if(pPacket->nRet == ERROR_NONE)
	{	
		// 1. DWCTask에 저장 해둘 팀원정보 세이브
		m_vDwcTeamMemberList.clear();
		for(int i = 0 ; i < pPacket->nCount ; ++i) 
			m_vDwcTeamMemberList.push_back(pPacket->MemberList[i]);

		// 2. 커뮤니티창이 열려있다면, 바꾼정보를 새로고침해줌.
		CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
		if(pCommunityDlg)
		{
			CDnDwcDlg* pDWCDlg = pCommunityDlg->GetDWCDialog();
			if(pDWCDlg && pDWCDlg->IsShow())
				pDWCDlg->RefreshAllData();
		}

		// 3. PVP Info 새로고침
		CDnPVPLobbyChatTabDlg* pPvPLobbyChatTabDlg = GetInterface().GetPVPLobbyChatTabDlg();
		if(pPvPLobbyChatTabDlg)
			pPvPLobbyChatTabDlg->RefreshMyPVPInfo();

		MakeDwcCharacterData();
	}
}

void CDnDWCTask::OnRecvShowDwcTeamCreateDlg()
{
	GetInterface().OpenDwcTeamCreateDlg();
}

void CDnDWCTask::OnRecvDWCRankList(SCGetDWCRankPage* pPacket)
{
	ASSERT(pPacket);

	if(pPacket->nRetCode == ERROR_NONE)
	{
		GetInterface().SetDWCRankBoardInfo(pPacket);
	}
	else
	{
		GetInterface().ShowDWCRankBoardInfo(false);
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1 , 100424)); // mid: 시스템상 오류가 발생하였습니다.
	}
}

void CDnDWCTask::OnRecvDWCFindRank(SCGetDWCFindRank* pPacket)
{
	ASSERT(pPacket);

	if(pPacket->nRetCode == ERROR_NONE)
	{
		GetInterface().SetFindDWCRankBoardInfo(pPacket);
	}
	else
	{
		//GetInterface().ShowDWCRankBoardInfo(false);
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1 , 120269)); // mid: 검색결과가 없습니다
	}
}

void CDnDWCTask::OnRecvDwcRefreshMemberState(SCChangeDWCTeamMemberState* pPacket)
{
	ASSERT(pPacket);

	if(!m_vDwcTeamMemberList.empty())
	{
		for(int i = 0 ; i < (int)m_vDwcTeamMemberList.size() ; ++i)
		{
			if( m_vDwcTeamMemberList[i].biCharacterDBID == pPacket->biCharacterDBID )
			{
				m_vDwcTeamMemberList[i].Location = pPacket->Location;
				break;
			}
		}
	}

	CDnCommunityDlg* pCommunityDlg = (CDnCommunityDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
	if(pCommunityDlg)
	{
		CDnDwcDlg* pDWCDlg = pCommunityDlg->GetDWCDialog();
		if( pDWCDlg && pDWCDlg->IsShow() )
			pDWCDlg->RefreshAllData(CDnDwcDlg::eRefreshType::E_REFRESH_MEMBER_DATA);
	}
}

void CDnDWCTask::OnRecvDWCTeamNameInfo(SCDWCTeamNameInfo* pPacket)
{
	ASSERT(pPacket);
	_wcscpy(m_DWCTeamNameInfo.wszATeamName, _countof(m_DWCTeamNameInfo.wszATeamName), pPacket->wszATeamName, _countof(pPacket->wszATeamName));
	_wcscpy(m_DWCTeamNameInfo.wszBTeamName, _countof(m_DWCTeamNameInfo.wszBTeamName), pPacket->wszBTeamName, _countof(pPacket->wszBTeamName));
}

void CDnDWCTask::OnRecvDWCChannelInfo(SCDWCChannelInfo *pPacket)
{
	ASSERT(pPacket);
	m_DWCChannelInfo = pPacket->ChannelInfo;
}

const bool CDnDWCTask::IsMyName(wchar_t* pName)
{
	if( wcscmp(pName, m_wszMyName.c_str()) == 0 )
		return true;

	return false;
}

const bool CDnDWCTask::IsDWCRankSession()
{	// DWC 기간인지 체크
	// 0 - 정보없음
	// 1 - 일반
	// 2 - 일시정지
	// 3 - 중단됨

	//[_debug...] 와빵똥밑균珂굼웰 DWC  
#if 0
	if( m_DWCChannelInfo.cStatus == DWC::DWCStatus::STATUS_NONE ||				// 0 == 정보없음
		m_DWCChannelInfo.cStatus == DWC::DWCStatus::STATUS_FORCE_TERMINATED )	// 3 == 중단됨
	{
		return false;
	}
#else
		return false;
#endif
	// 상태 체크


	// 시간 체크
	__time64_t tCurrentTime;
	time(&tCurrentTime);
	if( tCurrentTime < m_DWCChannelInfo.tStartDate || tCurrentTime > m_DWCChannelInfo.tEndDate )
	{
		return false;
	}

	return true;
}

void CDnDWCTask::GetDWCRemainMatchTime(eMatchTime eType, std::wstring& wszStartTime , std::wstring& wszEndTime)
{
	if(eType > eDWC_Second_MatchTime)
		return;

	int nValue = m_DWCChannelInfo.nLadderTime[eType];
	if( nValue <= 0 )
	{
		wszStartTime.clear();
		wszEndTime.clear();

		wszStartTime = L"0";
		wszEndTime	 = L"0";
		return;
	}

	WCHAR str[10] = { 0 , };
	_itow(nValue, str, 10);

	wszStartTime.clear();
	wszEndTime.clear();

	std::wstring wszTempStr(str);
	if( wszTempStr.empty() == false )
	{
		int nMax = static_cast<int>(wszTempStr.length());		
		if( nMax < 7 )
		{
			// 7자리나 8자리 LadderTime이 아니면 뭔가 잘못들어온것임
			wszStartTime.clear();
			wszEndTime.clear();

			wszStartTime = L"0";
			wszEndTime	 = L"0";
			return;			
		}

		(nMax >= 8) ? nMax = 8 : nMax = 7;
		wszStartTime = wszTempStr.substr(0, nMax/2);
		if(wszStartTime.size() < 4) wszStartTime.insert(0, std::wstring(L"0"));
		wszEndTime = wszTempStr.substr(nMax/2, nMax);

		wszStartTime.insert(2, std::wstring(L":"));
		wszEndTime.insert(2, std::wstring(L":"));
	}
};

void CDnDWCTask::GetDWCSessionPeriod(std::wstring& wszStartTime, std::wstring& wszEndTime)
{
#ifdef _US
	DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_MM_DD_YY, wszStartTime, m_DWCChannelInfo.tStartDate );
	DN_INTERFACE::STRING::GetDayTextSlash( DN_INTERFACE::STRING::FORMAT_MM_DD_YY, wszEndTime,	m_DWCChannelInfo.tEndDate);
#else
	DN_INTERFACE::STRING::GetDayText( wszStartTime, m_DWCChannelInfo.tStartDate);
	DN_INTERFACE::STRING::GetDayText( wszEndTime,	m_DWCChannelInfo.tEndDate);
#endif // _US
}

const bool CDnDWCTask::IsInPartyList(INT64 biCharacterDBID)
{
	if(m_listDwcInOutMemeberList.empty())
		return false;

	std::list<TDWCTeamMember>::iterator it = m_listDwcInOutMemeberList.begin();
	for( ; it != m_listDwcInOutMemeberList.end() ; ++it)
	{
		TDWCTeamMember& Data = *it;
		if(Data.biCharacterDBID == biCharacterDBID)
			return true;
	}

	return false;
}

bool CDnDWCTask::PopComeInPartyMember(TDWCTeamMember D)
{
	if(m_listDwcInOutMemeberList.empty())
		return false;

	std::list<TDWCTeamMember>::iterator it = m_listDwcInOutMemeberList.begin();
	for( ; it != m_listDwcInOutMemeberList.end() ; ++it)
	{
		TDWCTeamMember& Data = *it;
		if(Data.biCharacterDBID == D.biCharacterDBID)
		{
			m_listDwcInOutMemeberList.erase(it);
			return true;
		}
	}

	return false;
}

void CDnDWCTask::SendDwcMemberInviteRequest(bool bAccept)
{
	m_InvitedDwcTeamMemberInfo.bAccept = bAccept;

	CSInviteDWCTeamMemberAck packet;
	memset(&packet, 0, sizeof(packet));
	packet.bAccept		    = bAccept;
	packet.nTeamID		    = m_InvitedDwcTeamMemberInfo.nTeamID;
	packet.nFromAccountDBID = m_InvitedDwcTeamMemberInfo.nFromAcountID;

	CClientSessionManager::GetInstance().SendPacket(CS_DWC, eDWC::CS_INVITE_DWCTEAM_MEMBACK, (char*)&packet, sizeof(packet));
}

const bool CDnDWCTask::IsDWCPvPModePlaying()
{
	if( IsDWCRankSession() == false )
		return false;
	
	if(CDnBridgeTask::IsActive() == false)
		return false;

	if( IsDWCChar() == true )
	{		
		if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon )
		{
			// DWC게임모드는 AllKill모드다.
			if( CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_AllKill) )
				return true;
		}
	}
	return false;
}

void CDnDWCTask::MakeDwcCharacterData()
{
	// 이름 저장
	if(CDnActor::s_hLocalActor) 
	{
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if(pPlayer == NULL)
			return;

		// 본인의 이름 저장
		m_wszMyName.clear();		
		m_wszMyName = pPlayer->GetName();

		// 본인이 팀장임 체크하는부분.
		if( m_vDwcTeamMemberList.empty() == false ) {
			for(int i = 0 ; i < (int)m_vDwcTeamMemberList.size() ; ++i)
			{							
				if( wcscmp(m_vDwcTeamMemberList[i].wszCharacterName, pPlayer->GetName()) == 0 )
				{
					m_bDwcLeader = m_vDwcTeamMemberList[i].bTeamLeader;
					break;
				}
			}
		}
	}
}

WCHAR* CDnDWCTask::GetMyDWCTeamName()
{ 
	if(!m_vDwcTeamInfoList.empty())
	{
		return m_vDwcTeamInfoList[0].wszTeamName;
	}

	return NULL;
}

void CDnDWCTask::OnRecvChangeDWCTeam(SCChangeDWCTeam* pPacket)
{
	ASSERT(pPacket);

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if(!hActor)
		return;

	CDnPlayerActor* pActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
	if(!pActor)
		return;

	if(wcslen(pPacket->wszTeamName) == 0)
	{
		// 팀이 없다.
		pActor->SetDWCTeamName(NULL);
	}
	else
		pActor->SetDWCTeamName(pPacket->wszTeamName);
}

WCHAR* CDnDWCTask::GetEnemyTeamName()
{
	int nResult = _tcscmp(m_DWCTeamNameInfo.wszATeamName, m_vDwcTeamInfoList[0].wszTeamName);

	if(nResult)
	{
		return m_DWCTeamNameInfo.wszATeamName;
	}
	else
	{
		return m_DWCTeamNameInfo.wszBTeamName;
	}
}
#endif // PRE_ADD_DWC