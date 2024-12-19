#include "StdAfx.h"

#include "ActionSignal.h"
#include "ActionObject.h"

#include "CSCRRotate.h"


// Override - CSignalCustomRender
CSignalCustomRender * CSCRRotate::Clone()
{ 
	return new CSCRRotate( m_szSignalName.c_str() );
}

void CSCRRotate::Initialize()
{
	m_LocalTime = 0;
	m_bActivate = true;
}

void CSCRRotate::Release()
{
}

void CSCRRotate::OnSelect( bool bFirst )
{
	EtVector3 vPos[2];
	float fSize = 30.f;
	
	EtVector3 vAxis = (EtVector3)m_pSignal->GetProperty(3)->GetVariableVector3();

	vPos[0] = EtVector3( 0.f, 0.f, fSize ) + vAxis;
	vPos[1] = EtVector3( 0.f, 0.f, -fSize ) + vAxis;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );

	vPos[0] = EtVector3( 0.f, fSize, 0.f ) + vAxis;
	vPos[1] = EtVector3( 0.f, -fSize, 0.f ) + vAxis;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );

	vPos[0] = EtVector3( fSize, 0.f, 0.f ) + vAxis;
	vPos[1] = EtVector3( -fSize, 0.f, 0.f ) + vAxis;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );



	EtVector3 vAni = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance();
	CString szFrame;	
	szFrame.Format( "%.1f, %.1f, %.1f", vAni.x, vAni.y, vAni.z );
	EternityEngine::DrawText2D( EtVector2( 0.f, 50.f ), szFrame );
}

void CSCRRotate::OnUnSelect()
{
}

void CSCRRotate::OnModify()
{
}

void CSCRRotate::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	//fDelta = ( LocalTime - m_LocalTime ) * 0.001f;
	//m_LocalTime = LocalTime;

	float fStartSpeed = m_pSignal->GetProperty(0)->GetVariableFloat();
	float fEndSpeed = m_pSignal->GetProperty(1)->GetVariableFloat();
	bool bLeft = m_pSignal->GetProperty(2)->GetVariableBool();
	EtVector3 vAxis = (EtVector3)m_pSignal->GetProperty(3)->GetVariableVector3();
	
	CActionObject *pObject = (CActionObject *)m_pSignal->GetParent()->GetParent();
	EtAniObjectHandle hHandle = pObject->GetObjectHandle();
	
	float fSpeed = 0.0f;

	// �ӵ�( ȸ������ )
	if( fStartSpeed == fEndSpeed )
	{
		fSpeed = fStartSpeed;
	}
	else
	{
		float frame = pObject->GetCurFrame();
		float w = ( frame - m_pSignal->GetStartFrame() ) / ( m_pSignal->GetEndFrame() - m_pSignal->GetStartFrame() );
		//float w = 1.f / ( m_pSignal->GetEndFrame() - m_pSignal->GetStartFrame() ) * ( frame - m_pSignal->GetStartFrame() );
		fSpeed = fStartSpeed + ( ( fEndSpeed - fStartSpeed ) * w );
	}

	// ����.
	if( bLeft )
		fSpeed = -fSpeed;

	pObject->SetRotationSpeed( fSpeed );
	pObject->SetAxisPosition( vAxis );	


	//fSpeed *= fDelta;

	//CString szFrame;
	////szFrame.Format( "_____________Speed : %.2f,  Time : %.4f", fSpeed, fDelta );
	//szFrame.Format( "_____________Frame : %.2f,  Time : %.2f", frame, m_pSignal->GetEndFrame() );
	//EternityEngine::DrawText2D( EtVector2( 0.f, 0.f ), szFrame );

	//pObject->RotateObject( EtToRadian( fSpeed ), vAxis, true ); 
	
	//pObject->RotateObject( fSpeed, vAxis, true ); 

	//					   //(frame < m_pSignal->GetEndFrame()) );

	//if( frame >= m_pSignal->GetEndFrame() )
	//	fSpeed = 0.0f;

	//pObject->SetRotationSpeed( fSpeed );

	//EtMatrix mat;
	//EtMatrixRotationY( &mat, fSpeed );
	//EtMatrixMultiply( &mat, &mat, hHandle->GetWorldMat() );
	//hHandle->Update( &mat );

}

void CSCRRotate::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
{
	
}


void CSCRRotate::OnStartSignal()
{
	CActionObject *pObject = (CActionObject *)m_pSignal->GetParent()->GetParent();
	pObject->SetRotate( true );	
}

void CSCRRotate::OnEndSignal()
{
	CActionObject *pObject = (CActionObject *)m_pSignal->GetParent()->GetParent();
	pObject->SetRotate( false );
}