#include "StdAfx.h"
#include "DNQuestCondition.h"
#include "DNUserBase.h"
#include "ReputationSystemRepository.h"
#include "DNMissionSystem.h"

//--------------------------------------------------------------------------------------------------------
// 퀘스트 조건 관련 코드

QuestCondition::QuestCondition()
{
	nQuestIndex = 0;
}

QuestCondition::~QuestCondition()
{
	for( size_t i = 0 ; i < ConditionList.size() ; i++ )
	{
		ConditionBase* pCB = ConditionList[i];
		SAFE_DELETE(pCB);
	}
	ConditionList.clear();
}

bool QuestCondition::Check( CDNUserBase * pUserBase )
{
	for( size_t i = 0 ; i < ConditionList.size() ; i++ )
	{
		ConditionBase* _Condition =(ConditionList[i]);

		if( _Condition->bUse && _Condition->Check(pUserBase) == false )
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// __HaveNormalItem
//////////////////////////////////////////////////////////////////////////
void __HaveNormalItem::Init(std::wstring& data) 
{
	nItemTableNumber = 0;
	nItemNumber = 0;

	if( data.empty() )
	{
		bUse = false;
		return;
	}

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	if( tokens.size() != 2 )
	{
		bUse = false;
		return;
	}

	nItemTableNumber = _wtoi(tokens[0].c_str()); // 잇어야할 아이템 넘버
	nItemNumber = _wtoi(tokens[1].c_str());       // 있어야할 아이템의 갯수
	bUse = true;
}

bool __HaveNormalItem::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( pUserBase == NULL )
		return false;

	//return true;

	if( pUserBase->GetItem()->GetInventoryItemCount( nItemTableNumber ) >= nItemNumber)			// 내가 그아이템을 몇개 가지고 있는지 체크
	{
		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// __UserLevel
//////////////////////////////////////////////////////////////////////////
void __UserLevel::Init(std::wstring& data) 
{
	nMinUserLevel = 0;
	nMaxUserLevel = 0;

	if( data.empty() )
	{
		bUse = false;
		return;
	}

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	if( tokens.size() != 2 )
	{
		bUse = false;
		return;
	}

	nMinUserLevel = _wtoi(tokens[0].c_str());
	nMaxUserLevel = _wtoi(tokens[1].c_str());
	bUse = true;
}

bool __UserLevel::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( pUserBase == NULL )
		return false;

	int nPlayerLevel = pUserBase->GetLevel();

	if( nMinUserLevel <= nPlayerLevel && nPlayerLevel <= nMaxUserLevel )
		return true;

	if( nMinUserLevel <= nPlayerLevel && nMaxUserLevel == -1 )
		return true; 

	return false;
}

//////////////////////////////////////////////////////////////////////////
// __UserClass
//////////////////////////////////////////////////////////////////////////
void __UserClass::Init(std::wstring& data) 
{
	if( data.empty() )
	{
		bUse = false;
		return;
	}

	ClassIDList.clear();

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	for( size_t i = 0 ; i < tokens.size() ; i++ )
	{
		ClassIDList.push_back( _wtoi(tokens[i].c_str()));
	}
	bUse = true;
}

bool __UserClass::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( ClassIDList.empty() )
		return true;

	if( pUserBase == NULL )
		return false;

	for( size_t i  = 0 ; i < ClassIDList.size() ; i++ )
	{
		if( ClassIDList[i] == pUserBase->GetClassID() )
			return true;
	}

	return false; 
}

//////////////////////////////////////////////////////////////////////////
// __PrevQuest
//////////////////////////////////////////////////////////////////////////
void __PrevQuest::Init(std::wstring& data) 
{
	if( data.empty() )
	{
		bUse = false;
		return;
	}

	PrevQuestList.clear();

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	for( size_t i = 0 ; i < tokens.size() ; i++ )
	{
		PrevQuestList.push_back( _wtoi(tokens[i].c_str()));
	}
	bUse = true;
}

bool __PrevQuest::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( pUserBase == NULL )
		return false;

	for( size_t i  = 0 ; i < PrevQuestList.size() ; i++ )
	{
		if( pUserBase->GetQuest()->IsClearQuest(PrevQuestList[i]) == false )
			return false;
	}

	return true; 
}

//////////////////////////////////////////////////////////////////////////
// __CompleteMission
//////////////////////////////////////////////////////////////////////////
void __CompleteMission::Init(std::wstring& data)
{
	if( data.empty() )
	{
		bUse = false;
		return;
	}

	MissionIDList.clear();

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	for( size_t i = 0 ; i < tokens.size() ; i++ )
	{
		MissionIDList.push_back( _wtoi(tokens[i].c_str()));
	}
	bUse = true;
}

