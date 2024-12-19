#include "StdAfx.h"
#include "DnWeapon.h"
#include "DnTableDB.h"
#include "MAActorRenderBase.h"
#include "EtSoundChannel.h"
#include "DnProjectile.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnMainFrame.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_DN_SMART_PTR_STATIC( CDnWeapon, 2000 )

DWORD CDnWeapon::s_dwUniqueCount = 0;
std::map<DWORD, DnWeaponHandle> CDnWeapon::s_dwMapUniqueSearch;
CSyncLock CDnWeapon::s_LockSearchMap;

CDnWeapon::CDnWeapon( bool bProcess, bool bIncreaseUniqueID )
: CDnUnknownRenderObject( bProcess )
{
	SetWeaponType( WeaponTypeEnum::Weapon );

	m_pParentWeapon = NULL;
	EtMatrixIdentity( &m_matWorld );
	CDnActionBase::Initialize( this );
	m_nLength = 0;
	m_nOriginalLength = 0;
	m_bLinkBone = false;
	m_nLinkBoneIndex = -1;

	m_fSoundRange = 1000.f;
	m_fRollOff = 0.5f;
	m_nDurability = m_nMaxDurability = 0;;
	m_bDestroyThis = false;

	m_dwUniqueID = -1;
	if( bIncreaseUniqueID ) {
		SetUniqueID( s_dwUniqueCount );
		s_dwUniqueCount++;
	}
	else {
		SetUniqueID( -1 );
	}

	m_nEquipIndex = -1;

	m_nAniIndex = -1;
	m_nBlendAniIndex = -1;
	m_fBlendAniFrame = 0.f;
	m_fBlendFrame = 0.f;
	m_fBlendStartFrame = 0.f;
	m_pSwordTrailStruct = NULL;
	m_nEmptyDurabilitySoundIndex = -1;
	m_nSetItemID = 0;
#if defined( PRE_ADD_OVERLAP_SETEFFECT )
	m_bOverLapSet = false;
#endif
	m_nEnchantActionValue = 0;
	m_eElement = ElementEnum_Amount;

	m_bShow = true;
	InsertUniqueSearchMap( this );
	m_bCreateObject = false;
	m_bSimpleSetItemDescription = false;
	m_nSimpleSetItemDescriptionID = -1;
	m_RecreateCashEquipType = (EquipTypeEnum)-1;
	m_bOneType = false;
}

CDnWeapon::~CDnWeapon()
{
	RemoveUniqueSearchMap( this );
	FreeObject();
}

bool CDnWeapon::Initialize( int nWeaponTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID)
{
	m_nClassID = nWeaponTableID;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox ) return false;
	if( !pSox->IsExistItem( m_nClassID ) ) return false;

	m_nLength = pSox->GetFieldFromLablePtr( nWeaponTableID, "_Length" )->GetInteger();
	m_nDurability = m_nMaxDurability = pSox->GetFieldFromLablePtr( nWeaponTableID, "_Durability" )->GetInteger();
	m_nDurabilityRepairCoin = pSox->GetFieldFromLablePtr( nWeaponTableID, "_DurabilityRepairCoin" )->GetInteger();
	m_EquipType = (EquipTypeEnum)pSox->GetFieldFromLablePtr( nWeaponTableID, "_EquipType" )->GetInteger();
	m_nOriginalLength = m_nLength;
	m_nSetItemID = pSox->GetFieldFromLablePtr( nWeaponTableID, "_SetItemID" )->GetInteger();
#if defined( PRE_ADD_OVERLAP_SETEFFECT )	
	m_bOverLapSet = pSox->GetFieldFromLablePtr( nWeaponTableID, "_SetOverlap" )->GetInteger() ? true : false;
#endif
	m_bSimpleSetItemDescription = ( pSox->GetFieldFromLablePtr( nWeaponTableID, "_Grade_SetItem" )->GetInteger() == 1 ) ? true : false;
	m_nSimpleSetItemDescriptionID = pSox->GetFieldFromLablePtr( nWeaponTableID, "_SetItemTextID" )->GetInteger();
	m_bOneType = ( pSox->GetFieldFromLablePtr( nWeaponTableID, "_OneType" )->GetInteger() == 1 ) ? true : false;

	// Item 정보 Initialize
	if( !CTaskManager::GetInstance().GetTask( "LoginTask" ) ) {
		if( CDnItem::Initialize( nWeaponTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID ) == false )
			return false;
	}

	// 무기는 기본 능력치에서 속성이 있을경우 기본속성으로 설정된다.
	for( int i=0; i<ElementEnum_Amount; i++ ) {
		if( GetElementAttack( (ElementEnum)i ) > 0 ) {
			m_eElement = (ElementEnum)i;
			break;
		}
	}

	if( m_nEnchantTableID > 0 && IsActiveEnchant() ) {
#ifdef PRE_FIX_MEMOPT_ENCHANT
		DNTableFileFormat* pEnchantSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
		DNTableFileFormat*  pEnchantNeedItemSox = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );
		if (pEnchantNeedItemSox == NULL || pEnchantSox == NULL)
			return false;
		int nNeedItemTableID = pEnchantSox->GetFieldFromLablePtr(m_nEnchantTableID, "_NeedItemTableID")->GetInteger();
		m_nEnchantActionValue = pEnchantNeedItemSox->GetFieldFromLablePtr(nNeedItemTableID, "_ActionChangeValue")->GetInteger();
#else
		DNTableFileFormat* pEnchantSox = GetDNTable( CDnTableDB::TENCHANT );
		m_nEnchantActionValue = pEnchantSox->GetFieldFromLablePtr( m_nEnchantTableID, "_ActionChangeValue" )->GetInteger();
#endif
	}
	if( IsExistAction( "Idle" ) ) SetActionQueue( "Idle" );

	return true;
}

void CDnWeapon::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bDestroyThis ) {
		if( m_hActor ) {
			for( int i=0; i<2; i++ ) {
				if( m_hActor->GetWeapon(i) == this ) {
					m_hActor->DetachWeapon(i);
					return;
				}
			}
		}
		else {
			delete this; 
		}
		return;
	}
	if( m_hObject ) {
		if( m_bLinkBone ) m_matWorld = *m_hObject->GetWorldMat();
		else m_hObject->Update( &m_matWorld );

		bool bParticleShow = true;
		if( m_hObject->GetObjectAlpha() < 0.5f ) {
			bParticleShow = false;
		}
		// 무기의 경우 Link Bone 일 경우 강제로 자기자신의 위치를 부모의 본에서 가져오는데
		// 프러스텀 컬링에 걸리는 경우 본 계산을 안해서 위치가 마지막 렌더된 곳의 위치로만 갱신됩니다.
		// 그래서 그냥 요딴식으로 컬링 마스크 구해다가 hide 시킵니다.
		if( m_hObject->GetFrustumMask() == 0 && m_bRTTRenderMode == false ) {
			bParticleShow = false;
		}

		int nCount = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetCount();
		for( int i = 0; i < nCount; i++) {
			EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetObject( i );
			if( hParticle ) {
				hParticle->Show( bParticleShow );
			}
		}
	}

	CDnActionBase::ProcessAction( LocalTime, fDelta );
	Process( &m_matWorld, LocalTime, fDelta );
	CDnActionSignalImp::Process( LocalTime, fDelta );

	if( m_hActor )
	{
		float fAlpha = m_hActor->GetAlpha();
		ApplyAlphaToSignalImps( fAlpha );
	}
}

void CDnWeapon::Update( EtMatrix *matWorld )
{
	m_matWorld = *matWorld;
}

void CDnWeapon::ChangeAnimation( int nAniIndex, float fFrame, float fBlendFrame )
{
	m_nBlendAniIndex = m_nAniIndex;
	m_fBlendStartFrame = fFrame;
	m_fBlendFrame = fBlendFrame;
	if( m_fBlendFrame == 0.f ) m_nBlendAniIndex = -1;

	if( m_nBlendAniIndex != -1 ) {
		m_fBlendAniFrame = m_fFrame;

		float fAniLength = (float)m_hObject->GetLastFrame( m_nBlendAniIndex );
		if( m_fBlendAniFrame >= fAniLength ) {
			m_fBlendAniFrame = fAniLength;
		}
	}

	m_nAniIndex = nAniIndex;
}

