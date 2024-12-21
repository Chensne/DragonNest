#include "StdAfx.h"
#include "RenderBase.h"
#include "InputDevice.h"
#include "GlobalValue.h"
#include "MainFrm.h"
#include "EtEffect2ToolDoc.h"
#include "EtEffect2ToolView.h"
#include "AxisRenderObject.h"
#include "EtResourceMng.h"

CRenderBase::CRenderBase()
: CInputReceiver( true )
{
	m_bActivate = false;

	m_Cross.m_vPosition = EtVector3( 420.f, 600.f, -680.f );
	m_Cross.m_vXAxis = EtVector3( 0.848041f, 0.f, 0.529931f );
	m_Cross.m_vYAxis = EtVector3( -0.318914f, 0.798635f, 0.510368f );
	m_Cross.m_vZAxis = EtVector3( -0.423226f, -0.601815f, 0.677272f );
	m_Cross.MakeUpCartesianByZAxis();

	m_bActivate = false;
	m_nMouseFlag = 0;
	m_CameraAt = EtVector3( 0.f, 0.f, 0.f );
	m_bShowGrid = true;
	m_BgColor = RGB(64, 64, 64);	

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
		m_szShaderFolder = "\\\\192.168.0.11\\ToolData\\SharedEffect";
	else m_szShaderFolder = szStr;

	memset( szStr, 0, sizeof(szStr) );
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "ResourceFolder", szStr, 2048 );
	if( strlen(szStr) == 0 )
		m_szResourceFolder = "R:\\GameRes\\Resource";
	else m_szResourceFolder = szStr;

	memset( szStr, 0, sizeof(szStr) );	
	GetRegistryString( HKEY_CURRENT_USER, REG_SUBKEY, "EnvironmentTexture", szStr, 2048 );
	m_szEnviTextureFile = szStr;

	GetRegistryNumber(HKEY_CURRENT_USER, REG_SUBKEY, "BackgroundColor", m_BgColor);
	DWORD dwVal;
	GetRegistryNumber(HKEY_CURRENT_USER, REG_SUBKEY, "ShowGrid", dwVal);
	m_bShowGrid = (dwVal != 0);
}

CRenderBase::~CRenderBase()
{
	Finalize();
}

void CRenderBase::Finalize()
{
	if( m_bActivate == false ) return;

	SAFE_RELEASE_SPTR( m_CameraHandle );
	SAFE_RELEASE_SPTR( m_hEnviTexture );
	m_FrameSync.End();

	EternityEngine::FinalizeEngine();
	m_bActivate = false;

	SetRegistryNumber(HKEY_CURRENT_USER, REG_SUBKEY, "BackgroundColor", m_BgColor);
	SetRegistryNumber(HKEY_CURRENT_USER, REG_SUBKEY, "ShowGrid", m_bShowGrid ? 1 : 0);
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
	//RemoveResourceDir( szStr ); 
	static CEtResourceMng s_resourceMng;	
	CEtResourceMng::GetInstance().RemoveResourcePath( szStr, true );
	
	//AddResourceDir( m_szShaderFolder );
	//AddResourceDir( m_szResourceFolder );
	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder, true );	
	//CEtResourceMng::GetInstance().AddResourcePath( m_szResourceFolder, true );

	SCameraInfo CamInfo;
	CamInfo.fFar = 1000000.f;
	CamInfo.fFogFar = CamInfo.fFogNear = 1000000.f;
	m_CameraHandle = EternityEngine::CreateCamera( &CamInfo );

	EternityEngine::SetGlobalAmbient( &EtColor( 0.9f, 0.9f, 0.9f, 1.f ) );
	SGraphicOption Option;
	Option.DynamicShadowType = ST_NONE;
	EternityEngine::SetGraphicOption( Option );

	m_FrameSync.Begin( -1 );
	SetEnvironmentTexture( m_szEnviTextureFile );

	m_bActivate = true;
}

void CRenderBase::ChangeShaderFolder( const char *szFullPath )
{
	//RemoveResourceDir( m_szShaderFolder );
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szShaderFolder, true );
	m_szShaderFolder = szFullPath;
	//AddResourceDir( m_szShaderFolder );
	CEtResourceMng::GetInstance().AddResourcePath( m_szShaderFolder, true );
}

