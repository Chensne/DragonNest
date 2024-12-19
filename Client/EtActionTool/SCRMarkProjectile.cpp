#include "StdAfx.h"
#include "SCRMarkProjectile.h"
#include "ActionSignal.h"
#include "ActionObject.h"
#include "InputReceiver.h"
#include "RenderBase.h"
#include "resource.h"
#include "MainFrm.h"
#include "UserMessage.h"
#include "PaneDefine.h"


CSCRMarkProjectile::CSCRMarkProjectile( const char *szSignalName )
: CSignalCustomRender( szSignalName )
, m_eMarkType( MARK_NONE )
, m_nFXIndex( -1 )
, m_nParticleIndex( -1 )
{
}

CSCRMarkProjectile::~CSCRMarkProjectile()
{
}

void CSCRMarkProjectile::Initialize()
{
	m_bActivate = true;
	OnModify();
}

void CSCRMarkProjectile::Release()
{
	switch( m_eMarkType )
	{
	case MARK_DECAL:
		{
			if( m_hTexture )
				SAFE_RELEASE_SPTR( m_hTexture );
		}
		break;
	case MARK_FX:
		{
			if( m_nFXIndex != -1 )
				EternityEngine::DeleteEffectData( m_nFXIndex );
			m_nFXIndex = -1;
		}
		break;
	case MARK_PARTICLE:
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
		}
		break;
	}
	
	m_szPrevFileName = "";
}

void CSCRMarkProjectile::OnModify()
{
	CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
	CString szFileName = pVariable->GetVariableString();

	m_eMarkType = static_cast<eMarkType>( m_pSignal->GetProperty(8)->GetVariableInt() );

	switch( m_eMarkType )
	{
	case MARK_DECAL:
		{
			if( m_szPrevFileName != szFileName ) 
			{
				SAFE_RELEASE_SPTR( m_hTexture );
				m_hTexture = LoadResource(szFileName, RT_TEXTURE);
				m_szPrevFileName = szFileName;
			}
		}
		break;
	case MARK_FX:
		{
			if( m_szPrevFileName != szFileName ) 
			{
				if( m_nFXIndex != -1 ) 
					EternityEngine::DeleteEffectData( m_nFXIndex );

				m_nFXIndex = EternityEngine::LoadEffectData( szFileName );
				m_szPrevFileName = szFileName;
			}

			m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
		}
		break;
	case MARK_PARTICLE:
		{
			CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
			CString szFileName = pVariable->GetVariableString();

			if( m_szPrevFileName != szFileName ) 
			{
				if( m_nParticleIndex != -1 ) 
					EternityEngine::DeleteParticleData( m_nParticleIndex );

				m_nParticleIndex = EternityEngine::LoadParticleData( szFileName );
				m_szPrevFileName = szFileName;
			}

			m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
		}
		break;
	}
}

