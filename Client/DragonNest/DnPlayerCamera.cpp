#include "StdAfx.h"
#include "DnPlayerCamera.h"
#include "DnWorld.h"
#include "VelocityFunc.h"
#include "MAMovementBase.h"
#include "DnLocalPlayerActor.h"
#include "GameOption.h"
#include "DnInterface.h"
#include "DnAniCamera.h"
#include "InputWrapper.h"
#include "EtConvexVolume.h"
#include "DnLifeSkillFishingTask.h"
#include "TaskManager.h"
#include "DnGameTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


float CDnPlayerCamera::s_fCenterPushWidth = 35.f;
float CDnPlayerCamera::s_fCenterPushHeight = 70.f;
float CDnPlayerCamera::s_fLastDistance = CAMERA_ZOOM_MAX;

extern float s_fCrossHairHeightRatio;

#ifdef PRE_ADD_JAPAN_AUTO_AIMING
int g_iAutoAdmingRangeDelta = 500;
#endif


CDnPlayerCamera::CDnPlayerCamera()
: CInputReceiver( true )
{
	m_CameraType = CameraTypeEnum::PlayerCamera;

	if(s_fLastDistance > CAMERA_ZOOM_MAX || s_fLastDistance < CAMERA_ZOOM_MIN)
		s_fLastDistance = CAMERA_ZOOM_MAX;
	
	//rlkt_camera
	if (CGameOption::GetInstance().m_bExtendView)
		s_fLastDistance += 200.f;

	m_fDistance = s_fLastDistance;
	m_fDistanceBeforeSmooth = s_fLastDistance;
	m_fDistanceAccel = 0.f;

	m_CameraInfo.fFogNear = 100000.f;
	m_CameraInfo.fFogFar = 100000.f;
	m_CameraInfo.fFar = 100000.f;
	m_fChangeViewVectorDelta = 0.f;
	m_fChangeViewCurDelta = 0.f;
	m_fPitchValue = 0.f;
	m_fYawValue = 0.f;
	m_fForceYawValue = 0.f;

	m_fPrevPropDistance = 0.f;
	m_fCurrentDistance = 0.f;
	m_fTargetDistance = 0.f;

	m_fChangeViewPrevYaw = m_fChangeViewYaw = 0.f;

	m_bLockFreeYaw = false;
	m_bFollowCam = false;
	m_bFirstProcess = true;

	m_fPitchSmoothValue = 0.f;
	m_fYawSmoothValue = 0.f;

	m_fSmoothAnglePower = 20.0f;
	m_fSmoothZoomPower = 20.0f;

	m_bResetLookWhenActivated = false;

#if defined( PRE_PLAYER_CAMERA )
	m_bUsePitchCamera = true;
#else
	m_bUsePitchCamera = false;
#endif

#ifdef PRE_ADD_JAPAN_AUTO_AIMING
	m_bPrevAutoAmingBtnDown = false;

	m_ahLockOnTexture[ 0 ] = LoadResource( "Otto_CrossHair01.dds", RT_TEXTURE, true );
	m_ahLockOnTexture[ 1 ] = LoadResource( "Otto_CrossHair02.dds", RT_TEXTURE, true );

	m_iNowLockOnTextureIndex = 0;
	m_LockOnFrameChangeTime = 0;
#endif
	m_bPrevTurnBtnDown = false;

	m_PitchCamera = new CDnAniCamera;
	if( !m_PitchCamera->AddCameraData( "PlayerCamera.cam" ) ) {
		m_bUsePitchCamera = false;
	}

	m_fMinZoom = CAMERA_ZOOM_MIN;
	m_fMaxZoom = CAMERA_ZOOM_MAX;	

	//rlkt_camera
	if (CGameOption::GetInstance().m_bExtendView)
		m_fMaxZoom += 200.f;
	

	m_bMouseInvert = false;
}

CDnPlayerCamera::~CDnPlayerCamera()
{
	SAFE_DELETE( m_PitchCamera );
	s_fLastDistance = m_fDistance;

#ifdef PRE_ADD_JAPAN_AUTO_AIMING
	for( int i = 0; i < LOCKON_TEXTURE_COUNT; ++i )
		SAFE_RELEASE_SPTR( m_ahLockOnTexture[ i ] );	
#endif // PRE_ADD_JAPAN_AUTO_AIMING
}

bool CDnPlayerCamera::Activate()
{
	// 그래픽 퀄리티 낮음
	if( CGameOption::GetInstance().GetCurGraphicQuality() >= 2 )
	{
		if( m_CameraInfo.fFar > 5000.0f ) m_CameraInfo.fFar = 5000.0f;
		if( m_CameraInfo.fFogFar > 4500.0f ) {
			m_CameraInfo.fFogFar = 4500.0f;
			if( m_CameraInfo.fFogNear > 3000.0f ) {
				m_CameraInfo.fFogNear = 3000.0f;
			}
		}
	}
	m_hCamera = EternityEngine::CreateCamera( &m_CameraInfo );

	m_bMouseInvert = CGameOption::GetInstance().GetMouseInvert();

	if( m_bResetLookWhenActivated ) // 현재 다른 카메라가 붙어있을때 예약되어 사용됩니다.
	{
		ResetLook();
		m_bResetLookWhenActivated = false;
	}

	m_bFirstProcess = true;
	return ( m_hCamera ) ? true : false;
}

