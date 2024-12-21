#include "StdAfx.h"
#include "SCRParticle.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRParticle::CSCRParticle( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
	m_nParticleIndex = -1;
}

CSCRParticle::~CSCRParticle()
{
	if( m_nParticleIndex != -1 )
		EternityEngine::DeleteParticleData( m_nParticleIndex );
//	SAFE_RELEASE_SPTR( m_hParticle );
}

void CSCRParticle::Initialize()
{
	m_bActivate = true;
	m_bLoop = false;
	m_bIterator = false;
	OnModify();

}

void CSCRParticle::Release()
{
	if( m_nParticleIndex != -1 )
	{
		if( m_hParticle && m_hParticle->IsPlay() )
		{
			m_hParticle->Show( false );
			m_hParticle->StopPlay();
			EternityEngine::DeleteParticleData( m_nParticleIndex );
		}
		EternityEngine::DeleteParticleData( m_nParticleIndex );
	}
	m_nParticleIndex = -1;
	m_szPrevFileName = "";
}

void CSCRParticle::OnModify()
{
	CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
	CString szFileName = pVariable->GetVariableString();

	if( m_szPrevFileName != szFileName ) {
		if( m_nParticleIndex != -1 ) 
			EternityEngine::DeleteParticleData( m_nParticleIndex );

		m_nParticleIndex = EternityEngine::LoadParticleData( szFileName );
		m_szPrevFileName = szFileName;
	}

	
	m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
	EtVector3 vRotate = m_pSignal->GetProperty(8)->GetVariableVector3();
	m_matExWorld.RotateYaw(vRotate.y);
	m_matExWorld.RotateRoll(vRotate.z);
	m_matExWorld.RotatePitch(vRotate.x);	
	m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(3)->GetVariableVector3();

	m_bLoop = m_pSignal->GetProperty(1)->GetVariableBool();
	m_bIterator = m_pSignal->GetProperty(2)->GetVariableBool();
}

