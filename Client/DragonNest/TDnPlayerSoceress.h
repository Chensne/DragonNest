#pragma once

#include "DnWeapon.h"
#include "TDnPlayerCommon.h"
#include "DnEtcObject.h"
#include "DnDamageBase.h"

template < class T >

class TDnPlayerSoceress : public TDnPlayerCommon<T>
{
public:
	TDnPlayerSoceress( int nClassID, bool bProcess = true )
		: TDnPlayerCommon<T>(nClassID, bProcess)
	{
		memset( m_nKickActionIndex, -1, sizeof(m_nKickActionIndex) );
		m_fPitchAngle = 0.f;
	}

	virtual ~TDnPlayerSoceress()
	{
		for( int i=0; i<4; i++ ) SAFE_RELEASE_SPTR( m_hKick[i] );
		SAFE_RELEASE_SPTR( m_hChargeObject );
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
	DnEtcHandle m_hChargeObject;

	static RotateBoneStruct s_RotateBoneDefine[];

public:
	// Actor Message
	virtual bool Initialize()
	{
		TDnPlayerCommon<T>::Initialize();

		int j = 0;
		for(; j < 4; j++)
			SAFE_RELEASE_SPTR(m_hKick[j]);

		m_hKick[0] = CDnWeapon::CreateWeapon( 32, 0 );
		m_hKick[1] = CDnWeapon::CreateWeapon( 33, 0 );
		m_hKick[2] = CDnWeapon::CreateWeapon( 34, 0 );
		m_hKick[3] = CDnWeapon::CreateWeapon( 32, 0 );
		for( int i=0; i<4; i++ ) {
			m_hKick[i]->CreateObject();
			m_hKick[i]->SetLinkActor( GetMySmartPtr() );
		}

		m_nKickActionIndex[0] = GetElementIndex( "Attack_SideKick" );
		m_nKickActionIndex[1] = GetElementIndex( "Attack_SoccerKick" );
		m_nKickActionIndex[2] = GetElementIndex( "Attack_Down" );
		m_nKickActionIndex[3] = GetElementIndex( "Jump_Kick" );

		for( int i=0; ; i++ ) {
			if( s_RotateBoneDefine[i].szBoneName == NULL ) break;
			s_RotateBoneDefine[i].nBoneIndex = GetBoneIndex( s_RotateBoneDefine[i].szBoneName );
		}

		SAFE_RELEASE_SPTR(m_hChargeObject);

		m_hChargeObject = (new CDnEtcObject)->GetMySmartPtr();
		if( m_hChargeObject->Initialize( NULL, NULL, "Soceress_ChargeAction.act" ) == false ) {
			SAFE_RELEASE_SPTR( m_hChargeObject );
		}
		m_hChargeObject->Show( false );

		return true;
	}

	virtual DnWeaponHandle GetStandardWeapon() { return m_hWeapon[1]; }

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
	virtual void CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, int nLoopCount = 0, float fFrame = 0.f, float fBlendFrame = 3.f )
	{
		CheckChargeEffect( szActionName, ( m_szCustomAction.empty() ) ? NULL : m_szCustomAction.c_str() );
		TDnPlayerCommon<T>::CmdMixedAction( szActionBone, szMaintenanceBone, szActionName, nLoopCount, fFrame, fBlendFrame );
	}
	virtual void CmdAction( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, bool bCheckOverlapAction = true, bool bFromStateBlow = false, bool bSkillChain = false )
	{
		if( !IsCustomAction() ) DestroyChargeEffect();
		CheckChargeEffect( szActionName );
		TDnPlayerCommon<T>::CmdAction( szActionName, nLoopCount, fBlendFrame, bCheckOverlapAction, bFromStateBlow, bSkillChain );
	}

