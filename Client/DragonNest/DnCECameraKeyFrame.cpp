#include "stdafx.h"

#include "TaskManager.h"
#include "DnGameTask.h"

#include "DnLocalPlayerActor.h"

#include "CameraData.h"
#include "DnCamera.h"
#include "DnCECameraKeyFrame.h"
#include "DnPlayerCamera.h"




CDnCECameraKeyFrame::CDnCECameraKeyFrame( DnCameraHandle hCamera, char * pStrCamFile, bool bDefaultEffect ) :
CDnCameraEffectBase( hCamera, bDefaultEffect),
m_pCamData(NULL),
m_pPlayerCam(NULL)
{	
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
	if( pTask )
	{
		std::string strCamFile( pStrCamFile );

		// 직전 CamData 와 같음.
		if( pTask->CompareCamFileName( strCamFile ) )
		{
			CCameraData * pCam = pTask->GetCamData();
			if( pCam )
				m_pCamData = pCam;
			else
				LoadCamFile( pTask, strCamFile );
		}

		// 다른 CamData - 로딩.
		else
			LoadCamFile( pTask, strCamFile );	
	}


	m_StartTime = hCamera->GetLocalTime();

	m_dwFrame = m_pCamData->m_Header.nFrame;
	m_bDestroy = false;

	EtQuaternionNormalize( &m_PreQuat, &m_PreQuat );	
	

	m_pPlayerCam = dynamic_cast< CDnPlayerCamera * >( hCamera.GetPointer() );
	
}

void CDnCECameraKeyFrame::LoadCamFile( CDnGameTask *pTask, std::string & strCamFile )
{
	// 이전 CameraData 제거.
	CCameraData * pCam = pTask->GetCamData();
	if( pCam )
	{
		pCam->Clear();
		delete pCam;
		pCam = NULL;
	}

	m_pCamData = new CCameraData;
	CFileNameString strFN = strCamFile.c_str();
	CResMngStream Stream( CEtResourceMng::GetInstance().GetFullName( strFN ).c_str() );
	m_pCamData->LoadResource( (CStream *)(&Stream) );

	// 새 CameraData 설정.
	m_strCamFile.assign( strCamFile );
	pTask->SetCamFileName( m_strCamFile );
	pTask->SetCamData( m_pCamData );

}


void CDnCECameraKeyFrame::Process( LOCAL_TIME LocalTime, float fDelta )
{	
	float fFrame = ( (float)( (LocalTime - m_StartTime) ) / 1000.0f ) * 60.0f;
	
	// 종료 - 다른 Signal 처럼 Signal 길이만큼 수행하고 종료하는 것이 아니라,
	//        카메라 Keyframe 끝까지 수행하고 종료한다.
	if( fFrame > m_dwFrame )
	{
		m_bDestroy = true;
		return;
	}

	EtVector3 vPos(0.0f,0.0f,0.0f);
	EtQuat qQuat(0.0f,0.0f,0.0f,0.0f);
	float fFOV = 0.0f;
	CalcPosition( vPos, fFrame );
	CalcRotation( qQuat, fFrame );
	CalcFov( fFOV, fFrame );

	MatrixEx * pMatCam = m_hCamera->GetMatEx();
	EtMatrix matCam = *pMatCam;	
	matCam._41 = matCam._42 = matCam._43 = 0.0f;

	EtMatrix matKeyFrame; // test.
	EtMatrixIdentity( &matKeyFrame );	
	EtMatrixRotationQuaternion( &matKeyFrame, &qQuat );	
	//EtMatrixTransformation( &matKeyFrame, NULL,NULL,NULL,NULL, &qQuat, &vPos );

	EtMatrixMultiply( &matKeyFrame, &matKeyFrame, &matCam );	
	matKeyFrame._41 += vPos.x;  matKeyFrame._42 += vPos.y;  matKeyFrame._43 += vPos.z;

	MatrixEx matEx( matKeyFrame );

	if( m_pPlayerCam )
		matEx.m_vPosition += pMatCam->GetPosition();
	
	*pMatCam = matEx;	

//	m_hCamera->SetFOV( fFOV );

}



void CDnCECameraKeyFrame::CalcPosition( EtVector3 &Position, float fFrame )
{
	std::vector<SCameraPositionKey> & VecPosition = m_pCamData->m_vecPosition;

	if( fFrame < 0.f ) fFrame = 0.f;
	if( VecPosition.empty() )
	{
		return;
	}
	if( VecPosition.size() == 1 )
	{
		Position = VecPosition[ 0 ].vPosition;
		return;
	}

	int nFrame;
	int nRangeStart, nRangeEnd, nRangeCur;
	float fWeight;

	nRangeStart = 0;
	nRangeEnd = ( int )VecPosition.size() - 1;
	nRangeCur = ( int )( fFrame / VecPosition[ nRangeEnd ].nTime * nRangeEnd );
	nFrame = ( int )fFrame;
	while( 1 )
	{
		// 익스포트 잘못된 데이타가 있어서 예외처리 한다. 익스포터쪽 손보고 고치자
		// by mapping
		if( nRangeCur >= ( int )VecPosition.size() - 1 )
		{
			Position = VecPosition[ VecPosition.size() - 1 ].vPosition;
			return;
		}

		if( nFrame == VecPosition[ nRangeCur ].nTime )
		{
			fWeight = ( fFrame - VecPosition[ nRangeCur ].nTime ) / ( VecPosition[ nRangeCur + 1 ].nTime - VecPosition[ nRangeCur ].nTime );
			EtVec3Lerp( &Position, &VecPosition[ nRangeCur ].vPosition, 
				&VecPosition[ nRangeCur + 1 ].vPosition, fWeight );
			return;
		}
		if( nFrame < VecPosition[ nRangeCur ].nTime )
		{
			nRangeEnd = nRangeCur - 1;
		}
		else
		{
			if( nFrame < VecPosition[ nRangeCur + 1 ].nTime )
			{
				fWeight = ( fFrame - VecPosition[ nRangeCur ].nTime ) / ( VecPosition[ nRangeCur + 1 ].nTime - VecPosition[ nRangeCur ].nTime );
				EtVec3Lerp( &Position, &VecPosition[ nRangeCur ].vPosition, 
					&VecPosition[ nRangeCur + 1 ].vPosition, fWeight );
				return;
			}
			nRangeStart = nRangeCur + 1;
		}
		nRangeCur = ( nRangeStart + nRangeEnd ) >> 1;
	}
}

