#include "Stdafx.h"
#include "EtAniKey.h"

CEtAniKey::CEtAniKey()
{
}

CEtAniKey::~CEtAniKey()
{
	Clear();
}

void CEtAniKey::Clear()
{
	m_vecPositionKey.clear();
	m_vecRotationKey.clear();
	m_vecScaleKey.clear();
}

void CEtAniKey::CalcPosition( EtVector3 &Position, float fFrame )
{
	if( fFrame < 0.f ) fFrame = 0.f;
	if( m_vecPositionKey.empty() )
	{
		Position = m_DefaultPosition;
		return;
	}
	if( m_vecPositionKey.size() == 1 )
	{
		Position = m_vecPositionKey[ 0 ].Position;
		return;
	}

	int nFrame;
	int nRangeStart, nRangeEnd, nRangeCur;
	float fWeight;

	nRangeStart = 0;
	nRangeEnd = ( int )m_vecPositionKey.size() - 1;
	nRangeCur = ( int )( fFrame / m_vecPositionKey[ nRangeEnd ].nTime * nRangeEnd );
	nFrame = ( int )fFrame;
	while( 1 )
	{
		// 익스포트 잘못된 데이타가 있어서 예외처리 한다. 익스포터쪽 손보고 고치자
		// by mapping
		if( nRangeCur >= ( int )m_vecPositionKey.size() - 1 )
		{
			Position = m_vecPositionKey[ m_vecPositionKey.size() - 1 ].Position;
			return;
		}
		else if( nRangeCur < 0 ) 
		{
			Position = m_vecPositionKey[0].Position;
			return;
		}

		if( nFrame == m_vecPositionKey[ nRangeCur ].nTime )
		{
			if( fFrame == nFrame )
			{
				Position = m_vecPositionKey[ nRangeCur ].Position;
			}
			else
			{
				fWeight = ( fFrame - m_vecPositionKey[ nRangeCur ].nTime ) / ( m_vecPositionKey[ nRangeCur + 1 ].nTime - m_vecPositionKey[ nRangeCur ].nTime );
				EtVec3Lerp( &Position, &m_vecPositionKey[ nRangeCur ].Position, 
					&m_vecPositionKey[ nRangeCur + 1 ].Position, fWeight );
			}
			return;
		}
		if( nFrame < m_vecPositionKey[ nRangeCur ].nTime )
		{
			nRangeEnd = nRangeCur - 1;
		}
		else
		{
			if( nFrame < m_vecPositionKey[ nRangeCur + 1 ].nTime )
			{
				fWeight = ( fFrame - m_vecPositionKey[ nRangeCur ].nTime ) / ( m_vecPositionKey[ nRangeCur + 1 ].nTime - m_vecPositionKey[ nRangeCur ].nTime );
				EtVec3Lerp( &Position, &m_vecPositionKey[ nRangeCur ].Position, 
					&m_vecPositionKey[ nRangeCur + 1 ].Position, fWeight );
				return;
			}
			nRangeStart = nRangeCur + 1;
		}
		nRangeCur = ( nRangeStart + nRangeEnd ) >> 1;
	}
}

