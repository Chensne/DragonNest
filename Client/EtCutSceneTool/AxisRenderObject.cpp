#include "StdAfx.h"
#include "EternityEngine.h"
#include "AxisRenderObject.h"
#include "EtMatrixEx.h"

/*
#include "TEtWorldProp.h"
#include "EtWorldSector.h"
*/


CAxisRenderObject::CAxisRenderObject( )
{
	ZeroMemory( m_pAxis, sizeof(m_pAxis) );
	ZeroMemory( m_pAxisBadak, sizeof(m_pAxisBadak) );
	ZeroMemory( m_pSelect, sizeof(m_pSelect) );
	ZeroMemory( m_nPrimitiveCount, sizeof(m_nPrimitiveCount) );
	m_fAxisLength = 35.f;
	m_fAxisPos = 20.f;
	m_fAxisRadius = 2.f;
	m_nSelectAxis = CAxisRenderObject::AXIS_NONE;
	m_vRotation = EtVector3( 0, 0, 0 );

	m_fScale = 2.f;
	m_bShow = true;
}                                                                                                                                                                 

CAxisRenderObject::~CAxisRenderObject()
{
	Destroy();
}

void CAxisRenderObject::Initialize()
{
	float fLength = m_fAxisLength - m_fAxisPos;

	m_nPrimitiveCount[0] = CreateCone( &m_pAxis[0], 0xFFFF0000, m_fAxisRadius, fLength, 10, 0, m_fAxisPos );
	CreateCone( &m_pAxisBadak[0], 0xFFFF0000, m_fAxisRadius, 0.f, 10, 0, m_fAxisPos );
	CreateCone( &m_pSelect[0], 0x80EEEE20, m_fAxisRadius + 2.f, fLength + 2.f, 10, 0, m_fAxisPos );

	m_nPrimitiveCount[1] = CreateCone( &m_pAxis[1], 0xFF00FF00, m_fAxisRadius, fLength, 10, 1, m_fAxisPos );
	CreateCone( &m_pAxisBadak[1], 0xFF00FF00, m_fAxisRadius, 0.f, 10, 1, m_fAxisPos );
	CreateCone( &m_pSelect[1], 0x80EEEE20, m_fAxisRadius + 2.f, fLength + 2.f, 10, 1, m_fAxisPos );

	m_nPrimitiveCount[2] = CreateCone( &m_pAxis[2], 0xFF0000FF, m_fAxisRadius, fLength, 10, 2, m_fAxisPos );
	CreateCone( &m_pAxisBadak[2], 0xFF0000FF, m_fAxisRadius, 0.f, 10, 2, m_fAxisPos );
	CreateCone( &m_pSelect[2], 0x80EEEE20, m_fAxisRadius + 2.f, fLength + 2.f, 10, 2, m_fAxisPos );
}

void CAxisRenderObject::Destroy()
{
	for( int i=0; i<3; i++ ) {
		SAFE_DELETEA( m_pAxis[i] );
		SAFE_DELETEA( m_pAxisBadak[i] );
		SAFE_DELETEA( m_pSelect[i] );
	}
}

void CAxisRenderObject::RenderCustom( float fElapsedTime )
{
	if( m_bShow )
		DrawAxis();
}

void CAxisRenderObject::Update( EtMatrix *pMatrix )
{
	m_WorldMat = *pMatrix;
}

/*
void CAxisRenderObject::Render()
{
	DrawAxis();
}

void CAxisRenderObject::GetExtent( EtVector3 &Origin, EtVector3 &Extent )
{
	Origin = EtVector3( m_WorldMat._41, m_WorldMat._42, m_WorldMat._43 );
	Extent = EtVector3( m_WorldMat._41, m_WorldMat._42, m_WorldMat._43 );
}
*/