void CDnWeapon::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hObject ) return;
	if( m_nActionIndex == -1 ) return;
	if( m_nActionIndex >= (int)m_nVecAniIndexList.size() ) return;
	if( m_nAniIndex == -1 ) return;

	m_hObject->SetAniFrame( m_nAniIndex, m_fFrame );

	// Blend Ani
	if( m_nBlendAniIndex != -1 ) {
		if( m_fFrame - m_fBlendStartFrame < m_fBlendFrame ) {
			float fWeight = 1.f - ( 1.f / m_fBlendFrame * ( m_fFrame - m_fBlendStartFrame ) );
			m_hObject->BlendAniFrame( m_nBlendAniIndex, m_fBlendAniFrame, fWeight, 0 );
		}
		else {
			m_nBlendAniIndex = -1;
		}
	}

	m_hObject->SkipPhysics( IsSignalRange( STE_PhysicsSkip ) );
}

int CDnWeapon::GetAniIndex( const char *szAniName )
{
	if( !m_hObject ) return -1;
	if( !m_hObject->GetAniHandle() ) return -1;
	for( int i=0; i<m_hObject->GetAniCount(); i++ ) {
		if( strcmp( m_hObject->GetAniName(i), szAniName ) == NULL ) return i;
	}
	return -1;
}

int CDnWeapon::GetBoneIndex( const char *szBoneName )
{
	if( !m_hObject ) return -1;
	EtAniHandle handle = m_hObject->GetAniHandle();
	if( !handle ) return -1;
	return handle->GetBoneIndex( szBoneName );
}

int CDnWeapon::GetDummyBoneIndex( const char *szBoneName )
{
	if( !m_hObject ) return -1;
	EtMeshHandle handle = m_hObject->GetMesh();
	if( !handle ) return -1;
	return handle->FindDummy( szBoneName );
}

void CDnWeapon::ShowWeapon( bool bShow )
{
	ShowRenderBase( bShow );
}

void CDnWeapon::ShowRenderBase( bool bShow )
{
	m_bShow = bShow;
	if( !m_hObject ) return;
	m_hObject->ShowObject( bShow );
	if( !bShow )
		ReleaseSignalImp();
}

void CDnWeapon::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	bool bProcessSignal = true;
	if( m_hActor && !m_hActor->IsShow() ) bProcessSignal = false;

	if( bProcessSignal )
		CDnActionSignalImp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	switch( Type ) {
		case STE_ShowWeapon:
			{
				ShowWeaponStruct *pStruct = (ShowWeaponStruct *)pPtr;
				
				if( m_hActor )
					m_hActor->ShowWeapon( pStruct->nEquipIndex, ( pStruct->bEnable == TRUE ) ? true : false );
			}
			break;
		case STE_Projectile:
			{
				if( !m_hActor ) break;

				if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
					break;

				// 게임에서는 현재 플레이어의 모든 발사체는 클라에서 게임서버로 보내므로 플레이어 자기 자신의 발사체 시그널 이외에는 처리하지 않는다.
				if( m_hActor->IsPlayerActor() && CDnActor::s_hLocalActor != m_hActor )
					break;

				ProjectileStruct *pStruct = (ProjectileStruct *)pPtr;

				MatrixEx LocalCross;
				if( m_hActor->IsPlayerActor() == true )
					LocalCross = m_matWorld; // 플레이어인 경우엔 무기 매트릭스 원래대로 넣어줌.
				else
					LocalCross = *m_hActor->GetMatEx();

				// Note: 한기 - 서버쪽에선 무기의 월드 매트릭스가 단위 행렬값임.... 계산 따로 안하는 듯. 
				// 그래서 겜 서버쪽 무기 발사체 시그널에선 액터의 월드 매트릭스 값을 넘겨줌. 
				// 중력값 먹는 Projectile/TargetPosition/Accell 타입의 발사체를 설정할 시엔 이 부분이 고려되어야 함..

				// #15557 현재 몬스터는 서버에서 패킷을 따로 보내주므로 여기서 자신의 위치에서 발사체를 쏘지 않도록 한다.
				if( m_hActor->IsPlayerActor() || ( (GetWeaponType() != WeaponTypeEnum::Projectile) && m_hActor->IsMonsterActor() ) )
				{
					CDnProjectile *pProjectile = CDnProjectile::CreateProjectile( m_hActor, LocalCross, pStruct );

					if( pProjectile ) 
						pProjectile->ShowWeapon( m_hActor->IsShow() );

					// Note: 남이 쏜 CrossHair 프로젝타일은 패킷으로 따로 받는다. signal 로 처리되는 것은 무시. 
					// CDnProjectile::CreateProjectile() 함수에서 NULL 리턴 됨
					if( pProjectile ) 
					{
						pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );

#ifdef PRE_MOD_PROJECTILE_HACK
						m_hActor->OnSkillProjectile( pProjectile );
						m_hActor->OnProjectile( pProjectile );
#else
						// 발사체에서 발사체를 쏘는 경우 부모 발사체를 일러줘서 패킷으로 서버에도 보내도록 한다.
						if( GetWeaponType() == WeaponTypeEnum::Projectile )
							pProjectile->SetParentProjectileID( GetUniqueID() );

						m_hActor->OnSkillProjectile( pProjectile );
						m_hActor->OnProjectile( pProjectile, nSignalIndex );
#endif
					}
				}
			}
			break;
		case STE_Destroy:
			if( IsProcess() ) SetDestroy();
			else m_bDestroyThis = true;
			break;
		case STE_AlphaBlending:
			{
				if( !m_hObject ) break;
				AlphaBlendingStruct *pStruct = (AlphaBlendingStruct *)pPtr;
				float fValue = 1.f / ( SignalEndTime - SignalStartTime - 16.6666f ) * ( LocalTime - SignalStartTime );
				fValue = EtClamp( fValue, 0.0f, 1.0f );
				float fCurAlpha = pStruct->fStartAlpha + ( ( pStruct->fEndAlpha - pStruct->fStartAlpha ) * fValue );
				fCurAlpha = EtClamp( fCurAlpha, 0.0f, 1.0f );
				m_hObject->SetObjectAlpha( fCurAlpha );
				if( fCurAlpha < 0.5f ) {
					if( m_hObject->IsShadowCast() )
						m_hObject->EnableShadowCast( false );
				}
			}
			break;
		case STE_AttachSwordTrail:
			{
				if( m_hSwordTrail ) break;
				AttachSwordTrailStruct *pStruct = (AttachSwordTrailStruct *)pPtr;
				EtTextureHandle hTexture = CEtResource::GetSmartPtr( pStruct->nTextureIndex );
				EtTextureHandle hNormalTexture = CEtResource::GetSmartPtr( pStruct->nNormalTextureIndex );

				m_hSwordTrail = EternityEngine::CreateSwordTrail( GetObjectHandle(), hTexture, hNormalTexture, pStruct->fLifeTime, pStruct->fMinSegment );
				m_hSwordTrail->SetBlendOP( (EtBlendOP)( pStruct->nBlendOP + 1 ) );
				m_hSwordTrail->SetSrcBlend( (EtBlendMode)( GetValue2ExceptionSwap( pStruct->nSrcBlend, 1, 0, 4 ) + 1 ) );
				m_hSwordTrail->SetDestBlend( (EtBlendMode)( GetValue2ExceptionSwap( pStruct->nDestBlend, 1, 0, 5 ) + 1 ) );
				m_hSwordTrail->SetTechniqueIndex( pStruct->nTechIndex );
				m_pSwordTrailStruct = pStruct;
			}
			break;
	}
}


