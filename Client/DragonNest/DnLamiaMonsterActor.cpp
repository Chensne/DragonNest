#include "StdAfx.h"
#include "DnLamiaMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnLamiaMonsterActor::CDnLamiaMonsterActor( int nClassID, bool bProcess )
: CDnDefenseMonsterActor( nClassID, bProcess )
{
}

CDnLamiaMonsterActor::~CDnLamiaMonsterActor()
{
	for( int i=0; i<2; i++ ) 
		SAFE_RELEASE_SPTR( m_hSubWeapon[i] );
}

void CDnLamiaMonsterActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
	CDnDefenseMonsterActor::AttachWeapon( hWeapon, nEquipIndex, bDelete );

	m_hSubWeapon[nEquipIndex] = CDnWeapon::CreateWeapon( hWeapon->GetClassID(), hWeapon->GetSeed(), 0, 0, 0, 0, false, false, false );
	m_hSubWeapon[nEquipIndex]->CreateObject();

	std::string szBoneName;
	switch( nEquipIndex ) {
		case 0: szBoneName = "~BoxBone01_R"; break;
		case 1: szBoneName = "~BoxBone02_R"; break;
	}
	m_hSubWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex, szBoneName.c_str() );
}

void CDnLamiaMonsterActor::DetachWeapon( int nEquipIndex )
{
	CDnDefenseMonsterActor::DetachWeapon( nEquipIndex );

	if( m_hSubWeapon[nEquipIndex] ) {
		m_hSubWeapon[nEquipIndex]->FreeObject();
		m_hSubWeapon[nEquipIndex]->UnlinkWeapon();
	}
	SAFE_RELEASE_SPTR( m_hSubWeapon[nEquipIndex] );
}


void CDnLamiaMonsterActor::DefenseWeapon( bool bCrash )
{
	CDnDefenseMonsterActor::DefenseWeapon( bCrash );
	
	if( bCrash ) {
		if( m_hSubWeapon[1]->IsExistAction( "Crash" ) ) 
			m_hSubWeapon[1]->SetActionQueue( "Crash" );
		else m_hSubWeapon[1]->SetDestroy();

		m_hSubWeapon[1]->UnlinkWeapon();
		m_hSubWeapon[1]->SetProcess( true );
	}
	else {
		if( m_hSubWeapon[1]->IsExistAction( "Defense" ) ) 
			m_hSubWeapon[1]->SetActionQueue( "Defense" );
	}
}

void CDnLamiaMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnDefenseMonsterActor::Process( LocalTime, fDelta );
	ProcessShieldState( m_hSubWeapon[1], LocalTime );
}

void CDnLamiaMonsterActor::OnDamageReadAdditionalPacket( CPacketCompressStream *pStream )
{
	CDnDefenseMonsterActor::OnDamageReadAdditionalPacket( pStream );
	if( m_hSubWeapon[1] && m_hWeapon[1] ) m_hSubWeapon[1]->SetDurability( m_hWeapon[1]->GetDurability() );
}