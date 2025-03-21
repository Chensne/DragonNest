#include "StdAFx.h"
#include "MAAiSimpleRush.h"
#include "MAMovementBase.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"

MAAiSimpleRush::MAAiSimpleRush( DnActorHandle hActor, MAAiReceiver *pReceiver )
: MAAiBase( hActor, pReceiver )
{
	m_nState = 0;
	m_ChangeStateTime = 0;
}

MAAiSimpleRush::~MAAiSimpleRush()
{
}

void MAAiSimpleRush::Process( LOCAL_TIME LocalTime, float fDelta )
{
	MAAiBase::Process( LocalTime, fDelta );

	if( m_hActor->IsDie() ) return;

	EtVector3 *pvPos = m_hActor->GetPosition();
	if( !m_hTarget ) {
		std::vector<DnActorHandle> hVecList;
		int nCount = CDnActor::ScanActor( *pvPos, 1500.f, hVecList );
		if( nCount > 1 ) {
			std::vector<DnActorHandle> hVecTargetResult;
			for( DWORD i=0; i<hVecList.size(); i++ ) {
				if( hVecList[i]->GetTeam() == m_hActor->GetTeam() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				hVecTargetResult.push_back( hVecList[i] );
			}

			if( !hVecTargetResult.empty() ) {
				m_hTarget = hVecTargetResult[ rand()%hVecTargetResult.size() ];
				if( m_hActor->IsMove() ) m_hActor->CmdStop( "Stand", 0, 6.f );
			}
		}
	}
	if( m_hTarget ) {
		if( m_hTarget->IsDie() ) {
			m_nState = 0;
			m_hTarget.Identity();
			return;
		}
		float fLength = EtVec3Length( &( *pvPos - *m_hTarget->GetPosition() ) );
		if( fLength > 2000.f && m_hActor->IsStay() ) {
			m_hTarget.Identity();
			return;
		}
		if( m_hActor->IsHit() || m_hActor->IsDown() || m_hActor->IsAir() ) {
			m_nState = 0;
			return;
		}
		float fUnitSize = (float)( m_hActor->GetUnitSize() + m_hTarget->GetUnitSize() );
		switch( m_nState ) {
			case 0:
				{
					int nPattern = rand()%120;
					std::string szAction;
					bool bMoveAttack = false;

					if( m_hActor->IsStay() && m_hActor->IsMovable() ) {
						if( fLength <= 200.f + fUnitSize  ) {
							if( nPattern < 10 && !IsDelay(1) ) szAction = "Attack1";
							else if( nPattern < 60 && !IsDelay(1) ) szAction = "Provoke";
							else if( nPattern < 100 && !IsDelay(1) ) szAction = "Attack2";
						}
						else if( fLength <= 400.f + fUnitSize ) {
							if( nPattern < 80 ) szAction = "Move_Front";
							else bMoveAttack = true;
						}
						else {
							if( nPattern < 80 ) szAction = "Move_Front";
							else bMoveAttack = true;
						}
					}

					int nLoopCount = 2 + rand()%3;

					if( bMoveAttack ) {
						m_nState = 1;
						m_hActor->CmdMove( m_hTarget, 100.f, "Move_Front", -1, 3.f );
						m_ChangeStateTime = LocalTime;
						break;
					}

					if( szAction.empty() ) break;
					if( strstr( szAction.c_str(), "Move" ) ) {
						m_hActor->CmdMove( m_hTarget, 100.f, szAction.c_str(), -1, 3.f );
					}
					else {
						nLoopCount = 0;
						SetDelay( 1, 3000 );

						m_hActor->CmdLook( m_hTarget, false );
						m_hActor->CmdStop( szAction.c_str(), nLoopCount, 3.f );
					}
				}
				break;
			case 1:
				if( LocalTime - m_ChangeStateTime < 5000 ) {
					if( ( m_hActor->IsMove() || m_hActor->IsStay() ) && fLength < 100.f + fUnitSize ) {
						if( rand()%2 ) m_hActor->CmdStop( "Attack1", 0, 3.f );
						else m_hActor->CmdStop( "Attack2", 0, 3.f );

						m_hActor->CmdLook( m_hTarget, false );

						SetDelay( 1, 3000 );
						m_nState = 0;
					}
				}
				else {
					m_nState = 0;
				}
				break;
		}
	}

	else {
		if( !IsDelay(0) && m_hActor->IsStay() && m_hActor->IsMovable() && !m_hActor->IsHit() && !m_hActor->IsDown() && !m_hActor->IsAir() ) {
			EtVector3 vTemp;
			vTemp = *pvPos;
			vTemp.y = 0.f;
			vTemp.x += cos( EtToRadian( rand()%360 ) ) * ( 300 + rand()%200 );
			vTemp.z += sin( EtToRadian( rand()%360 ) ) * ( 300 + rand()%200 );

			CDnMonsterActor *pMonster = dynamic_cast<CDnMonsterActor *>(m_hActor.GetPointer());
			if( pMonster ) {
				SAABox Box = *pMonster->GetGenerationArea();
				Box.Max.y = 1000000.f;
				Box.Min.y = -1000000.f;
				if( !Box.IsInside( vTemp ) ) {
					vTemp.x = Box.Min.x + ( rand()%(int)( Box.Max.x - Box.Min.x ) );
					vTemp.z = Box.Min.z + ( rand()%(int)( Box.Max.z - Box.Min.z ) );
					vTemp.y = CDnWorld::GetInstance().GetHeight( vTemp );
				}
			}

			m_hActor->CmdMove( vTemp, "Move_Front", -1, 8.f );
			SetDelay( 0, 5000 + rand()%3000 );
		}
	}
}