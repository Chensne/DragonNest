#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"

template < class T >

class TDnPlayerSoceress : public TDnPlayerCommon<T>
{
public:
	TDnPlayerSoceress( CMultiRoom *pRoom, int nClassID )
		: TDnPlayerCommon<T>(pRoom, nClassID)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
		m_fPitchAngle = 0.f;
	}

	virtual ~TDnPlayerSoceress()
	{
		for( int i=0; i<4; i++ ) SAFE_RELEASE_SPTR( m_hKick[i] );
	}

	struct RotateBoneStruct {
		char *szBoneName;
		int nBoneIndex;
		float fWeight;
	};

protected:
	int m_nKickActionIndex[4];
	DnWeaponHandle m_hKick[4];
	float m_fPitchAngle;

	static RotateBoneStruct s_RotateBoneDefine[];

public:
	// Actor Message
	virtual bool Initialize()
	{
		TDnPlayerCommon<T>::Initialize();

		m_hKick[0] = CDnWeapon::CreateWeapon( GetRoom(), 32, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( GetRoom(), 33, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( GetRoom(), 34, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( GetRoom(), 32, 0 );
		for( int i=0; i<4; i++ ) m_hKick[i]->CreateObject();

		m_nKickActionIndex[0] = GetElementIndex( "Attack_SideKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Attack_SoccerKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Attack_Down" );
		m_nKickActionIndex[3] = GetElementIndex( "Jump_Kick" );

		for( int i=0; ; i++ ) {
			if( s_RotateBoneDefine[i].szBoneName == NULL ) break;
			s_RotateBoneDefine[i].nBoneIndex = GetBoneIndex( s_RotateBoneDefine[i].szBoneName );
		}

		return true;
	}

	virtual DnWeaponHandle GetStandardWeapon() { return m_hWeapon[1]; }
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

	virtual void Process( LOCAL_TIME LocalTime, float fDelta )
	{
		TDnPlayerCommon<T>::Process( LocalTime, fDelta );

		if( m_fPitchAngle < -45.f ) m_fPitchAngle = -45.f;
		else if( m_fPitchAngle > 45.f ) m_fPitchAngle = 45.f;
	}

};