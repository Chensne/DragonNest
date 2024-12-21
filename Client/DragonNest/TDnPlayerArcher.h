#pragma once

#include "DnWeapon.h"
#include "DnLocalPlayerActor.h"
#include "TDnPlayerCommon.h"

template < class T >

class TDnPlayerArcher : public TDnPlayerCommon<T>
{
public:
	TDnPlayerArcher( int nClassID, bool bProcess = true )
		: TDnPlayerCommon<T>(nClassID, bProcess)
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
		bool bBattleMode;
	};

protected:
	int m_nKickActionIndex[4];
	DnWeaponHandle m_hKick[4];
	float m_fPitchAngle;
//	int m_nMaxVolleyCount;
//	int m_nCurVolleyCount;
//	std::vector<int> m_nVecShootActionList;

	static RotateBoneStruct s_RotateBoneDefine[];

public:
	// Actor Message
	virtual bool Initialize()
	{
		TDnPlayerCommon<T>::Initialize();

		int j = 0;
		for(; j < 4; j++)
			SAFE_RELEASE_SPTR(m_hKick[j]);

		m_hKick[0] = CDnWeapon::CreateWeapon( 29, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( 30, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( 31, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( 29, 0 );
		for( int i=0; i<4; i++ ) {
			m_hKick[i]->CreateObject();
			m_hKick[i]->SetLinkActor( GetMySmartPtr() );
		}

		m_nKickActionIndex[0] = GetElementIndex( "Attack_SideKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Attack_SoccerKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Attack_Down" );
		m_nKickActionIndex[3] = GetElementIndex( "Jump_Kick" );

		/*
		int nIndex = GetElementIndex( "Shoot_Stand" ); m_nVecShootActionList.push_back(nIndex);
		nIndex = GetElementIndex( "Shoot_Front" ); m_nVecShootActionList.push_back(nIndex);
		nIndex = GetElementIndex( "Shoot_Back" ); m_nVecShootActionList.push_back(nIndex);
		nIndex = GetElementIndex( "Shoot_Left" ); m_nVecShootActionList.push_back(nIndex);
		nIndex = GetElementIndex( "Shoot_Right" ); m_nVecShootActionList.push_back(nIndex);

		m_nMaxVolleyCount = 10000000;
		m_nCurVolleyCount = 0;
		*/
		for( int i=0; ; i++ ) {
			if( s_RotateBoneDefine[i].szBoneName == NULL ) break;
			s_RotateBoneDefine[i].nBoneIndex = GetBoneIndex( s_RotateBoneDefine[i].szBoneName );
		}
		return true;
	}

	virtual DnWeaponHandle GetWeapon( int nEquipIndex = 0, bool bActionMatchWeapon = true )
	{
		if( IsGhost() || !bActionMatchWeapon ) return m_hWeapon[nEquipIndex];
		if( nEquipIndex == 0 ) {
			for( int i=0; i<4; i++ ) {
				if( m_nActionIndex == m_nKickActionIndex[i] ) return m_hKick[i];
			}
		}
		return TDnPlayerCommon<T>::GetWeapon( nEquipIndex, bActionMatchWeapon );
	}

	virtual DnWeaponHandle GetActiveWeapon( int nEquipIndex )
	{
		if( IsGhost() ) return m_hWeapon[nEquipIndex];
		if( nEquipIndex == 0 ) {
			for( int i=0; i<4; i++ ) {
				if( m_nActionIndex == m_nKickActionIndex[i] ) return m_hKick[i];
			}
		}
		return TDnPlayerCommon<T>::GetActiveWeapon( nEquipIndex );
	}

	virtual void PreProcess( LOCAL_TIME LocalTime, float fDelta )
	{
		TDnPlayerCommon<T>::PreProcess( LocalTime, fDelta );
		if( IsSwapSingleSkin() ) return;
		if( strcmp( GetCurrentAction(), "Social_Sit01" ) == 0 ) return;

		if( m_fPitchAngle < -45.f ) m_fPitchAngle = -45.f;
		else if( m_fPitchAngle > 45.f ) m_fPitchAngle = 45.f;

		EtVector3 vRotate( 0.f, 0.f, 0.f );
		for( int i=0; ; i++ ) {
			if( s_RotateBoneDefine[i].szBoneName == NULL ) break;
			if( IsBattleMode() && !s_RotateBoneDefine[i].bBattleMode ) continue;

			// Rotha : 탈것모드일때 시점 바라보기 사용하지 않습니다.
			if( IsVehicleMode() )
				break;

			if( s_RotateBoneDefine[i].nBoneIndex == -1 )
			{
				s_RotateBoneDefine[i].nBoneIndex = GetBoneIndex( s_RotateBoneDefine[i].szBoneName );
			}
			else
			{
				vRotate.x = m_fPitchAngle * s_RotateBoneDefine[i].fWeight;
				m_hObject->SetBoneRotation( s_RotateBoneDefine[i].nBoneIndex, vRotate );
			}
		}
	}
	virtual void OnStopReady()
	{
		ResetCustomAction();
		ResetMixedAnimation();
	}

	/*
	// 아복잡해...
	virtual void OnChangeAction( const char *szPrevAction )
	{
		if( !IsLocalActor() ) return;
		if( !m_hWeapon[0] ) return;

		std::map<int,std::vector<int>>::iterator it = m_nMapShootActionMap.find( m_nActionIndex );
		if( it == m_nMapShootActionMap.end() ) return;

		if( m_hWeapon[0]->GetEquipType() < CDnWeapon::SmallBow || m_hWeapon[0]->GetEquipType() > CDnWeapon::CrossBow ) return;
		int nIndex = m_hWeapon[0]->GetEquipType() - CDnWeapon::SmallBow;

		m_nActionIndex = it->second[nIndex];
		m_szAction = GetElement(it->second[nIndex])->szName;

		if( m_pRender && m_nVecAniIndexList[m_nActionIndex] != -1 ) 
		{
			MAActorRenderBase::m_nAniIndex = m_nVecAniIndexList[m_nActionIndex];
		}
	}
	virtual void OnChangeActionQueue( const char *szPrevAction )
	{
		if( !IsLocalActor() ) return;
		if( !m_hWeapon[0] ) return;

		std::map<int,std::vector<int>>::iterator it = m_nMapShootActionMap.find( m_nActionIndex );
		if( it == m_nMapShootActionMap.end() ) return;

		if( m_hWeapon[0]->GetEquipType() < CDnWeapon::SmallBow || m_hWeapon[0]->GetEquipType() > CDnWeapon::CrossBow ) return;
		int nIndex = m_hWeapon[0]->GetEquipType() - CDnWeapon::SmallBow;
		m_szActionQueue = GetElement(it->second[nIndex])->szName;
	}
	virtual void OnChangeCustomAction()
	{
		if( !IsLocalActor() ) return;
		if( !m_hWeapon[0] ) return;

		std::map<int,std::vector<int>>::iterator it = m_nMapShootActionMap.find( m_nActionIndex );
		if( it == m_nMapShootActionMap.end() ) return;

		if( m_hWeapon[0]->GetEquipType() < CDnWeapon::SmallBow || m_hWeapon[0]->GetEquipType() > CDnWeapon::CrossBow ) return;
		int nIndex = m_hWeapon[0]->GetEquipType() - CDnWeapon::SmallBow;

		m_nCustomActionIndex = it->second[nIndex];
		m_szCustomAction = GetElement(it->second[nIndex])->szName;
	}
	*/
};