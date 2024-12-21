#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"

template < class T >
class TDnPlayerLencea : public TDnPlayerCommon<T>, public TBoostMemoryPool< TDnPlayerLencea<T> >
{
public:
	TDnPlayerLencea( CMultiRoom *pRoom, int nClassID )
		: TDnPlayerCommon<T>(pRoom, nClassID)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
	}

	virtual ~TDnPlayerLencea()
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

		
		m_hKick[0] = CDnWeapon::CreateWeapon( GetRoom(),5901, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( GetRoom(),5902, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( GetRoom(),5903, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( GetRoom(),5901, 0 );
		for( int i=0; i<4; i++ ) m_hKick[i]->CreateObject();

		m_nKickActionIndex[0] = GetElementIndex( "Skill_PushKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Skill_PutKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Skill_Probe" );
		m_nKickActionIndex[3] = GetElementIndex( "Skill_Swoop" );

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