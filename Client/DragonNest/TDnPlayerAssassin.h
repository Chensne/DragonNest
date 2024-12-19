#pragma once

#include "DnWeapon.h"
#include "DnLocalPlayerActor.h"
#include "TDnPlayerCommon.h"
#ifdef PRE_MOD_WARRIOR_RIGHTHAND
#include "DnLoadingTask.h"
#include "VillageClientSession.h"
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnActorClassDefine.h"

#if defined(PRE_ADD_ASSASSIN)

template < class T >

class TDnPlayerAssassin : public TDnPlayerCommon<T>
{
public:
	TDnPlayerAssassin( int nClassID, bool bProcess = true )
		: TDnPlayerCommon<T>(nClassID, bProcess)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
		m_fPitchAngle = 0.f;
	}

	virtual ~TDnPlayerAssassin()
	{
		for( int i=0; i<4; i++ ) SAFE_RELEASE_SPTR( m_hKick[i] );
#ifdef PRE_MOD_KALI_WEAPON
		SAFE_RELEASE_SPTR( m_hLeftHandWeapon );
#endif
	}

	struct RotateBoneStruct {
		char *szBoneName;
		int nBoneIndex;
		float fWeight;
		bool bBattleMode;
	};

	enum{
		E_DUAL_WEAPON_INDEX = 1,	// 보조무기가 듀얼 무기이다.
	};

protected:
	int m_nKickActionIndex[4];
	DnWeaponHandle m_hKick[4];
	float m_fPitchAngle;
#ifdef PRE_MOD_KALI_WEAPON
	DnWeaponHandle m_hLeftHandWeapon;
#endif

	static RotateBoneStruct s_RotateBoneDefine[];

