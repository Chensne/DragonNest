#include "StdAfx.h"
#include "EtAni.h"
#include "EtBone.h"
#include <map>
using namespace std;

CEtAni::CEtAni( CMultiRoom *pRoom )
: CEtResource( pRoom )
{
	memset(&m_AniHeader, 0, sizeof(m_AniHeader));
}

CEtAni::~CEtAni(void)
{
	Clear();
}

void CEtAni::Clear()
{
	SAFE_DELETE_PVEC( m_vecBone );
	SAFE_DELETE_VEC( m_mapBone );
}

int CEtAni::GetBoneIndex( const char *pBoneName )
{
	/*
	int i;

	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		if( stricmp( pBoneName, m_vecBone[ i ]->GetName() ) == 0 )
		{
			return i;
		}
	}

	return -1;
	*/
	std::map<std::string, CEtBone *>::iterator it = m_mapBone.find( pBoneName );
	if( it == m_mapBone.end() ) return -1;
	return it->second->GetBoneIndex();

}

CEtBone *CEtAni::FindBone( const char *pBoneName )
{
	int nBoneIndex;

	nBoneIndex = GetBoneIndex( pBoneName );
	if( nBoneIndex == -1 )
	{
		return NULL;
	}

	return m_vecBone[ nBoneIndex ];
}

int CEtAni::LoadAni( CStream *pStream )
{
	int i;
	CEtBone *pBone;
	char szAniName[ 256 ];

	Clear();
	pStream->Read( &m_AniHeader, sizeof( SAniFileHeader ) );
	pStream->Seek( ANI_HEADER_RESERVED, SEEK_CUR );

	for( i = 0; i < m_AniHeader.nAniCount; i++ )
	{
		pStream->Read( szAniName, 256 );
		m_vecAniName.push_back( szAniName );
	}

	m_vecAniLength.resize( m_AniHeader.nAniCount );
	pStream->Read( &m_vecAniLength[ 0 ], sizeof( int ) * m_AniHeader.nAniCount );

	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		pBone = new CEtBone();
		pBone->LoadBone( pStream, m_AniHeader.nAniCount, m_AniHeader.nVersion );
		m_vecBone.push_back( pBone );
		m_mapBone.insert( make_pair( pBone->GetName(), pBone ) );
		pBone->SetBoneIndex( i );
	}

	BuildHierarchy();
	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		if( m_vecBone[ i ]->GetParent() == NULL )
		{
			m_vecBone[ i ]->SetRootBone( true );
		}
	}

	return ET_OK;
}

int CEtAni::LoadMultiAni( CStream *pStream )
{
	int nSize = pStream->Size();
	char *pBuffer = new char[ nSize + 1 ];

	memset( pBuffer, 0, nSize + 1 );
	pStream->Read( pBuffer, nSize );

	char *pToken = strtok( pBuffer, "\n\r" );
	while( pToken )
	{
		if( m_vecBone.size() )
		{
			EtAniHandle hLoadAni = ::LoadResource( m_pBaseRoom, pToken, RT_ANI );
			if( hLoadAni )
			{
				MergeAni( hLoadAni );
				SAFE_RELEASE_SPTR( hLoadAni );
			}
		}
		else
		{
			CFileStream *pMainResource = dynamic_cast< CFileStream * >( pStream );
			if( pMainResource )
			{
				char szFileName[ _MAX_FNAME ];
				_GetPath( szFileName, _countof(szFileName), pMainResource->GetFileName() );
				strcat_s( szFileName, _MAX_FNAME, pToken );
				CFileStream *pSubAniStream = new CFileStream( szFileName );
				if( pSubAniStream )
				{
					if( pSubAniStream->IsValid() )
					{
						LoadAni( pSubAniStream );
					}
					delete pSubAniStream;
				}
			}
		}
		pToken = strtok( NULL, "\n\r" );
	}

	SAFE_DELETEA( pBuffer );

	return ET_OK;
}

int CEtAni::LoadResource( CStream *pStream )
{
	ASSERT( pStream && "Invalid Resource Stream( Animation )" );
	if( pStream == NULL )
	{
		return ETERR_INVALIDRESOURCESTREAM;
	}

	char szHeaderString[ 256 ];
	bool bMultiAni = false;
	if( pStream->Size() < ANI_HEADER_RESERVED + sizeof( SAniFileHeader ) )
	{
		bMultiAni = true;
	}
	else
	{
		pStream->Read( szHeaderString, 256 );
		if( stricmp( szHeaderString, ANI_FILE_STRING ) != 0 )
		{
			bMultiAni = true;
		}
	}
	if( bMultiAni )
	{
		return LoadMultiAni( pStream );
	}
	else
	{
		pStream->Seek( 0, SEEK_SET );
		return LoadAni( pStream );
	}
}

