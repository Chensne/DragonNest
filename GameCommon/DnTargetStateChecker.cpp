#include "StdAfx.h"
#include "DnTargetStateChecker.h"
#include "DnActor.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif


CDnTargetStateChecker::CDnTargetStateChecker( DnActorHandle hActor, CDnActorState::ActorStateEnum eRequiredState,
											 int iOperator, float fRadiusDistToSearch ) : IDnSkillUsableChecker( hActor ), 
																						  m_eRequiredState( eRequiredState ),
																						  m_iOperator( iOperator ),
																						  m_fRadiusDistToSearch( fRadiusDistToSearch )
{
	m_iType = TARGET_STATE_CHECKER;
}

CDnTargetStateChecker::~CDnTargetStateChecker(void)
{

}



bool CDnTargetStateChecker::CanUse( void )
{
	bool bResult = false;
	
	if( m_hHasActor )
	{
		DNVector(DnActorHandle) vlhTargetActors;
		CDnActor::ScanActor( ARG_STATIC_FUNC(m_hHasActor) *m_hHasActor->GetPosition(), m_hHasActor->GetUnitSize() + m_fRadiusDistToSearch, vlhTargetActors );
		EtVector3 vDistance;
		EtVector3 vDir;

		int iNumTargetActors = (int)vlhTargetActors.size();
		for( int i = 0; i < iNumTargetActors; ++i )
		{
			DnActorHandle hTargetActor = vlhTargetActors.at( i );

			if( hTargetActor == m_hHasActor->GetMySmartPtr() )
				continue;

			if( hTargetActor->IsDie() )
				continue;

			vDistance = *hTargetActor->GetPosition() - *m_hHasActor->GetPosition();
			float fLength = EtVec3Length( &vDistance );
			EtVec3Normalize( &vDir, &vDistance );

			if( EtToDegree( EtAcos( EtVec3Dot( &m_hHasActor->GetMatEx()->m_vZAxis, &vDir ) ) ) > 70.0f )
				continue;

			switch( m_iOperator )
			{
				// 오로지 이 상태에 대해서만 체크한다. 다른 상태들과 섞여 있으면 비트 조합이기 때문에 
				// 같거나 같지 않다가 의도대로 되지 않음.
				case OP_EQUAL:
					if( hTargetActor->GetState() == m_eRequiredState )
						bResult = true;
					break;

				case OP_NOT_EQUAL:
					if( hTargetActor->GetState() != m_eRequiredState )
						bResult = true;
					break;
				////////////////////////////////////

				case OP_AND:
					if( hTargetActor->GetState() & m_eRequiredState )
						bResult = true;
					break;

				case OP_NOT_AND:
					if( !(hTargetActor->GetState() & m_eRequiredState) )
						bResult = true;
					break;
			}

		}
	}

	return bResult;
}

IDnSkillUsableChecker* CDnTargetStateChecker::Clone()
{
	CDnTargetStateChecker* pNewChecker = new CDnTargetStateChecker(m_hHasActor, m_eRequiredState, m_iOperator, m_fRadiusDistToSearch);

	return pNewChecker;
}
