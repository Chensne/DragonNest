#include "StdAfx.h"
#include "RenderBase.h"
#include "InputDevice.h"
#include "ObjectBase.h"
#include "EtResourceMng.h"
#include <direct.h>

CRenderBase::CRenderBase()
 : CInputReceiver( true )
{
	m_CameraCross.m_vPosition = EtVector3( 420.f, 600.f, -680.f );
	m_CameraCross.m_vXAxis = EtVector3( 0.848041f, 0.f, 0.529931f );
	m_CameraCross.m_vYAxis = EtVector3( -0.318914f, 0.798635f, 0.510368f );
	m_CameraCross.m_vZAxis = EtVector3( -0.423226f, -0.601815f, 0.677272f );
	m_CameraCross.MakeUpCartesianByZAxis();

	m_bActivate = false;
	m_nMouseFlag = 0;
	m_CameraAt = EtVector3( 0.f, 0.f, 0.f );
	m_bShowGrid = true;
	m_bShowAxis = true;
	m_dwBgColor = D3DCOLOR_ARGB( 255, 100, 100, 100 );

	// Make Axis
	m_vAxis[0] = D3DXVECTOR3( -1, 0, 0 );
	m_vAxis[1] = D3DXVECTOR3( 1, 0, 0 );

	m_vAxis[2] = D3DXVECTOR3( 0, -1, 0 );
	m_vAxis[3] = D3DXVECTOR3( 0, 1, 0 );

	m_vAxis[4] = D3DXVECTOR3( 0, 0, -1 );
	m_vAxis[5] = D3DXVECTOR3( 0, 0, 1 );

	// Create X
	m_vAlphabetX[0] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );
	m_vAlphabetX[1] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );
	m_vAlphabetX[2] = D3DXVECTOR3( 0.1f, -0.1f, 0.f );
	m_vAlphabetX[3] = D3DXVECTOR3( -0.1f, 0.1f, 0.f );

	// Create Y
	m_vAlphabetY[0] = D3DXVECTOR3( -0.1f, 0.1f, 0.f );
	m_vAlphabetY[1] = D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_vAlphabetY[2] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );
	m_vAlphabetY[3] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );

	// Create Z
	m_vAlphabetZ[0] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );
	m_vAlphabetZ[1] = D3DXVECTOR3( 0.1f, -0.1f, 0.f );
	m_vAlphabetZ[2] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );
	m_vAlphabetZ[3] = D3DXVECTOR3( -0.1f, -0.1f, 0.f );
	m_vAlphabetZ[4] = D3DXVECTOR3( -0.1f, 0.1f, 0.f );
	m_vAlphabetZ[5] = D3DXVECTOR3( 0.1f, 0.1f, 0.f );

	// Make Direction
	m_vDirection[0] = D3DXVECTOR3( 0, 0, -10 );
	m_vDirection[1] = D3DXVECTOR3( 0, 0, 10 );
	m_vDirection[2] = D3DXVECTOR3( 2, -2, -10 );
	m_vDirection[3] = D3DXVECTOR3( 2, 2, -10 );
	m_vDirection[4] = D3DXVECTOR3( -2, -2, -10 );
	m_vDirection[5] = D3DXVECTOR3( -2, 2, -10 );
	m_vDirection[6] = D3DXVECTOR3( 2, -2, -10 );
	m_vDirection[7] = D3DXVECTOR3( -2, -2, -10 );
	m_vDirection[8] = D3DXVECTOR3( 2, 2, -10 );
	m_vDirection[9] = D3DXVECTOR3( -2, 2, -10 );
	m_vDirection[10] = D3DXVECTOR3( 2, 2, -10 );
	m_vDirection[11] = D3DXVECTOR3( 0, 0, -15 );
	m_vDirection[12] = D3DXVECTOR3( 2, -2, -10 );
	m_vDirection[13] = D3DXVECTOR3( 0, 0, -15 );
	m_vDirection[14] = D3DXVECTOR3( -2, 2, -10 );
	m_vDirection[15] = D3DXVECTOR3( 0, 0, -15 );
	m_vDirection[16] = D3DXVECTOR3( -2, -2, -10 );
	m_vDirection[17] = D3DXVECTOR3( 0, 0, -15 );

	// Make Point
	m_vPoint[0] = D3DXVECTOR3( -10, 0, 0 );
	m_vPoint[1] = D3DXVECTOR3( 10, 0, 0 );
	m_vPoint[2] = D3DXVECTOR3( 0, -10, 0 );
	m_vPoint[3] = D3DXVECTOR3( 0, 10, 0 );
	m_vPoint[4] = D3DXVECTOR3( 0, 0, -10 );
	m_vPoint[5] = D3DXVECTOR3( 0, 0, 10 );
	m_vPoint[6] = D3DXVECTOR3( 0, -10, 0 );
	m_vPoint[7] = D3DXVECTOR3( 0, 0, -10 );
	m_vPoint[8] = D3DXVECTOR3( 0, 10, 0 );
	m_vPoint[9] = D3DXVECTOR3( 0, 0, 10 );
	m_vPoint[10] = D3DXVECTOR3( 0, -10, 0 );
	m_vPoint[11] = D3DXVECTOR3( 0, 0, 10 );
	m_vPoint[12] = D3DXVECTOR3( 0, 10, 0 );
	m_vPoint[13] = D3DXVECTOR3( 0, 0, -10 );
	m_vPoint[14] = D3DXVECTOR3( -10, 0, 0 );
	m_vPoint[15] = D3DXVECTOR3( 0, 10, 0 );
	m_vPoint[16] = D3DXVECTOR3( 10, 0, 0 );
	m_vPoint[17] = D3DXVECTOR3( 0, 10, 0 );
	m_vPoint[18] = D3DXVECTOR3( -10, 0, 0 );
	m_vPoint[19] = D3DXVECTOR3( 0, -10, 0 );
	m_vPoint[20] = D3DXVECTOR3( 10, 0, 0 );
	m_vPoint[21] = D3DXVECTOR3( 0, -10, 0 );
	m_vPoint[22] = D3DXVECTOR3( 10, 0, 0 );
	m_vPoint[23] = D3DXVECTOR3( 0, 0, 10 );
	m_vPoint[24] = D3DXVECTOR3( 0, 0, 10 );
	m_vPoint[25] = D3DXVECTOR3( -10, 0, 0 );
	m_vPoint[26] = D3DXVECTOR3( -10, 0, 0 );
	m_vPoint[27] = D3DXVECTOR3( 0, 0, -10 );
	m_vPoint[28] = D3DXVECTOR3( 0, 0, -10 );
	m_vPoint[29] = D3DXVECTOR3( 10, 0, 0 );

	char szStr[2048] = { 0, };
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ShaderFolder", szStr, 2048 );
	if( strlen(szStr) == 0 )
		m_szShaderFolder = "\\\\eye-ad\\ToolData\\SharedEffect";
	else m_szShaderFolder = szStr;

	memset( szStr, 0, sizeof(szStr) );
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "EnvironmentTexture", szStr, 2048 );
	m_szEnviTextureFile = szStr;

	m_PrevLocalTime = m_LocalTime = 0;
	m_bLessPriority = false;
}

