#pragma once
#include "etbaseeffect.h"

class CEtEffectTrail;
typedef CSmartPtr< CEtEffectTrail >	EtEffectTrailHandle;

// Rotha LineTrailEffect를 좀더 세부적으로 사용하고 싶다는 요청으로 설정
// Bone 에 링크가 가능하고 , 블랜드 옵션등을 설정 가능하며
// 최소 / 최대 거리별 처리 등을 설정할 예정.
// 기본구조는 LineTrailEffect를 따라가도록 설정합니다.

#define MAX_EFFECT_TRAIL_VERTEX	300
class CEtEffectTrail : public CEtBaseEffect
{
public:
	CEtEffectTrail(void);
	virtual ~CEtEffectTrail(void);

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
