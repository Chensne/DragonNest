#pragma once

class MAHeadLookController
{
public:
	MAHeadLookController();
	virtual ~MAHeadLookController();

private:
	EtVector3 m_vHeadLook;
	EtVector3 m_vCurHeadLook;
	int m_nHeadLookResopneLength;
	int m_nHeadLookRotateSpeed;
	float m_fHeadLookHeadRatio;
	float m_fHeadLookYAxisMaxAngle;
	float m_fHeadLookXAxisMaxAngle;
	bool m_bHeadLookYAxisReverse;
	bool m_bHeadLookXAxisReverse;
	int m_nHeadLookBoneIndex[2];
	DnActorHandle m_hHeadLookTargetActor;
	EtVector3 m_vHeadLookTargetPos;

	float m_fHeadScale;

	EtAniObjectHandle	m_hHeadLookObject;

public:
	void InitializeHeadLook( EtAniObjectHandle hObject );
	void OnSignalHeadLook( HeadLookStruct *pStruct );
	void ProcessHeadLook( MatrixEx &Cross, float fDelta, bool bSignalRange );
	void SetHeadLookTarget( EtVector3 &vPos );
	void SetHeadLookTarget( DnActorHandle hActor );
	void GetCurHeadLookDir( /*IN OUT*/ EtVector3& vCurHeadLook ) { vCurHeadLook = m_vCurHeadLook; };

	void SetHeadScale( float fScale = 1.0f );
};
