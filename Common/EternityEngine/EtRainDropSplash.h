#pragma once
#include "etbaseeffect.h"

#define MAX_RAIN_DROP_SPLASH_COUNT		200

class CEtRainDropSplash;
typedef CSmartPtr< CEtRainDropSplash >	EtRainDropSplashHandle;

class CEtRainDropSplash : public CEtBaseEffect
{
public:
	CEtRainDropSplash(void);
	~CEtRainDropSplash(void);

protected:
	float m_fLifeTime;
	float m_fInvLifeTime;
	float m_fCreationRadius;
	int m_nSplashPerSecond;
	float m_fRemainCreationTime;
	int m_nBaseIndex;
	int m_nAllocSize;
	std::vector< EtVector4 > m_vecPosition;

	int m_nPositionParamIndex;

public:
	void Initialize( const char *pSplashTexture, float fSplashSize );

	void CalcAllocSize();

	void SetLifeTime( float fLifeTime );
	float GetLifeTime() { return m_fLifeTime; }

	void SetCreationRadius( float fRadius ) { m_fCreationRadius = fRadius; }
	float GetCreationRadius() { return m_fCreationRadius; }

	void SetSplashPerSecond( int nSplash );
	int GetSplashPerSecond() { return m_nSplashPerSecond; }

	void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );

	static EtRainDropSplashHandle CreateRainDropSplash( const char *pSplashTexture, float fSplashSize );
};
