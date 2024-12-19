#include "StdAfx.h"
#include "DnActionSignalImp.h"
#include "DnEtcObject.h"
#include "DnWorld.h"
#include "EtActionSignal.h"
#include "DnActor.h"
#include "DnWorldActProp.h"
#include "EtOptionController.h"
#include "DnLoadingTask.h"
#include "EtRTTRenderer.h"
#ifdef PRE_ADD_MARK_PROJECTILE
#include "DnProjectile.h"
#endif // PRE_ADD_MARK_PROJECTILE


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnActionSignalImp::CDnActionSignalImp() : m_bPlaySignalSound( true ) 
, m_vActionSignalScale( 1.f, 1.f, 1.f )
, m_bRTTRenderMode( false )
, m_pRTTRenderer( NULL )
{
	memset( &m_CurVoiceChannelData, 0, sizeof(m_CurVoiceChannelData) );
}

CDnActionSignalImp::~CDnActionSignalImp()
{
	ReleaseSignalImp( All, false );
}

void CDnActionSignalImp::ReleaseSignalImp( DWORD dwMask/* = All*/, bool bSoftDestruct )
{
	bool bUseLock = false;
	if( CDnLoadingTask::IsActive() && CDnLoadingTask::GetInstance().GetThreadID() == GetCurrentThreadId() ) bUseLock = true;
	ScopeLock< CSyncLock > Lock( g_pEtRenderLock, bUseLock );

	if( dwMask & EtcObject ) {
		for( DWORD i=0; i<TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetCount(); i++ ) {
			EtcObjectSignalStruct *pStruct = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetStruct(i);
			if( pStruct->bDefendenceParent ) {
				TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveObject(i);
				i--;
			}
		}
	}

	if( dwMask & ParticleSmartPtrSignal )
		for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetCount(); i++ ) {
			NormalParticleSignalStruct *pStruct = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetStruct(i);
			if( pStruct->bDefendenceParent || !bSoftDestruct ) {
				TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveObject(i);
				i--;
			}
			else {
				EtBillboardEffectHandle hHandle = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetObject(i);
				if( hHandle ) hHandle->StopPlay();
			}
		}
//		TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveAllSignalHandle();

	if( dwMask & LinkParticle )
		TSmartPtrSignalImp<EtBillboardEffectHandle,LinkParticleSignalStruct>::RemoveAllSignalHandle();

	if( dwMask & EffectSmartPtr )
		TSmartPtrSignalImp<EtEffectObjectHandle,TSmartPtrSignalStruct::Default>::RemoveAllSignalHandle();

	if( dwMask & LinkFX )
		TSmartPtrSignalImp<EtEffectObjectHandle,LinkFXSignalStruct>::RemoveAllSignalHandle();

	if( dwMask & ShaderCustomParamValue )
		TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::RemoveAllSignalHandle( false );

	if( dwMask & DecalSignal )
		TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>::RemoveAllSignalHandle( false );

	if( dwMask & OutlineSignal )
		TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::RemoveAllSignalHandle();

	if( dwMask & SoundChannelSignal ) {
		for( DWORD i=0; i<TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetCount(); i++ ) {
			SoundSignalStruct *pStruct = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetStruct(i);
			// 바로 없어져야하는 루프가 아닌 액션의 경우 문제가 됩니다.
			// 하지만 현제 바로 없어지지 않게 되있는거에 마춰서 작업된것들이 있어서 아직 못풉니다. ( 뒤에 fFadeDelta 체크 )
			if( pStruct->bLoop /*|| pStruct->fFadeDelta == 0.f*/ ) {
				// 이렇게 하면 자연스럽게 현제 플레이를 마지막으로 끝나야하지만 안끝난다. -_-
				// 그냥 삭제
				/*
				EtSoundChannelHandle hChannel = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetObject(i);
				if( hChannel ) {
					hChannel->SetLoopCount(0);
				}
				*/
				TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::RemoveObject( i, true );
				i--;
			}
		}
		if( m_CurVoiceChannelData.bLoop ) {
			SAFE_RELEASE_SPTR( m_hCurVoiceChannel );
		}
		/*
		TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::RemoveAllSignalHandle();
		SAFE_RELEASE_SPTR( m_hCurVoiceChannel );
		*/
	}
	if( dwMask & EyeLightTrailSignal ) {
		TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::RemoveAllSignalHandle();
	}

	if( dwMask & EffectTrailSignal ) {
		TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::RemoveAllSignalHandle();
	}

	TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::RemoveAllSignalHandle();
	TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::RemoveAllSignalHandle();
}


EtMatrix CDnActionSignalImp::GetAniBoneMatrix( const char *szBoneName )
{
	EtMatrix mat;
	EtMatrixIdentity(&mat);
	CDnActor* pActor = dynamic_cast<CDnActor*>(this);
	if( pActor )
	{
		mat = pActor->GetBoneMatrix( szBoneName );
	}
	else 
	{
		CDnWorldActProp* pWorldProp  = dynamic_cast<CDnWorldActProp*>(this);
		if( pWorldProp ) {
			mat = pWorldProp->GetBoneMatrix( szBoneName );
		}
		else {
			CDnEtcObject *pEtcObject = dynamic_cast<CDnEtcObject*>(this);
			if( pEtcObject ) {
				mat = pEtcObject->GetBoneMatrix( szBoneName );
			}
			else {
				CDnActionRenderBase* pRenderBase  = dynamic_cast<CDnActionRenderBase*>(this);
				if( pRenderBase ) {
					mat = pRenderBase->GetBoneMatrix( szBoneName );
				}
				else {			
					ASSERT( 0 && "GetAniBoneMatrix 실패!!!");
				}
			}
		}
	}
	return mat;
}

EtMatrix CDnActionSignalImp::GetDummyBoneMatrix( const char *szBoneName )
{
	EtMatrix mat;
	EtMatrixIdentity(&mat);

	if( GetAniObjectHandle() )
	{
		EtMeshHandle hMesh = GetAniObjectHandle()->GetMesh();
		if( hMesh )
		{
			char *pParentName = hMesh->GetDummyParentByName( szBoneName );

			if( pParentName )
			{
				int nBoneIndex = GetAniObjectHandle()->GetAniHandle()->GetBoneIndex( pParentName );
				if( nBoneIndex != -1 )
				{
					EtMatrix *pmatBoneTransMat = GetAniObjectHandle()->GetBoneTransMat( nBoneIndex );
					if( pmatBoneTransMat ) 
					{
						EtMatrix &MatDummyMat = hMesh->GetDummyMatByName( szBoneName );
						EtMatrixMultiply( &mat, pmatBoneTransMat, GetAniObjectHandle()->GetWorldMat() ); 
						EtMatrixMultiply( &mat, &MatDummyMat, &mat );
					}
				}
			}
		}
	}

	return mat;
}

void CDnActionSignalImp::SetRTTRenderMode( bool bRTTRender, CEtRTTRenderer *pRTTRenderer )
{
	m_bRTTRenderMode = bRTTRender;
	if( bRTTRender )
		m_pRTTRenderer = pRTTRenderer;
	else
		m_pRTTRenderer = NULL;
}

EtBillboardEffectHandle CDnActionSignalImp::CreateBillboardEffect( int nIndex, MatrixEx &Cross )
{
	if( m_bRTTRenderMode && m_pRTTRenderer )
		return m_pRTTRenderer->CreateBillboardEffect( nIndex, Cross, false );	// Reuse플래그. 이거 켜면 RTT및 캐릭터 전체의 부모가 바뀔때 제대로 적용되지 않아 여기선 false로 설정한다.
	else
		return EternityEngine::CreateBillboardEffect( nIndex, Cross ); 
}

EtEffectObjectHandle CDnActionSignalImp::CreateEffectObject( int nIndex, MatrixEx &Cross )
{
	if( m_bRTTRenderMode && m_pRTTRenderer )
		return m_pRTTRenderer->CreateEffectObject( nIndex, Cross );
	else
		return EternityEngine::CreateEffectObject( nIndex, Cross );
}

EtAniObjectHandle CDnActionSignalImp::CreateAniObject( const char *pSkinName, const char *pAniName )
{
	if( m_bRTTRenderMode && m_pRTTRenderer )
		return m_pRTTRenderer->CreateAniObject( pSkinName, pAniName );
	else
		return EternityEngine::CreateAniObject( pSkinName, pAniName );
}

