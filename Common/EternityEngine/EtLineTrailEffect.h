#pragma once
#include "etbaseeffect.h"

class CEtLineTrailEffect;
typedef CSmartPtr< CEtLineTrailEffect >	EtLineTrailHandle;

#define MAX_LINE_TRAIL_VERTEX	100
class CEtLineTrailEffect : public CEtBaseEffect
{
public:
	CEtLineTrailEffect(void);
	virtual ~CEtLineTrailEffect(void);

protected:
	float m_fLineWidth;
	float m_fLifeTime;
	float m_fInverseLifeTime;
	float m_fMinSegment;
	EtColor m_LineColor;

	bool m_bModify;
	float m_fCurrentStartTime;
	std::vector< EtVector3 > m_vecPosition;
	std::vector< EtVector3 > m_vecTangent;
	std::vector< EtVector2 > m_vecTime;

	bool	m_bShow;

	static EtMaterialHandle s_hMaterial;
	static EtIndexBuffer *s_pCommonIndexBuffer;

	float	m_fElapsedTime;

public:
	void Initialize( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment = 50.0f );
	void SetLineColor( EtColor &LineColor ) { m_LineColor = LineColor; }
	void CreateVertexBuffer();
	void CopyVertexBuffer();
	void AddPoint( EtVector3 &Point );
	void Show( bool bShow ) {m_bShow=bShow;}
	bool IsShow() {return m_bShow;}
	virtual void Render( float fElapsedTime );

	static void InitializeEffect();
	static void FinializeEffect();
};
