#include "StdAfx.h"
#include "DnCannonCamera.h"
#include "GameOption.h"
#include "DnMouseCursor.h"
#include "DnLocalPlayerActor.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

float CDnCannonCamera::s_fLastZDistance = CAMERA_ZOOM_MAX;

CDnCannonCamera::CDnCannonCamera( void ) : CInputReceiver( true ),
										   m_fPitchInputValue( 0.0f ),
										   m_fYawInputValue( 0.0f ),
										   m_fNowPitchValue( 0.0f ),
										   m_fNowYawValue( 0.0f ),
										   m_fNowZDistanceValue( 0.0f ),
										   m_fSmoothAnglePower( 20.0f ),
										   m_fSmoothZoomPower( 20.0f ),
										   m_bFirstProcess( true ),
										   m_fTargetDistance( 0.0f ),
										   m_fCurrentDistance( 0.0f ),
										   m_fMinZoom( CAMERA_ZOOM_MIN ),
										   m_fMaxZoom( CAMERA_ZOOM_MAX ),
										   m_fPlayerActorZDistance( 0.0f )
{
	m_CameraType = CameraTypeEnum::CannonCamera;

	m_CameraInfo.fFogNear = 100000.f;
	m_CameraInfo.fFogFar = 100000.f;
	m_CameraInfo.fFar = 100000.f;

	m_fZDistanceBeforeSmooth = s_fLastZDistance = CAMERA_ZOOM_MAX;
	m_fZDistanceAccel = 0.f;
}

CDnCannonCamera::~CDnCannonCamera( void )
{
	s_fLastZDistance = m_fNowZDistanceValue;
}

bool CDnCannonCamera::Activate( void )
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
	m_fZDistanceBeforeSmooth = s_fLastZDistance = CAMERA_ZOOM_MAX;
	m_fZDistanceAccel = 0.f;

	m_bFirstProcess = true;
	return ( m_hCamera ) ? true : false;
}

bool CDnCannonCamera::DeActivate( void )
{
	SAFE_RELEASE_SPTR( m_hCamera );
	return true;
}


void CDnCannonCamera::ResetCamera( void )
{
	if( !m_hAttachActor || !m_hAttachCannonMonster || !m_hCamera )
		return;

	CDnCamera::ResetCamera();

	MatrixEx* pCannonMonsterCross = m_hAttachCannonMonster->GetMatEx();
	m_matExWorld = *pCannonMonsterCross;
	m_fPitchInputValue = 0.0f;
	m_fNowPitchValue = 0.0f;
	m_fNowZDistanceValue = 300.0f;
	m_matExWorld.RotatePitch( m_fPitchInputValue );
	m_matExWorld.MoveLocalZAxis( -m_fNowZDistanceValue );
	m_matExWorld.MoveLocalYAxis( 200.0f );
	//m_matExWorld.RotateYaw( 1.0f );

	m_fYawInputValue = EtToDegree( atan2f(m_matExWorld.m_vZAxis.x, m_matExWorld.m_vZAxis.z) );
	m_fNowYawValue = m_fYawInputValue;

	// 외부에서 셋팅해줄 때 대포 뒤에 적절한 위치에 이미 셋팅되어있다고 가정하고 있으므로
	// 적절한 위치를 외부에서 셋팅하고 호출해줘야 함.
	_ASSERT( m_hAttachCannonMonster && m_hAttachActor && "대포 몬스터, 대포 쏘는 플레이어 액터 모두 셋팅된 상태여야 한다." );
	m_fPlayerActorZDistance = EtVec3Length( &EtVector3(m_hAttachActor->GetMatEx()->m_vPosition - m_hAttachCannonMonster->GetMatEx()->m_vPosition) );
}

void CDnCannonCamera::AttachActor( DnActorHandle hActor )
{
	m_hAttachActor = hActor;
}

void CDnCannonCamera::AttachCannonMonster( DnActorHandle hActor )
{
	if( m_hAttachCannonMonster )
		DetachCannonMonster();

	m_hAttachCannonMonster = hActor;

	ResetCamera();
	m_fNowYawValue = 0.f;
	m_fYawInputValue = 0.f;
}


void CDnCannonCamera::RotateActorToCamZDirection( void )
{
	// 대포 및 대포를 들고 있는 캐릭터의 방향을 카메라의 Z 방향으로 돌려줌.
	MatrixEx* pCannonCross = m_hAttachCannonMonster->GetMatEx();
	pCannonCross->m_vZAxis = m_matExWorld.m_vZAxis;
	pCannonCross->m_vZAxis.y = 0.0f;
	EtVec3Normalize( &pCannonCross->m_vZAxis, &pCannonCross->m_vZAxis );
	pCannonCross->MakeUpCartesianByZAxis();

	// 캐릭터는 대포를 중심으로 회전됨.
	MatrixEx* pActorCross = m_hAttachActor->GetMatEx();
	*pActorCross = *pCannonCross;
	pActorCross->MoveLocalZAxis( -m_fPlayerActorZDistance );


	//pActorCross->m_vZAxis = m_matExWorld.m_vZAxis;
	//pActorCross->m_vZAxis.y = 0.0f;
	//EtVec3Normalize( &pActorCross->m_vZAxis, &pActorCross->m_vZAxis );
	//pActorCross->MakeUpCartesianByZAxis();
}


