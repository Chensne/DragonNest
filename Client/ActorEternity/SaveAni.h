#pragma once

#define ANI_FILE_STRING	"Eternity Engine Ani File 0.1"
#define ANI_FILE_VERSION	11

struct SAniFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;
	int nBoneCount;
	int nAniCount;
	char cReserved[ 756 ];
};

struct SBoneHeader
{
	char szBoneName[ 256 ];
	char szParentName[ 256 ];
	char cReserved[ 512 ];
};

struct SPositionKey
{
	int nTime;
	Point3 Position;
	int nUse;
};

struct SRotationKey
{
	int nTime;
	Quat Rotation;
	int nUse;
};

struct SScaleKey
{
	int nTime;
	Point3 Scale;
	int nUse;
};

class CSaveAni
{
public:
	CSaveAni(void);
	virtual ~CSaveAni(void);

protected:
	Interface *m_pMaxInterface;
	INode *m_pNode;

	std::vector< SPositionKey > m_PositionList;
	std::vector< SRotationKey > m_RotationList;
	std::vector< SScaleKey > m_ScaleList;

	Matrix3 m_LocalMat;
	AffineParts m_Affine;

public:
	void ProcessBone( FILE *fp, INode *pNode, Interface *pMaxInterface);
	void GetAniData();
	void ReducePositionKey();
	void ReduceRotationKey();
	void ReduceScaleKey();
	void WriteAniInfo( FILE *fp );

	bool IsExistAniKey( INode *pNode, Interface *pMaxInterface );
};