void CDnCECameraKeyFrame::CalcRotation( EtQuat &Rotation, float fFrame )
{
	std::vector< SCameraRotationKey > & VecRotation = m_pCamData->m_vecRotation;

	if( fFrame < 0.f ) fFrame = 0.f;
	if( VecRotation.empty() )
	{
		return;
	}
	if( VecRotation.size() == 1 )
	{
		Rotation = VecRotation[ 0 ].qRotation;
		return;
	}

	int nFrame;
	int nRangeStart, nRangeEnd, nRangeCur;
	float fWeight;

	nRangeStart = 0;
	nRangeEnd = ( int )VecRotation.size() - 1;
	nRangeCur = ( int )( fFrame / VecRotation[ nRangeEnd ].nTime * nRangeEnd );
	nFrame = ( int )fFrame;
	while( 1 )
	{
		// 익스포트 잘못된 데이타가 있어서 예외처리 한다. 익스포터쪽 손보고 고치자
		// by mapping
		if( nRangeCur >= ( int )VecRotation.size() - 1 )
		{
			Rotation = VecRotation[ VecRotation.size() - 1 ].qRotation;
			return;
		}

		if( nFrame == VecRotation[ nRangeCur ].nTime )
		{
			fWeight = ( fFrame - VecRotation[ nRangeCur ].nTime ) / ( VecRotation[ nRangeCur + 1 ].nTime - VecRotation[ nRangeCur ].nTime );
			EtQuaternionSlerp( &Rotation, &VecRotation[ nRangeCur ].qRotation, 
				&VecRotation[ nRangeCur + 1 ].qRotation, fWeight );
			return;
		}
		if( nFrame < VecRotation[ nRangeCur ].nTime )
		{
			nRangeEnd = nRangeCur - 1;
		}
		else
		{
			if( nFrame < VecRotation[ nRangeCur + 1 ].nTime )
			{
				fWeight = ( fFrame - VecRotation[ nRangeCur ].nTime ) / ( VecRotation[ nRangeCur + 1 ].nTime - VecRotation[ nRangeCur ].nTime );
				EtQuaternionSlerp( &Rotation, &VecRotation[ nRangeCur ].qRotation, 
					&VecRotation[ nRangeCur + 1 ].qRotation, fWeight );
				return;
			}
			nRangeStart = nRangeCur + 1;
		}
		nRangeCur = ( nRangeStart + nRangeEnd ) >> 1;
	}
}

void CDnCECameraKeyFrame::CalcFov( float &fFov, float fFrame )
{
	std::vector< SCameraFOVKey > & VecFOV = m_pCamData->m_vecFOV;

	if( fFrame < 0.f ) fFrame = 0.f;
	if( VecFOV.empty() )
	{
		fFov = 1.f;
		return;
	}
	if( VecFOV.size() == 1 )
	{
		fFov = 1.f;
		return;
	}

	int nFrame;
	int nRangeStart, nRangeEnd, nRangeCur;
	float fWeight;

	nRangeStart = 0;
	nRangeEnd = ( int )VecFOV.size() - 1;
	nRangeCur = ( int )( fFrame / VecFOV[ nRangeEnd ].nTime * nRangeEnd );
	nFrame = ( int )fFrame;
	while( 1 )
	{
		// 익스포트 잘못된 데이타가 있어서 예외처리 한다. 익스포터쪽 손보고 고치자
		// by mapping
		if( nRangeCur >= ( int )VecFOV.size() - 1 )
		{
			fFov = VecFOV[ VecFOV.size() - 1 ].fFOV;
			return;
		}

		if( nFrame == VecFOV[ nRangeCur ].nTime )
		{
			fWeight = ( fFrame - VecFOV[ nRangeCur ].nTime ) / ( VecFOV[ nRangeCur + 1 ].nTime - VecFOV[ nRangeCur ].nTime );
			fFov = VecFOV[ nRangeCur ].fFOV + ( ( VecFOV[ nRangeCur + 1 ].fFOV - VecFOV[ nRangeCur ].fFOV ) * fWeight );
			return;
		}
		if( nFrame < VecFOV[ nRangeCur ].nTime )
		{
			nRangeEnd = nRangeCur - 1;
		}
		else
		{
			if( nFrame < VecFOV[ nRangeCur + 1 ].nTime )
			{
				fWeight = ( fFrame - VecFOV[ nRangeCur ].nTime ) / ( VecFOV[ nRangeCur + 1 ].nTime - VecFOV[ nRangeCur ].nTime );
				fFov = VecFOV[ nRangeCur ].fFOV + ( ( VecFOV[ nRangeCur + 1 ].fFOV - VecFOV[ nRangeCur ].fFOV ) * fWeight );
				return;
			}
			nRangeStart = nRangeCur + 1;
		}
		nRangeCur = ( nRangeStart + nRangeEnd ) >> 1;
	}
}