#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"

template < class T >
class TDnPlayerWarrior : public TDnPlayerCommon<T>, public TBoostMemoryPool< TDnPlayerWarrior<T> >
{
public:
	TDnPlayerWarrior( CMultiRoom *pRoom, int nClassID )
		: TDnPlayerCommon<T>(pRoom, nClassID)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
	}

	virtual ~TDnPlayerWarrior()
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

		m_hKick[0] = CDnWeapon::CreateWeapon( GetRoom(), 5, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( GetRoom(), 6, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( GetRoom(), 7, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( GetRoom(), 5, 0 );
		for( int i=0; i<4; i++ ) m_hKick[i]->CreateObject();

		m_nKickActionIndex[0] = GetElementIndex( "Attack_SideKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Attack_SoccerKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Attack_Down" );
		m_nKickActionIndex[3] = GetElementIndex( "Jump_Kick" );

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