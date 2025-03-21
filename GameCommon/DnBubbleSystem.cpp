#include "StdAfx.h"
#include "DnBubbleSystem.h"
#include "DnBubbleConditionChecker.h"
#include "DnObserverEventMessage.h"
#include "DnBubbleEventHandler.h"
#include "DnBubble.h"
#include "DnTableDB.h"
#include "DnPlayerActor.h"
#include "Stream.h"

#ifdef _CLIENT
#include "../Client/DragonNest/DnInterface.h"
#endif

namespace BubbleSystem
{

CDnBubbleSystem::S_DEFINED_BUBBLE_EVENT::~S_DEFINED_BUBBLE_EVENT( void )
{
	SAFE_DELETE_PVEC( vlpConditions );
	SAFE_DELETE_PVEC( vlpEventHandlers );
}


CDnBubbleSystem::CDnBubbleSystem( void )
{

}

CDnBubbleSystem::~CDnBubbleSystem( void )
{
	RemoveAllBubbles( false );

	// 읽어들였던 정보 모두 해제.
	SAFE_DELETE_PVEC( m_vlpDefinedBubbleEvent );

	//map<int, IDnBubbleEventHandler*>::iterator iter = m_mapBubbleRemoveEventHandlers.begin();
	//for( iter; iter != m_mapBubbleRemoveEventHandlers.end(); ++iter )
	//{
	//	SAFE_DELETE( iter->second );
	//}
}


void CDnBubbleSystem::Initialize( DnActorHandle hActor )
{	
	// 이벤트 처리는 게임서버에서만 처리. 클라는 패킷만 받는다.
#ifdef _GAMESERVER
	// 현재 버블 시스템은 플레이어만 가능.
	if( false == hActor->IsPlayerActor() )
		return;

	m_hActor = hActor;

	// 해당 직업에 해당되는 버블 테이블 정보를 긁어와서 데이터를 구성해 둠.
	DNTableFileFormat* pBubbleTable = GetDNTable( CDnTableDB::TSKILLBUBBLE );
	char acBuffer[ 512 ] = { 0 };
	int iItemCount = pBubbleTable->GetItemCount();
	for( int iIndex = 0; iIndex < iItemCount; ++iIndex )
	{
		int iBubbleTableID = pBubbleTable->GetItemID( iIndex );

		// 직업 제한이 있는지 확인. 추후에 많아지면 넣자.
		// 0 이면 직접 제한 없음. 직업 제한이 존재한다면 직업 히스토리를 뒤져서 해당되는 자료만 담는다.
		int iNeedJobCode = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, "_NeedJob" )->GetInteger();
		if( 0 < iNeedJobCode )
		{
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
			bool bAvailableJob = pPlayerActor->IsPassJob( iNeedJobCode );
			if( false == bAvailableJob )
			{
				// 필요 직업에 충족되지 않음.
				continue;
			}
		}

		// 새로운 버블 이벤트 정의 구조체 생성.
		S_DEFINED_BUBBLE_EVENT* pNewBubbleInfo = new S_DEFINED_BUBBLE_EVENT;
		pNewBubbleInfo->iTableID = iBubbleTableID;

		// 조건 정보를 구성.
		for( int i = 0; i < 5; ++i )
		{
			sprintf_s( acBuffer, "_ConditionType%d", i+1 );
			int iConditionType = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, acBuffer )->GetInteger();

			sprintf_s( acBuffer, "_ConditionFactor%d", i+1 );
			string strArgument = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, acBuffer )->GetString();

			// 결과가 NULL 이라면 테이블에 조건 타입이 0 으로 셋팅되어있는 것임. 즉 비어있음.
			IDnConditionChecker* pConditionChecker = IDnConditionChecker::Create( iConditionType, strArgument.c_str() );
			if( pConditionChecker )
			{
				pNewBubbleInfo->vlpConditions.push_back( pConditionChecker );

				// 검색 시간 단축을 위해 조건 타입에 따라 특정 이벤트와 의미있는 버블 이벤트 정보를 여기서 연결해 놓는다.
				// 각 조건별로 연관있는 이벤트 메시지 타입별로 정렬해놓는 셈.
				int iEventType = _GetRelatedEventMessageType( iConditionType );
				if( NONE_BUBBLE_EVENT_MESSAGE != iEventType )
					m_mmapDefinedByEvent.insert( make_pair(iEventType, pNewBubbleInfo) );
				//////////////////////////////////////////////////////////////////////////
			}
			else
				break;
		}

		// 이벤트 핸들러 정보를 구성.
		for( int i = 0; i < 5; ++i )
		{
			sprintf_s( acBuffer, "_BubbleType%d", i+1 );
			int iEventHandlerType = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, acBuffer )->GetInteger();

			sprintf_s( acBuffer, "_BubbleFactor%d", i+1 );
			string strArgument = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, acBuffer )->GetString();

			IDnBubbleEventHandler* pEventHandler = IDnBubbleEventHandler::Create( iEventHandlerType, hActor, strArgument.c_str() );
			if( pEventHandler )
			{
				pNewBubbleInfo->vlpEventHandlers.push_back( pEventHandler );
				_OnCreateEventHandler( pBubbleTable, iBubbleTableID, pEventHandler, strArgument.c_str() );
			}
			else
				break;
		}

		m_vlpDefinedBubbleEvent.push_back( pNewBubbleInfo );
	}
