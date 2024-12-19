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
		E_DUAL_WEAPON_INDEX = 1,	// �������Ⱑ ��� �����̴�.
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

		m_nKickActionIndex[0] = GetElementIndex( "Skill_SlightTurn" );	// �̹� �۾��� �Ϸ�Ǽ�. ���ҽ��� �����ϴ�.
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

			// Rotha : Ż�͸���϶� ���� �ٶ󺸱� ������� �ʽ��ϴ�.
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
	// �����ƹ�Ÿ�ʿ��� CDnPlayerActor::Process�� ���� ȣ������ �ʾƼ� ���� ������ �Լ�.
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
							// �Ҽ����� ����ó�� ��ü �ִϰ� ������� �װ� ���� �ϸ� �ִϰ� Ƣ�� ������ ���´�.
							// �ַ� PullOut, PutIn �ִϿ��� ���°Ŷ� �̶��� ���尪�� 0.0���� �������ش�.
							// �̷��� ó���ص� ĳ�ù���-�Ϲݹ��� Ÿ�Դٸ����� �ִ� Ʋ������ Ƣ�� ������ �߻��ϱ⵵ �Ѵ�. �ִ��������� ��¿�� ���ٰ� �Ѵ�.
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
		// RefreshWeaponViewOrder�ȿ��� SetBattleMode������ �ѹ� �� RefreshLeftHandWeapon�Լ��� ȣ��� �� ������,
		// RefreshLeftHandWeapon�Լ� �ȿ��� �ߺ� ȣ��ɶ��� ó���� �Ǿ��ְ�,
		// ���� Ȥ�� RefreshWeaponViewOrder�ȿ��� SetBattleMode�� ȣ������ �ʰ� �ٲ� ��쿡�� ����� �����ϵ��� �ϱ� ���ؼ� �̷��� virtual�� ó���ϵ��� �Ѵ�.
		TDnPlayerCommon<T>::RefreshWeaponViewOrder( nEquipIndex );
		if( E_DUAL_WEAPON_INDEX == nEquipIndex ) RefreshLeftHandWeapon();
	}

	virtual void RefreshLeftHandWeapon()
	{
		// Weapon, CashWeapon, WeaponOrder � ���õ��ִ� �ڵ尡 ����� ���Ƽ�
		// ������ LeftHandWeapon�� ó���� ���ַ��� ���� ������ �ʹ� ������ ���Ҵ�.
		// �׷��� �̷��� �� �Լ����� ó���ϵ��� �Ѵ�.

		int nWeaponID = 0;
		int nWeaponEnchantLevel = 0;

		if( m_hWeapon[E_DUAL_WEAPON_INDEX] && m_hWeapon[E_DUAL_WEAPON_INDEX]->IsShow() )
			nWeaponID = m_hWeapon[E_DUAL_WEAPON_INDEX]->GetClassID();

		if( m_hCashWeapon[E_DUAL_WEAPON_INDEX] && m_hCashWeapon[E_DUAL_WEAPON_INDEX]->IsShow() 
			&& m_bWeaponViewOrder[E_DUAL_WEAPON_INDEX] && m_hCashWeapon[E_DUAL_WEAPON_INDEX]->GetObjectHandle() )
			nWeaponID = m_hCashWeapon[E_DUAL_WEAPON_INDEX]->GetClassID();
		if( m_hWeapon[E_DUAL_WEAPON_INDEX] ) nWeaponEnchantLevel = m_hWeapon[E_DUAL_WEAPON_INDEX]->GetEnchantLevel();

		// �ƽ� ���ܻ�Ȳ ó��.
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

		// ��ȭ�� üũ ���ϴ°� ������ ���Ƴ��� �� �Ŀ� �����ϹǷ� üũ���� �ʴ´�.
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
						// ��Ʋ��忡 ���� ��ũ ����
						const char *szBoneName = GetLinkWeaponBoneName( E_DUAL_WEAPON_INDEX + 1, m_bBattleMode );
						m_hLeftHandWeapon->ChangeLinkBone( szBoneName );
					}
					else
					{
						// �ƽſ����� BattleMode�� true�� ���¿����� STE_ChangeWeaponLink�� ���ؼ� ���� ��ڷ� �ű涧�� �ִ�.
						// �׷��� BattleMode�� ���Ͼ �Ʒ�ó�� ���� ���ؼ� ó���Ѵ�.
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

					// CDnWeapon�� �׼��� �����Ҷ� ���� CDnWeapon::GetWeaponMatchAction�� ���ؼ�
					// ��ȭ�ִ� �׼��̳� ��Ÿ��ƽ�� �ִ� �׼����� �ٲ��ִµ�,
					// ĳ������ ��쿡�� �Ϲݹ����� ��ȭ��ġ�� ���ͼ� �����ϴ°ŷ� �Ǿ��ִ�.
					// �׷���, ������ �����,
					// Į�� �޼չ���� �� ��ġ������ �ֹ������� �������� �ε����� �����ؼ� ����Ѵ�.
					// �Դٰ� CDnWeapon::GetWeaponMatchAction�� ���ο��� ȣ��Ǵ� ������ �ε����� �������ڷ� �����ϱ⵵ ����� ������.
					// �׷��� �ϴ� �� ���� ���������� ������ �Ѵ�.
					//
					// �������ڸ�,
					// CreateObject �Ŀ� LinkWeapon�� 1���ε����� �ؼ� �޼����� ���� �Ѵ�.
					// �̾ ��Ʋ��忡 ���� �簻���Ҷ���(GetLinkWeaponBoneName ȣ��) 1�� �ε����� �ؼ� �޼� ��ġ�� ���� �ϴµ�,
					// �������� m_nEquipIndex�� �ٽ� 0���� �����ؼ� ��ȭ����Ʈ�� ����� �ٵ��� �Ѵ�.
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