void CDnActionSignalImp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	// FX, ActionObject, Particle 등등
	// 기존에 넣어준것중 관리가 필요없을 경우 걍 지우진 않고(지우면 사라지기 때문에 안된다. ) 인덱싱을 -1로 바꾸는 이유.
	// 생성된 게 Loop 가 아니지만 들어왔을때 리스트에서 지워주게 되면 나중에 안지워지고 있는 객체를
	// 종료 시 삭제할 수 없게 된다. ( 리소스로 영원히 남아버리기 땜시.. )
	// 그래서 이런 경우엔 얻어올 일이 없고 아래에 인덱스와 겹치게 되므로 상위에서 생성된
	// 객체를 ActionIndex 와 SignalIndex 로 얻을때 정상적이지 못한걸 얻게될 수 있으므로
	// 리스트에 들어가 있는 객체의 인덱싱을 강제로 -1 로 바꿔버리게 한다.

	EtVector3 vScale = EtVector3(1.0f, 1.0f, 1.0f);

	//#28239 관련 - 서버쪽 Hit판정과 동일하게 스케일값 계산
	MAActorRenderBase* pRenderBase = dynamic_cast<MAActorRenderBase*>(GetRenderBase());
	if (pRenderBase)
		vScale = *pRenderBase->GetScale();

	switch( Type ) {
		case STE_Particle:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;

				ParticleStruct *pStruct = (ParticleStruct *)pPtr;
				if( pStruct->nParticleDataIndex == -1 ) break;
				if( pStruct->bLoop ) {
					if( pStruct->bLinkObject ) {
						if( TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
						TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
					}
					else {
						if( TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
						TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
					}
				}
				else {
					if( pStruct->bLinkObject ) {
						TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetStruct( m_nActionIndex, nSignalIndex );
						if( pSmartStruct ) {
							TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
//							pSmartStruct->nActionIndex = pSmartStruct->nSignalIndex = -1;
						}
					}
					else {
						TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetStruct( m_nActionIndex, nSignalIndex );
						if( pSmartStruct ) {
							TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
//							pSmartStruct->nActionIndex = pSmartStruct->nSignalIndex = -1;
						}
					}
				}

				MatrixEx Cross;
				Cross = *GetObjectCross();

				Cross.MoveLocalZAxis( pStruct->vPosition->z * vScale.y );
				Cross.MoveLocalXAxis( pStruct->vPosition->x * vScale.y );
				if( pStruct->bClingTerrain ) {
					Cross.m_vPosition.y = INSTANCE(CDnWorld).GetHeightWithProp( Cross.m_vPosition );
				}
				Cross.MoveLocalYAxis( pStruct->vPosition->y * m_vActionSignalScale.y );

				if( pStruct->szBoneName && strlen(pStruct->szBoneName) > 0 ) { 
					EtMatrix mat = GetAniBoneMatrix( pStruct->szBoneName );
					memcpy( &Cross.m_vXAxis, &mat._11, sizeof(EtVector3) );
					memcpy( &Cross.m_vYAxis, &mat._21, sizeof(EtVector3) );
					memcpy( &Cross.m_vZAxis, &mat._31, sizeof(EtVector3) );
					Cross.m_vPosition += *(EtVector3*)&mat._41;
					Cross.m_vPosition -= GetObjectCross()->m_vPosition;
				}

				if( pStruct->vRotation ) {
					Cross.RotateYaw( pStruct->vRotation->y );
					Cross.RotateRoll( pStruct->vRotation->z );
					Cross.RotatePitch( pStruct->vRotation->x );
				}

				/*
				if( pStruct->bLinkObject ) {
					if( pStruct->bLoop && TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
					TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
				}
				else {
					if( pStruct->bLoop && TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
					TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
				}
				*/

				EtBillboardEffectHandle hParticle = CreateBillboardEffect( pStruct->nParticleDataIndex, Cross );
				if( !hParticle ) break;
				hParticle->EnableTracePos( ( pStruct->bIterator == TRUE ) ? true : false );
				hParticle->EnableLoop( ( pStruct->bLoop == TRUE ) ? true : false );
				hParticle->SetScale( ( pStruct->fScale + 1.f ) * m_vActionSignalScale.z );
			
				//hParticle->EnableReduceFillRate(  pStruct->bOptimize );
				if( pStruct->nCullDistance != 0 ) {
					hParticle->SetCullDist( pStruct->nCullDistance * 100.0f );
				}
				hParticle->EnableCull( !pStruct->bDisableCull );

				if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
					hParticle->EnableCull( false );

				if( pStruct->bLinkObject ) {
					LinkParticleSignalStruct *pResult = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hParticle );
					pResult->vOffset = *pStruct->vPosition * vScale.y;					
					if( pStruct->vRotation ) {
						pResult->vRotate = *pStruct->vRotation;
					}
					else {
						pResult->vRotate = EtVector3(0,0,0);
					}
					pResult->bClingTerrain = ( pStruct->bClingTerrain == TRUE );
					pResult->szBoneName[0] = '\0';
					pResult->bDefendenceParent = ( pStruct->bDefendenceParent == TRUE ) ? true : false;
					pResult->bDeleteByChangeAction = ( pStruct->bDeleteByChangeAction == TRUE ) ? true : false;
					if( pStruct->szBoneName && strlen(pStruct->szBoneName)>0 )
						_strcpy(pResult->szBoneName, _countof(pResult->szBoneName), pStruct->szBoneName, (int)strlen(pStruct->szBoneName));
				}
				else {
					NormalParticleSignalStruct *pResult = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hParticle );
					pResult->bDefendenceParent = ( pStruct->bDefendenceParent == TRUE ) ? true : false;
					pResult->bDeleteByChangeAction = ( pStruct->bDeleteByChangeAction == TRUE ) ? true : false;
				}
				//OutputDebug( "Particle %d ( %d, %d )\n", nSignalIndex, (int)SignalStartTime, (int)SignalEndTime );
			}
			break;
		case STE_Particle_LoopEnd:
			{
				Particle_LoopEndStruct *pStruct = (Particle_LoopEndStruct *)pPtr;
				std::string szApplyAction = ( pStruct->szApplyActionName ) ? pStruct->szApplyActionName : "";
				int nActionIndex = GetElementIndex( szApplyAction.c_str() );
				for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetCount(); i++ ) {
					EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetObject(i);
					if( !hParticle ) continue;
					if( nActionIndex != -1 ) {
						TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::SmartPtrSignalImpStruct *pSignalStruct = (TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetStruct(i);
						if( pSignalStruct && pSignalStruct->nActionIndex != nActionIndex ) continue;
					}
					if( stricmp( hParticle->GetBillboardEffectData()->GetFileName(), pStruct->szFileName ) == NULL ) {
						hParticle->StopPlay();
						TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::RemoveObject( i , false);
						--i;
					}
				}
				for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetCount(); i++ ) {
					EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetObject(i);
					if( !hParticle ) continue;
					if( nActionIndex != -1 ) {
						TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::SmartPtrSignalImpStruct *pSignalStruct = (TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetStruct(i);
						if( pSignalStruct && pSignalStruct->nActionIndex != nActionIndex ) continue;
					}
					if( stricmp( hParticle->GetBillboardEffectData()->GetFileName(), pStruct->szFileName ) == NULL ) {
						hParticle->StopPlay();
						TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveObject( i , false);
						--i;
					}
				}
			}
			break;
		case STE_FX:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;

				FXStruct *pStruct = (FXStruct *)pPtr;
				if( pStruct->nFXDataIndex == -1 ) break;

				if( pStruct->bLoop ) {
					if( pStruct->bLinkObject ) {
						if( TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
						TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
					}
					else {
						if( TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
						TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
					}
				}
				else {
					if( pStruct->bLinkObject ) {
						TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetStruct( m_nActionIndex, nSignalIndex );
						if( pSmartStruct ) {
							TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
						}
					}
					else {
						TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetStruct( m_nActionIndex, nSignalIndex );
						if( pSmartStruct ) {
							TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
						}
					}
				}

				MatrixEx Cross = *GetObjectCross();
				Cross.MoveLocalZAxis( pStruct->vOffset->z * vScale.y );
				Cross.MoveLocalXAxis( pStruct->vOffset->x * vScale.y );
				if( pStruct->bClingTerrain ) {
					Cross.m_vPosition.y = INSTANCE(CDnWorld).GetHeightWithProp( Cross.m_vPosition );
				}

				float fTraceHitProjectileActorAdditionalHeight = 0.0f;
				if( TRUE == pStruct->bUseTraceHitProjectileActorAdditionalHeight )
					fTraceHitProjectileActorAdditionalHeight = GetTraceHitProjectileActorAdditionalHeight();
				
				// 추적중인 액터가 있는 발사체라면 원래 지정되어있던 오프셋값은 무시하고 
				// 추적중인 액터의 높이를 얻어와서 지정해준다. 
				if( 0.0f < fTraceHitProjectileActorAdditionalHeight )
				{
					Cross.MoveLocalYAxis( fTraceHitProjectileActorAdditionalHeight );
				}
				else
				{
					Cross.MoveLocalYAxis( pStruct->vOffset->y * m_vActionSignalScale.y );
				}

				if( pStruct->szBoneName && strlen(pStruct->szBoneName) > 0) {
					EtMatrix mat = GetAniBoneMatrix( pStruct->szBoneName );
					
					memcpy( &Cross.m_vXAxis, &mat._11, sizeof(EtVector3) );
					memcpy( &Cross.m_vYAxis, &mat._21, sizeof(EtVector3) );
					memcpy( &Cross.m_vZAxis, &mat._31, sizeof(EtVector3) );
					Cross.m_vPosition += *(EtVector3*)&mat._41;
					Cross.m_vPosition -= GetObjectCross()->m_vPosition;
				}
				if( pStruct->vRotation ) {
					Cross.RotateYaw( pStruct->vRotation->y );
					Cross.RotateRoll( pStruct->vRotation->z );
					Cross.RotatePitch( pStruct->vRotation->x );
				}

				EtEffectObjectHandle hHandle = CreateEffectObject( pStruct->nFXDataIndex, Cross );
				if( !hHandle ) break;
				hHandle->SetScale( ( pStruct->fScale + 1.f ) * m_vActionSignalScale.z );
				hHandle->SetLoop( pStruct->bLoop ? true : false );
				hHandle->EnableCull( !pStruct->bDisableCull );

				// 컷신 재생중엔 컬링 자체를 끔.
				if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
					hHandle->EnableCull( false );

				if( pStruct->bLinkObject ) {
					LinkFXSignalStruct *pResult = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hHandle );
					pResult->vOffset = *pStruct->vOffset * vScale.y;
					if( pStruct->vRotation ) {
						pResult->vRotate = *pStruct->vRotation;
					}
					else {
						pResult->vRotate = EtVector3(0,0,0);
					}
					pResult->szBoneName[ 0 ] = '\0';
					if( pStruct->szBoneName && strlen(pStruct->szBoneName)>0 ) {
						_strcpy(pResult->szBoneName, _countof(pResult->szBoneName), pStruct->szBoneName, (int)strlen(pStruct->szBoneName));
					}
					pResult->bClingTerrain = ( pStruct->bClingTerrain == TRUE );
					pResult->bUseTraceHitProjectileActorAdditionalHeight = ( pStruct->bUseTraceHitProjectileActorAdditionalHeight == TRUE );
				}
				else {
					TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hHandle );
				}
			}
			break;
		case STE_FX_LoopEnd:
			{
				FX_LoopEndStruct *pStruct = (FX_LoopEndStruct *)pPtr;
				for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetCount(); i++ ) {
					EtEffectObjectHandle hEffect = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetObject(i);
					if( !hEffect ) continue;
					if( stricmp( hEffect->GetEffectFileName(), pStruct->szFileName ) == NULL ) {
						hEffect->SetLoop( false );
						TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::RemoveObject( i ,( pStruct->bImmediate == TRUE ) ? true : false );
						--i;
					}
				}
				for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
					EtEffectObjectHandle hEffect = TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetObject(i);
					if( !hEffect ) continue;
					if( stricmp( hEffect->GetEffectFileName(), pStruct->szFileName ) == NULL ) {
						hEffect->SetLoop( false );
						TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveObject( i ,( pStruct->bImmediate == TRUE ) ? true : false );
						--i;
					}
				}
			}
			break;
		case STE_Decal:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;

				DecalStruct *pStruct = (DecalStruct *)pPtr;
				EtTextureHandle hTexture = pStruct->nTextureIndex == -1 ? CEtTexture::Identity() : CEtResource::GetSmartPtr( pStruct->nTextureIndex );
				if( hTexture ) {
					MatrixEx Cross = *GetObjectCross();
					Cross.MoveLocalZAxis( pStruct->vPosition->z * vScale.y );
					Cross.MoveLocalXAxis( pStruct->vPosition->x * vScale.y );
					EtDecalHandle hDecal = (new CEtDecal)->GetMySmartPtr();

					float fAngle = EtToDegree(atan2f(Cross.m_vZAxis.x, Cross.m_vZAxis.z));

					struct CalcDecalHeight : public CalcHeightFn
					{
						virtual float GetHeight( float fX, float fY, float fZ, int nTileScale )  override {
							return INSTANCE(CDnWorld).GetHeightWithProp( EtVector3(fX, fY, fZ), NULL, NULL, nTileScale );
						}
					};

					hDecal->Initialize( hTexture, Cross.m_vPosition.x, Cross.m_vPosition.z, pStruct->fRadius,
						pStruct->fLifeTime, pStruct->fAppearTime, fAngle + pStruct->fRotate, EtColor(1,1,1,1), pStruct->nSrcBlend, pStruct->nDestBlend, &CalcDecalHeight() , Cross.m_vPosition.y );

					if( pStruct->bLinkObject ) {
						LinkDecalSignalStruct *pResult = TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hDecal );
						pResult->fRadius = pStruct->fRadius;
						pResult->fRotate = pStruct->fRotate;
						if( pStruct->vPosition ) {
							pResult->vOffset = *pStruct->vPosition;
						}
					}
				}
			}
			break;
