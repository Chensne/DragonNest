#include "Stdafx.h"
#include "EtAniKey.h"
#include "MAActorRenderBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MAActorRenderBase::MAActorRenderBase()
: m_vAniDistance( 0.f, 0.f, 0.f )
, m_vScale( 1.f, 1.f, 1.f )
, m_vAddAniDistance( 0.f, 0.f, 0.f )
{
	ResetActorRenderBase();
	for( int i = 0; i < AL_MAX; i++) {
		m_fAlphaLayers[ i ] = 1.0f;
	}
}

MAActorRenderBase::~MAActorRenderBase()
{
	SAFE_RELEASE_SPTR( m_hObject );
}

void MAActorRenderBase::ResetActorRenderBase()
{
	m_LocalTime = 0;
	m_AniTime = 0;
	m_nAniIndex = -1;

	m_nBlendAniIndex = -1;
	m_fBlendAniFrame = 0.f;
	m_fBlendFrame = 0.f;
	m_fBlendStartFrame = 0.f;
	m_fBlendCurFrame = 0.f;
	m_fFrame = m_fPrevFrame = 0.f;

	m_nMixedBlendAniIndex = -1;
	m_nMixedBlendBoneIndex = -1;
	m_fMixedBlendFrame = m_fMixedFrame = m_fMixedBlendAniFrame = 0.f;
	m_MixedBlendAniTime = 0;

	m_vAniDistance = EtVector3( 0.f, 0.f, 0.f );
	m_vAddAniDistance = EtVector3( 0.f, 0.f, 0.f );

	m_fFps = s_fDefaultFps;
}

void MAActorRenderBase::ChangeAnimation( int nAniIndex, float fFrame, float fBlendFrame )
{
	m_nBlendAniIndex = m_nAniIndex;
	m_fBlendStartFrame = fFrame;
	m_fBlendFrame = fBlendFrame;
	if( m_fBlendFrame == 0.f ) m_nBlendAniIndex = -1;

	if( m_nBlendAniIndex != -1 ) {
		m_fBlendAniFrame = m_fFrame;

		if( m_hObject ) {
			float fAniLength = (float)m_hObject->GetLastFrame( m_nBlendAniIndex );
			if( m_fBlendAniFrame >= fAniLength ) {
				m_fBlendAniFrame = fAniLength;
			}
			if( m_fBlendStartFrame >= fAniLength ) {
				m_fBlendStartFrame = fAniLength;
			}
		}
	}

	m_nAniIndex = nAniIndex;
	m_AniTime = m_LocalTime - (LOCAL_TIME)( fFrame / m_fFps * 1000.f );
	if( m_AniTime < 0 ) m_AniTime = 0;
	m_fFrame = fFrame;
	m_fPrevFrame = fFrame;

	// 블렌딩 중 액션 변하면 알파값 보정이 안되므로 안전빵으로 넣는다.
	if( m_hObject && m_hObject->GetObjectAlpha() < 1.f ) {
		SetAlphaBlend( 1.f , AL_SIGNAL );
	}
}

bool MAActorRenderBase::IsExistMixedAnimation( int nBoneIndex, int nAniIndex )
{
	for( DWORD i=0; i<m_VecMixedAni.size(); i++ ) {
		if( m_VecMixedAni[i].nBoneIndex == nBoneIndex && m_VecMixedAni[i].nAniIndex == nAniIndex ) {
			return true;
		}
	}
	return false;
}

void MAActorRenderBase::AddMixedAnimation( const char *szActionBoneName, const char *szMaintenanceBoneName, int nAniIndex, float fFrame, float fBlendFrame, float fEndBlendFrame, int nLoopCount )
{
	AddMixedAnimation( GetBoneIndex( szActionBoneName ), GetBoneIndex( szMaintenanceBoneName ), nAniIndex, fFrame, fBlendFrame, 3.f, nLoopCount );
}

