#pragma once

#include "Timer.h"
class CDnRenderBase {
public:
	CDnRenderBase() {}
	virtual ~CDnRenderBase() {}

protected:

public:
	virtual void ChangeAnimation( int nAniIndex, float fFrame = 0.f, float fBlendFrame = 0.f ) {}
	virtual void Process( EtMatrix *pmatWorld, LOCAL_TIME LocalTime, float fDelta ) {}
	virtual int GetAniIndex( const char *szAniName ) { return -1; }
	virtual EtMatrix GetBoneMatrix( const char *szBoneName ) { EtMatrix matIdentity; return matIdentity; }
	virtual int GetBoneIndex( const char *szBoneName ) { return -1; }

	virtual void SetFPS( float fValue ) {}
	virtual float GetFPS() { return 60.f; }

	virtual void SetFrame( float fValue ) {}
	virtual void SetPrevFrame( float fValue ) {}

	virtual void AddAniDistance( EtVector3 &vVec ) {}
	virtual void CalcAniDistance( int nAniIndex, float fFrame, float fPrevFrame, EtVector3 &vDist ) {}
};