#ifdef PRE_ADD_MARK_PROJECTILE
		case STE_MarkProjectile:
			{
				CDnProjectile* pDnProjectile = dynamic_cast<CDnProjectile*>( this );
				if( pDnProjectile == NULL ) break;
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;

				MarkProjectileStruct* pStruct = (MarkProjectileStruct*)pPtr;

				switch( pStruct->nMarkType )
				{
					case PROJECTILE_MARK_DECAL:
						{
							EtTextureHandle hTexture = pStruct->nTextureIndex == -1 ? CEtTexture::Identity() : CEtResource::GetSmartPtr( pStruct->nTextureIndex );
							if( hTexture ) 
							{
								MatrixEx Cross;
								Cross.Identity();
								Cross.m_vPosition = *( pDnProjectile->GetTargetPosition() );
								Cross.MoveLocalZAxis( pStruct->vPosition->z * vScale.y );
								Cross.MoveLocalXAxis( pStruct->vPosition->x * vScale.y );

								EtDecalHandle hDecal = (new CEtDecal)->GetMySmartPtr();
								struct CalcDecalHeight : public CalcHeightFn
								{
									virtual float GetHeight( float fX, float fY, float fZ, int nTileScale )  override 
									{
										return INSTANCE(CDnWorld).GetHeightWithProp( EtVector3(fX, fY, fZ), NULL, NULL, nTileScale );
									}
								};

								if( hDecal )
								{
									hDecal->Initialize( hTexture, Cross.m_vPosition.x, Cross.m_vPosition.z, pStruct->fRadius, pStruct->fLifeTime, pStruct->fAppearTime, pStruct->fRotate, 
														EtColor(1,1,1,1), pStruct->nSrcBlend, pStruct->nDestBlend, &CalcDecalHeight(), Cross.m_vPosition.y );
								}
							}
						}
						break;
					case PROJECTILE_MARK_FX:
						{
							if( pStruct->nFXDataIndex == -1 ) break;

							if( pStruct->bLoop ) 
							{
								if( TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
								TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
							}
							else 
							{
								TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetStruct( m_nActionIndex, nSignalIndex );
								if( pSmartStruct ) 
									TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
							}

							MatrixEx Cross;
							Cross.Identity();
							Cross.m_vPosition = *( pDnProjectile->GetTargetPosition() );
							Cross.MoveLocalZAxis( pStruct->vPosition->z * vScale.y );
							Cross.MoveLocalXAxis( pStruct->vPosition->x * vScale.y );
							Cross.MoveLocalYAxis( pStruct->vPosition->y * m_vActionSignalScale.y );

							EtEffectObjectHandle hHandle = CreateEffectObject( pStruct->nFXDataIndex, Cross );
							if( hHandle )
							{
								hHandle->SetScale( ( pStruct->fScale + 1.f ) * m_vActionSignalScale.z );
								hHandle->SetLoop( pStruct->bLoop ? true : false );
								TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hHandle );
							}
						}
						break;
					case PROJECTILE_MARK_PARTICLE:
						{
							if( pStruct->nParticleDataIndex == -1 ) break;

							if( pStruct->bLoop )
							{
								if( TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;
								TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex );
							}
							else 
							{
								TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetStruct( m_nActionIndex, nSignalIndex );
								if( pSmartStruct ) 
									TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
							}

							MatrixEx Cross;
							Cross.Identity();
							Cross.m_vPosition = *( pDnProjectile->GetTargetPosition() );
							Cross.MoveLocalZAxis( pStruct->vPosition->z * vScale.y );
							Cross.MoveLocalXAxis( pStruct->vPosition->x * vScale.y );
							Cross.MoveLocalYAxis( pStruct->vPosition->y * m_vActionSignalScale.y );

							EtBillboardEffectHandle hParticle = CreateBillboardEffect( pStruct->nParticleDataIndex, Cross );
							if( hParticle )
							{
								hParticle->SetScale( ( pStruct->fScale + 1.f ) * m_vActionSignalScale.z );
								hParticle->EnableLoop( ( pStruct->bLoop == TRUE ) ? true : false );
								NormalParticleSignalStruct *pResult = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hParticle );
							}
						}
						break;
				}
			}
			break;
