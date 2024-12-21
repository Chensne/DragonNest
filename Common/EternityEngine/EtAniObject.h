#pragma once
#include "EtObject.h"
#include "EtAni.h"
#include "EtBone.h"
#include "EtSpring.h"

class CEtAniObject;
typedef CSmartPtr< CEtAniObject > EtAniObjectHandle;

struct SAniInfo
{
	SBoneAniInfo BoneAniInfo;
	int nBoneIndex;
};

struct SBoneRotationInfo
{
	int nBoneIndex;
	EtVector3 BoneRotation;
};

struct SBoneScaleInfo
{
	int nBoneIndex;
	float fScale;
};
							
class CEtAniObject : public CEtObject
{
public:
	CEtAniObject();
	virtual ~CEtAniObject();

protected:
	int m_nCalcPositionFlag;
	EtAniHandle m_hAni;

	// 아래의 애니 시스템 무시하고 Extra 애니 시스템으로 렌더링 할때 쓸 변수
	// Extra 변수 셋팅 돼 있으면 m_nExtraAniIndex 가 우선한다.
	int m_nExtraAniIndex;
	float m_fExtraAniFrame;
	std::vector< SAniInfo > m_vecAniInfo;
	std::vector< SAniInfo > m_vecBlendAniInfo;
	std::vector< std::vector< int > > m_vecDisableBone;
	std::vector< SBoneRotationInfo > m_vecBoneRotation;
	std::vector< SBoneScaleInfo > m_vecBoneScale;

	CEtSpring						m_Spring;
	bool							m_bSkipPhysics;
	bool							m_bForceSkipSimulateAni;

	static CSyncLock s_CalcAniLock;

public:
	int Initialize( EtSkinHandle hSkin, EtAniHandle hAni );

	EtAniHandle &GetAniHandle() { return m_hAni; }
	int GetAniCount() { return m_hAni->GetAniCount(); }
	int GetAniLength( int nAni ) { return m_hAni->GetAniLength( nAni ); }
	float GetLastFrame( int nAni ) { return m_hAni->GetLastFrame( nAni ); }
	const char *GetAniName( int nAni ) { return m_hAni->GetAniName( nAni ); }
	void CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec )
	{
		m_hAni->CalcAniDistance( nAni, fCurFrame, fPrevFrame, DistVec );
	}
	void SetCalcPositionFlag( int nFlag ) { m_nCalcPositionFlag = nFlag; }
	int GetCalcPositionFlag() { return m_nCalcPositionFlag; }

	virtual EtMatrix *GetBoneMat( int nBoneIndex );
	virtual EtMatrix *GetBoneTransMat( int nBoneIndex );
	virtual EtMatrix *GetDummyTransMat( int nDummyIndex );
	virtual int GetAniFileIndex() { return m_hAni->GetMyIndex(); }
	
	void SetExtraAniFrame( int nAni, float fFrame ) { m_nExtraAniIndex = nAni; m_fExtraAniFrame = fFrame; }
	void SetAniFrame( int nAni, float fFrame, int nBoneIndex = 0 );
	void BlendAniFrame( int nBlendAni, float fBlendFrame, float fBlendWeight, int nBlendBoneIndex = 0 );
	void BlendAniFrame( int nBlendAni, float fBlendFrame, float fBlendWeight, int nBlendBoneIndex, std::vector< int > &vecDisableBone );
	void BlendAniFrame( int nBlendAni, float fBlendFrame, float fBlendWeight, int nBlendBoneIndex, int nDisableBoneIndex );
	void ResetAniFrame();
	void SetBoneRotation( int nBoneIndex, EtVector3 &BoneRotation );
	void SetBoneScale( int nBoneIndex, float fScale );
	void ReBuildSkin( EtSkinHandle hSkin );

	void CalcAni();
	void InitRender( int nSaveMatIndex = -1 );

	void CreateSimulation( const char *szFileName );
	void SimulateAni();
	void DisableSimulation() { m_Spring.Disable(); }
	void ForceSkipSimulateAni( bool bForceSkip, bool bApplyChild = true );

	void SkipPhysics( bool bSkip ) {m_bSkipPhysics = bSkip;}

	virtual void OnLoadComplete( CBackgroundLoader *pLoader );
};