void CDnCannonCamera::Process( LOCAL_TIME LocalTime, float fDelta )
{
	float fYawDelta = 0.0f;
	float fPitchDelta = 0.0f;
	if( m_hAttachCannonMonster && m_hAttachActor &&
		false == CDnMouseCursor::GetInstance().IsShowCursor() )
	{
		if( false == CDnLocalPlayerActor::IsLockInput() )
		{
			bool isInverseMode = false;
			isInverseMode = CInputDevice::IsInverseMouse();

			if( GetMouseVariation().x != 0.0f )
			{
				fYawDelta += ((isInverseMode == false) ? GetMouseVariation().x : -GetMouseVariation().x) * CDnMouseCursor::GetInstance().GetMouseSensitivity() * GetFreezeResistanceRatio();
			}

			if( GetMouseVariation().y != 0.0f )
			{
				fPitchDelta += ((isInverseMode == false) ? GetMouseVariation().y : -GetMouseVariation().y) * CDnMouseCursor::GetInstance().GetMouseSensitivity() * GetFreezeResistanceRatio();
			}

			if( GetMouseVariation().z != 0.0f )
			{
				m_fZDistanceAccel += GetMouseVariation().z / 14.0f;
			}

			if( 0.0f != fYawDelta || 0.0f != fPitchDelta )
			{
				if( m_hAttachActor )
				{
					static_cast<CDnLocalPlayerActor*>(m_hAttachActor.GetPointer())->CannonRotated();
					//OutputDebug( "-- 카메라 회전\n" );
				}
			}

			m_fYawInputValue += fYawDelta;
			m_fPitchInputValue += fPitchDelta;
			m_fPitchInputValue = EtClamp( m_fPitchInputValue, -45.0f, 45.0f );

			m_fNowZDistanceValue += (m_fZDistanceBeforeSmooth - m_fNowZDistanceValue) * EtMin( 1.0f, (fDelta*m_fSmoothZoomPower) );
			if( m_fZDistanceAccel != 0.0f )
			{
				m_fZDistanceBeforeSmooth -= m_fZDistanceAccel * 35.0f / 6.0f;
				m_fZDistanceBeforeSmooth = EtClamp( m_fZDistanceBeforeSmooth, m_fMinZoom, m_fMaxZoom );

				m_fZDistanceAccel = 0.0f;
			}

			// 대포로 지정된 액터를 중심으로 카메라 회전.
			m_matExWorld.m_vXAxis = m_hAttachCannonMonster->GetMatEx()->m_vXAxis; //EtVector3( 1.0f, 0.0f, 0.0f );
			m_matExWorld.m_vYAxis = m_hAttachCannonMonster->GetMatEx()->m_vYAxis; //EtVector3( 0.0f, 1.0f, 0.0f );
			m_matExWorld.m_vZAxis = m_hAttachCannonMonster->GetMatEx()->m_vZAxis; //EtVector3( 0.0f, 0.0f, 1.0f );

			// 좌우 마우스 이동 입력에 대해서는 대포를 중심으로 월드 Y 축 기준으로 카메라가 회전된다.

			m_fNowYawValue += (m_fYawInputValue - m_fNowYawValue) * EtMin( 1.0f, (fDelta*m_fSmoothAnglePower) );

			float ROTATE_MAX_GAP = 60.f;

			if(m_fNowYawValue >= ROTATE_MAX_GAP) 
			{
				m_fNowYawValue = ROTATE_MAX_GAP;
				m_fYawInputValue = ROTATE_MAX_GAP;
			}

			if(m_fNowYawValue <= -ROTATE_MAX_GAP) 
			{
				m_fNowYawValue = -ROTATE_MAX_GAP;
				m_fYawInputValue = -ROTATE_MAX_GAP;
			}

			m_matExWorld.RotateYAxis( m_fNowYawValue );

			// 상하 마우스 이동 입력에 대해서는 대포를 중심으로 로컬 X 축 기준으로 카메라가 회전됨.
			// 위에서 월드 Y 축 기준으로 돌리면 로컬 X 축과 월드 X 축이 달라지기 때문이다.
			m_fNowPitchValue += (m_fPitchInputValue - m_fNowPitchValue) * EtMin( 1.0f, (fDelta*m_fSmoothAnglePower) );
			m_matExWorld.RotatePitch( m_fNowPitchValue );
		}

		// 대포 및 대포를 잡고 있는 캐릭터가 카메라의 방향을 바라보도록 조정.


		/* //Rotha - 대포 몸체는 회전 안하므로 뺍니다.
		RotateActorToCamZDirection();
		*/

		// 락 인풋 상태에서도 줌은 되어야 함.
		static_cast<CDnLocalPlayerActor*>(m_hAttachActor.GetPointer())->OnCannonCameraProcessEnd();
	}
}

