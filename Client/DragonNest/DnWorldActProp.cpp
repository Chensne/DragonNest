#include "StdAfx.h"
#include "DnWorldActProp.h"
#include "EtMatrixEx.h"
#include "DnWorldSector.h"
#include "EtSoundChannel.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "DNProtocol.h"
#include "EtActionSignal.h"
#include "TaskManager.h"
#include "DnGameTask.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

const LOCAL_TIME DONT_HAVE_LIFETIME = -1;

CDnWorldActProp::CDnWorldActProp() : m_pFSM( new TDnFiniteStateMachine<DnPropHandle>(GetMySmartPtr()) ),
									 m_LifeTime( DONT_HAVE_LIFETIME )
{
	EtMatrixIdentity( &m_matWorld );
	CDnActionBase::Initialize( this );

	m_bDestroy = false;

	m_pCheckPreSignalFunc = CDnWorldActProp::CheckPreSignal;
}

CDnWorldActProp::~CDnWorldActProp()
{
	SAFE_DELETE( m_pFSM );
}

bool CDnWorldActProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	bool bResult = CDnWorldProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( bResult == false ) return false;

	CDnWorldSector *pSector = dynamic_cast<CDnWorldSector *>(m_pParentSector);
	if( pSector ) pSector->InsertProcessProp( this );

	SetActionSignalScale( *GetScale() );
	return true;
}

bool CDnWorldActProp::InitializeTable( int nTableID )
{
	if( CDnWorldProp::InitializeTable( nTableID ) == false ) return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );
	m_szAniName = pSox->GetFieldFromLablePtr( nTableID, "_AniName" )->GetString();
	m_szActName = pSox->GetFieldFromLablePtr( nTableID, "_ActName" )->GetString();
	char *szStr = pSox->GetFieldFromLablePtr( nTableID, "_DefaultAction" )->GetString();

	int nActionCount = 1 + _GetNumSubStr( szStr, ';' );
	if( nActionCount > 0 ) {
		m_szDefaultActionName = _GetSubStrByCountSafe( _rand()%nActionCount, szStr, ';' );
	}

	return true;
}

bool CDnWorldActProp::CreateObject()
{
	ASSERT( !m_Handle );
	m_Handle = EternityEngine::CreateAniObject( GetPropName(), ( m_szAniName.empty() ) ? NULL : m_szAniName.c_str() );
	if( m_IntersectionType == Alpha ) {
		if( m_Handle ) {
			m_Handle->GetSkinInstance()->SetRenderAlphaTwoPass( true );
		}
	}

	if( !m_szActName.empty() ) {
		if( LoadAction( CEtResourceMng::GetInstance().GetFullName( m_szActName ).c_str() ) == false ) return false;
	}

	if( m_Handle ) {
		m_Handle->SetCalcPositionFlag( CALC_POSITION_X | CALC_POSITION_Y | CALC_POSITION_Z );
	}

	if( IsExistAction( m_szDefaultActionName.c_str() ) )
		SetActionQueue( m_szDefaultActionName.c_str(), 0, 0.f );

	return true;
}

void CDnWorldActProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	SyncRandomCreated();
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE

	CDnActionBase::ProcessAction( LocalTime, fDelta );
	Process( &m_matWorld, LocalTime, fDelta );
	CDnActionSignalImp::Process( LocalTime, fDelta );
 
	// 라이프타임 있는 경우만.
	if( m_LifeTime != DONT_HAVE_LIFETIME )
	{
		LOCAL_TIME Delta = LOCAL_TIME(fDelta * 1000.0f);
		if( m_LifeTime < Delta )
		{
			_OnLifeTimeEnd( LocalTime, fDelta );
			m_LifeTime = DONT_HAVE_LIFETIME;
		}
		else
			m_LifeTime -= Delta;
	}

	m_pFSM->Process( LocalTime, fDelta );
}