#endif
}

void CDnBubbleSystem::_OnCreateEventHandler( DNTableFileFormat* pBubbleTable, int iBubbleTableID, IDnBubbleEventHandler* pEventHandler, const char* pArgument )
{
#ifdef _GAMESERVER
	// 버블 추가 이벤트 핸들러라면 아이콘 정보도 추가해준다. 나중에 아이콘 정보도 클라로 보내준다.
	if( BUBBLE_HANDLER::GETTING_BUBBLE == pEventHandler->GetType() )
	{
		int iIconIndex = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, "_IconIndex" )->GetInteger();
		static_cast<CDnGettingBubbleHandler*>(pEventHandler)->SetIconIndex( iIconIndex );
	}
#endif // #ifdef _GAMESERVER


	//// 버블로 인한 상태효과 추가된 것들을 제거할 때 따로 버블 시스템에서 직접 해제한다.
	//if( BUBBLE_HANDLER::ADD_STATE_EFFECT == pEventHandler->GetType() )
	//{
	//	// 버블 삭제시, 해당 상태효과도 제거하도록 조건 타입 및 이벤트 핸들러 추가.
	//	IDnBubbleEventHandler* pRemoveEventHandler = IDnBubbleEventHandler::Create( BUBBLE_HANDLER::REMOVE_STATE_EFFECT, m_hActor, pArgument );
	//	m_mapBubbleRemoveEventHandlers[ static_cast<CDnAddStateEffectHandler*>(pEventHandler)->GetBubbleTypeID() ] = pRemoveEventHandler;
	//}
}

// 외부에서 메시지를 보내주는 것은 검색시간 단축을 위해 이벤트를 미리 연관지어 놓는다.
// EVENT_BUBBLE_ON_USE_SKILL 메시지가 오면 ON_USE_SKILL 조건으로 정렬된 이벤트 정의 정보로 검색한다.
// 따라서 여기에 정리되지 않은 이벤트들이 반드시 하나씩은 있어야 이벤트가 처리된다.
// 추후 이 부분은 없앨 수도 있다.. 
int CDnBubbleSystem::_GetRelatedEventMessageType( int iConditionType )
{
	int iResultMessageType = NONE_BUBBLE_EVENT_MESSAGE;

	switch( iConditionType )
	{
		case ON_USE_SKILL:
			{
				iResultMessageType = EVENT_BUBBLE_ON_USE_SKILL;
			}
			break;

		case BLOCK_SUCCESS:
			{
				iResultMessageType = EVENT_BUBBLE_BLOCK_SUCCESS;
			}
			break;

		case BUBBLE_COUNT_UPDATED:
			{
				iResultMessageType = EVENT_BUBBLE_COUNT_UPDATED;
			}
			break;

		case PARRING_SUCCESS:
			{
				iResultMessageType = EVENT_BUBBLE_PARRING_SUCCESS;
			}
			break;

		case COOLTIME_PARRING_SUCCESS:
			{
				iResultMessageType = EVENT_BUBBLE_COOLTIME_PARRING_SUCCESS;
			}
			break;

		case DO_NORMAL_ATTACK:
			{
				iResultMessageType = EVENT_ONCHANGEACTION;
			}
			break;

		case PLAYER_KILL_TARGET_ON_GHOUL_MODE:
			{
				iResultMessageType = EVENT_PLAYER_KILL_TARGET;
			}
			break;

		case ON_USE_SKILL_WITH_SPECIFIC_SKILLLEVEL:
			{
				iResultMessageType = EVENT_BUBBLE_ON_USE_SKILL;
			}

		case ONCRITICALHIT:
			{
				iResultMessageType = EVENT_ONCRITICALHIT;
			}
			break;
	}

	return iResultMessageType;
}


