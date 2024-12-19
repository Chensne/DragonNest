#pragma once
#include "etbaseeffect.h"

class CEtSnowEffect;
typedef CSmartPtr< CEtSnowEffect >	EtSnowHandle;

class CEtSnowEffect : public CEtBaseEffect
{
public:
	CEtSnowEffect(void);
	virtual ~CEtSnowEffect(void);

protected:
	float	m_fSnowTimer;
	std::vector<CEtMeshStream*> m_MeshStreamList;
	bool	m_bUsePointSprite;
	EtVector3 m_CamXVector;
	EtVector3 m_CamYVector;
	EtVector3 m_CamPos;
	int			m_nSnowCount;
	int			m_nSnowTimeOut;
	float		m_fSnowHeight;

public:
	void Initialize( const char *pRainTexture );
	virtual void Render( float fElapsedTime );
	void SetSnowCount( int nSnowCount ) {m_nSnowCount = nSnowCount; }
	void SetSnowTimeOut( int nSnowTimeOut) {m_nSnowTimeOut = nSnowTimeOut; }
	void SetSnowHeight( float fSnowHeight ) { m_fSnowHeight = fSnowHeight; }

	static EtSnowHandle CreateSnowEffect( const char *pSnowTexture, int nSnowCount, float fSnowSize = 0.0f );

protected:
	bool m_isUp;
	float m_fSnowSize;
	float m_fSnowAniSpeed;

public:
	void SetIsUp(bool isUp)
	{
		m_isUp = isUp;
		//방향에 따라 시작값을 바꿔준다..
		m_fSnowTimer = m_isUp ? 1.0f : 0.0f;
	}

	void SetSnowSize(float fSize) { m_fSnowSize = fSize; }
	void SetSnowAniSpeed(float fAniSpeed) { m_fSnowAniSpeed = fAniSpeed; }
};