bool CDnPlayerCamera::DeActivate()
{
	CDnCamera::DeActivate();
	SAFE_RELEASE_SPTR( m_hCamera );
	return true;
}

void CDnPlayerCamera::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hCamera ) return;

	if( m_hAttachActor )
	{
		bool isInverseMode = false;
		isInverseMode = CInputDevice::IsInverseMouse();

		bool bNowFishing = false;
		if( CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing() )
			bNowFishing = true;

		if( m_bLockFreeYaw && !m_hAttachActor->IsStay() && !bNowFishing )
			LockFreeYaw( false );

		float fYawDelta = 0.f;
		float fPitchDelta = 0.f;

		if( !CDnMouseCursor::GetInstance().IsShowCursor() )
		{
			if( !CDnLocalPlayerActor::IsLockInput() || bNowFishing )	// 낚시 중일때는 lockinput일때도 카메라는 동작하도록 함
			{
#ifdef PRE_ADD_JAPAN_AUTO_AIMING
				// 만약 오토록온 상태라면 사용자의 입력대신 오토 록온 프로세스로 처리된다.
				CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

				if( ( IsPushKey( g_WrappingKeyData[IW_AIMASSIST] ) || IsPushJoypadButton( IW_PAD(IW_AIMASSIST) ) ) 
					&& ( pTask && pTask->GetGameTaskType() != GameTaskType::PvP ) 
					)
				{
					DnActorHandle hLocalPlayerActor = CDnActor::s_hLocalActor;

					// 계속 누르고 있는 동안에는 마지막에 조준되었던 자동 타겟팅 액터가 바뀌지 않도록 한다.
					EtVector3 vStartPos, vDir;
					// 현재 카메라 위치에서 크로스 헤어 위치쪽으로 일직선을 그어 가장 가까이 있는 몹을 찾는다.
					int iCrossHairXPos = CEtDevice::GetInstance().Width() / 2;
					int iCrossHairYPos = (int)( CEtDevice::GetInstance().Height() * s_fCrossHairHeightRatio );
					m_hCamera->CalcPositionAndDir( iCrossHairXPos, iCrossHairYPos, vStartPos, vDir );
					
					float fAttackRange = 0.0f;

					// 내 액터의 사거리를 얻는다.
					DnWeaponHandle hMainWeapon = hLocalPlayerActor->GetWeapon( 0 );
					if( hMainWeapon )
						fAttackRange += (float)hMainWeapon->GetWeaponLength();
					fAttackRange += (float)g_iAutoAdmingRangeDelta;
					if( fAttackRange < 0.0f )
						fAttackRange = 1.0f;

					// 스킬 사용중인 경우 사거리 증가 표시.
					if( hLocalPlayerActor->IsProcessSkill() ) 
					{
						DnSkillHandle hSkill = hLocalPlayerActor->GetProcessSkill();
						if( hSkill )
						{
							fAttackRange += hSkill->GetIncreaseRange();
							//if( hSkill->GetTargetType() == CDnSkill::Friend ) bEnemy = false;
						}
					}

					// 죽은 놈들은 오토 타겟팅에서 제외 제외시킨다. (#14423)
					if( m_hLastAutoAmingActor && m_hLastAutoAmingActor->IsDie() )
						m_hLastAutoAmingActor.Identity();

					DnActorHandle hResultTargetActor;
					if( false == m_bPrevAutoAmingBtnDown || !m_hLastAutoAmingActor )
					{	
						// 선분을 그어 가장 가까운 몬스터를 찾는다.
						SSegment Segment;
						Segment.vOrigin = vStartPos;
						Segment.vDirection = vDir * fAttackRange;

						DNVector( DnActorHandle ) vlScanResult;
						hLocalPlayerActor->ScanActor( *hLocalPlayerActor->GetPosition(), fAttackRange, vlScanResult );

						CEtConvexVolume Frustum;
						float fNearestDegree = FLT_MAX;
						Frustum.Initialize( *m_hCamera->GetViewProjMatForCull() );
						m_hLastAutoAmingActor.Identity();
						int iNumActors = (int)vlScanResult.size();
						for( int iActor = 0; iActor < iNumActors; ++iActor )
						{
							DnActorHandle hActor = vlScanResult.at( iActor );
							// npc 를 제외한 다른 팀 액터들을 검색.
							// 죽은 놈들도 제외시킨다. (#14423)
							if( !hActor || 
								hActor->IsNpcActor() ||
								hActor->GetTeam() == hLocalPlayerActor->GetTeam() ||
								hActor->IsDie() )
								continue;
							
							// 카메라 절두체에 들어온 애들만.
							SAABox AABB;
							hActor->GetBoundingBox( AABB );
							bool bInFrustum = Frustum.TesToBox( AABB.GetCenter(), AABB.GetExtent() );
							if( bInFrustum )
							{
								// 일정 각도값 이하인 녀석들만 골라준다.
								// TODO: 추후엔 높이 각도까지 고려해야 함.
								// 아직은 정해진 값이 없으므로 그냥 임의로 30도로.
								EtVector3 vOrigToTargetDir = *hActor->GetPosition() - vStartPos;
								EtVec3Normalize( &vOrigToTargetDir, &vOrigToTargetDir );
								float fDot = EtVec3Dot( &vOrigToTargetDir, &vDir );
								float fResultDegree = EtToDegree( EtAcos( fDot ) );
								if( fResultDegree < 30.0f )
								{
									if( fResultDegree < fNearestDegree )
									{
										fNearestDegree = fResultDegree;
										hResultTargetActor = hActor;
									}
								}
							}
						}
					}
					else
					{
						hResultTargetActor = m_hLastAutoAmingActor;

						// 이전에 락온 되었던 대상이 사거리에서 빠져 나간다면 해제시킨다.
						DNVector( DnActorHandle ) vlScanResult;
						hLocalPlayerActor->ScanActor( *hLocalPlayerActor->GetPosition(), fAttackRange, vlScanResult );
						if( vlScanResult.end() == find( vlScanResult.begin(), vlScanResult.end(), hResultTargetActor ) )
						{
							m_hLastAutoAmingActor.Identity();
							hResultTargetActor.Identity();
						}
					}

					if( hResultTargetActor )
					{
						m_hLastAutoAmingActor = hResultTargetActor;

						// 플레이어 캐릭터와 비교해서 캐릭터의 각도를 담당하는 값을 틀어준다.
						EtVector3 vCurLookDir;
						//hLocalPlayerActor->GetCurHeadLookDir( vCurLookDir );
						vCurLookDir = *m_hCamera->GetDirection();

						EtVector3 vTargetActorAmingPos = *hResultTargetActor->GetPosition();

						// 몬스터 별로 스케일, anidistancet y 값 까지 고려된 정확한 위치를 뽑아낸다.
						SAABox AABB;
						hResultTargetActor->GetBoundingBox(AABB);
						float fHeight = AABB.Max.y - AABB.Min.y;
						//fHeight *= m_hActor->GetScale()->y;
						vTargetActorAmingPos.y += fHeight/4.0f;

						EtVector3 vPlayerActorToTargetDir = vTargetActorAmingPos - vStartPos/**hLocalPlayerActor->GetPosition()*/;
						EtVec3Normalize( &vPlayerActorToTargetDir, &vPlayerActorToTargetDir );

						EtVector2 vXZCurLookDir( vCurLookDir.x, vCurLookDir.z );
						EtVector2 vXZToTargetDir( vPlayerActorToTargetDir.x, vPlayerActorToTargetDir.z );

						EtVec2Normalize( &vXZCurLookDir, &vXZCurLookDir );
						EtVec2Normalize( &vXZToTargetDir, &vXZToTargetDir );

						float fXZDot = EtVec2Dot( &vXZCurLookDir, &vXZToTargetDir );
						fYawDelta = EtToDegree( EtAcos( fXZDot ) ) / 5.0f;
						EtVector3 vXZCross;
						EtVector3 vXZCurLookDirTemp( vXZCurLookDir.x, 0.0f, vXZCurLookDir.y );
						EtVector3 vXZToTargetDirTemp( vXZToTargetDir.x, 0.0f, vXZToTargetDir.y );
						EtVec3Cross( &vXZCross, &vXZToTargetDirTemp, &vXZCurLookDirTemp );
						if( 0.0f < vXZCross.y )
							fYawDelta = -fYawDelta;

						// pitch 각도 변화량을 구하기 위해선 약간 다르게..
						// y, z 만 추출해서 하면 부호에 따른 문제가 생기므로 자신의 기준 벡터별로 몇 도인지 구해서 차이값으로 처리.
						EtVector3 vCurLookDirZ( vCurLookDir.x, 0.0f, vCurLookDir.z );
						EtVec3Normalize( &vCurLookDirZ, &vCurLookDirZ );
						float fCurLookDot = EtVec3Dot( &vCurLookDir, &vCurLookDirZ );
						float fCurLookDegree = EtToDegree( EtAcos( fCurLookDot ) );
						if( vCurLookDir.y < 0.0f )
							fCurLookDegree = -fCurLookDegree;

						EtVector3 vToTargetDirZ( vPlayerActorToTargetDir.x, 0.0f, vPlayerActorToTargetDir.z );
						EtVec3Normalize( &vToTargetDirZ, &vToTargetDirZ );
						float fToTargetDot = EtVec3Dot( &vPlayerActorToTargetDir, &vToTargetDirZ );
						float fToTargetDegree = EtToDegree( EtAcos( fToTargetDot ) );
						if( vPlayerActorToTargetDir.y < 0.0f )
							fToTargetDegree = -fToTargetDegree;

						// pitch 는 음수로 하는 것이 카메라 뷰가 올라감.
						fPitchDelta = -((fToTargetDegree - fCurLookDegree) / 5.0f);

						// 대상에게 2D 락온 표시를 해준다.
						if( 300 < LocalTime - m_LockOnFrameChangeTime )
						{
							m_iNowLockOnTextureIndex = (m_iNowLockOnTextureIndex + 1) % LOCKON_TEXTURE_COUNT;
							m_LockOnFrameChangeTime = LocalTime;
						}

						EtMatrix matIdentity;
						EtMatrixIdentity( &matIdentity );
						EtVector3 vScreenPos;
						vTargetActorAmingPos.y += fHeight/4.0f;
						EtVec3Project( &vScreenPos, &vTargetActorAmingPos, 
									   m_hCamera->GetViewport(), 
									   m_hCamera->GetProjMat(),
									   m_hCamera->GetViewMat(),
									   &matIdentity );

						//GetEtDevice()->BeginScene();
						//CEtSprite::GetInstance().Begin( D3DXSPRITE_ALPHABLEND );
						//vScreenPos.z = 1.0f;
						//CEtSprite::GetInstance().Draw( (EtTexture *)m_ahLockOnTexture[ m_iNowLockOnTextureIndex ]->GetTexturePtr(), NULL, &EtVector3( 32.0f, 32.0f, 0.0f ), &vScreenPos, 0xffffffff );
						//CEtSprite::GetInstance().End();
						//GetEtDevice()->EndScene();

						EtVector4 Vertices[ 4 ];
						EtVector2 TexCoord[ 4 ];
						float fTextureWidth = (float)m_ahLockOnTexture[ m_iNowLockOnTextureIndex ]->Width();
						float fTextureHeight = (float)m_ahLockOnTexture[ m_iNowLockOnTextureIndex ]->Height();
						vScreenPos.x -= fTextureWidth / 2.0f;
						vScreenPos.y -= fTextureHeight / 2.0f;

						Vertices[ 0 ] = EtVector4( vScreenPos.x, vScreenPos.y, 0.0f, 0.0f );
						Vertices[ 1 ] = EtVector4( vScreenPos.x + fTextureWidth, vScreenPos.y, 0.0f, 0.0f );
						Vertices[ 2 ] = EtVector4( vScreenPos.x + fTextureWidth, vScreenPos.y + fTextureHeight, 0.0f, 0.0f );
						Vertices[ 3 ] = EtVector4( vScreenPos.x, vScreenPos.y + fTextureHeight, 0.0f, 0.0f );

						TexCoord[ 0 ] = EtVector2( 0.0f, 0.0f );
						TexCoord[ 1 ] = EtVector2( 1.0f, 0.0f );
						TexCoord[ 2 ] = EtVector2( 1.0f, 1.0f );
						TexCoord[ 3 ] = EtVector2( 0.0f, 1.0f );

						EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, m_ahLockOnTexture[ m_iNowLockOnTextureIndex ]->GetTexturePtr() );

						//EtVector3 vPitchCross;
						//vPlayerActorToTargetDir.x = 0.0f;
						//vCurLookDir.x = 0.0f;
						//EtVec3Normalize( &vPlayerActorToTargetDir, &vPlayerActorToTargetDir );
						//EtVec3Normalize( &vCurLookDir, &vCurLookDir );
						//EtVec3Cross( &vPitchCross, &vPlayerActorToTargetDir, &vCurLookDir );
						//if( 0.0f < vPitchCross.y )
						//	fPitchDelta = -fPitchDelta;

						//EtVector2 vYZCurLookDir( vCurLookDir.y, vCurLookDir.z );
						//EtVector2 vYZToTargetDir( vPlayerActorToTargetDir.y, vPlayerActorToTargetDir.z );

						//EtVec2Normalize( &vYZCurLookDir, &vYZCurLookDir );
						//EtVec2Normalize( &vYZToTargetDir, &vYZToTargetDir );

						//float fYZDot = EtVec2Dot( &vYZCurLookDir, &vYZToTargetDir );
						//fPitchDelta = EtToDegree( EtAcos( fYZDot ) ) / 5.0f;
						//EtVector3 vYZCross;
						//EtVector3 vYZCurLookDirTemp( vYZCurLookDir.x, 0.0f, vYZCurLookDir.y );
						//EtVector3 vYZToTargetDirTemp( vYZToTargetDir.x, 0.0f, vYZToTargetDir.y );
						//EtVec3Cross( &vYZCross, &vYZCurLookDirTemp, &vYZToTargetDirTemp );
						//if( 0.0f < vYZCross.y )
						//	fPitchDelta = -fPitchDelta;
					}
				}
				else
#endif // PRE_ADD_JAPAN_AUTO_AIMING
				if( false == m_bPrevTurnBtnDown && 
					( IsPushKey( g_WrappingKeyData[IW_TURN] ) || IsPushJoypadButton( IW_PAD(IW_TURN) ) ) )		// 뒤로 돌기 버튼은 누르고 있는 동안 한번만.
				{
					// 뒤 돌기
					fYawDelta = EtToDegree( ET_PI );
				}
				// 기획의 요청에 따라 마우스 입력이 오토 타겟팅과 같이 먹도록 처리.
				//else
				{
					if( GetMouseVariation().x != 0.f ) {
						fYawDelta += ((isInverseMode == false) ? GetMouseVariation().x : -GetMouseVariation().x) * CDnMouseCursor::GetInstance().GetMouseSensitivity() * GetFreezeResistanceRatio();
					}
					if( GetMouseVariation().y != 0.f ) {
						fPitchDelta += ((isInverseMode == false) ? GetMouseVariation().y : -GetMouseVariation().y) * CDnMouseCursor::GetInstance().GetMouseSensitivity() * GetFreezeResistanceRatio();
					}

					if( IsPushJoypadButton( IW_PAD((isInverseMode == false) ? IW_LOOKRIGHT : IW_LOOKLEFT) ) )
						fYawDelta += GetCameraSpeed() * GetFreezeResistanceRatio() * CInputJoyPad::VIEW_DELTA;
					else if( IsPushJoypadButton( IW_PAD((isInverseMode == false) ? IW_LOOKLEFT : IW_LOOKRIGHT) ) )
						fYawDelta -= GetCameraSpeed() * GetFreezeResistanceRatio() * CInputJoyPad::VIEW_DELTA;

					if( IsPushJoypadButton( IW_PAD((isInverseMode == false) ? IW_LOOKDOWN : IW_LOOKUP) ) )
						fPitchDelta += GetCameraSpeed() * GetFreezeResistanceRatio() * CInputJoyPad::VIEW_DELTA;
					else if( IsPushJoypadButton( IW_PAD((isInverseMode == false) ? IW_LOOKUP : IW_LOOKDOWN) ) )
						fPitchDelta -= GetCameraSpeed() * GetFreezeResistanceRatio() * CInputJoyPad::VIEW_DELTA;
				}
			}
		}

		if( m_bMouseInvert )
			fPitchDelta = -fPitchDelta;

		if( !CDnMouseCursor::GetInstance().IsShowCursor() || bNowFishing )	// 낚시 중일때는 lockinput일때도 카메라는 동작하도록 함
		{
			if( GetMouseVariation().z != 0.f ) {
				m_fDistanceAccel = GetMouseVariation().z / 14.f;
			}

			if ( IsPushJoypadButton( IW_PAD(IW_ZOOMIN) ) ) {
				m_fDistanceAccel = 1.4f;
			}
			else if( IsPushJoypadButton( IW_PAD(IW_ZOOMOUT) ) ) {
				m_fDistanceAccel = -1.4f;
			}
		}