#endif // PRE_ADD_MARK_PROJECTILE
		case STE_ActionObject:
			{
				ActionObjectStruct *pStruct = (ActionObjectStruct *)pPtr;
				DnEtcHandle hHandle = (new CDnEtcObject)->GetMySmartPtr();
				if( hHandle->Initialize( pStruct->szSkinName, pStruct->szAniName, pStruct->szActionName ) == false ) {
					SAFE_RELEASE_SPTR( hHandle );
					break;
				}

#ifdef PRE_ADD_ANIOBJ_SHADOWOFF
				if (pStruct->bShadowOff)
				{
					EtAniObjectHandle hAniHandle = hHandle->GetAniObjectHandle();
					if (hAniHandle)
						hAniHandle->EnableShadowCast(false);
				}
#endif
	
#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL				
				CDnActor* pPlayerActor = dynamic_cast<CDnActor*>(this);
				if( pPlayerActor && pPlayerActor->IsPlayerActor() ) 
				{
					hHandle->SetPlayerActor( pPlayerActor->GetActorHandle() );
				}
				else if( !pPlayerActor )
				{
					CDnProjectile* pProjectile = dynamic_cast<CDnProjectile*>(this);
					if( pProjectile && pProjectile->GetLinkActor() )
					{
						hHandle->SetPlayerActor( pProjectile->GetLinkActor() );
					}
				}
#endif 
				*hHandle->GetMatEx() = *GetObjectCross();

				hHandle->GetMatEx()->MoveLocalZAxis( pStruct->vOffset->z * vScale.y );
				hHandle->GetMatEx()->MoveLocalXAxis( pStruct->vOffset->x * vScale.y );
				if( pStruct->bClingTerrain ) {
					hHandle->GetMatEx()->m_vPosition.y = INSTANCE(CDnWorld).GetHeightWithProp( hHandle->GetMatEx()->m_vPosition );
				}
				hHandle->GetMatEx()->MoveLocalYAxis( pStruct->vOffset->y * m_vActionSignalScale.y );

				hHandle->GetMatEx()->RotatePitch( pStruct->vRotate->x );
				hHandle->GetMatEx()->RotateYaw( pStruct->vRotate->y );
				hHandle->GetMatEx()->RotateRoll( pStruct->vRotate->z );

				hHandle->SetActionQueue( pStruct->szAction );
				
				EtVector3 vScale = *GetActionSignalScale();
				if( pStruct->vScale ) {
					vScale.x *= (pStruct->vScale->x + 1.0f);
					vScale.y *= (pStruct->vScale->y + 1.0f);
					vScale.z *= (pStruct->vScale->z + 1.0f);
				}
				hHandle->SetScale( vScale );

				TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::SmartPtrSignalImpStruct *pSmartStruct = (TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::GetStruct( m_nActionIndex, nSignalIndex );
				if( pSmartStruct ) {
					TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
//					pSmartStruct->nActionIndex = pSmartStruct->nSignalIndex = -1;
				}

				EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hHandle );
				pResult->bLinkObject = ( pStruct->bLinkObject == TRUE ) ? true : false;
				pResult->bDefendenceParent = ( pStruct->bDefendenceParent == TRUE ) ? true : false;
				if( pStruct->bLinkObject ) {
					pResult->vOffset = *pStruct->vOffset;
					pResult->vRotate = *pStruct->vRotate;
					pResult->bClingTerrain = (pStruct->bClingTerrain != FALSE );
				}

				if( TRUE == pStruct->bBindParentAction )
				{
					CDnActor* pActor = dynamic_cast<CDnActor*>( this );
					if( pActor )
					{
#if !defined( PRE_ADD_ACTION_OBJECT_END_ACTION )
						pActor->AddBindActionObject( this );
#endif
						pActor->AddBindeActionObjectHandle( hHandle );
						hHandle->CDnActionBase::SetFPS( GetFPS() );
					}
				}
			}
			break;

		case STE_Sound:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow() ) break;
				SoundStruct *pStruct = (SoundStruct *)pPtr;
#ifdef PRE_MOD_LOAD_SOUND_SIGNAL
				if( pStruct->nVecSoundIndex == NULL ) pStruct->nVecSoundIndex = new std::vector<int>;
#else
				if( pStruct->nVecSoundIndex == NULL ) break;