void MAActorRenderBase::AddMixedAnimation( int nBoneIndex, int nMaintenanceBoneIndex, int nAniIndex, float fFrame, float fBlendFrame, float fEndBlendFrame, int nLoopCount )
{
	if( !m_hObject ) return;
	if( IsExistMixedAnimation( nBoneIndex, nAniIndex ) ) return;

	if( nBoneIndex == -1 || nMaintenanceBoneIndex == -1 || nAniIndex == -1 /*|| fBlendFrame <= 0.f*/ ) return;
	int nBlendAniIndex = m_nAniIndex;
	if( !m_VecMixedAni.empty() ) {
		nBlendAniIndex = m_VecMixedAni[0].nAniIndex;
	}

	MixedAniStruct Struct;
	Struct.nBoneIndex = nBoneIndex;
	Struct.nMaintenanceBoneIndex = nMaintenanceBoneIndex;

	Struct.nAniIndex = nAniIndex;
	Struct.AniTime = m_LocalTime - (LOCAL_TIME)( fFrame / m_fFps * 1000.f );
	Struct.fFrame = fFrame;
	Struct.fLastFrame = (float)m_hObject->GetLastFrame( nAniIndex );
	Struct.fBlendFrame = fBlendFrame;
	Struct.fEndBlendFrame = fEndBlendFrame;
	Struct.nBlendAniIndex = nBlendAniIndex;
	Struct.fBlendAniFrame = m_fFrame;
	Struct.fBlendStartFrame = fFrame;
	Struct.nLoopCount = nLoopCount;
	m_nMixedBlendAniIndex = -1;

	m_VecMixedAni.push_back( Struct );
}

int MAActorRenderBase::GetBoneIndex( const char *szBoneName )
{
	if( !m_hObject ) return -1;
	if( !m_hObject->GetAniHandle() ) return -1;
	return m_hObject->GetAniHandle()->GetBoneIndex( szBoneName );
}

int MAActorRenderBase::GetDummyBoneIndex( const char *szBoneName )
{
	if( !m_hObject ) return -1;
	if( !m_hObject->GetMesh() ) return -1;
	return m_hObject->GetMesh()->FindDummy( szBoneName );
}

int MAActorRenderBase::GetSubMeshIndex( const char *szSubMeshName )
{
	if( !m_hObject ) return 0;
	return m_hObject->GetSubMeshIndex(szSubMeshName);
}
const char *MAActorRenderBase::GetBoneName( int nBoneIndex )
{
	if( !m_hObject ) return "";
	CEtBone *pBone = m_hObject->GetAniHandle()->GetBone( nBoneIndex );
	if( !pBone ) return "";
	return pBone->GetName();
}

void MAActorRenderBase::Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta )
{
	m_LocalTime = LocalTime;

	if( !m_hObject ) return;

	EtMatrix matTemp, matScale;
	EtMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
	EtMatrixMultiply( &matTemp, &matScale, pmatWorld );

	m_hObject->Update( &matTemp );

	MAFaceAniBase::Process( LocalTime, fDelta );

}

void MAActorRenderBase::PreProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hObject ) return;
	if( m_nAniIndex == -1 ) 
	{
		// SetAniFrame()에 ResetAniFrame()이 들어 있는데 m_nAniIndex가 -1이라서 리셋 안해주는 경우 생김..
		m_hObject->ResetAniFrame();
		return;
	}

	if( m_hObject->GetAniCount() < m_nAniIndex )
		return;

	// Calc Ani
	if( 0.0f < m_fFps )
		m_fFrame = ( ( m_LocalTime - m_AniTime ) / 1000.f ) * m_fFps;

	float fAniLength = (float)m_hObject->GetLastFrame( m_nAniIndex );

	if( m_fFrame < 0.f ) m_fFrame = fAniLength;

	if( m_fFrame >= fAniLength ) {
		m_fFrame = fAniLength;
	}
	m_hObject->SetAniFrame( m_nAniIndex, m_fFrame );

	int nBoneIndex = 0;
