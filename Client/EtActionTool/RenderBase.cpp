#include "StdAfx.h"
#include "RenderBase.h"
#include "InputDevice.h"
#include "GlobalValue.h"
#include "MainFrm.h"

#include "EtActionToolDoc.h"
#include "EtActionToolView.h"

#include "ActionBase.h"
#include "SignalCustomRender.h"
#include "EtResourceMng.h"

#include "DnGameTable.h"
#include "DnGameWeaponMng.h"

CRenderBase::CRenderBase()
: CInputReceiver( true )
{
	m_bActivate = false;

	m_matExWorld.m_vPosition = EtVector3( 420.f, 600.f, -680.f );
	m_matExWorld.m_vXAxis = EtVector3( 0.848041f, 0.f, 0.529931f );
	m_matExWorld.m_vYAxis = EtVector3( -0.318914f, 0.798635f, 0.510368f );
	m_matExWorld.m_vZAxis = EtVector3( -0.423226f, -0.601815f, 0.677272f );
	m_matExWorld.MakeUpCartesianByZAxis();

	m_bActivate = false;
	m_nMouseFlag = 0;
	m_CameraAt = EtVector3( 0.f, 0.f, 0.f );
	m_bShowGrid = true;
	m_bShowAxis = true;
	m_dwBgColor = D3DCOLOR_ARGB( 255, 100, 100, 100 );
	m_bLessPriority = false;
	m_bHitSignalShow = false;

	m_PrevLocalTime = 0;

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
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", szStr, 2048 );
	if( strlen(szStr) == 0 )
		m_szResourceFolder = "\\\\eye-ad\\ToolData\\Resource";
	else m_szResourceFolder = szStr;

	memset( szStr, 0, sizeof(szStr) );
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "EnvironmentTexture", szStr, 2048 );
	m_szEnviTextureFile = szStr;
}

CRenderBase::~CRenderBase()
{
	Finalize();
}

void CRenderBase::Finalize()
{
	if( m_bActivate == false ) return;

	CDnGameWeaponMng::DestroyInstance();
	CDnGameTable::DestroyInstance();

	SAFE_DELETE( m_pSoundEngine );
	SAFE_RELEASE_SPTR( m_CameraHandle );
	SAFE_RELEASE_SPTR( m_hEnviTexture );
	m_FrameSync.End();

	EternityEngine::FinalizeEngine();
	m_bActivate = false;
}

void CRenderBase::Initialize( HWND hWnd )
{
	CRect rcRect;
	m_hWnd = hWnd;

	CalcRenderRect( rcRect );

	EternityEngine::InitializeEngine( m_hWnd, rcRect.Width(), rcRect.Height(), true, true );
	GetEtStateManager()->SetEnable( false );

	char szStr[1024] = { 0, };
	GetCurrentDirectory( 1024, szStr );
	strcat_s( szStr, "\\" );
	CEtResourceMng::GetInstance().RemoveResourcePath( szStr ); 

	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder );
	CEtResourceMng::GetInstance().AddResourcePath( m_szResourceFolder, true );

	SCameraInfo CamInfo;
	CamInfo.fFar = 1000000.f;
	CamInfo.fFogFar = CamInfo.fFogNear = 1000000.f;
	m_CameraHandle = EternityEngine::CreateCamera( &CamInfo );

	EternityEngine::SetGlobalAmbient( &EtColor( 0.9f, 0.9f, 0.9f, 1.f ) );
	SGraphicOption Option;
	Option.DynamicShadowType = ST_NONE;
	EternityEngine::SetGraphicOption( Option );

	m_pSoundEngine = new CEtSoundEngine;
	m_pSoundEngine->Initialize();

	m_FrameSync.Begin( -1 );
	SetEnvironmentTexture( m_szEnviTextureFile );

	CDnGameTable::CreateInstance();
	CDnGameWeaponMng::CreateInstance();

	m_bActivate = true;
}

void CRenderBase::ChangeShaderFolder( const char *szFullPath )
{
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szShaderFolder );
	m_szShaderFolder = szFullPath;
	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder );
}

void CRenderBase::ChangeResourceFolder( const char *szFullPath )
{
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szResourceFolder );
	m_szResourceFolder = szFullPath;
	CEtResourceMng::GetInstance().AddResourcePath( m_szResourceFolder );
}

