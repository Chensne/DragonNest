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

	// �о�鿴�� ���� ��� ����.
	SAFE_DELETE_PVEC( m_vlpDefinedBubbleEvent );

	//map<int, IDnBubbleEventHandler*>::iterator iter = m_mapBubbleRemoveEventHandlers.begin();
	//for( iter; iter != m_mapBubbleRemoveEventHandlers.end(); ++iter )
	//{
	//	SAFE_DELETE( iter->second );
	//}
}


void CDnBubbleSystem::Initialize( DnActorHandle hActor )
{	
	// �̺�Ʈ ó���� ���Ӽ��������� ó��. Ŭ��� ��Ŷ�� �޴´�.
#ifdef _GAMESERVER
	// ���� ���� �ý����� �÷��̾ ����.
	if( false == hActor->IsPlayerActor() )
		return;

	m_hActor = hActor;

	// �ش� ������ �ش�Ǵ� ���� ���̺� ������ �ܾ�ͼ� �����͸� ������ ��.
	DNTableFileFormat* pBubbleTable = GetDNTable( CDnTableDB::TSKILLBUBBLE );
	char acBuffer[ 512 ] = { 0 };
	int iItemCount = pBubbleTable->GetItemCount();
	for( int iIndex = 0; iIndex < iItemCount; ++iIndex )
	{
		int iBubbleTableID = pBubbleTable->GetItemID( iIndex );

		// ���� ������ �ִ��� Ȯ��. ���Ŀ� �������� ����.
		// 0 �̸� ���� ���� ����. ���� ������ �����Ѵٸ� ���� �����丮�� ������ �ش�Ǵ� �ڷḸ ��´�.
		int iNeedJobCode = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, "_NeedJob" )->GetInteger();
		if( 0 < iNeedJobCode )
		{
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
			bool bAvailableJob = pPlayerActor->IsPassJob( iNeedJobCode );
			if( false == bAvailableJob )
			{
				// �ʿ� ������ �������� ����.
				continue;
			}
		}

		// ���ο� ���� �̺�Ʈ ���� ����ü ����.
		S_DEFINED_BUBBLE_EVENT* pNewBubbleInfo = new S_DEFINED_BUBBLE_EVENT;
		pNewBubbleInfo->iTableID = iBubbleTableID;

		// ���� ������ ����.
		for( int i = 0; i < 5; ++i )
		{
			sprintf_s( acBuffer, "_ConditionType%d", i+1 );
			int iConditionType = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, acBuffer )->GetInteger();

			sprintf_s( acBuffer, "_ConditionFactor%d", i+1 );
			string strArgument = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, acBuffer )->GetString();

			// ����� NULL �̶�� ���̺� ���� Ÿ���� 0 ���� ���õǾ��ִ� ����. �� �������.
			IDnConditionChecker* pConditionChecker = IDnConditionChecker::Create( iConditionType, strArgument.c_str() );
			if( pConditionChecker )
			{
				pNewBubbleInfo->vlpConditions.push_back( pConditionChecker );

				// �˻� �ð� ������ ���� ���� Ÿ�Կ� ���� Ư�� �̺�Ʈ�� �ǹ��ִ� ���� �̺�Ʈ ������ ���⼭ ������ ���´�.
				// �� ���Ǻ��� �����ִ� �̺�Ʈ �޽��� Ÿ�Ժ��� �����س��� ��.
				int iEventType = _GetRelatedEventMessageType( iConditionType );
				if( NONE_BUBBLE_EVENT_MESSAGE != iEventType )
					m_mmapDefinedByEvent.insert( make_pair(iEventType, pNewBubbleInfo) );
				//////////////////////////////////////////////////////////////////////////
			}
			else
				break;
		}

		// �̺�Ʈ �ڵ鷯 ������ ����.
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
	// ���� �߰� �̺�Ʈ �ڵ鷯��� ������ ������ �߰����ش�. ���߿� ������ ������ Ŭ��� �����ش�.
	if( BUBBLE_HANDLER::GETTING_BUBBLE == pEventHandler->GetType() )
	{
		int iIconIndex = pBubbleTable->GetFieldFromLablePtr( iBubbleTableID, "_IconIndex" )->GetInteger();
		static_cast<CDnGettingBubbleHandler*>(pEventHandler)->SetIconIndex( iIconIndex );
	}