void CDnBubbleSystem::Clear( void )
{
	RemoveAllBubbles();
}


void CDnBubbleSystem::SetDurationTime( int iBubbleTypeID, float fDurationTime )
{
	// 나머지 모든 버블의 지속시간을 새로 얻는 버블의 지속시간으로 변경함.
	deque<CDnBubble*>& dqBubbles = m_mapBubblesByTypeID[ iBubbleTypeID ];
	for( int i = 0; i < (int)dqBubbles.size(); ++i )
	{
		CDnBubble* pBubble = dqBubbles.at( i );
		pBubble->SetDurationTime( fDurationTime );
	}
}


void CDnBubbleSystem::_CreateBubble( const S_CREATE_BUBBLE& Info )
{
	CDnBubble* pNewBubble = _CreateNewBubble( Info );
	_OnCreatedBubble( pNewBubble );
}


CDnBubble* CDnBubbleSystem::_CreateNewBubble( const S_CREATE_BUBBLE &Info )
{
	CDnBubble* pNewBubble = new CDnBubble;
	pNewBubble->SetTypeID( Info.iBubbleTypeID );
	pNewBubble->SetIconIndex( Info.iIconIndex );

	m_mapBubblesByTypeID[ Info.iBubbleTypeID ].push_front( pNewBubble );
	SetDurationTime( Info.iBubbleTypeID, Info.fDurationTime );

	return pNewBubble;
}


void CDnBubbleSystem::_OnCreatedBubble( CDnBubble* pCreatedBubble )
{	
	// 버블 갯수 갱신 이벤트 처리.
	if( pCreatedBubble )
	{
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_COUNT_UPDATED ) );
		pEvent->SetBubbleTypeID( pCreatedBubble->GetTypeID() );
		OnEvent( pEvent );

	}
}

//void CDnBubbleSystem::_OnRemoveBubble( CDnBubble* pBubbleToRemove )
//{
//	// TODO: 테이블에 정의된대로 하나의 버블이 제거 될 때 상태효과를 지우거나 함..
//	// 버블로 상태효과가 주어지는 거라면 남은 갯수 기준으로 기존 갯수 기준의 상태효과 지우고 상태효과 새로 추가해야 함.
//
//}

void CDnBubbleSystem::_OnRemovedBubbles( int iBubbleTypeID, int iCount )
{
	//map<int, IDnBubbleEventHandler*>::iterator iter = m_mapBubbleRemoveEventHandlers.find( iBubbleTypeID );
	//if( m_mapBubbleRemoveEventHandlers.end() != iter )
	//{
	//	iter->second->ProcessEvent( this, NULL );
	//}

	// 버블 갯수 갱신 이벤트 처리.
	boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_COUNT_UPDATED ) );
	pEvent->SetBubbleTypeID( iBubbleTypeID );
	OnEvent( pEvent );
}


int CDnBubbleSystem::GetBubbleCountByTypeID( int iBubbleTypeID )
{
	int iResult = 0;

	BubblesByTypeID_iter iter= m_mapBubblesByTypeID.find( iBubbleTypeID );
	if( m_mapBubblesByTypeID.end() != iter )
	{
		iResult = (int)iter->second.size();
	}

	return iResult;	
}

void CDnBubbleSystem::GetAllAppliedBubbles( /*OUT*/ vector<S_BUBBLE_INFO>& vlBubbleInfos )
{
	BubblesByTypeID_iter iter = m_mapBubblesByTypeID.begin();
	for( iter; iter != m_mapBubblesByTypeID.end(); ++iter )
	{
		// 우선은 각 버블별로 지속시간이 남은 것은 무시하고 먼저 추가된 버블 순으로 삭제한다.
		CDnBubble* pBubble = iter->second.front();
		S_BUBBLE_INFO Info;
		Info.iBubbleTypeID = pBubble->GetTypeID();
		Info.iCount = (int)iter->second.size();
		Info.iIconIndex = pBubble->GetIconIndex();
		Info.fDurationTime = pBubble->GetDurationTime();
		Info.fRemainTime = pBubble->GetRemainTime();

		vlBubbleInfos.push_back( Info );
#ifdef _CLIENT
		//GetInterface().SetBubble(Info.iCount,Info.fRemainTime,Info.fDurationTime);
#endif
	}
}


void CDnBubbleSystem::AddBubble( const S_CREATE_BUBBLE& Info )
{
	_CreateBubble( Info );
}

void CDnBubbleSystem::AddBubbleAndCountRevision( const S_CREATE_BUBBLE& Info )
{
	_CreateBubble( Info );
	RevisionBubbleCount( Info );
}