bool CRenderBase::Render()
{
	if( m_bActivate == false ) return false;

	bool bResult = m_FrameSync.CheckSync();
	if( bResult == true ) {
		LOCAL_TIME LocalTime = m_FrameSync.GetMSTime();
		CString szFrame;
		szFrame.Format( "FPS : %.2f", m_FrameSync.GetFps() );
		EternityEngine::DrawText2D( EtVector2( 0.f, 0.f ), szFrame );

		if( m_bShowGrid ) {
			DrawGrid();
		}
		if( m_bShowAxis ) {
			DrawAxis();
		}
		for( DWORD i=0; i<m_VecLightInfo.size(); i++ ) {
			DrawLight( &m_VecLightInfo[i] );
		}
		m_VecLightInfo.clear();

		UpdateCamera( m_matExWorld );

		CActionBase *pBase = CGlobalValue::GetInstance().GetRootObject();
		if( pBase ) pBase->Process( GetLocalTime() );

		pBase = CGlobalValue::GetInstance().GetRootLightObject();
		if( pBase ) pBase->Process( GetLocalTime() );

		EternityEngine::ClearScreen( m_dwBgColor );
		float fDelta = ( LocalTime - m_PrevLocalTime ) * 0.001f;
		EternityEngine::RenderFrame( fDelta );
		EternityEngine::ShowFrame( NULL );

		m_PrevLocalTime = LocalTime;
	}
	m_FrameSync.UpdateTime();

	return bResult;
}

void CRenderBase::Process()
{
	if( m_bActivate == false ) return;

	LOCAL_TIME LocalTime = m_FrameSync.GetMSTime();
	if( CInputDevice::IsActive() )
		CInputDevice::GetInstance().Process( LocalTime, ( LocalTime - m_PrevLocalTime ) * 0.001f );

}


void CRenderBase::Reconstruction( int cx, int cy )
{
	if( m_bActivate == false ) return;

	CRect rcRect;
	CalcRenderRect( rcRect );

	if( cx == 0 ) cx = rcRect.Width();
	if( cy == 0 ) cy = rcRect.Height();
	EternityEngine::ReinitializeEngine( cx, cy );
}

void CRenderBase::CalcRenderRect( CRect &rcRect )
{
	GetClientRect( m_hWnd, &rcRect );
}

void CRenderBase::UpdateCamera( MatrixEx &Cross )
{
	m_matExWorld = Cross;
	if( !m_CameraHandle ) return;
	m_CameraHandle->Update( Cross );
}

void CRenderBase::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	bool bForground = false;
	HWND hActiveWnd = GetActiveWindow();
	if( hActiveWnd == m_hWnd ) bForground = true;
	if( IsChild( hActiveWnd, m_hWnd ) ) bForground = true;
	if( !bForground ) return;

	CSignalCustomRender *pCustomRender = CGlobalValue::GetInstance().GetSignalCustomRenderObject();
	if( pCustomRender ) {
		if( ( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) || ( GetAsyncKeyState( VK_LMENU ) & 0x8000 ) || ( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) ) {
			pCustomRender->OnInputReceive( nReceiverState, LocalTime, this );
			return;
		}
	}

	CActionBase *pBase = CGlobalValue::GetInstance().GetControlObject();
	if( pBase ) {
		if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
			pBase->OnInputReceive( nReceiverState, LocalTime );
			return;
		}
		else pBase->OnInputReceive( -1, LocalTime );
	}


	if( nReceiverState & IR_MOUSE_MOVE ) {
		if( m_nMouseFlag & WB_DOWN ) {

			m_matExWorld.m_vPosition -= ( m_matExWorld.m_vXAxis * GetMouseVariation().x );
			m_matExWorld.m_vPosition += ( m_matExWorld.m_vYAxis * GetMouseVariation().y );

			m_CameraAt -= ( m_matExWorld.m_vXAxis * GetMouseVariation().x );
			m_CameraAt += ( m_matExWorld.m_vYAxis * GetMouseVariation().y );

		}
		if( m_nMouseFlag & LB_DOWN ) {

			MatrixEx TempCross;
			TempCross = m_matExWorld;
			TempCross.RotateYaw(180);
			TempCross.m_vPosition = m_CameraAt;
			float fLength = EtVec3Length( &( m_matExWorld.m_vPosition - m_CameraAt ) );


			TempCross.RotateYAxis( GetMouseVariation().x / 2.f );
			TempCross.RotatePitch( -GetMouseVariation().y / 2.f );

			m_matExWorld = TempCross;
			m_matExWorld.RotateYaw(180);
			m_matExWorld.m_vPosition = m_CameraAt + ( m_matExWorld.m_vZAxis * -fLength );

		}
	}
	if( nReceiverState & IR_MOUSE_WHEEL ) {
		if( IsInMouseRect( CGlobalValue::GetInstance().GetView() ) )
			m_matExWorld.m_vPosition += ( m_matExWorld.m_vZAxis * ( GetMouseVariation().z )  );
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
	int nRange = 50;
	int nOffset = 5;
	float fSize = 10.0f;
	float fTemp = nRange * fSize;

	EtVector3 vTemp[2];
	for( int i=nRange; i>= -nRange; i-- )
	{
		D3DCOLOR dcColor = (abs(i) % nOffset == 0) ? D3DCOLOR_ARGB(200,200,200,200) : D3DCOLOR_ARGB(150,63,63,63);
		if( i == 0 ) dcColor = D3DCOLOR_ARGB( 255, 255, 255, 0 );

		vTemp[0] = EtVector3( i * fSize, 0, -fTemp );
		vTemp[1] = EtVector3( i * fSize, 0,  fTemp );
		EternityEngine::DrawLine3D( vTemp[0], vTemp[1], dcColor );

		vTemp[0] = EtVector3( -fTemp, 0, i * fSize );
		vTemp[1] = EtVector3(  fTemp, 0, i * fSize );
		EternityEngine::DrawLine3D( vTemp[0], vTemp[1], dcColor );
	}

}

