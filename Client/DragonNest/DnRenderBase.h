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
	virtual EtMatrix GetBoneMatrix( const char *szBoneName, bool *bExistBone = NULL ) { EtMatrix matIdentity; if( bExistBone ) *bExistBone = false; return matIdentity; }
	virtual int GetBoneIndex( const char *szBoneName ) { return -1; }
	virtual int GetDummyBoneIndex( const char *szBoneName ) { return -1; }

	virtual void SetFPS( float fValue ) {}
	virtual float GetFPS() { return 60.f; }

	virtual void ShowRenderBase( bool bShow ) {}
	virtual bool IsShow() { return false; }

	virtual void SetFrame( float fValue ) {}
	virtual void SetPrevFrame( float fValue ) {}

	virtual EtAniObjectHandle GetObjectHandle() { return CEtAniObject::Identity(); }

	virtual void AddAniDistance( EtVector3 &vVec ) {}
	virtual void CalcAniDistance( int nAniIndex, float fFrame, float fPrevFrame, EtVector3 &vDist ) {}
};