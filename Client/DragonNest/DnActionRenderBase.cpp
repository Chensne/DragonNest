#include "StdAfx.h"
#include "DnActionRenderBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnActionRenderBase::CDnActionRenderBase()
: m_vAniDistance( 0.f, 0.f, 0.f )
, m_vScale( 1.f, 1.f, 1.f )
{
	m_nAniIndex = -1;
	m_nBlendAniIndex = -1;
	m_fBlendAniFrame = 0.f;
	m_fBlendFrame = 0.f;
	m_fBlendStartFrame = 0.f;
	m_fFrame = m_fPrevFrame = 0.f;
	m_AniTime = 0;
	m_fFps = s_fDefaultFps;
	m_bShow = false;

	CDnActionBase::Initialize( this );
}

CDnActionRenderBase::~CDnActionRenderBase()
{
	SAFE_RELEASE_SPTR( m_hObject );
}

bool CDnActionRenderBase::Initialize( const char *szSkinName, const char *szAniName, const char *szActName )
{
	bool bValid[2] = { true, };
	if( szSkinName ) {
		m_hObject = EternityEngine::CreateAniObject( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
		if( m_hObject && m_hObject->GetAniHandle() ) {
			for( int i=0; i<m_hObject->GetAniCount(); i++ ) {
				m_mapAniList.insert( make_pair( m_hObject->GetAniName(i), i ) );
			}
		}
		else bValid[0] = false;
	}
	if( szActName ) {
		bValid[1] = LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );
	}
	if( bValid[0] == false && bValid[1] == false ) return false;

	m_bShow = true;
	return true;
}

void CDnActionRenderBase::ChangeAnimation( int nAniIndex, float fFrame, float fBlendFrame )
{
	m_nBlendAniIndex = m_nAniIndex;
	m_fBlendStartFrame = fFrame;
	m_fBlendFrame = fBlendFrame;
	if( m_fBlendFrame == 0.f ) m_nBlendAniIndex = -1;

	if( m_nBlendAniIndex != -1 ) {
		m_fBlendAniFrame = m_fFrame;

		float fAniLength = (float)m_hObject->GetLastFrame( m_nBlendAniIndex );
		if( m_fBlendAniFrame >= fAniLength ) {
			m_fBlendAniFrame = fAniLength;
		}
	}

	m_nAniIndex = nAniIndex;
	m_AniTime = m_LocalTime - (LOCAL_TIME)( fFrame / m_fFps * 1000.f );
	if( m_AniTime < 0 ) m_AniTime = 0;
	m_fFrame = fFrame;
	m_fPrevFrame = fFrame;
}

void CDnActionRenderBase::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hObject ) return;

	EtMatrix matWorld;
	EtMatrix matScale;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &matWorld, &matScale, pmatWorld );

	m_hObject->Update( &matWorld );
}

int CDnActionRenderBase::GetAniIndex( const char *szAniName )
{
	std::map<std::string, int>::iterator it = m_mapAniList.find( szAniName );
	if( it == m_mapAniList.end() ) return -1;
	return it->second;
}

EtMatrix CDnActionRenderBase::GetBoneMatrix( const char *szBoneName, bool *bExistBone )
{
	if( bExistBone ) *bExistBone = false;
	EtMatrix mat;
	EtMatrixIdentity( &mat );
	if( !m_hObject || !m_hObject->GetAniHandle() ) {		
		return mat;
	}

	int nBoneIndex = m_hObject->GetAniHandle()->GetBoneIndex( szBoneName );
	if( nBoneIndex == -1 ) return mat;
	if( m_hObject->GetSaveMatIndex() == -1 ) return mat;
	EtMatrixMultiply( &mat, m_hObject->GetBoneTransMat( nBoneIndex ), m_hObject->GetWorldMat() );
	if( bExistBone ) *bExistBone = true;
	return mat;
}

int CDnActionRenderBase::GetBoneIndex( const char *szBoneName )
{
	if( !m_hObject || !m_hObject->GetAniHandle() ) return -1;
	return m_hObject->GetAniHandle()->GetBoneIndex( szBoneName );
}

int CDnActionRenderBase::GetDummyBoneIndex( const char *szBoneName )
{
	if( !m_hObject || !m_hObject->GetMesh() ) return -1;
	return m_hObject->GetMesh()->FindDummy( szBoneName );
}

void CDnActionRenderBase::SetFPS( float fValue )
{
	m_fFps = fValue;
}

float CDnActionRenderBase::GetFPS()
{
	return m_fFps;
}

void CDnActionRenderBase::ShowRenderBase( bool bShow )
{
	m_bShow = bShow;

	if( m_hObject ) 
		m_hObject->ShowObject( m_bShow );
}

bool CDnActionRenderBase::IsShow()
{
	return m_bShow;
}

void CDnActionRenderBase::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActionBase::ProcessAction( LocalTime, fDelta );

	if( m_nAniIndex != -1 ) {
		m_fFrame = ( ( m_LocalTime - m_AniTime ) / 1000.f ) * m_fFps;

		float fAniLength = (float)m_hObject->GetLastFrame( m_nAniIndex );

		if( m_fFrame < 0.f ) m_fFrame = fAniLength;

		if( m_fFrame >= fAniLength ) {
			m_fFrame = fAniLength;
		}
		if( m_hObject ) m_hObject->SetAniFrame( m_nAniIndex, m_fFrame );

		if( m_nBlendAniIndex != -1 ) {
			if( m_fFrame - m_fBlendStartFrame < m_fBlendFrame ) {
				float fWeight = 1.f - ( 1.f / m_fBlendFrame * ( m_fFrame - m_fBlendStartFrame ) );
				m_hObject->BlendAniFrame( m_nBlendAniIndex, m_fBlendAniFrame, fWeight );
			}
			else {
				m_nBlendAniIndex = -1;
			}
		}
		// Calc Ani Distance
		if( m_hObject ) m_hObject->CalcAniDistance( m_nAniIndex, m_fFrame, m_fPrevFrame, m_vAniDistance );
	}
	m_fPrevFrame = m_fFrame;


	Process( m_matExWorld, LocalTime, fDelta );
}

void CDnActionRenderBase::SetCalcPositionFlag( int nFlag )
{
	if( m_hObject ) m_hObject->SetCalcPositionFlag( nFlag );
}
