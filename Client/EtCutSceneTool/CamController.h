#pragma once

// 이터니티 엔진 관련
#include "EternityEngine.h"
#include "EtMatrixEx.h"

using namespace EternityEngine;


class CArcBall;


// 카메라 설정
class CCamController
{
private:
	// 결과값을 퍼갈 것들이 여기 저장된다.
	EtVector3				m_vCamPos;
	EtVector3				m_vLookAt;
	EtVector3				m_vUp;
	EtVector3				m_vSide; // bintitle.

	// 원래 카메라 값들.
	EtVector3				m_vOriCamPos;
	EtVector3				m_vOriLookAt;
	EtVector3				m_vOriUp;

	// 월드를 돌려주는 아크볼
	CArcBall*			m_pWorldArcball;
	CArcBall*			m_pCamArcball;			// 역회전을 카메라에 적용시켜 주면 된다.

	float					m_fLookAtLength;

	int						m_iPrevDownX;
	int						m_iPrevDownY;

	// 일단 월드 돌려주는 것으로,,
	EtMatrix				m_matWorld;				// 최종 매트릭스
	EtMatrix				m_matTrans;
	EtMatrix				m_matWorldRot;			// 최종 월드 회전 행렬

	EtMatrix				m_matNowRot;
	EtMatrix				m_matLastRot;

	EtMatrix				m_matCamNowRot;
	EtMatrix				m_matCamLastRot;
	EtMatrix				m_matInvCamNowRot;

	EtMatrix				m_matView;
	EtMatrix				m_matProj;

	D3DVIEWPORT9			m_Viewport;

public:
	CCamController(void);
	~CCamController(void);

private:
	void _UpdatePrevDown( int iXPos, int iYPos );

public:
	void SetCamPos( const EtVector3& vCamPos );
	void SetLookAt( const EtVector3& vLookAt );
	void SetUpVector( const EtVector3& vUp );
	float GetLookAtLength( void ) { return m_fLookAtLength; };

	void SetOriCamPos( const EtVector3& vCamPos );
	void SetOriLookAt( const EtVector3& vLookAt );
	void SetOriUpVector( const EtVector3& vUp );

	void SetViewMatrix( const EtMatrix& matView );
	void SetProjMatrix( const EtMatrix& matProj );
	void SetViewport( const EtViewPort& Viewport );

	void OnLButtonDown( int iXPos, int iYPos );
	void OnLButtonCon( int iXPos, int iYPos );
	void OnRButtonDown( int iXPos, int iYPos );
	void OnRButtonCon( int iXPos, int iYPos );
	void OnCButtonDown( int iXPos, int iYPos );
	void OnCButtonCon( int iXPos, int iYPos );
	void OnMouseWheel( int iWheelDelta );

	void OnMoveFrontAndBack( float fSpeed ); // bintitle.
	void OnMoveLeftAndRight( float fSpeed ); // bintitle.


	void GetMatrix( /*IN OUT*/ EtMatrix& matResult );
	const EtVector3& GetUpVector( void ) { return m_vUp; };
	const EtVector3& GetCamPos( void ) { return m_vCamPos; };
	const EtVector3& GetLookAt( void ) { return m_vLookAt; };

	void Reset( void );
};