//	int nDisableBoneIndex = -1;
	std::vector<int> nVecDisableBone;

	// Mixed Ani 
	for( DWORD i=0; i<m_VecMixedAni.size(); i++ ) {
		float fFrame = ( ( m_LocalTime - m_VecMixedAni[i].AniTime ) / 1000.f ) * m_fFps;
		m_VecMixedAni[i].fFrame = fFrame;
		if( fFrame >= m_VecMixedAni[i].fLastFrame ) {
			if( m_VecMixedAni[i].nLoopCount > 0 || m_VecMixedAni[i].nLoopCount == -1 ) {
				if( m_VecMixedAni[i].nLoopCount > 0 )
					m_VecMixedAni[i].nLoopCount--;
				m_VecMixedAni[i].AniTime = m_LocalTime;

				m_hObject->SetAniFrame( m_VecMixedAni[i].nAniIndex, fFrame, m_VecMixedAni[i].nBoneIndex );
				nBoneIndex = m_VecMixedAni[i].nMaintenanceBoneIndex;
				nVecDisableBone.push_back( m_VecMixedAni[i].nBoneIndex );
				if( fFrame - m_VecMixedAni[i].fBlendStartFrame < m_VecMixedAni[i].fBlendFrame ) {
					float fWeight = 1.f - ( 1.f / m_VecMixedAni[i].fBlendFrame * ( fFrame - m_VecMixedAni[i].fBlendStartFrame ) );
					m_hObject->BlendAniFrame( m_VecMixedAni[i].nBlendAniIndex, m_VecMixedAni[i].fBlendAniFrame, fWeight, 0, nBoneIndex );
				}
			}
			else {
				m_nMixedBlendAniIndex = m_VecMixedAni[i].nAniIndex;
				m_fMixedBlendAniFrame = m_VecMixedAni[i].fLastFrame;
				m_nMixedBlendBoneIndex = m_VecMixedAni[i].nBoneIndex;

				m_fMixedFrame = m_fFrame;
				m_MixedBlendAniTime = LocalTime;
				m_fMixedBlendFrame = m_VecMixedAni[i].fEndBlendFrame;
//				if( m_fMixedBlendFrame == 0.f ) m_fMixedBlendFrame = 6.f;

				m_VecMixedAni.erase( m_VecMixedAni.begin() + i );
				i--;
				break;  
			}
		}
		else {
			m_hObject->SetAniFrame( m_VecMixedAni[i].nAniIndex, fFrame, m_VecMixedAni[i].nBoneIndex );
			nBoneIndex = m_VecMixedAni[i].nMaintenanceBoneIndex;
			nVecDisableBone.push_back( m_VecMixedAni[i].nBoneIndex );
			if( fFrame - m_VecMixedAni[i].fBlendStartFrame < m_VecMixedAni[i].fBlendFrame ) {
				if( m_VecMixedAni[i].fBlendFrame > 0.f && fFrame - m_VecMixedAni[i].fBlendStartFrame >= 0.f ) {
					float fWeight = 1.f - ( 1.f / m_VecMixedAni[i].fBlendFrame * ( fFrame - m_VecMixedAni[i].fBlendStartFrame ) );
					m_hObject->BlendAniFrame( m_VecMixedAni[i].nBlendAniIndex, m_VecMixedAni[i].fBlendAniFrame, fWeight, 0, nBoneIndex );
				}
			}
		}
	}

	// Blend Ani
	if( m_nBlendAniIndex != -1 ) {
		if( m_fFrame - m_fBlendStartFrame < m_fBlendFrame ) {
			if( m_fBlendFrame > 0.f && m_fFrame - m_fBlendStartFrame >= 0.f ) {
				float fWeight = 1.f - ( 1.f / m_fBlendFrame * ( m_fFrame - m_fBlendStartFrame ) );
				m_hObject->BlendAniFrame( m_nBlendAniIndex, m_fBlendAniFrame, fWeight, 0, nVecDisableBone );
				if( m_nMixedBlendAniIndex != -1 ) {
					m_hObject->BlendAniFrame( m_nMixedBlendAniIndex, m_fMixedBlendAniFrame, fWeight, m_nMixedBlendBoneIndex );
				}
			}
		}
		else {
			m_nBlendAniIndex = -1;
			//			m_nMixedBlendAniIndex = -1;
		}
	}
	else if( m_nMixedBlendAniIndex != -1 ) {
		float fFrame = ( ( m_LocalTime - m_MixedBlendAniTime ) / 1000.f ) * m_fFps;
		if( fFrame < m_fMixedBlendFrame ) {
			float fWeight = 1.f - ( 1.f / m_fMixedBlendFrame * fFrame );
			m_hObject->BlendAniFrame( m_nMixedBlendAniIndex, m_fMixedBlendAniFrame, fWeight, m_nMixedBlendBoneIndex );
		}
		else m_nMixedBlendAniIndex = -1;
	}

	// Calc Ani Distance
	CalcAniDistance( m_nAniIndex, m_fFrame, m_fPrevFrame, m_vAniDistance );
	m_vAniDistance += m_vAddAniDistance;
	m_vAddAniDistance = EtVector3( 0.f, 0.f, 0.f );
	if( m_hObject->GetCalcPositionFlag() & CALC_POSITION_X ) m_vAniDistance.x = 0.f;
	if( m_hObject->GetCalcPositionFlag() & CALC_POSITION_Y ) m_vAniDistance.y = 0.f;
	if( m_hObject->GetCalcPositionFlag() & CALC_POSITION_Z ) m_vAniDistance.z = 0.f;

	m_fPrevFrame = m_fFrame;
}

