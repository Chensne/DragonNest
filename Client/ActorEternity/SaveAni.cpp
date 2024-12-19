#include "StdAfx.h"
#include ".\saveani.h"
#include "DumpUtil.h"
#include "ActorEternity.h"

CSaveAni::CSaveAni(void)
{
}

CSaveAni::~CSaveAni(void)
{
}

void CSaveAni::ProcessBone( FILE *fp, INode *pNode, Interface *pMaxInterface )
{
	SBoneHeader BoneHeader;
	TimeValue StartTime;

	m_pMaxInterface = pMaxInterface;
	m_pNode = pNode;
	m_PositionList.clear();
	m_RotationList.clear();
	m_ScaleList.clear();

	memset( &BoneHeader, 0, sizeof( SBoneHeader ) );
	strcpy( BoneHeader.szBoneName, pNode->GetName() );
	strcpy( BoneHeader.szParentName, pNode->GetParentNode()->GetName() );
	fwrite( &BoneHeader, sizeof( SBoneHeader ), 1, fp );

	StartTime = m_pMaxInterface->GetAnimRange().Start();
	m_LocalMat = m_pNode->GetNodeTM( StartTime ) * Inverse( m_pNode->GetParentNode()->GetNodeTM( StartTime ) );
	decomp_affine( m_LocalMat, &m_Affine );

	GetAniData();
	ReducePositionKey();
	ReduceRotationKey();
	ReduceScaleKey();
	WriteAniInfo( fp );
}

void CSaveAni::GetAniData()
{
	int i;
	int nStart, nEnd;
	SPositionKey PositionKey;
	SRotationKey RotationKey;
	SScaleKey ScaleKey;
	Matrix3 LocalMat;
	AffineParts AffineParts;

	nStart = m_pMaxInterface->GetAnimRange().Start() / GetTicksPerFrame();
	nEnd = m_pMaxInterface->GetAnimRange().End() / GetTicksPerFrame();
	nEnd++;

	for( i = nStart; i < nEnd; i++ )
	{
		LocalMat = m_pNode->GetNodeTM( i * GetTicksPerFrame() ) * Inverse( m_pNode->GetParentNode()->GetNodeTM( i * GetTicksPerFrame() ) );
		decomp_affine( LocalMat, &AffineParts );

		PositionKey.nTime = i - nStart;
		PositionKey.Position.x = AffineParts.t.x;
		PositionKey.Position.y = AffineParts.t.z;
		PositionKey.Position.z = AffineParts.t.y;
		PositionKey.nUse = 1;
		m_PositionList.push_back( PositionKey );

		RotationKey.nTime = i - nStart;
		RotationKey.Rotation.x = AffineParts.q.x;
		RotationKey.Rotation.y = AffineParts.q.z;
		RotationKey.Rotation.z = AffineParts.q.y;
		RotationKey.Rotation.w = AffineParts.q.w;
		RotationKey.nUse = 1;
		m_RotationList.push_back( RotationKey );

		ScaleKey.nTime = i - nStart;
		ScaleKey.Scale.x = AffineParts.k.x;
		ScaleKey.Scale.y = AffineParts.k.z;
		ScaleKey.Scale.z = AffineParts.k.y;
		ScaleKey.nUse = 1;
		m_ScaleList.push_back( ScaleKey );
	}
}

void CSaveAni::ReducePositionKey()
{
	int i, nPrevFrame, nSize;
	float fError;
	Point3 Estimate, Diff;

	nSize = m_PositionList.size();
	if( nSize <= 2 )
	{
		return;
	}

	nPrevFrame = 0;
	for( i = 1; i < nSize - 1; i++ )
	{
		Diff = m_PositionList[ nPrevFrame ].Position - m_PositionList[ i ].Position;
		fError = DotProd( Diff, Diff );
		if( fError < 0.01f )
		{
			m_PositionList[ i ].nUse = 0;
		}
		else
		{
			m_PositionList[ i - 1 ].nUse = 1;
			nPrevFrame = i;
		}
	}

	std::vector< SPositionKey >::iterator it;

	it = m_PositionList.begin();
	while( it != m_PositionList.end() )
	{
		if( it->nUse == 0 )
		{
			it = m_PositionList.erase( it );
		}
		else
		{
			it++;
		}
	}
	if( m_PositionList.size() == 2 )
	{
		if( m_PositionList[ 0 ].Position == m_PositionList[ 1 ].Position )
		{
			m_PositionList.clear();
		}
	}
}

