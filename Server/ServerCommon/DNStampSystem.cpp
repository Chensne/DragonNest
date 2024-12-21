#include "StdAfx.h"

#if defined( PRE_ADD_STAMPSYSTEM )

#include "DNStampSystem.h"
#include "DNGameDataManager.h"
#include "DNUserSession.h"
#include "DNDBConnection.h"
#include "DNMailSender.h"
#include "DNMissionSystem.h"
#include "DNMissionScheduler.h"

CDNStampSystem::CDNStampSystem( CDNUserSession * pUserSession )
{
	m_pUserSession = pUserSession;
	m_nWeekDay = 0;
	m_pStampTableData = NULL;
	m_nCheckDailyTime = 0;
}

CDNStampSystem::~CDNStampSystem()
{
	m_MapCompleteSlotSet.clear();
}

INT64 CDNStampSystem::_ConvertToStampTableTime( time_t tTime )
{
	tm pTime = *localtime( &tTime );

	INT64 biConvertTime = 0;
	INT64 biYearTemp = 1900 + pTime.tm_year;
	biConvertTime += ( biYearTemp * 100000000 );
	biConvertTime += ( ( 1 + pTime.tm_mon ) * 1000000 );
	biConvertTime += ( pTime.tm_mday * 10000 );
	biConvertTime += ( pTime.tm_hour * 100 );
	biConvertTime += pTime.tm_min;

	return biConvertTime;
}

bool CDNStampSystem::_CheckQuestAllComplete( const std::set<int> & rSetAssignmentID ) const
{
	for each( const std::set<int>::value_type nAssignmentID in rSetAssignmentID )
	{
		if( !m_pUserSession->GetQuest()->IsClearQuest( nAssignmentID ) )
		{
			return false;
		}
	}
	return true;
}

bool CDNStampSystem::_CheckMissionComplete( const std::set<int> & rSetAssignmentID, int nArrayIndex ) const
{
	if( !CDNMissionScheduler::IsActive() || nArrayIndex >= DAILYMISSIONMAX )
		return false;

	int nLevel = m_pUserSession->GetLevel();
	int nClassID = m_pUserSession->GetClassID();
	int nMissionID = m_pUserSession->GetMissionData()->DailyMission[nArrayIndex].nMissionID;

	for each( const std::set<int>::value_type nMissionGroupID in rSetAssignmentID )
	{
		bool bCheckGroup = CDNMissionScheduler::GetInstance().IsDailyMissionGroup( nLevel, nClassID, nMissionGroupID, nMissionID);
		if( true == bCheckGroup )
		{
			return true;
		}
	}
	return false;
}

bool CDNStampSystem::_CheckAndInsertSlot( int nChallengeIndex, int nWeekDay )
{
	SlotSet & rSlotSet = m_MapCompleteSlotSet[nChallengeIndex];
	pair<SlotSet::iterator, bool> result = rSlotSet.insert(nWeekDay);
	if(false == result.second)
	{
		return false;
	}

	m_pUserSession->SendStampSystemAddComplete( nChallengeIndex, nWeekDay );
	return true;
}

void CDNStampSystem::_CheckAndGiveReward( int nChallengeIndex, const StampSystem::TStampChallenge & rChallengeData ) const
{
	std::map<int, SlotSet>::const_iterator iter = m_MapCompleteSlotSet.find(nChallengeIndex);
	if( m_MapCompleteSlotSet.end() == iter )
	{
		return;
	}

	int nRewardMailID = 0;
	size_t nCompleteSlotCount = iter->second.size();

	switch( nCompleteSlotCount )
	{
	case StampSystem::RewardCount::Day3: // 4일차 보상이 3일차로 바뀜
		{
			nRewardMailID = rChallengeData.nMailID4; //변수명 정리는 추후에 하기로 하였음
		}
		break;
	case StampSystem::RewardCount::Day5: // 7일차 보상이 5일차로 바뀜
		{
			nRewardMailID = rChallengeData.nMailID7; //변수명 정리는 추후에 하기로 하였음
		}
		break;
	}

	if( 0 < nRewardMailID )
	{
#if defined(PRE_SPECIALBOX)
		CDNMailSender::Process(m_pUserSession, nRewardMailID, DBDNWorldDef::PayMethodCode::Stamp, nRewardMailID);
#endif	// #if defined(PRE_SPECIALBOX)
	}
}