void CDnWeapon::OnHitSuccess( DnActorHandle hActor, HitTypeEnum HitType, EtVector3 &vHitPosition, bool bPlaySound )
{
	EffectStruct EffectIndex;
	int nSoundIndex = -1;

	switch( HitType ) {
		case HitTypeEnum::Normal:
			EffectIndex = m_NormalHitEffect[ hActor->GetMaterial() ];
			if( !m_nVecNormalHitSound[ hActor->GetMaterial() ].empty() )
				nSoundIndex = m_nVecNormalHitSound[ hActor->GetMaterial() ][_rand()%m_nVecNormalHitSound[ hActor->GetMaterial() ].size()];
			break;
		case HitTypeEnum::Critical:
			EffectIndex = m_CriticalHitEffect[ hActor->GetMaterial() ];
			if( !m_nVecCriticalHitSound.empty() )
				nSoundIndex = m_nVecCriticalHitSound[ _rand()%m_nVecCriticalHitSound.size() ];
			break;
		case HitTypeEnum::Stun:
			EffectIndex = m_StunHitEffect[ hActor->GetMaterial() ];
			if( !m_nVecStunHitSound.empty() )
				nSoundIndex = m_nVecStunHitSound[ _rand()%m_nVecStunHitSound.size() ];
			break;
		case HitTypeEnum::Defense:
			EffectIndex = m_DefenseEffect;
			if( !m_nVecDefenseSound.empty() )
				nSoundIndex = m_nVecDefenseSound[ _rand()%m_nVecDefenseSound.size() ];
			break;
	}
	MatrixEx Cross;
	Cross = *hActor->GetMatEx();
	Cross.m_vZAxis = vHitPosition - *hActor->GetPosition();
	if( EtVec3LengthSq( &Cross.m_vZAxis ) == 0.f ) Cross.m_vZAxis = EtVector3( 0.f, 0.f, 1.f );
	EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis );
	Cross.MakeUpCartesianByZAxis();

	switch( hActor->GetHitCheckType() ) {
		case CDnActor::BoundingBox:
			{
				Cross.m_vPosition += Cross.m_vZAxis * (float)( hActor->GetUnitSize() * 0.8f );	// 임시로 유닛크기의 80프로정도에 찍게 해주자

				if( GetWeaponType() == CDnWeapon::WeaponTypeEnum::Projectile ) // 프로젝타일일 경우
				{	
					Cross.m_vPosition.y = vHitPosition.y;
				}
				else
				{
					if( m_hObject )
					{
						if( m_bLinkBone && m_nLinkBoneIndex == -1 )
							Cross.m_vPosition.y = hActor->GetMatEx()->m_vPosition.y + 50.f;
						else
							Cross.m_vPosition.y = m_matWorld._42;
					}
					else
					{
						Cross.m_vPosition.y = hActor->GetMatEx()->m_vPosition.y + 50.f;	// 발차기 같은 경우 m_hObject 가 없기 떔시 임시로 50.f 높이값 보정해준다.
					}
				}
			}
			break;
		case CDnActor::Collision:
			{
				CDnProjectile *pProjectile = dynamic_cast<CDnProjectile *>(this);
				if( pProjectile ) {
					Cross.m_vPosition = vHitPosition;
				}
				else {
					if ( m_hActor )
					{
						Cross.m_vZAxis = *m_hActor->GetPosition() - vHitPosition;
						float fLength = EtVec3Length( &Cross.m_vZAxis );
						EtVec3Normalize( &Cross.m_vZAxis, &Cross.m_vZAxis );
						Cross.m_vPosition = vHitPosition;// + ( Cross.m_vZAxis * ( fLength - m_hActor->GetUnitSize() - 30.f ) );
					}
				}
			}
			break;
	}

	if( EffectIndex.nIndex != -1 ) {
		switch( EffectIndex.cType ) {
			case 0: CreateBillboardEffect( EffectIndex.nIndex, Cross ); break;
			case 1: CreateEffectObject( EffectIndex.nIndex, Cross ); break;
		}
		
	}
	if( bPlaySound && nSoundIndex != -1 ) {
		CEtSoundChannel *pChannel = CEtSoundEngine::GetInstance().PlaySound( "3D", nSoundIndex, false, true );
		if( pChannel ) {
			pChannel->SetVolume( 1.f );
			pChannel->SetPosition( Cross.m_vPosition ); 
			pChannel->SetRollOff( 3, 0.f, 1.f, m_fSoundRange * m_fRollOff, 1.f, m_fSoundRange, 0.f );

			pChannel->Resume();
		}
	}

	// 피니쉬 이펙트. 사운드는 언제나 출력~ 
	if( hActor->IsDie() ) {
		if( m_FinishEffect.nIndex != -1 ) {
			switch( m_FinishEffect.cType ) {
				case 0: CreateBillboardEffect( m_FinishEffect.nIndex, Cross ); break;
				case 1: CreateEffectObject( m_FinishEffect.nIndex, Cross ); break;
			}
		}
		if( m_FinishEffect.hEtcObject ) CreateEtcObject( m_FinishEffect.hEtcObject, hActor, Cross );

		if( !m_nVecFinishSound.empty() ) {
			nSoundIndex = m_nVecFinishSound[ _rand()%m_nVecFinishSound.size() ];
			if( nSoundIndex != -1 ) {
				CEtSoundChannel *pChannel = CEtSoundEngine::GetInstance().PlaySound( "3D", nSoundIndex, false, true );
				if( pChannel ) {
					pChannel->SetVolume( 1.f );
					pChannel->SetPosition( Cross.m_vPosition ); 
					pChannel->SetRollOff( 3, 0.f, 1.f, m_fSoundRange * m_fRollOff, 1.f, m_fSoundRange, 0.f );

					pChannel->Resume();
				}
			}
		}
	}
	// 액션 오브젝트를 찍어줍니다 ( 라이트 때문 )
	if( EffectIndex.hEtcObject ) CreateEtcObject( EffectIndex.hEtcObject, hActor, Cross );
}

float CDnWeapon::GetRepairPrice()
{
	float fValue = 0.f;
	int nGapDur = m_nMaxDurability - m_nDurability;
	if( nGapDur > 0 )
	{
		float fEnchantRevision = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RepairDurabilityRevision );
		float fRatio = ( 1.f / m_nMaxDurability ) * (float)nGapDur;

		fValue = m_nDurabilityRepairCoin * fRatio;

		if( m_nDurability == 0 ) fValue *= 1.3f;
		if( m_cEnchantLevel > 0 ) fValue *= pow( fEnchantRevision, m_cEnchantLevel );
	}
	if( fValue != 0.f && fValue < 1.f ) fValue = 1.f;
	return fValue;
}

void CDnWeapon::OnEmptyDurability()
{
	if( m_hActor && m_hActor == CDnActor::s_hLocalActor ) {
		wchar_t wszTemp[256] = {0};
		swprintf_s( wszTemp, _countof(wszTemp), L"%s[%s]", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1734 ), m_szName.c_str() );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );

		if( m_nEmptyDurabilitySoundIndex != -1 )
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nEmptyDurabilitySoundIndex );
	}
}

void CDnWeapon::LinkWeapon( DnActorHandle hActor, int nEquipIndex, const char *szBoneName )
{
	if( !m_hObject ) return;
	m_hActor = hActor;
	m_nEquipIndex = nEquipIndex;
	MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(m_hActor.GetPointer());
	if( pRender ) {
		std::string szLinkBoneName;
		if( szBoneName ) {
			szLinkBoneName = szBoneName;
		}
		else {
			switch( nEquipIndex ) {
				case 0:
					szLinkBoneName = "~BoxBone01";
					break;
				case 1:
					szLinkBoneName = "~BoxBone02";
					break;
			}
		}

		int nBoneIndex = -1;
		if( !szLinkBoneName.empty() ) nBoneIndex = pRender->GetBoneIndex( szLinkBoneName.c_str() );
		m_hObject->SetParent( pRender->GetObjectHandle(), nBoneIndex );
		m_nLinkBoneIndex = nBoneIndex;

		// 한가지 주의해야할 점에 대해 적어둔다. 이건 ChangeLinkBone 함수에서 m_nLinkBoneIndex 설정할때도 해당되는 이야기다.
		// 무기를 Link하는건 결국엔 SetParent한다는 이야긴데, 드네에선 크게 세가지 형태의 Link가 있다.
		// 첫번째는 기본적인 무기 어태치로, 메이스같이 ani가 없는 무기를 특정 본을 SetParent하는 것이다.
		// 두번째는 워려 건틀렛이나 아카 글러브처럼 플레이어의 ani를 그대로 사용하면서 BoneIndex를 -1로 설정한채 SetParent하는 것이다.
		// 세번째는 크로스보우처럼 자체 ani를 가지고 있는채 특정 본을 SetParent하는 것이다.
		//
		// 이 세가지 경우 외에는 잘못 설정해서 문제가 발생할 수 있는데,
		// 예를 들어서 플레이어의 ani를 그대로 사용하는 두번째 케이스인데, 특정본에 붙도록 설정하면, 뭔가 꼬여버리는 것이다.
		// 좀 더 정확하게 말하자면 이럴때 꼭 문제가 되는건 아니다.
		// Matrix계산하는 곳에서 특정 순서에 의해서만 문제가 발생할 수 있기 때문에,
		// AniObject계산 순서에 따라 문제없이 보일수도 있을 것이다.
		// 하지만 어쨌든 문제가 발생할 수 있는 부분이니 나중에 문제 발생시 확인해보면 좋을 것이다.
		if( m_hObject->GetAniHandle() && m_nLinkBoneIndex != -1 && pRender->GetObjectHandle() && pRender->GetObjectHandle()->GetAniHandle() == m_hObject->GetAniHandle() )
		{
			// 이럴때가 문제.
		}
	}

	ResetDefaultAction( nEquipIndex );

	m_bLinkBone = true;
}

