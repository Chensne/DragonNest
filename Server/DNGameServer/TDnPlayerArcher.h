#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"

template < class T >

class TDnPlayerArcher : public TDnPlayerCommon<T>
{
public:
	TDnPlayerArcher( CMultiRoom *pRoom, int nClassID )
		: TDnPlayerCommon<T>(pRoom, nClassID)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
		m_fPitchAngle = 0.f;
	}

	virtual ~TDnPlayerArcher()
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

		m_hKick[0] = CDnWeapon::CreateWeapon( GetRoom(), 29, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( GetRoom(), 30, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( GetRoom(), 31, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( GetRoom(), 29, 0 );
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

	/*
	void CalcWeaponTypeShootAction( const char *szAction, std::vector<int> &nVecList ) {
		const char *szType[] = { "_SmallBow", "_BigBow", "_CrossBow" };
		char szTemp[64];
		int nIndex;
		for( int i=0; i<3; i++ ) {
			sprintf_s( szTemp, "%s%s", szAction, szType[i] );
			nIndex = GetElementIndex( szTemp );
			nVecList.push_back( nIndex );
		}
	}
	*/

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

	/*
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true )
	{
		if( !IsValidEquipTypeAction( szActionName ) ) {
			// 무기를 바꿔꼈음에도 불구하고 다른 액션을 했다..핵쓴거다
			OutputDebug( "아쳐 핵썻다!!\n" );
		}
		TDnPlayerCommon<T>::CmdAction( szActionName, nLoopCount, fBlendFrame, bCheckOverlapAction );
	}
	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, float fFrame, float fBlendFrame = 3.f ) 
	{
		if( !IsValidEquipTypeAction( szActionName ) ) {
			// 무기를 바꿔꼈음에도 불구하고 다른 액션을 했다..핵쓴거다
			OutputDebug( "아쳐 핵썻다!!\n" );
		}
		TDnPlayerCommon<T>::CmdMixedAction( szActionBone, szMaintenanceBone, szActionName, fFrame, fBlendFrame );
	}
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f ) 
	{
		if( !IsValidEquipTypeAction( szActionName ) ) {
			// 무기를 바꿔꼈음에도 불구하고 다른 액션을 했다..핵쓴거다
			OutputDebug( "아쳐 핵썻다!!\n" );
		}
		TDnPlayerCommon<T>::CmdStop( szActionName, nLoopCount, fBlendFrame, fStartFrame );
	}
	virtual void SetCustomAction( const char *szActionName, float fFrame )
	{
		if( !IsValidEquipTypeAction( szActionName ) ) {
			// 무기를 바꿔꼈음에도 불구하고 다른 액션을 했다..핵쓴거다
			OutputDebug( "아쳐 핵썻다!!\n" );
		}
		TDnPlayerCommon<T>::SetCustomAction( szActionName, fFrame );
	}
	*/

	/*
	bool IsValidEquipTypeAction( const char *szActionName )
	{
		if( !m_hWeapon[0] ) return true;
		int nEquipType = (int)m_hWeapon[0]->GetEquipType();
		if( nEquipType < CDnWeapon::SmallBow || nEquipType > CDnWeapon::CrossBow ) return true;

		std::map<std::string, std::vector<int>>::iterator it = m_szMapShootActionMap.find( szActionName );
		if( it == m_szMapShootActionMap.end() ) return true;

		int nIndex = nEquipType - CDnWeapon::SmallBow;
		int nCurIndex = GetElementIndex( szActionName );
		if( std::find( it->second.begin(), it->second.end(), nCurIndex ) == it->second.end() ) return false;
		return true;
	}
	*/
};