void CRenderBase::ChangeResourceFolder( const char *szFullPath )
{
	//RemoveResourceDir( m_szResourceFolder );
	CEtResourceMng::GetInstance().RemoveResourcePath( m_szResourceFolder, true );
	m_szResourceFolder = szFullPath;
	//AddResourceDir( m_szResourceFolder );
	CEtResourceMng::GetInstance().AddResourcePath( m_szResourceFolder, true );
}

void CRenderBase::Render()
{
	if( m_bActivate == false ) return;

	if( m_FrameSync.CheckSync() == true ) {
		LOCAL_TIME LocalTime = m_FrameSync.GetMSTime();
		CString szFrame;
		szFrame.Format( "FPS : %.2f", m_FrameSync.GetFps() );
		EternityEngine::DrawText2D( EtVector2( 0.f, 0.f ), szFrame );
		char *szTemp[3] = { "High", "Mid", "Low" };
		szFrame.Format( "Particle Count Option : %s", szTemp[CEtBillboardEffect::GetEffectCountOption()] );
		EternityEngine::DrawText2D( EtVector2( 0.f, 0.06f ), szFrame );
		szFrame.Format( "Press up/down key to switch option" );
		EternityEngine::DrawText2D( EtVector2( 0.f, 0.09f ), szFrame );

		EternityEngine::ClearScreen( D3DCOLOR_XRGB( GetRValue(m_BgColor), GetGValue(m_BgColor), GetBValue(m_BgColor) ) );

		if( m_bShowGrid ) {
			bool bOldZ = GetEtDevice()->EnableZ( false );
			DrawGrid();
			DrawAxis();
			GetEtDevice()->EnableZ( bOldZ );
		}
		for( DWORD i=0; i<m_VecLightInfo.size(); i++ ) {
			DrawLight( &m_VecLightInfo[i] );
		}
		m_VecLightInfo.clear();

		UpdateCamera( m_Cross );

		/*CActionBase *pBase = CGlobalValue::GetInstance().GetRootObject();
		if( pBase ) pBase->Process( GetLocalTime() );*/

		/*pBase = CGlobalValue::GetInstance().GetRootLightObject();
		if( pBase ) pBase->Process( GetLocalTime() );*/
		
		float fDelta = ( LocalTime - m_PrevLocalTime ) * 0.001f;
		EternityEngine::RenderFrame( fDelta );
		EternityEngine::ShowFrame( NULL );
		CGlobalValue::GetInstance().Process();	
		CGlobalValue::GetInstance().CalcTick(fDelta);

		m_PrevLocalTime = LocalTime;	
	}
	m_FrameSync.UpdateTime();
}