void CDnWeapon::LinkWeapon( DnActorHandle hActor, CDnWeapon *pWeapon )
{
	if( !m_hObject ) return;
	m_pParentWeapon = pWeapon;
	m_hActor = hActor;

	std::string szLinkBoneName = "~BoxBone02";
	int nBoneIndex = m_pParentWeapon->GetBoneIndex( szLinkBoneName.c_str() );
	if(nBoneIndex==-1) {
		OutputDebug("Can't Find ~BoxBone02 \n");
		return;
	}
	m_hObject->SetParent( m_pParentWeapon->GetObjectHandle(), nBoneIndex );
	m_bLinkBone = true;
	m_nLinkBoneIndex = nBoneIndex;

	ResetDefaultAction( -1 );
}

void CDnWeapon::UnlinkWeapon()
{
	if( m_hObject )
		m_hObject->SetParent( CEtObject::Identity(), 0 );

	m_bLinkBone = false;
	m_nLinkBoneIndex = -1;
	m_pParentWeapon = NULL;
}

void CDnWeapon::ChangeLinkBone( const char *szBoneName )
{
	if( m_bLinkBone == false ) return;
	MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(m_hActor.GetPointer());
	if( pRender ) {
		int nBoneIndex = pRender->GetBoneIndex( szBoneName );
		if( nBoneIndex != -1 ) {
			m_hObject->SetParent( pRender->GetObjectHandle(), nBoneIndex );
			m_nLinkBoneIndex = nBoneIndex;

			// 위 LinkWeapon함수의 주석 참조.
			if( m_hObject->GetAniHandle() && m_nLinkBoneIndex != -1 && pRender->GetObjectHandle() && pRender->GetObjectHandle()->GetAniHandle() == m_hObject->GetAniHandle() )
			{
			}
		}
	}
}

CDnWeapon &CDnWeapon::operator = ( CDnWeapon &e )
{
	FreeObject();

	m_WeaponType = e.m_WeaponType;
	m_nClassID = e.m_nClassID;
	m_nLength = e.m_nLength;
	m_EquipType = e.m_EquipType;
	m_szFileName = e.m_szFileName;

	if( e.m_hObject ) {
		m_hObject = EternityEngine::CreateAniObject( e.m_hObject->GetSkin()->GetFullName(), ( e.m_hObject->GetAniHandle() ) ? e.m_hObject->GetAniHandle()->GetFullName() : NULL );
		if( m_hObject ) {
			m_hObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );
		}
	}
	LoadAction( CDnActionBase::m_szFileName.c_str() );

	for( int i=0; i<CDnActorState::ActorMaterialEnum_Amount; i++ ) {
		if( e.m_NormalHitEffect[i].nIndex != -1 ) CEtResource::GetResource( e.m_NormalHitEffect[i].nIndex )->AddRef();
		if( e.m_CriticalHitEffect[i].nIndex != -1 ) CEtResource::GetResource( e.m_CriticalHitEffect[i].nIndex )->AddRef();
		if( e.m_StunHitEffect[i].nIndex != -1 ) CEtResource::GetResource( e.m_StunHitEffect[i].nIndex )->AddRef();

		for( DWORD j=0; j<e.m_nVecNormalHitSound[i].size(); j++ ) {
			if( e.m_nVecNormalHitSound[i][j] != -1 ) CEtSoundEngine::GetInstance().AddSoundRef( e.m_nVecNormalHitSound[i][j] );
		}


		m_NormalHitEffect[i] = e.m_NormalHitEffect[i];
		m_CriticalHitEffect[i] = e.m_CriticalHitEffect[i];
		m_StunHitEffect[i] = e.m_StunHitEffect[i];
		m_nVecNormalHitSound[i] = e.m_nVecNormalHitSound[i];

		if( e.m_NormalHitEffect[i].hEtcObject ) {
			m_NormalHitEffect[i].hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
			m_NormalHitEffect[i].hEtcObject->Initialize( NULL, NULL, e.m_NormalHitEffect[i].hEtcObject->GetActionFileName() );
		}
		if( e.m_CriticalHitEffect[i].hEtcObject ) {
			m_CriticalHitEffect[i].hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
			m_CriticalHitEffect[i].hEtcObject->Initialize( NULL, NULL, e.m_CriticalHitEffect[i].hEtcObject->GetActionFileName() );
		}
		if( e.m_StunHitEffect[i].hEtcObject ) {
			m_StunHitEffect[i].hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
			m_StunHitEffect[i].hEtcObject->Initialize( NULL, NULL, e.m_StunHitEffect[i].hEtcObject->GetActionFileName() );
		}

	}

	if( e.m_DefenseEffect.nIndex != -1 ) CEtResource::GetResource( e.m_DefenseEffect.nIndex )->AddRef();
	m_DefenseEffect = e.m_DefenseEffect;
	if( e.m_DefenseEffect.hEtcObject ) {
		m_DefenseEffect.hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
		m_DefenseEffect.hEtcObject->Initialize( NULL, NULL, e.m_DefenseEffect.hEtcObject->GetActionFileName() );
	}

	if( e.m_FinishEffect.nIndex != -1 ) CEtResource::GetResource( e.m_FinishEffect.nIndex )->AddRef();
	m_FinishEffect = e.m_FinishEffect;
	if( e.m_FinishEffect.hEtcObject ) {
		m_FinishEffect.hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
		m_FinishEffect.hEtcObject->Initialize( NULL, NULL, e.m_FinishEffect.hEtcObject->GetActionFileName() );
	}

	for( DWORD i=0; i<e.m_nVecCriticalHitSound.size(); i++ ) {
		if( e.m_nVecCriticalHitSound[i] != -1 ) CEtSoundEngine::GetInstance().AddSoundRef( e.m_nVecCriticalHitSound[i] );
	}
	for( DWORD i=0; i<e.m_nVecStunHitSound.size(); i++ ) {
		if( e.m_nVecStunHitSound[i] != -1 ) CEtSoundEngine::GetInstance().AddSoundRef( e.m_nVecStunHitSound[i] );
	}
	for( DWORD i=0; i<e.m_nVecDefenseSound.size(); i++ ) {
		if( e.m_nVecDefenseSound[i] != -1 ) CEtSoundEngine::GetInstance().AddSoundRef( e.m_nVecDefenseSound[i] );
	}

	m_nVecCriticalHitSound = e.m_nVecCriticalHitSound;
	m_nVecStunHitSound = e.m_nVecStunHitSound;
	m_nVecDefenseSound = e.m_nVecDefenseSound;

	// 사운드 정보 읽는다.
	m_fSoundRange = e.m_fSoundRange;
	m_fRollOff = e.m_fRollOff;

	return *this;
}

bool CDnWeapon::InitializeClass()
{
	s_dwUniqueCount = 0;
	return true;
}

void CDnWeapon::ProcessClass( LOCAL_TIME LocalTime, float fDelta )
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		s_pVecProcessList[i]->Process( LocalTime, fDelta );
		if( s_pVecProcessList[i]->IsDestroy() ) 
		{
			CDnWeapon* pWeapon = s_pVecProcessList[i];
			SAFE_DELETE(pWeapon);
			i--;
		}
	}
}

void CDnWeapon::ReleaseClass()
{
	DeleteAllProcessObject();
}

