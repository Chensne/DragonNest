#pragma once

class MatrixEx
{
public:
	EtVector3	m_vXAxis;
	float m_fDistanceX;
	EtVector3	m_vYAxis;
	float m_fDistanceY;
	EtVector3	m_vZAxis;	
	float m_fDistanceZ;
	EtVector3	m_vPosition;
	float m_fW;

public:
	MatrixEx();
	MatrixEx( const EtMatrix &Matrix );

	void Identity();
	void RotateYaw(float fAngle);
	void RotateRoll(float fAngle);
	void RotatePitch(float fAngle);
	void RotateYAxis(float fAngle);
	void RotateZAxis(float fAngle);
	void RotateXAxis(float fAngle);
	void MoveLocalZAxis(float fDist) { m_vPosition+=fDist*m_vZAxis; }
	void MoveLocalYAxis(float fDist) { m_vPosition+=fDist*m_vYAxis; }
	void MoveLocalXAxis(float fDist) { m_vPosition+=fDist*m_vXAxis; }
	void SetPosition(float fX, float fY, float fZ);
	void SetPosition(EtVector3 Vector) { m_vPosition=Vector; }
	EtVector3 GetPosition() { return m_vPosition; };
	D3DXVECTOR2 GetPositionXZ()	{ return D3DXVECTOR2( m_vPosition.x, m_vPosition.z ); }
	void CopyRotationFromThis( MatrixEx *pCross );
	void MakeUpCartesianByZAxis();
	void MakeUpCartesianByYAxis();
	void MakeUpCartesianByXAxis();
	void Look( EtVector3 *pAt, EtVector3 *pUpVec = NULL );

	operator EtMatrix *();
	operator EtMatrix &();
	MatrixEx &operator =( EtMatrix &Matrix );
};

