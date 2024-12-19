#include "StdAfx.h"
#include "EtBone.h"
#include "EtAniKey.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

int CEtBone::s_CalcPositionFlag = CALC_POSITION_Y;

CEtBone::CEtBone(void)
{
	m_bRootBone = false;
	m_bEnableBlend = true;
	m_pParent = NULL;
	m_pBoneRotation = NULL;
	m_fScale = 1.0f;

	memset( &m_BoneHeader, 0, sizeof( m_BoneHeader ) );
	memset( &m_AniInfo, 0, sizeof( m_AniInfo ) );
}

CEtBone::~CEtBone(void)
{
	Clear();
}

void CEtBone::Clear()
{
	SAFE_DELETE_PVEC( m_vecAniKey );
}

void CEtBone::LoadBone( CStream *pStream, int nAniCount, int nAniVersion )
{
	int i;
	CEtAniKey *pAniKey;

	Clear();
	pStream->Read( &m_BoneHeader, sizeof( SBoneHeader ) );
	pStream->Seek( BONE_HEADER_RESERVED, SEEK_CUR );

	for( i = 0; i < nAniCount; i++ )
	{
		pAniKey = new CEtAniKey();
		pAniKey->LoadKey( pStream, nAniVersion );
		m_vecAniKey.push_back( pAniKey );
	}
}

void CEtBone::SaveBone( CStream *pStream, int nAniCount )
{
	int i;
	char cReserved[ BONE_HEADER_RESERVED ];

	pStream->Write( &m_BoneHeader, sizeof( SBoneHeader ) );
	memset( cReserved, 0, BONE_HEADER_RESERVED );
	pStream->Write( cReserved, BONE_HEADER_RESERVED );

	for( i = 0; i < ( int )m_vecAniKey.size(); i++ )
	{
		m_vecAniKey[ i ]->SaveKey( pStream );
	}
}

void CEtBone::MergeBone( CEtBone *pMergeBone, int nAniCount )
{
	int i;

	for( i = 0; i < nAniCount; i++ )
	{
		CEtAniKey *pAniKey, *pMergeKey;

		pAniKey = new CEtAniKey();
		pMergeKey = pMergeBone->GetAniKey( i );
		pAniKey->CopyKey( pMergeKey );
		m_vecAniKey.push_back( pAniKey );
	}
}

void CEtBone::Reset()
{
	m_AniInfo.fFrame = 0.0f;
	m_AniInfo.nAni = 0;
	m_vecBlendAniInfo.clear();
}

void CEtBone::SetAni( SBoneAniInfo *pInfo )
{
	if( IsValidAniIndex( pInfo->nAni ) )
	{
		int i;

		m_AniInfo = *pInfo;
		for( i = 0; i < ( int )m_vecChild.size(); i++ )
		{
			m_vecChild[ i ]->SetAni( pInfo );
		}
	}
}

void CEtBone::BlendAni( SBoneAniInfo *pInfo )
{
	if( IsValidAniIndex( pInfo->nAni ) )
	{
		int i;

		if( !m_bEnableBlend )
		{
			return;
		}
		m_vecBlendAniInfo.push_back( *pInfo );
		for( i = 0; i < ( int )m_vecChild.size(); i++ )
		{
			m_vecChild[ i ]->BlendAni( pInfo );
		}
	}
}

void CEtBone::EnableBlend( bool bEnable ) 
{ 
	m_bEnableBlend = bEnable; 
	int i;
	for( i = 0; i < ( int )m_vecChild.size(); i++ )
	{
		m_vecChild[ i ]->EnableBlend( bEnable );
	}
}

void CEtBone::CalcAni()
{
	if( m_pParent )
	{
		EtMatrix AniMat;

		GetAniMatrix( &AniMat );
		if( m_fScale != 1.0f )
		{
			EtMatrix ScaleMat;
			EtMatrixScaling( &ScaleMat, m_fScale, m_fScale, m_fScale );
			EtMatrixMultiply( &AniMat, &ScaleMat, &AniMat );
		}
		EtMatrixMultiply( &m_TransMat, &AniMat, m_pParent->GetTransMat() );
		if( m_pBoneRotation )
		{
			EtMatrix RotationMat;
			EtVector3 Save;

			memcpy( &Save, &m_TransMat._41, sizeof( EtVector3 ) );
			EtMatrixRotationYawPitchRoll( &RotationMat, EtToRadian( m_pBoneRotation->y ), EtToRadian( m_pBoneRotation->x ), EtToRadian( m_pBoneRotation->z ) );
			EtMatrixMultiply( &m_TransMat, &m_TransMat, &RotationMat );
			memcpy( &m_TransMat._41, &Save, sizeof( EtVector3 ) );
		}
	}
	else
	{
		GetAniMatrix( &m_TransMat );
	}
}

