#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"

#if defined(PRE_ADD_ACADEMIC)

template < class T >

class TDnPlayerAcademic : public TDnPlayerCommon<T>
{
public:
	TDnPlayerAcademic( CMultiRoom *pRoom, int nClassID )
		: TDnPlayerCommon<T>(pRoom, nClassID)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
		m_fPitchAngle = 0.f;
	}

	virtual ~TDnPlayerAcademic()
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

		m_hKick[0] = CDnWeapon::CreateWeapon( GetRoom(), 5036, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( GetRoom(), 5037, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( GetRoom(), 5038, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( GetRoom(), 5036, 0 );
		for( int i=0; i<4; i++ ) m_hKick[i]->CreateObject();

		m_nKickActionIndex[0] = GetElementIndex( "Skill_SpannerSmashing" );
		m_nKickActionIndex[1] = GetElementIndex( "Skill_MegaHammer" );
		m_nKickActionIndex[2] = GetElementIndex( "Skill_MagnifyAttack" );
		m_nKickActionIndex[3] = GetElementIndex( "Skill_AirBomb" );

		for( int i=0; ; i++ ) {
			if( s_RotateBoneDefine[i].szBoneName == NULL ) break;
			s_RotateBoneDefine[i].nBoneIndex = GetBoneIndex( s_RotateBoneDefine[i].szBoneName );
		}
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

#endif	// #if defined(PRE_ADD_ACADEMIC)