CRenderBase::~CRenderBase()
{
	Finalize();
}


void CRenderBase::Reconstruction()
{
	if( m_bActivate == false ) return;

	CRect rcRect;
	if( CGlobalValue::GetInstance().GetView() == NULL ) return;
	CGlobalValue::GetInstance().GetView()->GetClientRect( &rcRect );
	if( rcRect.Width() == 0 || rcRect.Height() == 0 ) return;
	EternityEngine::ReinitializeEngine( rcRect.Width(), rcRect.Height() );
}

void CRenderBase::Finalize()
{
	if( m_bActivate == false ) return;
	SAFE_RELEASE_SPTR( m_CameraHandle );
	SAFE_RELEASE_SPTR( m_hEnviTexture );

	m_FrameSync.End();

	CInputDevice::GetInstance().Finalize();

	EternityEngine::FinalizeEngine();
}

void CRenderBase::Initialize( HWND hWnd )
{
	CRect rcRect;
	m_hWnd = hWnd;
	GetClientRect( m_hWnd, &rcRect );

	EternityEngine::InitializeEngine( m_hWnd, rcRect.Width(), rcRect.Height(), true, true );
	GetEtStateManager()->SetEnable( false );
//	CEtResource::SetWaitDeleteMode( false );

	//AddResourceDir( m_szShaderFolder );
	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder );

	SCameraInfo CamInfo;
	CamInfo.fFar = 1000000.f;
	CamInfo.fFogFar = CamInfo.fFogNear = 1000000.f;
	m_CameraHandle = EternityEngine::CreateCamera( &CamInfo );

	m_FrameSync.Begin( -1 );

	SetEnvironmentTexture( m_szEnviTextureFile );

	// 기본값 설정.
	EternityEngine::SetCaptureScreenClearColor( D3DCOLOR_ARGB( 255, 100, 100, 100 ) );

	m_bActivate = true;
}

