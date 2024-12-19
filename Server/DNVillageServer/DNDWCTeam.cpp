#include "Stdafx.h"
#if defined(PRE_ADD_DWC)
#include "DNDWCTeam.h"
#include "DNDWCSystem.h"
#include "DNUserSession.h"
#include "DNUserSessionManager.h"
#include "DNWorldUserState.h"
#include "DNMasterConnection.h"

CDnDWCTeam::CDnDWCTeam(UINT nDWCTeamID, TDWCTeam *pInfo, CDNDWCSystem *pManager)
: m_nTeamID(nDWCTeamID), m_Info(*pInfo), m_pManager(pManager), m_biLeaderCharacterDBID(0), m_bRecvMemberList(false)
{
	if(m_Info.nDWCPoint == 0)
	{
		m_Info.nDWCPoint = LadderSystem::Stats::InitGradePoint;
		m_Info.nHiddenDWCPoint = LadderSystem::Stats::InitGradePoint;
	}
}

void CDnDWCTeam::UpdateTeamInfo( TDWCTeam *pInfo )
{
	m_Info = *pInfo;
	if(m_Info.nDWCPoint == 0)
	{
		m_Info.nDWCPoint = LadderSystem::Stats::InitGradePoint;
		m_Info.nHiddenDWCPoint = LadderSystem::Stats::InitGradePoint;
	}
}

bool CDnDWCTeam::AddMember(TDWCTeamMember *pMember, TCommunityLocation* pLocation)
{
	if( m_VectTotalMember.size() > DWC::DWC_MAX_MEMBERISZE )
		return false;

	for(  std::vector<TDWCTeamMember>::iterator iter = m_VectTotalMember.begin() ; iter != m_VectTotalMember.end() ; iter++ )
	{
		if( iter->biCharacterDBID == pMember->biCharacterDBID )
			return false;
	}
	
	sWorldUserState aWorldUserState;
	bool bRetVal = g_pWorldUserState->GetUserState(pMember->biCharacterDBID, &aWorldUserState);

	if(bRetVal)
	{
		pMember->Location.cServerLocation = aWorldUserState.nLocationState;
		pMember->Location.nChannelID = aWorldUserState.nChannelID;
		pMember->Location.nMapIdx = aWorldUserState.nMapIdx;

		if(pLocation)
		{
			pLocation->cServerLocation = aWorldUserState.nLocationState;
			pLocation->nChannelID = aWorldUserState.nChannelID;
			pLocation->nMapIdx = aWorldUserState.nMapIdx;
		}
	}

	g_pDWCTeamManager->RegistDWCCharacter(pMember->biCharacterDBID, GetTeamID());
	m_VectTotalMember.push_back(*pMember);

	if( pMember->bTeamLeader )
	{
		if(!m_bRecvMemberList)	//리더가 처음 팀생성했을 경우
			m_bRecvMemberList = true;

		m_biLeaderCharacterDBID = pMember->biCharacterDBID;
	}
	
	return true;
}

bool CDnDWCTeam::DelMember(INT64 biCharacterDBID)
{
	for(  std::vector<TDWCTeamMember>::iterator iter = m_VectTotalMember.begin() ; iter != m_VectTotalMember.end() ; iter++ )
	{
		if( iter->biCharacterDBID == biCharacterDBID )
		{
			g_pDWCTeamManager->RemoveDWCCharacter(biCharacterDBID);
			m_VectTotalMember.erase(iter);
			return true;
		}
	}
	return false;
}

void CDnDWCTeam::UpdateMemberList(int nCount, TDWCTeamMember *MemberList)
{
	m_VectTotalMember.clear();

	for(int i=0; i < MIN(nCount, DWC::DWC_MAX_MEMBERISZE); i++)
	{
		m_VectTotalMember.push_back(MemberList[i]);
		sWorldUserState aWorldUserState;
		if(g_pWorldUserState->GetUserState(m_VectTotalMember[i].biCharacterDBID, &aWorldUserState))
		{				
			TCommunityLocation Location;
			Location.Reset();
			Location.cServerLocation = aWorldUserState.nLocationState;
			Location.nChannelID = aWorldUserState.nChannelID;
			Location.nMapIdx = aWorldUserState.nMapIdx;
			ChangeDWCTeamMemberState(m_VectTotalMember[i].biCharacterDBID, &Location, false);
			
			//혹시나 해서 등록 안되어 있는 유저 있으면 여기서 추가해 준다.
			if( !g_pDWCTeamManager->IsRegisterdCharacter(m_VectTotalMember[i].biCharacterDBID) )
				g_pDWCTeamManager->RegistDWCCharacter(m_VectTotalMember[i].biCharacterDBID, m_nTeamID);
		}		
		if( MemberList[i].bTeamLeader )
			m_biLeaderCharacterDBID = m_VectTotalMember[i].biCharacterDBID;
	}

	m_bRecvMemberList = true;
}