void CSCRMarkProjectile::OnSelect( bool bFirst )
{
	switch( m_eMarkType )
	{
	case MARK_DECAL:
		{
			if( bFirst && m_hTexture ) 
			{
				CEtDecal *pDecal = new CEtDecal;
				MatrixEx Cross = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
				Cross.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
				pDecal->Initialize( m_hTexture, Cross.m_vPosition.x, Cross.m_vPosition.z, m_pSignal->GetProperty(2)->GetVariableFloat(),
					m_pSignal->GetProperty(1)->GetVariableFloat(), m_pSignal->GetProperty(13)->GetVariableFloat(),  m_pSignal->GetProperty(3)->GetVariableFloat(), EtColor(1,1,1,1), m_pSignal->GetProperty(6)->GetVariableInt(), m_pSignal->GetProperty(5)->GetVariableInt() );
			}

			EtVector3 vPos[2];
			float fSize = 30.f;

			CUnionValueProperty *pVariable = m_pSignal->GetProperty(4);
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
		break;
	case MARK_FX:
		{
			if( bFirst && m_nFXIndex != -1 ) 
			{
				m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
				m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
				m_hFX = EternityEngine::CreateEffectObject( m_nFXIndex, m_matExWorld );
				m_hFX->SetScale( m_pSignal->GetProperty(12)->GetVariableFloat() + 1.f );
			}

			EtVector3 vPos[2];
			float fSize = 30.f;

			CUnionValueProperty *pVariable = m_pSignal->GetProperty(4);
			
			MatrixEx Cross;
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
		break;
	case MARK_PARTICLE:
		{
			if( bFirst && m_nParticleIndex != -1 )
			{
				m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
				m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
				float fScale = m_pSignal->GetProperty(12)->GetVariableFloat() + 1.f;
				m_hParticle = EternityEngine::CreateBillboardEffect( m_nParticleIndex, m_matExWorld );
				if( m_hParticle )
				{
					m_hParticle->EnableTracePos( false );
					m_hParticle->SetCullDist( 1000000.f );
					m_hParticle->SetScale( fScale );
				}
			}

			EtVector3 vPos[2];
			float fSize = 30.f;

			CUnionValueProperty *pVariable = m_pSignal->GetProperty(4);
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
		break;
	}
}

void CSCRMarkProjectile::OnUnSelect()
{
}

void CSCRMarkProjectile::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	switch( m_eMarkType )
	{
	case MARK_DECAL:
		{
			CEtDecal *pDecal = new CEtDecal;
			MatrixEx Cross = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();
			Cross.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
			pDecal->Initialize( m_hTexture, Cross.m_vPosition.x, Cross.m_vPosition.z, m_pSignal->GetProperty(2)->GetVariableFloat(), 
				m_pSignal->GetProperty(1)->GetVariableFloat(), m_pSignal->GetProperty(13)->GetVariableFloat(), m_pSignal->GetProperty(3)->GetVariableFloat(), EtColor(1,1,1,1), m_pSignal->GetProperty(6)->GetVariableInt(), m_pSignal->GetProperty(5)->GetVariableInt());
		}
		break;
	case MARK_FX:
		{
			if( m_nFXIndex == -1 ) return;
			m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();

			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();
			m_hFX = EternityEngine::CreateEffectObject( m_nFXIndex, m_matExWorld );
			m_hFX->SetScale( m_pSignal->GetProperty(12)->GetVariableFloat() + 1.f );
		}
		break;
	case MARK_PARTICLE:
		{
			if( m_nParticleIndex == -1 ) return;

			char szTempStr[32] = {0,};
			CActionObject *pActionObject = ( ( CActionObject * )m_pSignal->GetParent()->GetParent() );
			EtAniObjectHandle hParent = pActionObject->GetObjectHandle();
			m_matExWorld = *pActionObject->GetMatEx();
			m_matExWorld.m_vPosition = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance() + m_pSignal->GetProperty(4)->GetVariableVector3();

			float fScale = m_pSignal->GetProperty(12)->GetVariableFloat() + 1.f;
			m_hParticle = EternityEngine::CreateBillboardEffect( m_nParticleIndex, m_matExWorld );
			if( m_hParticle ) 
			{
				m_hParticle->EnableTracePos( false );
				m_hParticle->SetCullDist( 1000000.f );
				m_hParticle->SetScale( fScale );
			}
		}
		break;
	}
}

void CSCRMarkProjectile::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime, CInputReceiver *pReceiver )
{	
	if( GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) return;

	if( nReceiverState & CInputReceiver::IR_MOUSE_MOVE ) 
	{
		if( pReceiver->IsPushMouseButton(0) ) 
		{
			MatrixEx *pCamera = CRenderBase::GetInstance().GetLastUpdateCamera();
			MatrixEx Cross;

			Cross.m_vPosition = m_pSignal->GetProperty(4)->GetVariableVector3();
			Cross.m_vXAxis = pCamera->m_vXAxis;
			Cross.m_vYAxis = EtVector3( 0.f, 1.f, 0.f );
			EtVec3Cross( &Cross.m_vZAxis, &Cross.m_vXAxis, &Cross.m_vYAxis );
			Cross.MoveLocalXAxis( pReceiver->GetMouseVariation().x );
			Cross.MoveLocalZAxis( -pReceiver->GetMouseVariation().y );

			m_pSignal->GetProperty(4)->SetVariable( Cross.m_vPosition );

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 4, (LPARAM)m_pSignal->GetProperty(4) );
		}
		else if( pReceiver->IsPushMouseButton(1) ) 
		{
			EtVector3 vPos = m_pSignal->GetProperty(4)->GetVariableVector3();
			vPos.y += -pReceiver->GetMouseVariation().y;
			m_pSignal->GetProperty(4)->SetVariable( vPos );

			CWnd *pWnd = GetPaneWnd( PROPERTY_PANE );
			pWnd->SendMessage( UM_PROPERTY_PANE_SET_MODIFY, 4, (LPARAM)m_pSignal->GetProperty(4) );
		}
	}
}