void CRenderBase::Resize()
{
	if( m_bActivate == false ) return;
}

bool CRenderBase::Render()
{
	if( m_bActivate == false ) return false;

	m_LocalTime = m_FrameSync.GetMSTime();
	CInputDevice::GetInstance().Process( m_FrameSync.GetMSTime(), ( m_LocalTime - m_PrevLocalTime ) * 0.001f );

	bool bResult = m_FrameSync.CheckSync();

	if( bResult == true ) {
		CString szFrame;
		szFrame.Format( "FPS : %.2f", m_FrameSync.GetFps() );
		EternityEngine::DrawText2D( EtVector2( 0.f, 0.f ), szFrame );

		if( m_bShowGrid ) DrawGrid();
		if( m_bShowAxis ) DrawAxis();
		for( DWORD i=0; i<m_VecLightInfo.size(); i++ ) {
			DrawLight( &m_VecLightInfo[i] );
		}
		m_VecLightInfo.clear();

		CObjectBase *pBase = CGlobalValue::GetInstance().GetRootObject();
		pBase->Process( m_LocalTime );
		if( m_CameraHandle ) {
			m_CameraHandle->Update( m_CameraCross );
		}

		EternityEngine::ClearScreen( m_dwBgColor );
		EternityEngine::RenderFrame( ( m_LocalTime - m_PrevLocalTime ) / 1000.f );
		EternityEngine::ShowFrame( NULL );
	}
	m_FrameSync.UpdateTime();
	m_PrevLocalTime = m_LocalTime;

	return bResult;
}


void CRenderBase::RenderReducedScale()
{
}

void CRenderBase::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	bool bForground = false;
	HWND hActiveWnd = GetActiveWindow();
	if( hActiveWnd == m_hWnd ) bForground = true;
	if( IsChild( hActiveWnd, m_hWnd ) ) bForground = true;
	if( !bForground ) return;

	CObjectBase *pBase = CGlobalValue::GetInstance().GetControlObject();
	if( pBase ) {
		if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
			pBase->OnInputReceive( nReceiverState, LocalTime );
			return;
		}
		else pBase->OnInputReceive( -1, LocalTime );
	}

	if( nReceiverState & IR_MOUSE_MOVE ) {
		if( m_nMouseFlag & WB_DOWN ) {

			EtVector3 vMove;
			vMove = -m_CameraCross.m_vXAxis * GetMouseVariation().x;
			vMove += m_CameraCross.m_vYAxis * GetMouseVariation().y;
			if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) {
				vMove *= 0.1f;
			}
			m_CameraCross.m_vPosition += vMove;
			m_CameraAt += vMove;
		}
		if( m_nMouseFlag & LB_DOWN ) {

			MatrixEx TempCross;
			TempCross = m_CameraCross;
			TempCross.RotateYaw(180);
			TempCross.m_vPosition = m_CameraAt;
			float fLength = EtVec3Length( &( m_CameraCross.m_vPosition - m_CameraAt ) );
			
			float fRotateX = GetMouseVariation().x / 2.f;
			float fRotateY = -GetMouseVariation().y / 2.f;
			if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) {
				fRotateX *= 0.1f;
				fRotateY *= 0.1f;
			}

			TempCross.RotateYAxis( fRotateX );
			TempCross.RotatePitch( fRotateY );

			m_CameraCross = TempCross;
			m_CameraCross.RotateYaw(180);
			m_CameraCross.m_vPosition = m_CameraAt + ( m_CameraCross.m_vZAxis * -fLength );

		}
	}
	if( nReceiverState & IR_MOUSE_WHEEL ) {
		if( IsInMouseRect( CGlobalValue::GetInstance().GetView() ) ) {
			EtVector3 vZoom = m_CameraCross.m_vZAxis * ( GetMouseVariation().z );
			if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) {
				vZoom *= 0.1f;
			}
			m_CameraCross.m_vPosition += vZoom;
		}
	}
	if( nReceiverState & IR_MOUSE_LB_DOWN ) {
		if( IsInMouseRect( CGlobalValue::GetInstance().GetView() ) )
			m_nMouseFlag |= LB_DOWN;
	}
	if( nReceiverState & IR_MOUSE_LB_UP ) {
		m_nMouseFlag &= ~LB_DOWN;
	}
	if( nReceiverState & IR_MOUSE_RB_DOWN ) {
		if( IsInMouseRect( CGlobalValue::GetInstance().GetView() ) )
			m_nMouseFlag |= RB_DOWN;
	}
	if( nReceiverState & IR_MOUSE_RB_UP ) {
		m_nMouseFlag &= ~RB_DOWN;
	}
	if( nReceiverState & IR_MOUSE_WB_DOWN ) {
		if( IsInMouseRect( CGlobalValue::GetInstance().GetView() ) )
			m_nMouseFlag |= WB_DOWN;
	}
	if( nReceiverState & IR_MOUSE_WB_UP ) {
		m_nMouseFlag &= ~WB_DOWN;
	}
}