void CDnWorldActProp::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_Handle ) return;
	if( m_nActionIndex == -1 ) return;
	if( m_nActionIndex >= (int)m_nVecAniIndexList.size() ) return;
	if( m_nVecAniIndexList[m_nActionIndex] == -1 ) return;

	m_Handle->SetAniFrame( m_nVecAniIndexList[m_nActionIndex], m_fFrame );
	if( !m_bIsStaticCollision ) m_Handle->Update( pmatWorld );
}

int CDnWorldActProp::GetAniIndex( const char *szAniName )
{
	if( !m_Handle ) return -1;
	for( int i=0; i<m_Handle->GetAniCount(); i++ ) {
		if( strcmp( m_Handle->GetAniName(i), szAniName ) == NULL ) return i;
	}
	return -1;
}

void CDnWorldActProp::ShowRenderBase( bool bShow )
{
	if( !m_Handle ) return;
	m_Handle->ShowObject( bShow );
	Show( bShow );
}

void CDnWorldActProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_AlphaBlending:
			{
				if( !CDnWorldProp::IsShow() ) break;
				if( !m_Handle ) break;
				m_Handle->GetSkinInstance()->SetRenderAlphaTwoPass( true );
				AlphaBlendingStruct *pStruct = (AlphaBlendingStruct *)pPtr;
				float fValue = 1.f / ( SignalEndTime - SignalStartTime - 16.6666f ) * ( LocalTime - SignalStartTime );
				fValue = EtClamp( fValue, 0.0f, 1.0f );
				float fCurAlpha = pStruct->fStartAlpha + ( ( pStruct->fEndAlpha - pStruct->fStartAlpha ) * fValue );
				fCurAlpha = EtClamp( fCurAlpha, 0.0f, 1.0f );
				m_Handle->SetObjectAlpha( fCurAlpha );
				if( fCurAlpha < 0.5f ) {
					if( m_Handle->IsShadowCast() )
						EnableCollision( false );
				}
			}
			break;
		case STE_EnableCollision:
			{
				EnableCollisionStruct *pStruct = (EnableCollisionStruct *)pPtr;
				if( !m_Handle ) break;

				if( !CDnWorldProp::IsShow() ) EnableCollision( false );
				else EnableCollision( pStruct->bEnable ? true : false );
			};
			break;
		case STE_Destroy:
			{
				m_bDestroy = true;
			}
			break;
		case STE_ObjectVisible:
			{
				ObjectVisibleStruct *pStruct = (ObjectVisibleStruct *)pPtr;
				Show( ( pStruct->bShow == TRUE ) );
//				ShowRenderBase( ( pStruct->bShow == TRUE ) );
			}
			break;
		case STE_ChangeAction:
			{
				// #43618 컷신 재생중일땐 무시한다.
				if( false == CGlobalInfo::GetInstance().IsPlayingCutScene() )
				{
					ChangeActionStruct *pStruct = (ChangeActionStruct *)pPtr;
					if( _rand()%100 < pStruct->nRandom ) 
					{
						CmdAction( pStruct->szAction );
					}
				}
			}
			return;
		case STE_CameraEffect_Shake:
			{
				if( !IsShow() ) break;
				CameraEffect_ShakeStruct *pStruct = (CameraEffect_ShakeStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

				if( CDnActor::s_hLocalActor ) {
					float fDistance = EtVec3Length( &EtVector3( GetMatEx()->m_vPosition - *CDnActor::s_hLocalActor->GetPosition() ) );
					if( fDistance <= pStruct->fRange ) {
						float fRolloffDist = pStruct->fRange * pStruct->fRolloff;
						float fRatio = 1.f;

						if( fDistance > fRolloffDist ) 
							fRatio = 1.f - 1.f / ( pStruct->fRange - fRolloffDist ) * ( fDistance - fRolloffDist );

						float fLength = (float)( SignalEndTime - SignalStartTime );
						if( CDnActor::s_hLocalActor->IsModifyPlaySpeed() ) {
							float fTemp = CDnActionBase::m_fFps / 60.f;
							fLength *= fTemp;
						}
						DWORD dwTime = (DWORD)( ( fLength - 16.6666f ) * fRatio );
						pStruct->nCameraEffectRefIndex = hCamera->Quake( dwTime, pStruct->fBeginRatio, pStruct->fEndRatio, pStruct->fShakeValue * fRatio );
					}
				}
			}
			break;

	}
	CDnActionSignalImp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	// 파티클 컬링땜에 셋팅하게 되는 부분입니다만 상속받은 CDnActionSignalImp 쪽에서 먼저 생성 후 계산해야하기때문에
	// 파티클은 아래에서 합니다.
	switch( Type ) {
		case STE_Particle:
			{
				if( !CDnWorldProp::IsShow() ) break;
				ParticleStruct *pStruct = (ParticleStruct *)pPtr;
				if( pStruct->nParticleDataIndex == -1 ) break;
				EtBillboardEffectHandle hParticle;
				if( pStruct->bLinkObject ) hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, LinkParticleSignalStruct>::GetSignalHandle( m_nActionIndex, nSignalIndex );
				else hParticle = TSmartPtrSignalImp<EtBillboardEffectHandle, NormalParticleSignalStruct>::GetSignalHandle( m_nActionIndex, nSignalIndex );
				if( hParticle ) {
					if( m_fVisibleRange > 0.f ) {
						hParticle->SetCullDist( m_fVisibleRange );
					}
				}
			}
			break;
	}
}

