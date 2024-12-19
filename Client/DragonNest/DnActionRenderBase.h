#pragma once

#include "DnActionBase.h"
#include "DnRenderBase.h"
#include "EtMatrixEx.h"
#include "Timer.h"

class CDnActionRenderBase : virtual public CDnActionBase, 
							public CDnRenderBase
{
public:
	CDnActionRenderBase();
	virtual ~CDnActionRenderBase();

protected:
	MSDT_DECL(float) m_fFps;
	MatrixEx m_matExWorld;
	EtAniObjectHandle m_hObject;
	LOCAL_TIME m_AniTime;

	int m_nAniIndex;
	float m_fFrame;
	float m_fPrevFrame;

	int m_nBlendAniIndex;
	float m_fBlendAniFrame;
	float m_fBlendFrame;
	float m_fBlendStartFrame;

	EtVector3 m_vAniDistance;
	EtVector3 m_vScale;
	bool m_bShow;

protected:
	std::map<std::string, int> m_mapAniList;

public:
	bool Initialize( const char *szSkinName, const char *szAniName, const char *szActName );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void SetCalcPositionFlag( int nFlag );

	void SetPosition( EtVector3 &vPos ) { m_matExWorld.m_vPosition = vPos; }
	EtVector3 *GetPosition() { return &m_matExWorld.m_vPosition; }
	MatrixEx *GetMatEx() { return &m_matExWorld; }
	void SetCross( MatrixEx *pCross ) { m_matExWorld = *pCross; }

	void SetScale( EtVector3 &vScale ) { m_vScale = vScale; }

	virtual EtAniObjectHandle GetObjectHandle() { return m_hObject; }

	// CDnRenderBase
	virtual void ChangeAnimation( int nAniIndex, float fFrame = 0.f, float fBlendFrame = 0.f );
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta );
	virtual int GetAniIndex( const char *szAniName );
	virtual EtMatrix GetBoneMatrix( const char *szBoneName, bool *bExistBone = NULL );
	virtual int GetBoneIndex( const char *szBoneName );
	virtual int GetDummyBoneIndex( const char *szBoneName );
	virtual bool IsShow();

	virtual void SetFPS( float fValue );
	virtual float GetFPS();

	virtual void ShowRenderBase( bool bShow );
};