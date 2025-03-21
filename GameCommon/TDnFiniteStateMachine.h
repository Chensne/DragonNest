#pragma once

#include "TDnFSMState.h"
#include "TDnFSMTransitCondition.h"



// 유한 상태 기계.. 핸들링될 객체인 템플릿 인자는 스마트 포인터를 상속받은 녀석이라고 가정함.
template < class EntityHandle >
class TDnFiniteStateMachine: public TBoostMemoryPool<TDnFiniteStateMachine<EntityHandle>>
{
private:
	EntityHandle m_hEntity;

	TDnFSMState<EntityHandle>* m_pEntryState;
	TDnFSMState<EntityHandle>* m_pNowState;

	vector< TDnFSMState<EntityHandle>* > m_vlpStates;
	vector< TDnFSMTransitCondition<EntityHandle>* > m_vlpConditions;

	TDnFiniteStateMachine( void ) {};
public:
	TDnFiniteStateMachine( EntityHandle hEntity ) : m_hEntity(hEntity), 
													m_pEntryState( NULL ), 
													m_pNowState( NULL ) {};
	~TDnFiniteStateMachine(void)
	{
		SAFE_DELETE_PVEC( m_vlpStates );
		SAFE_DELETE_PVEC( m_vlpConditions );
	}


	void Initialize( void ) 
	{
		// 핸들링될 객체엔 FSM state 들을 구성하는 함수가 반드시 구현되어 있어야 함.
		//bool bSuccess = m_hEntity->MakeUpFSM();
		//_ASSERT( bSuccess && "FSM 구성 실패!" );
	};

	TDnFSMState<EntityHandle>* GetCurrentState( void ) { return m_pNowState; };

	void AddState( TDnFSMState<EntityHandle>* pState )
	{
		m_vlpStates.push_back( pState );
	}

	void AddCondition( TDnFSMTransitCondition<EntityHandle>* pCondition )
	{
		m_vlpConditions.push_back( pCondition );
	}

	void SetEntryState( TDnFSMState<EntityHandle>* pState )
	{
		m_pEntryState= pState;
		m_pNowState = pState;
	}

	void ChangeState( TDnFSMState<EntityHandle>* pState, LOCAL_TIME LocalTime, float fDelta )
	{
		m_pNowState->OnLeave( LocalTime, fDelta );

		m_pNowState = pState;
		m_pNowState->OnEntry( LocalTime, fDelta );
	}

	void Process( LOCAL_TIME LocalTime, float fDelta )
	{
		if( m_pNowState )
		{
			TDnFSMState<EntityHandle>* pState = NULL;
			do
			{
				pState = m_pNowState->CanTransition();

				if( pState )
					ChangeState( pState, LocalTime, fDelta );

			} while( pState );

			m_pNowState->OnProcess( LocalTime, fDelta );

			//TDnFSMState<EntityHandle>* pState = m_pNowState->CanTransition();

			//if( pState )
			//	ChangeState( pState, LocalTime, fDelta );
			//else
			//{
			//	m_pNowState->OnProcess( LocalTime, fDelta );
			//}
		}
	}

};