void CDnWorldActProp::CmdAction( const char *szActionName, int nLoopCount, float fBlendFrame )
{
	if( !m_Handle || !szActionName) return;

	SetActionQueue( szActionName, nLoopCount, fBlendFrame );
}

void CDnWorldActProp::OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol ) {
		case eProp::SC_CMDACTION:

			{
				CPacketCompressStream Stream( pPacket, 128 );
				int nActionIndex, nLoopCount;
				float fBlendFrame;

				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );

				if( nActionIndex != -1 && GetElement( nActionIndex ) ) {
					CmdAction( GetElement( nActionIndex )->szName.c_str(), nLoopCount, fBlendFrame );
				}
			}
			break;

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
		case eProp::SC_QUERY_RANDOM_CREATED:
	#else
		case eProp::CS_QUERY_RANDOM_CREATED:
	#endif
			{
				// 시작하자마자 컷신 나오는 스테이지는 서버와 동기 맞추고 컷신 종료 후 다시 프랍의 
				// 원래 상태로 돌리기 위해 CutSceneTask 쪽에서 show 를 해버리는 경우가 있으므로 
				// 서버의 랜덤 프랍 생성결과를 CmdShow 뿐만 아니라 받아둔다.
				CPacketCompressStream Stream( pPacket, 32 );
	#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
				Stream.Read( &m_VisibleOptions[PropDef::Option::eRandomCreate], sizeof(PropDef::Option::eValue) );
	#else
				Stream.Read( &m_bRandomResultIsVisible, sizeof(bool) );
	#endif
			}
			break;
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE
	}
	CDnWorldProp::OnDispatchMessage( dwProtocol, pPacket );
}

void CDnWorldActProp::OnChangeAction( const char *szPrevAction )
{
	// 프랍에 파티클 박아논것들이 ChangeAction 이 되면서 남아있으면(Gate들 Particle) 보기가 안조아서 체크해서 없에주게 해논다
	if( strcmp( m_szAction.c_str(), szPrevAction ) == NULL ) return;

	CDnActionSignalImp::OnChangeAction( szPrevAction );

	int nIndex = GetElementIndex( szPrevAction );

	for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetCount(); i++ ) {
		EtEffectObjectHandle hEffect = TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::GetObject(i);
		if( hEffect ) {
			TSmartPtrSignalImp<EtEffectObjectHandle, TSmartPtrSignalStruct::Default>::RemoveObject( i, true );
			i--;
		}
	}
	for( DWORD i=0; i<TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetCount(); i++ ) {
		EtEffectObjectHandle hEffect = TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::GetObject(i);
		if( hEffect ) {
			TSmartPtrSignalImp<EtEffectObjectHandle, LinkFXSignalStruct>::RemoveObject( i, true );
			i--;
		}
	}

}