void CEtAniKey::CalcRotation( EtQuat &Rotation, float fFrame )
{
	if( m_vecRotationKey.empty() )
	{
		Rotation = m_DefaultRotation;
		return;
	}
	if( ( m_vecRotationKey.size() == 1 ) || ( fFrame > m_vecRotationKey[ m_vecRotationKey.size() - 1 ].nTime ) )
	{
		ConvertShortToQuat( m_vecRotationKey[ 0 ].Rotation, Rotation );
		return;
	}

	int nFrame;
	int nRangeStart, nRangeEnd, nRangeCur;
	float fWeight;

	nRangeStart = 0;
	nRangeEnd = ( int )m_vecRotationKey.size() - 1;
	nRangeCur = ( int )( fFrame / m_vecRotationKey[ nRangeEnd ].nTime * nRangeEnd );
	nFrame = ( int )fFrame;
	while( 1 )
	{
		// 익스포트 잘못된 데이타가 있어서 예외처리 한다. 익스포터쪽 손보고 고치자
		// by mapping
		if( nRangeCur >= ( int )m_vecRotationKey.size() - 1 )
		{
			ConvertShortToQuat( m_vecRotationKey[ m_vecRotationKey.size() - 1 ].Rotation, Rotation );
			return;
		}
		else if( nRangeCur < 0 )
		{
			ConvertShortToQuat( m_vecRotationKey[0].Rotation, Rotation );
			return;
		}
		if( nFrame == m_vecRotationKey[ nRangeCur ].nTime )
		{
			if( fFrame == nFrame )
			{
				ConvertShortToQuat( m_vecRotationKey[ nRangeCur ].Rotation, Rotation );
			}
			else
			{
				fWeight = ( fFrame - m_vecRotationKey[ nRangeCur ].nTime ) / ( m_vecRotationKey[ nRangeCur + 1 ].nTime - m_vecRotationKey[ nRangeCur ].nTime );
				EtQuat Quat1, Quat2;
				ConvertShortToQuat( m_vecRotationKey[ nRangeCur ].Rotation, Quat1 );
				ConvertShortToQuat( m_vecRotationKey[ nRangeCur + 1 ].Rotation, Quat2 );
				EtQuaternionSlerp( &Rotation, &Quat1, &Quat2, fWeight );
			}
			return;
		}
		if( nFrame < m_vecRotationKey[ nRangeCur ].nTime )
		{
			nRangeEnd = nRangeCur - 1;
		}
		else
		{
			if( nFrame < m_vecRotationKey[ nRangeCur + 1 ].nTime )
			{
				fWeight = ( fFrame - m_vecRotationKey[ nRangeCur ].nTime ) / ( m_vecRotationKey[ nRangeCur + 1 ].nTime - m_vecRotationKey[ nRangeCur ].nTime );
				EtQuat Quat1, Quat2;
				ConvertShortToQuat( m_vecRotationKey[ nRangeCur ].Rotation, Quat1 );
				ConvertShortToQuat( m_vecRotationKey[ nRangeCur + 1 ].Rotation, Quat2 );
				EtQuaternionSlerp( &Rotation, &Quat1, &Quat2, fWeight );
				return;
			}
			nRangeStart = nRangeCur + 1;
		}
		nRangeCur = ( nRangeStart + nRangeEnd ) >> 1;
	}
}

void CEtAniKey::CalcScale( EtVector3 &Scale, float fFrame )
{
	if( m_vecScaleKey.empty() )
	{
		Scale = m_DefaultScale;
		return;
	}
	if( ( m_vecScaleKey.size() == 1 ) || ( fFrame > m_vecScaleKey[ m_vecScaleKey.size() - 1 ].nTime ) )
	{
		Scale = m_vecScaleKey[ 0 ].Scale;
		return;
	}

	int nFrame;
	int nRangeStart, nRangeEnd, nRangeCur;

	nRangeStart = 0;
	nRangeEnd = ( int )m_vecScaleKey.size() - 1;
	nRangeCur = ( int )( fFrame / m_vecScaleKey[ nRangeEnd ].nTime * nRangeEnd );
	nFrame = ( int )fFrame;
	while( 1 )
	{
		// 익스포트 잘못된 데이타가 있어서 예외처리 한다. 익스포터쪽 손보고 고치자
		// by mapping
		if( nRangeCur >= ( int )m_vecScaleKey.size() - 1 )
		{
			Scale = m_vecScaleKey[ m_vecScaleKey.size() - 1 ].Scale;
			return;
		}
		else if( nRangeCur < 0 ) 
		{
			Scale = m_vecScaleKey[0].Scale;
			return;
		}
		if( nFrame == m_vecScaleKey[ nRangeCur ].nTime )
		{
			Scale = m_vecScaleKey[ nRangeCur ].Scale;
			return;
		}
		if( nFrame < m_vecScaleKey[ nRangeCur ].nTime )
		{
			nRangeEnd = nRangeCur - 1;
		}
		else
		{
			if( nFrame < m_vecScaleKey[ nRangeCur + 1 ].nTime )
			{
				Scale = m_vecScaleKey[ nRangeCur ].Scale;
				return;
			}
			nRangeStart = nRangeCur + 1;
		}
		nRangeCur = ( nRangeStart + nRangeEnd ) >> 1;
	}
}

void CEtAniKey::LoadKey( CStream *pStream, int nAniVersion )
{
	if( nAniVersion <= 10 )
	{
		return _LoadKeyOld( pStream );
	}

	int nCount;

	pStream->Read( &m_DefaultPosition, sizeof( EtVector3 ) );
	pStream->Read( &m_DefaultRotation, sizeof( EtQuat ) );
	pStream->Read( &m_DefaultScale, sizeof( EtVector3 ) );

	m_vecPositionKey.clear();
	m_vecRotationKey.clear();
	m_vecScaleKey.clear();

	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecPositionKey.resize( nCount );
		pStream->Read( &m_vecPositionKey[ 0 ], sizeof( SPositionKey ) * nCount );
	}
	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecRotationKey.resize( nCount );
		pStream->Read( &m_vecRotationKey[ 0 ], sizeof( SRotationKey ) * nCount );
	}
	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecScaleKey.resize( nCount );
		pStream->Read( &m_vecScaleKey[ 0 ], sizeof( SScaleKey ) * nCount );
	}
}