	virtual void CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.0f, bool bChargeKey = false, bool bCheckOverlapAction = true, bool bOnlyCheck = false )
	{
		CheckChargeEffect( szActionName );
		TDnPlayerCommon<T>::CmdPassiveSkillAction( nSkillID, szActionName, nLoopCount, fBlendFrame, fStartFrame, bChargeKey, bCheckOverlapAction, bOnlyCheck );
	}

	virtual bool ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta, bool bCheckValid = true, bool bAutoUsedFromServer = false, bool bSendPacketToServer = true ) 
	{
		bool bResult = TDnPlayerCommon<T>::ExecuteSkill( hSkill, LocalTime, fDelta, bCheckValid, bAutoUsedFromServer, bSendPacketToServer );

		if( bResult ) DestroyChargeEffect();
		return bResult;
	}
	virtual void OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam ) {

		//if( CDnWeapon::Hp_Delta != HitParam.HitType )
		if( !HitParam.szActionName.empty() )
			DestroyChargeEffect();

		TDnPlayerCommon<T>::OnDamage( pHitter, HitParam );
	}
	virtual void CmdStop( const char *szActionName, int nLoopCount = 0, float fBlendFrame = 3.f, float fStartFrame = 0.f, bool bResetStop = false, bool bForce = false )
	{
		if( !IsCustomAction() ) DestroyChargeEffect();
		else CheckChargeEffect( szActionName, ( m_szCustomAction.empty() ) ? NULL : m_szCustomAction.c_str()  );
		TDnPlayerCommon<T>::CmdStop( szActionName, nLoopCount, fBlendFrame, fStartFrame, bResetStop, bForce );
	}

	virtual void ResetActor()
	{
		DestroyChargeEffect();
		TDnPlayerCommon<T>::ResetActor();
	}

protected:
	void CreateChargeEffect()
	{
		DnEtcHandle hHandle = TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::GetSignalHandle( -666, -666 );
		if( hHandle && strcmp( hHandle->GetCurrentAction(), "Charge" ) == NULL ) {
			return;
		}
		TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( -666, -666, true );

		hHandle = (new CDnEtcObject)->GetMySmartPtr();
		if( hHandle->Initialize( NULL, NULL, "Soceress_ChargeAction.act" ) == false ) {
			SAFE_RELEASE_SPTR( hHandle );
		}
		*hHandle->GetMatEx() = *GetObjectCross();

		hHandle->GetMatEx()->MoveLocalZAxis( 7.f );
		hHandle->GetMatEx()->MoveLocalXAxis( 74.f );
		hHandle->GetMatEx()->MoveLocalYAxis( 35.f );

		hHandle->SetActionQueue( "Charge" );

		EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::InsertSignalHandle( -666, -666, hHandle );
		pResult->bLinkObject = true;
		pResult->bDefendenceParent = true;
		pResult->bClingTerrain = false;
		memset( pResult->szBoneName, 0, sizeof(pResult->szBoneName) );
		pResult->vOffset = EtVector3( 7.f, 74.f, 35.f );
		pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
	}
	void DestroyChargeEffect()
	{
		DnEtcHandle hHandle = TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::GetSignalHandle( -666, -666 );
		if( hHandle ) {
			hHandle->SetActionQueue( "Destroy" );
		}
	}
	void CheckChargeEffect( const char *szActionName, const char *szCustomAction = NULL )
	{
		if( IsSwapSingleSkin() ) return;

		bool bCustomAction = true;
		if( szCustomAction ) bCustomAction = ( strcmp( szCustomAction, szActionName ) == NULL ) ? true : false;
		if( strstr( szActionName, "Charge_" ) && bCustomAction ) {
			CreateChargeEffect();
		}
		else if( strstr( szActionName, "Shoot_" ) || strstr( szActionName, "ChargeShoot_" ) || strstr( szActionName, "Jump" ) ) {
			DestroyChargeEffect();
		}
	}
	virtual void OnStopReady()
	{
		DestroyChargeEffect();
		ResetCustomAction();
		ResetMixedAnimation();
	}

};
