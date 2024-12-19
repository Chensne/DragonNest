#pragma once

#define BONE_HEADER_RESERVED	( 1024 - sizeof( SBoneHeader ) )
struct SBoneHeader
{
	char szBoneName[ 256 ];
	char szParentName[ 256 ];
};
struct SBoneAniInfo
{
	int nAni;
	float fFrame;
	float fWeight;
};

class CEtAniKey;

class CEtBone: public TBoostMemoryPool< CEtBone >
{
public:
	CEtBone();
	virtual ~CEtBone();
	void Clear();

protected:
	SBoneHeader m_BoneHeader;
	EtMatrix m_TransMat;

	std::vector< CEtBone * > m_vecChild;
	std::vector< CEtAniKey * > m_vecAniKey;
	
	CEtBone *m_pParent;
	bool m_bRootBone;
	int m_nBoneIndex;

	SBoneAniInfo m_AniInfo;

	static int s_CalcPositionFlag;

public:
	const char *GetName() { return m_BoneHeader.szBoneName; }
	const char *GetParentName() { return m_BoneHeader.szParentName; }
	EtMatrix *GetTransMat() { return &m_TransMat; }

	void SetBoneIndex( int nBoneIndex ) { m_nBoneIndex = nBoneIndex; }
	int GetBoneIndex() { return m_nBoneIndex; }
	void SetRootBone( bool bRootBone ) { m_bRootBone = bRootBone; }
	bool IsRootBone() { return m_bRootBone; }
	void AddChild( CEtBone *pBone ) { m_vecChild.push_back( pBone ); }
	void SetParent( CEtBone *pParent ) { m_pParent = pParent; }
	CEtBone *GetParent() { return m_pParent; }

	CEtAniKey *GetAniKey( int nIndex ) { return m_vecAniKey[ nIndex ]; }
	void LoadBone( CStream *pStream, int nAniCount, int nAniVersion );

	void Reset();
	void SetAni( SBoneAniInfo *pInfo );
	void CalcAni();
	void GetAniMatrix( EtMatrix *pOutMat );
	void GetAniRotationMatrix( int nAni, float fFrame, EtMatrix &AniMat );
	void CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec );
	static void SetCalcPositionFlag( int nFlag ) { s_CalcPositionFlag = nFlag; }

	void MergeBone( CEtBone *pMergeBone, int nAniCount );
};