#endif
				if( pStruct->nVecSoundIndex->empty() && pStruct->bLoadingComplete ) break;
				if( !pStruct->bLoadingComplete ) LoadSoundStruct( pStruct );

				switch( pStruct->nType ) {
					case 0:
						{
							if( TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) {
								if( !pStruct->bLoop ) {
									EtSoundChannelHandle hChannel = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetSignalHandle( m_nActionIndex, nSignalIndex );
									if( hChannel ) {
										CEtSoundEngine::GetInstance().RemoveChannel( hChannel );
										TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::RemoveSignalHandle( m_nActionIndex, nSignalIndex, false );
									}
								}
								else return;
							}
						}
						break;
					case 1:
						if( m_hCurVoiceChannel && m_hCurVoiceChannel->IsPlay() ) {
							if( pStruct->bHighPrior ) {
								m_hCurVoiceChannel->Pause();
								memset( &m_CurVoiceChannelData, 0, sizeof(m_CurVoiceChannelData) );
								SAFE_RELEASE_SPTR( m_hCurVoiceChannel );
							}
							else return;
						}
						break;
				}

				if( false == m_bPlaySignalSound )
					break;

				EtSoundChannelHandle hChannel;
				MatrixEx Cross = *GetObjectCross();
				if( _rand()%100 < pStruct->nRandom ) {
					// 너무 멀면 일단 걍 플레이 안한다.
					EtVector3 vListen = CEtSoundEngine::GetInstance().GetListenerPos();

					float fLength = EtVec3Length( &(EtVector3)( GetObjectCross()->m_vPosition - vListen ) );
					if( fLength > pStruct->fRange * 1.5f ) break;
					if( pStruct->nVecSoundIndex->empty() ) break;

#ifdef PRE_FIX_3DSOUND_DISTANCE
					bool b3DSound = true;
					if (fLength < 4.f)
						b3DSound = false;

					hChannel = CEtSoundEngine::GetInstance().PlaySound( ( pStruct->nType == 0 ) ? "3D" : "VOICE", (*pStruct->nVecSoundIndex)[_rand()%pStruct->nVecSoundIndex->size()], ( pStruct->bLoop ) ? true : false, true, b3DSound );
#else
					hChannel = CEtSoundEngine::GetInstance().PlaySound( ( pStruct->nType == 0 ) ? "3D" : "VOICE", (*pStruct->nVecSoundIndex)[_rand()%pStruct->nVecSoundIndex->size()], ( pStruct->bLoop ) ? true : false, true );
#endif
					if ( hChannel )
					{
						hChannel->SetVolume( pStruct->fVolume );
						hChannel->SetRollOff( 3, 0.f, 1.f, pStruct->fRange * pStruct->fRollOff, 1.f, pStruct->fRange, 0.f );
						hChannel->SetPosition( Cross.m_vPosition );
						hChannel->Resume();
						switch( pStruct->nType ) {
							case 0:
								{
									SoundSignalStruct *pSignalStruct = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hChannel );
									pSignalStruct->bChangeActionFadeVolume = ( pStruct->bLoop ) ? true : false;
									if( !pStruct->bLoop && pStruct->fFadeDelta > 0.f ) 
										pSignalStruct->bChangeActionFadeVolume = true;
									pSignalStruct->fFadeDelta = pSignalStruct->fDelta = pStruct->fFadeDelta;
									pSignalStruct->bBeginFade = false;
									pSignalStruct->fVolume = pStruct->fVolume;
									pSignalStruct->bLoop = ( pStruct->bLoop ) ? true : false;
								}
								break;
							case 1:
								hChannel->SetPriority( 0 ); // 중요도를 올립니다. 음성은 끊키면 안되므로..
								m_hCurVoiceChannel = hChannel;
								m_CurVoiceChannelData.bChangeActionFadeVolume = ( pStruct->bLoop ) ? true : false;
								if( !pStruct->bLoop && pStruct->fFadeDelta > 0.f )
									m_CurVoiceChannelData.bChangeActionFadeVolume = true;
								m_CurVoiceChannelData.fFadeDelta = m_CurVoiceChannelData.fDelta = pStruct->fFadeDelta;
								m_CurVoiceChannelData.bBeginFade = false;
								m_CurVoiceChannelData.fVolume = pStruct->fVolume;
								m_CurVoiceChannelData.bLoop = ( pStruct->bLoop ) ? true : false;
#ifdef PRE_MOD_RELEASE_SOUND_SIGNAL
								m_CurVoiceChannelData.pSignal = GetSignal( m_nActionIndex, nSignalIndex );
#endif
								break;
						}
					}
				}
			}
			break;
		case STE_ShaderCustomParameter:
			{
				ShaderCustomParameterStruct *pStruct = (ShaderCustomParameterStruct *)pPtr;
				CDnRenderBase *pRenderBase = dynamic_cast<CDnRenderBase *>(this);
				if( !pRenderBase ) break;
				EtAniObjectHandle hHandle = pRenderBase->GetObjectHandle();
				if( !hHandle ) break;

				TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::SmartPtrSignalImpStruct *pSignalStruct =
					(TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::GetStruct( m_nActionIndex, nSignalIndex );
				if( !pSignalStruct ) {
					int nIndex = hHandle->AddCustomParam( pStruct->szParamStr );
					if( nIndex == -1 ) break;
					if( pStruct->nSubmeshIndex == -1 ) {
						ShaderCustomParamValueSignalStruct *pSignalStruct2;
						pSignalStruct2 = TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hHandle );
						pSignalStruct2->bApply = true;
						pSignalStruct2->EndTime = SignalEndTime;
						pSignalStruct2->nSubMeshIndex = -1;
						pSignalStruct2->nCustomParamIndex = nIndex;
						pSignalStruct2->bContinueToNextAction = (pStruct->bContinueToNextAction == TRUE) ? true : false;
					}
					else {
						for( int i=0; i<hHandle->GetSubMeshCount(); i++ ) {
							if( pStruct->nSubmeshIndex == i ) {
								ShaderCustomParamValueSignalStruct *pSignalStruct2;
								pSignalStruct2 = TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hHandle );
								pSignalStruct2->bApply = true;
								pSignalStruct2->EndTime = SignalEndTime;
								pSignalStruct2->nSubMeshIndex = pStruct->nSubmeshIndex;
								pSignalStruct2->nCustomParamIndex = nIndex;
								pSignalStruct2->bContinueToNextAction = (pStruct->bContinueToNextAction == TRUE) ? true : false;
								break;
							}
						}
					}
				}
				else {
					pSignalStruct->bApply = true;
					pSignalStruct->EndTime = SignalEndTime;
					pSignalStruct->bContinueToNextAction = (pStruct->bContinueToNextAction == TRUE) ? true : false;
				}
				if( pSignalStruct && pSignalStruct->nCustomParamIndex != -1 ) {
					switch( pStruct->nParamType ) {
						case EPT_INT:
							hHandle->SetCustomParam( pSignalStruct->nCustomParamIndex, (void*)&pStruct->nValue, pSignalStruct->nSubMeshIndex );
							break;
						case EPT_FLOAT:
							hHandle->SetCustomParam( pSignalStruct->nCustomParamIndex, (void*)&pStruct->fValue, pSignalStruct->nSubMeshIndex );
							break;
						case EPT_VECTOR:
							hHandle->SetCustomParam( pSignalStruct->nCustomParamIndex, (void*)pStruct->vValue, pSignalStruct->nSubMeshIndex );
							break;
						case EPT_TEX:
							break;
					}
				}
			}
			break;
		case STE_OutlineFilter:
			{

				if( m_bRTTRenderMode)
					break;

				OutlineFilterStruct *pStruct = (OutlineFilterStruct *)pPtr;
//				if( pStruct->nOutlineIndex != -1 ) break;
				if( pStruct->fWidth == 0.f ) break;
				if( pStruct->vColor && pStruct->vColor->w == 0.f ) break;
				if( TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) break;

				EtOutlineHandle hOutline =  CEtOutlineObject::Create( GetAniObjectHandle() );
				if( !hOutline ) break;

				if( pStruct->vColor ) {
					hOutline->SetColor( (EtColor)*pStruct->vColor );
				}
				hOutline->SetWidth( pStruct->fWidth );

				if(pStruct->nType == 1)
					hOutline->SkipProcessIntensity();

//				hOutline->SetType( (CEtOutlineObject::OutlineTypeEnum)pStruct->nType );
//				pStruct->nOutlineIndex = hOutline->GetMyIndex();
				hOutline->Show( true );

				TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hOutline );
			}
			break;
		case STE_ChangeAction:
			{
				ChangeActionStruct *pStruct = (ChangeActionStruct *)pPtr;
				if( _rand()%100 < pStruct->nRandom ) {
					SetActionQueue( pStruct->szAction );
				}
			}
			break;
		case STE_EyeLightTrail:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;
				EyeLightTrailStruct *pStruct = (EyeLightTrailStruct *)pPtr;
				int nHashCode = pStruct->nHashCode;
				DnEyeLightTrailHandle hHandle = TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::GetSignalHandle( nHashCode, -1 );
				if( !hHandle ) {
					CDnRenderBase *pRenderBase = dynamic_cast<CDnRenderBase *>(this);
					if( !pRenderBase ) break;
					DnEyeLightTrailHandle hHandle = (new CDnEyeLightTrailObject( pRenderBase ))->GetMySmartPtr();
					hHandle->Initialize( pStruct->szSkinName, pStruct->szLinkBoneName, pStruct->nLightTrailCount, pStruct->fScaleMin, pStruct->fScaleMax );
					TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::InsertSignalHandle( nHashCode, -1, hHandle );
//					pSignalStruct->nSignalIndex = nSignalIndex;
				}
				else {
					hHandle->SetScale( pStruct->fScaleMin * m_vActionSignalScale.x, pStruct->fScaleMax * m_vActionSignalScale.x );
				}
			}
			break;
		case STE_AttachEffectTrail:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;
				AttachEffectTrailStruct *pStruct = (AttachEffectTrailStruct *)pPtr;

				if( TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::IsExistSignalHandle( pStruct->nHashCode , -1 ) ) 
					break;

				EtTextureHandle hTexture = CEtResource::GetResource( pStruct->nTextureIndex );
				if( hTexture )
				{
					EtEffectTrailHandle hEffectTrail = EternityEngine::CreateEffectTrail( hTexture, pStruct->fLifeTime , pStruct->fTrailWidth , pStruct->fMinSegment );
					if( hEffectTrail )
					{
						EffectTrailSignalStruct *pResult = TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::InsertSignalHandle( pStruct->nHashCode , -1 , hEffectTrail );
						strcpy_s(pResult->szBoneName, pStruct->szLinkBoneName);
						pResult->vOffset = pStruct->vOffset;
						pResult->bLoop = pStruct->bLoop == TRUE ? true : false;

						hEffectTrail->SetBlendOP( (EtBlendOP)( pStruct->nBlendOP + 1 ) );
						hEffectTrail->SetSrcBlend( (EtBlendMode)( pStruct->nSrcBlend + 1 ) );
						hEffectTrail->SetDestBlend( (EtBlendMode)( pStruct->nDestBlend + 1 ) );
					}
				}
			}
			break;
		case STE_PointLight:
			{
				if( GetRenderBase() && !GetRenderBase()->IsShow()) break;

				PointLightStruct *pStruct = (PointLightStruct *)pPtr;
				float fValue = 1.f / ( SignalEndTime - SignalStartTime - 16.6666f ) * ( LocalTime - SignalStartTime );
				fValue = EtClamp( fValue, 0.0f, 1.0f );
				MatrixEx Cross = *GetObjectCross();
				Cross.MoveLocalXAxis( pStruct->vOffset->x * vScale.y );
				Cross.MoveLocalYAxis( pStruct->vOffset->y * vScale.y );
				Cross.MoveLocalZAxis( pStruct->vOffset->z * vScale.y );

				SLightInfo LightInfo;
				LightInfo.Type = LT_POINT;
				LightInfo.fRange = pStruct->fRange;
				LightInfo.Position = Cross.m_vPosition;
				if( fValue < pStruct->fBeginRatio ) {
					EtVector4 vTemp;
					float fWeight = ( 1.f / pStruct->fBeginRatio ) * fValue;
					EtVec4Lerp( &vTemp, &EtVector4( 0.f, 0.f, 0.f, 0.f ), pStruct->vDiffuse, fWeight );
					vTemp *= vTemp.w;
					LightInfo.Diffuse = D3DXCOLOR( vTemp.x, vTemp.y, vTemp.z, 1.f );
					EtVec4Lerp( &vTemp, &EtVector4( 0.f, 0.f, 0.f, 0.f ), pStruct->vSpecular, fWeight );
					vTemp *= vTemp.w;
					LightInfo.Specular = D3DXCOLOR( vTemp.x, vTemp.y, vTemp.z, 1.f );
				}
				else if( fValue > pStruct->fEndRatio ) {
					EtVector4 vTemp;
					float fWeight = ( 1.f / ( 1.f - pStruct->fEndRatio ) ) * ( fValue - pStruct->fEndRatio );
					EtVec4Lerp( &vTemp, pStruct->vDiffuse, &EtVector4( 0.f, 0.f, 0.f, 0.f ), fWeight );
					vTemp *= vTemp.w;
					LightInfo.Diffuse = D3DXCOLOR( vTemp.x, vTemp.y, vTemp.z, 1.f );
					EtVec4Lerp( &vTemp, pStruct->vSpecular, &EtVector4( 0.f, 0.f, 0.f, 0.f ), fWeight );
					vTemp *= vTemp.w;
					LightInfo.Specular = D3DXCOLOR( vTemp.x, vTemp.y, vTemp.z, 1.f );
				}
				else {
					EtVector4 vTemp = *pStruct->vDiffuse;
					vTemp *= vTemp.w;
					LightInfo.Diffuse = D3DXCOLOR( vTemp.x, vTemp.y, vTemp.z, 1.f );
					LightInfo.Specular = D3DXCOLOR( vTemp.x, vTemp.y, vTemp.z, 1.f );
				}

				if( TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::IsExistSignalHandle( m_nActionIndex, nSignalIndex ) ) {
					EtLightHandle hLight = TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::GetSignalHandle( m_nActionIndex, nSignalIndex );
					hLight->SetLightInfo( &LightInfo );
				}
				else {
					EtLightHandle hLight = CreateLight( &LightInfo );
					TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hLight );
				}
			}
			break;
		case STE_OtherSelfEffect:
			{
				OtherSelfEffectStruct *pStruct = (OtherSelfEffectStruct *)pPtr;
				if( !GetRenderBase() || !GetRenderBase()->GetObjectHandle() ) break;
				EtAniObjectHandle hParentObject = GetRenderBase()->GetObjectHandle();

				std::string szSkinName = hParentObject->GetSkinFileName();
				if( strlen( pStruct->szSkinName ) > 0 ) szSkinName = pStruct->szSkinName;

				EtAniObjectHandle hObject = CreateAniObject( szSkinName.c_str(), hParentObject->GetAniHandle()->GetFileName() );
				hObject->SetAniFrame( m_nVecAniIndexList[ m_nActionIndex ], CDnActionBase::m_fFrame );
				hObject->ShowObject( true );

				if( true == pStruct->bAllCalcPosition )
					hObject->SetCalcPositionFlag( CALC_POSITION_X|CALC_POSITION_Y | CALC_POSITION_Z );

				hObject->Update( *GetObjectCross() );
				hObject->EnableShadowCast( false );
				hObject->EnableShadowReceive( false );

				EtTextureHandle hTexture = CDnInCodeResource::GetInstance().GetNullWhiteTexture();
				int nTexture = hTexture.GetIndex();

				int nCustomParam = hObject->AddCustomParam( "g_DiffuseTex" );
				hObject->SetCustomParam( nCustomParam, (int*)&nTexture, -1 );

				EtVector4 vDiffuse = EtVector4( 0.f, 0.f, 0.f, 0.f );
				nCustomParam = hObject->AddCustomParam( "g_MaterialDiffuse" );
				hObject->SetCustomParam( nCustomParam, (EtVector4*)&vDiffuse, -1 );

				nCustomParam = hObject->AddCustomParam( "g_MaterialAmbient" );
				hObject->SetCustomParam( nCustomParam, (EtVector4*)pStruct->vBeginColor, -1 );

				hObject->SetObjectAlpha( pStruct->fBeginAlpha );
				hObject->GetSkinInstance()->EnableAlphaBlend( true );
				hObject->GetSkinInstance()->SetBlendOP( (EtBlendOP)( pStruct->nBlendOP + 1 ) );
				hObject->GetSkinInstance()->SetSrcBlend( (EtBlendMode)( pStruct->nSrcBlend + 1 ) );
				hObject->GetSkinInstance()->SetDestBlend( (EtBlendMode)( pStruct->nDestBlend + 1 ) );

				OtherSelfEffectSignalStruct *pResult = TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::InsertSignalHandle( m_nActionIndex, nSignalIndex, hObject );
				pResult->fLifeTime = pResult->fCurTime = pStruct->fLifeTime;
				pResult->vBeginColor = *pStruct->vBeginColor;
				pResult->vEndColor = *pStruct->vEndColor;
				pResult->fBeginAlpha = pStruct->fBeginAlpha;
				pResult->fEndAlpha = pStruct->fEndAlpha;
			}
			break;
