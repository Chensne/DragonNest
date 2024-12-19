#include "StdAfx.h"
#include "SCREnvironmentEffect.h"
#include "ActionSignal.h"
#include "ActionObject.h"


CSCREnvironmentEffect::CSCREnvironmentEffect( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
}

CSCREnvironmentEffect::~CSCREnvironmentEffect()
{
}

void CSCREnvironmentEffect::Initialize()
{
	m_bActivate = true;
	OnModify();

}

void CSCREnvironmentEffect::OnModify()
{
	m_matExWorld.Identity();
	m_matExWorld = *((CActionObject*)m_pSignal->GetParent()->GetParent())->GetMatEx();

	m_matExWorld.m_vPosition = m_pSignal->GetProperty(2)->GetVariableVector3();

	/*
	for( int i=0; i<2; i++ ) m_vVecList[i].clear();
	float fRadian = m_pSignal->GetProperty(4)->GetVariableFloat();
	float fRollOff = m_pSignal->GetProperty(5)->GetVariableFloat();

	float fCurAngle = 0.f;
	EtVector3 vPos;

	for( DWORD i=0;; i++ ) {
		vPos.x = cos( EtToRadian( fCurAngle ) ) * fRadian;
		vPos.z = sin( EtToRadian( fCurAngle ) ) * fRadian;
		vPos.y = 0.f;
		m_vVecList[0].push_back( vPos );

		vPos.x = cos( EtToRadian( fCurAngle ) ) * ( fRadian * fRollOff );
		vPos.z = sin( EtToRadian( fCurAngle ) ) * ( fRadian * fRollOff );
		m_vVecList[1].push_back( vPos );

		if( fCurAngle >= 360.f ) break;
		fCurAngle += 5.f;
	}
	*/
}

void CSCREnvironmentEffect::OnSelect( bool bFirst )
{
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


	/*
	if( m_vVecList[0].size() > 0 ) {
		for( DWORD i=0; i<m_vVecList[0].size()-1; i++ ) {
			EternityEngine::DrawLine3D( m_vVecList[0][i] + vDist, m_vVecList[0][i+1] + vDist, 0xFFFFFF00 ); 
			EternityEngine::DrawLine3D( m_vVecList[1][i] + vDist, m_vVecList[1][i+1] + vDist, 0xFFFFFF00 );
		}
	}
	*/
}

void CSCREnvironmentEffect::OnUnSelect()
{
}

void CSCREnvironmentEffect::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
}