// 플레이어 카메라와 똑같이 해줘야 함. 우선 먼저 처리할 것들 처리하고.. 마지막에 해보자.
void CDnCannonCamera::PostProcess( LOCAL_TIME LocalTime, float fDelta )
{
	float fHeightDistance = 0.f;
	float fPropDistance = 0.f;
	EtVector3 vLookAt;
	bool bPickHeight = false;
	bool bPickWater = false;
	if( m_hAttachCannonMonster )
	{
		MatrixEx TempCross = m_matExWorld;
		TempCross.m_vPosition = m_hAttachCannonMonster->GetMatEx()->m_vPosition;
		TempCross.m_vPosition.y += 130.0f;		// TODO: 추후에 필요하다면 PlayerCamera 의 s_fCenterPushHeight 처럼 따로 뺀다.
		EtVector3 vLookAt = TempCross.m_vPosition;
		TempCross.MoveLocalZAxis( -m_fNowZDistanceValue );

		// Height 높이값 체크
		bool bResult = false;
		EtVector3 vPickPos;
		EtVector3 vLookAtTemp = vLookAt;
		vLookAtTemp.y -= 20.f;
		bResult = CDnWorld::GetInstance().Pick( vLookAtTemp, -TempCross.m_vZAxis, vPickPos );
		if( bResult ) 
		{
			vLookAtTemp.y += 20.f;
			vPickPos.y += 20.f;
			float fLength = EtVec3Length( &( vLookAtTemp - vPickPos ) ); 
			if( fLength <= CAMERA_ZOOM_MAX )
			{
				fHeightDistance = m_fNowZDistanceValue - fLength;
				bPickHeight = true;
			}
		}

		// 물 높이값 체크
		vLookAtTemp = vLookAt;
		vLookAtTemp.y -= 20.f;
		bResult = CDnWorld::GetInstance().PickWater( vLookAtTemp, -TempCross.m_vZAxis, vPickPos );
		if( bResult ) 
		{
			vLookAtTemp.y += 20.f;
			vPickPos.y += 20.f;
			float fLength = EtVec3Length( &( vLookAtTemp - vPickPos ) ); 
			if( fLength <= CAMERA_ZOOM_MAX ) 
			{
				if( m_fNowZDistanceValue - fLength > fHeightDistance ) 
				{
					fHeightDistance = m_fNowZDistanceValue - fLength;
					bPickWater = true;
				}
			}
		}

		m_matExWorld = TempCross;
	}

	ProcessEffect( LocalTime, fDelta );

	if( !m_bFirstProcess ) 
		m_fTargetDistance = max( fPropDistance, fHeightDistance );

	float fValue = 500.f * fDelta;
	float fRange = fabs( m_fTargetDistance - m_fCurrentDistance );
	if( fRange < 90.f ) 
	{
		float fTemp = sin( fRange / 180.f * D3DX_PI );
		fValue *= fTemp;
	}

	// Height쪽 당겨질때 갭이 크면 강제로 당겨준다.
	bool bCheckPostHeight = false;
	if( fHeightDistance != 0.f && m_fTargetDistance == fHeightDistance && m_fCurrentDistance < m_fTargetDistance )
	{
		if( bPickHeight || bPickWater ) 
		{
			m_fCurrentDistance = m_fTargetDistance;
		}
	}

	if( m_fCurrentDistance < m_fTargetDistance ) 
	{
		m_fCurrentDistance += fValue;
		if( m_fCurrentDistance >= m_fTargetDistance ) 
			m_fCurrentDistance = m_fTargetDistance;
	}

	if( m_fCurrentDistance > m_fTargetDistance ) 
	{
		m_fCurrentDistance -= fValue;
		if( m_fCurrentDistance <= m_fTargetDistance )
			m_fCurrentDistance = m_fTargetDistance;
	}

	if( m_fCurrentDistance > 0.f ) 
		m_matExWorld.MoveLocalZAxis( m_fCurrentDistance );

	m_hCamera->Update( m_matExWorld );

	if( m_fCurrentDistance > 0.f ) 
		m_matExWorld.MoveLocalZAxis( -m_fCurrentDistance );

	m_bFirstProcess = false;
}
//
//void CDnCannonCamera::ViewActorDir( void )
//{
//	if( !m_hAttachActor )
//		return;
//
//	m_Cross = *m_hAttachActor->GetMatEx();
//	m_matExWorld.MoveLocalZAxis( -500.0f );
//	m_matExWorld.MoveLocalYAxis( 100.0f );
//}