#pragma once

#define MESH_FILE_STRING	"Eternity Engine Mesh File 0.13"
#define MESH_FILE_VERSION	13

#define MAX_TEXTURE_CHANNEL		7

struct SMeshFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;
	int nSubMeshCount;
	int nLODCount;
	bool bUVAni;
	char cSpace[ 3 ];
	Point3 MaxVec;
	Point3 MinVec;
	int nBoneCount;	
	int nCollisionPrimitiveCount;
	int nDummyCount;
	char cReserved[ 716 ];
};

struct SSubMeshHeader
{
	char szParentName[ 256 ];
	char szName[ 256 ];
	int nVertexCount;
	int nFaceCount;
	int nTextureChannel;
	bool bUseStrip;
	bool bUsePhysique;
	bool bExportVertexColor;
	char cReserved[ 497 ];
};

struct SLocalVertex
{
	int nVertexIndex;
	int nSmothGroup;
	float fU[ 7 ], fV[ 7 ];
	int nCheckFlag;
	bool bUseFaceNormal;
	Point3 FaceNormal;
	DWORD dwVertexColorIndex;
};

struct SMAXVertex
{
	Point3 Vertex;
	short nBone[ 4 ];
	float fWeight[ 4 ];
	DWORD dwVertexColor;
};

struct SPhysiqueInfo
{
	short nBoneIndex;
	float fWeight;
};

class CSaveMesh
{
public:
	CSaveMesh();
	~CSaveMesh();

protected:
	INode *m_pNode;
	Mesh *m_pMesh;
	Mtl *m_pMtl;
	std::vector< std::string > *m_pBoneList;
	int m_nMatCount;
	int m_nMatIndex;
	int m_nMAXVertexCount;

	int m_nTextureChannelCount;

	std::vector< SLocalVertex > m_LocalVertexList;
	std::vector< SLocalVertex > m_FinalVertexList;
	std::vector< WORD > m_FinalFaceList;

	char m_szMeshName[ 256 ];
	char m_szParentName[ 256 ];

public:
	void Initialize( INode *pNode, Mesh *pMesh, Mtl *pMtl, std::vector< std::string > *pBoneList, int nMatCount, int nMatIndex, int nMAXVertexCount );
	void SetMeshName( const char *pszMeshName, const char *pszParentName );
	int BuildSaveMesh( bool bUseStrip );
	Point3 GetFaceNormal( Mesh *pMesh, int nFaceNum, RVertex *pRVertex );
	void GetVertexList( Mesh *pMesh, std::vector< SLocalVertex > &VertexList, int nMatCount, int nMatID );
	void ReduceSameVertex( std::vector< SLocalVertex > &VertexList, std::vector< WORD > &FaceList, std::vector< SLocalVertex > &ReduceList );
	int WriteMesh( FILE *fp, std::vector< SMAXVertex > &SystemVertex, std::vector< std::string > &BoneList, 
		bool bUseStrip, bool bPhysique, bool bExportVertexColor );

};

bool TMNegParity( Matrix3 &TM );