void CSaveAni::ReduceRotationKey()
{
	int i, nSize;
	Quat CurQuat, PrevQuat;

	nSize = m_RotationList.size();
	if( nSize <= 2 )
	{
		return;
	}
	PrevQuat = m_RotationList[ 0 ].Rotation;
	for( i = 1; i < nSize - 1; i++ )
	{
		CurQuat = m_RotationList[ i ].Rotation / PrevQuat;
		if( CurQuat.IsIdentity() )
		{
			m_RotationList[ i ].nUse = 0;
		}
		else
		{
			m_RotationList[ i - 1 ].nUse = 1;
		}
		PrevQuat = m_RotationList[ i ].Rotation;
	}

	std::vector< SRotationKey >::iterator it;

	it = m_RotationList.begin();
	while( it != m_RotationList.end() )
	{
		if( it->nUse == 0 )
		{
			it = m_RotationList.erase( it );
		}
		else
		{
			it++;
		}
	}

	if( m_RotationList.size() == 2 )
	{
		if( m_RotationList[ 0 ].Rotation == m_RotationList[ 1 ].Rotation )
		{
			m_RotationList.clear();
		}
	}
}

void CSaveAni::ReduceScaleKey()
{
	int i, nPrevFrame, nSize;
	float fError;
	Point3 Estimate, Diff;

	nSize = m_ScaleList.size();
	if( nSize <= 2 )
	{
		return;
	}
	nPrevFrame = 0;
	for( i = 1; i < nSize - 1; i++ )
	{
		Diff = m_ScaleList[ nPrevFrame ].Scale - m_ScaleList[ i ].Scale;
		fError = DotProd( Diff, Diff );
		if( fError < 0.01f )
		{
			m_ScaleList[ i ].nUse = 0;
		}
		else
		{
			m_ScaleList[ i - 1 ].nUse = 1;
			nPrevFrame = i;
		}
	}

	std::vector< SScaleKey >::iterator it;

	it = m_ScaleList.begin();
	while( it != m_ScaleList.end() )
	{
		if( it->nUse == 0 )
		{
			it = m_ScaleList.erase( it );
		}
		else
		{
			it++;
		}
	}
	if( m_ScaleList.size() == 2 )
	{
		if( m_ScaleList[ 0 ].Scale == m_ScaleList[ 1 ].Scale )
		{
			m_ScaleList.clear();
		}
	}
}

void ConvertQuatToShort( Quat &Quaternion, short *pShortRotation )
{
	pShortRotation[ 0 ] = ( short )( Quaternion.x * 32767 );
	pShortRotation[ 1 ] = ( short )( Quaternion.y * 32767 );
	pShortRotation[ 2 ] = ( short )( Quaternion.z * 32767 );
	pShortRotation[ 3 ] = ( short )( Quaternion.w * 32767 );
}

void CSaveAni::WriteAniInfo( FILE *fp )
{
	int i, nSize;

	DumpAffine( fp, m_Affine );

	nSize = m_PositionList.size();
	fwrite( &nSize, sizeof( int ), 1, fp );
	if( nSize )
	{
		for( i = 0; i < nSize; i++ )
		{
			short nShortTime = ( short )m_PositionList[ i ].nTime;
			fwrite( &nShortTime, sizeof( short ), 1, fp );
			fwrite( &m_PositionList[ i ].Position, sizeof( Point3 ), 1, fp );
		}
	}

	nSize = m_RotationList.size();
	fwrite( &nSize, sizeof( int ), 1, fp );
	if( nSize )
	{
		for( i = 0; i < nSize; i++ )
		{
			short nShortTime = ( short )m_RotationList[ i ].nTime;
			short nRotation[ 4 ];
			fwrite( &nShortTime, sizeof( short ), 1, fp );
			ConvertQuatToShort( m_RotationList[ i ].Rotation, nRotation );
			fwrite( nRotation, sizeof( short ), 4, fp );
		}
	}

	nSize = m_ScaleList.size();
	fwrite( &nSize, sizeof( int ), 1, fp );
	if( nSize )
	{
		for( i = 0; i < nSize; i++ )
		{
			short nShortTime = ( short )m_ScaleList[ i ].nTime;
			fwrite( &nShortTime, sizeof( short ), 1, fp );
			fwrite( &m_ScaleList[ i ].Scale, sizeof( Point3 ), 1, fp );
		}
	}
}

bool CSaveAni::IsExistAniKey( INode *pNode, Interface *pMaxInterface )
{
	m_pMaxInterface = pMaxInterface;
	m_pNode = pNode;

	GetAniData();
	ReducePositionKey();
	ReduceRotationKey();
	ReduceScaleKey();

	if( !m_PositionList.empty() )
	{
		return true;
	}
	if( !m_RotationList.empty() )
	{
		return true;
	}
	if( !m_ScaleList.empty() )
	{
		return true;
	}

	return false;
}
