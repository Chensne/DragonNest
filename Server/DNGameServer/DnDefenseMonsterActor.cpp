#include "StdAfx.h"
#include "DnDefenseMonsterActor.h"
#include "DnWeapon.h"
#include "DnBlow.h"
#include "DnPlayerActor.h"
#include "DnStateBlow.h"

CDnDefenseMonsterActor::CDnDefenseMonsterActor( CMultiRoom *pRoom, int nClassID )
: CDnNormalMonsterActor( pRoom, nClassID )
{
	m_nDefenseProb = 0;
	m_bCrashDefense = false;
}

CDnDefenseMonsterActor::~CDnDefenseMonsterActor()
{
}

bool CDnDefenseMonsterActor::CheckDamageAction( DnActorHandle hActor )
{
	if( !m_bCrashDefense && m_hWeapon[1] && !IsDown() && !IsAir() ) {
		EtVector3 vTemp = *GetPosition() - *hActor->GetPosition();
		EtVec3Normalize( &vTemp, &vTemp );
		float fDot = EtVec3Dot( &vTemp, &m_Cross.m_vZAxis );
		int nDefenseProb = m_nDefenseProb;

		if( _rand(GetRoom())%100 < nDefenseProb ) {
			int nValue = m_hWeapon[1]->GetDurability();
			nValue -= (int)( m_HitParam.fDurability * 100.f );

			if( nValue < 0 ) nValue = 0;
			m_hWeapon[1]->SetDurability( nValue );

			if( nValue == 0 ) {
				m_bCrashDefense = true;
				DefenseWeapon( true );
				m_HitParam.szActionName = "Stun";
				m_fDownDelta = 3.f;

				MAMovementBase *pMovement = GetMovement();
				if( pMovement ) {
					pMovement->ResetMove();
					pMovement->ResetLook();
				}
				if( hActor && hActor->IsPlayerActor() ) {
					CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
					pPlayer->UpdateBrokenShield();
				}
			}
			else {
				DefenseWeapon( false );
				m_HitParam.szActionName = "Defense";
				m_HitParam.vVelocity.y = 0.f;
				m_HitParam.vResistance.y = 0.f;
			}

			//////////////////////////////////////////////////////////////////////////
			// #39059 디펜스 몬스터가 방패로 막으면 179번 상태효과 OnTargetHit 처리가 안됨.
			if( hActor && hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_179) )
			{
				DNVector(DnBlowHandle) vlhBlows;
				hActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_179, vlhBlows );

				int iNumBlow = (int)vlhBlows.size();
				for( int i = 0; i < iNumBlow; ++i )
				{
					DnBlowHandle hBlow = vlhBlows.at(i);
					if( hBlow && hBlow->GetBlowIndex() == STATE_BLOW::BLOW_179 )
						hBlow->OnTargetHit( GetMySmartPtr() );
				}
			}
			//////////////////////////////////////////////////////////////////////////

			return false;
		}
	}
	return CDnNormalMonsterActor::CheckDamageAction( hActor );
}

void CDnDefenseMonsterActor::DefenseWeapon( bool bCrash )
{
	if( bCrash ) {
		if( m_hWeapon[1]->IsExistAction( "Crash" ) ) {
			m_hWeapon[1]->SetActionQueue( "Crash" );
		}
		else m_hWeapon[1]->SetDestroy();

		m_hWeapon[1]->UnlinkWeapon();
		m_hWeapon[1]->SetProcess( true );

		m_hWeapon[1].Identity();
		m_bSelfDeleteWeapon[1] = false;
	}
	else {
		if( m_hWeapon[1]->IsExistAction( "Defense" ) )
			m_hWeapon[1]->SetActionQueue( "Defense" );
	}
}

void CDnDefenseMonsterActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_CanDefense:
			{
				CanDefenseStruct *pStruct = (CanDefenseStruct *)pPtr;
				m_nDefenseProb = pStruct->nProb;
			}
			return;
	}
	CDnNormalMonsterActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnDefenseMonsterActor::OnDamageWriteAdditionalPacket( CPacketCompressStream *pStream )
{
	int nValue = ( m_hWeapon[1] ) ? m_hWeapon[1]->GetDurability() : 0;
	pStream->Write( &nValue, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
}
