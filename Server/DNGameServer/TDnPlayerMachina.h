#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"

template < class T >
class TDnPlayerMachina : public TDnPlayerCommon<T>, public TBoostMemoryPool< TDnPlayerMachina<T> >
{
public:
	TDnPlayerMachina( CMultiRoom *pRoom, int nClassID )
		: TDnPlayerCommon<T>(pRoom, nClassID)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
	}

	virtual ~TDnPlayerMachina()
	{
		for( int i=0; i<4; i++ ) SAFE_RELEASE_SPTR( m_hKick[i] );
	}

protected:
	int m_nKickActionIndex[4];
	DnWeaponHandle m_hKick[4];


public:
	// Actor Message
	virtual bool Initialize()
	{
		TDnPlayerCommon<T>::Initialize();

		m_hKick[0] = CDnWeapon::CreateWeapon( GetRoom(),5951, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( GetRoom(),5952, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( GetRoom(),5953, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( GetRoom(),5951, 0 );
		for( int i=0; i<4; i++ ) m_hKick[i]->CreateObject();

		m_nKickActionIndex[0] = GetElementIndex( "Skill_ElbowAttack" );
		m_nKickActionIndex[1] = GetElementIndex( "Skill_OverBlow" );
		m_nKickActionIndex[2] = GetElementIndex( "Skill_HitAndHip" );
		m_nKickActionIndex[3] = GetElementIndex( "Skill_SmashingBlow" );

		return true;
	}

	virtual DnWeaponHandle GetWeapon( int nEquipIndex = 0 )
	{
		if( nEquipIndex == 0 ) {
			for( int i=0; i<4; i++ ) {
				if( m_nActionIndex == m_nKickActionIndex[i] ) return m_hKick[i];
			}
		}

		return TDnPlayerCommon<T>::GetWeapon( nEquipIndex );
	}
	virtual DnWeaponHandle GetActiveWeapon( int nEquipIndex )
	{
		if( nEquipIndex == 0 ) {
			for( int i=0; i<4; i++ ) {
				if( m_nActionIndex == m_nKickActionIndex[i] ) return m_hKick[i];
			}
		}

		return TDnPlayerCommon<T>::GetActiveWeapon( nEquipIndex );
	}
};