#ifdef PRE_ADD_JAPAN_AUTO_AIMING
		m_bPrevAutoAmingBtnDown = ( IsPushKey( g_WrappingKeyData[IW_AIMASSIST] ) || IsPushJoypadButton( IW_PAD(IW_AIMASSIST) ) );
#endif	//#ifdef PRE_ADD_JAPAN_AUTO_AIMING
		m_bPrevTurnBtnDown = ( IsPushKey( g_WrappingKeyData[IW_TURN] ) || IsPushJoypadButton(IW_PAD(IW_TURN) ) );
		
		m_fDistance += (m_fDistanceBeforeSmooth - m_fDistance) * EtMin(1.0f, ( fDelta * m_fSmoothZoomPower ));

		if( m_fDistanceAccel != 0.f ) {	
			m_fDistanceBeforeSmooth -= m_fDistanceAccel * 35.f / 6.f;

			m_fDistanceBeforeSmooth = EtClamp( m_fDistanceBeforeSmooth, m_fMinZoom, m_fMaxZoom ); // View Distance 를 조작가능하다
			m_fDistanceAccel = 0;		
		}

		if(m_fForceYawValue !=0)
		{
			fYawDelta = m_fForceYawValue * fDelta;
		}

		if( CDnLocalPlayerActor::IsLockInput() && !bNowFishing ) return;

		m_fYawValue += fYawDelta;
		m_fPitchValue += fPitchDelta;
		m_fPitchValue = EtClamp( m_fPitchValue, -45.f, 45.f );

		m_matExWorld.m_vXAxis = EtVector3(1, 0, 0);
		m_matExWorld.m_vYAxis = EtVector3(0, 1, 0);
		m_matExWorld.m_vZAxis = EtVector3(0, 0, 1);
		
		m_fYawSmoothValue += (m_fYawValue - m_fYawSmoothValue) * EtMin(1.0f, ( fDelta * m_fSmoothAnglePower ));

		if( m_fChangeViewVectorDelta > 0.f && m_fChangeViewCurDelta > 0.f && m_fChangeViewPrevYaw != 0.f ) m_fYawValue += -m_fChangeViewPrevYaw;
		if( !m_bUsePitchCamera ) {
			m_matExWorld.RotateYAxis( m_fYawSmoothValue );
		}

		m_fPitchSmoothValue += (m_fPitchValue - m_fPitchSmoothValue) * EtMin(1.0f, ( fDelta * m_fSmoothAnglePower ));

		if( !m_bUsePitchCamera ) {
			m_fPitchSmoothValue += (m_fPitchValue - m_fPitchSmoothValue) * EtMin(1.0f, ( fDelta * m_fSmoothAnglePower ));
			m_matExWorld.RotatePitch( m_fPitchSmoothValue );
		}

		if( m_bUsePitchCamera ) {
			float fPitchFrame = 100.0f - 100.0f * ( m_fPitchSmoothValue + 45.0f ) / 90.0f;
			EtVector3 vPos;
			EtQuat Quat;
			CDnAniCamera::AniCameraStruct *pCameraStruct = m_PitchCamera->GetCameraStruct( 0 );

			pCameraStruct->CalcPosition( vPos, fPitchFrame );
			pCameraStruct->CalcRotation( Quat, fPitchFrame );

			EtMatrix MatYaw, MatPitch;

			EtMatrixRotationY( &MatYaw, D3DXToRadian( m_fYawSmoothValue ) );
			EtMatrixRotationQuaternion( &MatPitch, &Quat );

			const static float fDivideScale = sqrtf(30.0f * 30.0f * 2.0f);

			m_matExWorld = MatPitch;	
			m_matExWorld.m_vPosition = vPos * m_fDistance / fDivideScale;
			EtMatrixMultiply(m_matExWorld, m_matExWorld, &MatYaw);

			if( m_hAttachActor ) {
				m_matExWorld.m_vPosition += m_hAttachActor->GetMatEx()->m_vPosition;
				m_matExWorld.MoveLocalXAxis( s_fCenterPushWidth );
				m_matExWorld.m_vPosition.y += s_fCenterPushHeight;
			}
		}

		CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor *>(m_hAttachActor.GetPointer());
		CDnVehicleActor *pVehicle = dynamic_cast<CDnVehicleActor *>(m_hAttachActor.GetPointer());
		if( (!pPlayer || !pPlayer->IsLocalActor()) && (!pVehicle || !pVehicle->IsLocalVehicle()) ) return;

		if( pPlayer && pPlayer->IsIgnoreRefreshViewCamera() ) return;
		if( m_bLockFreeYaw ) return;
		if( m_bFollowCam ) return;
		if( IsFreeze() ) return;

		if( !bNowFishing )
			RefreshPlayerView( fDelta, false );
	}
}

