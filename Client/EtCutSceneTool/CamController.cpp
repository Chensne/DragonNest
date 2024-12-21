#include "StdAfx.h"
#include "CamController.h"
#include "Arcball.h"


CCamController::CCamController(void) : m_fLookAtLength( 0.0f ),
									   m_iPrevDownX( 0 ),
									   m_iPrevDownY( 0 ),
									   m_pWorldArcball( new CArcBall ),
									   m_pCamArcball( new CArcBall ),
									   m_vCamPos( 0.0f, 0.0f, 0.0f ),
									   m_vLookAt( 0.0f, 0.0f, 0.0f ),
									   m_vUp( 0.0f, 0.0f, 0.0f ),
									   m_vSide( 1.0f, 0.0f, 0.0f )
{
	EtMatrix matIdentity;
	EtMatrixIdentity( &matIdentity );

	m_matWorld = matIdentity;
	m_matNowRot = matIdentity;
	m_matLastRot = matIdentity;
	m_matTrans = matIdentity;

	m_matCamNowRot= matIdentity;
	m_matCamLastRot = matIdentity;
	m_matInvCamNowRot = matIdentity;
	m_matWorldRot = matIdentity;
}



CCamController::~CCamController(void)
{
	delete m_pWorldArcball;
	delete m_pCamArcball;
}



void CCamController::Reset( void )
{
	m_fLookAtLength = 0.0f;
	m_iPrevDownX = 0;
	m_iPrevDownY = 0;

	m_vCamPos = EtVector3( 0.0f, 0.0f, 0.0f );
	m_vLookAt = EtVector3( 0.0f, 0.0f, 0.0f );
	m_vUp = EtVector3( 0.0f, 0.0f, 0.0f );
	m_vSide = EtVector3( 1.0f, 0.0f, 0.0f );

	EtMatrix matIdentity;
	EtMatrixIdentity( &matIdentity );

	m_matWorld = matIdentity;
	m_matNowRot = matIdentity;
	m_matLastRot = matIdentity;
	m_matTrans = matIdentity;

	m_matCamNowRot= matIdentity;
	m_matCamLastRot = matIdentity;
	m_matInvCamNowRot = matIdentity;
	m_matWorldRot = matIdentity;
}



void CCamController::SetOriCamPos( const EtVector3& vCamPos )
{
	m_vOriCamPos = vCamPos;
	SetCamPos( vCamPos );
}

void CCamController::SetOriLookAt( const EtVector3& vLookAt )
{
	m_vOriLookAt = vLookAt;
	SetLookAt( vLookAt );
}

void CCamController::SetOriUpVector( const EtVector3& vUp )
{
	m_vOriUp = vUp;
	SetUpVector( vUp );
}


void CCamController::SetCamPos( const EtVector3& vCamPos )
{
	m_vCamPos = vCamPos;

	EtVector3 vDir = m_vCamPos - m_vLookAt;
	m_fLookAtLength = D3DXVec3Length( &vDir );
}



void CCamController::SetLookAt( const EtVector3& vLookAt )
{
	m_vLookAt = vLookAt;

	EtVector3 vDir = m_vCamPos - m_vLookAt;
	m_fLookAtLength = D3DXVec3Length( &vDir );
}


void CCamController::SetUpVector( const EtVector3& vUp )
{
	m_vUp = vUp;
}



void CCamController::SetViewMatrix( const EtMatrix& matView )
{
	m_matView = matView;
}


void CCamController::SetProjMatrix( const EtMatrix& matProj )
{
	m_matProj = matProj;
}


void CCamController::SetViewport( const EtViewPort& Viewport )
{
	m_Viewport = Viewport;

	m_pWorldArcball->SetBound( (float)Viewport.Width, (float)Viewport.Height );
	m_pCamArcball->SetBound( (float)Viewport.Width, (float)Viewport.Height );
}



void CCamController::GetMatrix( EtMatrix& matResult )
{
	// ���� ��Ʈ���� ����ؼ� �Ѱ��ش�.
	// �ٶ󺸰� �ִ� ���� �������� ȸ�������ְ�, �ٽ� �����·�.
	EtMatrix matTrans, matOriTrans;
	EtMatrixTranslation( &matTrans, -m_vLookAt.x, -m_vLookAt.y, -m_vLookAt.z );
	EtMatrixTranslation( &matOriTrans, m_vLookAt.x, m_vLookAt.y, m_vLookAt.z );
	m_matWorld = matTrans * m_matNowRot * matOriTrans;

	matResult = m_matWorld;
}


void CCamController::_UpdatePrevDown( int iXPos, int iYPos )
{
	m_iPrevDownX = iXPos;
	m_iPrevDownY = iYPos;
}


void CCamController::OnLButtonDown( int iXPos, int iYPos )
{
	m_matLastRot = m_matNowRot;
	m_pWorldArcball->Click( iXPos, iYPos );
}