void CEtBone::GetAniMatrix( EtMatrix *pOutMat )
{
	int i;
	EtVector3 Position1, Position2;
	EtQuat Rotation1, Rotation2;
	EtVector3 Scale;

	if( m_bRootBone )
	{
		m_vecAniKey[ m_AniInfo.nAni ]->CalcPosition( Position1, 0.0f );
		m_vecAniKey[ m_AniInfo.nAni ]->CalcPosition( Position2, m_AniInfo.fFrame );
		if( s_CalcPositionFlag & CALC_POSITION_X )
		{
			Position1.x = Position2.x;
		}
		if( s_CalcPositionFlag & CALC_POSITION_Y )
		{
			Position1.y = Position2.y;
		}
		if( s_CalcPositionFlag & CALC_POSITION_Z )
		{
			Position1.z = Position2.z;
		}
		for( i = 0; i < ( int )m_vecBlendAniInfo.size(); i++ )
		{
			m_vecAniKey[ m_vecBlendAniInfo[ i ].nAni ]->CalcPosition( Position2, m_vecBlendAniInfo[ i ].fFrame );
			if( !( s_CalcPositionFlag & CALC_POSITION_X ) )
			{
				Position2.x = Position1.x;
			}
			if( !( s_CalcPositionFlag & CALC_POSITION_Y ) )
			{
				Position2.y = Position1.y;
			}
			if( !( s_CalcPositionFlag & CALC_POSITION_Z ) )
			{
				Position2.z = Position1.z;
			}
			EtVec3Lerp( &Position1, &Position1, &Position2, m_vecBlendAniInfo[ i ].fWeight );
		}
	}
	else
	{
		m_vecAniKey[ m_AniInfo.nAni ]->CalcPosition( Position1, m_AniInfo.fFrame );
		for( i = 0; i < ( int )m_vecBlendAniInfo.size(); i++ )
		{
			m_vecAniKey[ m_vecBlendAniInfo[ i ].nAni ]->CalcPosition( Position2, m_vecBlendAniInfo[ i ].fFrame );
			EtVec3Lerp( &Position1, &Position1, &Position2, m_vecBlendAniInfo[ i ].fWeight );
		}
	}

	m_vecAniKey[ m_AniInfo.nAni ]->CalcRotation( Rotation1, m_AniInfo.fFrame );
	for( i = 0; i < ( int )m_vecBlendAniInfo.size(); i++ )
	{
		m_vecAniKey[ m_vecBlendAniInfo[ i ].nAni ]->CalcRotation( Rotation2, m_vecBlendAniInfo[ i ].fFrame );
		EtQuaternionSlerp( &Rotation1, &Rotation1, &Rotation2, m_vecBlendAniInfo[ i ].fWeight );
	}

	m_vecAniKey[ m_AniInfo.nAni ]->CalcScale( Scale, m_AniInfo.fFrame );

	EtMatrixTransformation( pOutMat, NULL, NULL, &Scale, NULL, &Rotation1, &Position1 );
}

void CEtBone::GetAniRotationMatrix( int nAni, float fFrame, EtMatrix &AniMat )
{
	if( IsValidAniIndex( nAni ) )
	{
		EtQuat Rotation;

		m_vecAniKey[ nAni ]->CalcRotation( Rotation, fFrame );
		EtMatrixRotationQuaternion( &AniMat, &Rotation );
	}
}

void CEtBone::CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec )
{
	if( IsValidAniIndex( nAni ) )
	{
		EtVector3 Position1, Position2;

		m_vecAniKey[ nAni ]->CalcPosition( Position1, fPrevFrame );
		m_vecAniKey[ nAni ]->CalcPosition( Position2, fCurFrame );
		DistVec = Position2 - Position1;
	}
	else
	{
		DistVec = EtVector3( 0.0f, 0.0f, 0.0f );
	}
}

