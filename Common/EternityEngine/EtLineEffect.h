#pragma once
#include "etbaseeffect.h"

class CEtLineEffect;
typedef CSmartPtr< CEtLineEffect >	EtLineHandle;

#define MAX_LINE_VERTEX	4
class CEtLineEffect : public CEtBaseEffect
{
public:
	CEtLineEffect(void);
	virtual ~CEtLineEffect(void);

protected:
	float m_fLineWidth;
	float m_fLifeTime;
	float m_fInverseLifeTime;
	float m_fMinSegment;
	EtColor m_LineColor;
	float m_fUMoveSpeed;

	EtTextureHandle m_hTexture;

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
	void Initialize( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fUMoveSpeed );
	void SetLineColor( EtColor &LineColor ) { m_LineColor = LineColor; }
	void CreateVertexBuffer();
	void CopyVertexBuffer();
	void AddPoint( EtVector3 &start, EtVector3 &end );
	void ModifyStartPoint( EtVector3 &start );
	void ModifyEndPoint( EtVector3 &end );
	void Show( bool bShow ) {m_bShow=bShow;}
	bool IsShow() {return m_bShow;}
	virtual void Render( float fElapsedTime );

	static void InitializeEffect();
	static void FinializeEffect();
};
