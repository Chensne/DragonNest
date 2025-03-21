#include "Stdafx.h"
#if defined(PRE_ADD_DWC)
#include "DnDWCSystem.h"
#include "DNDWCTeam.h"
#include "DNWorldUserState.h"
#include "TimeSet.h"

CDNDWCSystem * g_pDWCTeamManager = NULL;

CDNDWCSystem::CDNDWCSystem() : m_bInit(false), m_bIsIncludeDWCVillage(false), m_bCloseMatchByCheat(false)
{
	memset(&m_DWCChannelInfo, 0, sizeof(m_DWCChannelInfo));
	m_MapDWCTeamList.clear();
	m_MapDWCCharacter.clear();
	m_VecLadderTime.clear();
}

CDNDWCSystem::~CDNDWCSystem()
{
	for( TMapDWCTeams::iterator iter = m_MapDWCTeamList.begin() ; iter != m_MapDWCTeamList.end() ; )
	{
		delete iter->second;
		m_MapDWCTeamList.erase(iter++);
	}

	m_MapDWCTeamList.clear();
	m_MapDWCCharacter.clear();
}

CDnDWCTeam * CDNDWCSystem::GetDWCTeam(UINT nDWCTeamID)
{
	TMapDWCTeams::iterator iter = m_MapDWCTeamList.find(nDWCTeamID);
	if(iter != m_MapDWCTeamList.end())
		return iter->second;

	return NULL;
}

CDnDWCTeam * CDNDWCSystem::AddDWCTeamResource(UINT nDWCTeamID, TDWCTeam *pTeamInfo)
{
	CDnDWCTeam *pDWCTeam = new CDnDWCTeam(nDWCTeamID, pTeamInfo, this);			
	m_MapDWCTeamList.insert( std::make_pair(nDWCTeamID, pDWCTeam) );
	return pDWCTeam; 
}

bool CDNDWCSystem::RegistDWCCharacter(INT64 biCharacterDBID, UINT nTeamID)
{
	CDnDWCTeam *pDWCTeam = GetDWCTeam(nTeamID);
	if(!pDWCTeam)
		return false;

	sWorldUserState aWorldUserState;
	if( !g_pWorldUserState->GetUserState(biCharacterDBID, &aWorldUserState))
		return false;

	TMapDWCCharacter::iterator iter = m_MapDWCCharacter.find(biCharacterDBID);
	if(iter == m_MapDWCCharacter.end())
		m_MapDWCCharacter.insert( std::make_pair(biCharacterDBID, nTeamID) );
	else
		iter->second = nTeamID;

	TCommunityLocation Location;
	Location.Reset();
	Location.cServerLocation = aWorldUserState.nLocationState;
	Location.nChannelID = aWorldUserState.nChannelID;
	Location.nMapIdx = aWorldUserState.nMapIdx;	
	pDWCTeam->ChangeDWCTeamMemberState(biCharacterDBID, &Location);

	return true;
}

bool CDNDWCSystem::RemoveDWCCharacter(INT64 biCharacterDBID)
{
	TMapDWCCharacter::iterator iter = m_MapDWCCharacter.find(biCharacterDBID);
	if(iter == m_MapDWCCharacter.end())
		return false;

	m_MapDWCCharacter.erase(iter);
	return true;
}

bool CDNDWCSystem::ChangeDWCTeamMemberState(INT64 biCharacterDBID, bool bDel, int nLocation, int nChannelID, int nMapIdx)
{
	TMapDWCCharacter::iterator iter = m_MapDWCCharacter.find(biCharacterDBID);
	if(iter == m_MapDWCCharacter.end())
		return false;

	CDnDWCTeam *pDWCTeam = GetDWCTeam(iter->second);
	if(!pDWCTeam)
		return false;
	
	TCommunityLocation Location;
	Location.Reset();
	Location.cServerLocation = nLocation;
	Location.nChannelID = nChannelID;
	Location.nMapIdx = nMapIdx;
	pDWCTeam->ChangeDWCTeamMemberState(biCharacterDBID, &Location);	

	if(bDel)
	{
		if( pDWCTeam->CheckRemoveResource() )
			DelDWCTeamResource(pDWCTeam->GetTeamID());
		else
			m_MapDWCCharacter.erase(iter);
	}

	return true;
}

UINT CDNDWCSystem::IsRegisterdCharacter(INT64 biCharacterDBID)
{
	TMapDWCCharacter::iterator iter = m_MapDWCCharacter.find(biCharacterDBID);
	if(iter == m_MapDWCCharacter.end())
		return 0;

	return iter->second;
}

CDnDWCTeam* CDNDWCSystem::UpdateDWCTeamResource(TAGetDWCTeamInfo *pPacket)
{
	return UpdateDWCTeamResource(pPacket->nTeamID, &pPacket->Info);
}