void CDnWeapon::ReleaseClass( WeaponTypeEnum Type )
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		CDnWeapon* pWeapon = s_pVecProcessList[i];
		if( pWeapon->GetWeaponType() & Type ) {
			SAFE_DELETE( pWeapon );
			i--;
		}
	}
	if( s_pVecProcessList.size() > 0 ) {
		assert(0&&"Siva 불려주삼");
	}
}

DnWeaponHandle CDnWeapon::FindWeaponFromUniqueID( DWORD dwUniqueID )
{
	/*
	for( int i=0; i<GetItemCount(); i++ ) {
		if( GetItem(i)->GetUniqueID() == dwUniqueID ) return GetItem(i)->GetMySmartPtr();
	}
	return CDnActor::Identity();
	*/
	ScopeLock<CSyncLock> Lock(s_LockSearchMap);
	std::map<DWORD, DnWeaponHandle>::iterator it = s_dwMapUniqueSearch.find( dwUniqueID );
	if( it != s_dwMapUniqueSearch.end() ) return it->second;

	return CDnActor::Identity();
}

DnWeaponHandle CDnWeapon::FindWeaponFromSerialID( INT64 nSerialID )
{
	/*
	for( int i=0; i<GetItemCount(); i++ ) {
		if( GetItem(i)->GetSerialID() == nSerialID ) return GetItem(i)->GetMySmartPtr();
	}
	return CDnActor::Identity();
	*/
	CDnItem *pItem = FindItemFromSerialID( nSerialID );
	if( pItem == NULL || pItem->GetItemType() != ITEMTYPE_WEAPON ) return CDnWeapon::Identity();

	return ((CDnWeapon*)pItem)->GetMySmartPtr();
}

DnWeaponHandle CDnWeapon::CreateWeapon( int nWeaponTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, bool bProcess, bool bIncreaseUniqueCount, int nLookItemID )
{
	DnWeaponHandle hWeapon = (new CDnWeapon( bProcess, bIncreaseUniqueCount ))->GetMySmartPtr();
	hWeapon->Initialize( nWeaponTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );
	return hWeapon;
}

void CDnWeapon::CreateObject( CDnActor *pActor ,  bool bIgnoreSoundEffectForVillage )
{
	if( !CDnTableDB::IsActive() ) return;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox->IsExistItem( m_nClassID ) ) return;

	if( pActor ) m_hActor = pActor->GetMySmartPtr();
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (pFileNameSox == NULL)
	{
		_ASSERT(0);
		return;
	}

	std::string szSkinName, szAniName, szActName, szSimName;
	CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, "_SkinName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, "_AniName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szActName, pSox, m_nClassID, "_ActName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szSimName, pSox, m_nClassID, "_SimName", pFileNameSox);
#else
	std::string szSkinName = pSox->GetFieldFromLablePtr( m_nClassID, "_SkinName" )->GetString();
	std::string szAniName = pSox->GetFieldFromLablePtr( m_nClassID, "_AniName" )->GetString();
	std::string szActName = pSox->GetFieldFromLablePtr( m_nClassID, "_ActName" )->GetString();
	std::string szSimName = pSox->GetFieldFromLablePtr( m_nClassID, "_SimName" )->GetString();
#endif

	switch( GetEquipType() ) {
		case EquipTypeEnum::Gauntlet:
		case EquipTypeEnum::Glove:
#ifdef PRE_ADD_KALI
		case EquipTypeEnum::Charm:
#endif
#ifdef PRE_ADD_ASSASSIN
		case EquipTypeEnum::Crook:
#endif
#ifdef PRE_ADD_MACHINA
		case EquipTypeEnum::Claw:
#endif

			if( pActor && pActor->GetObjectHandle() && pActor->GetObjectHandle()->GetAniHandle() ) {
				szAniName = pActor->GetObjectHandle()->GetAniHandle()->GetFileName();
			}
			break;
	}
	if( !szSkinName.empty() ) {
		m_hObject = EternityEngine::CreateAniObject( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), ( szAniName.empty() ) ? NULL : CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );				
		if( !m_hObject ) return;
		m_hObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );
		if( !szSimName.empty() ) {
			m_hObject->CreateSimulation( szSimName.c_str() );
		}
		/*
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( m_hObject ) {
			if( m_hObject->GetMesh() ) m_hObject->GetMesh()->SetDeleteImmediate( true );
			if( m_hObject->GetAniHandle() ) m_hObject->GetAniHandle()->SetDeleteImmediate( true );
		}
#endif
		*/
	}

	if( !szActName.empty() ) LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );

	// 사운드 정보 읽는다.
	m_fSoundRange = pSox->GetFieldFromLablePtr( m_nClassID, "_Sound_Range" )->GetFloat() * 100.f;
	m_fRollOff = pSox->GetFieldFromLablePtr( m_nClassID, "_Sound_Rolloff" )->GetFloat();
	const char *szFileName;
#ifdef PRE_FIX_MEMOPT_EXT
	szFileName = CommonUtil::GetFileNameFromFileEXT(pSox, m_nClassID, "_EmptyDurabilitySound");
#else
	szFileName = pSox->GetFieldFromLablePtr( m_nClassID, "_EmptyDurabilitySound" )->GetString();
#endif
	if( szFileName && strlen( szFileName ) > 0 ) {
		m_nEmptyDurabilitySoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	}

	ResetDefaultAction(-1);

	m_bCreateObject = true;
	if( bIgnoreSoundEffectForVillage && CTaskManager::IsActive() ) {
		if( CTaskManager::GetInstance().GetTask( "LoginTask" ) && CDnMainFrame::GetInstance().GetMainThreadID() == GetCurrentThreadId() ) return;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( CTaskManager::GetInstance().GetTask( "VillageTask" ) && !CTaskManager::GetInstance().GetTask( "PVPLobbyTask" ) ) return;
#endif
	}

	// 사운드와 파티클 읽는다. 액션도 읽는다.
	char szStr[64], szActStr[64];
	for( int i=0; i<CDnActorState::ActorMaterialEnum_Amount; i++ ) {
		sprintf_s( szStr, "_Material%d_Normal_Particle", i+1 );
		sprintf_s( szActStr, "_Material%d_Normal_Action", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatNormalParticleFileName, szMatNormalActionFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatNormalParticleFileName, pSox, m_nClassID, szStr, pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szMatNormalActionFileName, pSox, m_nClassID, szActStr, pFileNameSox);
		LoadEffect( szMatNormalParticleFileName.c_str(), szMatNormalActionFileName.c_str(), &m_NormalHitEffect[i] );
#else
		LoadEffect( pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString(), pSox->GetFieldFromLablePtr( m_nClassID, szActStr )->GetString(), &m_NormalHitEffect[i] );
#endif

		sprintf_s( szStr, "_Material%d_Critical_Particle", i+1 );
		sprintf_s( szActStr, "_Material%d_Critical_Action", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatCriticalParticleFileName, szMatCriticalActionFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatCriticalParticleFileName, pSox, m_nClassID, szStr, pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szMatCriticalActionFileName, pSox, m_nClassID, szActStr, pFileNameSox);
		LoadEffect( szMatCriticalParticleFileName.c_str(), szMatCriticalActionFileName.c_str(), &m_CriticalHitEffect[i] );
#else
		LoadEffect( pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString(), pSox->GetFieldFromLablePtr( m_nClassID, szActStr )->GetString(), &m_CriticalHitEffect[i] );
#endif

		sprintf_s( szStr, "_Material%d_Stun_Particle", i+1 );
		sprintf_s( szActStr, "_Material%d_Stun_Action", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatStunParticleFileName, szMatStunActionFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatStunParticleFileName, pSox, m_nClassID, szStr, pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szMatStunActionFileName, pSox, m_nClassID, szActStr, pFileNameSox);
		LoadEffect( szMatStunParticleFileName.c_str(), szMatStunActionFileName.c_str(), &m_StunHitEffect[i] );
#else
		LoadEffect( pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString(), pSox->GetFieldFromLablePtr( m_nClassID, szActStr )->GetString(), &m_StunHitEffect[i] );
#endif

		sprintf_s( szStr, "_Material%d_Normal_Sound", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatNormalSoundFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatNormalSoundFileName, pSox, m_nClassID, szStr, pFileNameSox);
		LoadSound( szMatNormalSoundFileName.c_str(), m_nVecNormalHitSound[i] );
#else
		LoadSound( pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString(), m_nVecNormalHitSound[i] );
#endif
	}