void CRenderBase::DrawAxis()
{
	float fDistance = EtVec3Length( &( m_matExWorld.m_vPosition - m_CameraAt ) );
	DWORD dwColor[3] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };

	EtMatrix matScale;
	float fScale = fDistance / 25.f;
	D3DXMatrixScaling( &matScale, fScale, fScale, fScale );

	EtVector3 vAxis[6];
	for( int i=0; i<6; i++ ) 
	{
		vAxis[i] = m_vAxis[i] * fScale;
		vAxis[i] += m_CameraAt;
		if( i % 2 == 1 ) EternityEngine::DrawLine3D( vAxis[i-1], vAxis[i], dwColor[i/2] );
	}

	EtMatrix matView = *m_CameraHandle->GetViewMat();

	EtMatrixInverse( &matView, NULL, &matView );

	EtMatrix matTemp = matView;
	*((EtVector3*)&matTemp._41) = m_CameraAt + EtVector3( 1.2f * fScale, 0.2f * fScale, 0 );
	EtMatrixMultiply( &matTemp, &matScale, &matTemp );

	EtVector3 vAlphabetX[4];
	for( int i=0; i<4; i++ ) 
	{
		EtVec3TransformCoord( &vAlphabetX[i], &m_vAlphabetX[i], &matTemp );
		if( i % 2 == 1 ) EternityEngine::DrawLine3D( vAlphabetX[i-1], vAlphabetX[i], dwColor[0] );
	}

	matTemp = matView;
	*((EtVector3*)&matTemp._41) = m_CameraAt + EtVector3( 0.2f * fScale, 1.2f * fScale, 0 );
	EtMatrixMultiply( &matTemp, &matScale, &matTemp );

	EtVector3 vAlphabetY[4];
	for( int i=0; i<4; i++ ) 
	{
		EtVec3TransformCoord( &vAlphabetY[i], &m_vAlphabetY[i], &matTemp );
		if( i % 2 == 1 ) EternityEngine::DrawLine3D( vAlphabetY[i-1], vAlphabetY[i], dwColor[1] );
	}

	matTemp = matView;
	*((EtVector3*)&matTemp._41) = m_CameraAt + EtVector3( 0.2f * fScale, 0, 1.2f * fScale );
	EtMatrixMultiply( &matTemp, &matScale, &matTemp );

	EtVector3 vAlphabetZ[6];
	for( int i=0; i<6; i++ ) 
	{
		EtVec3TransformCoord( &vAlphabetZ[i], &m_vAlphabetZ[i], &matTemp );
		if( i % 2 == 1 ) EternityEngine::DrawLine3D( vAlphabetZ[i-1], vAlphabetZ[i], dwColor[2] );
	}

}


void CRenderBase::UpdateLightGrid( SLightInfo *pInfo )
{
	m_VecLightInfo.push_back( *pInfo );
}


void CRenderBase::DrawLight( SLightInfo *pInfo )
{
	float fDistance = EtVec3Length( &( m_matExWorld.m_vPosition - m_CameraAt ) );
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

				// �ܰ�
				DrawSpotFunc1( fPhi, pInfo, false, 0xFF999900 );
				DrawSpotFunc1( -fPhi, pInfo, false, 0xFF999900 );
				DrawSpotFunc1( fPhi, pInfo, true, 0xFF999900 );
				DrawSpotFunc1( -fPhi, pInfo, true, 0xFF999900 );

				// ����
				DrawSpotFunc1( fTheta, pInfo, false, 0xFFFFFF00 );
				DrawSpotFunc1( -fTheta, pInfo, false, 0xFFFFFF00 );
				DrawSpotFunc1( fTheta, pInfo, true, 0xFFFFFF00 );
				DrawSpotFunc1( -fTheta, pInfo, true, 0xFFFFFF00 );

				// �ܰ� ���� ��
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

void CRenderBase::ChangeHitSignalShow( const bool bHitSignalShow )
{
	if( bHitSignalShow == m_bHitSignalShow )
		return;

	m_bHitSignalShow = bHitSignalShow;
}