void CDNStampSystem::_ClearCompleteSlot()
{
	m_MapCompleteSlotSet.clear();
	m_pUserSession->SendStampSystemClear();
}

void CDNStampSystem::_UpdateWeekDay( time_t tTime )
{
	tm pTime = *localtime( & tTime );
	int nCurrentTime = ( pTime.tm_hour * 100 ) + pTime.tm_min;

	m_nWeekDay = pTime.tm_wday + 1;
	if( nCurrentTime < m_nCheckDailyTime )
	{
		if( 1 < m_nWeekDay )
			-- m_nWeekDay;
		else
			m_nWeekDay = StampSystem::Common::MaxWeekDayCount;
	}
}

void CDNStampSystem::LoadUserData()
{
	time_t tTime;
	time( & tTime );
	
	tm pTime = *localtime( & tTime );

	DNTableFileFormat * pSox = GetDNTable( CDnTableDB::TSCHEDULE );
	if( pSox )
	{
		int nItemID = CDNSchedule::AlarmDailyStamp + 1;
		if( pSox->IsExistItem( nItemID ) ) 
		{
			m_nCheckDailyTime = ( pSox->GetFieldFromLablePtr( nItemID, "_Hour" )->GetInteger() * 100 ) + pSox->GetFieldFromLablePtr( nItemID, "_Minute" )->GetInteger();

			_UpdateWeekDay( tTime );
		}
	}

	INT64 biCurrentTime = _ConvertToStampTableTime( tTime );
	m_pStampTableData = g_pDataManager->GetStampTableData( biCurrentTime );

	if( 0 < m_nWeekDay && m_pStampTableData )
	{
		m_pUserSession->RegisterSchedule( CDNSchedule::AlarmDailyStamp, tTime );
	}
}

void CDNStampSystem::InitCompleteSlot( const StampSystem::StampCompleteInfo * pCompleteInfo )
{
	m_MapCompleteSlotSet.clear();

	for( int i = 0 ; i < StampSystem::Common::MaxStampSlotCount ; ++ i )
	{
		int nChallengeIndex = pCompleteInfo[i].cChallengeIndex;
		int nWeekDay = pCompleteInfo[i].nWeekDay;

		if( 0 == nChallengeIndex || 0 == nWeekDay )
		{
			break;
		}

		if( nChallengeIndex > StampSystem::Common::MaxChallengeCount || nWeekDay > StampSystem::Common::MaxWeekDayCount )
		{
			continue;
		}

		m_MapCompleteSlotSet[ nChallengeIndex ].insert( nWeekDay );
	}
}

void CDNStampSystem::ResetDailyData( time_t tCurrentTime )
{
	_UpdateWeekDay( tCurrentTime );
	
	m_pUserSession->SendStampSystemChangeWeekDay( m_nWeekDay );
}

void CDNStampSystem::ResetWeeklyData( time_t tCurrentTime )
{
	_ClearCompleteSlot();

	INT64 biCurrentTime = _ConvertToStampTableTime( tCurrentTime );

	m_pStampTableData = g_pDataManager->GetStampTableData( biCurrentTime );
	if( m_pStampTableData )
	{
		m_pUserSession->SendStampSystemChangeTable( m_pStampTableData->biStartTime );
		m_pUserSession->SendStampSystemChangeWeekDay( m_nWeekDay );
	}

	m_pUserSession->GetDBConnection()->QueryInitCompleteChallenge( m_pUserSession );
}