void CDnWorldActProp::Show( bool bShow )
{
	if( m_bShow == bShow ) return;
	CDnWorldProp::Show( bShow );
	if( !bShow ) ReleaseSignalImp();
}

EtMatrix CDnWorldActProp::GetBoneMatrix( const char *szBoneName, bool *bExistBone )
{
	if( bExistBone ) *bExistBone = false;
	EtMatrix mat;
	EtMatrixIdentity( &mat );
	if( !m_Handle || !m_Handle->GetAniHandle() ) {		
		return mat;
	}
	int nBoneIndex = m_Handle->GetAniHandle()->GetBoneIndex( szBoneName );
	if( nBoneIndex == -1 ) return mat;
	EtMatrixMultiply( &mat, m_Handle->GetBoneTransMat( nBoneIndex ), m_Handle->GetWorldMat() );
	if( bExistBone ) *bExistBone = true;
	return mat;
}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE

bool CDnWorldActProp::IsIgnoreVillageSignal( ActionElementStruct *pElement, CEtActionBase *pActionBase )
{
	CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
	if( !pTask ) return false;

	if( m_szDefaultActionName == pElement->szName ) return false;

	return true;
}

void CDnWorldActProp::CheckPreSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase )
{
	// 마을에선 Default Action 외에 액션에 들어있는 시그널들이 돌 일이 없으므로 패스합니다.
	switch( pSignal->GetSignalIndex() ) {
		case STE_Sound:
			{
				CDnWorldActProp *pProp = dynamic_cast<CDnWorldActProp *>(pActionBase);
				if( pProp && pProp->IsIgnoreVillageSignal( pElement, pActionBase ) == false ) break;

				return;
			}
			break;
		case STE_Particle:
			{
				CDnWorldActProp *pProp = dynamic_cast<CDnWorldActProp *>(pActionBase);
				if( pProp && pProp->IsIgnoreVillageSignal( pElement, pActionBase ) == false ) break;

				return;
			}
			break;
		case STE_FX:
			{
				CDnWorldActProp *pProp = dynamic_cast<CDnWorldActProp *>(pActionBase);
				if( pProp && pProp->IsIgnoreVillageSignal( pElement, pActionBase ) == false ) break;
				return;
			}
			break;
		case STE_ActionObject:
			{
				CDnWorldActProp *pProp = dynamic_cast<CDnWorldActProp *>(pActionBase);
				if( pProp && pProp->IsIgnoreVillageSignal( pElement, pActionBase ) == false ) break;
				return;
			}
			break;
	}
	CDnActionBase::CheckPreSignal( pElement, nElementIndex, pSignal, nSignalIndex, pActionBase );
}
#endif

#ifdef PRE_FIX_PROP_RANDOM_VISIBLE
void CDnWorldActProp::SyncRandomCreated()
{
	if( m_bFirstProcess )
	{
		bool bSyncComplete = false;
		CDnGameTask* pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
		if( pGameTask )
			bSyncComplete = pGameTask->IsSyncComplete();

		if( bSyncComplete )
		{
			if( 100 != m_iVisiblePercent )
				Send( eProp::CS_QUERY_RANDOM_CREATED, NULL );

			m_bFirstProcess = false;
		}
	}

	// 서버에서 랜덤 생성 실패한 것이라면 무조건 숨김.
#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	if (IsVisible() == false)
		Show(false);
#else
	if( false == m_bRandomResultIsVisible )
		Show( false );
#endif
}
#endif // #ifdef PRE_FIX_PROP_RANDOM_VISIBLE