void CEtAniKey::_LoadKeyOld( CStream *pStream )
{
	int nCount;

	pStream->Read( &m_DefaultPosition, sizeof( EtVector3 ) );
	pStream->Read( &m_DefaultRotation, sizeof( EtQuat ) );
	pStream->Read( &m_DefaultScale, sizeof( EtVector3 ) );

	m_vecPositionKey.clear();
	m_vecRotationKey.clear();
	m_vecScaleKey.clear();

	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecPositionKey.resize( nCount );
		int i;
		SPositionKeyOld PositionKey;
		SPositionKey CurKey;
		for( i = 0; i < nCount; i++ )
		{
			pStream->Read( &PositionKey, sizeof( SPositionKeyOld ) );
			CurKey.nTime = ( short )PositionKey.nTime;
			CurKey.Position = PositionKey.Position;
			m_vecPositionKey[ i ] = CurKey;
		}
	}
	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecRotationKey.resize( nCount );
		int i;
		SRotationKeyOld RotationKey;
		SRotationKey CurKey;
		for( i = 0; i < nCount; i++ )
		{
			pStream->Read( &RotationKey, sizeof( SRotationKeyOld ) );
			ConvertQuatToShort( RotationKey.Rotation, CurKey.Rotation );
			CurKey.nTime = ( short )RotationKey.nTime;
			m_vecRotationKey[ i ] = CurKey;
		}
	}
	pStream->Read( &nCount, sizeof( int ) );
	if( nCount )
	{
		m_vecScaleKey.resize( nCount );
		int i;
		SScaleKeyOld ScaleKey;
		SScaleKey CurKey;
		for( i = 0; i < nCount; i++ )
		{
			pStream->Read( &ScaleKey, sizeof( SScaleKeyOld ) );
			CurKey.nTime = ( short )ScaleKey.nTime;
			CurKey.Scale = ScaleKey.Scale;
			m_vecScaleKey[ i ] = CurKey;
		}
	}
}

void CEtAniKey::CopyKey( CEtAniKey *pSourceKey )
{
	std::vector< SPositionKey > *pPositionKey;
	std::vector< SRotationKey > *pRotationKey;
	std::vector< SScaleKey > *pScaleKey;

	memcpy( &m_DefaultPosition, pSourceKey->GetDefaultPosition(), sizeof( EtVector3 ) );
	memcpy( &m_DefaultRotation, pSourceKey->GetDefaultRotation(), sizeof( EtQuat ) );
	memcpy( &m_DefaultScale, pSourceKey->GetDefaultScale(), sizeof( EtVector3 ) );

	pPositionKey = pSourceKey->GetPositionKey();
	m_vecPositionKey.resize( pPositionKey->size() );
	if( !m_vecPositionKey.empty() )
	{
		memcpy( &m_vecPositionKey[ 0 ], &( ( *pPositionKey )[ 0 ] ), sizeof( SPositionKey ) * m_vecPositionKey.size() );
	}
	pRotationKey = pSourceKey->GetRotationKey();
	m_vecRotationKey.resize( pRotationKey->size() );
	if( !m_vecRotationKey.empty() )
	{
		memcpy( &m_vecRotationKey[ 0 ], &( ( *pRotationKey )[ 0 ] ), sizeof( SRotationKey ) * m_vecRotationKey.size() );
	}
	pScaleKey = pSourceKey->GetScaleKey();;
	m_vecScaleKey.resize( pScaleKey->size() );
	if( !m_vecScaleKey.empty() )
	{
		memcpy( &m_vecScaleKey[ 0 ], &( ( *pScaleKey )[ 0 ] ), sizeof( SScaleKey ) * m_vecScaleKey.size() );
	}
}

void CEtAniKey::SaveKey( CStream *pStream )
{
	int nCount;

	pStream->Write( &m_DefaultPosition, sizeof( EtVector3 ) );
	pStream->Write( &m_DefaultRotation, sizeof( EtQuat ) );
	pStream->Write( &m_DefaultScale, sizeof( EtVector3 ) );

	nCount = ( int )m_vecPositionKey.size();
	pStream->Write( &nCount, sizeof( int ) );
	if( nCount )
	{
		pStream->Write( &m_vecPositionKey[ 0 ], sizeof( SPositionKey ) * nCount );
	}
	nCount = ( int )m_vecRotationKey.size();
	pStream->Write( &nCount, sizeof( int ) );
	if( nCount )
	{
		pStream->Write( &m_vecRotationKey[ 0 ], sizeof( SRotationKey ) * nCount );
	}
	nCount = ( int )m_vecScaleKey.size();
	pStream->Write( &nCount, sizeof( int ) );
	if( nCount )
	{
		pStream->Write( &m_vecScaleKey[ 0 ], sizeof( SScaleKey ) * nCount );
	}
}