void CAxisRenderObject::DrawAxis()
{
	DWORD dwLightFlag, dwCullMode, dwZEnable;
	LPDIRECT3DDEVICE9 pDev = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
	pDev->GetRenderState( D3DRS_LIGHTING, &dwLightFlag );
	pDev->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
	pDev->GetRenderState( D3DRS_ZENABLE, &dwZEnable );

	pDev->SetRenderState( D3DRS_ZENABLE, FALSE );
	pDev->SetRenderState( D3DRS_LIGHTING, FALSE );
	pDev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	D3DXMATRIX mat, matScale;
	mat = m_WorldMat;

	EtCameraHandle hCamera = CEtCamera::GetActiveCamera();
	D3DXMatrixScaling( &matScale, m_fScale, m_fScale, m_fScale );
	D3DXMatrixMultiply( &mat, &matScale, &mat );
	pDev->SetTransform( D3DTS_WORLD, &mat );
	pDev->SetTransform( D3DTS_VIEW, hCamera->GetViewMat() );
	pDev->SetTransform( D3DTS_PROJECTION, hCamera->GetProjMat() );
	pDev->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	pDev->SetVertexShader( NULL );
	pDev->SetPixelShader( NULL );

	for( int i=0; i<4; i++ ) pDev->SetTexture( 0, NULL );

	for( DWORD i = 0; i < 3; i++ ) {
		pDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, m_nPrimitiveCount[i], m_pAxis[i], sizeof(VERTEX_FORMAT) );
		pDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, m_nPrimitiveCount[i], m_pAxisBadak[i], sizeof(VERTEX_FORMAT) );
		if( m_nSelectAxis == i ) {
			DWORD dwAlphaBlend;
			DWORD dwBlendOP, dwSrcBlend, dwDestBlend;

			pDev->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlend );
			pDev->GetRenderState( D3DRS_BLENDOP, &dwBlendOP );
			pDev->GetRenderState( D3DRS_SRCBLEND, &dwSrcBlend );
			pDev->GetRenderState( D3DRS_DESTBLEND, &dwDestBlend );

			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pDev->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
			pDev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			pDev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			pDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, m_nPrimitiveCount[i], m_pSelect[i], sizeof(VERTEX_FORMAT) );

			pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, dwAlphaBlend );
			pDev->SetRenderState( D3DRS_BLENDOP, dwBlendOP );
			pDev->SetRenderState( D3DRS_SRCBLEND, dwSrcBlend );
			pDev->SetRenderState( D3DRS_DESTBLEND, dwDestBlend );
		}
	}

	VERTEX_FORMAT vPos[2];

	vPos[0].vPos = EtVector3( 0.f, 0.f, 0.f );
	vPos[1].vPos = EtVector3( m_fAxisPos, 0, 0 );
	vPos[0].dwColor = vPos[1].dwColor = ( m_nSelectAxis == AXIS_X ) ? 0xFFFFFF20 : 0xFFFF0000;
	pDev->DrawPrimitiveUP( D3DPT_LINELIST, 2, &vPos[0], sizeof(VERTEX_FORMAT) );

	vPos[0].vPos = EtVector3( 0.f, 0.f, 0.f );
	vPos[1].vPos = EtVector3( 0, m_fAxisPos, 0 );
	vPos[0].dwColor = vPos[1].dwColor = ( m_nSelectAxis == AXIS_Y ) ? 0xFFFFFF20 : 0xFF00FF00;
	pDev->DrawPrimitiveUP( D3DPT_LINELIST, 2, vPos, sizeof(VERTEX_FORMAT) );

	vPos[0].vPos = EtVector3( 0.f, 0.f, 0.f );
	vPos[1].vPos = EtVector3( 0.f, 0.f, m_fAxisPos );
	vPos[0].dwColor = vPos[1].dwColor = ( m_nSelectAxis == AXIS_Z ) ? 0xFFFFFF20 : 0xFF0000FF;
	pDev->DrawPrimitiveUP( D3DPT_LINELIST, 2, vPos, sizeof(VERTEX_FORMAT) );

	pDev->SetRenderState( D3DRS_LIGHTING, dwLightFlag );
	pDev->SetRenderState( D3DRS_CULLMODE, dwCullMode );
	pDev->SetRenderState( D3DRS_ZENABLE, dwZEnable );
}