#ifndef _FINAL_BUILD
		case STE_Hit:
			if( CGlobalValue::GetInstance().m_bDrawHitSignal ) {
				int nIndex = ( m_nActionIndex << 16 ) | nSignalIndex;
				DrawHitSignal( (HitStruct *)pPtr, nIndex, CGlobalValue::GetInstance().m_fDrawHitSignalMinDelta + ( SignalEndTime - SignalStartTime ) / 1000.f );
			}
			break;
#endif
	}
}


void CDnActionSignalImp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	MatrixEx Cross = *GetObjectCross();

	/*
	for( DWORD i=0; i<m_hVecCurChannel.size(); i++ ) {
		if( m_hVecCurChannel[i] ) m_hVecCurChannel[i]->SetPosition( Cross.m_vPosition );
	}
	*/
	if( m_hCurVoiceChannel ) {
		m_hCurVoiceChannel->SetPosition( Cross.m_vPosition );
		if( m_CurVoiceChannelData.bBeginFade ) {
			m_CurVoiceChannelData.fDelta -= fDelta;
			m_hCurVoiceChannel->SetVolume( m_CurVoiceChannelData.fVolume * ( ( 1.f / m_CurVoiceChannelData.fFadeDelta ) * m_CurVoiceChannelData.fDelta ) );
			if( m_CurVoiceChannelData.fDelta <= 0.f ) {
				m_hCurVoiceChannel->Pause();
				SAFE_RELEASE_SPTR( m_hCurVoiceChannel );
			}
		}
	}
#ifdef PRE_MOD_RELEASE_SOUND_SIGNAL
	else {
		if( m_CurVoiceChannelData.pSignal ) {
			CheckPostSignal( NULL, -1, m_CurVoiceChannelData.pSignal, -1, NULL );
			m_CurVoiceChannelData.pSignal = NULL;
		}
	}
