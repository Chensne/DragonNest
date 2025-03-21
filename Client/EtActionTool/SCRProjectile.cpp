#include "StdAfx.h"
#include "SCRProjectile.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRProjectile::CSCRProjectile( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRProjectile::~CSCRProjectile()
{
}

void CSCRProjectile::Initialize()
{
	m_bActivate = true;
	OnModify();

}

void CSCRProjectile::OnModify()
{
	m_matExWorld.Identity();
	m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();

	m_matExWorld.m_vPosition = m_pSignal->GetProperty(1)->GetVariableVector3();
}

void CSCRProjectile::OnSelect( bool bFirst )
{
	EtVector3 vPos[2];
	float fSize = 30.f;

	CUnionValueProperty *pVariable = m_pSignal->GetProperty(1);
	EtVector3 vDist = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + pVariable->GetVariableVector3();

	vPos[0] = EtVector3( 0.f, 0.f, fSize ) + vDist;
	vPos[1] = EtVector3( 0.f, 0.f, -fSize ) + vDist;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );

	vPos[0] = EtVector3( 0.f, fSize, 0.f ) + vDist;
	vPos[1] = EtVector3( 0.f, -fSize, 0.f ) + vDist;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );

	vPos[0] = EtVector3( fSize, 0.f, 0.f ) + vDist;
	vPos[1] = EtVector3( -fSize, 0.f, 0.f ) + vDist;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );


	//  [12/10/2007 nextome]
	// Make Direction
	EtVector3 m_vDirection[18];
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
	pVariable = m_pSignal->GetProperty(8);
	
	if ( !pVariable )
		return;

	EtVector3 vDir = pVariable->GetVariableVector3();

	EtVector3 vDirection[18];
	MatrixEx Cross;
	Cross.m_vPosition = m_pSignal->GetProperty(1)->GetVariableVector3();
	Cross.m_vZAxis = -vDir;
	Cross.MakeUpCartesianByZAxis();

	for( int i=0; i<18; i++ ) {
		vDirection[i] = m_vDirection[i] * 5.0f;
		EtVec3TransformCoord( &vDirection[i], &vDirection[i], Cross );
		if( i % 2 == 1 ) {
			EternityEngine::DrawLine3D( vDirection[i-1], vDirection[i], 0xFFFFFF00 );
		}
	}

}

void CSCRProjectile::OnUnSelect()
{
}

void CSCRProjectile::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
}

void CSCRProjectile::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
{


	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) 
	{
		if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 )
		{
			if( pReceiver->IsPushMouseButton(0) )
			{
				MatrixEx *pCamera = CRenderBase::GetInstance().GetLastUpdateCamera();
				MatrixEx Cross;

				Cross.m_vPosition = m_pSignal->GetProperty(1)->GetVariableVector3();
				Cross.m_vXAxis = pCamera->m_vXAxis;
				Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
				EtVec3Cross( &Cross.m_vZAxis, &Cross.m_vXAxis, &Cross.m_vYAxis );
				Cross.MoveLocalXAxis( pReceiver->GetMouseVariation().x );
				Cross.MoveLocalZAxis( -pReceiver->GetMouseVariation().y );

				m_pSignal->GetProperty(1)->SetVariable( Cross.m_vPosition );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 1, (LPARAM)m_pSignal->GetProperty(1) );
			}
			else if( pReceiver->IsPushMouseButton(1) ) {
				EtVector3 vPos = m_pSignal->GetProperty(1)->GetVariableVector3();
				vPos.y += -pReceiver->GetMouseVariation().y;
				m_pSignal->GetProperty(1)->SetVariable( vPos );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 1, (LPARAM)m_pSignal->GetProperty(1) );
			}
			else if( pReceiver->IsPushMouseButton(2) ) {
				MatrixEx Cross;
				Cross.m_vZAxis = m_pSignal->GetProperty(8)->GetVariableVector3();
				Cross.MakeUpCartesianByZAxis();

				Cross.RotateYAxis( pReceiver->GetMouseVariation().x / 2.f );
				Cross.RotatePitch( pReceiver->GetMouseVariation().y / 2.f );
				m_pSignal->GetProperty(8)->SetVariable( Cross.m_vZAxis );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 8, (LPARAM)m_pSignal->GetProperty(8) );

			}


		
		}
	}
}