int CAxisRenderObject::CreateCone( VERTEX_FORMAT **pVertex, DWORD dwColor, float fRadius, float fLength, int nSegment, int nAxis, float fAxisPos )
{
	*pVertex = new VERTEX_FORMAT[ ( nSegment + 1 ) * 2 ];
	int nRadianOffset = 360 / (nSegment-1);
	EtVector3 vPos;

	switch( nAxis ) {
		case 0:
			vPos = EtVector3( fAxisPos + fLength, 0, 0 );
			break;
		case 1:
			vPos = EtVector3( 0, fAxisPos + fLength, 0 );
			break;
		case 2:
			vPos = EtVector3( 0, 0, fAxisPos + fLength );
			break;
	}
	if( nAxis > 2 ) (*pVertex)[0].vPos = -vPos;
	else (*pVertex)[0].vPos = vPos;
	(*pVertex)[0].dwColor = dwColor;
	for( int i=0; i<nSegment; i++ ) {
		switch( nAxis ) {
			case 0:
				vPos.x = fAxisPos;
				vPos.y = cos( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.z = sin( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				break;
			case 1:
				vPos.x = cos( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.y = fAxisPos;
				vPos.z = sin( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				break;
			case 2:
				vPos.x = cos( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.y = sin( ( nRadianOffset * i ) / 180.f * 3.1415926f ) * fRadius;
				vPos.z = fAxisPos;
				break;
		}
		if( nAxis > 2 )
			(*pVertex)[1+i].vPos = -vPos;
		else 
			(*pVertex)[1+i].vPos = vPos;
		(*pVertex)[1+i].dwColor = dwColor;
	}

	return ( nSegment - 1 );
}

int CAxisRenderObject::CheckAxis( const EtVector3& vCamPos, EtVector3 &vOrig, EtVector3 &vDir )
{
//	if( m_pProp == NULL ) return -1;
//	SAABox Box;
	SOBB Box;
	float fDistX, fDistY, fDistZ;


	m_nSelectAxis = AXIS_NONE;

	// X Axis Check	
	//vPos = cross.m_vPosition;
	MatrixEx cross = m_WorldMat;
	EtVector3 vPos = cross.m_vPosition;

	float fLength = m_fAxisLength - m_fAxisPos;
	Box.Center = vPos;
	Box.Center += cross.m_vXAxis * ( ( fLength + ( m_fAxisPos / 2.f ) ) * m_fScale );
	Box.Axis[0] = cross.m_vXAxis;
	Box.Axis[1] = cross.m_vYAxis;
	Box.Axis[2] = cross.m_vZAxis;
	Box.Extent[0] = ( ( m_fAxisPos / 2.f ) * m_fScale );
	Box.Extent[1] = ( m_fAxisRadius * m_fScale );
	Box.Extent[2] = ( m_fAxisRadius * m_fScale );

	if( TestLineToOBB( vOrig, vDir, Box ) == true ) {
		m_nSelectAxis = AXIS_X;
		//fDistX = EtVec3Length( &EtVector3( Box.Center - CRenderBase::GetInstance().GetLastUpdateCamera()->m_vPosition ) );
		fDistX = EtVec3Length( &EtVector3( Box.Center - vCamPos ) );
	}

	// Y Axis Check
	Box.Center = vPos;
	Box.Center += cross.m_vYAxis * ( ( fLength + ( m_fAxisPos / 2.f ) ) * m_fScale );
	Box.Extent[0] = ( m_fAxisRadius * m_fScale );
	Box.Extent[1] = ( ( m_fAxisPos / 2.f ) * m_fScale ) / 2.f;
	Box.Extent[2] = ( m_fAxisRadius * m_fScale );

	if( TestLineToOBB( vOrig, vDir, Box ) == true ) {
		fDistY = EtVec3Length( &EtVector3( Box.Center - vCamPos ) );

		if( m_nSelectAxis == AXIS_NONE ) m_nSelectAxis = AXIS_Y;
		else if( m_nSelectAxis == 0 && fDistY < fDistX )
			m_nSelectAxis = AXIS_Y;
	}

	// Z Axis Check
	Box.Center = vPos;
	Box.Center += cross.m_vZAxis * ( ( fLength + ( m_fAxisPos / 2.f ) ) * m_fScale );
	Box.Extent[0] = ( m_fAxisRadius * m_fScale );
	Box.Extent[1] = ( m_fAxisRadius * m_fScale );
	Box.Extent[2] = ( ( m_fAxisPos / 2.f ) * m_fScale ) / 2.f;

	if( TestLineToOBB( vOrig, vDir, Box/*, fDistX*/ ) == true ) {
		fDistZ = EtVec3Length( &EtVector3( Box.Center - vCamPos ) );

		if( m_nSelectAxis == AXIS_NONE ) m_nSelectAxis = AXIS_Z;
		else if( m_nSelectAxis == 0 && fDistZ < fDistX )
			m_nSelectAxis = AXIS_Z;
		else if( m_nSelectAxis == AXIS_Y && fDistZ < fDistY )
			m_nSelectAxis = AXIS_Z;
	}
	// ��ü Check
	SAABox ABox;
	float fDist;
	//((CTEtWorldProp*)m_pProp)->GetBoundingBox( ABox );
	if( m_nSelectAxis == AXIS_NONE ) {
		if( TestLineToBox( vOrig, vDir, ABox, fDist ) == true ) {
			m_nSelectAxis = AXIS_ALL;
		}
	}

	return m_nSelectAxis;

}

void CAxisRenderObject::RotateAxis( int nX, int nY )
{
	float fValue = ( nX * 0.3f ) + ( nY * 0.3f );
	switch( CAxisRenderObject::m_nSelectAxis ) {
		case 0:	m_vRotation.y += fValue;
			break;
		case 1:	m_vRotation.x += fValue;
			break;
		case 2:	m_vRotation.z += fValue;
			break;
	}
	MatrixEx Cross;

	Cross.RotateYaw( -m_vRotation.x );
	Cross.RotatePitch( m_vRotation.y );
	Cross.RotateRoll( -m_vRotation.z );

	Cross.SetPosition( *(EtVector3*)&m_WorldMat._41 );

	m_WorldMat = Cross;
	//if( CGlobalValue::GetInstance().GetRotationPtr() ) {
	//	*CGlobalValue::GetInstance().GetRotationPtr() = m_vRotation;
	//}
	//CGlobalValue::GetInstance().GetPropertyPaneView()->Refresh();
}

void CAxisRenderObject::SetRotation( EtVector3 rot )
{
	m_vRotation = rot;
	MatrixEx Cross;
	Cross.RotateYaw( m_vRotation.y );
	Cross.RotatePitch( -m_vRotation.x );
	Cross.RotateRoll( -m_vRotation.z );
	Cross.SetPosition( *(EtVector3*)&m_WorldMat._41 );

	m_WorldMat = Cross;
}
//
//void CAxisRenderObject::MoveAxis( int nX, int nY, float fSpeed )
//{
////	if( !m_pProp ) return;
//	EtMatrix matView = *(*CRenderBase::GetInstance().GetLastUpdateCamera());
//
//	EtVector3 vVec, vAxis;
//	EtVector3 *pPos = (EtVector3*)&m_WorldMat._41;//&EtVector3(0,0,0);//m_pProp->GetPosition();
//
//	MatrixEx cross;
////	static float fSpeed = 5.f;
//	switch( m_nSelectAxis ) {
//		case 0:
//			vAxis = CRenderBase::GetInstance().GetLastUpdateCamera()->m_vXAxis;
//			*pPos += ( vAxis * (float)nX * fSpeed );
//
//			vAxis = CRenderBase::GetInstance().GetLastUpdateCamera()->m_vZAxis;
//			vAxis.y = 0.f;
//			EtVec3Normalize( &vAxis, &vAxis );
//			*pPos += ( vAxis * (float)-nY * fSpeed );
//			break;
//		case 1:
//			vAxis = cross.m_vYAxis;//CRenderBase::GetInstance().GetLastUpdateCamera()->m_vYAxis;
//			*pPos += ( vAxis * (float)-nY * fSpeed );
//			break;
//		case 2:
//			vAxis = CRenderBase::GetInstance().GetLastUpdateCamera()->m_vXAxis;
//			*pPos += ( vAxis * (float)nX * fSpeed );
//
//			vAxis = CRenderBase::GetInstance().GetLastUpdateCamera()->m_vYAxis;
////			vAxis.y = 0.f;
//			EtVec3Normalize( &vAxis, &vAxis );
//			*pPos += ( vAxis * (float)-nY * fSpeed );
//			break;
//		case 3:
//			{
//				vAxis = CRenderBase::GetInstance().GetLastUpdateCamera()->m_vXAxis;
//				*pPos += ( vAxis * (float)nX * fSpeed );
//
//				vAxis = CRenderBase::GetInstance().GetLastUpdateCamera()->m_vYAxis;
//				*pPos += ( vAxis * (float)-nY * fSpeed );
//			}
//			break;
//	}
//
//	if( CGlobalValue::GetInstance().GetPositionPtr() ) {
//		*CGlobalValue::GetInstance().GetPositionPtr() = *pPos;
//	}
//	
//	CGlobalValue::GetInstance().GetPropertyPaneView()->Refresh();
////	((CTEtWorldProp*)m_pProp)->UpdateMatrixEx();
//
//}
//