#endif
	// Process Effect Sound Channel
	for( DWORD i=0; i<TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetCount(); i++ ) {
		EtSoundChannelHandle hChannel = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetObject(i);
		if( !hChannel ) {
			TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::RemoveObject(i, false );
			i--;
		}
		else {
			hChannel->SetPosition( Cross.m_vPosition );
			SoundSignalStruct *pStruct = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetStruct(i);
			if( pStruct->bBeginFade ) {
				pStruct->fDelta -= fDelta;
				hChannel->SetVolume( pStruct->fVolume * ( ( 1.f / pStruct->fFadeDelta ) * pStruct->fDelta ) );
				if( pStruct->fDelta <= 0.f ) {
					hChannel->Pause();
					TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::RemoveObject(i);
					i--;
				}
			}
		}

	}


	// Process Particle
	MatrixEx MatExTemp;
	for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetCount(); i++ ) {
		EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetObject(i);
		if( !hParticle ) {
			TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::RemoveObject(i, false);
			i--;
		}
		else {
			LinkParticleSignalStruct *pStruct = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetStruct(i);
			
			MatExTemp = Cross;
			MatExTemp.MoveLocalZAxis( pStruct->vOffset.z );
			MatExTemp.MoveLocalXAxis( pStruct->vOffset.x );

			EtAniObjectHandle hParentObject;
			if( GetRenderBase() ) hParentObject = GetRenderBase()->GetObjectHandle();

			hParticle->SetLink( Cross, pStruct->bClingTerrain, pStruct->bClingTerrain ? INSTANCE(CDnWorld).GetHeightWithProp( MatExTemp.m_vPosition )  : 0.f, 
				( pStruct->szBoneName && strlen(pStruct->szBoneName) > 0 ) ? pStruct->szBoneName : NULL, pStruct->vOffset, pStruct->vRotate , hParentObject );	
		}
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetCount(); i++ ) {
		EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetObject(i);
		if( !hParticle ) {
			TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::RemoveObject(i, false);
			i--;
		}
	}

	// Process FX
	for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetCount(); i++ ) {
		EtEffectObjectHandle hObject = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetObject(i);
		if( !hObject ) {
			TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::RemoveObject(i, false);
			i--;
		}
		else {
			LinkFXSignalStruct *pStruct = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetStruct(i);

			MatrixEx MatExTemp = Cross;
			if( pStruct->bClingTerrain ) {
				MatExTemp.m_vPosition.y = INSTANCE( CDnWorld ).GetHeightWithProp( MatExTemp.m_vPosition );
			}
			EtVector3 vOffset = pStruct->vOffset;
			vOffset.y *= m_vActionSignalScale.y;
			
			EtAniObjectHandle hAniObject = GetAniObjectHandle();
			if( pStruct->bUseTraceHitProjectileActorAdditionalHeight )
			{
				float fHeight = GetTraceHitProjectileActorAdditionalHeight();
				if( 0.0f < fHeight )		// 발사체가 추적중인 액터가 없으면 0 임.
					vOffset.y = fHeight;
			}

			hObject->SetLink( MatExTemp, pStruct->szBoneName, vOffset, pStruct->vRotate , hAniObject );
		}
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
		EtEffectObjectHandle hObject = TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetObject(i);
		if( !hObject ) {
			TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveObject(i, false);
			i--;
		}
	}

	// Process EtcObject
	for( DWORD i=0; i<TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetCount(); i++ ) {
		TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct *pStruct = 
			(TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::SmartPtrSignalImpStruct*)TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetStruct(i);

		DnEtcHandle hObject = pStruct->hHandle;
		if( hObject ) {
			if( pStruct->bLinkObject ) {

				MatExTemp = Cross;
				MatExTemp.MoveLocalZAxis( pStruct->vOffset.z );
				MatExTemp.MoveLocalXAxis( pStruct->vOffset.x );
				if( pStruct->bClingTerrain ) {
					MatExTemp.m_vPosition.y = INSTANCE(CDnWorld).GetHeightWithProp( MatExTemp.m_vPosition );
				}
				MatExTemp.MoveLocalYAxis( pStruct->vOffset.y * m_vActionSignalScale.y );

				if( pStruct->szBoneName && strlen(pStruct->szBoneName) > 0  ) 
				{
					EtMatrix mat = hObject->GetAniBoneMatrix( pStruct->szBoneName );

					MatExTemp.m_vXAxis = EtVector3(1,0,0);
					MatExTemp.m_vYAxis = EtVector3(0,1,0);
					MatExTemp.m_vZAxis = EtVector3(0,0,1);
					MatExTemp.m_vPosition += *(EtVector3*)&mat._41;
					MatExTemp.m_vPosition -= Cross.m_vPosition;
				}

				MatExTemp.RotateYaw( pStruct->vRotate.y );
				MatExTemp.RotateRoll( pStruct->vRotate.z );
				MatExTemp.RotatePitch( pStruct->vRotate.x );

				*(hObject->GetMatEx()) = MatExTemp;
			}
		}
		if( !hObject || hObject->IsDestroy() ) {
			TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveObject(i, true);
		}
	}
	// Process Shader Custom Param
	for( DWORD i=0; i<TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::GetCount(); i++ ) {
		TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::SmartPtrSignalImpStruct *pStruct = 
			(TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtAniObjectHandle, ShaderCustomParamValueSignalStruct>::GetStruct(i);
		if( !pStruct->bApply ) continue;
		if( LocalTime > pStruct->EndTime )
		{
			pStruct->bApply = false;
			pStruct->EndTime = 0;

			// #51234 다음 액션에서도 똑같은 shader custom parameter 를 건드릴 것으로 지정이 되어있으므로 여기선 원복시키지 않는다.
			// 어차피 다음 액션에 있는 시그널이 bContinueToNextAction 이 false 가 되면 이쪽으로 와서 원복된다.
			if( false == pStruct->bContinueToNextAction )
				pStruct->hHandle->RestoreCustomParam( pStruct->nCustomParamIndex, pStruct->nSubMeshIndex );
			else
				pStruct->bContinueToNextAction = false;
		}
	}

	// Process Decal
	for( DWORD i=0; i<TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>::GetCount(); i++ ) {
		EtDecalHandle hDecal = TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>::GetObject(i);
		if( !hDecal ) {
			TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>::RemoveObject(i, false);
			i--;
		}
		else {
			LinkDecalSignalStruct *pStruct = TSmartPtrSignalImp<EtDecalHandle, LinkDecalSignalStruct>::GetStruct(i);
			MatrixEx Cross = *GetObjectCross();
			Cross.MoveLocalZAxis( pStruct->vOffset.z );
			Cross.MoveLocalXAxis( pStruct->vOffset.x );
			float fAngle = EtToDegree(atan2f(Cross.m_vZAxis.x, Cross.m_vZAxis.z));
			struct CalcDecalHeight : public CalcHeightFn
			{
				virtual float GetHeight( float fX, float fY, float fZ, int nTileScale )  override {
					return INSTANCE(CDnWorld).GetHeightWithProp( EtVector3(fX, fY, fZ), NULL, NULL, nTileScale );
				}
			};

			hDecal->Update( Cross.m_vPosition.x, Cross.m_vPosition.z, pStruct->fRadius, fAngle + pStruct->fRotate, 
				EtColor(1,1,1,1), &CalcDecalHeight() , Cross.m_vPosition.y );
		}
	}
	// Process Outline Filter
	for( DWORD i=0; i<TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
		TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *pStruct = 
			(TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *)TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::GetStruct(i);
		EtOutlineHandle hOutline = pStruct->hHandle;
		if( !hOutline ) {
			TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::RemoveObject(i, false);
			i--;
		}
		else {
			if( IsSignalRange( STE_OutlineFilter, pStruct->nSignalIndex ) ) 
				hOutline->Show( true );
			else {
				TSmartPtrSignalImp<EtOutlineHandle, TSmartPtrSignalStruct::Default>::RemoveObject(i);
				i--;
			}
		}
	}
	// Process EyeLightTrail
	if( TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::GetCount() ) {
		for( DWORD i=0; i<TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
			DnEyeLightTrailHandle hHandle = TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::GetObject(i);
			TSmartPtrSignalStruct::Default *pStruct = TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::GetStruct(i);
			if( !hHandle ) continue;

			bool bShow = IsEyeLightTrailRange( ((TSmartPtrSignalImp<DnEyeLightTrailHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct*)pStruct)->nActionIndex );
			hHandle->Show( bShow );
			hHandle->Process( LocalTime, fDelta );
		}
	}
	// Process EffectTrail
	if( TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::GetCount() ) {
		for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::GetCount(); i++ ) {
			EtEffectTrailHandle hHandle = TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::GetObject(i);
			EffectTrailSignalStruct *pStruct = TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::GetStruct(i);

			if( hHandle ) 
			{
				EtVector3 vPos = GetObjectCross()->GetPosition();
				EtMatrix ResultMat;
				EtMatrixIdentity( &ResultMat );

				ResultMat = GetAniBoneMatrix( pStruct->szBoneName );
				
				if( ResultMat._41 == 0 &&  pStruct->szBoneName[ 0 ] == '#' )
					ResultMat = GetDummyBoneMatrix( pStruct->szBoneName );

				vPos = *(EtVector3*)&ResultMat._41;
				vPos += GetObjectCross()->m_vXAxis * pStruct->vOffset->x;
				vPos += GetObjectCross()->m_vYAxis * pStruct->vOffset->y;
				vPos += GetObjectCross()->m_vZAxis * pStruct->vOffset->z;

				bool bShow = IsEffectTrailRange( ((TSmartPtrSignalImp<EtEffectTrailHandle, EffectTrailSignalStruct>::SmartPtrSignalImpStruct*)pStruct)->nActionIndex );
				hHandle->Show( pStruct->bLoop ? true : bShow );
				hHandle->AddPoint( vPos );
			}
		}
	}
	// Process PointLight
	if( TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::GetCount() ) {
		for( DWORD i=0; i<TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
			TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct *pStruct = 
				(TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::SmartPtrSignalImpStruct*)TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::GetStruct(i);
			if( !IsSignalRange( STE_PointLight, pStruct->nSignalIndex ) ) {
				TSmartPtrSignalImp<EtLightHandle, TSmartPtrSignalStruct::Default>::RemoveObject(i);
				i--;
			}
		}
	}

	// Process OtherSelf Effect
	if( TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::GetCount() ) {
		for( DWORD i=0; i<TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::GetCount(); i++ ) {
			OtherSelfEffectSignalStruct *pStruct = (OtherSelfEffectSignalStruct *)TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::GetStruct(i);
			EtAniObjectHandle hObject = TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::GetObject(i);

			pStruct->fCurTime -= fDelta;
			if( pStruct->fCurTime <= 0.f ) {
				TSmartPtrSignalImp<EtAniObjectHandle, OtherSelfEffectSignalStruct>::RemoveObject(i);
				i--;
			}
			else {
				float fWeight = 1.f - ( ( 1.f / pStruct->fLifeTime ) * pStruct->fCurTime );

				EtVector4 vDiffuse;
				EtVec4Lerp( &vDiffuse, &pStruct->vBeginColor, &pStruct->vEndColor, fWeight );
//				EtVector4 vAmbient = vDiffuse / *CEtLight::GetGlobalAmbient();

				int nCustomParam = hObject->AddCustomParam( "g_MaterialAmbient" );
				hObject->SetCustomParam( nCustomParam, (EtVector4*)&vDiffuse, -1 );

				float fAlpha = pStruct->fBeginAlpha + ( ( pStruct->fEndAlpha - pStruct->fBeginAlpha ) * fWeight );
				hObject->SetObjectAlpha( fAlpha );
			}
		}
	}
#ifndef _FINAL_BUILD
	ProcessDrawHitSignsl( LocalTime, fDelta );
#endif
}

void CDnActionSignalImp::OnChangeAction( const char *szPrevAction )
{
	if( strcmp( szPrevAction, m_szAction.c_str() ) == NULL ) return;

	if( m_hCurVoiceChannel && m_CurVoiceChannelData.bChangeActionFadeVolume ) {
		m_CurVoiceChannelData.bBeginFade = true;
	}

	// Process Effect Sound Channel
	for( DWORD i=0; i<TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetCount(); i++ ) {
		EtSoundChannelHandle hChannel = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetObject(i);
		if( hChannel ) {
			SoundSignalStruct *pStruct = TSmartPtrSignalImp<EtSoundChannelHandle, SoundSignalStruct>::GetStruct(i);
			if( pStruct->bChangeActionFadeVolume )
				pStruct->bBeginFade = true;
		}

	}

	for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle,NormalParticleSignalStruct>::GetCount(); i++ ) {
		EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle,NormalParticleSignalStruct>::GetObject(i);
		if( hParticle ) {
			NormalParticleSignalStruct *pStruct = TSmartPtrSignalImp<EtBillboardEffectHandle,NormalParticleSignalStruct>::GetStruct(i);
			if( pStruct->bDeleteByChangeAction ) {
				TSmartPtrSignalImp<EtBillboardEffectHandle,NormalParticleSignalStruct>::RemoveObject( i, true );
				i--;
			}
		}
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle,LinkParticleSignalStruct>::GetCount(); i++ ) {
		EtBillboardEffectHandle hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle,LinkParticleSignalStruct>::GetObject(i);
		if( hParticle ) {
			LinkParticleSignalStruct *pStruct = TSmartPtrSignalImp<EtBillboardEffectHandle,LinkParticleSignalStruct>::GetStruct(i);
			if( pStruct->bDeleteByChangeAction ) {
				TSmartPtrSignalImp<EtBillboardEffectHandle,LinkParticleSignalStruct>::RemoveObject( i, true );
				i--;
			}
		}
	}
}

