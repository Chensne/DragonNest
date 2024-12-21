#include "StdAFx.h"
#include "MAAiNormal.h"
#include "MAMovementBase.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"


MAAiNormal::MAAiNormal( DnActorHandle hActor, MAAiReceiver *pReceiver )
: MAAiBase( hActor, pReceiver )
{
	m_nState = 0;
	m_ChangeStateTime = 0;
}

MAAiNormal::~MAAiNormal()
{
}


void MAAiNormal::Process( LOCAL_TIME LocalTime, float fDelta )
{
	MAAiBase::Process( LocalTime, fDelta );

	if( m_hActor->IsDie() ) return;

	EtVector3 *pvPos = m_hActor->GetPosition();
	if( !m_hTarget ) {
		std::vector<DnActorHandle> hVecList;
		int nCount = CDnActor::ScanActor( *pvPos, 1000.f, hVecList );
		if( nCount > 1 ) {
			std::vector<DnActorHandle> hVecTargetResult;
			for( DWORD i=0; i<hVecList.size(); i++ ) {
				if( hVecList[i]->GetTeam() == m_hActor->GetTeam() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				hVecTargetResult.push_back( hVecList[i] );
			}

			if( !hVecTargetResult.empty() ) {
				m_hTarget = hVecTargetResult[ rand()%hVecTargetResult.size() ];
				if( m_hActor->IsMove() ) m_hActor->CmdStop( "Stand", 0, -1.f );
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

		if( fLength > 1500.f && m_hActor->IsStay() ) {
			if( m_hActor->IsMove() ) m_hActor->CmdStop( "Stand", 0, -1.f );
			m_nState = 0;
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
					int nPattern = rand()%100;
					std::string szAction;
					bool bMoveAttack = false;

					if( fLength < fUnitSize && m_hActor->IsMovable() ) {
						if( !strstr( m_hActor->GetCurrentAction(), "Back" ) ) {
							if( nPattern < 40 )
								szAction = "Move_Back";
							else
								szAction = "Walk_Back";
						}
					}
					if( m_hActor->IsStay() && m_hActor->IsMovable() ) {
						if( fLength <= 200.f + fUnitSize ) {
							if( nPattern < 18 ) szAction = "Walk_Left";
							else if( nPattern < 36 ) szAction = "Walk_Right";
							else if( nPattern < 64 ) szAction = "Walk_Back";
							else if( nPattern < 69 ) szAction = "Move_Left";
							else if( nPattern < 70 ) szAction = "Move_Right";
							else {
								if( IsDelay(1) ) szAction = "Stand";
								else szAction = "Attack1";
							}
						}
						else if( fLength <= 400.f + fUnitSize ) {
							if( nPattern < 5 ) szAction = "Stand";
							else if( nPattern < 25 ) szAction = "Walk_Front";
							else if( nPattern < 45 ) szAction = "Walk_Left";
							else if( nPattern < 65 ) szAction = "Walk_Right";
							else if( nPattern < 75 ) szAction = "Walk_Back";
							else if( nPattern < 80 ) szAction = "Move_Front";
							else if( nPattern < 82 ) szAction = "Move_Left";
							else if( nPattern < 85 ) szAction = "Move_Right";
							else bMoveAttack = true;
						}
						else {
							if( nPattern < 5 ) szAction = "Stand";
							else if( nPattern < 45 ) szAction = "Walk_Front";
							else if( nPattern < 82 ) szAction = "Move_Left";
							else if( nPattern < 85 ) szAction = "Move_Right";
							else bMoveAttack = true;
						}
					}

					int nLoopCount = 2 + rand()%3;

					if( bMoveAttack ) {
						m_nState = 1;
						m_hActor->CmdMove( m_hTarget, 100.f, "Move_Front", -1, 6.f );
						m_ChangeStateTime = LocalTime;
						m_hActor->CmdLook( m_hTarget );
						break;
					}

					if( szAction.empty() ) break;
					if( strstr( szAction.c_str(), "Front" ) ) {
						m_hActor->CmdMove( m_hTarget, 100.f, szAction.c_str(), nLoopCount, 6.f );
					}
					else {
						if( strstr( szAction.c_str(), "Attack" ) ) {
							nLoopCount = 0;
							SetDelay( 1, 2000 );
							m_hActor->CmdLook( m_hTarget, false );
							m_hActor->CmdStop( szAction.c_str(), nLoopCount, -1.f );
						}
						else {
							m_hActor->CmdLook( m_hTarget );
							m_hActor->CmdAction( szAction.c_str(), nLoopCount, -1.f );
						}
					}
				}
				break;
			case 1:
				if( LocalTime - m_ChangeStateTime < 5000 )
				{
					if( ( m_hActor->IsMove() || m_hActor->IsStay() ) && fLength < 100.f + fUnitSize ) 
					{
						m_hActor->CmdStop( "Attack1", 0, -1.f );
//						m_hActor->CmdLook( m_hTarget );
//						m_hActor->CmdAction( "Attack1" );
						SetDelay( 1, 2000 );
						m_nState = 0;
					}

					if( m_hActor->IsMove() && LocalTime - m_ChangeStateTime > 2000 ) 
					{
						if( fLength < 400.f )
						{						
							if( rand()%100 < 40 )
							{
								if( rand()%2 )
									m_hActor->CmdStop( "Move_Left", 3+rand()%2, -1.f );
								else 
									m_hActor->CmdStop( "Move_Right", 3+rand()%2, -1.f );

								m_hActor->CmdLook( m_hTarget );

								m_nState = 0;
							}
						}
					}
				}
				else
				{
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

			m_hActor->CmdMove( vTemp, "Walk_Front", -1, 8.f );
			SetDelay( 0, 5000 + rand()%3000 );
		}
	}
}