bool CDnDWCTeam::ChangeDWCTeamMemberState(INT64 biCharacterDBID, TCommunityLocation* pLocation, bool bSend, bool bLogin)
{
	if(!pLocation)
		return false;

	for(  std::vector<TDWCTeamMember>::iterator iter = m_VectTotalMember.begin() ; iter != m_VectTotalMember.end() ; iter++ )
	{
		if( iter->biCharacterDBID == biCharacterDBID )
		{
			iter->Location = (*pLocation);
			if(bSend)
				SendChangeDWCTeamMemberState(&(*iter), bLogin);
			return true;
		}
	}
	return false;
}

bool CDnDWCTeam::CheckRemoveResource()
{
	bool bRetVal = false;
	for(int i = 0; i <  m_VectTotalMember.size() ; i++)
	{
		sWorldUserState aWorldUserState;
		bRetVal = g_pWorldUserState->GetUserState(m_VectTotalMember[i].biCharacterDBID, &aWorldUserState);
		if(bRetVal)
			return false;	//접속중인 팀원이 있으면 패스
	}

	return true;	
}

bool CDnDWCTeam::CheckDuplicationJob(BYTE cJobCode)
{
	for(int i = 0; i <  m_VectTotalMember.size() ; i++)
	{
		if( m_VectTotalMember[i].cJobCode == cJobCode)
			return false;
	}

	return true;
}

bool CDnDWCTeam::CheckCanLeaveTeam()
{
	// 게임중인 유저가 있는 경우 탈퇴나 해체 불가
	for( int i = 0 ; i < m_VectTotalMember.size() ; i++ )
	{		
		if(m_VectTotalMember[i].Location.cServerLocation == _LOCATION_GAME )
			return false;
	}

	return true;
}

void CDnDWCTeam::RemoveDWCCharacters()
{
	for(int i = 0; i <  m_VectTotalMember.size() ; i++)
		g_pDWCTeamManager->RemoveDWCCharacter(m_VectTotalMember[i].biCharacterDBID);
}

//게임서버에서 업데이트 된 DWC 스코어 정보
void CDnDWCTeam::UpdateDWCScore(MADWCUpdateScore *pPacket)
{
	m_Info.nTotalWin = pPacket->DWCScore.nTotalWin;
	m_Info.nTotalLose = pPacket->DWCScore.nTotalLose;
	m_Info.nTotalDraw = pPacket->DWCScore.nTotalDraw;
	m_Info.nDWCPoint = pPacket->DWCScore.nDWCPoint;
	m_Info.nHiddenDWCPoint = pPacket->DWCScore.nHiddenDWCPoint;
	m_Info.wTodayWin = pPacket->DWCScore.wTodayWin;
	m_Info.wTodayLose = pPacket->DWCScore.wTodayLose;
	m_Info.wTodayDraw = pPacket->DWCScore.wTodayDraw;
	m_Info.wConsecutiveWin = pPacket->DWCScore.wConsecutiveWin;
	m_Info.wConsecutiveLose = pPacket->DWCScore.wConsecutiveLose;
	m_Info.nWeeklyPlayCount = pPacket->DWCScore.nWeeklyPlayCount;

	//빌리지에 접속중인 유저한테만 업데이트 시켜준다.
	for( int i = 0 ; i < m_VectTotalMember.size() ; i++ )
	{	
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_VectTotalMember[i].nAccountDBID);
		if(pUserObj)
			pUserObj->SendGetDWCTeamInfo(this, ERROR_NONE);
	}
}

void CDnDWCTeam::GetMemberList( std::vector<TDWCTeamMember> &vMemberList )
{
	if( !m_bRecvMemberList )
		return;

	vMemberList.assign(m_VectTotalMember.begin(), m_VectTotalMember.end());	
}

