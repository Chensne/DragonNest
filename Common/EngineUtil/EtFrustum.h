#pragma once

class CEtFrustum
{
public:
	CEtFrustum();
	virtual ~CEtFrustum();

public:
	void SetPerspective( const float FOV /*rad*/, const float ViewAspect, const float nearZ, const float farZ );
	void Build(  const EtVector3& Eye, const EtVector3& Forward, const EtVector3& Right, const EtVector3& Up );
	bool IsPointIn( const EtVector3& Point );
	bool IsAABBIn  ( const EtVector3& v1, const EtVector3& v2);
	bool IsOBBIn   ( const EtVector3& Center, const EtVector3& HalfDimensions, const EtQuat& RotationQuat);
	bool IsHullIn( const EtVector3* aVertices, const char nVertices );
	bool IsSphereIn( const EtVector3& Center, const float Radius );

private:
	EtVector3 m_EyePosition;
	EtVector3 m_ForwardVector;
	EtVector3 m_RightVector;
	EtVector3 m_UpVector;

	float m_rFactor; 
	float m_uFactor;

	float m_NearZ;
	float m_FarZ;
};
