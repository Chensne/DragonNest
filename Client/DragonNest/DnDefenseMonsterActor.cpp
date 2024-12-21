#include "StdAfx.h"
#include "DnDefenseMonsterActor.h"
#include "DnWeapon.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDefenseMonsterActor::CDnDefenseMonsterActor( int nClassID, bool bProcess )
: CDnNormalMonsterActor( nClassID, bProcess )
{
	m_bCrashDefense = false;
	m_OrignalShieldColor = EtColor( 0.f, 0.f, 0.f, 0.f );
}

CDnDefenseMonsterActor::~CDnDefenseMonsterActor()
{
}

void CDnDefenseMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	CDnNormalMonsterActor::OnDamage( pHitter, HitParam );

	if( !m_bCrashDefense && m_hWeapon[1] ) {
		if( m_hWeapon[1]->GetDurability() == 0.f ) {
			DefenseWeapon( true );
			m_bCrashDefense = true;

			MAMovementBase *pMovement = GetMovement();
			if( pMovement ) {
				pMovement->ResetMove();
				pMovement->ResetLook();
			}

			if( CDnActor::s_hLocalActor && pHitter && pHitter->GetActorHandle() == CDnActor::s_hLocalActor ) 
			{
				CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				pPlayer->UpdateBrokenShield();
			}

		}
		else {
			DefenseWeapon( false );
		}
	}
}

void CDnDefenseMonsterActor::OnDamageReadAdditionalPacket( CPacketCompressStream *pStream )
{
	int nValue;
	pStream->Read( &nValue, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	if( m_hWeapon[1] ) m_hWeapon[1]->SetDurability( nValue );
}

void CDnDefenseMonsterActor::DefenseWeapon( bool bCrash )
{
	if( bCrash ) {
		m_hWeapon[1]->CDnActionBase::SetFPS( 60.f );
		if( m_hWeapon[1]->IsExistAction( "Crash" ) ) {
			m_hWeapon[1]->SetActionQueue( "Crash" );

			EtObjectHandle hObject = m_hWeapon[1]->GetObjectHandle();
			if( hObject ) {
				int nParamIndex = hObject->AddCustomParam( "g_MaterialAmbient" );
				if( nParamIndex != -1 ) hObject->SetCustomParam( nParamIndex, &m_OrignalShieldColor );
			}
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

void CDnDefenseMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnNormalMonsterActor::Process( LocalTime, fDelta );

	ProcessShieldState( m_hWeapon[1], LocalTime );
}

void CDnDefenseMonsterActor::ProcessShieldState( DnWeaponHandle hWeapon, LOCAL_TIME LocalTime )
{
	if( hWeapon && hWeapon->GetObjectHandle() ) {
		EtObjectHandle hObject = hWeapon->GetObjectHandle();
		int nParamIndex = hObject->AddCustomParam( "g_MaterialAmbient" );
		if( nParamIndex != -1 ) {
			SCustomParam *pCustomParam = hObject->GetCustomParam( 0, nParamIndex );
			if( m_OrignalShieldColor == EtColor( 0.f, 0.f, 0.f, 0.f ) ) {
				m_OrignalShieldColor = EtColor( pCustomParam->fFloat4[0], pCustomParam->fFloat4[1], pCustomParam->fFloat4[2], pCustomParam->fFloat4[3] );
			}
			EtColor TargetColor = m_OrignalShieldColor;

			float fPer = ( 1.f / hWeapon->GetMaxDurability() ) * (float)hWeapon->GetDurability();
			if( fPer < 0.25f ) {
				TargetColor.r = m_OrignalShieldColor.r + 1.f;
			}
			else if( fPer < 0.5f ) {
				TargetColor.r = m_OrignalShieldColor.r + ( abs( ( ( 1.f - m_OrignalShieldColor.r ) * cos( (DWORD)LocalTime * (float)0.003f ) ) ) * 2.2f );
			}
			hObject->SetCustomParam( nParamIndex, &TargetColor );
		}
	}
}