void CDnPlayerCamera::PostProcess( LOCAL_TIME LocalTime, float fDelta )
{
	float fHeightDistance = 0.f;
	float fPropDistance = 0.f;
	EtVector3 vCameraAt;
	bool bPickHeight = false;
	bool bPickWater = false;
	if( m_hAttachActor ) {
		MatrixEx TempCross = m_matExWorld;

		if( !m_bUsePitchCamera ) {
			TempCross.m_vPosition = m_hAttachActor->GetMatEx()->m_vPosition;
			TempCross.m_vPosition += TempCross.m_vXAxis * s_fCenterPushWidth;
			TempCross.m_vPosition.y += s_fCenterPushHeight;
			vCameraAt = TempCross.m_vPosition;
			//EternityEngine::DrawPoint3D( vCameraAt, D3DCOLOR_XRGB(255, 0, 255)  );
			TempCross.MoveLocalZAxis( -m_fDistance );
		}


		// Height 높이값 체크
		bool bResult = false;
		EtVector3 vPickPos;
		EtVector3 vAtTemp = vCameraAt;
		if( !m_bUsePitchCamera ) {
			vAtTemp.y -= 20.f;
			bResult = CDnWorld::GetInstance().Pick( vAtTemp, -TempCross.m_vZAxis, vPickPos );
			if( bResult ) {
				vAtTemp.y += 20.f;
				vPickPos.y += 20.f;
				float fLength = EtVec3Length( &( vAtTemp - vPickPos ) ); 
				if (fLength <= CAMERA_ZOOM_MAX) {
					fHeightDistance = m_fDistance - fLength;
					bPickHeight = true;
				}
			}
		}
		// 물 높이값 체크
		vAtTemp = vCameraAt;
		vAtTemp.y -= 20.f;
		bResult = CDnWorld::GetInstance().PickWater( vAtTemp, -TempCross.m_vZAxis, vPickPos );
		if( bResult ) {
			vAtTemp.y += 20.f;
			vPickPos.y += 20.f;
			float fLength = EtVec3Length( &( vAtTemp - vPickPos ) ); 
			if (fLength <= CAMERA_ZOOM_MAX) {
				if (m_fDistance - fLength > fHeightDistance) {
					fHeightDistance = m_fDistance - fLength;
					bPickWater = true;
				}
			}
		}

		// Prop Visible 체크
		fPropDistance = CDnWorld::GetInstance().GetPropCollisionDistance();
		if( m_fDistance - fPropDistance < CAMERA_ZOOM_MIN ) fPropDistance = m_fDistance - CAMERA_ZOOM_MIN;

		if( m_fChangeViewVectorDelta > 0.f ) {
			m_fChangeViewCurDelta -= fDelta;
			if( m_fChangeViewCurDelta <= 0.f ) {
				m_fChangeViewVectorDelta = m_fChangeViewCurDelta = m_fChangeViewPrevYaw = 0.f;
				m_fYawValue += m_fChangeViewYaw;
				OutputDebug( "끝!\n" );
			}
			else {
				float fWeight = 1.f - ( 1.f / m_fChangeViewVectorDelta ) * m_fChangeViewCurDelta;
				m_fChangeViewPrevYaw = m_fChangeViewYaw * fWeight;
				m_fYawValue += m_fChangeViewPrevYaw;
				OutputDebug( "회전 :%.2f\n", m_fChangeViewPrevYaw );

				MAMovementBase *pMovement = m_hAttachActor->GetMovement();
				if( pMovement ) {
					pMovement->SetMoveVectorX( m_hAttachActor->GetMatEx()->m_vXAxis );
					pMovement->SetMoveVectorZ( m_hAttachActor->GetMatEx()->m_vZAxis );
				}

			}
		}
		m_matExWorld = TempCross;
	}

	ProcessEffect( LocalTime, fDelta );

	if( !m_bFirstProcess ) m_fTargetDistance = max( fPropDistance, fHeightDistance );

	float fValue = 500.f * fDelta;
	float fRange = fabs( m_fTargetDistance - m_fCurrentDistance );
	if( fRange < 90.f ) {
		float fTemp = sin( fRange / 180.f * D3DX_PI );
		fValue *= fTemp;
	}

	// Height쪽 당겨질때 갭이 크면 강제로 당겨준다.
	bool bCheckPostHeight = false;
	if( fHeightDistance != 0.f && m_fTargetDistance == fHeightDistance && m_fCurrentDistance < m_fTargetDistance ) 
	{
		if( bPickHeight || bPickWater ) {
			m_fCurrentDistance = m_fTargetDistance;
		}
	}

	if( m_fCurrentDistance < m_fTargetDistance ) 
	{
		m_fCurrentDistance += fValue;
		if( m_fCurrentDistance >= m_fTargetDistance ) m_fCurrentDistance = m_fTargetDistance;
	}
	if( m_fCurrentDistance > m_fTargetDistance ) 
	{
		m_fCurrentDistance -= fValue;
		if( m_fCurrentDistance <= m_fTargetDistance ) m_fCurrentDistance = m_fTargetDistance;
	}

	if( m_fCurrentDistance > 0.f ) m_matExWorld.MoveLocalZAxis( m_fCurrentDistance );

	m_hCamera->Update( m_matExWorld );

	if( m_fCurrentDistance > 0.f ) m_matExWorld.MoveLocalZAxis( -m_fCurrentDistance );

	m_bFirstProcess = false;
}