void CRenderBase::Process()
{
	if( m_bActivate == false ) return;

	LOCAL_TIME LocalTime = m_FrameSync.GetMSTime();
	if( CInputDevice::IsActive() )
	{
		float fDelta = ( LocalTime - m_PrevLocalTime ) * 0.001f;
		CInputDevice::GetInstance().Process( LocalTime, fDelta );
	}
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
	m_Cross = Cross;
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

	/*CSignalCustomRender *pCustomRender = CGlobalValue::GetInstance().GetSignalCustomRenderObject();
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
	}*/
	
	if( nReceiverState & IR_MOUSE_MOVE ) {
		if( (m_nMouseFlag & RB_DOWN) || (m_nMouseFlag & WB_DOWN)) {

			m_Cross.m_vPosition -= ( m_Cross.m_vXAxis * GetMouseVariation().x );
			m_Cross.m_vPosition += ( m_Cross.m_vYAxis * GetMouseVariation().y );

			m_CameraAt -= ( m_Cross.m_vXAxis * GetMouseVariation().x );
			m_CameraAt += ( m_Cross.m_vYAxis * GetMouseVariation().y );

		}
		if( m_nMouseFlag & LB_DOWN ) {

			if( CGlobalValue::GetInstance().GetAxisRenderObject()->m_nSelectAxis == -1 ) {
				MatrixEx TempCross;
				TempCross = m_Cross;
				TempCross.RotateYaw(180);
				TempCross.m_vPosition = m_CameraAt;
				float fLength = EtVec3Length( &( m_Cross.m_vPosition - m_CameraAt ) );

				TempCross.RotateYAxis( GetMouseVariation().x / 2.f );
				TempCross.RotatePitch( -GetMouseVariation().y / 2.f );

				m_Cross = TempCross;
				m_Cross.RotateYaw(180);
				m_Cross.m_vPosition = m_CameraAt + ( m_Cross.m_vZAxis * -fLength );
			}
			else {
				if( CGlobalValue::GetInstance().GetPosOrRot() == 0 ) {
					float fScale = EtVec3Length( &( CRenderBase::GetInstance().GetLastUpdateCamera()->m_vPosition ) ) / 1000.f;
					CGlobalValue::GetInstance().GetAxisRenderObject()->MoveAxis((int)GetMouseVariation().x, (int)GetMouseVariation().y, fScale );
				}
				else if( CGlobalValue::GetInstance().GetPosOrRot() == 1 ) {
					CGlobalValue::GetInstance().GetAxisRenderObject()->RotateAxis((int)GetMouseVariation().x, (int)GetMouseVariation().y );
				}
			}
		}
		else {
			POINT mousePos;
			GetCursorPos( &mousePos );
			CGlobalValue::GetInstance().GetView()->ScreenToClient(&mousePos);
			EtVector3 vOrig, vDir;
			CRenderBase::GetInstance().GetCameraHandle()->CalcPositionAndDir( mousePos.x, mousePos.y, vOrig, vDir );
			CGlobalValue::GetInstance().GetAxisRenderObject()->CheckAxis(vOrig, vDir);
		}
	}
	if( nReceiverState & IR_MOUSE_WHEEL ) {
		if( IsInMouseRect( CGlobalValue::GetInstance().GetView() ) )
			m_Cross.m_vPosition += ( m_Cross.m_vZAxis * ( GetMouseVariation().z ) * 0.5f );
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
	float fSize  = 50.0f;
	int nStep = 5;

	const int iGridRange = 10;
	const float fTemp = (float)iGridRange * fSize;

	for( int iCnt = -iGridRange; iCnt <= iGridRange; ++iCnt )
	{
		D3DXVECTOR3 vecPos1( (float)iCnt * fSize, 0, -fTemp );
		D3DXVECTOR3 vecPos2( (float)iCnt * fSize, 0,  fTemp );

		D3DXVECTOR3 vecPos3( -fTemp, 0, (float)iCnt * fSize );
		D3DXVECTOR3 vecPos4(  fTemp, 0, (float)iCnt * fSize );

		D3DCOLOR dcColor = D3DCOLOR_ARGB(255, __min(255, GetRValue(m_BgColor)*3/2),
																	__min(255, GetGValue(m_BgColor)*3/2),
																	__min(255, GetBValue(m_BgColor)*3/2));
		if( iCnt == 0 ) continue;

		EternityEngine::DrawLine3D( vecPos1, vecPos2, dcColor );
		EternityEngine::DrawLine3D( vecPos3, vecPos4, dcColor );
	}

}

void CRenderBase::DrawAxis()
{
	float fDistance = EtVec3Length( &( m_Cross.m_vPosition - m_CameraAt ) );
	DWORD dwColor[3] = { D3DCOLOR_XRGB(255,32,32), D3DCOLOR_XRGB(32,255,32), D3DCOLOR_XRGB(32,32,255)};
	float fScale = fDistance / 25.f;

	// Draw Axis
	EtVector3 vAxis[6];
	for( int i=0; i<6; i++ ) {
		vAxis[i] = m_vAxis[i] * 500.f;
		//vAxis[i] += m_CameraAt;
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vAxis[i-1], vAxis[i], dwColor[i/2] );
		}
	}

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
	mat._42 = m_CameraAt.y + ( 0.3f * fScale );
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
	mat._41 = m_CameraAt.x + ( 0.3f * fScale );
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
	mat._41 = m_CameraAt.x + ( 0.3f * fScale );
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


void CRenderBase::UpdateLightGrid( SLightInfo *pInfo )
{
	m_VecLightInfo.push_back( *pInfo );
}


void CRenderBase::DrawLight( SLightInfo *pInfo )
{
	float fDistance = EtVec3Length( &( m_Cross.m_vPosition - m_CameraAt ) );
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
