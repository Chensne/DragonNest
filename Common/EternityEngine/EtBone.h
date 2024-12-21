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

class CEtBone
{
public:
	CEtBone();
	virtual ~CEtBone();
	void Clear();

protected:
	SBoneHeader m_BoneHeader;
	EtMatrix m_TransMat;

	std::vector< CEtBone * > m_vecChild;
	CEtBone *m_pParent;
	bool m_bRootBone;
	bool m_bEnableBlend;
	int m_nBoneIndex;

	std::vector< CEtAniKey * > m_vecAniKey;
	SBoneAniInfo m_AniInfo;
	std::vector< SBoneAniInfo > m_vecBlendAniInfo;
	EtVector3 *m_pBoneRotation;
	float m_fScale;

	static int s_CalcPositionFlag;

public:
	const char *GetName() { return m_BoneHeader.szBoneName; }
	const char *GetParentName() { return m_BoneHeader.szParentName; }
	EtMatrix *GetTransMat() { return &m_TransMat; }

	void SetBoneIndex( int nBoneIndex ) { m_nBoneIndex = nBoneIndex; }
	int GetBoneIndex() { return m_nBoneIndex;}
	void SetRootBone( bool bRootBone ) { m_bRootBone = bRootBone; }
	bool IsRootBone() { return m_bRootBone; }
	void AddChild( CEtBone *pBone ) { m_vecChild.push_back( pBone ); }
	void SetParent( CEtBone *pParent ) { m_pParent = pParent; }
	CEtBone *GetParent() { return m_pParent; }
	int GetChildBoneCount() { return (int)m_vecChild.size(); }
	CEtBone *GetChild( int nIndex ) { return m_vecChild[nIndex]; }

	CEtAniKey *GetAniKey( int nIndex ) { return m_vecAniKey[ nIndex ]; }
	void LoadBone( CStream *pStream, int nAniCount, int nAniVersion );
	void SaveBone( CStream *pStream, int nAniCount );
	void MergeBone( CEtBone *pMergeBone, int nAniCount );

	void Reset();
	void SetAni( SBoneAniInfo *pInfo );
	void BlendAni( SBoneAniInfo *pInfo );
	void EnableBlend( bool bEnable );
	void CalcAni();
	void GetAniMatrix( EtMatrix *pOutMat );
	void GetAniRotationMatrix( int nAni, float fFrame, EtMatrix &AniMat );
	void CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec );
	static void SetCalcPositionFlag( int nFlag ) { s_CalcPositionFlag = nFlag; }
	void SetBoneRotation( EtVector3 *pBoneRotation ) { m_pBoneRotation = pBoneRotation; }
	void SetBoneScale( float fScale ) { m_fScale = fScale; }
	bool IsValidAniIndex( int nAniIndex ) { return ( nAniIndex >= 0 ) && ( nAniIndex < ( int )m_vecAniKey.size() ); }
};