CDnDWCTeam* CDNDWCSystem::UpdateDWCTeamResource(UINT nTeamID, TDWCTeam *Info)
{
	CDnDWCTeam *pDWCTeam = GetDWCTeam(nTeamID);

	if (!pDWCTeam)	// 팀 자원이 없는경우 (길드원이 처음 접속)
	{
		pDWCTeam = new CDnDWCTeam(nTeamID, Info, this);
		m_MapDWCTeamList.insert( std::make_pair(nTeamID, pDWCTeam) );

	}
	else // 기존 팀자원이 있는 경우
		pDWCTeam->UpdateTeamInfo(Info);

	return pDWCTeam;
}

CDnDWCTeam* CDNDWCSystem::UpdateTeamMember(TAGetDWCTeamMember *pPacket)
{
	return UpdateTeamMember(pPacket->nTeamID, pPacket->cCount, pPacket->MemberList);
}

CDnDWCTeam* CDNDWCSystem::UpdateTeamMember(int nTeamID, int nCount, TDWCTeamMember *MemberList )
{
	CDnDWCTeam *pDWCTeam = GetDWCTeam(nTeamID);

	if (pDWCTeam)
		pDWCTeam->UpdateMemberList(nCount, MemberList);

	return pDWCTeam;
}

const WCHAR * CDNDWCSystem::GetDWCTeamName(UINT nDWCTeamID)
{
	TMapDWCTeams::iterator iter = m_MapDWCTeamList.find(nDWCTeamID);
	if(iter != m_MapDWCTeamList.end())
	{
		return (*iter->second).GetDWCTeamName();
	}

	return NULL;
}

bool CDNDWCSystem::DelDWCTeamResource(UINT nDWCTeamID)
{
	TMapDWCTeams::iterator iter = m_MapDWCTeamList.find(nDWCTeamID);
	if(iter == m_MapDWCTeamList.end())
		return false;

	iter->second->RemoveDWCCharacters();

	delete iter->second;
	m_MapDWCTeamList.erase(iter);

	return true;
}

void CDNDWCSystem::SetDWCMatchTimeForCheat(bool bSet)
{
	m_bCloseMatchByCheat = bSet;
}

void CDNDWCSystem::Initialize(TAGetDWCChannelInfo* pPacket)
{
	if(pPacket->nRetCode != ERROR_NONE)
		return;

	m_DWCChannelInfo = pPacket->ChannelInfo;	
	m_VecLadderTime.clear();

	for(int i = 0 ; i < 2 ; i ++)	//TAGetDWCChannelInfo 패킷의 nLadderTime 배열 크기가 2
	{
		if( pPacket->ChannelInfo.nLadderTime[i] > 0 )	
		{
			int nStartTime = pPacket->ChannelInfo.nLadderTime[i] / 10000;
			int nEndTime = pPacket->ChannelInfo.nLadderTime[i] % 10000;

			if( nStartTime <= 0 || nEndTime <= 0 )
			{
				DN_ASSERT(0 ,"Invalid Matching Time");
				continue;
			}

			MatchDuration matchDuration = {0,};
			matchDuration.cStartHour = nStartTime / 100;
			matchDuration.cStartMin = nStartTime % 100;
			matchDuration.cEndHour = nEndTime / 100;
			matchDuration.cEndMin = nEndTime % 100;

			m_VecLadderTime.push_back(matchDuration);
		}
	}

	m_bInit = true;
}

bool CDNDWCSystem::CheckDWCSeason()
{
	if(!m_bInit)
		return false;

	if( m_DWCChannelInfo.cStatus != DWC::STATUS_NORMAL && m_DWCChannelInfo.cStatus != DWC::STATUS_PAUSE)
		return false;

	CTimeSet CurTime;
	__time64_t tCurTime = CurTime.GetTimeT64_LC();
	if( tCurTime < m_DWCChannelInfo.tStartDate || tCurTime > m_DWCChannelInfo.tEndDate )
		return false;

	return true;
}

bool CDNDWCSystem::CheckDWCMatchTime(LadderSystem::MatchType::eCode MatchType)
{
	if( !CheckDWCSeason() )
		return false;

	if( MatchType == LadderSystem::MatchType::_3vs3_DWC )
	{
		if( m_DWCChannelInfo.cStatus != DWC::STATUS_NORMAL || m_bCloseMatchByCheat )
			return false;

		CTimeSet LocalSet;
		int nCurTimeCalcByMin = ((LocalSet.GetHour() * 60) + LocalSet.GetMinute());
		for(int i = 0 ; i < m_VecLadderTime.size() ; i++)
		{
			if( nCurTimeCalcByMin >= (((int)m_VecLadderTime[i].cStartHour * 60) + m_VecLadderTime[i].cStartMin) &&
				nCurTimeCalcByMin < (((int)m_VecLadderTime[i].cEndHour * 60) + m_VecLadderTime[i].cEndMin) )
				return true;
		}
		return false;
	}

	return true;
}

#endif