void CDnPlayerCamera::ChangeViewVector( float fDelta )
{
	if( !m_hAttachActor ) return;
	EtVector3 vVec, vTargetVec, vCross;
	vVec = m_matExWorld.m_vZAxis;
	vVec.y = 0.f;
	EtVec3Normalize( &vVec, &vVec );
	vTargetVec = m_hAttachActor->GetMatEx()->m_vZAxis;
	vTargetVec.y = 0.f;
	EtVec3Normalize( &vTargetVec, &vTargetVec );
	float fDot = EtVec3Dot( &vVec, &vTargetVec );
	if( fDot == 0.f ) return;
	if( fDot >= 1.f ) return;

	m_fChangeViewYaw = EtToDegree( EtAcos( fDot ) );
	EtVec3Cross( &vCross, &vVec, &vTargetVec );
	if( vCross.y <= 0.f ) m_fChangeViewYaw = -m_fChangeViewYaw;

	m_fChangeViewVectorDelta = fDelta;
	m_fChangeViewCurDelta = fDelta;
	m_fChangeViewPrevYaw = 0.f;
}

void CDnPlayerCamera::AttachActor( DnActorHandle hHandle )
{
	if( m_hAttachActor ) DetachActor();
	m_hAttachActor = hHandle;

	ResetCamera();
}

