#include "StdAfx.h"
#include "SCGravity.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRGravity::CSCRGravity( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRGravity::~CSCRGravity()
{
}

void CSCRGravity::Initialize()
{
	m_bActivate = true;
	OnModify();
}

void CSCRGravity::OnModify()
{
	for( int i=0; i<5; i++ ) m_vVecList[i].clear();

	float fDistanceMax = m_pSignal->GetProperty(0)->GetVariableFloat();
	float fDistanceMin = m_pSignal->GetProperty(6)->GetVariableFloat();
	float fHeightMax = m_pSignal->GetProperty(9)->GetVariableFloat();
	float fHeightMin = m_pSignal->GetProperty(10)->GetVariableFloat();
	float fCenterAngle = m_pSignal->GetProperty(8)->GetVariableFloat();
	float fAngle = m_pSignal->GetProperty(7)->GetVariableFloat();

	EtVector3 vPos = EtVector3( 0.f, 0.f, 0.f );
	EtVector3 vCenter = EtVector3( 0.f, 0.f, 1.f );

	EtMatrix matRotate;
	EtMatrixRotationY( &matRotate, fCenterAngle / 180.f * D3DX_PI );
	EtVec3TransformNormal( &vCenter, &vCenter, &matRotate );

	EtVector3 vTemp;

	int nStart = int(fCenterAngle - ( fAngle ));
	int nEnd = int(fCenterAngle + ( fAngle ));

	for( int i=nStart; i<=nEnd; i++ ) {
		EtMatrixRotationY( &matRotate, ( fCenterAngle - i ) / 180.f * D3DX_PI );
		D3DXVec3TransformNormal( &vTemp, &vCenter, &matRotate );

		EtVector3 vVtx = vPos + vTemp * fDistanceMin;
		vVtx.y = vPos.y + fHeightMax;
		m_vVecList[0].push_back( vVtx );
		if( i != nStart && i != nEnd ) m_vVecList[0].push_back( vVtx );

		vVtx.y = vPos.y + fHeightMin;
		m_vVecList[1].push_back( vVtx );
		if( i != nStart && i != nEnd ) m_vVecList[1].push_back( vVtx );

		vVtx = vPos + vTemp * fDistanceMax;
		vVtx.y = vPos.y + fHeightMax;
		m_vVecList[2].push_back( vVtx );
		if( i != nStart && i != nEnd ) m_vVecList[2].push_back( vVtx );

		vVtx.y = vPos.y + fHeightMin;
		m_vVecList[3].push_back( vVtx );
		if( i != nStart && i != nEnd ) m_vVecList[3].push_back( vVtx );

		if( i == nStart ) 
		{
			m_vVecList[4].push_back( m_vVecList[0][ m_vVecList[0].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[1][ m_vVecList[1].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[2][ m_vVecList[2].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[3][ m_vVecList[3].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[1][ m_vVecList[1].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[3][ m_vVecList[3].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[0][ m_vVecList[0].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[2][ m_vVecList[2].size() - 1 ] );
		}
		else if( i == nEnd ) 
		{
			m_vVecList[4].push_back( m_vVecList[0][ m_vVecList[0].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[1][ m_vVecList[1].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[2][ m_vVecList[2].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[3][ m_vVecList[3].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[1][ m_vVecList[1].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[3][ m_vVecList[3].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[0][ m_vVecList[0].size() - 1 ] );
			m_vVecList[4].push_back( m_vVecList[2][ m_vVecList[2].size() - 1 ] );

		}
	}

}

void CSCRGravity::OnSelect( bool bFirst )
{
	EtVector3 vDist = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(5)->GetVariableVector3();
	EtVector3 vPos[2];
	for( int i=0; i<5; i++ ) {
		if( m_vVecList[i].size() < 2 ) continue;
		for( DWORD j=0; j<m_vVecList[i].size(); j+=2 ) {
			vPos[0] = m_vVecList[i][j] + vDist;
			vPos[1] = m_vVecList[i][j+1] + vDist;
			EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );
		}
	}
}

void CSCRGravity::OnUnSelect()
{
}

void CSCRGravity::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	EtVector3 vDist = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(5)->GetVariableVector3();
	EtVector3 vPos[2];
	for( int i=0; i<5; i++ ) {
		if( m_vVecList[i].size() < 2 ) continue;
		for( DWORD j=0; j<m_vVecList[i].size(); j+=2 ) {
			vPos[0] = m_vVecList[i][j] + vDist;
			vPos[1] = m_vVecList[i][j+1] + vDist;
			EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );
		}
	}
}

void CSCRGravity::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
{
	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) {
		if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) {
			if( pReceiver->IsPushMouseButton(0) ) {
				MatrixEx *pCamera = CRenderBase::GetInstance().GetLastUpdateCamera();
				MatrixEx Cross;

				Cross.m_vPosition = m_pSignal->GetProperty(5)->GetVariableVector3();
				Cross.m_vXAxis = pCamera->m_vXAxis;
				Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
				EtVec3Cross( &Cross.m_vZAxis, &Cross.m_vXAxis, &Cross.m_vYAxis );
				Cross.MoveLocalXAxis( pReceiver->GetMouseVariation().x );
				Cross.MoveLocalZAxis( -pReceiver->GetMouseVariation().y );

				m_pSignal->GetProperty(5)->SetVariable( Cross.m_vPosition );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 5, (LPARAM)m_pSignal->GetProperty(5) );
			}
			else if( pReceiver->IsPushMouseButton(1) ) {
				EtVector3 vPos = m_pSignal->GetProperty(5)->GetVariableVector3();
				vPos.y += -pReceiver->GetMouseVariation().y;
				m_pSignal->GetProperty(5)->SetVariable( vPos );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 5, (LPARAM)m_pSignal->GetProperty(5) );
			}
		}
		float fValue = pReceiver->GetMouseVariation().x - pReceiver->GetMouseVariation().y;
		if( GetAsyncKeyState( VK_LMENU ) & 0x8000 ) {
			if( pReceiver->IsPushMouseButton(0) ) {
				float fTemp = m_pSignal->GetProperty(0)->GetVariableFloat() + fValue;
				if( fTemp <= m_pSignal->GetProperty(6)->GetVariableFloat() )
					fTemp = m_pSignal->GetProperty(6)->GetVariableFloat();

				m_pSignal->GetProperty(0)->SetVariable( fTemp );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 0, (LPARAM)m_pSignal->GetProperty(0) );

				OnModify();
			}
			else if( pReceiver->IsPushMouseButton(1) ) {
				float fTemp = m_pSignal->GetProperty(6)->GetVariableFloat() + fValue;
				if( fTemp >= m_pSignal->GetProperty(0)->GetVariableFloat() )
					fTemp = m_pSignal->GetProperty(0)->GetVariableFloat();

				m_pSignal->GetProperty(3)->SetVariable( fTemp );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 6, (LPARAM)m_pSignal->GetProperty(6) );

				OnModify();
			}
			else if( pReceiver->IsPushMouseButton(2) ) {
				float fTemp = m_pSignal->GetProperty(7)->GetVariableFloat() + fValue;
				if( fTemp < 0.f ) fTemp = 0.f;
				if( fTemp >= 180.f ) fTemp = 180.f;
				m_pSignal->GetProperty(7)->SetVariable( fTemp );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 7, (LPARAM)m_pSignal->GetProperty(7) );

				OnModify();
			}
		}
		if( GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) {
			if( pReceiver->IsPushMouseButton(0) ) {
				float fTemp = m_pSignal->GetProperty(9)->GetVariableFloat() + fValue;
				if( fTemp <= m_pSignal->GetProperty(10)->GetVariableFloat() )
					fTemp = m_pSignal->GetProperty(10)->GetVariableFloat();

				m_pSignal->GetProperty(9)->SetVariable( fTemp );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 9, (LPARAM)m_pSignal->GetProperty(9) );

				OnModify();
			}
			else if( pReceiver->IsPushMouseButton(1) ) {
				float fTemp = m_pSignal->GetProperty(10)->GetVariableFloat() + fValue;
				if( fTemp >= m_pSignal->GetProperty(9)->GetVariableFloat() )
					fTemp = m_pSignal->GetProperty(9)->GetVariableFloat();

				m_pSignal->GetProperty(10)->SetVariable( fTemp );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 10, (LPARAM)m_pSignal->GetProperty(10) );

				OnModify();
			}
			else if( pReceiver->IsPushMouseButton(2) ) {
				float fTemp = m_pSignal->GetProperty(8)->GetVariableFloat() + fValue;
				if( fTemp < 0.f ) fTemp = 0.f;
				if( fTemp >= 360.f ) fTemp = 360.f;
				m_pSignal->GetProperty(8)->SetVariable( fTemp );

				CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
				pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 8, (LPARAM)m_pSignal->GetProperty(8) );

				OnModify();
			}
		}
	}
}