void CCamController::OnLButtonCon( int iXPos, int iYPos )
{
	D3DXQUATERNION qRot;
	m_pWorldArcball->Drag( iXPos, iYPos, &qRot );

	EtMatrixRotationQuaternion( &m_matNowRot, &qRot );

	// ���� ���带 ī�޶��� �信 ���߰� ȸ���� ������ �ٽ� �����·� ���� ��Ų��.
	// �׷��� ī�޶� �������� ���� ���� ��ȯ�� ȸ�� ����� ���´�.
	m_matNowRot = m_matCamNowRot * m_matNowRot * m_matInvCamNowRot;
	EtMatrixMultiply( &m_matNowRot, &m_matLastRot, &m_matNowRot );
}


void CCamController::OnRButtonDown( int iXPos, int iYPos )
{
	m_matCamLastRot = m_matCamNowRot;
	m_pCamArcball->Click( iXPos, iYPos );
}


void CCamController::OnRButtonCon( int iXPos, int iYPos )
{
	D3DXQUATERNION qRot;
	m_pCamArcball->Drag( iXPos, iYPos, &qRot );

	EtMatrixRotationQuaternion( &m_matCamNowRot, &qRot );
	EtMatrixMultiply( &m_matCamNowRot, &m_matCamLastRot, &m_matCamNowRot );

	EtMatrixInverse( &m_matInvCamNowRot, NULL, &m_matCamNowRot );
	EtVector3 vResultCamPos, vResultUp;

	// ���� ��ǥ���� ȸ������ ����Ѵ�.
	// Look At �� ������ ���־����~
	EtVector3 vLocalCamPos = m_vOriCamPos - m_vOriLookAt;
	D3DXVec3TransformCoord( &vResultCamPos, &vLocalCamPos/*m_vOriCamPos*/, &m_matInvCamNowRot );
	D3DXVec3TransformCoord( &vResultUp, &m_vOriUp, &m_matInvCamNowRot );

	m_vCamPos = vResultCamPos + m_vOriLookAt;
	m_vUp = vResultUp;

	// Side���� ���ϱ�. bintitle.
	EtVector3 vDir = m_vCamPos - m_vLookAt;
	EtVec3Normalize( &vDir, &vDir );	
	EtVec3Cross( &m_vSide, &EtVector3(0.0f,1.0f,0.0f), &vDir );
	EtVec3Normalize( &m_vSide, &m_vSide );
}


void CCamController::OnCButtonDown( int iXPos, int iYPos )
{
	_UpdatePrevDown( iXPos, iYPos );
}


void CCamController::OnCButtonCon( int iXPos, int iYPos )
{
	// �̰� ���� ī�޶� �̵����� �ֵ��� ����.
	// Look at �� ���Ѵ�.
	int iXDelta = iXPos - m_iPrevDownX;
	int iYDelta = iYPos - m_iPrevDownY;

	// ���� ������ ���� ������ ���� ���͸� ���Ѵ�.
	EtVector3 vStart, vEnd;
	EtVector3 vPrevPos( (float)m_iPrevDownX, (float)m_iPrevDownY, 0.0f ), vNowPos( (float)iXPos, (float)iYPos, 0.0f );
	EtVec3Unproject( &vStart, &vPrevPos, &m_Viewport, &m_matProj, &m_matView, &m_matWorld );
	EtVec3Unproject( &vEnd, &vNowPos, &m_Viewport, &m_matProj, &m_matView, &m_matWorld );
	EtVector3 vDir = vEnd - vStart;

	// ī�޶� ���� �̵��ȴ�.
	EtVector3 vDelta = vDir * m_fLookAtLength;
	m_vCamPos -= vDelta;
	m_vLookAt -= vDelta;

	m_vOriCamPos -= vDelta;
	m_vOriLookAt -= vDelta;

	_UpdatePrevDown( iXPos, iYPos );
}



void CCamController::OnMouseWheel( int iWheelDelta )
{
	EtVector3 vDir = m_vCamPos - m_vLookAt;
	m_vCamPos += (vDir * ((float)iWheelDelta / 1000.0f));

	vDir = m_vOriCamPos - m_vOriLookAt;
	m_vOriCamPos += (vDir * ((float)iWheelDelta / 1000.0f));

	// ī�޶�� look at �Ÿ� ������Ʈ
	m_fLookAtLength = D3DXVec3Length( &vDir );
}


void CCamController::OnMoveFrontAndBack( float fSpeed )
{
	EtVector3 vDir = m_vCamPos - m_vLookAt;
	D3DXVec3Normalize( &vDir, &vDir );

	m_vCamPos += ( vDir * fSpeed );
	m_vOriCamPos = m_vCamPos;
}

void CCamController::OnMoveLeftAndRight( float fSpeed )
{
	EtVector3 vDir( m_vSide * fSpeed );
	m_vCamPos += vDir;
	m_vLookAt += vDir;

	m_vOriCamPos = m_vCamPos;
}