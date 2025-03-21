#pragma once

// 이터니티 엔진 관련
#include "EternityEngine.h"
#include "EtMatrixEx.h"


// 자유로운 카메라 조작을 위한 아크볼
class CArcBall
{
private:
	EtVector3			m_vClkPos;
	EtVector3			m_vDragPos;

	float				m_fAdjustWidth;
	float				m_fAdjustHeight;


public:
	CArcBall(void);
	CArcBall( float fNewWidth, float fNewHeight );
	~CArcBall(void);

private:
	void _MapToSphere( int iNewXPos, int iNewYPos, /* OUT */ EtVector3& vNewVec );

public:
	void SetBound( float fNewWidth, float fNewHeight );
	void Click( int iXPos, int iYPos );
	void Drag( int iNewXPos, int iNewYPos, /* OUT */ EtQuat* qNewRot );

};
