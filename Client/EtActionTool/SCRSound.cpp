#include "StdAfx.h"
#include "SCRSound.h"
#include "ActionSignal.h"
#include "ActionObject.h"


CSCRSound::CSCRSound( const char *szSignalName )
: CSignalCustomRender( szSignalName )
{
	m_nSoundIndex = -1;
}

CSCRSound::~CSCRSound()
{
	if( m_nSoundIndex != -1 ) {
		CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndex );
	}
}

void CSCRSound::Initialize()
{
	m_bActivate = true;
	/*
	CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
	CString szFileName = pVariable->GetVariableString();
	m_szPrevFileName = szFileName;
	OnModify();

	if( szFileName.IsEmpty() ) return;
	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( szFileName, false, false );
	if( m_nSoundIndex == -1 ) return;
	*/
	OnModify();
}

void CSCRSound::OnModify()
{
	CUnionValueProperty *pVariable = m_pSignal->GetProperty(0);
	CString szFileName = pVariable->GetVariableString();

	if( m_szPrevFileName != szFileName ) {
		if( m_hChannel ) {
			CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
			m_hChannel.Identity();
		}
		if( m_nSoundIndex != -1 ) {
			CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndex );
			m_nSoundIndex = -1;
		}
		if( !szFileName.IsEmpty() ) {
			m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( szFileName, false, false );
		}
		m_szPrevFileName = szFileName;
	}

	for( int i=0; i<2; i++ ) m_vVecList[i].clear();
	float fRadian = m_pSignal->GetProperty(2)->GetVariableFloat();
	float fRollOff = m_pSignal->GetProperty(3)->GetVariableFloat();

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


}

void CSCRSound::OnSelect( bool bFirst )
{
	if( m_nSoundIndex == -1 ) return;
	if( bFirst && !m_hChannel ) {
		m_hChannel = CEtSoundEngine::GetInstance().PlaySound( NULL, m_nSoundIndex );
		if( m_hChannel ) {
			CUnionValueProperty *pVariable = m_pSignal->GetProperty(1);
			m_hChannel->SetVolume( pVariable->GetVariableFloat() );
		}
	}
	if( m_vVecList[0].size() > 0 ) {
		EtVector3 vDist = ((CActionObject*)m_pSignal->GetParent()->GetParent())->GetAniDistance();
		for( DWORD i=0; i<m_vVecList[0].size()-1; i++ ) {
			EternityEngine::DrawLine3D( m_vVecList[0][i] + vDist, m_vVecList[0][i+1] + vDist, 0xFFFFFF00 ); 
			EternityEngine::DrawLine3D( m_vVecList[1][i] + vDist, m_vVecList[1][i+1] + vDist, 0xFFFFFF00 );
		}
	}
}

void CSCRSound::OnUnSelect()
{
	if( m_hChannel )
		CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
	m_hChannel.Identity();
}

void CSCRSound::OnProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nSoundIndex == -1 ) return;
	if( m_hChannel ) CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
	m_hChannel = CEtSoundEngine::GetInstance().PlaySound( NULL, m_nSoundIndex, false, false );
	if( m_hChannel ) {
		CUnionValueProperty *pVariable = m_pSignal->GetProperty(1);
		m_hChannel->SetVolume( pVariable->GetVariableFloat() );
	}

}