#ifdef PRE_FIX_MEMOPT_EXT
	std::string szDefenseParticleFileName, szDefenseActionFileName;
	CommonUtil::GetFileNameFromFileEXT(szDefenseParticleFileName, pSox, m_nClassID, "_Defense_Particle", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szDefenseActionFileName, pSox, m_nClassID, "_Defense_Action", pFileNameSox);
	LoadEffect( szDefenseParticleFileName.c_str(), szDefenseActionFileName.c_str(), &m_DefenseEffect );

	std::string szFinishParticleFileName, szFinishActionFileName;
	CommonUtil::GetFileNameFromFileEXT(szFinishParticleFileName, pSox, m_nClassID, "_Finish_Particle", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szFinishActionFileName, pSox, m_nClassID, "_Finish_Action", pFileNameSox);
	LoadEffect( szFinishParticleFileName.c_str(), szFinishActionFileName.c_str(), &m_FinishEffect );

	std::string szSoundFileName;
	CommonUtil::GetFileNameFromFileEXT(szSoundFileName, pSox, m_nClassID, "_Critical_Sound", pFileNameSox);
	LoadSound( szSoundFileName.c_str(), m_nVecCriticalHitSound );

	CommonUtil::GetFileNameFromFileEXT(szSoundFileName, pSox, m_nClassID, "_Stun_Sound", pFileNameSox);
	LoadSound( szSoundFileName.c_str(), m_nVecStunHitSound );

	CommonUtil::GetFileNameFromFileEXT(szSoundFileName, pSox, m_nClassID, "_Defense_Sound", pFileNameSox);
	LoadSound( szSoundFileName.c_str(), m_nVecDefenseSound );

	CommonUtil::GetFileNameFromFileEXT(szSoundFileName, pSox, m_nClassID, "_Finish_Sound", pFileNameSox);
	LoadSound( szSoundFileName.c_str(), m_nVecFinishSound );
#else
	LoadEffect( pSox->GetFieldFromLablePtr( m_nClassID, "_Defense_Particle" )->GetString(), pSox->GetFieldFromLablePtr( m_nClassID, "_Defense_Action" )->GetString(), &m_DefenseEffect );
	LoadEffect( pSox->GetFieldFromLablePtr( m_nClassID, "_Finish_Particle" )->GetString(), pSox->GetFieldFromLablePtr( m_nClassID, "_Finish_Action" )->GetString(), &m_FinishEffect );

	LoadSound( pSox->GetFieldFromLablePtr( m_nClassID, "_Critical_Sound" )->GetString(), m_nVecCriticalHitSound );
	LoadSound( pSox->GetFieldFromLablePtr( m_nClassID, "_Stun_Sound" )->GetString(), m_nVecStunHitSound );
	LoadSound( pSox->GetFieldFromLablePtr( m_nClassID, "_Defense_Sound" )->GetString(), m_nVecDefenseSound );
	LoadSound( pSox->GetFieldFromLablePtr( m_nClassID, "_Finish_Sound" )->GetString(), m_nVecFinishSound );
#endif
}

void CDnWeapon::LoadEtcObject()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox->IsExistItem( m_nClassID ) ) return;
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (pFileNameSox == NULL)
	{
		_ASSERT(0);
		return;
	}
#endif
	// 액션도 읽는다.
	char szActStr[64];
	for( int i=0; i<CDnActorState::ActorMaterialEnum_Amount; i++ ) {
		sprintf_s( szActStr, "_Material%d_Normal_Action", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatNormalActionFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatNormalActionFileName, pSox, m_nClassID, szActStr, pFileNameSox);
		LoadEffect( NULL, szMatNormalActionFileName.c_str(), &m_NormalHitEffect[i] );
#else
		LoadEffect( NULL, pSox->GetFieldFromLablePtr( m_nClassID, szActStr )->GetString(), &m_NormalHitEffect[i] );
#endif

		sprintf_s( szActStr, "_Material%d_Critical_Action", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatCriticalActionFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatCriticalActionFileName, pSox, m_nClassID, szActStr, pFileNameSox);
		LoadEffect( NULL, szMatCriticalActionFileName.c_str(), &m_CriticalHitEffect[i] );
#else
		LoadEffect( NULL, pSox->GetFieldFromLablePtr( m_nClassID, szActStr )->GetString(), &m_CriticalHitEffect[i] );
#endif

		sprintf_s( szActStr, "_Material%d_Stun_Action", i+1 );
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szMatStunActionFileName;
		CommonUtil::GetFileNameFromFileEXT(szMatStunActionFileName, pSox, m_nClassID, szActStr, pFileNameSox);
		LoadEffect( NULL, szMatStunActionFileName.c_str(), &m_StunHitEffect[i] );
#else
		LoadEffect( NULL, pSox->GetFieldFromLablePtr( m_nClassID, szActStr )->GetString(), &m_StunHitEffect[i] );
#endif
	}

#ifdef PRE_FIX_MEMOPT_EXT
	std::string szDefendActionFileName, szFinishActionFileName;
	CommonUtil::GetFileNameFromFileEXT(szDefendActionFileName, pSox, m_nClassID, "_Defense_Action", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szFinishActionFileName, pSox, m_nClassID, "_Finish_Action", pFileNameSox);
	LoadEffect( NULL, szDefendActionFileName.c_str(), &m_DefenseEffect );
	LoadEffect( NULL, szFinishActionFileName.c_str(), &m_FinishEffect );
#else
	LoadEffect( NULL, pSox->GetFieldFromLablePtr( m_nClassID, "_Defense_Action" )->GetString(), &m_DefenseEffect );
	LoadEffect( NULL, pSox->GetFieldFromLablePtr( m_nClassID, "_Finish_Action" )->GetString(), &m_FinishEffect );
#endif
}

void CDnWeapon::FreeObject()
{
	/*
	if( m_hObject && m_bLinkBone ) {
		m_hObject->SetParent( CEtObject::Identity(), -1, NULL );
	}
	*/
	UnlinkWeapon();
	for( int i=0; i<CDnActorState::ActorMaterialEnum_Amount; i++ ) {
		FreeEffect( &m_NormalHitEffect[i] );
		FreeEffect( &m_CriticalHitEffect[i] );
		FreeEffect( &m_StunHitEffect[i] );

		FreeSound( m_nVecNormalHitSound[i] );
	}
	FreeEffect( &m_DefenseEffect );
	FreeEffect( &m_FinishEffect );

	if( m_nEmptyDurabilitySoundIndex != -1 ) {
		CEtSoundEngine::GetInstance().RemoveSound( m_nEmptyDurabilitySoundIndex );
		m_nEmptyDurabilitySoundIndex = -1;
	}
	FreeSound( m_nVecCriticalHitSound );
	FreeSound( m_nVecStunHitSound );
	FreeSound( m_nVecDefenseSound );

	SAFE_RELEASE_SPTR( m_hObject );
	SAFE_RELEASE_SPTR( m_hSwordTrail );
	FreeAction();
	ReleaseSignalImp();

	m_RecreateCashEquipType = (EquipTypeEnum)-1;
	m_bCreateObject = false;
}

