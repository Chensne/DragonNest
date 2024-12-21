#pragma once
#include "EtResource.h"
#include "EtPrimitive.h"
#include "EtCollisionPrimitive.h"

#define MESH_FILE_STRING	"Eternity Engine Mesh File"
#define MESH_FILE_VERSION	13
#define MESH_HEADER_RESERVED	( 1024 - sizeof( SMeshFileHeader ) )

struct SMeshFileHeader
{
	char szHeaderString[ 256 ];
	int nVersion;
	int nSubMeshCount;
	int nLODCount;
	bool bUVAni;
	char cSpace[ 3 ];
	EtVector3 MaxVec;
	EtVector3 MinVec;
	int nBoneCount;	
	int nCollisionPrimitiveCount;
	int nDummyCount;
};

class CEtSubMesh;
struct SAABox;
class CEtMesh;
typedef CSmartPtr< CEtMesh > EtMeshHandle;

class CEtMesh : public CEtResource
{
public:
	CEtMesh();
	virtual ~CEtMesh();
	void Clear();

protected:
	SMeshFileHeader m_MeshHeader;
	std::vector< CEtSubMesh * > m_vecSubMesh;
	std::vector< std::string > m_vecBoneName;
	std::vector< EtMatrix > m_vecInvWorldMat;
	std::vector< std::string > m_vecDummyName;
	std::vector< std::string > m_vecDummyParentName;
	std::vector< EtMatrix > m_vecDummyMat;
	SSphere m_BoundingSphere;

	std::vector< SCollisionPrimitive * > m_vecCollisionPrimitive;
	std::vector< std::string > m_vecCollisionPrimitiveParentName;
	std::vector< int > m_vecCollisionPrimitiveParentIndex;

	EtResourceHandle m_hLinkAni;
	bool m_bLinkToAni;

public:
	CEtSubMesh *GetSubMesh( int nSubMeshIndex ) { return m_vecSubMesh[ nSubMeshIndex ]; }
	std::vector< std::string > & GetBoneNameList() {return m_vecBoneName;}
	int GetSubMeshCount() { return ( int )m_vecSubMesh.size(); }
	int GetSubMeshIndex( const char *pSubMeshName );
	std::vector< EtMatrix > &GetInvWorldMatList() { return m_vecInvWorldMat; }
	std::vector< EtMatrix > &GetDummyMatrixList() {return m_vecDummyMat; }
	std::vector< std::string > &GetDummyNameList() {return m_vecDummyName; }
	std::vector< std::string > &GetDummyParentNameList() {return m_vecDummyParentName; }
	EtMatrix &GetDummyMatByName( const char *pName );
	EtVector3 GetDummyPosByName( const char *pName );
	char * GetDummyParentByName( const char *pName );
	bool IsEmptyParentName() { return m_vecDummyParentName.empty() || m_vecDummyParentName[0].empty(); }

	void LinkToAni( EtResourceHandle hAni );

	int FindDummy( const char *pDummyName );
	int FindBone( const char *pBoneName );

	int LoadResource( CStream *pStream );

	void Draw( int nSubmeshIndex, int nVertexDeclIndex );

	bool TestLineToTriangle( EtVector3 &vPosition, EtVector3 &vDirection, EtMatrix &TransMat, float &fDist );
	void GetExtent( EtVector3 &Origin, EtVector3 &Extent );
	void GetBoundingBox( SAABox &Box );
	void GetBoundingSphere( SSphere &Sphere, bool bActorSize = false ){ Sphere = m_BoundingSphere; }

	int GetCollisionPrimitiveCount() { return ( int )m_vecCollisionPrimitive.size(); }
	std::vector< SCollisionPrimitive * > *GetCollisionPrimitive() { return &m_vecCollisionPrimitive; }
	std::vector< int > *GetCollisionPrimitiveParentIndex() { return &m_vecCollisionPrimitiveParentIndex; }

	void Assign( EtMeshHandle hMesh, EtVector4 ScaleBias );
};