int MAActorRenderBase::GetAniIndex( const char *szAniName )
{
	if( !m_hObject ) return -1;
	if( !m_hObject->GetAniHandle() ) return -1;
	for( int i=0; i<m_hObject->GetAniCount(); i++ ) {
		if( strcmp( m_hObject->GetAniName(i), szAniName ) == NULL ) return i;
	}
	return -1;
}

EtMatrix MAActorRenderBase::GetBoneMatrix( const char *szBoneName, bool *bExistBone )
{
	if( bExistBone ) *bExistBone = false;
	EtMatrix mat;
	EtMatrixIdentity(&mat);
	if( szBoneName == NULL || strlen(szBoneName) <= 0 || !m_hObject ) {
		return mat;
	}

	if( szBoneName[ 0 ] == '#' ) {
		char *pParentName = m_hObject->GetMesh()->GetDummyParentByName( szBoneName );
		if( pParentName == NULL) return mat;
		EtMatrix &MatDummyMat = m_hObject->GetMesh()->GetDummyMatByName( szBoneName );
		if( pParentName[0] == '\0' ) {		// 링크가 안걸려 있으면 Bip01 포지션 (x,z만)을 따라간다... 
			
			EtMatrix *pmatBoneTransMat = m_hObject->GetBoneTransMat( 0 );
			if( pmatBoneTransMat == NULL ) return *m_hObject->GetWorldMat();

			EtMatrix MatRot;
			EtMatrixRotationY(&MatRot, atan2f( pmatBoneTransMat->_31, pmatBoneTransMat->_33 ));	// 테스트 필요.
			MatRot._41 = pmatBoneTransMat->_41;
			MatRot._43 = pmatBoneTransMat->_43;
			EtMatrixMultiply( &mat, &MatRot, m_hObject->GetWorldMat());			

			EtMatrixMultiply(&mat, &MatDummyMat, &mat);
		}
		else {
			int nBoneIndex = m_hObject->GetAniHandle()->GetBoneIndex( pParentName );
			if( nBoneIndex != -1 )
			{
				EtMatrix *pmatBoneTransMat = m_hObject->GetBoneTransMat( nBoneIndex );
				if( pmatBoneTransMat == NULL ) return *m_hObject->GetWorldMat();

				EtMatrixMultiply( &mat, pmatBoneTransMat, m_hObject->GetWorldMat() ); 
			}
			else
			{
				mat = *m_hObject->GetWorldMat();
			}
			EtMatrixMultiply(&mat, &MatDummyMat, &mat);
		}

		if( bExistBone ) *bExistBone = true;
		return mat;
	}
	else {
		int nBoneIndex = m_hObject->GetAniHandle()->GetBoneIndex( szBoneName );
		if( nBoneIndex == -1 ) return mat;
		EtMatrixMultiply( &mat, m_hObject->GetBoneTransMat( nBoneIndex ), m_hObject->GetWorldMat() );
		if( bExistBone ) *bExistBone = true;
		return mat;
	}
}

EtVector3 *MAActorRenderBase::GetAniDistance()
{
	return &m_vAniDistance;
}

bool MAActorRenderBase::GetBoundingSphere( SSphere &Sphere )
{
	if( !m_hObject ) return false;
	m_hObject->GetBoundingSphere( Sphere );
	return true;
}