#endif // #ifdef _GAMESERVER


	//// ����� ���� ����ȿ�� �߰��� �͵��� ������ �� ���� ���� �ý��ۿ��� ���� �����Ѵ�.
	//if( BUBBLE_HANDLER::ADD_STATE_EFFECT == pEventHandler->GetType() )
	//{
	//	// ���� ������, �ش� ����ȿ���� �����ϵ��� ���� Ÿ�� �� �̺�Ʈ �ڵ鷯 �߰�.
	//	IDnBubbleEventHandler* pRemoveEventHandler = IDnBubbleEventHandler::Create( BUBBLE_HANDLER::REMOVE_STATE_EFFECT, m_hActor, pArgument );
	//	m_mapBubbleRemoveEventHandlers[ static_cast<CDnAddStateEffectHandler*>(pEventHandler)->GetBubbleTypeID() ] = pRemoveEventHandler;
	//}
}

// �ܺο��� �޽����� �����ִ� ���� �˻��ð� ������ ���� �̺�Ʈ�� �̸� �������� ���´�.
// EVENT_BUBBLE_ON_USE_SKILL �޽����� ���� ON_USE_SKILL �������� ���ĵ� �̺�Ʈ ���� ������ �˻��Ѵ�.
// ���� ���⿡ �������� ���� �̺�Ʈ���� �ݵ�� �ϳ����� �־�� �̺�Ʈ�� ó���ȴ�.
// ���� �� �κ��� ���� ���� �ִ�.. 
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
	// ������ ��� ������ ���ӽð��� ���� ��� ������ ���ӽð����� ������.
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
	// ���� ���� ���� �̺�Ʈ ó��.
	if( pCreatedBubble )
	{
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_BUBBLE_COUNT_UPDATED ) );
		pEvent->SetBubbleTypeID( pCreatedBubble->GetTypeID() );
		OnEvent( pEvent );

	}
}

//void CDnBubbleSystem::_OnRemoveBubble( CDnBubble* pBubbleToRemove )
//{
//	// TODO: ���̺� ���ǵȴ�� �ϳ��� ������ ���� �� �� ����ȿ���� ����ų� ��..
//	// ����� ����ȿ���� �־����� �Ŷ�� ���� ���� �������� ���� ���� ������ ����ȿ�� ����� ����ȿ�� ���� �߰��ؾ� ��.
//
//}

void CDnBubbleSystem::_OnRemovedBubbles( int iBubbleTypeID, int iCount )
{
	//map<int, IDnBubbleEventHandler*>::iterator iter = m_mapBubbleRemoveEventHandlers.find( iBubbleTypeID );
	//if( m_mapBubbleRemoveEventHandlers.end() != iter )
	//{
	//	iter->second->ProcessEvent( this, NULL );
	//}

	// ���� ���� ���� �̺�Ʈ ó��.
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
		// �켱�� �� ������ ���ӽð��� ���� ���� �����ϰ� ���� �߰��� ���� ������ �����Ѵ�.
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
	// ������ ������ ���� ������ �����ش�.
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
		// �켱�� �� ������ ���ӽð��� ���� ���� �����ϰ� ���� �߰��� ���� ������ �����Ѵ�.
        std::deque<CDnBubble*>& dqpBubbles = iter->second;
		for( int i = 0; i < iRemoveCount; ++i )
		{
			// ���� �ʿ��ϸ� �߰�.
			//_OnRemoveBubble( &dqpBubbles.back() );
			dqpBubbles.pop_back();
		}

		// ��� Ÿ���� ������ ��� �������� �����̳� ����.
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

		// �Ҹ��ڿ��� ȣ��ȴٸ� ���� �Ҹ� �̺�Ʈ ó���� ���� �ʵ��� �Ѵ�.
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

		// ��� ������ �����ϴ°�.
		bool bResult = true;
		int iNumConditions = (int)pDefinedEventInfo->vlpConditions.size();
		for( int iCondition = 0; iCondition < iNumConditions; ++iCondition )
		{
			IDnConditionChecker* pCondition = pDefinedEventInfo->vlpConditions.at( iCondition );
			bResult = pCondition->IsSatisfy( this, pEventObject );
			if( false == bResult )
				break;
		}

		// ������ �ϳ��� �������� ������ �� ���ǵ� ���� ������ ������ ����..
		if( false == bResult )
			continue;

		// ��� ������ ������. �����Ϳ� ���ǵȴ�� ����..
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
			m_hActor->CmdRemoveStateEffectFromID( iBlowID );		// �������� �� ������ �ִٰ� �������ٵ� Ȯ�� �غ��� ��.
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