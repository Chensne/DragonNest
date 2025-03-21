#pragma once

#pragma pack(2)
struct SPositionKeyOld
{
	int nTime;
	EtVector3 Position;
};

struct SPositionKey
{
	short nTime;
	EtVector3 Position;
};

struct SRotationKeyOld
{
	int nTime;
	EtQuat Rotation;
};

struct SRotationKey
{
	short nTime;
	short Rotation[ 4 ];
};

struct SScaleKeyOld
{
	int nTime;
	EtVector3 Scale;
};

struct SScaleKey
{
	short nTime;
	EtVector3 Scale;
};
#pragma pack(4)

_inline void ConvertShortToQuat( short *pShortRotation, EtQuat &Quat )
{
	Quat.x = pShortRotation[ 0 ] / 32767.0f;
	Quat.y = pShortRotation[ 1 ] / 32767.0f;
	Quat.z = pShortRotation[ 2 ] / 32767.0f;
	Quat.w = pShortRotation[ 3 ] / 32767.0f;
}

_inline void ConvertQuatToShort( EtQuat &Quat, short *pShortRotation )
{
	pShortRotation[ 0 ] = ( short )( Quat.x * 32767 );
	pShortRotation[ 1 ] = ( short )( Quat.y * 32767 );
	pShortRotation[ 2 ] = ( short )( Quat.z * 32767 );
	pShortRotation[ 3 ] = ( short )( Quat.w * 32767 );
}

class CEtAniKey
{
public:
	CEtAniKey();
	~CEtAniKey();
	void Clear();

protected:
	std::vector< SPositionKey >	m_vecPositionKey;
	std::vector< SRotationKey >	m_vecRotationKey;
	std::vector< SScaleKey >	m_vecScaleKey;
	EtVector3 m_DefaultPosition;
	EtQuat m_DefaultRotation;
	EtVector3 m_DefaultScale;

public:
	std::vector< SPositionKey > *GetPositionKey() { return &m_vecPositionKey; }
	std::vector< SRotationKey > *GetRotationKey() { return &m_vecRotationKey; }
	std::vector< SScaleKey > *GetScaleKey() { return &m_vecScaleKey; }

	EtVector3 *GetDefaultPosition() { return &m_DefaultPosition; }
	EtQuat *GetDefaultRotation() { return &m_DefaultRotation; }
	EtVector3 *GetDefaultScale() { return &m_DefaultScale; }

	void CalcPosition( EtVector3 &Position, float fFrame );
	void CalcRotation( EtQuat &Rotation, float fFrame );
	void CalcScale( EtVector3 &Scale, float fFrame );

	void LoadKey( CStream *pStream, int nAniVersion );
	void _LoadKeyOld( CStream *pStream );
	void CopyKey( CEtAniKey *pSourceKey );
	void SaveKey( CStream *pStream );

};