void CDnWeapon::LoadEffect( const char *szFileName, const char *szActName, EffectStruct *pStruct )
{
	char szExt[32] = { 0, };
	if( szFileName && strlen(szFileName) > 0 ) {
		_GetExt( szExt, _countof(szExt), szFileName );
		if( _stricmp( szExt, "ptc" ) == NULL ) {
			pStruct->cType = 0;
			pStruct->nIndex = EternityEngine::LoadParticleData( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
		}
		else if( _stricmp( szExt, "eff" ) == NULL ) {
			pStruct->cType = 1;
			pStruct->nIndex = EternityEngine::LoadEffectData( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
		}
	}
	if( szActName && strlen(szActName) > 0 ) {
		DnEtcHandle hHandle = (new CDnEtcObject)->GetMySmartPtr();
		hHandle->Initialize( NULL, NULL, szActName );
		pStruct->hEtcObject = hHandle;
	}
}

void CDnWeapon::FreeEffect( EffectStruct *pStruct )
{
	if( pStruct->nIndex != -1 ) {
		switch( pStruct->cType ) {
			case 0: EternityEngine::DeleteParticleData( pStruct->nIndex ); break;
			case 1: EternityEngine::DeleteEffectData( pStruct->nIndex ); break;
		}
	}
	SAFE_RELEASE_SPTR( pStruct->hEtcObject );
	*pStruct = EffectStruct();
}

void CDnWeapon::LoadSound( const char *szFileName, std::vector<int> &nVecResult )
{
	if( szFileName == NULL || strlen(szFileName) == 0 ) return;

	CFileNameString szTemp;
	bool bFind;
	for( int j=0; ; j++ ) {
		szTemp = CEtResourceMng::GetInstance().GetFullNameRandom( CFileNameString(szFileName), j, &bFind );
		if( bFind == false ) break;
		int nIndex = CEtSoundEngine::GetInstance().LoadSound( szTemp.c_str(), true, false );
		if( nIndex == -1 ) break;
		nVecResult.push_back( nIndex );
	}
}

void CDnWeapon::FreeSound( std::vector<int> &nVecList )
{
	for( DWORD i=0; i<nVecList.size(); i++ ) {
		if( nVecList[i] == -1 ) continue;
		CEtSoundEngine::GetInstance().RemoveSound( nVecList[i] );
	}
	nVecList.clear();
}

void CDnWeapon::ShowTrail( LOCAL_TIME nLengthTime , const ShowSwordTrailStruct *pStruct  )
{	
	if( m_hSwordTrail && pStruct ) 
	{	
		m_hSwordTrail->AddPoint(-1, (int)nLengthTime );

		if( m_pSwordTrailStruct ) 
		{
			EtTextureHandle hTex = CEtResource::GetSmartPtr( ( pStruct->nTextureIndex != -1 ) ? pStruct->nTextureIndex : m_pSwordTrailStruct->nTextureIndex );
			EtTextureHandle hNormalTex = CEtResource::GetSmartPtr( ( pStruct->nNormalTextureIndex != -1 ) ? pStruct->nNormalTextureIndex : m_pSwordTrailStruct->nNormalTextureIndex );

			m_hSwordTrail->ChangeTexture( hTex, hNormalTex );

			if( pStruct->bUseTextureTechique == TRUE )
			{
				m_hSwordTrail->SetBlendOP( (EtBlendOP)( pStruct->nBlendOP + 1 ) );
				m_hSwordTrail->SetSrcBlend( (EtBlendMode)( GetValue2ExceptionSwap( pStruct->nSrcBlend, 1, 0, 4 ) + 1 ) );
				m_hSwordTrail->SetDestBlend( (EtBlendMode)( GetValue2ExceptionSwap( pStruct->nDestBlend, 1, 0, 5 ) + 1 ) );
				m_hSwordTrail->SetTechniqueIndex( pStruct->nTechIndex );
			}
			else
			{
				m_hSwordTrail->SetBlendOP( (EtBlendOP)( m_pSwordTrailStruct->nBlendOP + 1 ) );
				m_hSwordTrail->SetSrcBlend( (EtBlendMode)( GetValue2ExceptionSwap( m_pSwordTrailStruct->nSrcBlend, 1, 0, 4 ) + 1 ) );
				m_hSwordTrail->SetDestBlend( (EtBlendMode)( GetValue2ExceptionSwap( m_pSwordTrailStruct->nDestBlend, 1, 0, 5 ) + 1 ) );
				m_hSwordTrail->SetTechniqueIndex( m_pSwordTrailStruct->nTechIndex );
			}
		}
	}
}

void CDnWeapon::SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop )
{
	m_szDefaultAction = szActionName;
	CDnActionBase::SetAction( GetWeaponMatchAction( szActionName ), fFrame, fBlendFrame, bLoop );
}

void CDnWeapon::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame )
{
	CDnActionBase::SetActionQueue( GetWeaponMatchAction( szActionName ), nLoopCount, fBlendFrame, fStartFrame );
}

bool CDnWeapon::IsExistAction( const char *szAction )
{
	return CDnActionBase::IsExistAction( GetWeaponMatchAction( szAction ) );
}

const char *CDnWeapon::GetWeaponMatchAction( const char *szAction )
{
	std::string szTemp[2] = { szAction, szAction };
	char szStr[64];

	int nEnchantTableID = m_nEnchantTableID;
	char cEnchantLevel = m_cEnchantLevel;
	int nEnchantActionValue = m_nEnchantActionValue;
	if( IsCashItem() && m_hActor && m_nEquipIndex >= 0 && m_nEquipIndex <= 1 && m_hActor->CDnActor::GetWeapon(m_nEquipIndex) ) {
		DnWeaponHandle hNormalWeapon = m_hActor->CDnActor::GetWeapon(m_nEquipIndex);
		nEnchantTableID = hNormalWeapon->GetEnchantTableID();
		cEnchantLevel = hNormalWeapon->GetEnchantLevel();
		nEnchantActionValue = hNormalWeapon->GetEnchantActionValue();
	}

	if( !( nEnchantTableID < 1 || cEnchantLevel < 1 || nEnchantActionValue == 0 ) ) {
		sprintf_s( szStr, "_%d", nEnchantActionValue );
		szTemp[0] += szStr;
		szTemp[1] += szStr;
	}
	if( m_hActor && !m_hActor->GetAdditionalWeaponAction().empty() ) {
		sprintf_s( szStr, "_%s", m_hActor->GetAdditionalWeaponAction().c_str() );
		szTemp[1] += szStr;
	}

	for( int i=1; i>=0; i-- ) {
		ActionElementStruct *pStruct = GetElement( szTemp[i].c_str() );
		if( pStruct ) return pStruct->szName.c_str();
	}
	return szAction;
}

CDnItem &CDnWeapon::operator = ( TItem &e )
{
	CDnItem::operator = (e);
	m_nDurability = e.wDur;
	return *this;
}

void CDnWeapon::InsertUniqueSearchMap( CDnWeapon *pWeapon )
{
	ScopeLock<CSyncLock> Lock(s_LockSearchMap);
	if( pWeapon == NULL || pWeapon->GetUniqueID() == -1 ) return;

	s_dwMapUniqueSearch[pWeapon->GetUniqueID()] = pWeapon->GetMySmartPtr();
}

void CDnWeapon::RemoveUniqueSearchMap( CDnWeapon *pWeapon )
{
	ScopeLock<CSyncLock> Lock(s_LockSearchMap);
	if( pWeapon->GetUniqueID() == -1 ) return;

	std::map<DWORD, DnWeaponHandle>::iterator it = s_dwMapUniqueSearch.find( pWeapon->GetUniqueID() );
	if( it != s_dwMapUniqueSearch.end() ) s_dwMapUniqueSearch.erase( it );
}

void CDnWeapon::SetUniqueID( DWORD dwUniqueID ) 
{
	RemoveUniqueSearchMap( this );
	m_dwUniqueID = dwUniqueID; 
	InsertUniqueSearchMap( this );
}


CDnWeapon::EquipTypeEnum CDnWeapon::GetEquipType(ITEMCLSID itemId)
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox ) return (EquipTypeEnum)-1;
	if( !pSox->IsExistItem(itemId) ) return (EquipTypeEnum)-1;

	return (EquipTypeEnum)pSox->GetFieldFromLablePtr( itemId, "_EquipType" )->GetInteger();
}

void CDnWeapon::CreateBillboardEffect( int nIndex, MatrixEx &Cross )
{
	EternityEngine::CreateBillboardEffect( nIndex, Cross ); 
}

void CDnWeapon::CreateEffectObject( int nIndex, MatrixEx &Cross )
{
	EtResourceHandle hResource = CEtResource::GetResource( nIndex );
	if( hResource) {
		if( strstr( hResource->GetFileName(), "(rot)") != NULL )  {
			EtMatrix MatRot;
			EtMatrixRotationZ(&MatRot, ET_PI * 2 * (_rand() % 1023) / 1023.f );
			EtMatrixMultiply(&MatRot, &MatRot, Cross );
			EternityEngine::CreateEffectObject( nIndex, &MatRot); 
		}
		else {
			EternityEngine::CreateEffectObject( nIndex, Cross ); 
		}		
	}
}
void CDnWeapon::CreateEtcObject( DnEtcHandle hHandle, DnActorHandle hTarget, MatrixEx &Cross )
{
	DnEtcHandle hObject = (new CDnEtcObject)->GetMySmartPtr();
	hObject->Initialize( NULL, NULL, hHandle->GetActionFileName() );
	hObject->SetCross( &Cross );
	hObject->SetActionQueue( "Hit" );
	hObject->SetApplyLightObject( m_hActor->GetObjectHandle() ); //hTarget->GetObjectHandle()
}

