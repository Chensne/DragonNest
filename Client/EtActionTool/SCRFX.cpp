#include "StdAfx.h"
#include "SCRFX.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRFX::CSCRFX( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
	m_nFXIndex = -1;
}

CSCRFX::~CSCRFX()
{
	if( m_nFXIndex != -1 )
		EternityEngine::DeleteEffectData( m_nFXIndex );
}

void CSCRFX::Initialize()
{
	m_bActivate = true;
	OnModify();

}

void CSCRFX::Release()
{
	if( m_nFXIndex != -1 )
	{
		// fx�� ��ƼŬ�� �޸� �����ʿ� ���°ǰ�...
		//if( m_hFX && m_hFX->IsShow() )
		//{
		//	m_hFX->Show( false );
		//	EternityEngine::DeleteEffectData( m_nFXIndex );
		//}
		EternityEngine::DeleteEffectData( m_nFXIndex );
	}
	m_nFXIndex = -1;
	m_szPrevFileName = "";
}

void CSCRFX::OnModify()
{
	CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
	CString szFileName = pVariable->GetVariableString();

	if( m_szPrevFileName != szFileName ) {
		if( m_nFXIndex != -1 ) 
			EternityEngine::DeleteEffectData( m_nFXIndex );

		m_nFXIndex = EternityEngine::LoadEffectData( szFileName );
		m_szPrevFileName = szFileName;
	}

	m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
	EtVector3 vRotate = m_pSignal->GetProperty(5)->GetVariableVector3();
	m_matExWorld.RotateYaw(vRotate.y);
	m_matExWorld.RotateRoll(vRotate.z);
	m_matExWorld.RotatePitch(vRotate.x);

	m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
}

void CSCRFX::OnSelect( bool bFirst )
{
	if( bFirst && m_nFXIndex != -1 ) {
		char *szBoneName = m_pSignal->GetProperty(6)->GetVariableString();
		/*
		if(szBoneName && strlen(szBoneName)>0) {
			int nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
			if( nBoneIndex != -1 ) {
				m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
			}
		}
		else {*/
			m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
		//}
		EtVector3 vRotate = m_pSignal->GetProperty(5)->GetVariableVector3();		
		EtVector3 vPosition = m_pSignal->GetProperty(2)->GetVariableVector3();

		int nBoneIndex = -1;
		if(szBoneName && strlen(szBoneName)>0) {
			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance();
			if( ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle() )
				nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
			//if( nBoneIndex != -1 ) {
				/*EtMatrix BoneMat = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
				m_matExWorld.m_vPosition += EtVector3(BoneMat._41, BoneMat._42, BoneMat._43);*/
			//}
		}
		else {
			m_matExWorld.RotateYaw(vRotate.y);
			m_matExWorld.RotateRoll(vRotate.z);
			m_matExWorld.RotatePitch(vRotate.x);
			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
		}
		m_hFX = EternityEngine::CreateEffectObject( m_nFXIndex, m_matExWorld );
		m_hFX->SetScale( m_pSignal->GetProperty(7)->GetVariableFloat() + 1.f );
		if( nBoneIndex != -1 ) {
			m_hFX->SetLink( *m_matExWorld, szBoneName, vPosition, vRotate, ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle() );
		}
	}

	EtVector3 vPos[2];
	float fSize = 30.f;

	CUnionValueProperty *pVariable = m_pSignal->GetProperty(2);
	
	MatrixEx Cross;
	char *szBoneName = m_pSignal->GetProperty(6)->GetVariableString();
	int nBoneIndex = -1;
	if(szBoneName && strlen(szBoneName)>0) {
		if( ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle() )
			nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
	}
	if( nBoneIndex != -1 ) {
		EtMatrix BoneMat = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
		Cross = BoneMat;
	}

	EtVector3 vDist = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + 
		Cross.m_vXAxis * pVariable->GetVariableVector3().x +
		Cross.m_vYAxis * pVariable->GetVariableVector3().y +
		Cross.m_vZAxis * pVariable->GetVariableVector3().z;

	vPos[0] = Cross.m_vZAxis*fSize + vDist;
	vPos[1] = Cross.m_vZAxis*-fSize + vDist;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );

	vPos[0] = Cross.m_vYAxis*fSize + vDist;
	vPos[1] = Cross.m_vYAxis*-fSize + vDist;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );

	vPos[0] = Cross.m_vXAxis*fSize + vDist;
	vPos[1] = Cross.m_vXAxis*-fSize + vDist;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xFFFFFF00 );
}

void CSCRFX::OnUnSelect()
{
}

void CSCRFX::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nFXIndex == -1 ) return;
	char *szBoneName = m_pSignal->GetProperty(6)->GetVariableString();
	/*
	if(szBoneName && strlen(szBoneName)>0) {
		int nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
		if( nBoneIndex != -1 ) {
			m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
		}
	}
	else {*/
	m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
	//}
	EtVector3 vRotate = m_pSignal->GetProperty(5)->GetVariableVector3();
	EtVector3 vPosition = m_pSignal->GetProperty(2)->GetVariableVector3();

	int nBoneIndex = -1;
	if(szBoneName && strlen(szBoneName)>0) {
		//m_matExWorld.m_vPosition = /*((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() +*/ m_pSignal->GetProperty(2)->GetVariableVector3();			
		m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance();
		if( ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle() )
			nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
		//if( nBoneIndex != -1 ) {
			/*EtMatrix BoneMat = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
			m_matExWorld.m_vPosition += EtVector3(BoneMat._41, BoneMat._42, BoneMat._43);*/
		//}
	}
	else {
		m_matExWorld.RotateYaw(vRotate.y);
		m_matExWorld.RotateRoll(vRotate.z);
		m_matExWorld.RotatePitch(vRotate.x);
		m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(2)->GetVariableVector3();
	}
	m_hFX = EternityEngine::CreateEffectObject( m_nFXIndex, m_matExWorld );
	m_hFX->SetScale( m_pSignal->GetProperty(7)->GetVariableFloat() + 1.f );

	if( nBoneIndex != -1 ) {
		m_hFX->SetLink( *m_matExWorld, szBoneName, vPosition, vRotate, ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle() );
	}
}

void CSCRFX::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
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