void CDnPlayerCamera::DetachActor()
{
	m_hAttachActor.Identity();
}

void CDnPlayerCamera::ResetCamera()
{
	if( !m_hAttachActor || !m_hCamera ) return;

	CDnCamera::ResetCamera();

	MatrixEx *pActorCross = m_hAttachActor->GetMatEx();
	m_matExWorld = *pActorCross;
	m_fPitchValue = 0.f;
	m_fPitchSmoothValue = 0.f;
//	m_fDistance = 200.f;
	m_matExWorld.RotatePitch( m_fPitchValue );
	m_matExWorld.MoveLocalZAxis( -m_fDistance );
	m_matExWorld.RotateYaw( 1.f );

	m_fYawValue = EtToDegree( atan2f( m_matExWorld.m_vZAxis.x, m_matExWorld.m_vZAxis.z) );
	m_fYawSmoothValue = m_fYawValue;
	m_fForceYawValue = 0.f;

}

void CDnPlayerCamera::ResetLook()
{
	if( !m_hAttachActor || !m_hCamera ) return;

	MatrixEx *pActorCross = m_hAttachActor->GetMatEx();
	m_matExWorld = *pActorCross;
	m_matExWorld.RotateYaw( 1.f );

	m_fYawValue = EtToDegree( atan2f( m_matExWorld.m_vZAxis.x, m_matExWorld.m_vZAxis.z) );
	m_fYawSmoothValue = m_fYawValue;
	m_fForceYawValue = 0.f;
}