void CDnBubbleSystem::RevisionBubbleCount( const S_CREATE_BUBBLE& Info )
{
	// 서버의 갯수로 버블 갯수를 맞춰준다.
    std::deque<CDnBubble*>& dqBubbles = m_mapBubblesByTypeID[ Info.iBubbleTypeID ];
	int iClientBubbleCount = (int)dqBubbles.size();
	if( iClientBubbleCount < Info.iServerBubbleCount )
	{
		int iAddCount = Info.iServerBubbleCount - iClientBubbleCount;
		for( int i = 0; i < iAddCount; ++i )
			_CreateNewBubble( Info );
	}
	else
	if( iClientBubbleCount > Info.iServerBubbleCount )
	{
		int iRemoveCount = iClientBubbleCount - Info.iServerBubbleCount;
		RemoveBubbleByTypeID( Info.iBubbleTypeID, iRemoveCount );
	}
}


void CDnBubbleSystem::RemoveBubbleByTypeID( int iBubbleTypeID, int iRemoveCount )
{
	BubblesByTypeID_iter iter= m_mapBubblesByTypeID.find( iBubbleTypeID );
	if( m_mapBubblesByTypeID.end() != iter )
	{
		// 우선은 각 버블별로 지속시간이 남은 것은 무시하고 먼저 추가된 버블 순으로 삭제한다.
        std::deque<CDnBubble*>& dqpBubbles = iter->second;
		for( int i = 0; i < iRemoveCount; ++i )
		{
			// 추후 필요하면 추가.
			//_OnRemoveBubble( &dqpBubbles.back() );
			dqpBubbles.pop_back();
		}

		// 헤당 타입의 버블이 모두 없어지면 컨테이너 제거.
		if( dqpBubbles.empty() )
			m_mapBubblesByTypeID.erase( iter );

		_OnRemovedBubbles( iBubbleTypeID, iRemoveCount );
	}
}


void CDnBubbleSystem::RemoveAllBubbleByTypeID( int iBubbleTypeID )
{
	BubblesByTypeID_iter iter= m_mapBubblesByTypeID.find( iBubbleTypeID );
	if( m_mapBubblesByTypeID.end() != iter )
	{
		int iRemovedBubbleCount = (int)iter->second.size();		

		SAFE_DELETE_PVEC( iter->second );
		m_mapBubblesByTypeID.erase( iter );

		_OnRemovedBubbles( iBubbleTypeID, iRemovedBubbleCount );
	}
}


void CDnBubbleSystem::RemoveAllBubbles( bool bHandleRemoveEvent /*= true*/ )
{
	//SAFE_DELETE_PVEC( m_vlpBubbles );
	BubblesByTypeID_iter iter = m_mapBubblesByTypeID.begin();
	for( iter; iter != m_mapBubblesByTypeID.end(); ++iter )
	{
		int iBubbleTypeID = iter->first;
		int iRemovedBubbleCount = (int)iter->second.size();		

		SAFE_DELETE_PVEC( iter->second );

		// 소멸자에서 호출된다면 버블 소멸 이벤트 처리를 하지 않도록 한다.
		if( bHandleRemoveEvent )
			_OnRemovedBubbles( iter->first, iRemovedBubbleCount );
	}

	//m_vlpBubbles.clear();
	m_mapBubblesByTypeID.clear();
}


CDnBubble* CDnBubbleSystem::GetBubble( int iBubbleTypeID )
{
	CDnBubble* pResult = NULL;

	BubblesByTypeID_iter iter= m_mapBubblesByTypeID.find( iBubbleTypeID );
	if( m_mapBubblesByTypeID.end() != iter )
	{
		std::deque<CDnBubble*>& dqBubbles = iter->second;
		if( dqBubbles.empty() )
		{
			pResult = dqBubbles.front();
		}
	}

	return pResult;
}


void CDnBubbleSystem::Process( LOCAL_TIME LocalTime, float fDelta )
{
	BubblesByTypeID_iter iter = m_mapBubblesByTypeID.begin();
	for( iter; iter != m_mapBubblesByTypeID.end(); )
	{
        std::deque<CDnBubble*>& dqpBubbles = iter->second;
		int iNumBubbles = (int)dqpBubbles.size();
        std::deque<CDnBubble*>::iterator iterDeque = dqpBubbles.begin();
		for( iterDeque; iterDeque != iter->second.end(); )
		{
			CDnBubble* pBubble = *iterDeque;
			pBubble->Process( LocalTime, fDelta );

			if( pBubble->IsEnd() )
			{	
				iterDeque = dqpBubbles.erase( iterDeque );
				continue;
			}

			++iterDeque;
		}

		int iNumRemoved = iNumBubbles - (int)dqpBubbles.size();
		if( 0 < iNumRemoved )
			_OnRemovedBubbles( iter->first, iNumRemoved );

		if( dqpBubbles.empty() )
			iter = m_mapBubblesByTypeID.erase( iter );
		else
			++iter;
	}
}