void CEtAni::BuildHierarchy()
{
	int i;

	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		CEtBone *pBone;

		pBone = FindBone( m_vecBone[ i ]->GetParentName() );
		if( pBone )
		{
			m_vecBone[ i ]->SetParent( pBone );
			pBone->AddChild( m_vecBone[ i ] );
		}
	}
}

void CEtAni::Reset()
{
	int i;

	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		m_vecBone[ i ]->Reset();
	}
}

float CEtAni::CalcCorrectFrame( int nAni, float fFrame )
{
	if( ( nAni >= m_AniHeader.nAniCount ) || ( fFrame < 0.0f ) )
	{
		ASSERT( 0 && "Invalid Ani Frame!!!( CEtAni::CalcCorrectFrame )" );
		return 0.0f;
	}

	/*
	fFrame = fmod( fFrame, ( float )GetAniLength( nAni ) );
	if( fFrame > GetLastFrame( nAni ) )
	{
		fFrame -= GetLastFrame( nAni );
	}
	*/
	if( fFrame > ( float )GetAniLength( nAni ) )
		fFrame = ( float )GetAniLength( nAni );

	return fFrame;
}

void CEtAni::SetAni( SBoneAniInfo *pAniInfo )
{
	int i;

	pAniInfo->fFrame = CalcCorrectFrame( pAniInfo->nAni, pAniInfo->fFrame );

	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		if( m_vecBone[ i ]->IsRootBone() )
		{
			m_vecBone[ i ]->SetAni( pAniInfo );
		}
	}
}

void CEtAni::CalcAni( std::vector< EtMatrix > &vecInvWorldMat, std::vector< EtMatrix > &vecTransMat )
{
	int i;
	EtMatrix VertexTransMat;

	if( ( int )vecTransMat.size() < m_AniHeader.nBoneCount )
	{
		vecTransMat.resize( m_AniHeader.nBoneCount );
	}
	for( i = 0; i < m_AniHeader.nBoneCount; i++ )
	{
		m_vecBone[ i ]->CalcAni();
		// 임시 siva
		if( i >= (int)vecInvWorldMat.size() ) return;
		//임시 2hogi
		if (i >= (int)m_vecBone.size()) return;
		EtMatrixMultiply( &VertexTransMat, &vecInvWorldMat[ i ], m_vecBone[ i ]->GetTransMat() );
		vecTransMat[ i ] = VertexTransMat;
	}
}

void CEtAni::CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec )
{
	/*
	if( ( nAni < 0 ) || ( nAni >= m_AniHeader.nAniCount ) )
	{
		ASSERT( 0 && "Invalid AniIndex!!!( CEtAni::CalcAniDistance )" );
		DistVec = EtVector3( 0.0f, 0.0f, 0.0f );
		return;
	}
	if( !m_vecBone.empty() )
	{
		if( ( m_vecAniLength[ nAni ] - 1 ) < fPrevFrame )
		{
			fPrevFrame = 0.0f;
		}
		if( ( m_vecAniLength[ nAni ] - 1 ) < fCurFrame )
		{
			fCurFrame = 0.0f;
		}
		m_vecBone[ 0 ]->CalcAniDistance( nAni, fCurFrame, fPrevFrame, DistVec );
	}
	*/
	fCurFrame = CalcCorrectFrame( nAni, fCurFrame );
	fPrevFrame = CalcCorrectFrame( nAni, fPrevFrame );

	if( m_vecBone.empty() )
	{
		DistVec = EtVector3( 0.0f, 0.0f, 0.0f );
	}
	else
	{
		m_vecBone[ 0 ]->CalcAniDistance( nAni, fCurFrame, fPrevFrame, DistVec );
	}
}

void CEtAni::SetCalcPositionFlag( int nFlag )
{
	CEtBone::SetCalcPositionFlag( nFlag );
}

int CEtAni::MergeAni( CEtAni *pAni )
{
	int i, nBoneCount;

	if( pAni->GetBoneCount() != GetBoneCount() )
	{
		return ETERR_DIFFERENTBONECOUNT;
	}

	for( i = 0; i < pAni->GetAniCount(); i++ )
	{
		m_vecAniName.push_back( pAni->GetAniName( i ) );
		m_vecAniLength.push_back( pAni->GetAniLength( i ) );
	}

	nBoneCount = pAni->GetBoneCount();
	for( i = 0; i < nBoneCount; i++)
	{
		m_vecBone[ i ]->MergeBone( pAni->FindBone( m_vecBone[ i ]->GetName() ), pAni->GetAniCount() );
	}
	m_AniHeader.nAniCount += pAni->GetAniCount();

	return ET_OK;
}
