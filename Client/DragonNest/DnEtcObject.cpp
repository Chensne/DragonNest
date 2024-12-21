#include "StdAfx.h"
#include "DnEtcObject.h"
#include "DnActor.h"

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
#include "GameOption.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#endif 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_DN_SMART_PTR_STATIC( CDnEtcObject, 400 )

CDnEtcObject::CDnEtcObject( bool bProcess )
: CDnUnknownRenderObject( bProcess )
, m_bUseAsStateEffect( false )
, m_bBillboard( false )
, m_bYBillboard( false )
{

}

CDnEtcObject::~CDnEtcObject()
{
	ReleaseSignalImp();
}

bool CDnEtcObject::Initialize( const char *szSkinName, const char *szAniName, const char *szActName )
{
	bool bResult = CDnActionRenderBase::Initialize( szSkinName, szAniName, szActName );
	
	SetCalcPositionFlag( CALC_POSITION_X | CALC_POSITION_Y | CALC_POSITION_Z );
	return bResult;
}

void CDnEtcObject::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActionRenderBase::Process( LocalTime, fDelta );
	CDnActionSignalImp::Process( LocalTime, fDelta );

	if( m_bBillboard ) {
		EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
		if( hCamera ) {
			if( m_bYBillboard ) {
				m_matExWorld.m_vXAxis = *(EtVector3*)&hCamera->GetInvViewMat()->_11;				
				m_matExWorld.m_vXAxis.y = 0.f;
				EtVec3Normalize(&m_matExWorld.m_vXAxis, &m_matExWorld.m_vXAxis);
				m_matExWorld.m_vYAxis = EtVector3(0, 1, 0);
				EtVec3Cross(&m_matExWorld.m_vZAxis, &m_matExWorld.m_vXAxis, &m_matExWorld.m_vYAxis);
				EtVec3Normalize(&m_matExWorld.m_vZAxis, &m_matExWorld.m_vZAxis);
			}
			else {
				m_matExWorld.m_vXAxis = *(EtVector3*)&hCamera->GetInvViewMat()->_11;
				m_matExWorld.m_vYAxis = *(EtVector3*)&hCamera->GetInvViewMat()->_21;
				m_matExWorld.m_vZAxis = *(EtVector3*)&hCamera->GetInvViewMat()->_31;
			}
		}
	}

	// 은신 상태효과 처럼 실시간으로 액터의 알파 상태가 바뀔 수 있으므로 주기적으로 업데이트.
	if( m_hParentActor )
	{
		float fAlpha = m_hParentActor->GetAlpha();
		ApplyAlphaToSignalImps( fAlpha );
	}
}

void CDnEtcObject::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pTask && pTask->GetGameTaskType() != GameTaskType::PvP )	
	{ 
		if( ! CGameOption::GetInstance().m_bEnablePlayerEffect && m_hPlayerActor  )
		{	
			if( m_hPlayerActor->IsPlayerActor() && CDnActor::s_hLocalActor->GetUniqueID() != m_hPlayerActor->GetUniqueID() )
				if( ! SetEnableSignal( Type, pPtr ) )		
					return;
		}			
	}