void CSCRParticle::OnSelect( bool bFirst )
{
	if( bFirst && m_nParticleIndex != -1 ) {
		char *szBoneName = m_pSignal->GetProperty(9)->GetVariableString();
		if(szBoneName && strlen(szBoneName)>0) {
			int nBoneIndex = -1;
			if( ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle() )
				nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
			if( nBoneIndex != -1 ) {
				EtMatrix *pMatrix = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
				if( pMatrix ) {
					m_matExWorld = *pMatrix;
				}
				else {
					MessageBox(0, "�׷� ���̸��� ���׿�~", "����", MB_OK );
				}
			}
		}
		else {
			m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
		}
		EtVector3 vRotate = m_pSignal->GetProperty(8)->GetVariableVector3();
		m_matExWorld.RotateYaw(vRotate.y);
		m_matExWorld.RotateRoll(vRotate.z);
		m_matExWorld.RotatePitch(vRotate.x);
		if(szBoneName && strlen(szBoneName)>0) {
			m_matExWorld.m_vPosition = /*((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() +*/ m_pSignal->GetProperty(3)->GetVariableVector3();			
			int nBoneIndex = -1;
			if( ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle() )
				nBoneIndex = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetAniHandle()->GetBoneIndex( szBoneName );
			if( nBoneIndex != -1 ) {
				EtMatrix *BoneMat = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle()->GetBoneTransMat(nBoneIndex);
				if( BoneMat ) {
					m_matExWorld.m_vPosition += EtVector3(BoneMat->_41, BoneMat->_42, BoneMat->_43);
				}
				else {
					MessageBox(0, "�׷� ���̸��� ���׿�~", "����", MB_OK );
				}
			}
		}
		else {
			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(3)->GetVariableVector3();
		}
		float fScale = m_pSignal->GetProperty(10)->GetVariableFloat() + 1.f;
		m_hParticle = EternityEngine::CreateBillboardEffect( m_nParticleIndex, m_matExWorld );
		if( m_hParticle ) {
			m_hParticle->EnableTracePos( m_bIterator );
			m_hParticle->SetCullDist( 1000000.f );
			m_hParticle->SetScale( fScale );
		}
	}

	EtVector3 vPos[2];
	float fSize = 30.f;

	CUnionValueProperty *pVariable = m_pSignal->GetProperty(3);
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

void CSCRParticle::OnUnSelect()
{
}

void CSCRParticle::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nParticleIndex == -1 ) return;

	char *szBoneName = m_pSignal->GetProperty(9)->GetVariableString();
	char szTempStr[32] = {0,};
	if( szBoneName == NULL ) szBoneName = szTempStr;	// Ư�� ��ǻ�Ϳ����� NULL�� ���͹��� ���� �״´ٰ� �ϳ׿�.. �˻��صӴϴ�.
	CActionObject *pActionObject = ( ( CActionObject * )m_pSignal->GetParent()->GetParent() );
	EtAniObjectHandle hParent = pActionObject->GetObjectHandle();
	int nLinkBoneIndex = -1;
	int nLinkType = CEtObject::LT_BONE;
	if( hParent )
	{
		EtMeshHandle hMesh = hParent->GetMesh();
		if( hMesh )
		{
			nLinkBoneIndex = hMesh->FindDummy( szBoneName );
			if( nLinkBoneIndex != -1 )
			{
				nLinkType = CEtObject::LT_MESH_DUMMY;
			}
			else
			{
				nLinkBoneIndex = hMesh->FindBone( szBoneName );
			}
		}
	}
	if( nLinkBoneIndex != -1 )
	{
		EtMatrix *pMatrix = NULL;
		if( nLinkType == CEtObject::LT_BONE )
		{
			pMatrix = hParent->GetBoneTransMat( nLinkBoneIndex );
		}
		else
		{
			pMatrix = hParent->GetDummyTransMat( nLinkBoneIndex );
		}
		if( pMatrix )
		{
			m_matExWorld = *pMatrix;
		}
		else
		{
			MessageBox(0, "�׷� ���̸��� ���׿�~", "����", MB_OK );
		}
	}
	else {
		m_matExWorld = *pActionObject->GetMatEx();
	}
	EtVector3 vRotate = m_pSignal->GetProperty(8)->GetVariableVector3();
	m_matExWorld.RotateYaw(vRotate.y);
	m_matExWorld.RotateRoll(vRotate.z);
	m_matExWorld.RotatePitch(vRotate.x);

	if( nLinkBoneIndex != -1 )
	{
		m_matExWorld.m_vPosition = m_pSignal->GetProperty(3)->GetVariableVector3();			
		EtMatrix *pMatrix = NULL;
		if( nLinkType == CEtObject::LT_BONE )
		{
			pMatrix = hParent->GetBoneTransMat( nLinkBoneIndex );
		}
		else
		{
			pMatrix = hParent->GetDummyTransMat( nLinkBoneIndex );
		}
		if( pMatrix )
		{
			m_matExWorld = *pMatrix;
		}
		else
		{
			MessageBox(0, "�׷� ���̸��� ���׿�~", "����", MB_OK );
		}
	}
	else {
		m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(3)->GetVariableVector3();
	}
	float fScale = m_pSignal->GetProperty(10)->GetVariableFloat() + 1.f;
	m_hParticle = EternityEngine::CreateBillboardEffect( m_nParticleIndex, m_matExWorld );
	if( m_hParticle ) {
		m_hParticle->EnableTracePos( m_bIterator );
		m_hParticle->SetCullDist( 1000000.f );
		m_hParticle->SetScale( fScale );
	}
	/*	if(szBoneName && strlen(szBoneName)>0) {
			m_hParticle->SetLink( *m_matExWorld,
						false, ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx()->GetPosition().y,
						szBoneName, m_matExWorld.GetPosition(), EtVector3(0,0,0), ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetObjectHandle() );
		}*/
	//}
}

void CSCRParticle::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
{
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) return;

	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) {
		if( pReceiver->IsPushMouseButton(0) ) {
			MatrixEx *pCamera = CRenderBase::GetInstance().GetLastUpdateCamera();
			MatrixEx Cross;
			Cross.m_vPosition = m_pSignal->GetProperty(3)->GetVariableVector3();
			Cross.m_vXAxis = pCamera->m_vXAxis;
			Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
			EtVec3Cross( &Cross.m_vZAxis, &Cross.m_vXAxis, &Cross.m_vYAxis );
			Cross.MoveLocalXAxis( pReceiver->GetMouseVariation().x );
			Cross.MoveLocalZAxis( -pReceiver->GetMouseVariation().y );

			m_pSignal->GetProperty(3)->SetVariable( Cross.m_vPosition );

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 3, (LPARAM)m_pSignal->GetProperty(3) );
		}
		else if( pReceiver->IsPushMouseButton(1) ) {
			EtVector3 vPos = m_pSignal->GetProperty(3)->GetVariableVector3();
			vPos.y += -pReceiver->GetMouseVariation().y;
			m_pSignal->GetProperty(3)->SetVariable( vPos );

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 3, (LPARAM)m_pSignal->GetProperty(3) );
		}

	}
}