bool __CompleteMission::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( MissionIDList.empty() )
		return true;

	if( pUserBase == NULL )
		return false;

	//for( size_t i  = 0 ; i < MissionIDList.size() ; i++ )
	//{
	//	int nMissionArrayIndex = MissionIDList[ i ] - 1;		// 테이블에 적혀 있는 ItemID 에다 1 을 뺀값이 ArrayIndex 이다.
	//	//CDnMissionTask::MissionInfoStruct* pMissionStruct = CDnMissionTask::GetInstance().GetMissionFromArrayIndex( nMissionArrayIndex );
	//	//ASSERT( pMissionStruct != NULL && "QuestCondition : Invalid Mission Index!!" );
	//	//if( !pMissionStruct || !pMissionStruct->bAchieve ) {
	//	//	return false;
	//	//}
	//	
	//}

	for( size_t i = 0 ; i < MissionIDList.size() ; ++ i)
	{
		int nMissionID		= MissionIDList[i];
		bool bAchieve		= pUserBase->GetMissionSystem()->bIsAchieveMission( nMissionID );
		if( false == bAchieve )
		{
			return false;
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// __Reputation_Favor
//////////////////////////////////////////////////////////////////////////
void __Reputation_Favor::Init(std::wstring& data) 
{
	nNpcID = 0;
	nReputationValue_Min = 0;
	nReputationValue_Max = 0;

	if( data.empty() )
	{
		bUse = false;
		return;
	}

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	if( tokens.size() != 3 )
	{
		bUse = false;
		return;
	}

	nNpcID = _wtoi(tokens[0].c_str()); // 잇어야할 아이템 넘버
	nReputationValue_Min = _wtoi(tokens[1].c_str());       // 있어야할 아이템의 갯수
	nReputationValue_Max = _wtoi(tokens[2].c_str());
	bUse = true;
}

bool __Reputation_Favor::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( pUserBase == NULL ) 
		return false;

#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	REPUTATION_TYPE iFavorValue = pUserBase->GetReputationSystem()->GetNpcReputation( nNpcID, IReputationSystem::NpcFavor );

	if(nReputationValue_Min <= iFavorValue && iFavorValue <=nReputationValue_Max) // 지정된 호감도이상일때만 작동합니다.
		return true;
	else if(nReputationValue_Min <= iFavorValue && nReputationValue_Max == -1)
		return true;
	else if(iFavorValue <=nReputationValue_Max && nReputationValue_Min == -1)
		return true;
#endif	// #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)

	return false;
}

//////////////////////////////////////////////////////////////////////////
// __Reputation_Hatred
//////////////////////////////////////////////////////////////////////////
void __Reputation_Hatred::Init(std::wstring& data) 
{
	nNpcID = 0;
	nReputationValue_Min = 0;
	nReputationValue_Max = 0;

	if( data.empty() )
	{
		bUse = false;
		return;
	}

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	if( tokens.size() != 3 )
	{
		bUse = false;
		return;
	}

	nNpcID = _wtoi(tokens[0].c_str()); // 잇어야할 아이템 넘버
	nReputationValue_Min = _wtoi(tokens[1].c_str());       // 있어야할 아이템의 갯수
	nReputationValue_Max = _wtoi(tokens[2].c_str());
	bUse = true;
}

bool __Reputation_Hatred::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( pUserBase == NULL ) 
		return false;

#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	REPUTATION_TYPE iFavorValue = pUserBase->GetReputationSystem()->GetNpcReputation( nNpcID, IReputationSystem::NpcMalice );

	if(nReputationValue_Min <= iFavorValue && iFavorValue <=nReputationValue_Max) // 지정된 호감도이상일때만 작동합니다.
		return true;
	else if(nReputationValue_Min <= iFavorValue && nReputationValue_Max == -1)
		return true;
	else if(iFavorValue <=nReputationValue_Max && nReputationValue_Min == -1)
		return true;
#endif	// #if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)

	return false;
}

#if defined(PRE_ADD_QUEST_CHECKCAHEITEM)
//////////////////////////////////////////////////////////////////////////
// __HaveCashItem
//////////////////////////////////////////////////////////////////////////
void __HaveCashItem::Init(std::wstring& data) 
{
	nItemTableNumber = 0;
	nItemNumber = 0;

	if( data.empty() )
	{
		bUse = false;
		return;
	}

	std::vector<std::wstring> tokens;
	TokenizeW(data, tokens, L":");
	if( tokens.size() != 2 )
	{
		bUse = false;
		return;
	}

	nItemTableNumber = _wtoi(tokens[0].c_str()); // 잇어야할 아이템 넘버
	nItemNumber = _wtoi(tokens[1].c_str());       // 있어야할 아이템의 갯수
	bUse = true;
}

bool __HaveCashItem::Check(CDNUserBase * pUserBase)
{
	if( !bUse )
		return true;

	if( pUserBase == NULL )
		return false;

	if( pUserBase->GetItem()->GetCashItemCountByItemID( nItemTableNumber ) >= nItemNumber)			// 내가 그아이템을 몇개 가지고 있는지 체크
	{
		return true;
	}

	return false;
}
#endif