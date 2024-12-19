#pragma once
#include "EtObject.h"
#include "EtAni.h"
#include "EtBone.h"

#include "MultiSmartPtr.h"
#include "MultiCommon.h"

class CEtAniObject;
typedef CMultiSmartPtr< CEtAniObject > EtAniObjectHandle;

class CEtAniObject : public CEtObject, public TBoostMemoryPool< CEtAniObject >
{
public:
	CEtAniObject( CMultiRoom *pRoom );
	virtual ~CEtAniObject();

protected:
	int m_nCalcPositionFlag;
	EtAniHandle m_hAni;
	SBoneAniInfo m_AniInfo;
	std::vector< EtMatrix > m_vecTransMat;
	bool m_bCalcAni;

public:
	int Initialize( EtSkinHandle hSkin, EtAniHandle hAni );

	EtAniHandle &GetAniHandle() { return m_hAni; }
	int GetAniCount() { if( !m_hAni ) return 0; return m_hAni->GetAniCount(); }
	int GetAniLength( int nAni ) { if( !m_hAni ) return 0; return m_hAni->GetAniLength( nAni ); }
	float GetLastFrame( int nAni ) { if( !m_hAni ) return 0.f; return m_hAni->GetLastFrame( nAni ); }
	const char *GetAniName( int nAni ) { if( !m_hAni ) return ""; return m_hAni->GetAniName( nAni ); }
	void CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec )
	{
		m_hAni->CalcAniDistance( nAni, fCurFrame, fPrevFrame, DistVec );
	}
	void SetCalcPositionFlag( int nFlag ) { m_nCalcPositionFlag = nFlag; }
	int GetCalcPositionFlag() { return m_nCalcPositionFlag; }
	virtual int GetAniFileIndex() { return m_hAni->GetMyIndex(); }

	virtual EtMatrix *GetBoneTransMat( int nBoneIndex );
	virtual EtMatrix *GetBoneMat( int nBoneIndex );
	void SetAniFrame( int nAni, float fFrame );

	void CalcAni();
	bool IsCalcAni() { return m_bCalcAni; }

	// 게임서버에서 플레이어의 이 플래그가 꺼져 있는데 본 매트릭스를 얻어와야할 경우가 있으므로 임시로 켰다 끄도록 한다. #28532
	void SetCalcAni( bool bCalcAni ) { m_bCalcAni = true; };
};