void CDnPlayerCamera::ForceRefreshEngineCamera()
{
	if( m_fCurrentDistance > 0.f ) m_matExWorld.MoveLocalZAxis( m_fCurrentDistance );
	if( m_hCamera ) m_hCamera->Update( m_matExWorld );
	if( m_fCurrentDistance > 0.f ) m_matExWorld.MoveLocalZAxis( -m_fCurrentDistance );
}

void CDnPlayerCamera::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	if( nReceiverState & IR_MOUSE_WB_DOWN ) {
		if( m_hAttachActor && m_hAttachActor->IsStay() ) LockFreeYaw( true );
		else LockFreeYaw( false );
	}
	if( nReceiverState & IR_MOUSE_MOVE ) {
		if( m_hAttachActor ) {
			if(m_hAttachActor->IsPlayerActor())
			{
				CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>(m_hAttachActor.GetPointer());
				if( pPlayer && !pPlayer->IsBattleMode() && m_hAttachActor->IsStay() )
					LockFreeYaw( true );
			}
			else if(m_hAttachActor->IsVehicleActor())
			{
				CDnLocalVehicleActor *pVehicle = dynamic_cast<CDnLocalVehicleActor*>(m_hAttachActor.GetPointer());
				if( pVehicle && m_hAttachActor->IsStay() )
					LockFreeYaw( true );
			}
		}
	}
	if( nReceiverState & IR_MOUSE_WB_UP || nReceiverState & IR_MOUSE_WB_DOWN || nReceiverState & IR_MOUSE_MOVE )
	{
		if( m_hAttachActor )
		{
			if( CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing() )
				LockFreeYaw( true );
		}
	}

	if( nReceiverState & IR_KEY_DOWN ) {
#ifndef _FINAL_BUILD
		bool bPushTemp = false;
		/*if( IsPushKey( DIK_F1 ) ) {
			s_fCenterPushWidth -= 1.f;
			ReleasePushKey( DIK_F1 );
			bPushTemp = true;
		}
		if( IsPushKey( DIK_F2 ) ) {
			s_fCenterPushWidth += 1.f;
			ReleasePushKey( DIK_F2 );
			bPushTemp = true;
		}
		if( IsPushKey( DIK_F3 ) ) {
			s_fCenterPushHeight -= 1.f;
			ReleasePushKey( DIK_F3 );
			bPushTemp = true;
		}
		if( IsPushKey( DIK_F4 ) ) {
			s_fCenterPushHeight += 1.f;
			ReleasePushKey( DIK_F4 );
			bPushTemp = true;
		}
		if( bPushTemp ) {
			WCHAR wszTemp[256];
			wsprintf( wszTemp, L"카메라 옵셋 Width : %d, Height : %d", (int)s_fCenterPushWidth, (int)s_fCenterPushHeight );
			CDnInterface::GetInstance().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
		}*/
#endif
	}
}
void CDnPlayerCamera::LockFreeYaw( bool bLock ) 
{ 
	if( m_bLockFreeYaw == bLock ) return;
	m_bLockFreeYaw = bLock; 

	if( !m_bLockFreeYaw ) {
		RefreshPlayerView( 0.f, false );
	}
}