bool CDnActionSignalImp::IsEyeLightTrailRange( int nHashCode )
{
	if( m_nActionIndex == -1 ) return false;
	ActionElementStruct *pStruct = GetElement( m_nActionIndex );

	std::map<int, std::vector<CEtActionSignal *>>::iterator it = pStruct->MapSearch.find( STE_EyeLightTrail );
	if( it == pStruct->MapSearch.end() ) return false;

	CEtActionSignal *pSignal;
	for( DWORD i=0; i<it->second.size(); i++ ) {
		pSignal = it->second[i];
		if( m_fFrame >= pSignal->GetStartFrame() && m_fFrame < pSignal->GetEndFrame() ) {
			if( ((EyeLightTrailStruct*)pSignal->GetData())->nHashCode == nHashCode ) return true;
		}
	}
	return false;
}

bool CDnActionSignalImp::IsEffectTrailRange( int nHashCode )
{
	if( m_nActionIndex == -1 ) return false;
	ActionElementStruct *pStruct = GetElement( m_nActionIndex );

	std::map<int, std::vector<CEtActionSignal *>>::iterator it = pStruct->MapSearch.find( STE_AttachEffectTrail );
	if( it == pStruct->MapSearch.end() ) return false;

	CEtActionSignal *pSignal;
	for( DWORD i=0; i<it->second.size(); i++ ) {
		pSignal = it->second[i];
		if( m_fFrame >= pSignal->GetStartFrame() && m_fFrame < pSignal->GetEndFrame() ) {
			if( ((AttachEffectTrailStruct*)pSignal->GetData())->nHashCode == nHashCode ) return true;
		}
	}
	return false;
}

EtLightHandle CDnActionSignalImp::CreateLight( SLightInfo *pInfo )
{
	return EternityEngine::CreateLight( pInfo );
}

#ifndef _FINAL_BUILD
void CDnActionSignalImp::DrawHitSignal( HitStruct *pHit, int nIndex, float fDelta )
{
	std::map<int, bool>::iterator it = m_nMapHitSignal.find( nIndex );
	if( it != m_nMapHitSignal.end() ) return;

	DrawHitSignalStruct *pStruct = new DrawHitSignalStruct;

	EtVector3 vCenter = EtVector3( 0.f, 0.f, 1.f );
	EtMatrix matRotate;

	EtMatrixRotationY( &matRotate, pHit->fCenterAngle / 180.f * D3DX_PI );
	EtVec3TransformNormal( &vCenter, &vCenter, &matRotate );

	float fScale = 1.f;
	CDnActor *pActor = dynamic_cast<CDnActor*>(this);
	if( pActor )
		fScale = pActor->GetScale()->y;

	int nStart = int(pHit->fCenterAngle - ( pHit->fAngle ));
	int nEnd = int(pHit->fCenterAngle + ( pHit->fAngle ));

	EtVector3 vVtx = EtVector3( 0.f, 0.f, 0.f );
	EtVector3 vPos = EtVector3( 0.f, 0.f, 0.f );
	EtVector3 vTemp = EtVector3( 0.f, 0.f, 0.f );

	for( int i=nStart; i<=nEnd; i++ ) {
		EtMatrixRotationY( &matRotate, ( pHit->fCenterAngle - i ) / 180.f * D3DX_PI );
		D3DXVec3TransformNormal( &vTemp, &vCenter, &matRotate );

		vVtx = vPos + vTemp * pHit->fDistanceMin * fScale;
		vVtx.y = vPos.y + pHit->fHeightMax * fScale;
		pStruct->vVecList[0].push_back( vVtx );
		if( i != nStart && i != nEnd ) pStruct->vVecList[0].push_back( vVtx );

		vVtx.y = vPos.y + pHit->fHeightMin * fScale;
		pStruct->vVecList[1].push_back( vVtx );
		if( i != nStart && i != nEnd ) pStruct->vVecList[1].push_back( vVtx );

		vVtx = vPos + vTemp * pHit->fDistanceMax * fScale;
		vVtx.y = vPos.y + pHit->fHeightMax * fScale;
		pStruct->vVecList[2].push_back( vVtx );
		if( i != nStart && i != nEnd ) pStruct->vVecList[2].push_back( vVtx );

		vVtx.y = vPos.y + pHit->fHeightMin * fScale;
		pStruct->vVecList[3].push_back( vVtx );
		if( i != nStart && i != nEnd ) pStruct->vVecList[3].push_back( vVtx );

		if( i == nStart ) 
		{
			pStruct->vVecList[4].push_back( pStruct->vVecList[0][ pStruct->vVecList[0].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[1][ pStruct->vVecList[1].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[2][ pStruct->vVecList[2].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[3][ pStruct->vVecList[3].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[1][ pStruct->vVecList[1].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[3][ pStruct->vVecList[3].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[0][ pStruct->vVecList[0].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[2][ pStruct->vVecList[2].size() - 1 ] );
		}
		else if( i == nEnd ) 
		{
			pStruct->vVecList[4].push_back( pStruct->vVecList[0][ pStruct->vVecList[0].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[1][ pStruct->vVecList[1].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[2][ pStruct->vVecList[2].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[3][ pStruct->vVecList[3].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[1][ pStruct->vVecList[1].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[3][ pStruct->vVecList[3].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[0][ pStruct->vVecList[0].size() - 1 ] );
			pStruct->vVecList[4].push_back( pStruct->vVecList[2][ pStruct->vVecList[2].size() - 1 ] );
		}
	}
	pStruct->vOffset = *pHit->vOffset;

	pStruct->fDelta = fDelta;
	pStruct->nIndex = nIndex;

	for( int i=0; i<5; i++ ) 
	{
		for( DWORD j=0; j<pStruct->vVecList[i].size(); j++ ) 
		{
			EtVec3TransformNormal( &pStruct->vVecList[i][j], &pStruct->vVecList[i][j], *GetObjectCross() );
		}
	}

	m_nMapHitSignal.insert( make_pair( nIndex, true ) );

	m_pVecDrawHitSignalList.push_back( pStruct );
}

void CDnActionSignalImp::ProcessDrawHitSignsl( LOCAL_TIME LocalTime, float fDelta )
{
	for( DWORD i=0; i<m_pVecDrawHitSignalList.size(); i++ ) {
		DrawHitSignalStruct *pStruct = m_pVecDrawHitSignalList[i];
		pStruct->fDelta -= fDelta;

		if( pStruct->fDelta <= 0.f ) {
			std::map<int, bool>::iterator it = m_nMapHitSignal.find( pStruct->nIndex );
			if( it != m_nMapHitSignal.end() ) {
				m_nMapHitSignal.erase( it );
			}

			SAFE_DELETE( pStruct );
			m_pVecDrawHitSignalList.erase( m_pVecDrawHitSignalList.begin() + i );
			i--;
			continue;
		}

		EtVector3 vDist = GetObjectCross()->m_vPosition;
		vDist += GetObjectCross()->m_vXAxis * pStruct->vOffset.x;
		vDist += GetObjectCross()->m_vYAxis * pStruct->vOffset.y;
		vDist += GetObjectCross()->m_vZAxis * pStruct->vOffset.z;

		EtVector3 vPos[2];
		for( int i=0; i<5; i++ ) {
			if( pStruct->vVecList[i].size() < 2 ) continue;
			for( DWORD j=0; j<pStruct->vVecList[i].size(); j+=2 ) {
				vPos[0] = pStruct->vVecList[i][j] + vDist;
				vPos[1] = pStruct->vVecList[i][j+1] + vDist;
				EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );
			}
		}
		
	}
}

#endif

void CDnActionSignalImp::ApplyAlphaToSignalImps( float fAlpha )
{
	for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetCount(); i++ ) {
		EtBillboardEffectHandle hHandle = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetObject(i);
		if( hHandle ) hHandle->SetColor( &EtColor( 1.0f, 1.0f, 1.0f, fAlpha ) );
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetCount(); i++ ) {
		EtBillboardEffectHandle hHandle = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetObject(i);
		if( hHandle ) hHandle->SetColor( &EtColor( 1.0f, 1.0f, 1.0f, fAlpha ) );
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetCount(); i++ ) {
		DnEtcHandle hHandle = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetObject(i);
		if( hHandle && hHandle->GetObjectHandle() ) 
			hHandle->GetObjectHandle()->SetObjectAlpha( fAlpha );
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetCount(); i++ ) {
		EtEffectObjectHandle hHandle = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetObject(i);
		if( hHandle ) hHandle->SetAlpha( fAlpha );
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
		EtEffectObjectHandle hHandle = TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetObject(i);
		if( hHandle ) hHandle->SetAlpha( fAlpha );
	}
}