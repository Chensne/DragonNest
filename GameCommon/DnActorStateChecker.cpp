#include "StdAfx.h"
#include "DnActorStateChecker.h"
#include "DnActor.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CDnActorStateChecker::CDnActorStateChecker( DnActorHandle hActor, 
										    CDnActorState::ActorStateEnum eRequiredState, int iOperator ) : IDnSkillUsableChecker( hActor ),
																											m_iOperator( iOperator ),
																											m_eRequiredState( eRequiredState )
{
	_ASSERT( 0 <= m_iOperator && m_iOperator < OP_COUNT && "잘못된 연산자 대입" );
	m_iType = STATE_CHECKER;
}

CDnActorStateChecker::~CDnActorStateChecker(void)
{

}



bool CDnActorStateChecker::CanUse( void )
{
	bool bResult = false;

	if( m_hHasActor )
	{
		// DnActorState::StateEnum 의 각 요소들은 비트 조합이 가능하도록 선언되어 있다.
		switch( m_iOperator )
		{
			case OP_EQUAL:
				bResult = ( m_hHasActor->GetState() == m_eRequiredState );
				break;
			
			case OP_NOT_EQUAL:
				bResult = ( m_hHasActor->GetState() != m_eRequiredState );

#ifdef _GAMESERVER
				// 공격스킬 사용후 < 공격중에서는 사용불가능한 스킬을 연달아 바로 사용하면 여기에 걸려서 서버에서는 못써서 실패가 될수가 있다.
				// 서버 프레임과 클라 프레임과의 미묘한 동기가 틀어지는경우인데 일단은 공격중일때 라는 상황은 잦은 경우 이기 때문에 빼두도록 한다.
				if((m_eRequiredState == CDnActorState::ActorStateEnum::Attack) && (bResult == false) ) 
					bResult = true;
#endif

				break;

			case OP_AND:
				bResult = ( m_hHasActor->GetState() & m_eRequiredState ) ? true : false;
				break;

			case OP_NOT_AND:
				bResult = !( m_hHasActor->GetState() & m_eRequiredState ) ? true : false;
				break;
		}
	}

	return bResult;
}

IDnSkillUsableChecker* CDnActorStateChecker::Clone()
{
	CDnActorStateChecker* pNewChecker = new CDnActorStateChecker(m_hHasActor, m_eRequiredState, m_iOperator);

	return pNewChecker;
}