public:
	// Actor Message
	virtual bool Initialize()
	{
		TDnPlayerCommon<T>::Initialize();

		int j = 0;
		for (; j < 4; j++)
			SAFE_RELEASE_SPTR(m_hKick[j]);

		m_hKick[0] = CDnWeapon::CreateWeapon( 5525, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( 5526, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( 5527, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( 5525, 0 );
		for( int i=0; i<4; i++ ) {
			m_hKick[i]->CreateObject();
			m_hKick[i]->SetLinkActor( GetMySmartPtr() );
		}

		m_nKickActionIndex[0] = GetElementIndex( "Skill_SlightTurn" );	// 이미 작업이 완료되서. 리소스를 따릅니다.
		m_nKickActionIndex[1] = GetElementIndex( "Skill_ReturnSpin" );
		m_nKickActionIndex[2] = GetElementIndex( "Skill_SoulKiss" );
		m_nKickActionIndex[3] = GetElementIndex( "Skill_Ghostkick" );

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

#ifdef PRE_MOD_KALI_WEAPON
	virtual void ReleaseWeaponSignalImp()
	{
		TDnPlayerCommon<T>::ReleaseWeaponSignalImp();
		if( m_hLeftHandWeapon ) {
			m_hLeftHandWeapon->ReleaseSignalImp();
			m_hLeftHandWeapon->ResetDefaultAction( 0 );
		}
	}
	virtual void Process( LOCAL_TIME LocalTime, float fDelta )
	{
		TDnPlayerCommon<T>::Process( LocalTime, fDelta );
		ProcessAdditionalWeapon( LocalTime, fDelta );
	}
	// 렌더아바타쪽에서 CDnPlayerActor::Process를 직접 호출하지 않아서 만든 별도의 함수.
	virtual void ProcessAdditionalWeapon( LOCAL_TIME LocalTime, float fDelta )
	{
		if( m_hLeftHandWeapon ) m_hLeftHandWeapon->Process( LocalTime, fDelta );
	}
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
	{
		TDnPlayerCommon<T>::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
		switch( Type ) {
			case STE_SendAction_Weapon :
				{
					SendAction_WeaponStruct *pStruct = (SendAction_WeaponStruct *)pPtr;

					if( !m_hLeftHandWeapon || E_DUAL_WEAPON_INDEX + 1 != pStruct->nWeaponIndex )
						break;

					int nWeaponIndex = pStruct->nWeaponIndex - 1;

					if( !GetActiveWeapon( nWeaponIndex ) )
						break;

					if( pStruct->szActionName )
					{
						if( GetActiveWeapon( nWeaponIndex )->IsExistAction( pStruct->szActionName ) ) {
							// 소서리스 무기처럼 자체 애니가 있을경우 그걸 블렌드 하면 애니가 튀는 현상이 나온다.
							// 주로 PullOut, PutIn 애니에서 나는거라 이때는 블렌드값을 0.0으로 설정해준다.
							// 이렇게 처리해도 캐시무기-일반무기 타입다를때는 애니 틀어져서 튀는 현상이 발생하기도 한다. 애니팀에서도 어쩔수 없다고 한다.
							float fBlendFrame = 3.0f;
							if( strstr( GetActiveWeapon( nWeaponIndex )->GetCurrentAction(), "_Weapon" ) ) fBlendFrame = 0.0f;
							m_hLeftHandWeapon->SetActionQueue( pStruct->szActionName, pStruct->nLoopCount, fBlendFrame );
						}
					}
				}
				break;

			case STE_ShowSwordTrail:
				{
					ShowSwordTrailStruct *pStruct = (ShowSwordTrailStruct *)pPtr;
					if( E_DUAL_WEAPON_INDEX == pStruct->nWeaponIndex )
					{
						if( m_hLeftHandWeapon )
							m_hLeftHandWeapon->ShowTrail( SignalEndTime - LocalTime, pStruct );
					}
				}
				break;

			case STE_ChangeWeaponLink:
				{
					if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
					{
						if( m_hLeftHandWeapon ) {
							ChangeWeaponLinkStruct* pStruct = (ChangeWeaponLinkStruct *)pPtr;
							if( pStruct->szLinkBoneName ) {
								if( pStruct->nWeaponIndex == E_DUAL_WEAPON_INDEX ) {
									std::string szName = pStruct->szLinkBoneName;
									bool bBattleMode = false;
									if( szName == "~BoxBone02" ) bBattleMode = true;
									else if( szName == "~BoxBone02_1" ) bBattleMode = false;
									const char *szBoneName = GetLinkWeaponBoneName( E_DUAL_WEAPON_INDEX + 1, bBattleMode );
									m_hLeftHandWeapon->ChangeLinkBone( szBoneName );
								}
							}
						}
					}
				}
				break;
		}
	}
	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false )
	{
		TDnPlayerCommon<T>::AttachWeapon( hWeapon, nEquipIndex, bDelete );
		if( E_DUAL_WEAPON_INDEX == nEquipIndex ) RefreshLeftHandWeapon();
	}
	virtual void DetachWeapon( int nEquipIndex = 0 )
	{
		TDnPlayerCommon<T>::DetachWeapon( nEquipIndex );
		if( E_DUAL_WEAPON_INDEX == nEquipIndex ) RefreshLeftHandWeapon();
	}
	virtual void AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false, bool bRefreshWeaponOrder = true )
	{
		TDnPlayerCommon<T>::AttachCashWeapon( hWeapon, nEquipIndex, bDelete, bRefreshWeaponOrder );
		if( E_DUAL_WEAPON_INDEX == nEquipIndex ) RefreshLeftHandWeapon();
	}
	virtual void DetachCashWeapon( int nEquipIndex = 0 )
	{
		TDnPlayerCommon<T>::DetachCashWeapon( nEquipIndex );
		if( E_DUAL_WEAPON_INDEX == nEquipIndex ) RefreshLeftHandWeapon();
	}

	virtual void RefreshWeaponViewOrder( int nEquipIndex )
	{
		// RefreshWeaponViewOrder안에서 SetBattleMode때문에 한번 더 RefreshLeftHandWeapon함수가 호출될 수 있지만,
		// RefreshLeftHandWeapon함수 안에서 중복 호출될때의 처리도 되어있고,
		// 차후 혹시 RefreshWeaponViewOrder안에서 SetBattleMode를 호출하지 않게 바꿀 경우에도 제대로 동작하도록 하기 위해서 이렇게 virtual로 처리하도록 한다.
		TDnPlayerCommon<T>::RefreshWeaponViewOrder( nEquipIndex );
		if( E_DUAL_WEAPON_INDEX == nEquipIndex ) RefreshLeftHandWeapon();
	}

	virtual void RefreshLeftHandWeapon()
	{
		// Weapon, CashWeapon, WeaponOrder 등에 관련되있는 코드가 상당히 많아서
		// 일일이 LeftHandWeapon쪽 처리를 해주려니 차후 관리가 너무 어려울거 같았다.
		// 그래서 이렇게 한 함수에서 처리하도록 한다.

		int nWeaponID = 0;
		int nWeaponEnchantLevel = 0;

		if( m_hWeapon[E_DUAL_WEAPON_INDEX] && m_hWeapon[E_DUAL_WEAPON_INDEX]->IsShow() )
			nWeaponID = m_hWeapon[E_DUAL_WEAPON_INDEX]->GetClassID();

		if( m_hCashWeapon[E_DUAL_WEAPON_INDEX] && m_hCashWeapon[E_DUAL_WEAPON_INDEX]->IsShow() 
			&& m_bWeaponViewOrder[E_DUAL_WEAPON_INDEX] && m_hCashWeapon[E_DUAL_WEAPON_INDEX]->GetObjectHandle() )
			nWeaponID = m_hCashWeapon[E_DUAL_WEAPON_INDEX]->GetClassID();
		if( m_hWeapon[E_DUAL_WEAPON_INDEX] ) nWeaponEnchantLevel = m_hWeapon[E_DUAL_WEAPON_INDEX]->GetEnchantLevel();

		// 컷신 예외상황 처리.
		if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
		{
			if( nWeaponID > 0 && GetActiveWeapon( E_DUAL_WEAPON_INDEX ) && GetActiveWeapon( E_DUAL_WEAPON_INDEX )->IsShow() )
			{
				if( IsHideWeaponBySignalInCutScene( E_DUAL_WEAPON_INDEX ) )
					nWeaponID = 0;
			}
		}

		int nCurrentWeaponID = 0;
		if( m_hLeftHandWeapon )
			nCurrentWeaponID = m_hLeftHandWeapon->GetClassID();

		// 강화는 체크 안하는게 어차피 갈아낄때 뺀 후에 장착하므로 체크하지 않는다.
		if( nWeaponID != nCurrentWeaponID ) {
			SAFE_RELEASE_SPTR( m_hLeftHandWeapon );
			if( nWeaponID > 0 ) {
				m_hLeftHandWeapon = CDnWeapon::CreateWeapon( nWeaponID, 0, 0, nWeaponEnchantLevel, 0, 0, false, false, false );
				if( m_hLeftHandWeapon ) {
					m_hLeftHandWeapon->CreateObject( this );
					if( m_bRTTRenderMode && m_hLeftHandWeapon->GetObjectHandle() )
					{
						m_hLeftHandWeapon->GetObjectHandle()->SetRenderType( RT_TARGET );
						m_hLeftHandWeapon->SetRTTRenderMode( m_bRTTRenderMode, m_pRTTRenderer );
					}
					m_hLeftHandWeapon->LinkWeapon( GetMySmartPtr(), 1 );

					if( CGlobalInfo::GetInstance().IsPlayingCutScene() == false )
					{
						// 배틀모드에 따라 링크 갱신
						const char *szBoneName = GetLinkWeaponBoneName( E_DUAL_WEAPON_INDEX + 1, m_bBattleMode );
						m_hLeftHandWeapon->ChangeLinkBone( szBoneName );
					}
					else
					{
						// 컷신에서는 BattleMode가 true인 상태에서도 STE_ChangeWeaponLink를 통해서 본을 등뒤로 옮길때가 있다.
						// 그래서 BattleMode를 못믿어서 아래처럼 직접 구해서 처리한다.
						if( GetActiveWeapon( E_DUAL_WEAPON_INDEX ) )
						{
							int nLinkBoneIndex = GetActiveWeapon( E_DUAL_WEAPON_INDEX )->GetLinkBoneIndex();
							if( nLinkBoneIndex >= 0 )
							{
								std::string szName = GetBoneName( nLinkBoneIndex );
								if( !szName.empty() )
								{
									bool bBattleMode = false;
									if( szName == "~BoxBone02" ) bBattleMode = true;
									else if( szName == "~BoxBone02_1" ) bBattleMode = false;
									const char *szBoneName = GetLinkWeaponBoneName( E_DUAL_WEAPON_INDEX + 1, bBattleMode );
									m_hLeftHandWeapon->ChangeLinkBone( szBoneName );
								}
							}
						}
					}

					// CDnWeapon에 액션을 설정할때 보통 CDnWeapon::GetWeaponMatchAction를 통해서
					// 강화있는 액션이나 판타스틱포 있는 액션으로 바꿔주는데,
					// 캐시템일 경우에는 일반무기의 강화수치를 얻어와서 적용하는거로 되어있다.
					// 그런데, 문제가 생긴게,
					// 칼리 왼손무기는 본 위치때문에 주무기지만 보조무기 인덱스로 설정해서 사용한다.
					// 게다가 CDnWeapon::GetWeaponMatchAction가 내부에서 호출되는 구조라 인덱스를 별도인자로 전달하기도 어려운 구조다.
					// 그래서 하는 수 없이 강제설정을 만들기로 한다.
					//
					// 정리하자면,
					// CreateObject 후에 LinkWeapon은 1번인덱스로 해서 왼손으로 가게 한다.
					// 이어서 배틀모드에 따라 재갱신할때도(GetLinkWeaponBoneName 호출) 1번 인덱스로 해서 왼손 위치로 가게 하는데,
					// 마지막에 m_nEquipIndex는 다시 0으로 설정해서 강화이펙트가 제대로 붙도록 한다.
					m_hLeftHandWeapon->ForceSetEquipIndex( E_DUAL_WEAPON_INDEX );
					if( m_hLeftHandWeapon->IsExistAction( "Idle" ) ) m_hLeftHandWeapon->SetAction( "Idle", 0.f, 0.f );
				}
			}
		}
	}

	virtual void Show( bool bShow )
	{
		if( IsShow() == bShow ) return;
		TDnPlayerCommon<T>::Show( bShow );
		if( m_hLeftHandWeapon || bShow ) RefreshLeftHandWeapon();
	}

	virtual void ShowWeapon( int nEquipIndex, bool bShow )
	{
		bool bRefresh = false;
		if( E_DUAL_WEAPON_INDEX == nEquipIndex && m_hWeapon[nEquipIndex] && m_hWeapon[nEquipIndex]->IsShow() != bShow )
			bRefresh = true;

		if( bShow == false && IsShow() == false )
			bRefresh = false;

		if( IsSwapSingleSkin() )
			bRefresh = false;

		TDnPlayerCommon<T>::ShowWeapon( nEquipIndex, bShow );

		if( true == bRefresh )
			RefreshLeftHandWeapon();
	}

	virtual void ShowCashWeapon( int nEquipIndex, bool bShow )
	{
		TDnPlayerCommon<T>::ShowCashWeapon( nEquipIndex, bShow );

		bool bRefresh = false;
		if( E_DUAL_WEAPON_INDEX == nEquipIndex && m_hCashWeapon[nEquipIndex] && m_hCashWeapon[nEquipIndex]->IsShow() != bShow )
			bRefresh = true;

		if( true == bRefresh )
			RefreshLeftHandWeapon();
	}

#ifdef PRE_ADD_37745
	virtual void SetBattleMode( bool bEnable, bool bForce )
	{
		TDnPlayerCommon<T>::SetBattleMode( bEnable, bForce );
#else
	virtual void SetBattleMode( bool bEnable )
	{
		TDnPlayerCommon<T>::SetBattleMode( bEnable );
#endif
		if( m_hLeftHandWeapon )
		{
			const char *szBoneName = GetLinkWeaponBoneName( E_DUAL_WEAPON_INDEX + 1, m_bBattleMode );
			m_hLeftHandWeapon->ChangeLinkBone( szBoneName );
		}
	}
#endif
};

#endif	// #if defined(PRE_ADD_ASSASSIN)