void CDnBubbleSystem::OnEvent( boost::shared_ptr<::IDnObserverNotifyEvent>& pEvent )
{
	IDnObserverNotifyEvent* pEventObject = pEvent.get();

	if( pEvent == NULL )
		return;

    std::pair<DefinedByEventMMap_iter, DefinedByEventMMap_iter> iter_pair = m_mmapDefinedByEvent.equal_range( pEvent->GetEventType() );
	DefinedByEventMMap_iter iter = iter_pair.first;
	for( iter; iter != iter_pair.second; ++iter )
	{
		S_DEFINED_BUBBLE_EVENT* pDefinedEventInfo = iter->second;

		// 모든 조건이 충족하는가.
		bool bResult = true;
		int iNumConditions = (int)pDefinedEventInfo->vlpConditions.size();
		for( int iCondition = 0; iCondition < iNumConditions; ++iCondition )
		{
			IDnConditionChecker* pCondition = pDefinedEventInfo->vlpConditions.at( iCondition );
			bResult = pCondition->IsSatisfy( this, pEventObject );
			if( false == bResult )
				break;
		}

		// 조건이 하나라도 충족되지 않으면 이 정의된 버블 정보완 무관한 내용..
		if( false == bResult )
			continue;

		// 모든 조건이 충족됨. 데이터에 정의된대로 실행..
		int iNumEventHandler = (int)pDefinedEventInfo->vlpEventHandlers.size();
		for( int iEventHandler = 0; iEventHandler < iNumEventHandler; ++iEventHandler )
		{
			IDnBubbleEventHandler* pEventHandler = pDefinedEventInfo->vlpEventHandlers.at( iEventHandler );
			pEventHandler->ProcessEvent( this, pEventObject );
		}
	}
}

#ifdef _GAMESERVER
void CDnBubbleSystem::AddBubbleStateBlow( int iBubbleTypeID, int iBlowID )
{
	m_mapBubbleStateBlowsByBubbleType[ iBubbleTypeID ].push_back( iBlowID );
}

void CDnBubbleSystem::RemoveBubbleStateBlow( int iBubbleTypeID )
{
	map<int, vector<int> >::iterator iter = m_mapBubbleStateBlowsByBubbleType.find( iBubbleTypeID );
	if( m_mapBubbleStateBlowsByBubbleType.end() != iter )
	{
		vector<int>& vlBlowIDs = iter->second;
		for( int i = 0; i < (int)vlBlowIDs.size(); ++i )
		{
			int iBlowID = vlBlowIDs.at( i );
			m_hActor->CmdRemoveStateEffectFromID( iBlowID );		// 서버에서 한 프레임 있다가 삭제될텐데 확인 해봐야 함.
		}
		iter->second.clear();
	}
}
#else
void CDnBubbleSystem::CreateBubbleFromPacketStream( ::CPacketCompressStream* pStream )
{
	if( NULL == pStream )
		return;

	S_CREATE_BUBBLE Info;
	
	pStream->Read( &Info.iBubbleTypeID, sizeof(int) );
	pStream->Read( &Info.iIconIndex, sizeof(int) );
	pStream->Read( &Info.fDurationTime, sizeof(float) );
	pStream->Read( &Info.iServerBubbleCount, sizeof(int) );

	this->AddBubbleAndCountRevision( Info );
}

void CDnBubbleSystem::RefreshBubbleDurationTimeFromPacketStream( ::CPacketCompressStream* pStream )
{
	if( NULL == pStream )
		return;

	S_CREATE_BUBBLE Info;

	pStream->Read( &Info.iBubbleTypeID, sizeof(int) );
	pStream->Read( &Info.iIconIndex, sizeof(int) );
	pStream->Read( &Info.fDurationTime, sizeof(float) );
	pStream->Read( &Info.iServerBubbleCount, sizeof(int) );

	SetDurationTime( Info.iBubbleTypeID, Info.fDurationTime );
	RevisionBubbleCount( Info );



}
#endif // #ifdef _GAMESERVER

} // namespace BubbleSystem