bool CRenderBase::IsInMouseRect( CWnd *pWnd )
{
	CPoint p;
	CRect rcRect;
	GetCursorPos( &p );

	pWnd->GetWindowRect( &rcRect );
	if( p.x < rcRect.left || p.x > rcRect.right || p.y < rcRect.top || p.y > rcRect.bottom ) return false;
	return true;
}

void CRenderBase::DrawGrid()
{
	float fSize  = 10.0f;
	int nStep = 5;

	const int iGridRange = 50;
	const float fTemp = (float)iGridRange * fSize;

	for( int iCnt = -iGridRange; iCnt <= iGridRange; ++iCnt )
	{
		EtVector3 vecPos1( (float)iCnt * fSize, 0, -fTemp );
		EtVector3 vecPos2( (float)iCnt * fSize, 0,  fTemp );

		EtVector3 vecPos3( -fTemp, 0, (float)iCnt * fSize );
		EtVector3 vecPos4(  fTemp, 0, (float)iCnt * fSize );

		D3DCOLOR dcColor = (abs(iCnt) % nStep == 0) ? D3DCOLOR_ARGB(200,200,200,200) : D3DCOLOR_ARGB(150,63,63,63);
		if( iCnt == 0 ) dcColor = D3DCOLOR_ARGB( 255, 255, 255, 0 );

		EternityEngine::DrawLine3D( vecPos1, vecPos2, dcColor );
		EternityEngine::DrawLine3D( vecPos3, vecPos4, dcColor );
	}

}

void CRenderBase::UpdateLightGrid( SLightInfo *pInfo )
{
	m_VecLightInfo.push_back( *pInfo );
}