bool MAActorRenderBase::GetBoundingBox( SAABox &Box )
{
	if( !m_hObject ) return false;
	m_hObject->GetBoundingBox( Box );
	return true;
}

float MAActorRenderBase::GetAlphaLayer( ALPHA_LAYER AL )
{
	float fResult = m_fAlphaLayers[ AL ];
	return fResult;
}

float MAActorRenderBase::GetAlpha()
{
	float fResultAlpha = 1.0f;
	for( int i = 0; i < AL_MAX; i++ ) {
		fResultAlpha *= m_fAlphaLayers[ i ];
	}
	return fResultAlpha;
}

void MAActorRenderBase::SetAlphaBlend( float fAlpha, ALPHA_LAYER Layer )
{
	if( !m_hObject ) return;

	m_fAlphaLayers[ Layer ] = fAlpha;
	
	float fResultAlpha = 1.0f;
	for( int i = 0; i < AL_MAX; i++ ) {
		fResultAlpha *= m_fAlphaLayers[ i ];
	}

	const float SHADOW_VISIBLE_ALPHA = 0.8f;

	if( fResultAlpha < SHADOW_VISIBLE_ALPHA && m_hObject->GetObjectAlpha() >= SHADOW_VISIBLE_ALPHA ) {
		EnableCastShadow( false );
	}
	if( fResultAlpha >= SHADOW_VISIBLE_ALPHA && m_hObject->GetObjectAlpha() < SHADOW_VISIBLE_ALPHA ) {
		EnableCastShadow( true );
	}

	m_hObject->SetObjectAlpha( fResultAlpha );
	if( IsShow() ) ShowRenderBase( ( fResultAlpha <= 0.f ) ? false : true );
}

bool MAActorRenderBase::IsEnableCastShadow()
{
	if( !m_hObject ) return false;
	return m_hObject->IsShadowCast();
}

void MAActorRenderBase::EnableCastShadow( bool bEnable )
{
	if( !m_hObject ) return;
	m_hObject->EnableShadowCast( bEnable );
}

void MAActorRenderBase::SetFPS( float fValue )
{
	m_AniTime = m_LocalTime - (LOCAL_TIME)( m_fFrame / fValue * 1000.f );
	if( m_nMixedBlendAniIndex != -1 ) {
		m_MixedBlendAniTime = m_LocalTime - (LOCAL_TIME)( m_fMixedFrame / fValue * 1000.f );
	}
	for( DWORD i=0; i<m_VecMixedAni.size(); i++ ) {
		m_VecMixedAni[i].AniTime = m_LocalTime - (LOCAL_TIME)( m_VecMixedAni[i].fFrame / fValue * 1000.f );
	}
	m_fFps = fValue;
}

float MAActorRenderBase::GetFPS()
{
	return m_fFps;
}

void MAActorRenderBase::ShowRenderBase( bool bShow )
{
	if( m_hObject ) m_hObject->ShowObject( bShow );
}



void MAActorRenderBase::ResetMixedAnimation( bool bBlending )
{
	if( bBlending ) {
		for( DWORD i=0; i<m_VecMixedAni.size(); i++ ) {
			m_VecMixedAni[i].fLastFrame = m_VecMixedAni[i].fFrame;
		}
	}
	else m_VecMixedAni.clear();
}


void MAActorRenderBase::SetFrame( float fValue )
{
	m_fFrame = fValue;
	m_AniTime = m_LocalTime - (LOCAL_TIME)( m_fFrame / m_fFps * 1000.f );
	if( m_AniTime < 0 ) m_AniTime = 0;
}

void MAActorRenderBase::SetPrevFrame( float fValue )
{
	m_fPrevFrame = fValue;
}

void MAActorRenderBase::AddAniDistance( EtVector3 &vVec )
{
	m_vAddAniDistance += vVec;
}

void MAActorRenderBase::CalcAniDistance( int nAniIndex, float fFrame, float fPrevFrame, EtVector3 &vDist )
{
	vDist = EtVector3( 0.f, 0.f, 0.f );
	if( m_hObject ) m_hObject->CalcAniDistance( nAniIndex, fFrame, fPrevFrame, vDist );
}
