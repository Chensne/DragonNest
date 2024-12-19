#pragma once

#include "TDnFSMTransitCondition.h"


// FSMState 의 기본사항임.. 고로상속받아서 사용해야 함.. 
template< class EntityHandle >
class TDnFSMState
{
public:
	static int s_iUniqueID;

protected:
	struct S_AVAILABLE_TRANSIT_STATE
	{
		TDnFSMState<EntityHandle>* pFSMState;
		vector< TDnFSMTransitCondition<EntityHandle>* > pvlConditions;

		S_AVAILABLE_TRANSIT_STATE( void ) : pFSMState(NULL)
		{};

		//// 모양새는 별로 안 좋지만 Condition 만 여기서 메모리 해제..
		//~S_AVAILABLE_TRANSIT_STATE( void ) 
		//{
		//	SAFE_DELETE( pCondition );
		//};
	};

	struct IsEqual : unary_function<const S_AVAILABLE_TRANSIT_STATE&, bool>
	{
		int iUniqueIDToFind;
		IsEqual( int iUniqueID ) : iUniqueIDToFind(iUniqueID) {};

		bool operator () ( const S_AVAILABLE_TRANSIT_STATE& Rhs )
		{
			return Rhs.pFSMState->GetUniqueID() == iUniqueIDToFind;
		}
	};

	int m_iUniqueID;
	EntityHandle m_hEntity;

	vector<S_AVAILABLE_TRANSIT_STATE> m_vlAvailTransitState;



public:
	TDnFSMState( EntityHandle hEntity ) : m_hEntity( hEntity )
	{
		++s_iUniqueID;
		m_iUniqueID = s_iUniqueID;
	};

	virtual ~TDnFSMState(void) {};

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta ) 
	{ 
		if( m_hEntity )
			m_hEntity->OnFSMStateEntry( this ); 

		return true; 
	};
	
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta ) 
	{ 
		if( m_hEntity )
			m_hEntity->OnFSMStateProcess( this ); 
		
		return true; 
	};

	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta ) 
	{ 
		if( m_hEntity )
			m_hEntity->OnFSMStateLeave( this ); 
		
		return true; 
	};

	bool AddTransitState( TDnFSMState<EntityHandle>* pFSMState, vector< TDnFSMTransitCondition<EntityHandle>* > pvlCondition )
	{
		// 같은 state 인지 테스트 한 후에 집어 넣자.
		bool bResult = false;

		bool bNewState = true;
		if( false == m_vlAvailTransitState.empty() )
		{
			vector<S_AVAILABLE_TRANSIT_STATE>::iterator iter = find_if( m_vlAvailTransitState.begin(), m_vlAvailTransitState.end(), IsEqual(GetUniqueID()) );
			bNewState = (m_vlAvailTransitState.end() == iter);
			_ASSERT( bNewState && "같은 State들 끼리 Transit State 리스트에 추가할 수 없습니다." );
		}
		if( bNewState )
		{
			S_AVAILABLE_TRANSIT_STATE AvailState;
			AvailState.pFSMState = pFSMState;
			AvailState.pvlConditions = pvlCondition;
			m_vlAvailTransitState.push_back( AvailState );
			bResult = true;
		}

		return bResult;
	}

	bool AddTransitState( TDnFSMState<EntityHandle>* pFSMState, TDnFSMTransitCondition<EntityHandle>* pCondition )
	{
		// 같은 state 인지 테스트 한 후에 집어 넣자.
		bool bResult = false;

		bool bNewState = true;
		if( false == m_vlAvailTransitState.empty() )
		{
			vector<S_AVAILABLE_TRANSIT_STATE>::iterator iter = find_if( m_vlAvailTransitState.begin(), m_vlAvailTransitState.end(), IsEqual(GetUniqueID()) );
			bNewState = (m_vlAvailTransitState.end() == iter);
			_ASSERT( bNewState && "같은 State들 끼리 Transit State 리스트에 추가할 수 없습니다." );
		}
		if( bNewState )
		{
			S_AVAILABLE_TRANSIT_STATE AvailState;
			AvailState.pFSMState = pFSMState;
			AvailState.pvlConditions.push_back( pCondition );
			m_vlAvailTransitState.push_back( AvailState );
			bResult = true;
		}

		return bResult;
	}

	TDnFSMState<EntityHandle>* CanTransition( void )
	{
		TDnFSMState<EntityHandle>* pChangeState = NULL;

		int iNumAvailTransit =	(int)m_vlAvailTransitState.size();
		for( int iState = 0; iState < iNumAvailTransit; ++iState )
		{
			vector< TDnFSMTransitCondition<EntityHandle>* >& pvlCondition = m_vlAvailTransitState.at( iState ).pvlConditions;
			bool bSatisfy = false; 
			
			int iNumCondition = (int)pvlCondition.size();
			for( int iCondition = 0; iCondition < iNumCondition; ++iCondition )
			{
				bSatisfy = pvlCondition.at(iCondition)->IsSatisfy();
				if( false == bSatisfy )
					break;
			}

			if( bSatisfy )
			{
				pChangeState = m_vlAvailTransitState.at( iState ).pFSMState;
				break;
			}
		}

		return pChangeState;
	}

	int GetUniqueID( void ) { return m_iUniqueID; };
};

template< class EntityHandle >
int TDnFSMState<EntityHandle>::s_iUniqueID = 0;