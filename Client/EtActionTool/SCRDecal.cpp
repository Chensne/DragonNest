#include "StdAfx.h"
#include "SCRDecal.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRDecal::CSCRDecal( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCRDecal::~CSCRDecal()
{
}

void CSCRDecal::Initialize()
{
	m_bActivate = true;
	OnModify();
}

void CSCRDecal::Release()
{
	if( m_hTexture ) {
		SAFE_RELEASE_SPTR( m_hTexture );
	}
	m_szPrevFileName = "";
}

void CSCRDecal::OnModify()
{
	CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
	CString szFileName = pVariable->GetVariableString();

	if( m_szPrevFileName != szFileName ) {
		SAFE_RELEASE_SPTR( m_hTexture );
		m_hTexture = LoadResource(szFileName, RT_TEXTURE);
		m_szPrevFileName = szFileName;
	}

	//m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
	//m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
}

void CSCRDecal::OnSelect( bool bFirst )
{
	if( bFirst && m_hTexture ) {
		CEtDecal *pDecal = new CEtDecal;
		MatrixEx Cross = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
		Cross.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
		pDecal->Initialize( m_hTexture, Cross.m_vPosition.x, Cross.m_vPosition.z, m_pSignal->GetProperty(1)->GetVariableFloat(),
			m_pSignal->GetProperty(3)->GetVariableFloat(), m_pSignal->GetProperty(8)->GetVariableFloat(),  m_pSignal->GetProperty(4)->GetVariableFloat(), EtColor(1,1,1,1), m_pSignal->GetProperty(6)->GetVariableInt(), m_pSignal->GetProperty(7)->GetVariableInt() );
	}

	EtVector3 vPos[2];
	float fSize = 30.f;

	CUnionValueProperty *pVariable = m_pSignal->GetProperty(2);
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
}

void CSCRDecal::OnUnSelect()
{
}

void CSCRDecal::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	CEtDecal *pDecal = new CEtDecal;
	MatrixEx Cross = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
	Cross.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
	pDecal->Initialize( m_hTexture, Cross.m_vPosition.x, Cross.m_vPosition.z, m_pSignal->GetProperty(1)->GetVariableFloat(), 
		m_pSignal->GetProperty(3)->GetVariableFloat(), m_pSignal->GetProperty(8)->GetVariableFloat(), m_pSignal->GetProperty(4)->GetVariableFloat(), EtColor(1,1,1,1), m_pSignal->GetProperty(6)->GetVariableInt(), m_pSignal->GetProperty(7)->GetVariableInt());

	//if( m_nFXIndex == -1 ) return;
	/*m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
	EtVector3 vRotate = m_pSignal->GetProperty(5)->GetVariableVector3();
	m_matExWorld.RotateYaw(vRotate.y);
	m_matExWorld.RotateRoll(vRotate.z);
	m_matExWorld.RotatePitch(vRotate.x);
	m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
	m_hFX = EternityEngine::CreateEffectObject( m_nFXIndex, m_matExWorld );*/
}

void CSCRDecal::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
{	
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) return;

	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) {
		if( pReceiver->IsPushMouseButton(0) ) {
			MatrixEx *pCamera = CRenderBase::GetInstance().GetLastUpdateCamera();
			MatrixEx Cross;

			Cross.m_vPosition = m_pSignal->GetProperty(2)->GetVariableVector3();
			Cross.m_vXAxis = pCamera->m_vXAxis;
			Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
			EtVec3Cross( &Cross.m_vZAxis, &Cross.m_vXAxis, &Cross.m_vYAxis );
			Cross.MoveLocalXAxis( pReceiver->GetMouseVariation().x );
			Cross.MoveLocalZAxis( -pReceiver->GetMouseVariation().y );

			m_pSignal->GetProperty(2)->SetVariable( Cross.m_vPosition );

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 2, (LPARAM)m_pSignal->GetProperty(2) );
		}
		else if( pReceiver->IsPushMouseButton(1) ) {
			EtVector3 vPos = m_pSignal->GetProperty(2)->GetVariableVector3();
			vPos.y += -pReceiver->GetMouseVariation().y;
			m_pSignal->GetProperty(2)->SetVariable( vPos );

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 2, (LPARAM)m_pSignal->GetProperty(2) );
		}
	}
}