void CDNStampSystem::OnCompleteQuest( int nQuestID )
{
	if( NULL == m_pStampTableData )
		return;

	int nLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
	if( nLevelLimit > m_pUserSession->GetLevel() )
		return;

	for( int i = 0 ; i < m_pStampTableData->vChallengeList.size() ; ++ i )
	{
		const StampSystem::TStampChallenge & rChallengeData = m_pStampTableData->vChallengeList[i];
		if( StampSystem::ChallengeType::DailyQuest != rChallengeData.cType )
			continue;

		const std::set<int> & rSetAssignmentID = m_pStampTableData->vChallengeList[i].setAssignmentID;
		std::set<int>::const_iterator iter = rSetAssignmentID.find( nQuestID );
		if( iter == rSetAssignmentID.end() )
			continue;

		if( !_CheckQuestAllComplete( rSetAssignmentID ) )
			continue;

		int nChallengeIndex = i + 1;
		if( !_CheckAndInsertSlot( nChallengeIndex, m_nWeekDay ) )
			continue;

		m_pUserSession->GetDBConnection()->QueryAddCompleteChallenge( m_pUserSession, nChallengeIndex, m_nWeekDay );

		_CheckAndGiveReward( nChallengeIndex, rChallengeData );
	}
}

void CDNStampSystem::OnCompleteDailyMission( int nArrayIndex )
{
	if( NULL == m_pStampTableData )
		return;

	int nLevelLimit = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
	if( nLevelLimit > m_pUserSession->GetLevel() )
		return;

	for( int i = 0 ; i < m_pStampTableData->vChallengeList.size() ; ++ i )
	{
		const StampSystem::TStampChallenge & rChallengeData = m_pStampTableData->vChallengeList[i];
		if( StampSystem::ChallengeType::DailyMission != rChallengeData.cType )
			continue;

		const std::set<int> & rSetAssignmentID = m_pStampTableData->vChallengeList[i].setAssignmentID;

		if( !_CheckMissionComplete( rSetAssignmentID, nArrayIndex ) )
			continue;

		int nChallengeIndex = i + 1;
		if( !_CheckAndInsertSlot( nChallengeIndex, m_nWeekDay ) )
			continue;

		m_pUserSession->GetDBConnection()->QueryAddCompleteChallenge( m_pUserSession, nChallengeIndex, m_nWeekDay );

		_CheckAndGiveReward( nChallengeIndex, rChallengeData );
	}
}

void CDNStampSystem::SendInitData()
{
	if( NULL == m_pStampTableData )
		return;

	bool bTempCompleteData[StampSystem::Common::MaxStampSlotCount];
	memset( bTempCompleteData, 0, sizeof(bTempCompleteData) );

	for( std::map<int,SlotSet>::const_iterator iter = m_MapCompleteSlotSet.begin() ; iter != m_MapCompleteSlotSet.end() ; ++ iter )
	{
		int nChallengeIndex = iter->first;
		const SlotSet & rSlotSet = iter->second;

		for each( const SlotSet::value_type & nWeekDay in rSlotSet )
		{
			if( nChallengeIndex < 1 || nChallengeIndex > StampSystem::Common::MaxChallengeCount || nWeekDay < 1 || nWeekDay > StampSystem::Common::MaxWeekDayCount )
				continue;

			bTempCompleteData[((nChallengeIndex - 1) * StampSystem::Common::MaxWeekDayCount) + (nWeekDay - 1)] = true;
		}
	}

	m_pUserSession->SendStampSystemInit( m_nWeekDay, m_pStampTableData->biStartTime, bTempCompleteData );
}

void CDNStampSystem::ForceAddCompleteSlot( int nChallengeIndex, int nWeekDay )
{
	if( NULL == m_pStampTableData )
		return;

	if( nChallengeIndex == 0 || 
		nChallengeIndex > m_pStampTableData->vChallengeList.size() || 
		nWeekDay == 0 || 
		nWeekDay > StampSystem::Common::MaxWeekDayCount )
	{
		return;
	}

	if( !_CheckAndInsertSlot( nChallengeIndex, nWeekDay ) )
		return;

	m_pUserSession->GetDBConnection()->QueryAddCompleteChallenge( m_pUserSession, nChallengeIndex, nWeekDay );

	_CheckAndGiveReward( nChallengeIndex, m_pStampTableData->vChallengeList[ nChallengeIndex - 1 ] );
}

void CDNStampSystem::ForceClearCompleteSlot()
{
	_ClearCompleteSlot();

	m_pUserSession->GetDBConnection()->QueryInitCompleteChallenge( m_pUserSession );	
}

#endif // #if defined( PRE_ADD_STAMPSYSTEM )