void CDnPlayerCamera::SyncActorView()
{
	if( !m_hAttachActor )
		return;

	MatrixEx Cross = *m_hAttachActor->GetMatEx();
	MatrixEx *pCross = m_hAttachActor->GetMatEx();
	EtVector3 vTemp2 = Cross.m_vPosition + ( m_matExWorld.m_vXAxis * s_fCenterPushWidth );

	EtVector3 vTemp = m_matExWorld.m_vZAxis;
	vTemp.y = 0.f;
	EtVec3Normalize( &vTemp, &vTemp );

	EtVector3 vPos[2];
	vTemp2 += vTemp * 180.f;
	vPos[1] = vTemp2;
	vTemp2 -= Cross.m_vPosition;
	EtVec3Normalize( &vTemp2, &vTemp2 );
	Cross.m_vZAxis = vTemp2;
	Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
	EtVec3Cross( &Cross.m_vXAxis, &Cross.m_vYAxis, &Cross.m_vZAxis );
	EtVec3Normalize( &Cross.m_vXAxis, &Cross.m_vXAxis );

	*pCross = Cross;
}

void CDnPlayerCamera::RefreshPlayerView( float fDelta, bool bForce )
{
	if( !m_hAttachActor )
		return;

	MatrixEx Cross = *m_hAttachActor->GetMatEx();
	MatrixEx *pCross = m_hAttachActor->GetMatEx();
	EtVector3 vTemp2 = Cross.m_vPosition + ( m_matExWorld.m_vXAxis * s_fCenterPushWidth );

	EtVector3 vTemp = m_matExWorld.m_vZAxis;
	vTemp.y = 0.f;
	EtVec3Normalize( &vTemp, &vTemp );

	EtVector3 vPos[2];
	vTemp2 += vTemp * 180.f;
	vPos[1] = vTemp2;
	vTemp2 -= Cross.m_vPosition;
	EtVec3Normalize( &vTemp2, &vTemp2 );
	Cross.m_vZAxis = vTemp2;
	Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
	EtVec3Cross( &Cross.m_vXAxis, &Cross.m_vYAxis, &Cross.m_vZAxis );
	EtVec3Normalize( &Cross.m_vXAxis, &Cross.m_vXAxis );

	float fDot = EtVec3Dot( &Cross.m_vZAxis, &pCross->m_vZAxis );
	if( fDot >= 1.f ) fDot = 1.f;
	float fAngle = EtToDegree( EtAcos( fDot ) );
	float fAngleSpeed = m_hAttachActor->GetRotateAngleSpeed() * fDelta;

	if( fAngle > fAngleSpeed ) 
	{
		EtVector3 vCrossVec;
		D3DXVec3Cross( &vCrossVec, &Cross.m_vZAxis, &pCross->m_vZAxis );
		if( vCrossVec.y > 0.f )
			pCross->RotateYaw( fAngleSpeed );
		else pCross->RotateYaw( -fAngleSpeed );

		fDot = EtVec3Dot( &Cross.m_vZAxis, &pCross->m_vZAxis );
		if( fDot >= 1.f ) fDot = 1.f;
		fAngle = EtToDegree( EtAcos( fDot ) );
		EtVector3 vCrossVec2;
		D3DXVec3Cross( &vCrossVec2, &Cross.m_vZAxis, &pCross->m_vZAxis );

		if( fAngle <= fAngleSpeed || ( vCrossVec2.y * vCrossVec.y )  < 0.f ) {
			*pCross = Cross;
		}
	}
	else 
	{
		*pCross = Cross;
	}
}

void CDnPlayerCamera::FollowCam( bool bFollow ) 
{ 
	if( m_bFollowCam == bFollow ) return;
	m_bFollowCam = bFollow; 
	if( !m_bFollowCam ) {
		RefreshPlayerView( 0.f, false );
	}
}


void CDnPlayerCamera::SetZoomDistance(float fMin,float fMax)
{
	m_fMinZoom = fMin; 
	m_fMaxZoom = fMax;
	m_fDistanceBeforeSmooth = fMax;
	//m_fDistance = fMax; 
}

void CDnPlayerCamera::ResetZoomDistance()
{
	m_fMinZoom = CAMERA_ZOOM_MIN;
	m_fMaxZoom = CAMERA_ZOOM_MAX;
	m_fDistanceBeforeSmooth = CAMERA_ZOOM_MAX;
}