void CDnDWCTeam::GetMemberCharacterDBIDList(std::vector<INT64> &vMemberCharacterDBIDList)
{
	if( !m_bRecvMemberList )
		return;

	for(int i = 0; i <  m_VectTotalMember.size() ; i++)
		vMemberCharacterDBIDList.push_back(m_VectTotalMember[i].biCharacterDBID);
}

void CDnDWCTeam::ResetTeamScore()
{
	m_Info.nDWCRank = 0;
	m_Info.nTotalWin = 0;
	m_Info.nTotalLose = 0;
	m_Info.nTotalDraw = 0;
	m_Info.nDWCPoint = LadderSystem::Stats::InitGradePoint;
	m_Info.nHiddenDWCPoint = LadderSystem::Stats::InitGradePoint;
	m_Info.wTodayWin = 0;
	m_Info.wTodayLose = 0;
	m_Info.wTodayDraw = 0;
	m_Info.wConsecutiveWin = 0;
	m_Info.wConsecutiveLose = 0;
	m_Info.nWeeklyPlayCount = 0;
	m_Info.tLastPlayDate = 0;
}

void CDnDWCTeam::SendAddDWCTeamMember(UINT nAccountDBID, INT64 nCharacterDBID, LPCWSTR lpwszCharacterName, TP_JOB nJob, TCommunityLocation* pLocation)
{
	for( int i = 0 ; i < m_VectTotalMember.size() ; i++ )
	{
		bool bAlredySentByVillage = false;
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_VectTotalMember[i].nAccountDBID);
		if(pUserObj)
		{
			pUserObj->SendInviteDWCTeamMemberAck(nAccountDBID, nCharacterDBID, lpwszCharacterName, ERROR_NONE, 0, 0, nJob, pLocation, GetTeamID());
			bAlredySentByVillage = true;
		}

		g_pMasterConnection->SendAddDWCTeamMember(m_VectTotalMember[i].nAccountDBID, GetTeamID(), nAccountDBID, nCharacterDBID, lpwszCharacterName, nJob, pLocation, bAlredySentByVillage);
	}	
}

void CDnDWCTeam::SendDismissDWCTeam(UINT nTeamID, int nRet)
{
	for( int i = 0 ; i < m_VectTotalMember.size() ; i++ )
	{
		bool bAlredySentByVillage = false;
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_VectTotalMember[i].nAccountDBID);
		if(pUserObj)
		{
			pUserObj->SendDismissDWCTeam(nTeamID, nRet);
			pUserObj->SetDWCTeamID(0);
			bAlredySentByVillage = true;

			pUserObj->RefresDWCTeamName();
		}
		
		g_pMasterConnection->SendDismissDWCTeam(m_VectTotalMember[i].nAccountDBID, nTeamID, nRet, bAlredySentByVillage);
	}
}

void CDnDWCTeam::SendLeaveDWCTeamMember(UINT nTeamID, INT64 biLeaveUserCharacterDBID, LPCWSTR lpwszCharacterName, int nRet)
{
	for( int i = 0 ; i < m_VectTotalMember.size() ; i++ )
	{
		bool bAlredySentByVillage = false;
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_VectTotalMember[i].nAccountDBID);
		if(pUserObj)
		{
			bAlredySentByVillage = true;
			pUserObj->SendLeaveDWCTeamMember(nTeamID, biLeaveUserCharacterDBID, lpwszCharacterName, nRet);
		}
		
		g_pMasterConnection->SendLeaveDWCTeamMember(m_VectTotalMember[i].nAccountDBID, nTeamID, biLeaveUserCharacterDBID, lpwszCharacterName, nRet, bAlredySentByVillage);
	}
}

void CDnDWCTeam::SendChangeDWCTeamMemberState(TDWCTeamMember *pDWCTeamMember, bool bLogin)
{
	if(!pDWCTeamMember)
		return;

	for( int i = 0 ; i < m_VectTotalMember.size() ; i++ )
	{
		CDNUserSession *pUserObj = g_pUserSessionManager->FindUserSessionByAccountDBID(m_VectTotalMember[i].nAccountDBID);
		if(pUserObj)
			pUserObj->SendChangeDWCTeamMemberState(GetTeamID(), pDWCTeamMember->biCharacterDBID, &pDWCTeamMember->Location);
		else
			g_pMasterConnection->SendChangeDWCTeamMemberState(m_VectTotalMember[i].nAccountDBID, GetTeamID(), pDWCTeamMember->biCharacterDBID, &pDWCTeamMember->Location, bLogin);
	}
}

#endif	//#if defined(PRE_ADD_DWC)