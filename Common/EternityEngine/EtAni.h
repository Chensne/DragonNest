#pragma once
#include "EtResource.h"


#define ANI_FILE_STRING	"Eternity Engine Ani File"
#define ANI_FILE_VERSION	11
#define ANI_HEADER_RESERVED	( 1024 - sizeof( SAniFileHeader ) )
struct SAniFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;
	int nBoneCount;
	int nAniCount;
};

class CEtBone;
class CEtAni;
struct SBoneAniInfo;
typedef CSmartPtr< CEtAni >	EtAniHandle;

class CEtAni : public CEtResource
{
public:
	CEtAni();
	virtual ~CEtAni();
	void Clear();

protected:
	SAniFileHeader m_AniHeader;
	std::vector< CEtBone * >	m_vecBone;
	std::vector< std::string >	m_vecAniName;
	std::vector< int >			m_vecAniLength;

	struct SCacheBoneName
	{
		std::string szBoneName;
		int nBoneIndex;
	};
	std::vector< SCacheBoneName > m_vecCacheBoneName;

public:
	int GetAniCount() { return m_AniHeader.nAniCount; }
	int GetAniLength( int nAni ) { return m_vecAniLength[ nAni ]; }
	float GetLastFrame( int nAni ) { return m_vecAniLength[ nAni ] - 1.0f; }
	const char *GetAniName( int nAni ) { return m_vecAniName[ nAni ].c_str(); }

	int GetBoneCount() { return m_AniHeader.nBoneCount; }
	int GetBoneIndex( const char *pBoneName );
	CEtBone *GetBone( int nIndex );
	CEtBone *FindBone( const char *pBoneName );
	int ConvertBoneIndex( int nIndex );
	bool IsValidBoneIndex( int nBoneIndex ) { return ( ( nBoneIndex >= 0 ) && ( nBoneIndex < ( int )m_vecBone.size() ) ); }

	int LoadAni( CStream *pStream );
	int LoadMultiAni( CStream *pStream );
	int LoadResource( CStream *pStream );
	void BuildHierarchy();

	void Reset();
	float CalcCorrectFrame( int nAni, float fFrame );
	void SetAni( SBoneAniInfo *pAniInfo, int nBoneIndex = 0 );
	void EnableBlend( int nBoneIndex, bool bEnable );
	void BlendAni( SBoneAniInfo *pAniInfo, int nBoneIndex = 0 );
	int CalcAni( std::vector< EtMatrix > &vecInvWorldMat );
	void CalcAniDistance( int nAni, float fCurFrame, float fPrevFrame, EtVector3 &DistVec );
	void SetCalcPositionFlag( int nFlag );
	void CalcRootBoneMat( int nAni, float fFrame, EtMatrix &BoneMat );
	void SetBoneRotation( int nBoneIndex, EtVector3 *pBoneRotation );
	void SetBoneScale( int nBoneIndex, float fScale );

	int Save( const char *pFileName );
	int SaveAni( CStream *pStream );
	int MergeAni( CEtAni *pAni );
};