#endif

	CDnActionSignalImp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	switch( Type ) {
		case STE_Destroy:
			SetDestroy();
			break;
		case STE_Billboard:
			{
				BillboardStruct *pStruct = (BillboardStruct *)pPtr;
				m_bBillboard = true;
				m_bYBillboard = (pStruct->bYBillboard != FALSE);
			}
			break;
		case STE_AlphaBlending:
			{
				AlphaBlendingStruct *pStruct = (AlphaBlendingStruct *)pPtr;
				float fValue = 1.f / ( SignalEndTime - SignalStartTime - 16.6666f ) * ( LocalTime - SignalStartTime );				
				fValue = EtClamp( fValue, 0.0f, 1.0f );
				float fCurAlpha = pStruct->fStartAlpha + ( ( pStruct->fEndAlpha - pStruct->fStartAlpha ) * fValue );
				fCurAlpha = EtClamp( fCurAlpha, 0.0f, 1.0f );	
				if( m_hObject ) {
					m_hObject->SetObjectAlpha( fCurAlpha );
					if( fCurAlpha < 0.5f ) {
						if( m_hObject->IsShadowCast() )
							m_hObject->EnableShadowCast( false );
					}
				}

				for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetCount(); i++ ) {
					EtBillboardEffectHandle hHandle = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetObject(i);
					if( hHandle ) hHandle->SetColor( &EtColor( 1.0f, 1.0f, 1.0f, fCurAlpha ) );
				}
				for( DWORD i=0; i<TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetCount(); i++ ) {
					EtBillboardEffectHandle hHandle = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetObject(i);
					if( hHandle ) hHandle->SetColor( &EtColor( 1.0f, 1.0f, 1.0f, fCurAlpha ) );
				}
				for( DWORD i=0; i<TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetCount(); i++ ) {
					DnEtcHandle hHandle = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetObject(i);
					if( hHandle ) hHandle->GetObjectHandle()->SetObjectAlpha( fCurAlpha );
				}
				for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetCount(); i++ ) {
					EtEffectObjectHandle hHandle = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetObject(i);
					if( hHandle ) hHandle->SetAlpha( fCurAlpha );
				}
				for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
					EtEffectObjectHandle hHandle = TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetObject(i);
					if( hHandle ) hHandle->SetAlpha( fCurAlpha );
				}
			}
			break;
	}
}

void CDnEtcObject::ProcessClass( LOCAL_TIME LocalTime, float fDelta )
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		s_pVecProcessList[i]->Process( LocalTime, fDelta );
		if( s_pVecProcessList[i]->IsDestroy() ) 
		{
			CDnEtcObject* pWeapon = s_pVecProcessList[i];
			SAFE_DELETE(pWeapon);
			i--;
		}
	}
}

bool CDnEtcObject::InitializeClass()
{
	return true;
}

void CDnEtcObject::ReleaseClass()
{
	// Note: 한기 상태효과로 붙은 이펙트.. EtcObject 가 스테이지 이동시에 객체가 없어지면
	// 안되므로 전부 삭제를 하는 것이 아니라 골라서 삭제를 해줍니다. 
	// 겹치는 본 자리의 이펙트 처리를 하는 등 따로 리스트로 이 객체의 핸들을 갖고 관리하는 쪽에서
	// 핸들이 무효화가 되어버립니다. (#8199)
	//DeleteAllObject();
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		if( s_pVecProcessList[i]->IsUsingStateEffect() ) continue;
		s_pVecProcessList[i]->Release();
		i--;
	}
}

void CDnEtcObject::Show( bool bShow )
{
	ShowRenderBase( bShow );
	
	if( m_hObject ) 
	{
		m_hObject->EnableCollision( bShow );
	}

	if( !bShow ) 
	{
		ReleaseSignalImp();
	}
}

EtMatrix CDnEtcObject::GetBoneMatrix( const char *szBoneName, bool *bExistBone )
{
	EtMatrix Mat;
	EtMatrixIdentity(&Mat);
	if( m_hParentActor ) {
		Mat = m_hParentActor->GetBoneMatrix( szBoneName );
	}
	else {
		Mat = CDnActionRenderBase::GetBoneMatrix( szBoneName );
	}
	return Mat;
}

MatrixEx * CDnEtcObject::GetObjectCross()
{
	static MatrixEx Cross;
	if( m_hParentActor ) {
		Cross = *m_hParentActor->GetObjectCross();
		Cross.SetPosition( m_matExWorld.GetPosition() );
	}
	else {
		Cross = m_matExWorld;
	}
	return &Cross;
}


EtLightHandle CDnEtcObject::CreateLight( SLightInfo *pInfo )
{
	return EternityEngine::CreateLight( pInfo, m_hApplyLightObject );
}