void CRenderBase::DrawLight( SLightInfo *pInfo )
{
	float fDistance = EtVec3Length( &( m_CameraCross.m_vPosition - m_CameraAt ) );
	float fScale = fDistance / 100.f;

	switch( pInfo->Type ) {
		case LT_DIRECTIONAL:
			{
				EtVector3 vDirection[18];
				MatrixEx Cross;
				Cross.m_vZAxis = -pInfo->Direction;
				Cross.MakeUpCartesianByZAxis();

				for( int i=0; i<18; i++ ) {
					vDirection[i] = m_vDirection[i] * fScale;
					EtVec3TransformCoord( &vDirection[i], &vDirection[i], Cross );
					if( i % 2 == 1 ) {
						EternityEngine::DrawLine3D( vDirection[i-1], vDirection[i], 0xFFFFFF00 );
					}
				}
			}
			break;
		case LT_POINT:
			{
				EtVector3 vPoint[30];
				for( int i=0; i<30; i++ ) {
					vPoint[i] = m_vPoint[i] * ( fScale * 0.2f );
					vPoint[i] += pInfo->Position;
					if( i % 2 == 1 ) {
						EternityEngine::DrawLine3D( vPoint[i-1], vPoint[i], 0xFFFFFF00 );
					}
				}

				EtVector3 vVec[2];
				MatrixEx Cross;
				int nDist = 40;
				float fAngle = 360 / (float)(nDist-1.f);

				Cross.m_vPosition = pInfo->Position;

				vVec[0] = Cross.m_vPosition + ( Cross.m_vZAxis * pInfo->fRange );
				for( int i=0; i<nDist; i++ ) {
					Cross.RotateYaw( fAngle );
					vVec[1] = Cross.m_vPosition + ( Cross.m_vZAxis * pInfo->fRange );
					EternityEngine::DrawLine3D( vVec[0], vVec[1], 0xFFFFFF00 );
					vVec[0] = vVec[1];
				}

				Cross.Identity();
				Cross.m_vPosition = pInfo->Position;

				vVec[0] = Cross.m_vPosition + ( Cross.m_vZAxis * pInfo->fRange );
				for( int i=0; i<nDist; i++ ) {
					Cross.RotatePitch( fAngle );
					vVec[1] = Cross.m_vPosition + ( Cross.m_vZAxis * pInfo->fRange );
					EternityEngine::DrawLine3D( vVec[0], vVec[1], 0xFFFFFF00 );
					vVec[0] = vVec[1];
				}

				Cross.Identity();
				Cross.m_vPosition = pInfo->Position;

				vVec[0] = Cross.m_vPosition + ( Cross.m_vXAxis * pInfo->fRange );
				for( int i=0; i<nDist; i++ ) {
					Cross.RotateRoll( fAngle );
					vVec[1] = Cross.m_vPosition + ( Cross.m_vXAxis * pInfo->fRange );
					EternityEngine::DrawLine3D( vVec[0], vVec[1], 0xFFFFFF00 );
					vVec[0] = vVec[1];
				}
			}
			break;
		case LT_SPOT:
			{
				EtVector3 vVec[2], vVec2[2];
				MatrixEx Cross;
				float fTheta = EtToDegree( acos( pInfo->fTheta ) );
				float fPhi = EtToDegree( acos( pInfo->fPhi ) );

				// 외각
				DrawSpotFunc1( fPhi, pInfo, false, 0xFF999900 );
				DrawSpotFunc1( -fPhi, pInfo, false, 0xFF999900 );
				DrawSpotFunc1( fPhi, pInfo, true, 0xFF999900 );
				DrawSpotFunc1( -fPhi, pInfo, true, 0xFF999900 );

				// 내각
				DrawSpotFunc1( fTheta, pInfo, false, 0xFFFFFF00 );
				DrawSpotFunc1( -fTheta, pInfo, false, 0xFFFFFF00 );
				DrawSpotFunc1( fTheta, pInfo, true, 0xFFFFFF00 );
				DrawSpotFunc1( -fTheta, pInfo, true, 0xFFFFFF00 );

				// 외각 내각 원
				int nDist = 30;
				float fAngle = 360 / (float)(nDist-1.f);


				Cross.m_vPosition = pInfo->Position + ( pInfo->Direction * pInfo->fRange );
				Cross.m_vZAxis = -pInfo->Direction;
				Cross.MakeUpCartesianByZAxis();

				float fRangePhi = pInfo->fRange * tan( EtToRadian( fPhi ) );
				float fRangeTheta = pInfo->fRange * tan( EtToRadian( fTheta ) );

				vVec[0] = Cross.m_vPosition + ( Cross.m_vXAxis * fRangePhi );
				vVec2[0] = Cross.m_vPosition + ( Cross.m_vXAxis * fRangeTheta );
				for( int i=0; i<nDist; i++ ) {
					Cross.RotateRoll( fAngle );
					vVec[1] = Cross.m_vPosition + ( Cross.m_vXAxis * fRangePhi );
					vVec2[1] = Cross.m_vPosition + ( Cross.m_vXAxis * fRangeTheta );

					EternityEngine::DrawLine3D( vVec[0], vVec[1], 0xFF999900 );
					EternityEngine::DrawLine3D( vVec2[0], vVec2[1], 0xFFFFFF00 );

					vVec[0] = vVec[1];
					vVec2[0] = vVec2[1];
				}

			}
			break;
	}
}

void CRenderBase::DrawSpotFunc1( float fAngle, SLightInfo *pInfo, bool bYawPitch, DWORD dwColor )
{
	EtVector3 vVec[2];
	MatrixEx Cross;
	Cross.m_vZAxis = pInfo->Direction;
	Cross.MakeUpCartesianByZAxis();
	if( bYawPitch == false ) Cross.RotateYaw( fAngle );
	else Cross.RotatePitch( fAngle );

	float fRange = pInfo->fRange * tan( EtToRadian( abs(fAngle) ) );

	vVec[0] = pInfo->Position;
	vVec[1] = pInfo->Position + ( Cross.m_vZAxis * sqrtf( (pInfo->fRange*pInfo->fRange) + (fRange*fRange) ) );

	EternityEngine::DrawLine3D( vVec[0], vVec[1], dwColor );
}