void CDnWeapon::ResetDefaultAction( int nEquipIndex )
{
	if( IsExistAction( "Idle" ) ) SetAction( "Idle", 0.f, 0.f );
	if( m_hActor ) m_hActor->OnResetAttachWeaponAction( GetMySmartPtr(), nEquipIndex );
}

void CDnWeapon::RecreateCashWeapon( CDnActor *pActor, int nEquipIndex )
{
	if( !IsCreateObject() ) return;
	if( !pActor ) return;
	if( !pActor->IsPlayerActor() ) return;
	if( nEquipIndex < 0 || nEquipIndex > 1 ) return;
	if( !IsCashItem() ) return;

	DnWeaponHandle hMainWeapon = pActor->GetWeapon( nEquipIndex );
	if( !hMainWeapon ) return;
	if( hMainWeapon->GetEquipType() == m_RecreateCashEquipType ) return;
	switch( hMainWeapon->GetEquipType() ) {
		case Gauntlet:
		case Staff:
		case Shield:
		case Arrow:
		case Charm:
		case Bracelet:
		case KnuckleGear:
		case Claw:
			return;
	}

	m_RecreateCashEquipType = hMainWeapon->GetEquipType();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEAPON );
	if( !pSox->IsExistItem( m_nClassID ) ) return;

	static char *szSubStr[] = { "", "Sub1", "Sub2" };
	std::string szSkinName;
	std::string szAniName;
	std::string szActName;
	std::string szSimName;
	bool bValid = false;

	char szStr[64];
	for( int i=0; i<3; i++ ) {
		sprintf_s( szStr, "_EquipType%s", szSubStr[i] );
		int nEquipType = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetInteger();
		if( nEquipType == m_RecreateCashEquipType ) {
#ifdef PRE_FIX_MEMOPT_EXT
			sprintf_s( szStr, "_SkinName%s", szSubStr[i] );
			CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, szStr);
			sprintf_s( szStr, "_AniName%s", szSubStr[i] );
			CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, szStr);
			sprintf_s( szStr, "_ActName%s", szSubStr[i] );
			CommonUtil::GetFileNameFromFileEXT(szActName, pSox, m_nClassID, szStr);
			sprintf_s( szStr, "_SimName%s", szSubStr[i] );
			CommonUtil::GetFileNameFromFileEXT(szSimName, pSox, m_nClassID, szStr);
#else
			sprintf_s( szStr, "_SkinName%s", szSubStr[i] );
			szSkinName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
			sprintf_s( szStr, "_AniName%s", szSubStr[i] );
			szAniName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
			sprintf_s( szStr, "_ActName%s", szSubStr[i] );
			szActName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
			sprintf_s( szStr, "_SimName%s", szSubStr[i] );
			szSimName = pSox->GetFieldFromLablePtr( m_nClassID, szStr )->GetString();
#endif

			bValid = true;
			break;
		}
	}
	if( !bValid ) return;

	SAFE_RELEASE_SPTR( m_hObject );
	SAFE_RELEASE_SPTR( m_hSwordTrail );
	ReleaseSignalImp();
	FreeAction();

	int nMainWeaponClassID = hMainWeapon->GetClassID();
#ifdef PRE_FIX_MEMOPT_EXT
	if( szSkinName.empty() )	CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, nMainWeaponClassID, "_SkinName");
	if( szAniName.empty() )		CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, nMainWeaponClassID, "_AniName");
	if( szActName.empty() )		CommonUtil::GetFileNameFromFileEXT(szActName, pSox, nMainWeaponClassID, "_ActName");
	if( szSimName.empty() )		CommonUtil::GetFileNameFromFileEXT(szSimName, pSox, nMainWeaponClassID, "_SimName");
#else
	if( szSkinName.empty() ) szSkinName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_SkinName" )->GetString();
	if( szAniName.empty() ) szAniName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_AniName" )->GetString();
	if( szActName.empty() ) szActName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_ActName" )->GetString();
	if( szSimName.empty() ) szSimName = pSox->GetFieldFromLablePtr( nMainWeaponClassID, "_SimName" )->GetString();
#endif

	// 스킨이나 액트파일은 NONE일 경우 없으니 패스합니다.
	if( szAniName == "NONE" ) szAniName = "";
	if( szSimName == "NONE" ) szSimName = "";

	if( !szSkinName.empty() ) {
		m_hObject = EternityEngine::CreateAniObject( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), ( szAniName.empty() ) ? NULL : CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
		if( !m_hObject ) return;
		m_hObject->GetSkinInstance()->SetRenderAlphaTwoPass( true );
		if( !szSimName.empty() ) {
			m_hObject->CreateSimulation( szSimName.c_str() );
		}
		/*
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( m_hObject ) {
			if( m_hObject->GetMesh() ) m_hObject->GetMesh()->SetDeleteImmediate( true );
			if( m_hObject->GetAniHandle() ) m_hObject->GetAniHandle()->SetDeleteImmediate( true );
		}
#endif
		*/
	}

	if( !szActName.empty() ) {
		LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );
	}

	ResetDefaultAction( nEquipIndex );

	/*
	m_nEnchantTableID = hMainWeapon->m_nEnchantTableID;
	m_cEnchantLevel = hMainWeapon->m_cEnchantLevel;
	m_nEnchantActionValue = hMainWeapon->m_nEnchantActionValue;
	*/
}

#ifdef PRE_FIX_BOW_ACTION_MISMATCH
bool CDnWeapon::IsEnableNormalStandAction()
{
	//	CrossBow 와 SmallBow/Bigbow는 본셋팅부터 많이 달라서 Bigbow skin에 CrossBow 액션을 입혔을 경우 이상하게 셋팅되는 경우가 생긴다.
	//	Normal_Stand를 예외처리하는 부분에서도 같은 문제가 발생해서 수정. 예외처리를 하지 않을 수 있는 방법 필요.
	if (m_EquipType == SmallBow || m_EquipType == BigBow)
	{
		if (m_RecreateCashEquipType == CrossBow)
			return false;
	}

	return (IsExistAction( "Normal_Stand" ));
}
#endif

bool CDnWeapon::IsSubWeapon( CDnWeapon::EquipTypeEnum emType )
{
	switch( emType )
	{
	case CDnWeapon::Sword:
	case CDnWeapon::Axe:
	case CDnWeapon::Hammer:
	case CDnWeapon::SmallBow:
	case CDnWeapon::BigBow:
	case CDnWeapon::CrossBow:
	case CDnWeapon::Staff:
	case CDnWeapon::Mace:
	case CDnWeapon::Flail:
	case CDnWeapon::Wand:		
	case CDnWeapon::Cannon:
	case CDnWeapon::BubbleGun:	
	case CDnWeapon::Chakram:
	case CDnWeapon::Fan:
	case CDnWeapon::Scimiter:
	case CDnWeapon::Dagger:
	case CDnWeapon::Spear:
	case CDnWeapon::KnuckleGear:
	//case CDnWeapon::LE_UNK:
	//case CDnWeapon::MC_UNK:
		return false;
	case CDnWeapon::Shield:
	case CDnWeapon::Arrow:
	case CDnWeapon::Book:
	case CDnWeapon::Orb:
	case CDnWeapon::Puppet:
	case CDnWeapon::Gauntlet:	
	case CDnWeapon::Glove:		
	case CDnWeapon::Charm:
	case CDnWeapon::Crook:
	case CDnWeapon::Bracelet:
	case CDnWeapon::Claw:
		return true;
	default:
		CDebugSet::ToLogFile( "CDnInventory::IsSubWeapon, default case(%d)", emType );
		ASSERT(0&&"CDnInventory::IsSubWeapon");
		break;
	}
	return false;
}