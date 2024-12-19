#pragma once
#include "etbaseeffect.h"

#define RAIN_CYLINDER_SIDE_COUNT		16
#define RAIN_CYLINDER_RADIUS			50.0f
#define RAIN_CYLINDER_HEIGHT			500.0f
#define MAX_RAIN_CYLINDER_COUNT			4

class CEtRainEffect;
typedef CSmartPtr< CEtRainEffect >	EtRainHandle;

class CEtRainEffect : public CEtBaseEffect
{
public:
	CEtRainEffect(void);
	virtual ~CEtRainEffect(void);

protected:
	float m_fRainSpeed;
	float m_fRaniStretchValue;
	float m_fRainOffset;
	float m_fMaxRainAngle;
	float m_fInterpolationSpeed;
	int m_nLayerCount;
	float m_fRainSpeedDecreaseRate;

	EtVector3 m_vRainDir;
	EtMatrix m_RotationMat;
	EtVector3 m_vMovement;
	EtVector3 m_vPrevMoveDir;
	EtVector3 m_vCurUp;
	EtVector4 m_vVertexOffset;

public:
	void SetRainSpeed( float fSpeed ) { m_fRainSpeed = fSpeed; }
	float GetRainSpeed() { return m_fRainSpeed; }

	void SetStretchValue( float fValue ) { m_fRaniStretchValue = 1.0f / fValue; }
	float GetStretchValue() { return 1.0f / m_fRaniStretchValue; }

	void SetRainDir( EtVector3 &vDir );
	void GetRainDir( EtVector3 &vDir ) { vDir = m_vRainDir; }

	void SetLayerCount( int nCount ) { m_nLayerCount = min( nCount, MAX_RAIN_CYLINDER_COUNT ); }
	int GetLayerCount() { return m_nLayerCount; }

	void SetRainSpeedDecreaseRate( float fRate ) { m_fRainSpeedDecreaseRate = fRate; }
	float GetRainSpeedDecreaseRate() { return m_fRainSpeedDecreaseRate; }

	void Initialize( const char *pRainTexture );
	void SetMovement( EtVector3 &vMovement );

	void CalcRotationMat( float fElapsedTime );
	void RenderRainCylinder( EtVector3 &vPosition, EtVector3 &vScale );
	virtual void Render( float fElapsedTime );

	static EtRainHandle CreateRainEffect( const char *pRainTexture );
};