void CRenderBase::DrawAxis()
{
	float fDistance = EtVec3Length( &( m_CameraCross.m_vPosition - m_CameraAt ) );
	DWORD dwColor[3] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
	float fScale = fDistance / 25.f;

	// Draw Axis
	EtVector3 vAxis[6];
	for( int i=0; i<6; i++ ) {
		vAxis[i] = m_vAxis[i] * fScale;
		vAxis[i] += m_CameraAt;
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAxis[i-1], vAxis[i], dwColor[i/2] );
		}
	}

	// Draw Alphabet
	EtMatrix matScale, mat;
	EtMatrix matTemp = *m_CameraHandle->GetViewMat();

	EtMatrixInverse( &matTemp, NULL, &matTemp );
	D3DXMatrixScaling( &matScale, fScale, fScale, fScale );

	// Draw Alphabet X
	mat = matTemp;
	mat._41 = m_CameraAt.x + ( 1.2f * fScale );
	mat._42 = m_CameraAt.y + ( 0.2f * fScale );
	mat._43 = m_CameraAt.z;
	EtMatrixMultiply( &mat, &matScale, &mat );

	EtVector3 vAlphabetX[4];
	for( int i=0; i<4; i++ ) {
		EtVec3TransformCoord( &vAlphabetX[i], &m_vAlphabetX[i], &mat );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAlphabetX[i-1], vAlphabetX[i], dwColor[0] );
		}
	}

	// Draw Alphabet Y
	mat = matTemp;
	mat._41 = m_CameraAt.x + ( 0.2f * fScale );
	mat._42 = m_CameraAt.y + ( 1.2f * fScale );
	mat._43 = m_CameraAt.z;
	EtMatrixMultiply( &mat, &matScale, &mat );

	EtVector3 vAlphabetY[4];
	for( int i=0; i<4; i++ ) {
		EtVec3TransformCoord( &vAlphabetY[i], &m_vAlphabetY[i], &mat );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAlphabetY[i-1], vAlphabetY[i], dwColor[1] );
		}
	}

	// Draw Alphabet Z
	mat = matTemp;
	mat._41 = m_CameraAt.x + ( 0.2f * fScale );
	mat._42 = m_CameraAt.y;
	mat._43 = m_CameraAt.z + ( 1.2f * fScale );
	EtMatrixMultiply( &mat, &matScale, &mat );

	EtVector3 vAlphabetZ[6];
	for( int i=0; i<6; i++ ) {
		EtVec3TransformCoord( &vAlphabetZ[i], &m_vAlphabetZ[i], &mat );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAlphabetZ[i-1], vAlphabetZ[i], dwColor[2] );
		}
	}

}


void CRenderBase::ChangeShaderFolder( const char *szFullPath )
{
	//RemoveResourceDir( m_szShaderFolder );
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szShaderFolder );
	m_szShaderFolder = szFullPath;
	//AddResourceDir( m_szShaderFolder );
	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder );
}


void CRenderBase::SetEnvironmentTexture( const char *szFileName )
{
	SAFE_RELEASE_SPTR( m_hEnviTexture );
	if( szFileName == NULL ) m_szEnviTextureFile.Empty();
	m_szEnviTextureFile = szFileName;
	if( szFileName != NULL && strlen(szFileName) > 0 ) {
		m_hEnviTexture = EternityEngine::LoadEnvTexture( m_szEnviTextureFile.GetBuffer() );
	}

	SetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "EnvironmentTexture", m_szEnviTextureFile.GetBuffer() );
}


void CRenderBase::ChangePriority( bool bLessPriority )
{
	if( m_bLessPriority == bLessPriority ) return;
	m_bLessPriority = bLessPriority;
	m_FrameSync.End();
	m_FrameSync.CheckSecureFrameLoad(1);
	if( m_bLessPriority ) {
		m_FrameSync.Begin( 60 );
	}
	else {
		m_FrameSync.Begin( -1 );
	}
}